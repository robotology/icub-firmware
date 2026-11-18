#!/usr/bin/env python3
"""
test_host.py - the host tools of TETHER (tethercodec.py, tether.py) against the code of the board running on the PC
(tether_pcboard, built from test_tether's sources): protocol 6 end to end, w/out a board.

    python3 test_host.py ./tether_pcboard

- the browse: BrowseInfo w/ maxframesize, the items and the fields w/ 16-bit sizes, the crc32
- the big values: ask / set+ack of 400 bytes, run+ack w/ an argument of 400 bytes, the light ack
- Codec.split(): a line whose replies do not fit one frame goes in more frames, and nothing gets "no room"
- the stream: three big variables in one frame, decoded
- embot-tether rpc --cli and str --cli, as a user runs them
"""

import os
import socket
import struct
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.normpath(os.path.join(HERE, '..', '..', '..', '..', '..', '..', 'embot', 'svc', 'host', 'tether'))   # embot/svc/host/tether
TETHER = os.path.join(TOOLS, 'tether.py')
sys.path.insert(0, TOOLS)
import tethercodec as rc        # noqa: E402

RPC, STR, HOST = 16666, 17777, 17778        # the ports of the board on the PC, and where this test gets the stream
passed = failed = 0


def check(ok, what):
    global passed, failed
    print(f'  {"PASS" if ok else "FAIL"}  {what}')
    if ok:
        passed += 1
    else:
        failed += 1


class Link:
    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.connect(('127.0.0.1', RPC))
        self.sock.settimeout(1.0)
        self.codec = rc.Codec(rc.Protocol())

    def transact(self, rops, tries=None):
        self.sock.send(self.codec.build_frame(rops))
        try:
            return self.codec.decode_frame(self.sock.recv(2048))
        except socket.timeout:
            return None

    def all(self, rops):
        out = []
        for chunk in self.codec.split(rops):
            f = self.transact(chunk)
            out += f.rops if f else []
        return out


def main():
    board = subprocess.Popen([sys.argv[1], str(RPC), str(STR), str(HOST), '60'], stdout=subprocess.PIPE, text=True)
    print(board.stdout.readline().strip())
    try:
        run()
    finally:
        board.terminate()
        board.wait()
    print(f'{passed}/{passed + failed} host checks passed')
    return 0 if failed == 0 else 1


def run():
    link = Link()
    print('browse')
    info = rc.browse_info(link.transact)
    check(info is not None and info.protocol == 6 and info.maxframesize == 1472 and info.maxdatasize == 512 and info.nvars == 15,
          'browse_info: protocol 6, frames of 1472 bytes, 512 bytes of data, 15 variables')
    proto = rc.browse(link.transact, info)
    link.codec = rc.Codec(proto)
    link.codec.set_limits(info)
    table = proto.by_name['IDtable']
    hist = proto.by_name['IDviewof_latency']
    load = proto.by_name['OPloadTable']
    check(table.size == 400 and table.spec.describe() == 'f32[100]' and table.access == 'RW', 'IDtable: f32[100], 400 bytes, RW')
    check(hist.size == 288 and hist.spec.describe() == '{total:u32, below:u32, beyond:u32, min:f32, max:f32, mean:f32, lo:f32, hi:f32, bins:u16[128]}',
          'IDviewof_latency: embot::instr::HistogramSnapshot<128>')
    check(load.size == 400 and load.result.size == 4 and load.result.describe() == 'sum:f32', 'OPloadTable: values:f32[100] -> sum:f32')
    with tempfile.TemporaryDirectory() as d:
        path = os.path.join(d, 'dictionary.toml')
        proto.save(path, 'pcboard')
        again = rc.Protocol.load(path)
        check(again.by_name['IDtable'].size == 400 and again.by_name['IDviewof_latency'].spec.describe() == hist.spec.describe() and
              again.meta.get('maxframesize') == 1472, 'the dictionary file keeps the big variables and maxframesize')

    print('big values')
    values = '[' + ', '.join(str(0.25 * k) for k in range(100)) + ']'
    r = link.all(link.codec.parse_line(f'set[+ack]<IDtable, {values}>; ask<IDtable>'))
    check(len(r) == 2 and r[0].cmd == rc.CMD.ack and r[0].size == 0 and r[0].refcmd == rc.CMD.set, 'set[+ack] of 400 bytes: ack w/out data')
    check(r[1].cmd == rc.CMD.say and list(struct.unpack('<100f', r[1].data)) == [0.25 * k for k in range(100)], 'ask<IDtable>: the 400 bytes back')
    text = link.codec.format_rop(r[0])
    check(text == 'ack<IDtable, set>', f'format of the light ack: {text}')
    r = link.all(link.codec.parse_line('run[+ack]<OPloadTable, [' + ', '.join(str(k + 1) for k in range(100)) + ']>'))
    text = link.codec.format_rop(r[0])
    check(text == 'ack<OPloadTable, run, sum=5050>', f'run[+ack] w/ 400 bytes of argument: {text}')
    r = link.all(link.codec.parse_line('run[+ack]<OPdivide, {a = 1, b = 0}>'))
    check(link.codec.format_rop(r[0]) == 'nak<OPdivide, run, "division by zero">', 'the nak w/ its text')

    print('split')
    line = '; '.join(['ask<IDtable>'] * 5 + ['ask<IDviewof_latency>'] * 3 + ['set[+ack]<IDgain, 2.5>'])
    rops = link.codec.parse_line(line)
    chunks = link.codec.split(rops)
    r = link.all(rops)
    check(len(chunks) == 3, f'5 x 404 + 3 x 272 + 40 bytes of replies: {len(chunks)} frames')
    check(len(r) == 9 and all(x.cmd != rc.CMD.nak for x in r), 'all the replies, no nak "no room"')
    big = link.codec.build_frame(rops)
    f = link.transact(rops)
    naks = [x for x in f.rops if x.cmd == rc.CMD.nak]
    check(len(big) < 1472 and len(naks) >= 1 and rc.format_text(naks[0].data) == 'no room',
          'the same ROPs in one frame: the board answers nak "no room" to what does not fit')
    try:
        link.codec.split([rc.make_rop(rc.CMD.set, 0x11, bytes(513))])
        check(False, 'a ROP of 513 bytes is refused')
    except rc.ProtocolError:
        check(True, 'a ROP of 513 bytes is refused')
    v5 = struct.pack('<IBBHQ', rc.SIGNATURE, 8, 1, 0, 0) + bytes([0, 0x05, 0, 0, 0, 0, 0, 0])
    link.sock.send(v5)
    try:
        link.sock.recv(2048)
        check(False, 'a frame of protocol 5: no reply')
    except socket.timeout:
        check(True, 'a frame of protocol 5: no reply')
    try:
        rc.Codec.decode_frame(v5)
        check(False, 'the codec refuses a frame of protocol 5')
    except rc.ProtocolError as e:
        check('layout = 8' in str(e), 'the codec refuses a frame of protocol 5')

    print('stream')
    rx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    rx.bind(('127.0.0.1', HOST))
    rx.settimeout(1.0)
    r = link.all(link.codec.parse_line('set[+ack]<MGstreamADD, IDtable>; set[+ack]<MGstreamADD, IDviewof_latency>; '
                                       'set[+ack]<MGstreamADD, IDskinPATCH>; set[+ack]<MGstreamSTART, 20>; ask<MGstreamSTART>'))
    check(all(x.cmd in (rc.CMD.ack, rc.CMD.say) for x in r) and link.codec.format_rop(r[4]) == 'say<MGstreamSTART, 20 ms>',
          'three big variables streamed every 20 ms')
    frame = rc.Codec.decode_frame(rx.recv(2048))
    check(frame.size == 16 + 404 + 292 + 64 and [x.id for x in frame.rops] == [0x11, 0x10, 0x0F], f'a frame of {frame.size} bytes w/ the three sig')
    flat = link.codec.flatten(frame.rops[1])
    check(len(flat) == 8 + 128 and flat[8][0] == 'IDviewof_latency.bins[0]', 'flatten for the csv: IDviewof_latency.bins[0 ... 127]')
    link.all(link.codec.parse_line('set[+ack]<MGstreamSTOP>; set[+ack]<MGstreamREM, IDtable>; '
                                   'set[+ack]<MGstreamREM, IDviewof_latency>; set[+ack]<MGstreamREM, IDskinPATCH>'))
    rx.close()

    print('tether.py')
    with tempfile.TemporaryDirectory() as d:
        dic = os.path.join(d, 'dictionary.toml')
        lines = ['ask<IDskinPATCH>', 'set[+ack]<IDtable, ' + values + '>', 'ask<IDtable>; ask<IDtable>; ask<IDtable>; ask<IDtable>',
                 'run[+ack]<OPreadRegister, 3>', 'quit']
        cli = subprocess.Popen([sys.executable, TETHER, 'rpc', '--cli', '--ip', '127.0.0.1', '--port', str(RPC), '--dictionary', dic],
                               stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
        time.sleep(1.0)                     # the browse
        for line in lines:
            cli.stdin.write(line + '\n')
            cli.stdin.flush()
            time.sleep(0.3)                 # the replies are printed as they arrive
        out = cli.communicate(timeout=10)[0]
        check('wrote ' + dic in out and os.path.isfile(dic), 'rpc --cli: browse, dictionary.toml written')
        check('say<IDskinPATCH, [' in out, 'rpc --cli: say<IDskinPATCH, [...]>')
        check('ack<IDtable, set>' in out, 'rpc --cli: ack<IDtable, set>')
        check('(frame 2 of 2)' in out and out.count('say<IDtable, [') == 4, 'rpc --cli: four ask of 400 bytes in two frames')
        check('ack<OPreadRegister, run, value=858993459>' in out, 'rpc --cli: the result of a run')
        if failed:
            print(out)
        csv = os.path.join(d, 'stream.csv')
        stream = subprocess.Popen([sys.executable, TETHER, 'str', '--cli', '--localport', str(HOST), '--boardport', str(STR),
                                   '--ip', '127.0.0.1', '--dictionary', dic, '--log', csv], stdout=subprocess.PIPE, text=True)
        time.sleep(0.5)
        link.all(link.codec.parse_line('set[+ack]<MGstreamADD, IDviewof_latency>; set[+ack]<MGstreamADD, IDticks>; set[+ack]<MGstreamSTART, 10>'))
        time.sleep(1.5)
        link.all(link.codec.parse_line('set[+ack]<MGstreamSTOP>'))
        time.sleep(0.3)
        stream.terminate()
        sout = stream.communicate(timeout=5)[0]
        head = open(csv).readline().split(',') if os.path.isfile(csv) else []
        check(len(head) >= 2 + 136 and 'IDviewof_latency.bins[127]' in [h.strip() for h in head], 'str --cli: the csv has IDviewof_latency.bins[0 ... 127]')
        if failed:
            print(sout)


if __name__ == '__main__':
    sys.exit(main())

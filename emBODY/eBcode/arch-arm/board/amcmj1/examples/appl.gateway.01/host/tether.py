#!/usr/bin/env python3
# tether - the host of TETHER, the diagnostic and operations service of the amcmj1 board (ROP protocol, version 5).
#
# one script, two services of the board, each one w/ a graphical page (the default) or a text interface (--cli):
#
#   python3 tether.py rpc --ip 10.0.1.99                  the page of the variables and operations (UDP 6666)
#   python3 tether.py rpc --ip 10.0.1.99 --cli            the same, from a prompt
#   python3 tether.py str                                 the plots of the stream (UDP 7777)
#   python3 tether.py str --cli [--log stream.csv]        the same, as a text summary and a csv file
#
# the graphical pages need nicegui (pip install nicegui). the text interface needs nothing. both read and write
# dictionary.toml, the Dictionary of the board (see below), and use tethercodec.py: keep the files in one folder.
#
# the dictionary: at startup `rpc` asks the board for the CRC32 of its Dictionary (MGbrowseINFO). if dictionary.toml
# does not exist, or it was made from another Dictionary (or another protocol), it browses the board and rewrites the
# file (the old one is kept as .bak). --no-browse works offline w/ the file as it is. `str` only reads the file:
# the streaming socket does not support the browse, so start `rpc` once.
#
# ---- rpc --cli: the grammar of one line (up to 32 ROPs separated by ';', or the max of the board):  cmd[options]<args>
#   ask<IDname>                       say<IDname, value> or nak<IDname, ask, "why">
#   set<IDname, value>                no reply
#   set[+ack]<IDname, value>          ack<IDname, set, value now held> or nak<IDname, set, "why">
#   sig<IDname, value>                no reply
#   run<OPname> or run<OPname, arg>   no reply
#   run[+ack]<OPname[, arg]>          ack<OPname, run[, result]> or nak<OPname, run, "why the operation failed">
#   ping<> or ping<name>              ack<name, ping>
#
#   values: 42, -3, 0x1F, 1.5, true, [1, 2, 3], {a = 1, b = -2, c = 1.5}, an IDname (it gives its ID).
#   a struct needs all its fields. a plain integer is taken as the raw bytes of any type (e.g. 0x3FC00000FFFE0001).
#   an ID can also be a number (e.g. 0x05): no check is done on it, and set needs the size: set<0x05, 4, 1>
#
# management, built in:
#   set[+ack]<MGstreamADD, IDname>  set[+ack]<MGstreamREM, IDname>  ask<MGstreamLIST>
#   set[+ack]<MGstreamSTART, ms>    0 = the default period (100 ms on amcmj1), else the period in ms, inside the range
#                                   of the board (1 ... 10000 on amcmj1). the ack carries the period applied.
#                                   the stream goes to this PC, port 7777 (`tether.py str`)
#   set[+ack]<MGstreamSTOP>         stops the stream
#   ask<MGstreamSTART>              the period in ms, or stopped
#   ask<MGstats>  ask<MGversion>  ask<MGbrowseINFO>  ask<MGbrowseITEM, index>  ask<MGbrowseFIELD, IDname, n>
#
# local commands: list, browse, help, quit.  `list` shows every operation w/ its argument and its result, e.g.
#     OPsensorTRQstartTX  0xC3   4 B      periodms:u32 -> accepted:bool    run[+ack]<OPsensorTRQstartTX, periodms>
#
# ---- str: the board streams one frame per period, w/ a sig<IDname, value> for each streamed variable, to the PC that
# started the stream, on port 7777. choose the variables and start it from `rpc`:
#   > set[+ack]<MGstreamADD, IDdummyUINT32>; set[+ack]<MGstreamSTART, 0>      0 = default period (100 ms)
#
# keepalive: the board stops the stream if its host is silent for 10 s. so `str` pings the streaming socket of the board
# every 2 s (--ping), silently. the IP of the board is --ip or, if not given, the source of the first frame.
#
# time: the time of a frame is the one of the board when the frame was made from the Dictionary: it is not the time of
# the sampling. a variable that needs it carries its own u64 timestamp as a field.
#
# str --cli: a summary every second (frames/s, the period between frames, the last value of every variable), --all for
# every frame, --log for a csv (one column per variable, array item, field; it starts again, stream.1.csv ..., when the
# set of streamed variables changes), --rtt for the round trip time of the pings.
# str (page): every streamed variable is split in signals (a scalar is one, an array gives name[k], a struct
# name.field); tick a signal to plot it against the board time. record writes the csv, and --pj host:port forwards
# every frame as a flat JSON object to the UDP Server of PlotJuggler ({"timestamp": <board time in s>, "name": value}).

import argparse
import collections
import json
import os
import queue
import socket
import struct
import sys
import threading
import time

import tethercodec as rc

try:
    import readline  # line editing and history for input()
except ImportError:
    readline = None

ui = None        # nicegui, imported only by the pages
nrun = None

DEFAULT_DICTIONARY = './dictionary.toml'


def load_nicegui():
    global ui, nrun
    try:
        from nicegui import ui as _ui, run as _run
    except ImportError:
        sys.exit('[error] the page needs nicegui: pip install nicegui (or use --cli)')
    ui, nrun = _ui, _run


# ---------------------------------------------------------------------------------------------------------------------
# the dictionary of the board, in dictionary.toml

def load_dictionary(path, log=print):
    """the Dictionary from the file, or None if it does not exist"""
    if not os.path.exists(path):
        return None
    try:
        proto = rc.Protocol.load(path)
    except OSError as e:
        sys.exit(f'[error] cannot read the dictionary file: {e}')
    log(f'[info] loaded {len(proto.entries("var"))} variables and {len(proto.entries("op"))} operations from {path}')
    return proto


def is_current(proto, info):
    """the file describes the Dictionary of the board, in the protocol of this host"""
    return (proto is not None and proto.crc32 == info.crc32 and
            proto.meta.get('protocol') == rc.PROTOCOL_VERSION == info.protocol)


def sync_dictionary(transact, peer, path, proto, log, force=False):
    """the dictionary to use: the file if it matches the board, else the browse of the board (written to path).
    transact(list of Rop) -> Frame or None. returns (Protocol or None, BoardInfo or None)"""
    try:
        info = rc.browse_info(transact)
    except rc.ProtocolError as e:
        log(f'[warn] the board does not support the browse ({e})')
        return proto, None
    if info is None:
        log(f'[warn] the board does not answer at {peer[0]}:{peer[1]}' + (f': using {path}' if proto else ''))
        return proto, None
    log(f'[info] board: protocol {info.protocol}, application {rc.format_application(info.application)}, '
        f'{info.nvars} variables, {info.nops} operations, up to {info.maxrops} ROPs per frame, '
        f'dictionary crc32 0x{info.crc32:08X}')
    if info.protocol != rc.PROTOCOL_VERSION:
        log(f'[warn] the board speaks protocol {info.protocol}, this host {rc.PROTOCOL_VERSION}')
    if is_current(proto, info) and not force:
        log(f'[info] {path} matches the Dictionary of the board')
        return proto, info
    if not force:
        log(f'[info] {path} ' + ('was made from another Dictionary' if proto else 'does not exist') + ': browsing the board')
    try:
        new = rc.browse(transact, info)
    except rc.ProtocolError as e:
        log(f'[error] browse failed: {e}')
        return proto, info
    # the old file is kept as .bak only if it describes another Dictionary
    backup = False
    if os.path.exists(path):
        try:
            old = rc.Protocol.load(path)
            same = old.crc32 == new.crc32 and old.meta.get('protocol') == rc.PROTOCOL_VERSION
        except OSError:
            same = False
        if not same:
            os.replace(path, path + '.bak')
            backup = True
    new.save(path, f'{peer[0]}:{peer[1]}')
    new.path = path
    log(f'[info] wrote {path} w/ {len(new.entries("var"))} variables and {len(new.entries("op"))} operations'
        + (f' (the old one is {path}.bak)' if backup else ''))
    return new, info


# ---------------------------------------------------------------------------------------------------------------------
# rpc, text interface

PROMPT = '> '


class RpcCli:
    def __init__(self, args):
        self.args = args
        self.verbose = args.verbose
        self.peer = (args.ip, args.port)
        self.codec = rc.Codec(rc.Protocol())
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        rc.bind_or_exit(self.sock, args.localport)
        self.sock.connect(self.peer)   # we only receive from the board
        self.sock.settimeout(0.2)
        self.stop = threading.Event()
        self.lock = threading.Lock()
        self.prompting = False
        self.interactive = readline is not None and sys.stdin.isatty() and sys.stdout.isatty()
        self.capture = None            # a queue while a transaction waits for its reply

    def set_dictionary(self, proto, info=None):
        self.codec = rc.Codec(proto)
        self.codec.set_maxrops(info.maxrops if info else 0)

    def out(self, text):
        """prints without destroying the line the user is typing"""
        with self.lock:
            if self.interactive and self.prompting:
                buf = readline.get_line_buffer()
                sys.stdout.write('\r\033[K' + text + '\n' + PROMPT + buf)
            else:
                sys.stdout.write(text + '\n')
            sys.stdout.flush()

    # ---- rx

    def start(self):
        threading.Thread(target=self.rx_loop, daemon=True).start()

    def rx_loop(self):
        while not self.stop.is_set():
            try:
                frame = self.sock.recv(65535)
            except socket.timeout:
                continue
            except ConnectionRefusedError:
                if self.capture is None:
                    self.out(f'[warn] ICMP port unreachable: is {self.peer[0]}:{self.peer[1]} listening?')
                continue
            except OSError as e:
                if self.stop.is_set():
                    break
                self.out(f'[error] rx: {e}')
                continue
            q = self.capture
            if q is not None:
                q.put(frame)
            else:
                self.on_frame(frame)

    def on_frame(self, frame):
        if self.verbose:
            self.out(f'[debug] rx {len(frame)} bytes: {frame.hex(" ")}')
        try:
            f = self.codec.decode_frame(frame)
        except rc.ProtocolError as e:
            self.out(f'[error] received an incorrect frame of {len(frame)} bytes: {e}')
            self.out(f'        {frame[:64].hex(" ")}{" ..." if len(frame) > 64 else ""}')
            return
        for w in f.warnings + f.errors:
            self.out(w)
        self.out(f'[info] received {f.numOfROPs} ROP{"" if f.numOfROPs == 1 else "s"} sent at time {rc.format_time(f.time)}')
        for rop in f.rops:
            self.out('    ' + self.codec.format_rop(rop))

    # ---- tx

    def send(self, rops):
        frame = self.codec.build_frame(rops)
        # printed before the transmission, so that it comes before the reply
        self.out(f'[info] sent a frame w/ {len(rops)} ROP{"" if len(rops) == 1 else "s"} ({len(frame)} bytes)')
        if self.verbose:
            self.out(f'[debug] tx {frame.hex(" ")}')
        try:
            self.sock.send(frame)
        except OSError as e:
            self.out(f'[error] tx: {e}')

    def transact(self, rops, tries=3):
        """sends rops and waits for the reply frame, w/ retries. returns a Frame or None"""
        frame = self.codec.build_frame(rops)
        for _ in range(tries):
            q = queue.Queue()
            self.capture = q
            try:
                self.sock.send(frame)
                data = q.get(timeout=self.args.timeout)
            except (queue.Empty, OSError):
                continue
            finally:
                self.capture = None
            try:
                return self.codec.decode_frame(data)
            except rc.ProtocolError:
                continue
        return None

    # ---- user

    def send_line(self, line):
        try:
            rops = self.codec.parse_line(line)
        except rc.ProtocolError as e:
            self.out(f'[error] {e}. nothing was sent')
            return
        if rops:
            self.send(rops)

    @staticmethod
    def help():
        print('       commands: ask<ID>, set[+ack]<ID, value>, sig<ID, value>, run[+ack]<OP[, arg]>, ping<[ID]>')
        print('       values:   42, -3, 0x1F, 1.5, true, [1, 2, 3], {a = 1, b = 2}, IDname')
        print('       stream:   set[+ack]<MGstreamADD, ID>, set[+ack]<MGstreamREM, ID>, ask<MGstreamLIST>,')
        print('                 set[+ack]<MGstreamSTART, ms> (0 = default period), set[+ack]<MGstreamSTOP>, ask<MGstreamSTART>')
        print('       board:    ask<MGstats>, ask<MGversion>, ask<MGbrowseINFO>, ask<MGbrowseITEM, i>, ask<MGbrowseFIELD, ID, n>')
        print('       local:    list (the Dictionary, w/ how to call each operation and what it gives back), browse (read it')
        print('                 again from the board), help, quit')

    def run(self):
        print(f'[info] tether rpc is connected to {self.peer[0]}:{self.peer[1]} and is ready to accept ROPs')
        print(f'       write up to {self.codec.maxrops} ROPs each one separated by a ; and send them by hitting return')
        while True:
            try:
                self.prompting = True
                line = input(PROMPT)
            except (EOFError, KeyboardInterrupt):
                print()
                break
            finally:
                self.prompting = False
            line = line.strip()
            if not line:
                continue
            if line in ('quit', 'exit', 'q'):
                break
            if line == 'help':
                self.help()
            elif line == 'list':
                print(self.codec.protocol.table())
            elif line == 'browse':
                proto, info = sync_dictionary(self.transact, self.peer, self.args.dictionary, self.codec.protocol,
                                              self.out, force=True)
                if info is not None and proto is not None:
                    self.set_dictionary(proto, info)
            else:
                self.send_line(line)
        self.stop.set()
        self.sock.close()


def rpc_cli(args):
    proto = load_dictionary(args.dictionary)
    info = None
    host = RpcCli(args)
    host.start()
    if not args.no_browse:
        proto, info = sync_dictionary(host.transact, host.peer, args.dictionary, proto, host.out)
    if proto is None:
        sys.exit(f'[error] no Dictionary: {args.dictionary} does not exist and the board did not give its own')
    host.set_dictionary(proto, info)
    host.run()


# ---------------------------------------------------------------------------------------------------------------------
# rpc, page

class Link:
    """the rpc socket. transact() is thread safe: the page calls it from worker threads"""

    def __init__(self, ip, port, localport, timeout, tries):
        self.peer = (ip, port)
        self.timeout = timeout
        self.tries = tries
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        rc.bind_or_exit(self.sock, localport)
        self.sock.connect(self.peer)    # we only receive from the board
        self.lock = threading.Lock()
        self.codec = rc.Codec(rc.Protocol())
        self.protocol = None        # shared by all the pages (browser tabs)
        self.info = None
        self.history = []           # log lines, replayed when a page opens

    def _drain(self):
        """discards late replies of previous transactions"""
        self.sock.setblocking(False)
        try:
            while True:
                self.sock.recv(65535)
        except (BlockingIOError, OSError):
            pass
        finally:
            self.sock.setblocking(True)

    def transact(self, rops, tries=None):
        """sends rops, returns the decoded reply Frame or None. use tries=1 for what must not be repeated (run)"""
        frame = self.codec.build_frame(rops)
        with self.lock:
            self._drain()
            for _ in range(tries or self.tries):
                try:
                    self.sock.send(frame)
                except OSError:
                    continue
                deadline = time.monotonic() + self.timeout
                while (left := deadline - time.monotonic()) > 0:
                    self.sock.settimeout(left)
                    try:
                        data = self.sock.recv(65535)
                    except (socket.timeout, OSError):     # OSError: ICMP port unreachable
                        break
                    try:
                        return self.codec.decode_frame(data)
                    except rc.ProtocolError:
                        continue
        return None

    def set_dictionary(self, proto, info):
        self.protocol = proto
        self.info = info
        self.codec = rc.Codec(proto if proto is not None else rc.Protocol())
        self.codec.set_maxrops(info.maxrops if info else 0)


def reply_of(frame, vid, what):
    """the say / ack of vid in a reply. raises ProtocolError on no reply, or on a nak (w/ the text of the board)"""
    if frame is None:
        raise rc.ProtocolError(f'no reply to {what}')
    for r in frame.rops:
        if r.id == vid:
            if r.cmd == rc.CMD.nak:
                why = f': {rc.format_text(r.data)}' if r.data else ''
                raise rc.ProtocolError(f'nak to {what}{why}')
            if r.cmd in (rc.CMD.say, rc.CMD.ack):
                return r
    raise rc.ProtocolError(f'the reply to {what} does not contain it')


def decode(spec, data):
    """bytes -> the python value that TypeSpec.encode() accepts back"""
    if spec.kind == spec.RAW or len(data) != spec.size:
        return int.from_bytes(data, 'little')

    def unpack(scalar, count, chunk):
        vals = list(struct.unpack('<' + rc.SCALARS[scalar][2] * count, chunk))
        return vals[0] if count == 1 else vals

    if spec.kind == spec.SCALAR:
        return unpack(spec.scalar, spec.count, data)
    out = {}
    for name, scalar, count, offset in spec.fields:
        n = rc.SCALARS[scalar][1] * count
        out[name] = unpack(scalar, count, data[offset:offset + n])
    return out


def parse_scalar_text(scalar, text):
    text = (text or '').strip()
    if not text:
        raise rc.ProtocolError('empty value')
    try:
        if scalar in ('f32', 'f64'):
            return float(text)
        return int(text, 0)
    except ValueError:
        raise rc.ProtocolError(f'"{text}" is not a valid {scalar}')


class ValueEditor:
    """the inputs of one TypeSpec: one per scalar (array items and struct fields included).
    value() gives what TypeSpec.encode() wants, show(data) displays the bytes received"""

    def __init__(self, spec, readonly):
        self.spec = spec
        self.readonly = readonly
        self.cells = {}         # key (None for a plain scalar, or field name) -> (scalar, [widgets])
        if spec.kind == spec.RAW:
            if spec.size > 0:
                self.cells[None] = ('raw', [self._widget('raw', f'raw {spec.size} B', 'w-48')])
        elif spec.kind == spec.SCALAR:
            self._add(None, spec.name or '', spec.scalar, spec.count)
        else:
            for name, scalar, count, _ in spec.fields:
                self._add(name, name, scalar, count)

    def _add(self, key, label, scalar, count):
        if count == 1:
            ws = [self._widget(scalar, label)]
        else:
            ws = [self._widget(scalar, f'{label}[{k}]') for k in range(count)]
        self.cells[key] = (scalar, ws)

    def _widget(self, scalar, label, width='w-24'):
        if scalar == 'bool':
            w = ui.switch(label)
            if self.readonly:
                w.disable()
            return w
        if scalar in ('f32', 'f64') and width == 'w-24':
            width = 'w-28'
        w = ui.input(label, value='0x0' if scalar == 'raw' else '0').props('dense outlined').classes(width)
        if self.readonly:
            w.props('readonly')
        return w

    def value(self):
        def one(scalar, w):
            if scalar == 'bool':
                return bool(w.value)
            if scalar == 'raw':
                return parse_scalar_text('u64', w.value)
            return parse_scalar_text(scalar, w.value)

        if not self.cells:
            return None
        if self.spec.kind != self.spec.STRUCT:
            scalar, ws = self.cells[None]
            vals = [one(scalar, w) for w in ws]
            return vals[0] if len(ws) == 1 and (scalar == 'raw' or self.spec.count == 1) else vals
        out = {}
        for name, (scalar, ws) in self.cells.items():
            vals = [one(scalar, w) for w in ws]
            out[name] = vals[0] if len(ws) == 1 else vals
        return out

    def show(self, data):
        v = decode(self.spec, data)

        def put(scalar, w, x):
            if scalar == 'bool':
                w.value = bool(x)
            elif scalar == 'raw':
                w.value = f'0x{x:X}'
            else:
                w.value = rc.format_scalar(scalar, x)

        if self.spec.kind == self.spec.RAW or len(data) != self.spec.size:
            if None in self.cells:
                put('raw', self.cells[None][1][0], int.from_bytes(data, 'little'))
            return
        for key, (scalar, ws) in self.cells.items():
            x = v if key is None else v[key]
            xs = x if isinstance(x, list) else [x]
            for w, item in zip(ws, xs):
                put(scalar, w, item)


class RpcPage:

    def __init__(self, link, args):
        self.link = link
        self.args = args
        self.editors = {}           # var id -> ValueEditor
        self.results = {}           # op id -> label w/ the last result
        self.streamboxes = {}       # var id -> checkbox
        self.syncing = False        # true while the checkboxes are set from MGstreamLIST
        self.polling = False
        self.logview = None

    @property
    def protocol(self):
        return self.link.protocol

    @property
    def info(self):
        return self.link.info

    # ---- log

    def log(self, text):
        line = f'{time.strftime("%H:%M:%S")}  {text}'
        print(line, flush=True)
        self.link.history = (self.link.history + [line])[-1000:]
        if self.logview is not None:
            self.logview.push(line)

    def fail(self, what, e):
        self.log(f'[error] {what}: {e}')
        ui.notify(f'{what}: {e}', type='negative', position='bottom-right')

    # ---- layout

    def build(self):
        ui.colors(primary='#1d4ed8')
        ui.query('body').classes('bg-slate-50')
        with ui.header().classes('items-center gap-6 py-2 bg-slate-800'):
            ui.label('TETHER · rpc').classes('text-lg font-bold tracking-wide')
            ui.label(f'{self.link.peer[0]}:{self.link.peer[1]}').classes('font-mono opacity-80')
            self.boardlabel = ui.label().classes('font-mono text-sm opacity-80')
            ui.space()
            ui.button('browse', icon='refresh', on_click=self.on_browse).props('flat color=white')

        with ui.row().classes('w-full no-wrap items-start gap-4 p-2'):
            self.body = ui.column().classes('grow gap-4')
            with ui.column().classes('w-[420px] gap-4 shrink-0'):
                self.build_side()
        self.update_boardlabel()
        self.rebuild()

    def update_boardlabel(self):
        if self.info:
            i = self.info
            self.boardlabel.text = (f'protocol {i.protocol} · app {rc.format_application(i.application)} · '
                                    f'crc32 0x{i.crc32:08X}')
        elif self.protocol is not None and self.protocol.crc32 is not None:
            self.boardlabel.text = f'offline · {self.args.dictionary} crc32 0x{self.protocol.crc32:08X}'
        else:
            self.boardlabel.text = 'offline'

    def build_side(self):
        with ui.card().classes('w-full'):
            ui.label('stream').classes('font-bold')
            ui.label('the board streams the ticked variables to this PC, port 7777 (tether.py str)') \
                .classes('text-xs text-gray-500')
            with ui.row().classes('items-center gap-2'):
                self.period = ui.input('period ms (0 = default)', value='0').props('dense outlined').classes('w-40')
                ui.button('start', icon='play_arrow', on_click=self.on_stream_start).props('dense')
                ui.button('stop', icon='stop', on_click=self.on_stream_stop).props('dense outline')
            with ui.row().classes('items-center gap-2'):
                self.streamstate = ui.label('state: ?').classes('font-mono text-sm')
                ui.button(icon='sync', on_click=self.on_stream_refresh).props('flat dense round').tooltip('ask the board')

        with ui.card().classes('w-full'):
            ui.label('board').classes('font-bold')
            with ui.row().classes('items-center gap-2'):
                ui.button('version', on_click=lambda: self.on_ask_mg(rc.MG_VERSION, 'MGversion')).props('dense outline')
                ui.button('stats', on_click=lambda: self.on_ask_mg(rc.MG_STATS, 'MGstats')).props('dense outline')
                ui.button('ping', on_click=self.on_ping).props('dense outline')
            with ui.row().classes('items-center gap-2'):
                self.pollswitch = ui.switch('auto ask the RO variables', on_change=self.on_poll_switch)
                self.pollperiod = ui.input('every s', value='0.5').props('dense outlined').classes('w-20')
            self.polltimer = ui.timer(0.5, self.on_poll, active=False)

        with ui.card().classes('w-full'):
            ui.label('command line').classes('font-bold')
            ui.label('same grammar as tether.py rpc --cli, e.g. ask<IDstatus>; set[+ack]<IDdummyUINT32, 7>') \
                .classes('text-xs text-gray-500')
            self.cmdline = ui.input(placeholder='ask<MGstreamLIST>').props('dense outlined').classes('w-full font-mono')
            self.cmdline.on('keydown.enter', self.on_cmdline)

        with ui.card().classes('w-full'):
            with ui.row().classes('w-full items-center'):
                ui.label('log').classes('font-bold')
                ui.space()
                ui.button(icon='delete_sweep', on_click=lambda: self.logview.clear()).props('flat dense round')
            self.logview = ui.log(max_lines=1000).classes('h-80 font-mono text-xs')
            for line in self.link.history:
                self.logview.push(line)

    def rebuild(self):
        """the variables and operations, from the Dictionary"""
        self.body.clear()
        self.editors.clear()
        self.results.clear()
        self.streamboxes.clear()
        with self.body:
            if self.protocol is None:
                ui.label('no Dictionary: the board does not answer and the file does not exist. '
                         'press browse when the board is up').classes('text-red-700')
                return
            with ui.card().classes('w-full'):
                ui.label('variables').classes('font-bold')
                for e in self.protocol.entries('var'):
                    self.variable_row(e)
            ops = self.protocol.entries('op')
            if ops:
                with ui.card().classes('w-full'):
                    ui.label('operations').classes('font-bold')
                    for e in ops:
                        self.operation_row(e)

    def head(self, e, badge):
        ui.label(f'0x{e.id:02X}').classes('font-mono text-gray-400 w-10 pt-2')
        with ui.column().classes('gap-0 w-52'):
            ui.label(e.name).classes('font-medium')
            with ui.row().classes('gap-1 items-center'):
                if badge:
                    color = 'green' if badge == 'RW' else 'gray'
                    ui.badge(badge, color=color).props('outline')
                what = f'{e.size} B · {e.spec.describe() if e.size else "no argument"}'
                if e.kind == 'op' and e.result.size > 0:
                    what += f' → {e.result.describe()}'
                ui.label(what).classes('text-xs text-gray-500 font-mono')

    def variable_row(self, e):
        with ui.row().classes('w-full no-wrap items-start gap-3 py-2 border-b border-slate-200'):
            self.head(e, e.access)
            with ui.row().classes('grow gap-2 items-center'):
                self.editors[e.id] = ValueEditor(e.spec, readonly=e.access != 'RW')
            with ui.row().classes('gap-1 items-center no-wrap shrink-0'):
                ui.button('ask', on_click=lambda e=e: self.on_ask(e)).props('dense flat')
                if e.access == 'RW':
                    ui.button('set', on_click=lambda e=e: self.on_set(e)).props('dense')
                cb = ui.checkbox('stream', on_change=lambda ev, e=e: self.on_stream_tick(e, ev.value))
                self.streamboxes[e.id] = cb

    def operation_row(self, e):
        with ui.row().classes('w-full no-wrap items-start gap-3 py-2 border-b border-slate-200'):
            self.head(e, None)
            with ui.row().classes('grow gap-2 items-center'):
                ed = ValueEditor(e.spec, readonly=False)
            with ui.column().classes('gap-0 items-end shrink-0'):
                ui.button('run', icon='play_arrow', on_click=lambda e=e, ed=ed: self.on_run(e, ed)).props('dense')
                self.results[e.id] = ui.label().classes('text-xs font-mono text-gray-600')

    # ---- calls to the board (in worker threads, so a timeout does not freeze the page)

    async def transact(self, rops, tries=None):
        return await nrun.io_bound(self.link.transact, rops, tries)

    async def on_ask(self, e):
        try:
            r = reply_of(await self.transact([rc.make_rop(rc.CMD.ask, e.id)]), e.id, f'ask<{e.name}>')
        except rc.ProtocolError as ex:
            return self.fail(e.name, ex)
        self.editors[e.id].show(r.data)
        self.log(f'say<{e.name}, {self.link.codec.format_value(e.id, r.data)}>')

    async def on_set(self, e):
        try:
            data = e.spec.encode(self.editors[e.id].value(), self.link.codec.resolve_value)
            rop = rc.make_rop(rc.CMD.set, e.id, data, rc.OPT_ACK)
            r = reply_of(await self.transact([rop]), e.id, f'set<{e.name}>')
        except rc.ProtocolError as ex:
            return self.fail(e.name, ex)
        self.editors[e.id].show(r.data)
        self.log(f'ack<{e.name}, set, {self.link.codec.format_value(e.id, r.data)}>')

    async def on_run(self, e, ed):
        try:
            data = e.spec.encode(ed.value(), self.link.codec.resolve_value) if e.size else b''
            rop = rc.make_rop(rc.CMD.run, e.id, data, rc.OPT_ACK)
            r = reply_of(await self.transact([rop], tries=1), e.id, f'run<{e.name}>')     # never run it twice
        except rc.ProtocolError as ex:
            if e.id in self.results:
                self.results[e.id].text = str(ex)
            return self.fail(e.name, ex)
        result = self.link.codec.format_value(e.id, r.data, rc.CMD.run) if r.data else ''
        if e.id in self.results:
            self.results[e.id].text = result
        self.log(f'ack<{e.name}, run' + (f', {result}' if result else '') + '>')
        ui.notify(f'{e.name} done' + (f': {result}' if result else ''), type='positive', position='bottom-right')

    async def on_ask_mg(self, mgid, name):
        try:
            r = reply_of(await self.transact([rc.make_rop(rc.CMD.ask, mgid)]), mgid, f'ask<{name}>')
        except rc.ProtocolError as ex:
            return self.fail(name, ex)
        self.log(f'say<{name}, {self.link.codec.format_value(mgid, r.data)}>')

    async def on_ping(self):
        t = time.monotonic()
        try:
            reply_of(await self.transact([rc.make_rop(rc.CMD.ping, rc.ID_NONE)], tries=1), rc.ID_NONE, 'ping')
        except rc.ProtocolError as ex:
            return self.fail('ping', ex)
        self.log(f'ack<ping> in {(time.monotonic() - t) * 1000:.2f} ms')

    # ---- stream

    async def on_stream_tick(self, e, on):
        if self.syncing:
            return
        mg, name = (rc.MG_STREAM_ADD, 'MGstreamADD') if on else (rc.MG_STREAM_REM, 'MGstreamREM')
        try:
            reply_of(await self.transact([rc.make_rop(rc.CMD.set, mg, bytes([e.id]), rc.OPT_ACK)]), mg,
                     f'set<{name}, {e.name}>')
        except rc.ProtocolError as ex:
            self.fail(e.name, ex)
        else:
            self.log(f'ack<{name}, set, {e.name}>')
        await self.on_stream_refresh()

    async def on_stream_start(self):
        try:
            ms = parse_scalar_text('u16', self.period.value)
            data = rc.TypeSpec.from_type('u16', 2).encode(ms, None)
            r = reply_of(await self.transact([rc.make_rop(rc.CMD.set, rc.MG_STREAM_START, data, rc.OPT_ACK)]),
                         rc.MG_STREAM_START, 'set<MGstreamSTART>')
        except rc.ProtocolError as ex:
            return self.fail('stream start', ex)
        self.log(f'ack<MGstreamSTART, set, {int.from_bytes(r.data, "little")} ms>')
        await self.on_stream_refresh()

    async def on_stream_stop(self):
        try:
            reply_of(await self.transact([rc.make_rop(rc.CMD.set, rc.MG_STREAM_STOP, b'', rc.OPT_ACK)]),
                     rc.MG_STREAM_STOP, 'set<MGstreamSTOP>')
        except rc.ProtocolError as ex:
            return self.fail('stream stop', ex)
        self.log('ack<MGstreamSTOP, set>')
        await self.on_stream_refresh()

    async def on_stream_refresh(self):
        """asks MGstreamLIST and MGstreamSTART, then ticks the checkboxes as the board says"""
        frame = await self.transact([rc.make_rop(rc.CMD.ask, rc.MG_STREAM_LIST),
                                     rc.make_rop(rc.CMD.ask, rc.MG_STREAM_START)])
        try:
            lst = reply_of(frame, rc.MG_STREAM_LIST, 'ask<MGstreamLIST>').data
            per = reply_of(frame, rc.MG_STREAM_START, 'ask<MGstreamSTART>').data
        except rc.ProtocolError as ex:
            self.streamstate.text = 'state: ?'
            return self.fail('stream state', ex)
        ids = set(lst[1:1 + lst[0]]) if lst else set()
        ms = int.from_bytes(per, 'little')
        names = ', '.join(self.link.codec.protocol.name_of(i) for i in sorted(ids)) or 'none'
        self.streamstate.text = f'state: {"stopped" if ms == 0 else f"running, {ms} ms"} · {len(ids)} variables'
        self.streamstate.tooltip(names)
        self.syncing = True
        try:
            for vid, cb in self.streamboxes.items():
                cb.value = vid in ids
        finally:
            self.syncing = False

    # ---- polling of the RO variables

    def on_poll_switch(self, ev):
        try:
            self.polltimer.interval = max(0.05, float(self.pollperiod.value))
        except ValueError:
            self.polltimer.interval = 0.5
        self.polltimer.active = ev.value

    async def on_poll(self):
        if self.polling or self.protocol is None:
            return
        ro = [e for e in self.protocol.entries('var') if e.access != 'RW']
        if not ro:
            return
        self.polling = True
        try:
            n = self.link.codec.maxrops
            for start in range(0, len(ro), n):
                chunk = ro[start:start + n]
                frame = await self.transact([rc.make_rop(rc.CMD.ask, e.id) for e in chunk], tries=1)
                if frame is None:
                    self.log('[warn] auto ask: no reply')
                    return
                for r in frame.rops:
                    if r.cmd == rc.CMD.say and r.id in self.editors:
                        self.editors[r.id].show(r.data)
        finally:
            self.polling = False

    # ---- command line

    async def on_cmdline(self):
        line = (self.cmdline.value or '').strip()
        if not line:
            return
        try:
            rops = self.link.codec.parse_line(line)
        except rc.ProtocolError as ex:
            return self.fail('command line', ex)
        self.log(f'> {line}')
        frame = await self.transact(rops, tries=1)
        if frame is None:
            self.log('    (no reply)')
            return
        for r in frame.rops:
            self.log('    ' + self.link.codec.format_rop(r))
            if r.cmd in (rc.CMD.say, rc.CMD.ack) and r.id in self.editors and r.refcmd & 0x0F != rc.CMD.run:
                self.editors[r.id].show(r.data)

    # ---- dictionary

    async def on_browse(self):
        old = self.protocol.crc32 if self.protocol is not None else None
        proto, info = await nrun.io_bound(sync_dictionary, self.link.transact, self.link.peer, self.args.dictionary,
                                          self.protocol, self.log)
        if info is None:
            return self.fail('browse', 'the board does not answer')
        self.link.set_dictionary(proto, info)
        self.update_boardlabel()
        if proto is not None and proto.crc32 == old:
            self.log('[info] same Dictionary: the page is kept')
        else:
            self.log('[info] new Dictionary: the page is rebuilt')
            self.rebuild()
        await self.on_stream_refresh()


def rpc_page(args):
    load_nicegui()
    link = Link(args.ip, args.port, args.localport, args.timeout, args.tries)

    def log(text):
        line = f'{time.strftime("%H:%M:%S")}  {text}'
        print(line, flush=True)
        link.history.append(line)

    proto = load_dictionary(args.dictionary, log)
    if args.no_browse:
        link.set_dictionary(proto, None)
    else:
        link.set_dictionary(*sync_dictionary(link.transact, link.peer, args.dictionary, proto, log))

    @ui.page('/')
    async def index(client):
        page = RpcPage(link, args)
        page.build()
        await client.connected()
        if link.info is not None:
            await page.on_stream_refresh()

    ui.run(title='TETHER rpc', port=args.gui_port, reload=False, native=args.native,
           show=not args.no_open and not args.native, favicon='🛰')


# ---------------------------------------------------------------------------------------------------------------------
# str: the receiver of the stream, shared by the text interface and the page

class Stat:
    """min / mean / max of a series"""

    def __init__(self):
        self.reset()

    def reset(self):
        self.n, self.sum, self.min, self.max = 0, 0, None, None

    def add(self, v):
        self.n += 1
        self.sum += v
        self.min = v if self.min is None else min(self.min, v)
        self.max = v if self.max is None else max(self.max, v)

    def text(self, fmt):
        if self.n == 0:
            return 'n/a'
        return f'min {fmt(self.min)} / mean {fmt(self.sum / self.n)} / max {fmt(self.max)}'


class CsvLog:
    """wide csv: one row per frame, one column per variable (or array item, or field)"""

    def __init__(self, base):
        root, ext = os.path.splitext(base)
        self.root, self.ext = root, ext or '.csv'
        self.file = None
        self.layout = None
        self.index = 0

    def path(self):
        return f'{self.root}{self.ext}' if self.index == 0 else f'{self.root}.{self.index}{self.ext}'

    def write(self, boardtime, rops, codec):
        layout = tuple((r.id, r.size) for r in rops)
        cells = [c for r in rops for c in codec.flatten(r)]
        if layout != self.layout:
            if self.file is not None:
                self.file.close()
                self.index += 1
            self.layout = layout
            self.file = open(self.path(), 'w')
            self.file.write(','.join(['boardtime_us'] + [col for col, _ in cells]) + '\n')
            print(f'[info] logging {len(rops)} variable{"" if len(rops) == 1 else "s"} to {self.path()}')
        self.file.write(','.join([str(boardtime)] + [str(v) for _, v in cells]) + '\n')

    def close(self):
        if self.file is not None:
            self.file.close()


class StreamRx:
    """the streaming socket: the frames of the board, and the pings that keep the stream alive"""

    def __init__(self, args, codec):
        self.args = args
        self.codec = codec
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        rc.bind_or_exit(self.sock, args.localport)
        self.sock.settimeout(0.05)
        self.bad = 0
        self.foreign = 0
        self.target = (args.ip, args.boardport) if args.ip else None   # else learned from the first frame
        self.seq = 0
        self.pending = {}         # seq -> time of tx
        self.rtt = Stat()
        self.lastrtt = None
        self.pings = 0
        self.replies = 0
        self.lost = 0
        self.lostsince = 0        # value of lost at the last reply
        self.silent = False       # true after pings lost in a row: warned once, until a reply arrives
        self.onwarn = print

    @property
    def lostinarow(self):
        return self.lost - self.lostsince

    def ping(self):
        self.seq = self.seq % 255 + 1          # 1 ... 255: the ID of the ping is its sequence number
        frame = self.codec.build_frame([rc.make_rop(rc.CMD.ping, self.seq)])
        self.pending[self.seq] = time.monotonic()
        self.pings += 1
        try:
            self.sock.sendto(frame, self.target)
        except OSError:
            pass                                # the expiry tells if the board does not answer

    def expire_pings(self):
        now = time.monotonic()
        expired = [s for s, t in self.pending.items() if now - t > 2.0]
        for seq in expired:
            del self.pending[seq]
            self.lost += 1
        # warn once if the last pings (at least 2) were all lost: the board may stop the stream (keepalive)
        if expired and not self.silent and self.lostinarow >= 2:
            self.silent = True
            self.onwarn(f'[warn] the board at {self.target[0]}:{self.target[1]} does not answer the pings: '
                        f'it may stop the stream when its keepalive expires')

    def on_pong(self, rops):
        now = time.monotonic()
        for r in rops:
            t = self.pending.pop(r.id, None)
            if t is not None:
                self.replies += 1
                self.lastrtt = (now - t) * 1000.0
                self.rtt.add(self.lastrtt)
                self.lostsince = self.lost
                if self.silent:
                    self.silent = False
                    self.onwarn(f'[info] the board at {self.target[0]}:{self.target[1]} answers the pings again')

    def decode(self, frame, src):
        """the sig ROPs of a frame, as (Frame, [Rop]), or None for what is not a stream frame"""
        if self.args.ip and src[0] != self.args.ip:
            self.foreign += 1
            return None
        try:
            f = self.codec.decode_frame(frame)
        except rc.ProtocolError as e:
            self.bad += 1
            self.onwarn(f'[error] received an incorrect frame of {len(frame)} bytes from {src[0]}:{src[1]}: {e}')
            return None
        for w in f.warnings + f.errors:
            self.onwarn(w)
        if self.target is None and self.args.ping > 0:
            # the board is who streams to us: ping it from now on
            self.target = (src[0], self.args.boardport)
            self.onwarn(f'[info] the stream comes from {src[0]}: it pings {src[0]}:{self.args.boardport} '
                        f'every {self.args.ping:g} s to keep it alive')
        if f.rops and all(r.cmd == rc.CMD.ack and (r.refcmd & 0x0F) == rc.CMD.ping for r in f.rops):
            self.on_pong(f.rops)
            return None
        return f, [r for r in f.rops if r.cmd == rc.CMD.sig], sum(1 for r in f.rops if r.cmd != rc.CMD.sig)


# ---- str, text interface

class StrCli:
    def __init__(self, args, codec):
        self.args = args
        self.codec = codec
        self.rx = StreamRx(args, codec)
        self.log = CsvLog(args.log) if args.log else None
        self.last = {}            # id -> Rop
        self.order = []           # ids of the last frame
        self.lasttime = None      # board time of the previous frame
        self.period = Stat()
        self.frames = 0           # in the current summary interval
        self.total = 0
        self.notsig = 0

    def on_frame(self, frame, src):
        got = self.rx.decode(frame, src)
        if got is None:
            return
        f, sigs, notsig = got
        self.notsig += notsig
        self.frames += 1
        self.total += 1
        if self.lasttime is not None and f.time >= self.lasttime:
            self.period.add(f.time - self.lasttime)
        self.lasttime = f.time
        for rop in sigs:
            self.last[rop.id] = rop
        self.order = [r.id for r in sigs]
        if self.log and sigs:
            self.log.write(f.time, sigs, self.codec)
        if self.args.all:
            print(f'[info] {len(sigs)} ROP{"" if len(sigs) == 1 else "s"} at {rc.format_time(f.time)}: '
                  + '; '.join(self.codec.format_rop(r) for r in sigs))

    def summary(self, elapsed):
        rate = self.frames / elapsed if elapsed > 0 else 0.0
        board = rc.format_time(self.lasttime) if self.lasttime is not None else 'n/a'
        print(f'[info] {rate:7.1f} frames/s, period {self.period.text(lambda v: f"{v:.0f}")} us, board time {board}')
        for vid in self.order:
            print('    ' + self.codec.format_rop(self.last[vid]))

    def run(self):
        args, rx = self.args, self.rx
        where = f' from {args.ip}' if args.ip else ''
        print(f'[info] tether str listens on UDP port {args.localport}{where}. ctrl-c to stop')
        if args.ping > 0:
            if rx.target:
                print(f'[info] it pings {rx.target[0]}:{rx.target[1]} every {args.ping:g} s to keep the stream alive')
            else:
                print(f'[info] it will ping the board every {args.ping:g} s to keep the stream alive, from the first frame')
        else:
            print('[info] no ping: the board stops the stream when its keepalive expires (if enabled)')
        t0 = time.monotonic()
        nextping = t0
        waiting = False
        try:
            while True:
                now = time.monotonic()
                if rx.target and args.ping > 0 and now >= nextping:
                    rx.ping()
                    nextping = now + args.ping
                try:
                    frame, src = rx.sock.recvfrom(65535)
                    self.on_frame(frame, src)
                except (socket.timeout, ConnectionRefusedError):
                    pass
                now = time.monotonic()
                if now - t0 >= args.every:
                    rx.expire_pings()
                    if not args.all:
                        if self.frames > 0:
                            if not args.quiet:
                                self.summary(now - t0)
                            elif waiting or self.total == self.frames:
                                print('[info] the stream is running')
                            waiting = False
                        elif not waiting:
                            print('[info] no stream frames: waiting ...')
                            waiting = True
                    if args.rtt and rx.target and args.ping > 0:
                        print(f'[info] ping: {rx.replies}/{rx.pings} replies, rtt '
                              f'{rx.rtt.text(lambda v: f"{v:.3f}")} ms, lost {rx.lost}')
                    self.frames = 0
                    self.period.reset()
                    rx.rtt.reset()
                    t0 = now
        except KeyboardInterrupt:
            print()
        finally:
            rx.sock.close()
            if self.log:
                self.log.close()
            print(f'[info] received {self.total} stream frames, {rx.bad} incorrect, {self.notsig} non-sig ROPs, '
                  f'{rx.foreign} from other IPs')
            if args.rtt and rx.pings:
                print(f'[info] sent {rx.pings} pings, {rx.replies} replies, {rx.lost + len(rx.pending)} lost')


def str_cli(args):
    proto = load_dictionary(args.dictionary)
    if proto is None:
        print(f'[warn] {args.dictionary} does not exist: run `tether.py rpc` once to write it. the values are shown raw')
        proto = rc.Protocol()
    StrCli(args, rc.Codec(proto)).run()


# ---- str, page

def signals_of(spec, prefix, data):
    """[(name, number)] of a sig: an array gives name[k], a struct gives name.field. a raw value is one integer"""
    if spec.kind == spec.RAW or len(data) != spec.size:
        return [(prefix, int.from_bytes(data, 'little'))]
    if spec.kind == spec.SCALAR:
        items = [(prefix, spec.scalar, spec.count, 0)]
    else:
        items = [(f'{prefix}.{name}', scalar, count, offset) for name, scalar, count, offset in spec.fields]
    out = []
    for name, scalar, count, offset in items:
        n = rc.SCALARS[scalar][1] * count
        vals = struct.unpack('<' + rc.SCALARS[scalar][2] * count, data[offset:offset + n])
        vals = [float(v) for v in vals]                 # bool -> 0.0 / 1.0
        if count == 1:
            out.append((name, vals[0]))
        else:
            out.extend((f'{name}[{k}]', v) for k, v in enumerate(vals))
    return out


class StrStore:
    """the streaming socket in its own thread. keeps the last samples of every signal"""

    def __init__(self, args, codec):
        self.args = args
        self.codec = codec
        self.rx = StreamRx(args, codec)
        self.lock = threading.Lock()
        self.data = {}                  # signal -> (deque of board time in us, deque of values)
        self.order = []                 # signals in order of arrival
        self.last = {}                  # signal -> last value
        self.t0 = None                  # board time of the first frame: x = 0
        self.tlast = None
        self.frames = 0
        self.periods = collections.deque(maxlen=2000)
        self.csv = None
        self.csvpath = None
        # forward to PlotJuggler (UDP server, JSON): one datagram per frame
        self.pjsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.pj = None                  # (host, port) when forwarding
        self.pjsent = 0
        self.pjerrors = 0
        self.stop = threading.Event()
        self.rx.onwarn = lambda text: None      # the page shows the state of the pings

    def start(self):
        threading.Thread(target=self.loop, daemon=True).start()

    def loop(self):
        rx = self.rx
        nextping = time.monotonic()
        while not self.stop.is_set():
            now = time.monotonic()
            if rx.target and self.args.ping > 0 and now >= nextping:
                rx.expire_pings()
                rx.ping()
                nextping = now + self.args.ping
            try:
                frame, src = rx.sock.recvfrom(65535)
            except (socket.timeout, ConnectionRefusedError):
                continue
            except OSError:
                if self.stop.is_set():
                    break
                continue
            self.on_frame(frame, src)

    def on_frame(self, frame, src):
        got = self.rx.decode(frame, src)
        if got is None:
            return
        f, sigs, _ = got
        if not sigs:
            return
        with self.lock:
            if self.tlast is not None and f.time < self.tlast:
                self._clear()                               # the board restarted: its time went back
            if self.t0 is None:
                self.t0 = f.time
            if self.tlast is not None:
                self.periods.append(f.time - self.tlast)
            self.tlast = f.time
            self.frames += 1
            pj = {'timestamp': f.time / 1e6} if self.pj is not None else None
            for r in sigs:
                e = self.codec.protocol.entry(r.id)
                spec = e.spec if e is not None else rc.TypeSpec.raw(len(r.data))
                for name, v in signals_of(spec, self.codec.protocol.name_of(r.id), r.data):
                    d = self.data.get(name)
                    if d is None:
                        d = self.data[name] = (collections.deque(maxlen=self.args.history),
                                               collections.deque(maxlen=self.args.history))
                        self.order.append(name)
                    d[0].append(f.time)
                    d[1].append(v)
                    self.last[name] = v
                    if pj is not None:
                        pj[name] = v
            if self.csv is not None:
                self.csv.write(f.time, sigs, self.codec)
        if pj is not None:
            self.forward(pj)

    def forward(self, msg):
        """one JSON object per frame, flat: {"timestamp": board time in s, "IDpaxiniFT.force[0]": 1.2, ...}"""
        try:
            self.pjsock.sendto(json.dumps(msg).encode(), self.pj)
            self.pjsent += 1
        except OSError:
            self.pjerrors += 1

    def plotjuggler(self, target):
        """target = (host, port) or None to stop"""
        self.pj = target
        self.pjsent = self.pjerrors = 0

    def _clear(self):
        self.data.clear()
        self.order.clear()
        self.last.clear()
        self.periods.clear()
        self.t0 = self.tlast = None

    def clear(self):
        with self.lock:
            self._clear()

    def window(self, names, seconds, maxpoints):
        """{name: [[x, v], ...]} of the last seconds, x in s from the first frame, at most maxpoints per signal"""
        out = {}
        with self.lock:
            if self.tlast is None:
                return out, 0.0
            tend = self.tlast
            tstart = tend - int(seconds * 1e6)
            for name in names:
                d = self.data.get(name)
                if d is None:
                    continue
                ts, vs = d
                # the deques are sorted: walk back from the end to the start of the window
                n = len(ts)
                i = n
                while i > 0 and ts[i - 1] >= tstart:
                    i -= 1
                step = max(1, (n - i) // maxpoints)
                t0 = self.t0
                out[name] = [[(ts[k] - t0) / 1e6, vs[k]] for k in range(i, n, step)]
            return out, (tend - self.t0) / 1e6

    def record(self, path):
        with self.lock:
            if self.csv is not None:
                self.csv.close()
            self.csv = CsvLog(path) if path else None
            self.csvpath = path


PALETTE = ['#2563eb', '#dc2626', '#16a34a', '#d97706', '#7c3aed', '#0891b2', '#db2777', '#4b5563',
           '#65a30d', '#ea580c', '#0d9488', '#9333ea']


def parse_hostport(text):
    host, sep, port = (text or '').strip().rpartition(':')
    if not sep or not host or not port.isdigit() or not 0 < int(port) < 65536:
        raise ValueError(f'"{text}" is not host:port, e.g. 127.0.0.1:9870')
    return host, int(port)


class StrPage:

    def __init__(self, store, args):
        self.store = store
        self.args = args
        self.selected = []              # signals to plot, in order of selection
        self.known = []                 # signals shown in the list
        self.boxes = {}                 # signal -> (checkbox, value label)
        self.paused = False
        self.charts = []
        self.lastframes = 0
        self.lasttick = time.monotonic()

    def build(self):
        ui.colors(primary='#1d4ed8')
        ui.query('body').classes('bg-slate-50')
        with ui.header().classes('items-center gap-6 py-2 bg-slate-800'):
            ui.label('TETHER · stream').classes('text-lg font-bold tracking-wide')
            ui.label(f'UDP :{self.args.localport}').classes('font-mono opacity-80')
            self.status = ui.label('waiting for the stream ...').classes('font-mono text-sm opacity-80')
            ui.space()
            self.pingstatus = ui.label().classes('font-mono text-sm')

        with ui.row().classes('w-full no-wrap items-start gap-4 p-2'):
            with ui.column().classes('w-80 shrink-0 gap-4'):
                with ui.card().classes('w-full'):
                    ui.label('signals').classes('font-bold')
                    ui.label('they appear when the board streams them. tick to plot').classes('text-xs text-gray-500')
                    self.siglist = ui.column().classes('w-full gap-0')
                with ui.card().classes('w-full'):
                    ui.label('plot').classes('font-bold')
                    with ui.row().classes('items-center gap-2'):
                        self.window = ui.number('window s', value=self.args.window, min=0.1, step=1) \
                            .props('dense outlined').classes('w-28')
                        self.pausebtn = ui.button('pause', icon='pause', on_click=self.on_pause).props('dense')
                    self.split = ui.switch('one chart per signal', on_change=lambda: self.rebuild_charts())
                    ui.button('clear data', icon='delete_sweep', on_click=self.on_clear).props('dense outline')
                with ui.card().classes('w-full'):
                    ui.label('record').classes('font-bold')
                    self.csvpath = ui.input('csv file', value='stream.csv').props('dense outlined').classes('w-full')
                    self.recbtn = ui.button('record', icon='fiber_manual_record', on_click=self.on_record) \
                        .props('dense color=red')
                    self.recstatus = ui.label().classes('text-xs text-gray-500 font-mono')
                with ui.card().classes('w-full'):
                    ui.label('PlotJuggler').classes('font-bold')
                    ui.label('forwards every frame as JSON to the UDP Server of PlotJuggler') \
                        .classes('text-xs text-gray-500')
                    pj = self.store.pj
                    self.pjaddr = ui.input('host:port', value=f'{pj[0]}:{pj[1]}' if pj else self.args.pj_default) \
                        .props('dense outlined').classes('w-full font-mono')
                    self.pjswitch = ui.switch('forward', value=pj is not None, on_change=self.on_pj)
                    self.pjstatus = ui.label().classes('text-xs text-gray-500 font-mono')
            self.plots = ui.column().classes('grow gap-2')

        self.rebuild_charts()
        ui.timer(1.0 / self.args.fps, self.on_tick)

    def chart_options(self, names):
        return {
            'animation': False,
            'grid': {'left': 70, 'right': 24, 'top': 36, 'bottom': 48},
            'tooltip': {'trigger': 'axis', 'axisPointer': {'type': 'line'}},
            'legend': {'top': 4, 'type': 'scroll'},
            'xAxis': {'type': 'value', 'name': 'board time [s]', 'nameLocation': 'middle', 'nameGap': 28,
                      'axisLabel': {'showMinLabel': False, 'showMaxLabel': False}},
            'yAxis': {'type': 'value', 'scale': True},
            'dataZoom': [{'type': 'inside', 'disabled': True}],
            'series': [{'name': n, 'type': 'line', 'showSymbol': False, 'data': [],
                        'lineStyle': {'width': 1.5}, 'color': self.color(n)} for n in names],
        }

    def color(self, name):
        order = self.store.order
        return PALETTE[order.index(name) % len(PALETTE)] if name in order else PALETTE[0]

    def rebuild_charts(self):
        """one chart w/ every selected signal, or one chart per signal. the charts are reused when possible:
        echarts does not like to be destroyed and created again at every click"""
        groups = [[n] for n in self.selected] if self.split.value and self.selected else [list(self.selected)]
        if len(groups) == len(self.charts) and self.charts:
            for (ch, _), names in zip(self.charts, groups):
                ch.options['series'] = self.chart_options(names)['series']
            self.charts = [(ch, names) for (ch, _), names in zip(self.charts, groups)]
            self.draw(force=True)
            return
        self.plots.clear()
        self.charts = []
        height = 'h-[78vh]' if len(groups) == 1 else ('h-80' if len(groups) <= 3 else 'h-60')
        with self.plots:
            for names in groups:
                with ui.card().classes('w-full p-1'):
                    ch = ui.echart(self.chart_options(names)).classes(f'w-full {height}')
                self.charts.append((ch, names))
        self.draw(force=True)

    def draw(self, force=False):
        if self.paused and not force:
            return
        span = max(0.1, float(self.window.value or self.args.window))
        data, tend = self.store.window(self.selected, span, self.args.maxpoints)
        for ch, names in self.charts:
            for s, n in zip(ch.options['series'], names):
                s['data'] = data.get(n, [])
            ch.options['xAxis']['min'] = round(max(0.0, tend - span), 6)
            ch.options['xAxis']['max'] = round(max(span, tend), 6)
            ch.options['dataZoom'][0]['disabled'] = not self.paused
            ch.update()

    def on_tick(self):
        st, rx = self.store, self.store.rx
        if len(st.order) != len(self.known):
            self.known = list(st.order)
            self.rebuild_list()
        for name, (_, lab) in self.boxes.items():
            v = st.last.get(name)
            lab.text = '' if v is None else (f'{v:.6g}' if v != int(v) or abs(v) >= 1e9 else f'{int(v)}')
        now = time.monotonic()
        fps = (st.frames - self.lastframes) / max(1e-3, now - self.lasttick)
        self.lastframes, self.lasttick = st.frames, now
        per = list(st.periods)[-200:]
        per_txt = f'period {sum(per) / len(per) / 1000:.2f} ms' if per else 'period n/a'
        board = rc.format_time(st.tlast) if st.tlast is not None else 'n/a'
        self.status.text = f'{fps:6.1f} frames/s · {per_txt} · board time {board}'
        if rx.target is None:
            self.pingstatus.text = 'ping: waiting for the first frame'
            self.pingstatus.classes(replace='font-mono text-sm')
        elif rx.lostinarow >= 2:
            self.pingstatus.text = f'ping: no reply from {rx.target[0]}'
            self.pingstatus.classes(replace='font-mono text-sm text-red-300')
        else:
            rtt = f'{rx.lastrtt:.2f} ms' if rx.lastrtt is not None else '...'
            self.pingstatus.text = f'ping {rx.target[0]}: rtt {rtt}'
            self.pingstatus.classes(replace='font-mono text-sm text-green-300')
        if st.pj is not None:
            err = f', {st.pjerrors} errors' if st.pjerrors else ''
            self.pjstatus.text = f'sent {st.pjsent} frames to {st.pj[0]}:{st.pj[1]}{err}'
        else:
            self.pjstatus.text = 'off'
        if st.csv is not None:
            self.recstatus.text = f'recording to {st.csv.path()}'
        self.draw()

    def rebuild_list(self):
        self.siglist.clear()
        self.boxes.clear()
        with self.siglist:
            for name in self.known:
                with ui.row().classes('w-full items-center no-wrap gap-1'):
                    cb = ui.checkbox(name, value=name in self.selected,
                                     on_change=lambda ev, n=name: self.on_select(n, ev.value)).classes('font-mono text-sm')
                    ui.space()
                    lab = ui.label().classes('font-mono text-xs text-gray-500')
                self.boxes[name] = (cb, lab)

    def on_select(self, name, on):
        if on and name not in self.selected:
            self.selected.append(name)
        elif not on and name in self.selected:
            self.selected.remove(name)
        self.rebuild_charts()

    def on_pause(self):
        self.paused = not self.paused
        self.pausebtn.text = 'resume' if self.paused else 'pause'
        self.pausebtn.props(f'icon={"play_arrow" if self.paused else "pause"}')
        self.draw(force=True)
        if self.paused:
            ui.notify('paused: zoom w/ the wheel, drag to pan', position='bottom-right')

    def on_pj(self, ev):
        if not ev.value:
            self.store.plotjuggler(None)
            return
        try:
            self.store.plotjuggler(parse_hostport(self.pjaddr.value))
        except ValueError as e:
            ui.notify(str(e), type='negative')
            self.pjswitch.value = False

    def on_clear(self):
        self.store.clear()
        self.known = []
        self.rebuild_list()
        self.draw(force=True)

    def on_record(self):
        st = self.store
        if st.csv is None:
            path = (self.csvpath.value or '').strip()
            if not path:
                return ui.notify('give a csv file name', type='warning')
            st.record(path)
            self.recbtn.text = 'stop'
            self.recbtn.props('icon=stop')
            self.recstatus.text = f'recording to {path} (it starts at the next frame)'
        else:
            last = st.csv.path() if st.csv.file else None
            st.record(None)
            self.recbtn.text = 'record'
            self.recbtn.props('icon=fiber_manual_record')
            self.recstatus.text = f'saved {last}' if last else 'nothing recorded'


def str_page(args):
    load_nicegui()
    proto = load_dictionary(args.dictionary)
    if proto is None:
        print(f'[warn] {args.dictionary} does not exist: run `tether.py rpc` once to write it. the values are taken raw')
        proto = rc.Protocol()
    store = StrStore(args, rc.Codec(proto))
    args.pj_default = args.pj or '127.0.0.1:9870'
    if args.pj:
        try:
            store.plotjuggler(parse_hostport(args.pj))
        except ValueError as e:
            sys.exit(f'[error] --pj: {e}')
        print(f'[info] forwarding the stream to PlotJuggler at {args.pj}')
    store.start()

    @ui.page('/')
    def index():
        StrPage(store, args).build()

    ui.run(title='TETHER stream', port=args.gui_port, reload=False, show=not args.no_open, favicon='📈')


# ---------------------------------------------------------------------------------------------------------------------

def main():
    ap = argparse.ArgumentParser(prog='tether.py', description='host of TETHER, the diagnostic and operations service '
                                 'of the amcmj1 board (ROP protocol 5): rpc = variables and operations, str = the stream')
    sub = ap.add_subparsers(dest='service', metavar='{rpc,str}', required=True)

    common = argparse.ArgumentParser(add_help=False)
    common.add_argument('--dictionary', default=DEFAULT_DICTIONARY,
                        help=f'the Dictionary of the board, written by the browse (default {DEFAULT_DICTIONARY})')
    common.add_argument('--cli', action='store_true', help='text interface instead of the page')
    common.add_argument('--no-open', action='store_true', help='do not open the browser')

    r = sub.add_parser('rpc', parents=[common], help='variables and operations of the board (UDP 6666)')
    r.add_argument('--ip', required=True, help='IP address of the board')
    r.add_argument('--port', type=int, default=6666, help='UDP port of the rpc socket on the board (default 6666)')
    r.add_argument('--no-browse', action='store_true', help='do not ask the board for its Dictionary: use the file as it is')
    r.add_argument('--localport', type=int, default=0, help='local UDP port (default: any)')
    r.add_argument('--timeout', type=float, default=0.5, help='seconds to wait for a reply (default 0.5)')
    r.add_argument('--tries', type=int, default=3, help='page: transmissions of ask / set before giving up (default 3)')
    r.add_argument('--verbose', action='store_true', help='--cli: hex dump of tx and rx frames')
    r.add_argument('--gui-port', type=int, default=8080, help='page: local port of the web page (default 8080)')
    r.add_argument('--native', action='store_true', help='page: open in its own window (needs pywebview)')

    s = sub.add_parser('str', parents=[common], help='the stream of the board (UDP 7777)')
    s.add_argument('--localport', type=int, default=7777, help='local UDP port where the board streams (default 7777)')
    s.add_argument('--ip', default=None, help='IP of the board: frames from other IPs are ignored, and pings go '
                                              'there (default: the source of the first frame)')
    s.add_argument('--boardport', type=int, default=7777, help='UDP port of the streaming socket of the board (default 7777)')
    s.add_argument('--ping', type=float, default=2.0, help='seconds between the silent pings that keep the stream '
                                                         'alive (default 2, 0 = no ping)')
    s.add_argument('--every', type=float, default=1.0, help='--cli: seconds between summaries (default 1.0)')
    s.add_argument('--all', action='store_true', help='--cli: print every frame instead of a periodic summary')
    s.add_argument('--quiet', action='store_true', help='--cli: no periodic summary, only when the stream starts or stops')
    s.add_argument('--rtt', action='store_true', help='--cli: show the round trip time of the pings')
    s.add_argument('--log', default=None, help='--cli: csv file where to log the stream')
    s.add_argument('--window', type=float, default=10.0, help='page: seconds shown by the plot (default 10)')
    s.add_argument('--history', type=int, default=200000, help='page: samples kept per signal (default 200000)')
    s.add_argument('--maxpoints', type=int, default=3000, help='page: max points drawn per signal (default 3000)')
    s.add_argument('--fps', type=float, default=10.0, help='page: refreshes of the plot per second (default 10)')
    s.add_argument('--gui-port', type=int, default=8081, help='page: local port of the web page (default 8081)')
    s.add_argument('--pj', default=None, metavar='HOST:PORT',
                   help='page: forward the stream to PlotJuggler from the start, e.g. --pj 127.0.0.1:9870')

    args = ap.parse_args()
    if args.service == 'rpc':
        (rpc_cli if args.cli else rpc_page)(args)
    else:
        (str_cli if args.cli else str_page)(args)


if __name__ == '__main__':
    main()

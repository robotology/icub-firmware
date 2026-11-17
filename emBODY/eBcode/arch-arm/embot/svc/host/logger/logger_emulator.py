#!/usr/bin/env python3
# logger_emulator - a board w/ theLogger, on the PC: to try logger.py w/out a board, and to test it.
# it follows embot_net_theLogger.cpp: the same commands, the same replies, the same decoration. the lines are made up:
# a counter, and now and then a longer line.
#
#   python3 logger_emulator.py                          UDP 9999, a line every 100 ms, the lines go nowhere until hello
#   python3 logger_emulator.py --port 19999 --dest 127.0.0.1:9999 --period 0.02 --name 10.0.1.98
#
# --name is the IP that the decoration shows (default the one of the socket, 127.0.0.1).

import argparse
import re
import socket
import threading
import time

ADDR_RE = re.compile(r'^(\d+)\.(\d+)\.(\d+)\.(\d+):(\d+)$')
HELP = [
    'help: hello (or an empty line): the log goes to who sends it',
    'help: on: the lines are taken and sent',
    'help: off: the lines are discarded, also those not yet sent',
    'help: deco on: each line begins w/ [<ip of the board> @ <time>]',
    'help: deco off: the lines w/out decoration',
    'help: dest <ip>:<port>: the log goes to <ip>:<port>, e.g. dest 10.0.1.104:9999',
    'help: status: the destination, the modes and the counters',
    'help: help: this list',
]


class Emulator:
    def __init__(self, args):
        self.args = args
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(('', args.port))
        self.sock.settimeout(0.01)
        self.dest = None
        if args.dest:
            m = ADDR_RE.match(args.dest)
            self.dest = (f'{m.group(1)}.{m.group(2)}.{m.group(3)}.{m.group(4)}', int(m.group(5)))
        self.on = True
        self.deco = args.deco
        self.lines = self.frames = self.dropped = 0
        self.buffer = []
        self.t0 = time.monotonic()
        self.lock = threading.Lock()

    def now_us(self):
        return int((time.monotonic() - self.t0) * 1e6)

    def puts(self, text):
        with self.lock:
            if not self.on:
                return
            if self.deco:
                us = self.now_us()
                s, rest = divmod(us, 1000000)
                text = f'[{self.args.name} @ S{s}:m{rest // 1000}:u{rest % 1000}] {text}'
            self.buffer.append(text + '\n')
            self.lines += 1

    def flush(self):
        with self.lock:
            if not self.buffer or self.dest is None:
                return
            frame, self.buffer = ''.join(self.buffer), []
            self.frames += 1
            dest = self.dest
        self.sock.sendto(frame.encode(), dest)

    def answer(self, to, text):
        self.sock.sendto(('[theLogger] ' + text + '\n').encode(), to)

    def destination(self):
        return 'none' if self.dest is None else f'{self.dest[0]}:{self.dest[1]}'

    def onoff(self, v):
        return 'on' if v else 'off'

    def execute(self, cmd, src):
        cmd = cmd.strip().lower()
        if cmd in ('', 'hello'):
            self.dest = src
            self.answer(src, f'ok: dest {self.destination()}, {self.onoff(self.on)}, deco {self.onoff(self.deco)}')
        elif cmd in ('on', 'off'):
            with self.lock:
                self.on = cmd == 'on'
                if not self.on:
                    self.buffer = []
            self.answer(src, f'ok: {cmd}')
        elif cmd in ('deco on', 'deco off'):
            self.deco = cmd == 'deco on'
            self.answer(src, f'ok: {cmd}')
        elif cmd.startswith('dest ') and len(cmd) > 5:
            m = ADDR_RE.match(cmd[5:].strip())
            if m:
                self.dest = (f'{m.group(1)}.{m.group(2)}.{m.group(3)}.{m.group(4)}', int(m.group(5)))
                self.answer(src, f'ok: dest {self.destination()}, {self.onoff(self.on)}, deco {self.onoff(self.deco)}')
            else:
                self.answer(src, f'error: bad address "{cmd[5:]}" (e.g. dest 10.0.1.104:9999)')
        elif cmd == 'status':
            self.answer(src, f'status: dest {self.destination()}, {self.onoff(self.on)}, deco {self.onoff(self.deco)}, '
                             f'commands on, lines {self.lines}, frames {self.frames}, dropped {self.dropped}')
        elif cmd == 'help':
            self.sock.sendto(''.join(f'[theLogger] {h}\n' for h in HELP).encode(), src)
        else:
            self.answer(src, f'error: unknown command "{cmd}" (help: the list of the commands)')

    def run(self):
        n = 0
        nextline = time.monotonic()
        end = time.monotonic() + self.args.seconds if self.args.seconds > 0 else None
        while end is None or time.monotonic() < end:
            try:
                data, src = self.sock.recvfrom(256)
                self.execute(data.decode('ascii', 'replace'), src)
            except socket.timeout:
                pass
            now = time.monotonic()
            if now >= nextline:
                nextline = now + self.args.period
                n += 1
                self.puts(f'tick {n}: wave = {((n * 7) % 200 - 100) / 10:.1f}')
                if n % 10 == 0:
                    self.puts(f'echo: {n % 13 + 3} bytes from 10.0.1.104:59270, replied')
                if n % 50 == 0:
                    self.puts(f'warning: the queue of tCAN is at {n % 90 + 10} %')
            self.flush()


def main():
    ap = argparse.ArgumentParser(description='a board w/ theLogger, on the PC')
    ap.add_argument('--port', type=int, default=9999, help='the UDP port of theLogger (default 9999)')
    ap.add_argument('--dest', default=None, help='the destination at start, ip:port (default none: until hello)')
    ap.add_argument('--deco', action='store_true', help='decoration on at start')
    ap.add_argument('--period', type=float, default=0.1, help='seconds between two lines (default 0.1)')
    ap.add_argument('--name', default='127.0.0.1', help='the IP shown by the decoration')
    ap.add_argument('--seconds', type=float, default=0, help='stop after so many seconds (default never)')
    Emulator(ap.parse_args()).run()


if __name__ == '__main__':
    main()

#!/usr/bin/env python3
# logger - the viewer of theLogger, the log over UDP of the embot boards (embot::net::theLogger, one day embot::svc).
#
# the board sends the lines of embot::core::print() over UDP and accepts a few text commands on the same port (9999):
#   (empty) or hello      the lines go to the sender, from now on
#   on / off              the lines are taken and sent, or discarded
#   deco on / deco off    each line begins w/ [<ip of the board> @ S<seconds>:m<ms>:u<us>]
#   dest <ip>:<port>      the lines go elsewhere
#   status                the destination, the modes and the counters
#   help                  the list of the commands
# every command has a reply to its sender, beginning w/ [theLogger]. a print() never begins so: the viewer tells them apart.
#
# one program, two faces:
#
#   python3 logger.py --ip 10.0.1.99                       a page in the browser (nicegui), the default
#   python3 logger.py --ip 10.0.1.99 --ip 10.0.1.98        several boards at once, one colour each
#   python3 logger.py --ip 10.0.1.99 --cli                 the same, in the terminal
#   python3 logger.py --localport 9999                     no hello: it receives the boards that log to this PC:9999
#                                                          (their destination at start), and adds them as they speak
#
# the viewer has one UDP socket, on --localport (default: any free port). at start, and w/ the button hello, it sends hello
# to each board given w/ --ip: their lines come to this socket. a board that sends to this socket w/out being asked (e.g.
# because its destination is this PC:9999 and the viewer listens on 9999) is added by itself.
#
# the page: the lines of all the boards, w/ the time of the PC, the board, the time of the board (from the decoration)
# and the text; a filter by text or regular expression; a pause that keeps receiving; the switches on and deco and the
# counters of status (also the lines lost by the board) for each board, polled every --poll s; record to .txt or .csv.
#
# the cli: the lines as they arrive. a line typed is a command for every board, "<ip>: <command>" for one board only
# (e.g. "10.0.1.99: deco on"). quit to exit.

import argparse
import collections
import csv
import datetime
import os
import re
import socket
import sys
import threading
import time

ui = None                       # nicegui, imported only by the page

DEFAULT_BOARDPORT = 9999
REPLY = '[theLogger] '
# [10.0.1.99 @ S12:m345:u678] text
DECO_RE = re.compile(r'^\[(\d+\.\d+\.\d+\.\d+) @ S(\d+):m(\d+):u(\d+)\] ?(.*)$')
# status: dest 10.0.1.104:54321, on, deco off, commands on, lines 1234, frames 310, dropped 0
STATUS_RE = re.compile(r'^status: dest (\S+), (on|off), deco (on|off), commands (on|off), lines (\d+), frames (\d+), dropped (\d+)')
# ok: dest 10.0.1.104:54321, on, deco off      (the reply to hello and to dest)
DEST_RE = re.compile(r'^ok: dest (\S+), (on|off), deco (on|off)')

PALETTE = ['#2563eb', '#dc2626', '#16a34a', '#d97706', '#7c3aed', '#0891b2', '#db2777', '#4b5563']
ANSI = ['\033[34m', '\033[31m', '\033[32m', '\033[33m', '\033[35m', '\033[36m', '\033[95m', '\033[90m']


class Record:
    """a line received: from the log (reply False) or the reply to a command (reply True)"""

    __slots__ = ('seq', 'pctime', 'board', 'boardtime', 'text', 'reply')

    def __init__(self, seq, pctime, board, boardtime, text, reply):
        self.seq = seq
        self.pctime = pctime            # time.time() at the arrival on the PC
        self.board = board              # 'ip:port' of the board that sent it
        self.boardtime = boardtime      # microseconds of the board from the decoration, None w/out decoration
        self.text = text                # w/out the decoration
        self.reply = reply


def format_pctime(t):
    return datetime.datetime.fromtimestamp(t).strftime('%H:%M:%S.%f')[:-3]


def format_boardtime(us):
    return '-' if us is None else f'{us / 1e6:.6f}'


def parse_hostport(text, default_port):
    host, sep, port = (text or '').strip().partition(':')
    if not host:
        raise ValueError('give the IP of the board, e.g. 10.0.1.99 or 10.0.1.99:9999')
    if sep and not port.isdigit():
        raise ValueError(f'"{text}": the port must be a number')
    try:
        socket.inet_aton(host)
    except OSError:
        host = socket.gethostbyname(host)
    return host, int(port) if sep else default_port


class Board:
    """what the viewer knows of a board: its address, its colour, the last status"""

    def __init__(self, address, colour):
        self.address = address          # (ip, port)
        self.name = f'{address[0]}:{address[1]}'
        self.colour = colour
        self.dest = None                # the destination of its lines, as the board says
        self.mine = None                # this viewer as the board sees it (from the reply to hello)
        self.on = None
        self.deco = None
        self.lines = self.frames = self.dropped = None
        self.lastseen = None            # time.time() of the last datagram from it
        self.lastreply = None
        self.received = 0               # log lines received here
        self.polls = 0                  # status sent by the poll whose reply is not shown

    def tothis(self):
        """True if its lines come to this viewer, False if they go elsewhere, None if unknown"""
        if self.dest is None or self.mine is None:
            return None
        return self.dest == self.mine

    def summary(self):
        if self.on is None:
            return 'no status yet'
        where = {True: 'to this PC', False: f'elsewhere ({self.dest})', None: f'to {self.dest}'}[self.tothis()]
        out = f'{"on" if self.on else "off"} · deco {"on" if self.deco else "off"} · {where}'
        if self.lines is not None:
            out += f' · lines {self.lines}, frames {self.frames}, dropped {self.dropped}'
        return out


class Hub:
    """the socket of the viewer, the boards and the records. a thread receives; anyone reads"""

    def __init__(self, localport, history):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            self.sock.bind(('', localport))
        except OSError as e:
            sys.exit(f'[error] cannot bind UDP port {localport}: {e}')
        self.sock.settimeout(0.2)
        self.localport = self.sock.getsockname()[1]
        self.lock = threading.Lock()
        self.boards = {}                # (ip, port) -> Board
        self.records = collections.deque(maxlen=history)
        self.seq = 0
        self.csv = None
        self.txt = None
        self.recpath = None
        self.stop = threading.Event()
        self.onrecord = None            # optional callback(Record), from the thread of rx

    # ---- the boards

    def add(self, address):
        with self.lock:
            b = self.boards.get(address)
            if b is None:
                b = self.boards[address] = Board(address, len(self.boards))
            return b

    def remove(self, address):
        with self.lock:
            self.boards.pop(address, None)

    def board_list(self):
        with self.lock:
            return list(self.boards.values())

    def send(self, board, command, poll=False):
        if poll:
            board.polls += 1
        try:
            self.sock.sendto(command.encode('ascii', 'replace'), board.address)
        except OSError:
            pass

    # ---- rx

    def start(self):
        threading.Thread(target=self.loop, daemon=True).start()

    def loop(self):
        while not self.stop.is_set():
            try:
                data, src = self.sock.recvfrom(65535)
            except socket.timeout:
                continue
            except OSError:                     # e.g. ICMP port unreachable on windows
                continue
            self.on_datagram(data, src)

    def on_datagram(self, data, src):
        now = time.time()
        board = self.add(src)                   # a board that speaks w/out being asked is added
        board.lastseen = now
        text = data.decode('utf-8', 'replace')
        for line in text.split('\n'):
            line = line.rstrip('\r')
            if not line:
                continue
            if line.startswith(REPLY):
                body = line[len(REPLY):]
                board.lastreply = body
                if self.on_reply(board, body):
                    continue                    # the answer to a poll: not shown
                self.store(Record(0, now, board.name, None, line, True))
            else:
                m = DECO_RE.match(line)
                if m:
                    bt = int(m.group(2)) * 1000000 + int(m.group(3)) * 1000 + int(m.group(4))
                    self.store(Record(0, now, board.name, bt, m.group(5), False))
                else:
                    self.store(Record(0, now, board.name, None, line, False))
                board.received += 1

    def on_reply(self, board, body):
        """it updates the board from a reply. True if the reply answers a poll and must not be shown"""
        m = STATUS_RE.match(body)
        if m:
            board.dest = m.group(1)
            board.on = m.group(2) == 'on'
            board.deco = m.group(3) == 'on'
            board.lines, board.frames, board.dropped = int(m.group(5)), int(m.group(6)), int(m.group(7))
            if board.polls > 0:
                board.polls -= 1
                return True
            return False
        m = DEST_RE.match(body)
        if m:
            board.dest = m.group(1)
            board.on = m.group(2) == 'on'
            board.deco = m.group(3) == 'on'
            return False
        if body in ('ok: on', 'ok: off'):
            board.on = body == 'ok: on'
        elif body in ('ok: deco on', 'ok: deco off'):
            board.deco = body == 'ok: deco on'
        return False

    def store(self, r):
        with self.lock:
            self.seq += 1
            r.seq = self.seq
            self.records.append(r)
            if self.csv is not None:
                self.csv.writerow([f'{r.pctime:.6f}', r.board, '' if r.boardtime is None else r.boardtime,
                                   'reply' if r.reply else 'line', r.text])
            elif self.txt is not None:
                self.txt.write(format_record(r) + '\n')
        if self.onrecord is not None:
            self.onrecord(r)

    def since(self, seq):
        """the records after seq"""
        with self.lock:
            if not self.records or self.records[-1].seq <= seq:
                return []
            return [r for r in self.records if r.seq > seq]

    def all(self):
        with self.lock:
            return list(self.records)

    def clear(self):
        with self.lock:
            self.records.clear()

    # ---- record

    def record(self, path):
        """path ending in .csv: one row per line (pc time, board, board time in us, kind, text). else plain text.
        None stops"""
        with self.lock:
            if self._file is not None:
                self._file.close()
            self._file = self.csv = self.txt = None
            self.recpath = path
            if path:
                self._file = open(path, 'w', newline='' if path.lower().endswith('.csv') else None, encoding='utf-8')
                if path.lower().endswith('.csv'):
                    self.csv = csv.writer(self._file)
                    self.csv.writerow(['pctime', 'board', 'boardtime_us', 'kind', 'text'])
                else:
                    self.txt = self._file

    _file = None


def matcher(text, regex, case):
    """a function Record -> bool for the filter, or raises re.error"""
    if not text:
        return lambda r: True
    if regex:
        rx = re.compile(text, 0 if case else re.IGNORECASE)
        return lambda r: rx.search(r.text) is not None
    if case:
        return lambda r: text in r.text
    low = text.lower()
    return lambda r: low in r.text.lower()


def format_record(r):
    return f'{format_pctime(r.pctime)}  {r.board:<21}  {format_boardtime(r.boardtime):>14}  {r.text}'


# ---------------------------------------------------------------------------------------------------------------------
# cli

def run_cli(args, hub):
    colours = sys.stdout.isatty()
    lock = threading.Lock()

    def out(r):
        b = hub.boards.get(LoggerPage.address_of(r.board))
        line = format_record(r)
        if colours and b is not None:
            line = ANSI[b.colour % len(ANSI)] + line + '\033[0m'
        with lock:
            print(line, flush=True)

    hub.onrecord = out
    hub.start()
    if not args.no_hello:
        for b in hub.board_list():
            hub.send(b, 'hello')
    print(f'[info] logger listens on UDP {hub.localport}. a command for all the boards, or "<ip>: <command>". quit to exit',
          flush=True)
    try:
        for line in sys.stdin:
            line = line.strip()
            if line in ('quit', 'exit', 'q'):
                break
            target, sep, cmd = line.partition(': ')
            boards = hub.board_list()
            if sep:
                try:
                    addr = parse_hostport(target, DEFAULT_BOARDPORT)
                except (ValueError, OSError) as e:
                    print(f'[error] {e}')
                    continue
                boards = [hub.add(addr)]
                line = cmd
            if not boards:
                print('[error] no board: give one w/ "<ip>: hello"')
            for b in boards:
                hub.send(b, line)
    except KeyboardInterrupt:
        pass
    hub.stop.set()


# ---------------------------------------------------------------------------------------------------------------------
# page

class LoggerPage:

    def __init__(self, hub, args):
        self.hub = hub
        self.args = args
        self.paused = False
        self.lastseq = 0
        self.filter = lambda r: True
        self.cards = {}                 # (ip, port) -> (status label, on switch, deco switch, show checkbox, warn label)
        self.hidden = set()             # boards not shown
        self.syncing = False
        self.nextpoll = 0.0

    def build(self):
        ui.colors(primary='#1d4ed8')
        ui.query('body').classes('bg-slate-50')
        with ui.header().classes('items-center gap-6 py-2 bg-slate-800'):
            ui.label('theLogger · viewer').classes('text-lg font-bold tracking-wide')
            ui.label(f'UDP :{self.hub.localport}').classes('font-mono opacity-80')
            self.rate = ui.label().classes('font-mono text-sm opacity-80')
        with ui.row().classes('w-full no-wrap items-start gap-4 p-2'):
            with ui.column().classes('w-[430px] shrink-0 gap-4'):
                with ui.card().classes('w-full'):
                    ui.label('boards').classes('font-bold')
                    with ui.row().classes('w-full items-center no-wrap gap-2'):
                        self.newboard = ui.input('ip[:port]', placeholder='10.0.1.99').props('dense outlined') \
                            .classes('grow font-mono')
                        self.newboard.on('keydown.enter', self.on_add)
                        ui.button('add', icon='add', on_click=self.on_add).props('dense')
                    self.boardlist = ui.column().classes('w-full gap-2')
                with ui.card().classes('w-full'):
                    ui.label('filter').classes('font-bold')
                    self.ftext = ui.input('text', on_change=self.on_filter).props('dense outlined clearable').classes('w-full font-mono')
                    with ui.row().classes('items-center gap-2'):
                        self.fregex = ui.switch('regular expression', on_change=self.on_filter)
                        self.fcase = ui.switch('case', on_change=self.on_filter)
                    self.freplies = ui.switch('show the replies [theLogger]', value=True, on_change=self.on_filter)
                    self.ferror = ui.label().classes('text-xs text-red-700')
                with ui.card().classes('w-full'):
                    ui.label('view').classes('font-bold')
                    with ui.row().classes('items-center gap-2'):
                        self.pausebtn = ui.button('pause', icon='pause', on_click=self.on_pause).props('dense')
                        ui.button('clear', icon='delete_sweep', on_click=self.on_clear).props('dense outline')
                    self.pausestate = ui.label().classes('text-xs text-gray-500')
                with ui.card().classes('w-full'):
                    ui.label('record').classes('font-bold')
                    ui.label('every line received, also the filtered ones. .csv: pc time, board, board time [us], kind, text') \
                        .classes('text-xs text-gray-500')
                    self.recpath = ui.input('file', value='logger.txt').props('dense outlined').classes('w-full font-mono')
                    self.recbtn = ui.button('record', icon='fiber_manual_record', on_click=self.on_record).props('dense color=red')
                    self.recstate = ui.label().classes('text-xs text-gray-500 font-mono')
            with ui.column().classes('grow gap-1'):
                ui.label('pc time       board                     board time [s]  text') \
                    .classes('font-mono text-xs text-gray-500 whitespace-pre pl-2')
                self.view = ui.log(max_lines=self.args.lines).classes('w-full h-[84vh] font-mono text-xs bg-white')
        self.rebuild_boards()
        self.refill()
        ui.timer(0.2, self.on_tick)
        ui.timer(1.0, self.on_status)

    # ---- the boards

    def rebuild_boards(self):
        self.boardlist.clear()
        self.cards.clear()
        with self.boardlist:
            boards = self.hub.board_list()
            if not boards:
                ui.label('no board yet: add one, or wait for a board that logs to this port').classes('text-xs text-gray-500')
            for b in boards:
                colour = PALETTE[b.colour % len(PALETTE)]
                with ui.column().classes('w-full gap-0 border-l-4 pl-2').style(f'border-color: {colour}'):
                    with ui.row().classes('w-full items-center no-wrap gap-1'):
                        show = ui.checkbox(value=b.address not in self.hidden,
                                           on_change=lambda ev, a=b.address: self.on_show(a, ev.value)).props('dense')
                        ui.label(b.name).classes('font-mono font-medium').style(f'color: {colour}')
                        ui.space()
                        ui.button('hello', on_click=lambda a=b.address: self.command(a, 'hello')).props('dense flat size=sm') \
                            .tooltip('the lines of this board go to this viewer')
                        ui.button('status', on_click=lambda a=b.address: self.command(a, 'status')).props('dense flat size=sm')
                        ui.button('help', on_click=lambda a=b.address: self.command(a, 'help')).props('dense flat size=sm')
                        ui.button(icon='close', on_click=lambda a=b.address: self.on_remove(a)).props('dense flat round size=sm')
                    with ui.row().classes('items-center gap-2'):
                        on = ui.switch('on', value=bool(b.on), on_change=lambda ev, a=b.address: self.on_switch(a, 'on', ev.value))
                        deco = ui.switch('deco', value=bool(b.deco),
                                         on_change=lambda ev, a=b.address: self.on_switch(a, 'deco', ev.value))
                    status = ui.label(b.summary()).classes('text-xs font-mono text-gray-600')
                    warn = ui.label().classes('text-xs text-red-700')
                self.cards[b.address] = (status, on, deco, show, warn)

    def command(self, address, cmd):
        b = self.hub.boards.get(address)
        if b is not None:
            self.hub.send(b, cmd)

    def on_add(self):
        try:
            address = parse_hostport(self.newboard.value, self.args.boardport)
        except (ValueError, OSError) as e:
            return ui.notify(str(e), type='warning')
        b = self.hub.add(address)
        self.hub.send(b, 'hello')
        self.newboard.value = ''
        self.rebuild_boards()

    def on_remove(self, address):
        self.hub.remove(address)
        self.hidden.discard(address)
        self.rebuild_boards()
        self.refill()

    def on_show(self, address, on):
        if on:
            self.hidden.discard(address)
        else:
            self.hidden.add(address)
        self.refill()

    def on_switch(self, address, what, on):
        if self.syncing:
            return
        cmd = ('on' if on else 'off') if what == 'on' else ('deco on' if on else 'deco off')
        self.command(address, cmd)

    def on_status(self):
        """every second: the cards from what the boards said. every --poll s a silent status to each board"""
        now = time.time()
        boards = self.hub.board_list()
        if len(boards) != len(self.cards):
            self.rebuild_boards()
        self.syncing = True
        try:
            for b in boards:
                card = self.cards.get(b.address)
                if card is None:
                    continue
                status, on, deco, _, warn = card
                status.text = b.summary()
                if b.on is not None and on.value != b.on:
                    on.value = b.on
                if b.deco is not None and deco.value != b.deco:
                    deco.value = b.deco
                if b.lastseen is None:
                    warn.text = 'silent: is the board up? is the port right?'
                elif b.tothis() is False:
                    warn.text = 'its lines go elsewhere: press hello to have them here'
                elif now - b.lastseen > 3 * self.args.poll + 2:
                    warn.text = f'silent for {now - b.lastseen:.0f} s'
                else:
                    warn.text = ''
        finally:
            self.syncing = False
        if self.args.poll > 0 and now >= self.nextpoll:
            self.nextpoll = now + self.args.poll
            for b in boards:
                self.hub.send(b, 'status', poll=True)

    # ---- the lines

    def shown(self, r):
        if r.reply and not self.freplies.value:
            return False
        a = self.address_of(r.board)
        if a in self.hidden:
            return False
        return self.filter(r)

    @staticmethod
    def address_of(name):
        ip, _, port = name.partition(':')
        return ip, int(port)

    def push(self, r):
        b = self.hub.boards.get(self.address_of(r.board))
        colour = PALETTE[b.colour % len(PALETTE)] if b is not None else '#000'
        self.view.push(format_record(r), style=f'color: {colour}' + ('; font-style: italic' if r.reply else ''))

    def refill(self):
        """the view again from the records kept, w/ the filter of now"""
        self.view.clear()
        recs = [r for r in self.hub.all() if self.shown(r)][-self.args.lines:]
        for r in recs:
            self.push(r)
        self.lastseq = self.hub.seq

    def on_tick(self):
        st = self.hub
        total = sum(b.received for b in st.board_list())
        self.rate.text = f'{len(st.board_list())} boards · {total} lines received'
        if self.paused:
            self.pausestate.text = f'paused: {st.seq - self.lastseq} new lines kept'
            return
        for r in st.since(self.lastseq):
            if self.shown(r):
                self.push(r)
        self.lastseq = st.seq
        if st.recpath:
            self.recstate.text = f'recording to {st.recpath}'

    def on_filter(self):
        try:
            self.filter = matcher(self.ftext.value or '', self.fregex.value, self.fcase.value)
            self.ferror.text = ''
        except re.error as e:
            self.ferror.text = f'regular expression: {e}'
            return
        self.refill()

    def on_pause(self):
        self.paused = not self.paused
        self.pausebtn.text = 'resume' if self.paused else 'pause'
        self.pausebtn.props(f'icon={"play_arrow" if self.paused else "pause"}')
        if not self.paused:
            self.pausestate.text = ''
            self.refill()

    def on_clear(self):
        self.hub.clear()
        self.refill()

    def on_record(self):
        if self.hub.recpath is None:
            path = (self.recpath.value or '').strip()
            if not path:
                return ui.notify('give a file name: .txt or .csv', type='warning')
            try:
                self.hub.record(path)
            except OSError as e:
                return ui.notify(str(e), type='negative')
            self.recbtn.text = 'stop'
            self.recbtn.props('icon=stop')
            self.recstate.text = f'recording to {path}'
        else:
            last = self.hub.recpath
            self.hub.record(None)
            self.recbtn.text = 'record'
            self.recbtn.props('icon=fiber_manual_record')
            self.recstate.text = f'saved {last}'


def run_page(args, hub):
    global ui
    try:
        from nicegui import ui as _ui
    except ImportError:
        sys.exit('[error] the page needs nicegui: pip install nicegui (or use --cli)')
    ui = _ui
    hub.start()
    if not args.no_hello:
        for b in hub.board_list():
            hub.send(b, 'hello')

    @ui.page('/')
    def index():
        LoggerPage(hub, args).build()

    ui.run(title='theLogger', port=args.gui_port, reload=False, show=not args.no_open, favicon='📜')


def main():
    ap = argparse.ArgumentParser(prog='logger.py', description='viewer of theLogger, the log over UDP of the embot boards')
    ap.add_argument('--ip', action='append', default=[], help='a board, ip or ip:port (default port 9999). repeat it for more boards')
    ap.add_argument('--boardport', type=int, default=DEFAULT_BOARDPORT, help='the port of theLogger on the boards (default 9999)')
    ap.add_argument('--localport', type=int, default=0, help='the local UDP port of the viewer (default: any). 9999 receives '
                                                             'also the boards that log to this PC:9999 w/out a hello')
    ap.add_argument('--no-hello', action='store_true', help='do not send hello at start: the boards keep their destination')
    ap.add_argument('--cli', action='store_true', help='text interface instead of the page')
    ap.add_argument('--poll', type=float, default=3.0, help='page: seconds between two silent status to each board (0 = never)')
    ap.add_argument('--lines', type=int, default=5000, help='page: lines in the view (default 5000)')
    ap.add_argument('--history', type=int, default=100000, help='lines kept in memory, for the filter (default 100000)')
    ap.add_argument('--record', default=None, help='record from the start to this file (.txt or .csv)')
    ap.add_argument('--gui-port', type=int, default=8090, help='page: local port of the web page (default 8090)')
    ap.add_argument('--no-open', action='store_true', help='page: do not open the browser')
    args = ap.parse_args()

    hub = Hub(args.localport, args.history)
    for text in args.ip:
        try:
            hub.add(parse_hostport(text, args.boardport))
        except (ValueError, OSError) as e:
            sys.exit(f'[error] --ip {text}: {e}')
    if args.record:
        hub.record(args.record)
    if args.cli:
        run_cli(args, hub)
    else:
        run_page(args, hub)
    hub.record(None)


if __name__ == '__main__':
    main()

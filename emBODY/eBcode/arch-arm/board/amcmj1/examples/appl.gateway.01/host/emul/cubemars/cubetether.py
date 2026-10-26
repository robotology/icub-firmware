"""
The link of the CubeMars panel to the amcmj1 board: the commands socket for the MIT, TETHER (ROP protocol 5, UDP, rpc socket 6666)
for the rest.

    cubemars_panel.py --backend tether  --UDP 5001, 6666-->  amcmj1 (tGATEWAY)  --CAN 29 bit-->  CubeMars (or cubemars_emulator.py)

The panel does not talk CAN here: it asks the board to do it. The MIT travels as the Speedgoat sends it: a command frame on the commands
socket (cubegateway.py), and the board sends it to the CAN as it arrives. The board does not repeat it: if no frame arrives for
cfg.watchdogms the board stops the motor ("no news = stop"). So this link keeps the last command alive: every 1/keepalive_hz seconds it
sends it again, with seq + 1, until disable() or another command. The first MIT after an enable is dropped by the board on purpose:
the keepalive sends the next one.

  MIT     command frame set<IDcubeMIT, {kp, kd, pos, vel, torque, seq}> on UDP 5001 (not by rpc: IDcubeMIT is read only)
  servo   run[+ack]<OPcubeSERVO, {packet, a, b, c}>                  packet 0 duty, 1 current, 2 brake, 3 velocity,
                                                                   4 position, 5 origin, 6 pos-vel (see amcmj1_cubedriver.h)
  board   run[+ack]<OPcubeINIT / DEINIT / START / STOP / ENABLE / DISABLE>

It needs tethercodec.py, which is in the folder above.
"""

import os
import socket
import struct
import sys
import threading
import time

_HERE = os.path.dirname(os.path.abspath(__file__))
_HOST = os.path.dirname(os.path.dirname(_HERE))        # host/: the folders below hold the modules shared by the tools
for _d in ("tether", "emul/common", "emul/cubemars", "emul/paxini", "emul/speedgoat"):
    sys.path.insert(0, os.path.join(_HOST, _d))
import tethercodec as rc                            # noqa: E402
import cubegateway as gw                            # noqa: E402

ERPM2RADS = 6.283185307 / 60.0 / (14.0 * 9.0)       # AK70-9: 14 pole pairs, reduction 9
SERVO = {"duty": 0, "current": 1, "brake": 2, "velocity": 3, "position": 4, "origin": 5, "posvel": 6}
STATES = {0: "off", 1: "initialised", 2: "running", 3: "error"}
COMMANDS = {0: "none", 1: "init", 2: "deinit", 3: "start", 4: "stop"}


def num(x):
    return f"{float(x):.7g}"


class BoardError(Exception):
    pass


class TetherLink:
    """same interface of the CAN Link of cubemars_panel.py (command(), status, lock, log, close()) + the buttons of the board"""

    is_board = True

    def __init__(self, ip="10.0.1.99", port=6666, timeout=0.3, tries=3, keepalive_hz=20.0, dictionary=None, browse=True,
                 poll_hz=5.0, cmdport=gw.PORT_CMD):
        self.peer = (ip, port)
        self.cmdsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)     # the commands socket of the board: the MIT goes here
        self.cmdsock.connect((ip, cmdport))
        self.timeout, self.tries = timeout, tries
        self.keepalive = 1.0 / keepalive_hz
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(("", 0))
        self.sock.connect(self.peer)
        self.io = threading.Lock()                  # one transaction at a time
        self.lock = threading.Lock()                # the data for the gui
        self.codec = rc.Codec(rc.Protocol())
        self.log = None
        self.running = True
        self.status = None                          # (t, (pos deg, speed ERPM, current A, temp, error)) as the CAN link
        self.status_count = 0
        self.pos32 = None
        self.start_frames = 0
        self.flag77 = 0
        self.tx_count = 0
        self.board_text = "board: not read yet"
        self.last_error = ""
        self.seq = int(time.time()) & 0x7FFFFFFF    # any start: the board counts only the gaps
        self.keep = None                            # ("mit", (kp, kd, pos, vel, torque)) or ("servo", (packet, a, b, c))
        self.keeplock = threading.Lock()
        self.sendlock = threading.RLock()           # command() and the keepalive do not interleave: a stale resend after a new command is impossible
        self.proto_info = None
        if dictionary and os.path.exists(dictionary) and not browse:
            self.codec = rc.Codec(rc.Protocol.load(dictionary))
        else:
            self._browse()
        self.threads = [threading.Thread(target=self._keepalive, daemon=True),
                        threading.Thread(target=self._poll, args=(1.0 / poll_hz,), daemon=True)]
        for t in self.threads:
            t.start()

    # ---- the board
    def _say(self, text):
        if self.log:
            self.log(text)

    def _transact(self, rops, tries=None):
        frame = self.codec.build_frame(rops)
        with self.io:
            self.sock.setblocking(False)
            try:
                while True:
                    self.sock.recv(65535)           # late replies
            except (BlockingIOError, OSError):
                pass
            self.sock.setblocking(True)
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
                    except (socket.timeout, OSError):
                        break
                    try:
                        return self.codec.decode_frame(data)
                    except rc.ProtocolError:
                        continue
        return None

    def _browse(self):
        def transact(rops):
            return self._transact(rops)
        info = rc.browse_info(transact)
        if info is None:
            raise ConnectionError(f"the board does not answer at {self.peer[0]}:{self.peer[1]}")
        self.proto_info = info
        proto = rc.browse(transact, info)
        self.codec = rc.Codec(proto)
        self.codec.set_maxrops(info.maxrops)
        for need in ("IDcubeMIT", "OPcubeSERVO", "OPcubeINIT"):
            if proto.entry(self.codec.resolve_id(need)[0]) is None:
                raise ConnectionError(f"{need} is not in the Dictionary of the board: it is an old firmware")

    def run(self, text, ack=True):
        """run<OPname, value>: returns the text of the ack. BoardError if nak or no reply"""
        rops = self.codec.parse_line(("run[+ack]<" if ack else "run<") + text + ">")
        if not ack:
            with self.io:
                self.sock.send(self.codec.build_frame(rops))
            return ""
        frame = self._transact(rops, tries=1)           # a run is not repeated
        if frame is None:
            raise BoardError("no reply from the board")
        for r in frame.rops:
            if r.id == rops[0].id and r.cmd == rc.CMD.nak:
                raise BoardError(rc.format_text(r.data) or "nak")
            if r.id == rops[0].id and r.cmd == rc.CMD.ack:
                return self.codec.format_rop(r)
        raise BoardError("the reply does not contain the ack")

    def put(self, name, value_text):
        """set<IDname, value>, no ack"""
        rops = self.codec.parse_line(f"set<{name}, {value_text}>")
        with self.io:
            self.sock.send(self.codec.build_frame(rops))
        self.tx_count += 1

    def ask(self, name):
        """the value of a variable as a dict {field: value}"""
        vid = self.codec.resolve_id(name)[0]
        frame = self._transact([rc.make_rop(rc.CMD.ask, vid)])
        if frame is None:
            raise BoardError("no reply from the board")
        for r in frame.rops:
            if r.id == vid and r.cmd == rc.CMD.say:
                entry = self.codec.protocol.entry(vid)
                return {k.lstrip("."): float(v) for k, v in entry.spec.flatten("", r.data)}
        raise BoardError(f"no say<{name}>")

    # ---- the buttons of the board
    def init(self, canid=0x68, repeatms=10, watchdogms=100):
        return self.run(f"OPcubeINIT, {{canid={canid}, repeatms={repeatms}, watchdogms={watchdogms}}}")

    def start(self):
        return self.run("OPcubeSTART")

    def stop(self):
        with self.keeplock:
            self.keep = None
        return self.run("OPcubeSTOP")

    def enable(self):
        """the gate opens: init if needed, then start. the board drops the first MIT, the keepalive sends the next one"""
        return self.run("OPcubeENABLE")

    def disable(self):
        with self.keeplock:
            self.keep = None
        return self.run("OPcubeDISABLE")

    def deinit(self):
        with self.keeplock:
            self.keep = None
        return self.run("OPcubeDEINIT")

    # ---- the commands of the panel: command(enc, args) as in the CAN link. enc is one of the enc_*() of the panel
    def command(self, enc, args):
        enc(*args)                                      # the same checks of the panel: ValueError if out of range
        with self.sendlock:
            self._command(enc, args)

    def _command(self, enc, args):
        name = enc.__name__
        if name == "enc_mit":
            with self.keeplock:
                self.keep = ("mit", tuple(float(a) for a in args))
            self._send_mit(*args)
        elif name == "enc_disable":
            self.stop()
        elif name in self.SERVOARGS:
            kind, mapper = self.SERVOARGS[name]
            with self.keeplock:
                self.keep = ("servo", (SERVO[kind],) + mapper(*args))
            self._send_servo(SERVO[kind], *mapper(*args))
        else:
            raise ValueError(f"{name} is not available through the board")

    SERVOARGS = {
        "enc_duty": ("duty", lambda d: (d, 0, 0)),
        "enc_current": ("current", lambda a: (a, 0, 0)),
        "enc_brake": ("brake", lambda a: (a, 0, 0)),
        "enc_rpm": ("velocity", lambda e: (e, 0, 0)),
        "enc_pos": ("position", lambda d: (d, 0, 0)),
        "enc_origin": ("origin", lambda p: (1.0 if p else 0.0, 0, 0)),
        "enc_posspd": ("posvel", lambda d, e, a: (d, e, a)),
    }

    def _send_mit(self, kp, kd, pos, vel, torque):
        self.seq = (self.seq + 1) & 0xFFFFFFFF
        self.cmdsock.send(gw.mit_frame(kp, kd, pos, vel, torque, self.seq))
        self.tx_count += 1
        self._say(f"tx command frame set<IDcubeMIT> seq={self.seq}")

    def _send_servo(self, packet, a, b, c):
        try:
            self.run(f"OPcubeSERVO, {{packet={packet}, a={num(a)}, b={num(b)}, c={num(c)}}}", ack=True)
        except (BoardError, rc.ProtocolError) as e:
            self.last_error = f"OPcubeSERVO: {e}"
            with self.keeplock:
                self.keep = None                        # e.g. the motor is not running: do not insist
            raise ValueError(self.last_error)
        self.last_error = ""
        if packet == SERVO["origin"]:
            with self.keeplock:
                self.keep = None                        # a one shot
        self._say(f"tx run OPcubeSERVO packet={packet}")

    def _keepalive(self):
        while self.running:
            time.sleep(self.keepalive)
            with self.sendlock:
                with self.keeplock:
                    k = self.keep
                if k is None:
                    continue
                try:
                    if k[0] == "mit":
                        self._send_mit(*k[1])
                    else:
                        self._send_servo(*k[1])
                except (ValueError, OSError, BoardError) as e:
                    self.last_error = str(e)

    def _poll(self, period):
        while self.running:
            time.sleep(period)
            try:
                st = self.ask("IDcubeSTATE")
                bd = self.ask("IDcubeSTATUS")
            except (BoardError, rc.ProtocolError, OSError, KeyError) as e:
                self.board_text = f"board: no answer ({e})"
                continue
            deg = st["pos"] * 180.0 / 3.14159265359
            erpm = st["vel"] / ERPM2RADS
            with self.lock:
                if st["counter"] != getattr(self, "_lastcounter", None):
                    self.status = (time.time(), (deg, int(round(erpm)), st["current"], int(st["temp"]), int(st["error"])))
                    self.status_count = int(st["counter"])
                self._lastcounter = st["counter"]
                self.board_text = (f"board: {STATES.get(int(bd['state']), bd['state'])}, last command "
                                   f"{COMMANDS.get(int(bd['lastcommand']), bd['lastcommand'])}, commands {int(bd['commands'])}, "
                                   f"errors {int(bd['errors'])}, watchdog stops {int(bd['watchdogs'])}"
                                   + (f"   [{self.last_error}]" if self.last_error else ""))

    def close(self):
        self.running = False
        with self.keeplock:
            self.keep = None
        for t in self.threads:
            t.join(0.5)
        self.sock.close()
        self.cmdsock.close()

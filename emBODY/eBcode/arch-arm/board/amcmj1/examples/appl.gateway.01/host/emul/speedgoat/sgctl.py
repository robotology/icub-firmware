"""
The control link between the panel (speedgoat_panel.py) and whoever plays the Speedgoat: speedgoat_emulator.py --daemon on the PC, or the
Simulink model on the real Speedgoat. Two small binary UDP packets, little endian, the same bytes in Python and in MATLAB
(speedgoat/matlab/amcmj1_ctl_decode.m, amcmj1_status_frame.m: tests/golden.txt keeps them equal).

  panel  --CTL 36 bytes, UDP port 6000 (to the Speedgoat)-->   daemon / Simulink       sent every 200 ms (keepalive) and at every click
  panel  <--STATUS 60 bytes, UDP port 6001 (to the panel)--    daemon / Simulink       every 100 ms; the panel binds 6001, the daemon answers to
                                                                                       the source address of the CTL packets

CTL     <4s B B B B I 6f>      magic 'SGC1', run (0 stop, 1 run), mode, dev (1 cube, 2 pax, 3 both), flags (0), cnt (counts the sends),
                               value, kp, kd, freq, tparam, duration [s] (0 = until stop)
STATUS  <4s B B B B B 3x I f 6f 4I>   magic 'SGS1', state (0 idle, 1 starting, 2 running, 3 stopping), mode, dev, cube state (0 off, 1 initialised,
                               2 running, 3 error), err, cnt (echo of the CTL), t [s] since the start, cube pos vel current, pax Mx My Mz,
                               sent, received, lost, clamped

mode: 0 zero (motor free), 1 velocity [rad/s], 2 step (the sign changes every tparam s), 3 sine (freq Hz), 4 position [rad],
      5 torque [N*m], 6 ramp (velocity from 0 to value in tparam s, then it holds), 7 idle (heartbeats only: no setpoint)
err:  0 ok, 1 enable of the CubeMars failed, 2 enable of the Paxini failed, 3 the panel stopped talking (keepalive lost), 4 the board does not answer
A CTL with run 1 starts the devices (enable on the rpc socket) and the profile; a CTL with run 0, a duration that expires or a lost keepalive
stops it (disable). The values of the profile can change while it runs (not dev). The watchdog of the board does the rest.
"""

import socket
import struct
import threading
import time
from collections import namedtuple

CTL_FMT = "<4sBBBBI6f"
STATUS_FMT = "<4sBBBBB3xIf6f4I"
CTL_MAGIC, STATUS_MAGIC = b"SGC1", b"SGS1"
CTL_SIZE, STATUS_SIZE = struct.calcsize(CTL_FMT), struct.calcsize(STATUS_FMT)      # 36, 60
PORT_CTL, PORT_STATUS = 6000, 6001

MODES = {"zero": 0, "velocity": 1, "step": 2, "sine": 3, "position": 4, "torque": 5, "ramp": 6, "idle": 7}
MODE_NAMES = {v: k for k, v in MODES.items()}
DEVS = {"cube": 1, "pax": 2, "both": 3}
DEV_NAMES = {v: k for k, v in DEVS.items()}
STATE_NAMES = {0: "idle", 1: "starting", 2: "running", 3: "stopping"}
ERR_NAMES = {0: "", 1: "enable of the CubeMars failed", 2: "enable of the Paxini failed", 3: "keepalive of the panel lost", 4: "the board does not answer"}
CUBE_NAMES = {0: "off", 1: "initialised", 2: "running", 3: "error"}

Ctl = namedtuple("Ctl", "run mode dev cnt value kp kd freq tparam duration")
Status = namedtuple("Status", "state mode dev cube err cnt t pos vel current mx my mz sent received lost clamped")


def pack_ctl(run, mode, dev, cnt, value=0.0, kp=0.0, kd=0.0, freq=0.0, tparam=0.0, duration=0.0):
    return struct.pack(CTL_FMT, CTL_MAGIC, int(run), int(mode), int(dev), 0, cnt & 0xFFFFFFFF, value, kp, kd, freq, tparam, duration)


def unpack_ctl(data):
    """-> Ctl, or None if it is not a CTL packet"""
    if len(data) != CTL_SIZE:
        return None
    magic, run, mode, dev, _flags, cnt, value, kp, kd, freq, tparam, duration = struct.unpack(CTL_FMT, data)
    if magic != CTL_MAGIC or run > 1 or mode not in MODE_NAMES or dev not in DEV_NAMES:
        return None
    return Ctl(run, mode, dev, cnt, value, kp, kd, freq, tparam, duration)


def pack_status(state, mode, dev, cube, err, cnt, t, pos=0.0, vel=0.0, current=0.0, mx=0.0, my=0.0, mz=0.0, sent=0, received=0, lost=0, clamped=0):
    return struct.pack(STATUS_FMT, STATUS_MAGIC, int(state), int(mode), int(dev), int(cube), int(err), cnt & 0xFFFFFFFF, t,
                       pos, vel, current, mx, my, mz, sent & 0xFFFFFFFF, received & 0xFFFFFFFF, lost & 0xFFFFFFFF, clamped & 0xFFFFFFFF)


def unpack_status(data):
    """-> Status, or None if it is not a STATUS packet"""
    if len(data) != STATUS_SIZE:
        return None
    v = struct.unpack(STATUS_FMT, data)
    return Status(*v[1:]) if v[0] == STATUS_MAGIC else None


class Client:
    """the panel side of the link: a thread that sends the CTL (keepalive 200 ms) and one that receives the STATUS.
    set(...) changes what is sent from now on; status() is the last STATUS received and its age"""

    def __init__(self, host="127.0.0.1", ctl_port=PORT_CTL, status_port=PORT_STATUS, period=0.2):
        self.period, self.lock = period, threading.Lock()
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            self.sock.bind(("", status_port))      # no SO_REUSEADDR: on Windows a second panel would share the port and steal the STATUS packets
        except OSError as e:
            self.sock.close()
            raise OSError(f"UDP port {status_port} is already in use: is another panel still open? ({e})")
        self.sock.settimeout(0.1)
        self.dest = (host, ctl_port)
        self.cnt, self.running = 0, True
        self.params = dict(run=0, mode=0, dev=3, value=0.0, kp=0.0, kd=0.0, freq=0.0, tparam=0.0, duration=0.0)
        self.last, self.last_at = None, None
        self.wakeup = threading.Event()
        self.threads = [threading.Thread(target=self._tx, daemon=True), threading.Thread(target=self._rx, daemon=True)]
        for th in self.threads:
            th.start()

    def set(self, **kw):
        with self.lock:
            self.params.update(kw)
        self.wakeup.set()                       # sent now, not at the next keepalive

    def set_host(self, host, ctl_port=PORT_CTL):
        with self.lock:
            self.dest = (host, ctl_port)
            self.last, self.last_at = None, None

    def status(self):
        """(Status or None, age in seconds or None)"""
        with self.lock:
            return self.last, (None if self.last_at is None else time.monotonic() - self.last_at)

    def _tx(self):
        while self.running:
            with self.lock:
                self.cnt += 1
                pkt = pack_ctl(cnt=self.cnt, **self.params)
                dest = self.dest
            try:
                self.sock.sendto(pkt, dest)
            except OSError:
                pass
            self.wakeup.wait(self.period)
            self.wakeup.clear()

    def _rx(self):
        while self.running:
            try:
                data, _ = self.sock.recvfrom(256)
            except socket.timeout:
                continue
            except ConnectionResetError:
                continue                        # Windows: the other side is not there (ICMP of a previous send): not an error, it comes back
            except OSError:
                return
            st = unpack_status(data)
            if st is not None:
                with self.lock:
                    self.last, self.last_at = st, time.monotonic()

    def close(self, stop=True):
        """by default it tells the other side to stop first"""
        if stop:
            self.set(run=0)
            time.sleep(0.05)
            for _ in range(3):
                self.set(run=0)
                time.sleep(0.02)
        self.running = False
        self.wakeup.set()
        self.sock.close()

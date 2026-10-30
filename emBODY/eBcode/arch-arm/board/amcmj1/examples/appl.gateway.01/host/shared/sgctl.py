"""
The link between the panel (sgact_panel.py) and whoever plays sgact: the Simulink model on the Speedgoat, or
sgact_daemon.py on the PC. Version 4: the panel sends requests (init, deinit, start, stop), sgact
answers with the state of its supervisor. What the requests can contain (devices, modes, parameters) is in
sgact_panel.toml (sgact_spec.py): its CRC travels in every CTL, sgact refuses the requests of a different file.

  panel  --CTL 100 bytes, UDP port 6000 (to sgact)-->  sgact        every 200 ms (keepalive) and at every click
  panel  <--STATUS 104 bytes, UDP port 6001--------    sgact        every 100 ms

Two binary packets, little endian, the same bytes in Python and in MATLAB (host/speedgoat/simulink/matlab/sgact_ctl_frame.m,
sgact_status_frame.m; host/speedgoat/simulink/tests/golden.txt keeps them equal):

CTL     <4s B B B B I I I f 8f 9f B B B B h h>   'SGC3', action (0 none, 1 init, 2 deinit, 3 start, 4 stop), mode,
                                  devices (mask), flags (bit 0: EXTFAULT pressed), cnt (counts the sends), reqid (+1 at every request: sgact
                                  acts when it changes), crc (of sgact_panel.toml), duration [s] (0 = until stop), params[8],
                                  the config of the motor_macro (sgact_spec.MMCFG_NAMES): current pid kp ki kd, velocity pid
                                  kp ki kd, current limits nominal peak overload [A] (9f), flags poles entlr (3B) 0 (B),
                                  rotencres rotindexoff (2h). sgact takes it at the init of the motor_macro
STATUS  <4s 8B I I I f 9f B B B B 4I f I f I B 3x>  'SGS4', state, mode (255 none), inited (mask), err, result, reason, action,
                                  motor_cubemars_status, reqid, cnt (echo), crc (of sgact), trun [s],
                                  motor_cubemars pos vel current, sensor_torque mx my mz, motor_macro pos vel current,
                                  motor_macro_mode (control mode of amc.mot), motor_macro_configured (bit0 pids, bit1 setup),
                                  motor_macro_emulated, gw_extfault (amc.gtw holds the EXTFAULT), sent, received, lost, clamped, motor_macro_pwm [%], motor_macro_faults,
                                  sensor_position pos, counter, flags (bit 0 valid, bit 1 emulated)
                                  (the angles in rad and rad/s). The CTL did not change from version 3: it keeps 'SGC3'
The names of the states, results, reasons and errors are below (and in host/speedgoat/simulink/matlab/sgact_const.m).
"""

import socket
import struct
import threading
import time
from collections import namedtuple

CTL_FMT = "<4sBBBBIIIf8f9fBBBBhh"
STATUS_FMT = "<4sBBBBBBBBIIIf9fBBBB4IfIfIB3x"
CTL_MAGIC, STATUS_MAGIC = b"SGC3", b"SGS4"
CTL_SIZE, STATUS_SIZE = struct.calcsize(CTL_FMT), struct.calcsize(STATUS_FMT)      # 100, 104
assert (CTL_SIZE, STATUS_SIZE) == (100, 104)
MMCFG_SIZE = 14                         # the numbers of the config of the motor_macro in the CTL
PORT_CTL, PORT_STATUS = 6000, 6001
MAX_PARAMS = 8
MODE_NONE = 255

STATE_NAMES = {0: "idle", 1: "initialising", 2: "deinitialising", 3: "running", 4: "stopping", 5: "EXTFAULT"}
CTL_EXTFAULT = 0x01                     # bit of the flags of the CTL
RESULT_NAMES = {0: "", 1: "accepted", 2: "rejected", 3: "done", 4: "failed"}
REASON_NAMES = {0: "", 1: "wrong state", 2: "devices not initialised", 3: "parameter out of range", 4: "unknown mode",
                5: "different sgact_panel.toml", 6: "unknown action"}
ERR_NAMES = {0: "", 1: "init of the motor_cubemars failed", 2: "init of the sensor_torque failed", 3: "keepalive of the panel lost",
             4: "amc.gtw does not answer", 5: "motor_macro: init failed or control mode not reached",
             6: "motor_macro: fault (external fault pressed, or hwfault)", 7: "init of the sensor_position failed"}
CONTROLMODE_NAMES = {0x00: "idle", 0x06: "current", 0x0A: "speedvoltage", 0x50: "openloop", 0xA0: "hwfault"}
FAULT_EXTERNAL = 1 << 7
CUBE_NAMES = {0: "off", 1: "initialised", 2: "running", 3: "error"}

Ctl = namedtuple("Ctl", "action mode devices cnt reqid crc duration params mmcfg extfault")
Status = namedtuple("Status", "state mode inited err result reason action motor_cubemars_status reqid cnt crc trun "
                              "motor_cubemars_pos motor_cubemars_vel motor_cubemars_current sensor_torque_mx sensor_torque_my sensor_torque_mz "
                              "motor_macro_pos motor_macro_vel motor_macro_current motor_macro_mode motor_macro_configured "
                              "motor_macro_emulated gw_extfault sent received lost clamped motor_macro_pwm motor_macro_faults "
                              "sensor_position_pos sensor_position_counter sensor_position_flags")


def _params8(params):
    p = [float(x) for x in params][:MAX_PARAMS]
    return p + [0.0] * (MAX_PARAMS - len(p))


def _mmcfg(mmcfg):
    v = [float(x) for x in mmcfg][:MMCFG_SIZE]
    v += [0.0] * (MMCFG_SIZE - len(v))
    return v[:9] + [int(v[9]) & 0xFF, int(v[10]) & 0xFF, int(v[11]) & 0xFF, 0,
                    max(-32768, min(32767, int(v[12]))), max(-32768, min(32767, int(v[13])))]


def pack_ctl(action, mode, devices, cnt, reqid, crc, duration=0.0, params=(), mmcfg=(), extfault=False):
    return struct.pack(CTL_FMT, CTL_MAGIC, int(action), int(mode), int(devices), CTL_EXTFAULT if extfault else 0, cnt & 0xFFFFFFFF, reqid & 0xFFFFFFFF,
                       crc & 0xFFFFFFFF, float(duration), *_params8(params), *_mmcfg(mmcfg))


def unpack_ctl(data):
    """-> Ctl, or None if it is not a CTL v3 packet"""
    if len(data) != CTL_SIZE:
        return None
    v = struct.unpack(CTL_FMT, data)
    if v[0] != CTL_MAGIC or v[1] > 4:
        return None
    mm = list(v[17:26]) + [float(v[26]), float(v[27]), float(v[28]), float(v[30]), float(v[31])]
    return Ctl(v[1], v[2], v[3], v[5], v[6], v[7], v[8], list(v[9:17]), mm, bool(v[4] & CTL_EXTFAULT))


def pack_status(state, mode, inited, err, result, reason, action, motor_cubemars_status, reqid, cnt, crc, trun,
                motor_cubemars_pos=0.0, motor_cubemars_vel=0.0, motor_cubemars_current=0.0, sensor_torque_mx=0.0, sensor_torque_my=0.0, sensor_torque_mz=0.0,
                motor_macro_pos=0.0, motor_macro_vel=0.0, motor_macro_current=0.0, motor_macro_mode=0, motor_macro_configured=0,
                motor_macro_emulated=0, sent=0, received=0, lost=0, clamped=0, motor_macro_pwm=0.0, motor_macro_faults=0,
                sensor_position_pos=0.0, sensor_position_counter=0, sensor_position_flags=0, gw_extfault=0):
    return struct.pack(STATUS_FMT, STATUS_MAGIC, int(state), int(mode), int(inited), int(err), int(result), int(reason),
                       int(action), int(motor_cubemars_status), reqid & 0xFFFFFFFF, cnt & 0xFFFFFFFF, crc & 0xFFFFFFFF, float(trun),
                       motor_cubemars_pos, motor_cubemars_vel, motor_cubemars_current, sensor_torque_mx, sensor_torque_my, sensor_torque_mz,
                       motor_macro_pos, motor_macro_vel, motor_macro_current, int(motor_macro_mode) & 0xFF,
                       int(motor_macro_configured) & 0xFF, int(motor_macro_emulated) & 0xFF, int(gw_extfault) & 0xFF,
                       sent & 0xFFFFFFFF, received & 0xFFFFFFFF, lost & 0xFFFFFFFF, clamped & 0xFFFFFFFF,
                       float(motor_macro_pwm), int(motor_macro_faults) & 0xFFFFFFFF,
                       float(sensor_position_pos), int(sensor_position_counter) & 0xFFFFFFFF, int(sensor_position_flags) & 0xFF)


def unpack_status(data):
    """-> Status, or None if it is not a STATUS v4 packet"""
    if len(data) != STATUS_SIZE:
        return None
    v = struct.unpack(STATUS_FMT, data)
    return Status(*v[1:]) if v[0] == STATUS_MAGIC else None


def _ip_of(host):
    """the IPv4 of host (a name or an address), or host itself if it cannot be resolved"""
    try:
        return socket.gethostbyname(host)
    except OSError:
        return host


class Client:
    """the panel side of the link: a thread that sends the CTL (keepalive 200 ms, and at once after a change) and one
    that receives the STATUS. request(action, ...) makes a new request (reqid + 1); live(...) changes the mode, the
    parameters and the duration that go with the next packets w/out a new request (sgact applies them to the running
    mode if it is the same); status() is the last STATUS and its age"""

    def __init__(self, crc, host="127.0.0.1", ctl_port=PORT_CTL, status_port=PORT_STATUS, period=0.2):
        self.crc, self.period, self.lock = crc, period, threading.Lock()
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            self.sock.bind(("", status_port))      # no SO_REUSEADDR: on Windows a second panel would share the port and steal the STATUS
        except OSError as e:
            self.sock.close()
            raise OSError(f"UDP port {status_port} is already in use: is another panel still open? ({e})")
        self.sock.settimeout(0.1)
        self.dest = (host, ctl_port)
        self.peer = _ip_of(host)                # a STATUS is accepted only from here
        self.other, self.other_at = None, None
        self.cnt, self.reqid, self.running = 0, int(time.time()) & 0xFFFF, True     # a new panel starts from a new reqid
        self.fields = dict(action=0, mode=0, devices=0, duration=0.0, params=[0.0] * MAX_PARAMS, mmcfg=[0.0] * MMCFG_SIZE,
                           extfault=False)
        self.last, self.last_at = None, None
        self.wakeup = threading.Event()
        self.threads = [threading.Thread(target=self._tx, daemon=True), threading.Thread(target=self._rx, daemon=True)]
        for th in self.threads:
            th.start()

    def request(self, action, **kw):
        """a new request: action and the fields that go with it (mode, devices, duration, params). returns its id"""
        with self.lock:
            self.fields.update(kw)
            self.fields["action"] = action
            self.reqid = (self.reqid + 1) & 0xFFFFFFFF
            rid = self.reqid
        self.wakeup.set()
        return rid

    def live(self, **kw):
        with self.lock:
            self.fields.update(kw)
        self.wakeup.set()

    def set_host(self, host, ctl_port=PORT_CTL):
        with self.lock:
            self.dest = (host, ctl_port)
            self.peer = _ip_of(host)
            self.last, self.last_at = None, None
            self.other, self.other_at = None, None

    def foreign(self):
        """(IP, age in seconds) of the last STATUS that came from another IP and was ignored, or (None, None)"""
        with self.lock:
            return self.other, (None if self.other_at is None else time.monotonic() - self.other_at)

    def status(self):
        """(Status or None, age in seconds or None)"""
        with self.lock:
            return self.last, (None if self.last_at is None else time.monotonic() - self.last_at)

    def _tx(self):
        while self.running:
            with self.lock:
                self.cnt += 1
                f = self.fields
                pkt = pack_ctl(f["action"], f["mode"], f["devices"], self.cnt, self.reqid, self.crc, f["duration"], f["params"],
                               f["mmcfg"], f["extfault"])
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
                data, src = self.sock.recvfrom(256)
            except socket.timeout:
                continue
            except ConnectionResetError:
                continue                        # Windows: the other side is not there (ICMP of a previous send): it comes back
            except OSError:
                return
            st = unpack_status(data)
            if st is not None:
                with self.lock:
                    if src[0] == self.peer:     # only the sgact that gets our CTL: the Speedgoat sends the STATUS to a fixed IP
                        self.last, self.last_at = st, time.monotonic()
                    else:
                        self.other, self.other_at = src[0], time.monotonic()

    def close(self):
        self.running = False
        self.wakeup.set()
        for th in self.threads:
            th.join(0.5)                        # the port is free when close() returns: a new client can bind it
        self.sock.close()

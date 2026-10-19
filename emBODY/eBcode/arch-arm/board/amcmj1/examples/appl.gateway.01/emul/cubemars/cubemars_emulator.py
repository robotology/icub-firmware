#!/usr/bin/env python3
"""
CubeMars AK70-9 (KV60) actuator emulator, for python-can. Servo mode and MIT (force control) mode.

Source: "AK series product manual v3.2.0", sec. 4.1 (servo), 4.2 (MIT), 4.3.1 (feedback), 4.4.1 (examples).

Bus: classic CAN, 1 Mbps, EXTENDED frames (29 bit), DLC 8.
  ID29 = (packet_id << 8) | driver_id           driver_id: bits [7..0], packet_id: bits [28..8]
  Standard (11-bit) frames, e.g. the ones of the PX3Q, are ignored: the two can share the bus.

RX, commands (integers are big endian)
  packet_id  mode                payload
  0          duty                int32  duty * 100000
  1          current loop        int32  mA (A * 1000), +-60000
  2          current brake       int32  mA (A * 1000), 0..60000
  3          velocity            int32  ERPM (electrical), +-100000
  4          position            int32  deg * 10000 (+-36000 deg)
  5          set origin          uint8  0 = temporary, 1 = permanent
  6          position-velocity   int32 pos (deg * 10000), int16 speed (ERPM / 10), int16 accel (ERPM/s^2 / 10)
  8          MIT                 Kp (12 bit, 0..500), Kd (12 bit, 0..5), pos (16 bit, +-12.56 rad),
                                 vel (12 bit, +-30 rad/s), torque (12 bit, +-32 N.m); bytes as in the manual (pack_cmd)
  15         motor disable       none. The motor answers once with a 0x29 frame with data[7] = 0x77
  16         feedback config     data[6..7] = flags: bit 15 = add the 0x2A frame (int32 position), bit 14 = single turn

TX, feedback
  packet_id 0x2C   start frame: FA FB FC FD, when the motor enters servo mode (power up, and MIT -> servo)
  packet_id 0x29   status: pos int16 (0.1 deg), speed int16 (10 ERPM), current int16 (0.01 A), temp int8, error uint8
  packet_id 0x2A   optional: pos int32 (0.0001 deg) in data[0..3]
  Timed (default, --feedback-hz 100, 1..2000 Hz) or query-reply (--query-reply: one answer to each control command).

NOT in the manual, an assumption of this emulator: the manual does not say what the motor sends back in MIT mode over CAN.
Here the same 0x29 (and 0x2A) status frames as in the servo mode are sent, in both modes.

The dynamics is a simple model, not the motor: a rigid inertia on the output shaft (J, viscous friction b), the torque is
limited by Tmax (32 N.m = 30 A * Kt 1.0621 N.m/A) and the speed by Vmax (30 rad/s). The servo loops are simple cascades of the
position (P) and velocity (P) loops; the MIT mode is the impedance law  tau = Kp (p_des - p) + Kd (v_des - v) + t_ff.
Duty cycle has no electrical model: duty * Vmax is used as velocity target.

State publishing (for cubemars_viewer.py, or any other program): the state is sent as a JSON datagram over UDP, by default to
127.0.0.1:5005 at 30 Hz. --publish HOST:PORT (repeatable), --publish-hz N (0 = off). It is independent of the CAN feedback.

Printing. By default nothing is printed but the start banner:
  -v / --verbose        the commands received (a line each)
  --trace-feedback N    one line every N feedback frames. 0 = never (default)
  --sniff               every frame on the bus, ours or not

Examples
  python cubemars_emulator.py --interface vector --channel 0 --app-name CANalyzer --id 0x68
  python cubemars_emulator.py --interface virtual --channel test -v
"""

import argparse
import json
import math
import socket
import struct
import sys
import threading
import time

import can

# packet ids of the commands
P_DUTY, P_CURRENT, P_BRAKE, P_RPM, P_POS, P_ORIGIN, P_POSSPD, P_MIT, P_DISABLE, P_FRAMECFG = 0, 1, 2, 3, 4, 5, 6, 8, 15, 16
# packet ids of the feedback
F_START, F_STATUS, F_POS32 = 0x2C, 0x29, 0x2A
PNAMES = {P_DUTY: "duty", P_CURRENT: "current", P_BRAKE: "currentBrake", P_RPM: "velocity", P_POS: "position",
          P_ORIGIN: "setOrigin", P_POSSPD: "posVelocity", P_MIT: "MIT", P_DISABLE: "disable", P_FRAMECFG: "feedbackConfig"}

DEFAULT_ID = 0x68                               # the id used in the examples of the manual (sec. 4.4.1)

# AK70-9 KV60 (manual, sec. 4.2 table, 3.1.5.2 poles)
POLE_PAIRS = 14
GEAR = 9.0
KT = 1.0621                                     # N.m/A at the output
TMAX = 32.0                                     # N.m
VMAX = 30.0                                     # rad/s
PMAX = 12.56                                    # rad, range of the MIT position
KP_MAX, KD_MAX = 500.0, 5.0
IMAX = TMAX / KT                                # ~30 A

DEG2RAD = math.pi / 180.0
ERPM2RADS = 2.0 * math.pi / 60.0 / (POLE_PAIRS * GEAR)      # electrical rpm -> output rad/s


def uint_to_float(x, x_min, x_max, bits):
    return x * (x_max - x_min) / float((1 << bits) - 1) + x_min


def float_to_uint(x, x_min, x_max, bits):
    x = min(max(x, x_min), x_max)
    return int((x - x_min) * float((1 << bits) - 1) / (x_max - x_min))


def clamp(x, lo, hi):
    return lo if x < lo else hi if x > hi else x


def pack_mit(kp, kd, pos, vel, torque):
    """The 8 bytes of the MIT command, as pack_cmd() of the manual (but a correct clamp, see the notes)."""
    p = float_to_uint(pos, -PMAX, PMAX, 16)
    v = float_to_uint(vel, -VMAX, VMAX, 12)
    k = float_to_uint(kp, 0.0, KP_MAX, 12)
    d = float_to_uint(kd, 0.0, KD_MAX, 12)
    t = float_to_uint(torque, -TMAX, TMAX, 12)
    return bytes([k >> 4, ((k & 0xF) << 4) | (d >> 8), d & 0xFF, p >> 8, p & 0xFF, v >> 4, ((v & 0xF) << 4) | (t >> 8), t & 0xFF])


def unpack_mit(b):
    k = (b[0] << 4) | (b[1] >> 4)
    d = ((b[1] & 0xF) << 8) | b[2]
    p = (b[3] << 8) | b[4]
    v = (b[5] << 4) | (b[6] >> 4)
    t = ((b[6] & 0xF) << 8) | b[7]
    return (uint_to_float(k, 0.0, KP_MAX, 12), uint_to_float(d, 0.0, KD_MAX, 12), uint_to_float(p, -PMAX, PMAX, 16),
            uint_to_float(v, -VMAX, VMAX, 12), uint_to_float(t, -TMAX, TMAX, 12))


class Mode:
    DISABLED, DUTY, CURRENT, BRAKE, RPM, POS, POSSPD, MIT = range(8)
    NAMES = ("disabled", "duty", "current", "currentBrake", "velocity", "position", "posVelocity", "MIT")


DEFAULT_PUBLISH = ("127.0.0.1", 5005)


class AK70:
    SUBSTEPS = 10                               # physics steps per 1 ms tick, so that Kd = 5 stays stable

    def __init__(self, bus, devid=DEFAULT_ID, feedback_hz=100, query_reply=False, inertia=0.005, friction=0.01,
                 error=0, verbose=False, trace_feedback=0, sniff=False, startframe=True, publish=(), publish_hz=0):
        if not 0 <= devid <= 0xFF:
            raise ValueError(f"driver id must be in [0, 255], not {devid}")
        if not 1 <= feedback_hz <= 2000:
            raise ValueError(f"feedback rate must be in [1, 2000] Hz, not {feedback_hz}")
        self.bus = bus
        self.devid = devid
        self.feedback_hz = feedback_hz
        self.query_reply = query_reply
        self.J, self.b = inertia, friction
        self.error = error
        self.verbose = verbose
        self.trace_feedback = max(0, int(trace_feedback))
        self.sniff = sniff
        self.startframe = startframe
        self.publish = [tuple(p) for p in publish]        # (host, port) destinations of the state datagrams
        self.publish_hz = publish_hz                      # 0 = no publishing
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) if (self.publish and publish_hz > 0) else None
        self.seq = 0
        self.t0 = time.time()
        self.lock = threading.Lock()
        self.running = False
        # state (output shaft)
        self.theta = 0.0                        # rad
        self.omega = 0.0                        # rad/s
        self.torque = 0.0                       # N.m, the applied one (limited)
        self.origin = 0.0                       # rad, the position reported is theta - origin
        self.origin_permanent = False
        self.mode = Mode.DISABLED
        self.target = 0.0                       # rad (position modes), rad/s (velocity), A (current), duty
        self.hold = 0.0                         # rad, current brake
        self.wlim, self.alim = VMAX, 1000.0     # rad/s, rad/s^2 of the position-velocity mode
        self.ilim = IMAX                        # A, current brake
        self.wref = 0.0                         # velocity reference of the profile
        self.mit = (0.0, 0.0, 0.0, 0.0, 0.0)    # kp, kd, pos, vel, torque
        self.send_pos32 = False
        self.single_turn = False
        self.tx = 0                             # status frames sent
        self.temp = 25.0
        self.cmds = 0
        self.last_cmd = None

    # ---- helpers
    def pos(self):
        return self.theta - self.origin

    def _id(self, packet):
        return (packet << 8) | self.devid

    def _send(self, packet, data):
        try:
            self.bus.send(can.Message(arbitration_id=self._id(packet), data=bytes(data), is_extended_id=True))
        except can.CanError as e:
            print(f"tx error: {e}", file=sys.stderr)

    def status_frame(self, flag=None):
        with self.lock:
            deg = math.degrees(self.pos())
            if self.single_turn:
                deg %= 360.0
            erpm = self.omega / ERPM2RADS
            cur = self.torque / KT
            d = struct.pack(">hhh", int(clamp(round(deg * 10.0), -32000, 32000)), int(clamp(round(erpm / 10.0), -32000, 32000)),
                            int(clamp(round(cur * 100.0), -6000, 6000)))
            d += struct.pack(">bB", int(clamp(round(self.temp), -20, 127)), self.error & 0xFF)
            pos32 = int(clamp(round(deg * 10000.0), -2**31, 2**31 - 1))
            want32 = self.send_pos32
        if flag is not None:
            d = d[:7] + bytes([flag])
        return d, struct.pack(">i", pos32) + bytes(4), want32

    def send_status(self, flag=None):
        d, d32, want32 = self.status_frame(flag)
        self._send(F_STATUS, d)
        if want32:
            self._send(F_POS32, d32)
        self.tx += 1
        if self.trace_feedback and self.tx % self.trace_feedback == 0:
            pos, spd, cur = struct.unpack(">hhh", d[:6])
            print(f"fb #{self.tx}: pos={pos / 10.0:.1f} deg  speed={spd * 10} ERPM  I={cur / 100.0:.2f} A  temp={d[6]}  err={d[7]}",
                  flush=True)

    # ---- state publishing
    def snapshot(self):
        with self.lock:
            kp, kd, p, v, t = self.mit
            mode = self.mode
            if mode in (Mode.POS, Mode.POSSPD):
                target = math.degrees(self.target - self.origin)          # deg
            elif mode in (Mode.RPM, Mode.DUTY):
                target = self.target                                      # rad/s
            elif mode == Mode.CURRENT:
                target = self.target                                      # A
            elif mode == Mode.BRAKE:
                target = self.ilim                                        # A
            else:
                target = 0.0
            self.seq += 1
            return {"seq": self.seq, "t": round(time.time() - self.t0, 3), "id": self.devid, "model": "AK70-9 KV60",
                    "mode": Mode.NAMES[mode], "pos_deg": math.degrees(self.pos()), "speed_rads": self.omega,
                    "speed_erpm": self.omega / ERPM2RADS, "torque_nm": self.torque, "current_a": self.torque / KT,
                    "temp_c": self.temp, "error": self.error, "target": target,
                    "mit": {"kp": kp, "kd": kd, "pos_rad": p, "vel_rads": v, "torque_nm": t},
                    "cmds": self.cmds, "last_cmd": self.last_cmd, "fb_tx": self.tx, "fb_hz": 0 if self.query_reply else self.feedback_hz,
                    "query_reply": self.query_reply, "origin_permanent": self.origin_permanent,
                    "single_turn": self.single_turn, "pos32": self.send_pos32,
                    "limits": {"tmax": TMAX, "vmax": VMAX, "imax": IMAX, "pmax": PMAX, "kp_max": KP_MAX, "kd_max": KD_MAX}}

    def publish_state(self):
        if self.sock is None:
            return
        data = json.dumps(self.snapshot(), separators=(",", ":")).encode("utf-8")
        for dst in self.publish:
            try:
                self.sock.sendto(data, dst)
            except OSError:
                pass                                                      # nobody listens, or the network is down: ignore

    # ---- commands
    def handle(self, msg):
        if not msg.is_extended_id or msg.is_error_frame or msg.is_remote_frame:
            return
        if (msg.arbitration_id & 0xFF) != self.devid:
            return
        packet = (msg.arbitration_id >> 8) & 0x1FFFFF
        data = bytes(msg.data)
        name = PNAMES.get(packet)
        if name is None:
            if self.verbose:
                print(f"rx unknown packet id {packet} data={data.hex(' ')}", flush=True)
            return
        self.cmds += 1
        self.last_cmd = name
        info = self._apply(packet, data)
        if self.verbose:
            print(f"rx {name} id=0x{msg.arbitration_id:08X} data={data.hex(' ')}  {info}", flush=True)
        if packet == P_DISABLE:
            self.send_status(flag=0x77)         # the manual: once, with data[7] = 0x77
        elif self.query_reply and packet != P_FRAMECFG:
            self.send_status()

    def _apply(self, packet, data):
        def i32(o=0):
            return struct.unpack(">i", data[o:o + 4])[0]

        def i16(o):
            return struct.unpack(">h", data[o:o + 2])[0]

        with self.lock:
            was_mit = self.mode == Mode.MIT
            if packet == P_DUTY and len(data) >= 4:
                duty = clamp(i32() / 100000.0, -1.0, 1.0)
                self.mode, self.target = Mode.DUTY, duty * VMAX
                info = f"duty={duty:.3f}"
            elif packet == P_CURRENT and len(data) >= 4:
                cur = clamp(i32() / 1000.0, -60.0, 60.0)
                self.mode, self.target = Mode.CURRENT, cur
                info = f"I={cur:.3f} A"
            elif packet == P_BRAKE and len(data) >= 4:
                self.ilim = clamp(i32() / 1000.0, 0.0, 60.0)
                self.mode, self.hold = Mode.BRAKE, self.theta
                info = f"brake I={self.ilim:.3f} A"
            elif packet == P_RPM and len(data) >= 4:
                erpm = clamp(i32(), -100000, 100000)
                self.mode, self.target = Mode.RPM, erpm * ERPM2RADS
                info = f"{erpm} ERPM = {self.target:.3f} rad/s"
            elif packet == P_POS and len(data) >= 4:
                deg = clamp(i32() / 10000.0, -36000.0, 36000.0)
                self.mode, self.target = Mode.POS, deg * DEG2RAD + self.origin
                self.wlim, self.alim = VMAX, 1000.0
                info = f"pos={deg:.4f} deg"
            elif packet == P_ORIGIN and len(data) >= 1:
                self.origin = self.theta
                self.origin_permanent = data[0] == 1
                info = "origin " + ("permanent" if self.origin_permanent else "temporary")
            elif packet == P_POSSPD and len(data) >= 8:
                deg = clamp(i32() / 10000.0, -36000.0, 36000.0)
                self.wlim = clamp(abs(i16(4)) * 10 * ERPM2RADS, 0.0, VMAX)
                self.alim = max(abs(i16(6)) * 10 * ERPM2RADS, 1e-3)
                self.mode, self.target = Mode.POSSPD, deg * DEG2RAD + self.origin
                info = f"pos={deg:.4f} deg  speed<={self.wlim:.3f} rad/s  accel<={self.alim:.3f} rad/s^2"
            elif packet == P_MIT and len(data) >= 8:
                self.mit = unpack_mit(data)
                self.mode = Mode.MIT
                kp, kd, p, v, t = self.mit
                info = f"Kp={kp:.2f} Kd={kd:.3f} p={p:.3f} rad v={v:.3f} rad/s t={t:.3f} N.m"
            elif packet == P_DISABLE:
                self.mode = Mode.DISABLED
                info = "disabled"
            elif packet == P_FRAMECFG and len(data) >= 8:
                flags = (data[6] << 8) | data[7]
                self.send_pos32 = bool(flags & 0x8000)
                self.single_turn = bool(flags & 0x4000)
                info = f"pos32={self.send_pos32} singleTurn={self.single_turn}"
            else:
                info = "(short payload, ignored)"
            self.wref = self.omega if self.mode in (Mode.POS, Mode.POSSPD) else 0.0
            enter_servo = was_mit and self.mode not in (Mode.MIT, Mode.DISABLED)
        if enter_servo and self.startframe:
            self._send(F_START, b"\xFA\xFB\xFC\xFD")
        return info

    # ---- dynamics
    def _torque_cmd(self, dt):
        m = self.mode
        if m == Mode.DISABLED:
            return 0.0
        if m == Mode.DUTY or m == Mode.RPM:
            return 2.0 * (self.target - self.omega) + self.b * self.target     # + friction feed-forward (no integral term)
        if m == Mode.CURRENT:
            return KT * self.target
        if m == Mode.BRAKE:
            return clamp(60.0 * (self.hold - self.theta) - 3.0 * self.omega, -KT * self.ilim, KT * self.ilim)
        if m in (Mode.POS, Mode.POSSPD):
            e = self.target - self.theta
            cap = min(self.wlim, math.sqrt(2.0 * self.alim * abs(e)))
            w = clamp(20.0 * e, -cap, cap)
            step = self.alim * dt
            self.wref = clamp(w, self.wref - step, self.wref + step)
            return 2.0 * (self.wref - self.omega) + self.b * self.wref
        kp, kd, p, v, t = self.mit
        return kp * (p + self.origin - self.theta) + kd * (v - self.omega) + t

    def step(self, dt):
        h = dt / self.SUBSTEPS
        with self.lock:
            for _ in range(self.SUBSTEPS):
                tau = clamp(self._torque_cmd(h), -TMAX, TMAX)
                self.torque = tau
                self.omega += (tau - self.b * self.omega) / self.J * h
                self.omega = clamp(self.omega, -VMAX, VMAX)
                self.theta += self.omega * h
            i = self.torque / KT
            self.temp += (25.0 + 0.05 * i * i - self.temp) * dt / 30.0     # slow heating with the current

    # ---- threads
    def _rx(self):
        while self.running:
            msg = self.bus.recv(0.05)
            if msg is None:
                continue
            if self.sniff:
                print(f"{msg.timestamp:.3f} {'EXT' if msg.is_extended_id else 'STD'} 0x{msg.arbitration_id:X} "
                      f"[{len(msg.data)}] {bytes(msg.data).hex(' ')}", flush=True)
            self.handle(msg)

    def _sim(self):
        tick = 0.001
        nxt = time.perf_counter()
        n = 0
        every = max(1, round(1000.0 / self.feedback_hz))
        pevery = max(1, round(1000.0 / self.publish_hz)) if self.publish_hz > 0 else 0
        while self.running:
            self.step(tick)
            n += 1
            if pevery and n % pevery == 0:
                self.publish_state()
            if not self.query_reply and n % every == 0:
                self.send_status()
            nxt += tick
            d = nxt - time.perf_counter()
            if d > 0:
                time.sleep(d)
            elif d < -0.1:
                nxt = time.perf_counter()

    def start(self):
        self.running = True
        if self.startframe:
            self._send(F_START, b"\xFA\xFB\xFC\xFD")
        self.threads = [threading.Thread(target=self._rx, daemon=True), threading.Thread(target=self._sim, daemon=True)]
        for t in self.threads:
            t.start()

    def stop(self):
        self.running = False
        for t in getattr(self, "threads", []):
            t.join(1.0)

    def run(self):
        self.start()
        try:
            while True:
                time.sleep(0.5)
        except KeyboardInterrupt:
            pass
        self.stop()


def main(argv=None):
    ap = argparse.ArgumentParser(description="CubeMars AK70-9 emulator (servo + MIT)")
    ap.add_argument("--interface", default="vector", help="python-can interface: vector, virtual, socketcan, pcan ...")
    ap.add_argument("--channel", default="0", help="vector: the application channel (0 = first channel of the VN1610)")
    ap.add_argument("--app-name", default="CANalyzer", help="vector: application name in Vector Hardware Config; 'none' = global index")
    ap.add_argument("--bitrate", type=int, default=1000000)
    ap.add_argument("--id", type=lambda s: int(s, 0), default=DEFAULT_ID, help=f"driver id [0, 255], default 0x{DEFAULT_ID:02X}")
    ap.add_argument("--feedback-hz", type=int, default=100, help="timed feedback rate, 1..2000 Hz")
    ap.add_argument("--query-reply", action="store_true", help="no timed feedback: one answer to each control command")
    ap.add_argument("--inertia", type=float, default=0.005, help="kg.m^2 on the output shaft")
    ap.add_argument("--friction", type=float, default=0.01, help="N.m.s/rad viscous friction")
    ap.add_argument("--error", type=lambda s: int(s, 0), default=0, help="error code in the feedback (0 = none, 1..7 as the manual)")
    ap.add_argument("--publish", action="append", default=None, metavar="HOST:PORT",
                    help=f"where the state is published (UDP, JSON); repeatable. Default {DEFAULT_PUBLISH[0]}:{DEFAULT_PUBLISH[1]}")
    ap.add_argument("--publish-hz", type=int, default=30, help="state publishing rate, 0 = off (default 30)")
    ap.add_argument("--no-startframe", action="store_true", help="do not send the 0x2C start frame")
    ap.add_argument("-v", "--verbose", action="store_true", help="print the commands received. Off by default")
    ap.add_argument("--trace-feedback", type=int, default=0, metavar="N", help="one line every N feedback frames. 0 = never")
    ap.add_argument("--sniff", action="store_true", help="debug: print every frame seen on the bus. Heavy")
    a = ap.parse_args(argv)
    if not 0 <= a.id <= 0xFF:
        ap.error("--id must be in [0, 255]")
    if not 1 <= a.feedback_hz <= 2000:
        ap.error("--feedback-hz must be in [1, 2000]")

    dests = []
    for d in (a.publish or ["%s:%d" % DEFAULT_PUBLISH]):
        host, _, port = d.rpartition(":")
        if not host or not port.isdigit():
            ap.error(f"--publish wants HOST:PORT, not {d!r}")
        dests.append((host, int(port)))
    if a.publish_hz < 0:
        ap.error("--publish-hz must be >= 0")

    kw = {"interface": a.interface, "bitrate": a.bitrate}
    if a.interface == "vector":
        kw.update(channel=int(a.channel), app_name=None if a.app_name.lower() == "none" else a.app_name, fd=False)
    else:
        kw["channel"] = a.channel
    try:
        bus = can.Bus(**kw)
    except Exception as e:                      # noqa
        print(f"cannot open the bus {kw}: {e}", file=sys.stderr)
        return 1
    emu = AK70(bus, devid=a.id, feedback_hz=a.feedback_hz, query_reply=a.query_reply, inertia=a.inertia, friction=a.friction,
               error=a.error, verbose=a.verbose, trace_feedback=a.trace_feedback, sniff=a.sniff, startframe=not a.no_startframe,
               publish=dests, publish_hz=a.publish_hz)
    print(f"AK70-9 emulator on {a.interface}:{a.channel} id=0x{emu.devid:02X} feedback "
          f"{'on query' if a.query_reply else str(a.feedback_hz) + ' Hz'}; state -> "
          f"{', '.join('%s:%d' % d for d in dests) if a.publish_hz else 'off'} (Ctrl+C to quit)", flush=True)
    emu.run()
    bus.shutdown()
    return 0


if __name__ == "__main__":
    sys.exit(main())

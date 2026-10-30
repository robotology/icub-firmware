#!/usr/bin/env python3
"""
A stand-in of the Speedgoat: it streams MIT command frames to the amcmj1 and collects the state frames that come back.

    speedgoat_emulator.py  --UDP 5001-->  amcmj1 (tGATEWAY)  --CAN-->  cubemars_emulator.py  --UDP-->  cubemars_viewer.py
                          <--UDP 5003--

It is a low-level test of amc.gtw: profiles straight to the board, no supervisor. To use the panel: sgact_daemon.py.

A frame is sent at every period (10 ms by default, then 1 ms), also when the profile has nothing to say: the board takes the frames as
its clock and as the watchdog. Python is not real time: the period is kept by a monotonic schedule, and the summary says how late it was.

  python speedgoat_emulator.py --board 10.0.1.99 --profile zero --duration 5 --enable
  python speedgoat_emulator.py --profile velocity --value 1 --kd 1 --duration 10 --enable --disable-at-end      (+1 rad/s)
  python speedgoat_emulator.py --profile step --value 1 --step-s 3 --duration 12 --enable --disable-at-end       (+1, -1, +1 ... rad/s)
  python speedgoat_emulator.py --profile sine --value 2 --freq 0.5 --kd 1 --duration 10 --enable --disable-at-end
  python speedgoat_emulator.py --profile position --value 0.5 --kp 5 --kd 0.5 --duration 6 --enable --disable-at-end
  python speedgoat_emulator.py --profile torque --value 0.1 --duration 3 --enable --disable-at-end

profiles (the MIT law is  torque_out = kp (pos - p) + kd (vel - v) + torque):
  zero      kp 0, kd 0, torque 0: the motor is free        velocity   kp 0, kd > 0, vel = value rad/s
  step      velocity, the sign changes every --step-s      sine       velocity, value * sin(2 pi freq t)
  position  kp, kd > 0, pos = value rad, vel 0             torque     kp 0, kd 0, torque = value N*m
  ramp      velocity from 0 to value in --step-s seconds, then it holds
  idle      no MIT at all: frames w/out a setpoint (the board sends the last one again, or nothing)

The firmware clamps the setpoint to its limits (kp 25, kd 3, vel 10 rad/s, torque 3 N*m at power on, see IDcubeLIMITS) and counts it.
--enable [cube|pax|both] runs OPcubeENABLE and/or OPpaxENABLE on the rpc socket first and waits for the acks (without a value: both). The motor starts with the SECOND frame: the first one after the
enable is dropped on purpose. Ctrl-C stops the stream: the board disables the motor after cfg.watchdogms (100 ms).
"""

import argparse
import math
import os
import socket
import sys
import threading
import time

_HERE = os.path.dirname(os.path.abspath(__file__))
_HOST = os.path.dirname(os.path.dirname(_HERE))        # host/
sys.path.insert(0, os.path.join(_HOST, "shared"))      # the codecs shared by the tools
import amc_gtw as gw                        # noqa: E402
import sgact_spec                           # noqa: E402   only for the config of the motor_macro (--mm)

MM_MODES = {"velocity": gw.CM_SPEEDVOLTAGE, "current": gw.CM_CURRENT, "voltage": gw.CM_OPENLOOP}

ERPM2RADS = 6.283185307 / 60.0 / (14.0 * 9.0)
gw_pmax = 12.56                                 # rad, range of the MIT position


def setpoint(args, t):
    """the MIT of the profile at t seconds: (kp, kd, pos, vel, torque), or None for no MIT"""
    p = args.profile
    if p == "zero":
        return (0.0, 0.0, 0.0, 0.0, 0.0)
    if p == "velocity":
        return (0.0, args.kd, 0.0, args.value, 0.0)
    if p == "step":
        sign = 1.0 if int(t // args.step_s) % 2 == 0 else -1.0
        return (0.0, args.kd, 0.0, sign * args.value, 0.0)
    if p == "sine":
        return (0.0, args.kd, 0.0, args.value * math.sin(2.0 * math.pi * args.freq * t), 0.0)
    if p == "position":
        return (args.kp, args.kd, args.value, 0.0, 0.0)
    if p == "torque":
        return (0.0, 0.0, 0.0, 0.0, args.value)
    if p == "ramp":                             # velocity from 0 to value in step_s seconds, then it holds
        return (0.0, args.kd, 0.0, args.value * min(1.0, t / max(args.step_s, 1e-6)), 0.0)
    return None


class Collector(threading.Thread):
    """it reads the state frames: counts them, matches the echo of seq to the send time (round trip), keeps the last one"""

    def __init__(self, link):
        super().__init__(daemon=True)
        self.link, self.running = link, True
        self.sent = {}                          # seq -> time of the send
        self.rtt, self.received, self.last, self.last_at = [], 0, None, None
        self.lock = threading.Lock()

    def run(self):
        while self.running:
            st = self.link.recv_state(0.05)
            if st is None:
                continue
            now = time.monotonic()
            with self.lock:
                self.received += 1
                self.last, self.last_at = st, now
                t0 = self.sent.pop(st.gw.seq, None)
                if t0 is not None:
                    self.rtt.append(now - t0)


def describe(st):
    if st is None:
        return "no state yet"
    c, m = st.cube_state, st.cube_status
    return (f"cube {gw.CUBE_STATES.get(m.state, m.state) if m else '?'}  pos {c.pos:+.3f} rad  vel {c.vel:+.3f} rad/s ({c.vel / ERPM2RADS:+.0f} ERPM)  "
            f"I {c.current:+.2f} A  T {c.temp} C  err {c.error}  frames {c.counter}   pax {st.pax_torque.mx:+.3f} {st.pax_torque.my:+.3f} {st.pax_torque.mz:+.3f}   "
            f"gw: packets {st.gw.packets} clamped {st.gw.clamped} lost {st.gw.lost} discarded {st.gw.discarded} notrunning {st.gw.notrunning}"
            + (f"   motor_macro {gw.CM_NAMES.get(st.motor.controlmode, st.motor.controlmode)} vel {st.motor.velocity:+.1f} deg/s "
               f"I {st.motor.current:+.2f} A pwm {st.motor.pwm:+.1f} %{' EXTFAULT' if st.motor.faults & gw.FAULT_EXTERNAL else ''}"
               if st.motor is not None else ""))


def devices_of(enable):
    """--enable: None (not given) -> '', True (the old flag) -> 'cube', 'cube' / 'pax' / 'both' -> as is"""
    if not enable:
        return ""
    return "cube" if enable is True else enable


def run(args, link=None, out=print):
    link = link or gw.Gateway(args.board, args.cmd_port, args.rpc_port, args.state_port)
    coll = Collector(link)
    coll.start()
    result = {"enabled": None}
    cm = MM_MODES.get(getattr(args, "mm", None) or "", None)
    try:
        if cm is not None:                      # the motor_macro: its config from the TOML, then the control mode
            mmcfg = sgact_spec.load(args.spec).mmcfg
            for op, arg, name in ((gw.OP_MOTOR_PIDS, gw.pids_arg(*mmcfg[0:6]), "OPmotorPIDS"),
                                  (gw.OP_MOTOR_SETUP, gw.setup_arg(*mmcfg[6:14]), "OPmotorSETUP"),
                                  (gw.OP_MOTOR_MODE, gw.mode_arg(cm), f"OPmotorMODE {gw.CM_NAMES[cm]}")):
                ok, why = link.rpc_run(op, arg=arg)
                out(f"{name}: {'ack' if ok else 'nak / no reply: ' + str(why)}")
                if not ok:
                    result["mm"] = False
                    return result
        devices = devices_of(args.enable)
        if devices:
            ok, why = link.enable(devices)
            result["enabled"] = ok
            out(f"ENABLE {devices}: {'ack' if ok else 'nak / no reply: ' + str(why)}")
            if not ok:
                return result
        period = args.period_ms / 1000.0
        start = time.monotonic()
        seq = args.seq
        late, nsent, nextlog = [], 0, start + 1.0
        while True:
            now = time.monotonic()
            t = now - start
            if t >= args.duration:
                break
            sp = setpoint(args, t)
            target = (args.mm_value, cm, seq) if cm is not None else None
            if sp is None and target is None:
                link.send_empty()               # no setpoint: seq does not move, the board counts a gap of seq as lost frames
            else:
                with coll.lock:
                    coll.sent[seq & 0xFFFFFFFF] = now
                    if len(coll.sent) > 2000:
                        coll.sent.pop(next(iter(coll.sent)))
                link.send_command((*sp, seq) if sp is not None else None, target)
                seq += 1
            nsent += 1
            due = start + nsent * period
            late.append(max(0.0, time.monotonic() - due))
            if now >= nextlog:
                nextlog += 1.0
                with coll.lock:
                    out(f"t {t:5.1f}s  sent {nsent}  state {coll.received}  " + describe(coll.last))
            wait = due - time.monotonic()
            if wait > 0:
                time.sleep(wait)
        time.sleep(0.2)                         # the last states
    finally:
        coll.running = False
        if cm is not None:
            ok, why = link.rpc_run(gw.OP_MOTOR_MODE, arg=gw.mode_arg(gw.CM_IDLE))
            out(f"OPmotorMODE idle: {'ack' if ok else 'nak / no reply: ' + str(why)}")
        if args.disable_at_end and devices_of(args.enable):
            ok, why = link.disable(devices_of(args.enable))
            out(f"DISABLE {devices_of(args.enable)}: {'ack' if ok else 'nak / no reply: ' + str(why)}")
            result["disabled"] = ok
        coll.join(0.2)
    with coll.lock:
        rtt = sorted(coll.rtt)
        result.update(nextseq=seq, sent=nsent, received=coll.received, last=coll.last, late_max_ms=max(late) * 1000.0 if late else 0.0)
        out(f"sent {nsent} frames, received {coll.received} state frames ({100.0 * coll.received / max(1, nsent):.1f}%)")
        if rtt:
            out(f"round trip (send -> state with the same seq): min {rtt[0] * 1000:.2f} ms, median {rtt[len(rtt) // 2] * 1000:.2f} ms, "
                f"max {rtt[-1] * 1000:.2f} ms  ({len(rtt)} matched)")
        out(f"python lateness: max {result['late_max_ms']:.2f} ms")
        out("last state: " + describe(coll.last))
    return result

INFO = """\
speedgoat_emulator.py - a stand-in of the Speedgoat, to be run on a PC (Windows) before the real one exists.

WHAT IT DOES
  It plays the Speedgoat side of the amcmj1 gateway, with the same TETHER frames that the Simulink model will send:
    UDP 5001  -> board : one command frame every --period-ms (10 ms): the MIT setpoint kp kd pos vel torque + seq.
                         Frames with no setpoint (profile idle) are heartbeats: they feed the watchdog.
    UDP 5002  <> board : with --enable [cube|pax|both] / --disable-at-end it runs OPcubeENABLE / OPpaxENABLE and the DISABLEs (cube: the board
                         opens / closes the gate, no CAN frame goes to the motor at the enable, the motor starts with the MIT that
                         follows. pax: the board initialises and starts the PX3Q: its torques arrive in the state frame)
    UDP 5003  <- board : the state frame (CubeMars, 3 Paxini torques, CM7 motor state, gateway stats): this PC listens here and
                         the board sends it to the IP of the last command frame. The echo of seq gives the round trip.
  Every second it prints the latest state, at the end the loss, the round trip and the lateness of Python.

THE MIT LAW      torque_out = kp (pos - p) + kd (vel - v) + torque
PROFILES (--profile, --value, --kp, --kd, --freq, --step-s, --duration)
  zero      kp 0, kd 0, torque 0: the motor is FREE (it does not brake)
  velocity  kp 0, kd > 0, vel = --value rad/s            (kd brakes: --value 0 holds still)
  step      velocity whose sign changes every --step-s
  sine      velocity = --value * sin(2 pi --freq t)
  position  kp, kd > 0, pos = --value rad, vel 0         (pos must be inside +-12.56 rad)
  torque    kp 0, kd 0, torque = --value N*m             (the motor accelerates: keep it short)
  ramp      velocity from 0 to --value in --step-s seconds, then it holds
  idle      no setpoint at all: heartbeats only
LIMITS OF THE BOARD (clamp and count, see IDcubeLIMITS): kp 25, kd 3, |vel| 10 rad/s, |torque| 3 N*m; a NaN is rejected.
WATCHDOG: no valid frame for 100 ms and the board sends the disable to the motor (Ctrl-C does that).

THE MOTOR_MACRO (--mm velocity|current|voltage, --mm-value)
  Before streaming: OPmotorPIDS and OPmotorSETUP w/ [devices.motor_macro.config] of sgact_panel.toml, then OPmotorMODE.
  Every frame then carries set<IDmotorTARGET, {value, mode, seq}>. At the end: OPmotorMODE idle. Until the ICC exists,
  amc.gtw plays amc.mot (IDmotorSTATE says emulated).
    py speedgoat_emulator.py --profile idle --mm velocity --mm-value 90 --duration 5          only the motor_macro
    py speedgoat_emulator.py --profile velocity --value 1 --enable cube --mm current --mm-value 0.2 --duration 5 --disable-at-end

THE PANEL
  This program has no supervisor: to drive amc.gtw w/ the panel (sgact_panel.py) use the Speedgoat, or sgact_daemon.py
  (sgact on the PC: the same supervisor and modes as the Speedgoat).

EXAMPLES
  py speedgoat_emulator.py --profile velocity --value 1 --kd 1 --duration 10 --enable --disable-at-end
  py speedgoat_emulator.py --profile position --value 0.5 --kp 5 --kd 0.5 --duration 6 --enable --disable-at-end
  py speedgoat_emulator.py --test                       the automatic checks, safe ones (CubeMars)
  py speedgoat_emulator.py --test --enable both         the same, with the Paxini too (px3q_emulator.py must run)
  py speedgoat_emulator.py --test --enable pax          only the Paxini: do its samples arrive?
  py speedgoat_emulator.py --test all                   plus the limits and the watchdog (emulated motor only!)
  py speedgoat_emulator.py --profile idle --duration 10 --enable pax --disable-at-end     only the Paxini, no motor
"""


def _state_ok(st):
    return st is not None and st.cube_state is not None and st.cube_status is not None


def run_tests(args, link=None, out=print, scale=1.0):
    """--test: enable once, run a sequence of short streams, check what comes back, disable. 'all' adds the limits and the watchdog.
    The thresholds are the ones of the emulated motor: on a real motor with a load they may need to be relaxed.
    Returns 0 if everything passed"""
    link = link or gw.Gateway(args.board, args.cmd_port, args.rpc_port, args.state_port)
    results = []
    seqbox = [1]

    def sub(profile, duration, **kw):
        a = argparse.Namespace(**vars(args))
        a.profile, a.duration, a.enable, a.disable_at_end, a.seq = profile, duration * scale, False, False, seqbox[0]
        a.period_ms = 10.0
        for k, v in kw.items():
            setattr(a, k, v)
        r = run(a, link, out=lambda s: None)
        seqbox[0] = r["nextseq"]
        return r

    def check(name, cond, detail):
        results.append(bool(cond))
        out(f"  {'PASS' if cond else 'FAIL'}  {name:34s} {detail}")

    def link_ok(r):
        return r.get("sent", 0) > 0 and r.get("received", 0) >= 0.99 * r["sent"]

    devices = devices_of(args.enable) or "cube"          # --test --enable pax / both: the devices to check
    use_cube, use_pax = devices in ("cube", "both"), devices in ("pax", "both")
    done_disable = False
    ok, why = link.enable(devices)
    out(f"ENABLE {devices}: {'ack' if ok else 'nak / no reply: ' + str(why)}")
    if not ok:
        return 2
    try:
        r = sub("idle", 0.4)
        st = r.get("last")
        check("heartbeats, state comes back", link_ok(r) and _state_ok(st), f"{r.get('received')}/{r.get('sent')} states")
        if not _state_ok(st):
            return 2
        lost0, wd0 = st.gw.lost, st.cube_status.watchdogs
        pax0 = st.pax_torque.counter
        if use_pax and not use_cube:
            r = sub("idle", 1.0)
            st = r["last"]
            check("sensor_torque: samples arrive", link_ok(r) and _state_ok(st) and st.pax_torque.counter >= pax0 + 20,
                  f"counter {pax0} -> {st.pax_torque.counter}  Mx My Mz {st.pax_torque.mx:+.3f} {st.pax_torque.my:+.3f} {st.pax_torque.mz:+.3f}")
            r = sub("idle", 0.2)
            st = r["last"]
            check("no frame lost in the whole test", _state_ok(st) and st.gw.lost == lost0, f"lost {st.gw.lost - lost0}")
            ok, why = link.disable(devices)
            out(f"DISABLE {devices}: {'ack' if ok else 'nak / no reply: ' + str(why)}")
            done_disable = True
            out(f"{sum(results)}/{len(results)} checks passed")
            return 0 if all(results) else 1

        r = sub("velocity", 1.5, value=0.0, kd=1.0)
        st = r["last"]
        check("hold still (velocity 0, kd 1)", link_ok(r) and _state_ok(st) and abs(st.cube_state.vel) < 0.15, f"vel {st.cube_state.vel:+.3f} rad/s")

        for sign in (1.0, -1.0):
            r = sub("velocity", 3.0, value=sign, kd=1.0)
            st = r["last"]
            check(f"velocity {sign:+.0f} rad/s", link_ok(r) and _state_ok(st) and abs(st.cube_state.vel - sign) < 0.15,
                  f"vel {st.cube_state.vel:+.3f} rad/s ({st.cube_state.vel / ERPM2RADS:+.0f} ERPM)")

        r = sub("step", 2.4, value=1.0, kd=1.0, step_s=1.5 * scale)
        st = r["last"]
        check("step +1 then -1 rad/s", link_ok(r) and _state_ok(st) and st.cube_state.vel < -0.5, f"vel at the end {st.cube_state.vel:+.3f} rad/s (expected < -0.5)")

        sub("velocity", 1.5, value=0.0, kd=1.0)
        r = sub("velocity", 0.5, value=0.0, kd=1.0)
        p0 = r["last"].cube_state.pos
        target = p0 + 0.3
        if abs(target) < gw_pmax:
            r = sub("position", 3.0, value=target, kp=5.0, kd=0.5)
            st = r["last"]
            check("position +0.3 rad", link_ok(r) and abs(st.cube_state.pos - target) < 0.05 and abs(st.cube_state.vel) < 0.05,
                  f"pos {st.cube_state.pos:+.3f} (target {target:+.3f}) vel {st.cube_state.vel:+.3f}")
        else:
            out(f"  SKIP  position                           the motor is at {p0:+.2f} rad, out of the MIT range: restart the emulator")

        r = sub("torque", 1.0, value=0.05)
        st = r["last"]
        check("torque +0.05 N*m", link_ok(r) and _state_ok(st) and (st.cube_state.current > 0.005 or st.cube_state.vel > 0.1),
              f"I {st.cube_state.current:+.3f} A  vel {st.cube_state.vel:+.3f} rad/s")
        sub("velocity", 2.0, value=0.0, kd=1.0)

        if getattr(args, "test", "basic") == "all":
            c0 = sub("idle", 0.2)["last"].gw.clamped
            r = sub("velocity", 1.0, value=20.0, kd=5.0)
            st = r["last"]
            check("limits: vel 20 kd 5 clamped", link_ok(r) and st.gw.clamped > c0 and abs(st.cube_state.vel) <= 10.5,
                  f"clamped +{st.gw.clamped - c0}  vel {st.cube_state.vel:+.3f} rad/s")
            sub("velocity", 2.0, value=0.0, kd=1.0)

            sub("velocity", 1.0, value=1.0, kd=1.0)
            time.sleep(0.4 * max(scale, 0.25) + 0.3)            # silence: the board must disable by itself
            r = sub("idle", 0.2)
            st = r["last"]
            check("watchdog after 0.4 s of silence", _state_ok(st) and st.cube_status.watchdogs > wd0, f"watchdogs {wd0} -> {st.cube_status.watchdogs}")

        r = sub("idle", 0.2)
        st = r["last"]
        if use_pax:
            check("sensor_torque: samples arrive", _state_ok(st) and st.pax_torque.counter >= pax0 + 20,
                  f"counter {pax0} -> {st.pax_torque.counter}  Mx My Mz {st.pax_torque.mx:+.3f} {st.pax_torque.my:+.3f} {st.pax_torque.mz:+.3f}")
        check("no frame lost in the whole test", _state_ok(st) and st.gw.lost == lost0, f"lost {st.gw.lost - lost0}")
    finally:
        if not done_disable:
            ok, why = link.disable(devices)
            out(f"DISABLE {devices}: {'ack' if ok else 'nak / no reply: ' + str(why)}")
    out(f"{sum(results)}/{len(results)} checks passed")
    return 0 if all(results) else 1


def main():
    ap = argparse.ArgumentParser(description="a stand-in of the Speedgoat: streams MIT frames to the amcmj1 (see the top of this file)")
    ap.add_argument("--board", default="10.0.1.99")
    ap.add_argument("--cmd-port", type=int, default=gw.PORT_CMD)
    ap.add_argument("--rpc-port", type=int, default=gw.PORT_RPC)
    ap.add_argument("--state-port", type=int, default=gw.PORT_STATE, help="where this PC listens: the board sends the state here")
    ap.add_argument("--profile", default="zero", choices=["zero", "velocity", "step", "sine", "position", "torque", "ramp", "idle"])
    ap.add_argument("--value", type=float, default=1.0, help="rad/s (velocity, step, sine), rad (position), N*m (torque)")
    ap.add_argument("--kp", type=float, default=5.0)
    ap.add_argument("--kd", type=float, default=1.0)
    ap.add_argument("--freq", type=float, default=0.5, help="Hz of the sine")
    ap.add_argument("--step-s", type=float, default=3.0)
    ap.add_argument("--period-ms", type=float, default=10.0, help="10 now, 1 later")
    ap.add_argument("--duration", type=float, default=5.0, help="seconds")
    ap.add_argument("--seq", type=int, default=1, help="first seq")
    ap.add_argument("--enable", nargs="?", const="both", choices=["cube", "pax", "both"],
                    help="before streaming, enable on the rpc socket: the CubeMars (cube), the Paxini (pax) or both. without a value: both")
    ap.add_argument("--disable-at-end", action="store_true", help="disable at the end what --enable enabled (needs --enable)")
    ap.add_argument("--mm", choices=sorted(MM_MODES), default=None,
                    help="the motor_macro too: OPmotorPIDS and OPmotorSETUP w/ the config of the TOML, OPmotorMODE, then a target in "
                         "every frame, OPmotorMODE idle at the end")
    ap.add_argument("--mm-value", type=float, default=90.0, help="the target of the motor_macro: deg/s (velocity), A (current), %% (voltage)")
    ap.add_argument("--spec", default=None, help="sgact_panel.toml for --mm. default: $SGACT_SPEC, then host/speedgoat/sgact_panel.toml")
    ap.add_argument("--info", action="store_true", help="what this program does, the profiles, the limits, examples")
    ap.add_argument("--test", nargs="?", const="basic", choices=["basic", "all"],
                    help="automatic checks: enable, hold, velocity, step, position, torque (basic); all adds the limits and the watchdog, "
                         "which drive the motor hard: only with the emulated one")
    args = ap.parse_args()
    if args.info:
        print(INFO)
        return 0
    if args.test:
        try:
            return run_tests(args)
        except KeyboardInterrupt:
            print("stopped: the board disables the motor when the watchdog expires")
            return 1
    try:
        res = run(args)
    except KeyboardInterrupt:
        print("stopped: the board disables the motor when the watchdog expires")
        return 1
    return 0 if res.get("received", 0) > 0 else 2


if __name__ == "__main__":
    sys.exit(main())

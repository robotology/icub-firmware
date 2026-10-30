#!/usr/bin/env python3
"""
test_sgact_supervisor.py - the supervisor of the daemon (sgact_supervisor.py + sgact_modes.py) against a fake amc.gtw,
step by step (1 ms), w/out sockets. It checks the stories below and it writes the trace that
host/speedgoat/simulink/tests/test_parity.m plays again on sgact_controller.m (sgact_sup_a + modes + sgact_sup_b): the two supervisors must give the same outputs.

    py test_sgact_supervisor.py                     (writes ../simulink/tests/parity_trace.txt.gz if that folder exists)
    py test_sgact_supervisor.py --trace path.txt.gz
"""

import argparse
import gzip
import io
import math
import os
import sys

_HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, _HERE)
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(_HERE)), "shared"))     # host/shared
import sgact_spec                          # noqa: E402
from sgact_supervisor import C, Supervisor, fb_default, req_default      # noqa: E402

DT = 0.001
D = math.pi / 180.0                         # the panel speaks deg, the modes rad: a value of the TOML times D


class FakeGtw:
    """amc.gtw in a few lines: rpc enable / disable, MIT -> velocity, counters; it can be mute or have a broken device.
    the motor_cubemars: off, then OPcubeENABLE -> running (status 2), OPcubeDISABLE -> initialised (status 1); a MIT moves
    it only while it runs. the motor_macro as amcmj1_motdriver: pids + setup -> configured, then a control mode, then the
    targets of that mode; it can have an external fault, or ignore the control modes. the position sensor: always read
    (no rpc), it can be broken"""

    def __init__(self):
        self.cube = self.cube_run = self.pax = False
        self.cube_broken = self.pax_broken = self.pos_broken = self.mute = False
        self.cubecnt = self.paxcnt = self.poscnt = 0
        self.vel = self.pos = 0.0
        self.mit = False
        self.trx = 0.0
        self.mm_conf, self.mm_mode, self.mm_vel, self.mm_fault, self.mm_ignore = 0, 0, 0.0, False, False
        self.gwfault = False
        self.rpcs = []

    def apply(self, cmd, t):
        if cmd["rpc_go"]:
            op = cmd["rpc_op"]
            self.rpcs.append((t, op, cmd["rpc_arg"]))
            if op == C.OP_GW_FAULT:
                self.gwfault = cmd["rpc_arg"] != 0
                if self.gwfault:
                    self.cube_run, self.mm_mode = False, C.CM_IDLE
            if op == C.OP_CUBE_ENABLE and not self.cube_broken and not self.gwfault:
                self.cube, self.cube_run = True, True
            elif op == C.OP_CUBE_DISABLE:
                self.cube_run = False
            self.pax = True if op == C.OP_PAX_ENABLE else False if op == C.OP_PAX_DISABLE else self.pax
            if op == C.OP_MOTOR_PIDS:
                self.mm_conf |= 1
            elif op == C.OP_MOTOR_SETUP:
                self.mm_conf |= 2
            elif op == C.OP_MOTOR_MODE and not self.mm_ignore and (cmd["rpc_arg"] == C.CM_IDLE or (self.mm_conf == 3 and not self.gwfault)):
                self.mm_mode = cmd["rpc_arg"]
        if cmd["motor_macro_valid"] and not self.gwfault and cmd["motor_macro_mode"] == self.mm_mode and self.mm_mode == C.CM_SPEEDVOLTAGE:
            self.mm_vel = cmd["motor_macro_target"]
        if self.mm_mode == C.CM_IDLE:
            self.mm_vel = 0.0
        self.mit = cmd["motor_cubemars_mit_valid"] and not self.gwfault
        if self.cube_run and not self.cube_broken and self.mit:
            self.cubecnt += 1
            self.vel = cmd["motor_cubemars_vel"]
            self.pos += self.vel * DT
        if self.pax and not self.pax_broken:
            self.paxcnt += 1
        if not self.pos_broken:
            self.poscnt += 1
        if not self.mute:
            self.trx = t

    def fb(self):
        fb = fb_default()
        st = 0
        if self.cube and not self.cube_broken:
            st = 2 if self.cube_run else 1
        fb.update(valid=True, has_motor_cubemars_status=True, motor_cubemars_status=st, motor_cubemars_counter=self.cubecnt,
                  motor_cubemars_vel=self.vel, motor_cubemars_pos=self.pos, sensor_torque_counter=self.paxcnt, trx=self.trx,
                  has_motor_macro=True, motor_macro_mode=self.mm_mode, motor_macro_configured=self.mm_conf,
                  motor_macro_faults=C.FAULT_EXTERNAL if self.mm_fault else 0, motor_macro_vel=self.mm_vel,
                  gw_extfault=1 if self.gwfault else 0, has_sensor_position=True, sensor_position_counter=self.poscnt,
                  sensor_position_flags=0 if self.pos_broken else C.POS_VALID)
        return fb


def run(spec):
    act, dev = spec.actions, spec.devices
    cube, pax, macro, posn = 1 << dev["motor_cubemars"], 1 << dev["sensor_torque"], 1 << dev["motor_macro"], 1 << dev["sensor_position"]
    m = spec.mode_by_name
    sup = Supervisor(spec, speedgoat=True)        # as the Speedgoat: the trace is played again by test_parity.m
    gtw = FakeGtw()
    req = req_default(spec.max_params)
    req["mmcfg"] = list(spec.mmcfg)
    rid = [0]

    def ask(action, **kw):
        rid[0] += 1
        req.update(kw)
        req.update(have=True, alive=True, crc_ok=True, action=action, id=rid[0])

    def params(*v):
        return list(v) + [0.0] * (spec.max_params - len(v))

    # (time [s], what happens): the stories
    events = [
        (0.300, lambda: ask(act["init"], devices=cube | pax | posn)),
        (1.500, lambda: ask(act["start"], mode=m["motor_cubemars_velocity_step"].id, params=params(0.0, 60.0, 0.0, 1.0, 0.0, 0.5),
                            duration=0.0)),
        (2.500, lambda: req.update(params=params(0.0, 120.0, 0.0, 1.0, 0.0, 0.5))),         # live change
        (3.000, lambda: ask(act["stop"])),
        (3.800, lambda: ask(act["start"], mode=m["motor_cubemars_position"].id, params=params(20.0, 0.0, 5.0, 0.5, 0.0), duration=1.0)),
        (5.500, lambda: ask(act["start"], mode=m["motor_cubemars_position"].id, params=params(20.0, 0.0, 30.0, 0.5, 0.0))),  # kp out of range
        (5.550, lambda: ask(act["start"], mode=m["motor_cubemars_idle"].id, params=params(0.0, 0.0, 1.0, 0.0, 0.0))),   # kp must be 0
        (5.600, lambda: ask(act["start"], mode=m["motor_cubemars_idle"].id, params=params(), duration=0.0)),
        (5.700, lambda: ask(act["init"], devices=pax)),                                        # wrong state
        (6.500, lambda: req.update(alive=False)),                                              # the panel goes silent
        (8.000, lambda: ask(act["deinit"], devices=cube)),
        (8.500, lambda: ask(act["start"], mode=m["motor_cubemars_position"].id, params=params(20.0, 0.0, 5.0, 0.5, 0.0))),   # cube not initialised
        (9.000, lambda: setattr(gtw, "cube_broken", True)),
        (9.001, lambda: ask(act["init"], devices=cube)),                                       # it fails: err 1
        (11.000, lambda: setattr(gtw, "mute", True)),                                          # amc.gtw silent: err 4
        (13.000, lambda: (ask(act["init"], devices=pax), req.update(crc_ok=False))),          # another TOML
        (14.000, lambda: (setattr(gtw, "mute", False), setattr(gtw, "cube_broken", False), setattr(gtw, "pos_broken", True))),
        (14.001, lambda: ask(act["init"], devices=posn)),                                      # it fails: err 7
        (15.500, lambda: setattr(gtw, "pos_broken", False)),
        (15.501, lambda: ask(act["init"], devices=cube | pax | posn)),
        (17.000, lambda: ask(act["init"], devices=macro)),                                     # the motor_macro
        (18.000, lambda: ask(act["start"], mode=m["motor_macro_velocity"].id, params=params(90.0), duration=0.0)),
        (19.000, lambda: ask(act["stop"])),
        (20.000, lambda: ask(act["start"], mode=m["motor_macro_current"].id, params=params(0.5), duration=0.0)),
        (20.500, lambda: setattr(gtw, "mm_fault", True)),                                      # external fault: err 6
        (21.000, lambda: (setattr(gtw, "mm_fault", False), setattr(gtw, "mm_ignore", True))),
        (21.001, lambda: ask(act["start"], mode=m["motor_macro_voltage"].id, params=params(20.0), duration=0.0)),  # err 5
        (23.000, lambda: (setattr(gtw, "mm_ignore", False),
                          ask(act["start"], mode=m["spark_dual_motor_actuation"].id, params=params(0.0, 0.0, 5.0, 0.5, 0.0, 0.0)))),
        (23.300, lambda: ask(act["stop"])),
        (23.500, lambda: ask(act["start"], mode=m["motor_cubemars_position"].id, params=params(10.0, 0.0, 5.0, 0.5, 0.0), duration=0.0)),
        (24.000, lambda: ask(act["stop"])),
        (24.500, lambda: ask(act["start"], mode=m["idle"].id, params=params(), duration=0.0)),
        (25.000, lambda: ask(act["stop"])),
        (25.500, lambda: ask(act["deinit"], devices=macro)),
        (26.500, lambda: ask(act["init"], devices=macro)),
        (27.500, lambda: ask(act["start"], mode=m["spark_dual_motor_actuation"].id, params=params(10.0, 0.0, 5.0, 0.5, 0.0, 0.0))),
        (28.000, lambda: req.update(extfault=True)),                                          # EXTFAULT pressed
        (28.300, lambda: ask(act["start"], mode=m["idle"].id, params=params())),               # refused while in extfault
        (28.600, lambda: req.update(alive=False)),                                             # even w/out the panel it stays
        (29.000, lambda: req.update(extfault=False, alive=True)),                              # released
        (29.300, lambda: ask(act["start"], mode=m["motor_cubemars_position"].id, params=params(10.0, 0.0, 5.0, 0.5, 0.0))),
        (29.600, lambda: req.update(extfault=True)),                                           # pressed while it runs
        (30.000, lambda: req.update(extfault=False)),
        (30.200, lambda: req.update(extfault=True)),
        (30.210, lambda: setattr(gtw, "gwfault", False)),                                       # amc.gtw restarts: it forgets
    ]
    trace, log = [], []
    k, ev = 0, 0
    while k * DT < 31.0:
        t = round(k * DT, 6)
        while ev < len(events) and t >= events[ev][0] - 1e-9:
            events[ev][1]()
            ev += 1
        req["cnt"] = k // 200
        fb = gtw.fb()
        cmd, s = sup.step(fb, dict(req), t)
        gtw.apply(cmd, t)
        trace.append((t, dict(req), fb, cmd, s))
        log.append(s)
        k += 1
    return trace, gtw


def check(trace, spec, gtw):
    act = spec.actions
    ok = []

    def at(t):
        return min(trace, key=lambda r: abs(r[0] - t))

    def test(name, cond):
        ok.append(bool(cond))
        print(f"  {'PASS' if cond else 'FAIL'}  {name}")

    cube, pax, macro, posn = (1 << spec.devices[n] for n in ("motor_cubemars", "sensor_torque", "motor_macro", "sensor_position"))
    allthree = cube | pax | posn

    def gtw_cube_run_at(t):
        return at(t)[2]["motor_cubemars_status"] == 2

    def ops(t0, t1):
        return [(op, a) for t, op, a in gtw.rpcs if t0 <= t < t1]

    test("no panel: idle, nothing initialised", at(0.2)[4]["state"] == C.ST_IDLE and at(0.2)[4]["inited"] == 0)
    test("init cube + pax + pos: sensor_torque, motor_cubemars enabled, checked, then disabled again",
         ops(0.0, 1.4) == [(C.OP_PAX_ENABLE, 0), (C.OP_CUBE_ENABLE, 0), (C.OP_CUBE_DISABLE, 0)])
    test("init done, the three initialised", at(1.4)[4]["inited"] == allthree and at(1.4)[4]["result"] == C.RES_DONE
         and at(1.4)[2]["motor_cubemars_status"] == 1)
    test("before start: heartbeats only", not at(1.4)[3]["motor_cubemars_mit_valid"])
    test("velocity_step: OPcubeENABLE at the start (and the motor_macro is not initialised: no OPmotorMODE)",
         ops(1.5, 1.6) == [(C.OP_CUBE_ENABLE, 0)])
    test("velocity_step: +60 deg/s, then -60 deg/s after half s, kd 1", at(1.7)[3]["motor_cubemars_vel"] == 60.0 * D
         and at(2.2)[3]["motor_cubemars_vel"] == -60.0 * D and at(2.2)[3]["motor_cubemars_kd"] == 1.0)
    test("live change: 120 deg/s", abs(at(2.7)[3]["motor_cubemars_vel"]) == 120.0 * D)
    test("stop: vel 0 for 0.2 s, then idle, OPcubeDISABLE, no more MIT", at(3.1)[3]["motor_cubemars_vel"] == 0.0
         and at(3.1)[3]["motor_cubemars_mit_valid"] and at(3.5)[4]["state"] == C.ST_IDLE and not at(3.5)[3]["motor_cubemars_mit_valid"]
         and ops(3.0, 3.5) == [(C.OP_CUBE_DISABLE, 0)] and at(3.5)[2]["motor_cubemars_status"] == 1)
    test("position 20 deg kp 5 kd 0.5, duration 1 s: the MIT, then it stops by itself and disables",
         at(4.3)[3]["motor_cubemars_pos"] == 20.0 * D and at(4.3)[3]["motor_cubemars_kp"] == 5.0 and at(4.3)[3]["motor_cubemars_kd"] == 0.5
         and at(5.4)[4]["state"] == C.ST_IDLE and ops(4.7, 5.4) == [(C.OP_CUBE_DISABLE, 0)])
    test("kp 30 refused: parameter out of range", at(5.52)[4]["result"] == C.RES_REJECTED and at(5.52)[4]["reason"] == C.WHY_PARAMS)
    test("motor_cubemars_idle w/ kp 1 refused: its values are fixed at 0", at(5.58)[4]["result"] == C.RES_REJECTED
         and at(5.58)[4]["reason"] == C.WHY_PARAMS)
    test("motor_cubemars_idle: running, OPcubeDISABLE at the start (amc.gtw: MIT of rest, disable), no MIT",
         at(5.65)[4]["state"] == C.ST_RUNNING and not at(5.65)[3]["motor_cubemars_mit_valid"]
         and ops(5.6, 5.65) == [(C.OP_CUBE_DISABLE, 0)] and at(5.65)[2]["motor_cubemars_status"] == 1)
    test("init while running refused: wrong state", at(5.8)[4]["result"] == C.RES_REJECTED and at(5.8)[4]["reason"] == C.WHY_STATE)
    test("panel silent while running: stop, err 3, disabled", at(7.0)[4]["state"] == C.ST_IDLE and at(7.0)[4]["err"] == C.ERR_KEEPALIVE
         and ops(6.5, 7.0) == [(C.OP_CUBE_DISABLE, 0)])
    test("deinit cube: sensor_torque and sensor_position stay", at(8.4)[4]["inited"] == pax | posn)
    test("start w/out the motor_cubemars refused: devices", at(8.6)[4]["result"] == C.RES_REJECTED and at(8.6)[4]["reason"] == C.WHY_DEVICES)
    test("init of a broken motor_cubemars: err 1, failed", at(10.5)[4]["err"] == C.ERR_MOTOR_CUBEMARS and at(10.5)[4]["result"] == C.RES_FAILED
         and at(10.5)[4]["inited"] == pax | posn)
    test("amc.gtw silent: err 4, nothing initialised", at(12.5)[4]["err"] == C.ERR_AMCGTW and at(12.5)[4]["inited"] == 0)
    test("another TOML: refused", at(13.5)[4]["result"] == C.RES_REJECTED and at(13.5)[4]["reason"] == C.WHY_SPEC)
    test("init of a sensor_position that does not move: err 7, no rpc", at(15.4)[4]["err"] == C.ERR_SENSOR_POSITION
         and at(15.4)[4]["inited"] == 0 and ops(14.0, 15.4) == [])
    test("init cube + pax + pos again", at(16.9)[4]["inited"] == allthree)
    test("init motor_macro: OPmotorPIDS then OPmotorSETUP, initialised", ops(17.0, 17.9) == [(C.OP_MOTOR_PIDS, 0), (C.OP_MOTOR_SETUP, 0)]
         and at(17.9)[4]["inited"] & macro and at(17.9)[4]["result"] == C.RES_DONE and at(17.9)[3]["mmcfg"] == list(spec.mmcfg))
    test("motor_macro_velocity 90 deg/s: OPcubeDISABLE, OPmotorMODE speedvoltage, targets of 90 deg/s (in rad/s), no MIT",
         ops(18.0, 18.5) == [(C.OP_CUBE_DISABLE, 0), (C.OP_MOTOR_MODE, 0x0A)] and at(18.5)[3]["motor_macro_valid"]
         and at(18.5)[3]["motor_macro_mode"] == 0x0A and at(18.5)[3]["motor_macro_target"] == 90.0 * D
         and at(18.5)[2]["motor_macro_mode"] == 0x0A and not at(18.5)[3]["motor_cubemars_mit_valid"])
    test("stop: ramp, then idle and OPmotorMODE idle", 0 < at(19.25)[3]["motor_macro_target"] < 90.0 * D and at(19.9)[4]["state"] == C.ST_IDLE
         and not at(19.9)[3]["motor_macro_valid"] and ops(19.0, 19.9) == [(C.OP_MOTOR_MODE, 0)])
    test("motor_macro_current 0.5 A", at(20.3)[3]["motor_macro_mode"] == 0x06 and at(20.3)[3]["motor_macro_target"] == 0.5)
    test("external fault while it runs: idle at once, err 6, OPmotorMODE idle", at(20.6)[4]["state"] == C.ST_IDLE
         and at(20.6)[4]["err"] == C.ERR_MMFAULT and (C.OP_MOTOR_MODE, 0) in ops(20.5, 20.6))
    test("control mode not reached in CHECK_TIME: stop, err 5", at(22.3)[4]["state"] == C.ST_IDLE and at(22.3)[4]["err"] == C.ERR_MOTOR_MACRO)
    test("spark_dual_motor_actuation on the Speedgoat: OPcubeENABLE, OPmotorMODE speedvoltage, running",
         ops(23.0, 23.2) == [(C.OP_CUBE_ENABLE, 0), (C.OP_MOTOR_MODE, 0x0A)] and at(23.2)[4]["state"] == C.ST_RUNNING)
    test("spark_dual_motor_actuation stop: OPmotorMODE idle, OPcubeDISABLE", ops(23.3, 23.5) == [(C.OP_MOTOR_MODE, 0), (C.OP_CUBE_DISABLE, 0)]
         and at(23.45)[4]["state"] == C.ST_IDLE)
    d = Supervisor(spec)                            # the daemon
    r = req_default(spec.max_params)
    r.update(have=True, alive=True, crc_ok=True, action=act["start"], id=1, mode=spec.mode_by_name["spark_dual_motor_actuation"].id)
    _, sd = d.step(fb_default(), r, 0.0)
    test("spark_dual_motor_actuation (Simulink blocks): the daemon refuses it", sd["result"] == C.RES_REJECTED and sd["reason"] == C.WHY_MODE)
    test("a mode of the motor_cubemars w/ the motor_macro initialised: OPcubeENABLE, OPmotorMODE idle",
         ops(23.5, 23.9) == [(C.OP_CUBE_ENABLE, 0), (C.OP_MOTOR_MODE, 0)] and at(23.9)[3]["motor_cubemars_pos"] == 10.0 * D)
    test("idle: all the motors stop (OPcubeDISABLE, OPmotorMODE idle), nothing sent", ops(24.5, 24.9) == [(C.OP_CUBE_DISABLE, 0), (C.OP_MOTOR_MODE, 0)]
         and at(24.9)[4]["state"] == C.ST_RUNNING and not at(24.9)[3]["motor_cubemars_mit_valid"] and not at(24.9)[3]["motor_macro_valid"])
    test("idle: stop at once, nothing more", at(25.1)[4]["state"] == C.ST_IDLE and ops(25.0, 25.5) == [])
    test("deinit motor_macro: OPmotorMODE idle, not initialised", not (at(26.4)[4]["inited"] & macro) and ops(25.5, 26.5) == [(C.OP_MOTOR_MODE, 0)])
    test("EXTFAULT while spark runs: at once extfault, OPgwFAULT 1 (amc.gtw stops both), no setpoint", at(27.9)[4]["state"] == C.ST_RUNNING
         and at(28.001)[4]["state"] == C.ST_EXTFAULT and ops(28.0, 28.1) == [(C.OP_GW_FAULT, 1)] and at(28.05)[2]["gw_extfault"] == 1
         and not at(28.05)[3]["motor_cubemars_mit_valid"] and not at(28.05)[3]["motor_macro_valid"] and at(28.05)[4]["mode"] == C.MODE_NONE)
    test("EXTFAULT: requests refused (wrong state), it stays w/out the panel too", at(28.4)[4]["result"] == C.RES_REJECTED
         and at(28.4)[4]["reason"] == C.WHY_STATE and at(28.9)[4]["state"] == C.ST_EXTFAULT and ops(28.1, 29.0) == [])
    test("EXTFAULT released: OPgwFAULT 0, idle, the devices still initialised", at(29.1)[4]["state"] == C.ST_IDLE
         and at(29.1)[4]["inited"] == allthree | macro and ops(29.0, 29.1) == [(C.OP_GW_FAULT, 0)] and at(29.1)[2]["gw_extfault"] == 0)
    test("EXTFAULT while a cubemars mode runs: OPgwFAULT 1, the motor_cubemars stops; released: OPgwFAULT 0",
         at(29.65)[4]["state"] == C.ST_EXTFAULT and ops(29.6, 29.7) == [(C.OP_GW_FAULT, 1)] and not gtw_cube_run_at(29.65)
         and at(30.1)[4]["state"] == C.ST_IDLE and ops(30.0, 30.1) == [(C.OP_GW_FAULT, 0)])
    test("amc.gtw out of step (it lost the EXTFAULT): OPgwFAULT 1 again after FAULT_SYNC", ops(30.21, 30.69) == []
         and ops(30.69, 30.75) == [(C.OP_GW_FAULT, 1)] and at(30.8)[2]["gw_extfault"] == 1 and at(30.8)[4]["state"] == C.ST_EXTFAULT)
    print(f"{sum(ok)}/{len(ok)} supervisor checks passed")
    return all(ok)


# the columns of the trace: the names are in its second line, test_parity.m finds them by name
REQ_COLS = ["have", "alive", "crc_ok", "cnt", "id", "action", "devices", "mode", "duration", "extfault"]
FB_COLS = ["trx", "has_motor_cubemars_status", "motor_cubemars_status", "motor_cubemars_counter", "sensor_torque_counter", "motor_cubemars_vel", "motor_cubemars_pos",
           "has_motor_macro", "motor_macro_mode", "motor_macro_configured", "motor_macro_faults", "motor_macro_vel",
           "has_sensor_position", "sensor_position_counter", "sensor_position_flags", "gw_extfault"]
CMD_COLS = ["motor_cubemars_mit_valid", "motor_cubemars_kp", "motor_cubemars_kd", "motor_cubemars_pos", "motor_cubemars_vel", "motor_cubemars_torque",
            "motor_macro_valid", "motor_macro_mode", "motor_macro_target", "rpc_go", "rpc_op", "rpc_arg"]
SUP_COLS = ["state", "mode", "inited", "err", "result", "reason", "action", "reqid", "trun"]


def write_trace(trace, path, spec):
    """one line per step: t, the request (w/ params and the config of the motor_macro), the feedback, the command, the status"""
    names = (["t"] + [f"req_{c}" for c in REQ_COLS] + [f"req_params{k + 1}" for k in range(spec.max_params)]
             + [f"req_mmcfg{k + 1}" for k in range(14)] + [f"fb_{c}" for c in FB_COLS] + [f"cmd_{c}" for c in CMD_COLS]
             + [f"sup_{c}" for c in SUP_COLS])
    # 6 MB of text, 0.35 MB gzipped; mtime 0: the same trace gives the same bytes (no noise in git)
    with gzip.GzipFile(path, "wb", mtime=0) as gz, io.TextIOWrapper(gz, newline="\n") as f:
        f.write(f"% parity trace of test_sgact_supervisor.py, sgact_panel.toml crc 0x{spec.crc:08X}\n")
        f.write("% cols: " + " ".join(names) + "\n")
        for t, r, fb, c, s in trace:
            row = ([t] + [r[k] for k in REQ_COLS] + list(r["params"]) + list(r["mmcfg"]) + [fb[k] for k in FB_COLS]
                   + [c[k] for k in CMD_COLS] + [s[k] for k in SUP_COLS])
            f.write(" ".join(f"{float(v):.17g}" for v in row) + "\n")


def main():
    ap = argparse.ArgumentParser(description="the supervisor of the daemon against a fake amc.gtw")
    ap.add_argument("--spec", default=None)
    ap.add_argument("--trace", default=None)
    args = ap.parse_args()
    spec = sgact_spec.load(args.spec)
    trace, gtw = run(spec)
    good = check(trace, spec, gtw)
    path = args.trace
    if path is None:
        d = os.path.join(_HERE, "..", "simulink", "tests")
        path = os.path.join(d, "parity_trace.txt.gz") if os.path.isdir(d) else None
    if path:
        write_trace(trace, path, spec)
        print(f"trace written to {os.path.abspath(path)}")
    return 0 if good else 1


if __name__ == "__main__":
    sys.exit(main())

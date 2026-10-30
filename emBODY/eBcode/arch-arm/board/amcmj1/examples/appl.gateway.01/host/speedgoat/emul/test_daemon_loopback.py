#!/usr/bin/env python3
"""
test_daemon_loopback.py - the daemon (sgact_daemon.py) and the client of the panel (sgctl.Client),
all on 127.0.0.1, with a fake amc.gtw that speaks TETHER over UDP (5001 commands, 5002 rpc with ack, 5003 state).
No window, no board. It uses the UDP ports 5001-5003 and 6000-6001 of this PC: close the real tools first.

    py test_daemon_loopback.py
"""

import argparse
import math
import os
import select
import socket
import struct
import sys
import threading
import time

_HERE = os.path.dirname(os.path.abspath(__file__))
_HOST = os.path.dirname(os.path.dirname(_HERE))        # host/
sys.path.insert(0, os.path.join(_HOST, "shared"))      # the codecs shared by the tools
sys.path.insert(0, _HERE)
import amc_gtw as gw            # noqa: E402
import sgact_spec                   # noqa: E402
import sgctl                        # noqa: E402
import sgact_daemon as emu         # noqa: E402
import tethercodec as rc            # noqa: E402


class FakeGtw(threading.Thread):
    """amc.gtw on 127.0.0.1: enable / disable on the rpc socket (ack), MIT -> velocity, a state frame per command frame.
    the motor_macro as amcmj1_motdriver: pids and setup, then a control mode, then the targets of that mode (deg/s)"""

    def __init__(self):
        super().__init__(daemon=True)
        self.codec = rc.Codec(rc.Protocol())
        self.cmd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.cmd.bind(("127.0.0.1", gw.PORT_CMD))
        self.cmd.settimeout(0.05)
        self.rpc = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.rpc.bind(("127.0.0.1", gw.PORT_RPC))
        self.rpc.settimeout(0.01)
        self.cube = self.pax = False
        self.vel = self.pos = 0.0
        self.cubecnt = self.paxcnt = self.packets = 0
        self.ops, self.running = [], True
        self.mm_conf, self.mm_mode, self.mm_vel, self.mm_targets, self.mm_args = 0, 0, 0.0, [], {}
        self.gwfault = 0

    def run(self):
        while self.running:
            try:
                ready, _, _ = select.select([self.rpc], [], [], 0)      # no wait: the command frames must not queue up
                if not ready:
                    raise socket.timeout
                data, addr = self.rpc.recvfrom(2048)
                fr = self.codec.decode_frame(data)
                for r in fr.rops:
                    if r.cmd == rc.CMD.run:
                        self.ops.append(r.id)
                        self.cube = True if r.id == gw.OP_CUBE_ENABLE else False if r.id == gw.OP_CUBE_DISABLE else self.cube
                        self.pax = True if r.id == gw.OP_PAX_ENABLE else False if r.id == gw.OP_PAX_DISABLE else self.pax
                        good = True
                        self.mm_args[r.id] = bytes(r.data[:r.size])
                        if r.id == gw.OP_MOTOR_PIDS:
                            self.mm_conf |= 1
                        elif r.id == gw.OP_MOTOR_SETUP:
                            self.mm_conf |= 2
                        elif r.id == gw.OP_GW_FAULT:
                            self.gwfault = r.data[0]
                            if self.gwfault:
                                self.cube, self.mm_mode = False, gw.CM_IDLE
                        elif r.id == gw.OP_MOTOR_MODE:
                            good = r.data[0] == gw.CM_IDLE or self.mm_conf == 3
                            self.mm_mode = r.data[0] if good else self.mm_mode
                        cmd = rc.CMD.ack if good else rc.CMD.nak
                        ack = rc.Rop(int(cmd), 0, r.id, 0, int(rc.CMD.run), b"")
                        self.rpc.sendto(self.codec.build_frame([ack]), addr)
            except socket.timeout:
                pass
            try:
                data, addr = self.cmd.recvfrom(2048)
            except socket.timeout:
                continue
            self.packets += 1
            fr = self.codec.decode_frame(data)
            mit = False
            for r in fr.rops:
                if r.cmd == rc.CMD.set and r.id == gw.ID_CUBE_MIT and self.cube:
                    kp, kd, pos, vel, torque, seq = struct.unpack(gw.MIT_FMT, r.data[:24])
                    self.vel, mit = vel, True
                    self.pos += vel * 0.01
                    self.cubecnt += 1
                if r.cmd == rc.CMD.set and r.id == gw.ID_MOTOR_TARGET:
                    value, mode, seq = struct.unpack(gw.TARGET_FMT, r.data[:12])
                    if mode == self.mm_mode and mode != gw.CM_IDLE:
                        self.mm_targets.append((value, mode))
                        self.mm_vel = value if mode == gw.CM_SPEEDVOLTAGE else self.mm_vel
            if self.mm_mode == gw.CM_IDLE:
                self.mm_vel = 0.0
            if self.pax:
                self.paxcnt += 1
            state = 2 if (self.cube and mit) else 1 if self.cube else 0
            rops = [rc.make_rop(rc.CMD.sig, gw.ID_CUBE_STATUS, struct.pack(gw.STATUS_FMT, state, 0, 0, self.cubecnt, 0)),
                    rc.make_rop(rc.CMD.sig, gw.ID_CUBE_STATE, struct.pack(gw.STATE_FMT, self.pos, self.vel, 0.1 * self.vel, 30, 0, self.cubecnt)),
                    rc.make_rop(rc.CMD.sig, gw.ID_PAX_TORQUE, struct.pack(gw.TORQUE_FMT, 0.5 * self.pax, -0.5 * self.pax, 0.25 * self.pax, self.paxcnt)),
                    rc.make_rop(rc.CMD.sig, gw.ID_MOTOR_STATE, struct.pack(gw.MOTOR_FMT, 0.01 * self.mm_vel, self.mm_vel, 0.0, 0.0, 0,
                                                                           self.packets, self.mm_mode, self.mm_conf, 1, self.gwfault)),
                    rc.make_rop(rc.CMD.sig, gw.ID_GW_STATS, struct.pack(gw.STATS_FMT, 0, self.packets, 0, 0, 0, 0, 0, 0, 0)),
                    rc.make_rop(rc.CMD.sig, gw.ID_POS_STATE, struct.pack(gw.POS_FMT, 12.5, self.packets, 0, 0,
                                                                         gw.POS_VALID | gw.POS_EMULATED, 0))]
            self.cmd.sendto(self.codec.build_frame(rops), (addr[0], gw.PORT_STATE))


def wait_for(client, cond, timeout=3.0):
    t0 = time.monotonic()
    while time.monotonic() - t0 < timeout:
        st, age = client.status()
        if st is not None and cond(st):
            return st
        time.sleep(0.02)
    return client.status()[0]


def main():
    spec = sgact_spec.load(None)
    fake = FakeGtw()
    fake.start()
    args = argparse.Namespace(board="127.0.0.1", cmd_port=gw.PORT_CMD, rpc_port=gw.PORT_RPC, state_port=gw.PORT_STATE,
                              period_ms=10.0, seq=1, ctl_port=sgctl.PORT_CTL, ctl_bind="127.0.0.1", spec=None)
    stop = threading.Event()
    lines = []
    th = threading.Thread(target=emu.run_daemon, kwargs=dict(args=args, out=lines.append, stop=stop), daemon=True)
    th.start()
    time.sleep(0.3)
    client = sgctl.Client(spec.crc, "127.0.0.1")
    a, m = spec.actions, spec.mode_by_name
    both = spec.mask(["motor_cubemars", "sensor_torque"])
    three = spec.mask(["motor_cubemars", "sensor_torque", "sensor_position"])
    ok = []

    def test(name, cond):
        ok.append(bool(cond))
        print(f"  {'PASS' if cond else 'FAIL'}  {name}")

    st = wait_for(client, lambda s: True)
    test("STATUS v4 arrives, the same sgact_panel.toml", st is not None and st.crc == spec.crc and st.state == 0)
    test("STATUS: the sensor_position (12.5 deg, emulated)", abs(st.sensor_position_pos - math.radians(12.5)) < 1e-6
         and st.sensor_position_flags == gw.POS_VALID | gw.POS_EMULATED)
    rid = client.request(a["init"], devices=three)
    st = wait_for(client, lambda s: s.reqid == rid and s.result == 3)
    test("init motor_cubemars + sensor_torque + sensor_position: done", st.result == 3 and st.inited == three)
    time.sleep(0.1)
    test("rpc: sensor_torque enable, motor_cubemars enable (acked), then disabled", fake.ops[:3] == [gw.OP_PAX_ENABLE, gw.OP_CUBE_ENABLE,
         gw.OP_CUBE_DISABLE] and any("OPpaxENABLE: ack" in s for s in lines))
    step = m["motor_cubemars_velocity_step"].id
    rid = client.request(a["start"], mode=step, params=[0.0, 60.0, 0.0, 1.0, 0.0, 60.0], duration=0.0)
    st = wait_for(client, lambda s: s.reqid == rid and s.state == 3 and abs(s.motor_cubemars_vel - 1.0) < 1e-6)
    test("start velocity_step 60 deg/s: enabled, running, the motor_cubemars at 1.047 rad/s", st.state == 3
         and abs(st.motor_cubemars_vel - math.radians(60)) < 1e-6 and fake.ops[3] == gw.OP_CUBE_ENABLE)
    client.live(mode=step, params=[0.0, 120.0, 0.0, 1.0, 0.0, 60.0], duration=0.0)
    st = wait_for(client, lambda s: abs(s.motor_cubemars_vel - 2.0) < 1e-6)
    test("live change: 120 deg/s", abs(st.motor_cubemars_vel - math.radians(120)) < 1e-6)
    rid = client.request(a["start"], mode=m["motor_cubemars_position"].id, params=[0.0] * 5, duration=0.0)
    st = wait_for(client, lambda s: s.reqid == rid)
    test("start while running: rejected, wrong state", st.result == 2 and st.reason == 1)
    rid = client.request(a["stop"])
    st = wait_for(client, lambda s: s.reqid == rid and s.state == 0)
    for _ in range(50):                         # the rpc goes in a thread of the daemon: it can arrive after the STATUS
        if fake.ops[-1] == gw.OP_CUBE_DISABLE:
            break
        time.sleep(0.02)
    test("stop: vel 0, idle, OPcubeDISABLE", st.state == 0 and wait_for(client, lambda s: abs(s.motor_cubemars_vel) < 1e-6).motor_cubemars_vel == 0
         and fake.ops[-1] == gw.OP_CUBE_DISABLE)
    rid = client.request(a["deinit"], devices=three)
    st = wait_for(client, lambda s: s.reqid == rid and s.result == 3)
    test("deinit: nothing initialised, rpc motor_cubemars then sensor_torque disable", st.inited == 0
         and fake.ops[-2:] == [gw.OP_CUBE_DISABLE, gw.OP_PAX_DISABLE])
    # the motor_macro
    macro = spec.mask(["motor_macro"])
    client.live(mmcfg=spec.mmcfg)
    rid = client.request(a["init"], devices=macro)
    st = wait_for(client, lambda s: s.reqid == rid and s.result == 3)
    test("init motor_macro: done, configured (pids and setup acked)", st.result == 3 and st.inited == macro and st.motor_macro_configured == 3
         and fake.ops[-2:] == [gw.OP_MOTOR_PIDS, gw.OP_MOTOR_SETUP])
    test("the config of the TOML reaches amc.gtw", fake.mm_args.get(gw.OP_MOTOR_PIDS) == gw.pids_arg(*spec.mmcfg[0:6])
         and fake.mm_args.get(gw.OP_MOTOR_SETUP) == gw.setup_arg(*spec.mmcfg[6:14]))
    rid = client.request(a["start"], mode=m["motor_macro_velocity"].id, params=[90.0], duration=0.0)
    st = wait_for(client, lambda s: s.reqid == rid and s.state == 3 and abs(s.motor_macro_vel - math.radians(90)) < 1e-4)
    test("start motor_macro_velocity 90 deg/s: speedvoltage, targets of 90 deg/s", st.state == 3 and st.motor_macro_mode == gw.CM_SPEEDVOLTAGE
         and abs(fake.mm_targets[-1][0] - 90.0) < 1e-4 and abs(st.motor_macro_vel - math.radians(90)) < 1e-4)
    rid = client.request(a["stop"])
    st = wait_for(client, lambda s: s.reqid == rid and s.state == 0 and s.motor_macro_mode == gw.CM_IDLE)
    test("stop: ramp, then OPmotorMODE idle", st.state == 0 and st.motor_macro_mode == gw.CM_IDLE and fake.ops[-1] == gw.OP_MOTOR_MODE
         and fake.mm_args[gw.OP_MOTOR_MODE] == bytes([gw.CM_IDLE]))
    client.live(extfault=True)
    st = wait_for(client, lambda s: s.state == 5)
    st = wait_for(client, lambda s: s.gw_extfault == 1)
    test("EXTFAULT pressed: state extfault, OPgwFAULT 1, amc.gtw holds it", st.state == 5 and fake.ops[-1] == gw.OP_GW_FAULT
         and st.gw_extfault == 1 and fake.mm_mode == gw.CM_IDLE)
    client.live(extfault=False)
    st = wait_for(client, lambda s: s.state == 0 and s.gw_extfault == 0)
    test("EXTFAULT released: OPgwFAULT 0, idle", st.state == 0 and st.gw_extfault == 0 and fake.gwfault == 0)
    client.close()
    other = sgctl.Client(spec.crc ^ 1, "127.0.0.1")
    rid = other.request(a["init"], devices=both)
    st = wait_for(other, lambda s: s.reqid == rid)
    test("a panel with another sgact_panel.toml: rejected", st.result == 2 and st.reason == 5)
    other.close()
    stop.set()
    th.join(2.0)
    fake.running = False
    print(f"{sum(ok)}/{len(ok)} loopback checks passed")
    return 0 if all(ok) else 1


if __name__ == "__main__":
    sys.exit(main())

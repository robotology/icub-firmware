#!/usr/bin/env python3
"""
sgact_daemon.py - sgact on the PC: it plays the Speedgoat for the panel (sgact_panel.py), with the same supervisor and
modes as the Simulink model (sgact_supervisor.py, sgact_modes.py), from the same sgact_panel.toml.

    sgact_panel.py  --CTL 6000-->     sgact_daemon.py  --commands 5001-->  amc.gtw (amcmj1)
                    <--STATUS 6001--                   --rpc 5002------->
                                                       <--state 5003-----

It streams a command frame every --period-ms all the time (the setpoints of the running mode, else heartbeats) and it
executes the requests of the panel, each one independent: init / deinit of the devices, start / stop of a mode.
Unlike the Speedgoat, it reads the ack / nak of the rpc of amc.gtw and prints them. A STATUS goes to the panel every 100 ms.

    py sgact_daemon.py                                  then, in another window:  py ..\\panel\\sgact_panel.py
    py sgact_daemon.py --board 10.0.1.99 --period-ms 1
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
sys.path.insert(0, _HERE)
import amc_gtw as gw                        # noqa: E402
import sgact_spec                           # noqa: E402
import sgctl                                # noqa: E402
from sgact_supervisor import Supervisor, fb_default, req_default      # noqa: E402
from speedgoat_emulator import Collector, describe                   # noqa: E402


def fb_of(st, trx, nrecv):
    """a gw.State (or None) -> the dict of SGACT_FB that the supervisor reads"""
    fb = fb_default()
    if st is not None:
        fb.update(valid=True, time=st.time, gw=list(st.gw))
        if st.cube_status is not None:
            c = st.cube_status
            fb.update(has_motor_cubemars_status=True, motor_cubemars_status=c.state, motor_cubemars_lastcommand=c.lastcommand,
                      motor_cubemars_errors=c.errors, motor_cubemars_commands=c.commands, motor_cubemars_watchdogs=c.watchdogs)
        if st.cube_state is not None:
            c = st.cube_state
            fb.update(has_motor_cubemars_state=True, motor_cubemars_pos=c.pos, motor_cubemars_vel=c.vel, motor_cubemars_current=c.current,
                      motor_cubemars_temp=c.temp, motor_cubemars_error=c.error, motor_cubemars_counter=c.counter)
        if st.pax_torque is not None:
            p = st.pax_torque
            fb.update(has_sensor_torque=True, sensor_torque_mx=p.mx, sensor_torque_my=p.my, sensor_torque_mz=p.mz, sensor_torque_counter=p.counter)
        if st.motor is not None:
            m = st.motor                                # amc.mot speaks deg and deg/s: the buses speak rad and rad/s
            fb.update(has_motor_macro=True, motor_macro_pos=math.radians(m.position), motor_macro_vel=math.radians(m.velocity),
                      motor_macro_current=m.current, motor_macro_pwm=m.pwm, motor_macro_faults=m.faults,
                      motor_macro_mode=m.controlmode, motor_macro_configured=m.configured, motor_macro_emulated=m.emulated,
                      motor_macro_counter=m.counter, gw_extfault=m.gwfault)
        if st.pos is not None:
            p = st.pos                                  # deg on the wire: rad on the buses
            fb.update(has_sensor_position=True, sensor_position_pos=math.radians(p.position), sensor_position_counter=p.counter,
                      sensor_position_errors=p.errors, sensor_position_flags=p.flags)
    fb.update(trx=trx, nrecv=nrecv)
    return fb


OP_NAMES = {gw.OP_CUBE_ENABLE: "OPcubeENABLE", gw.OP_CUBE_DISABLE: "OPcubeDISABLE", gw.OP_PAX_ENABLE: "OPpaxENABLE",
            gw.OP_PAX_DISABLE: "OPpaxDISABLE", gw.OP_MOTOR_PIDS: "OPmotorPIDS", gw.OP_MOTOR_SETUP: "OPmotorSETUP",
            gw.OP_MOTOR_MODE: "OPmotorMODE", gw.OP_GW_FAULT: "OPgwFAULT"}


def rpc_arg(op, arg, mmcfg):
    """the bytes of the argument of an rpc of the supervisor (the same as amc.gtw TX of the Speedgoat)"""
    if op == gw.OP_MOTOR_PIDS:
        return gw.pids_arg(*mmcfg[0:6])
    if op == gw.OP_MOTOR_SETUP:
        return gw.setup_arg(*mmcfg[6:14])
    if op == gw.OP_MOTOR_MODE:
        return gw.mode_arg(arg)
    if op == gw.OP_GW_FAULT:
        return gw.fault_arg(arg)
    return b""


class Daemon:
    """sgact on the PC for the panel (sgctl.py v3), with the same supervisor and modes as the Speedgoat
    (sgact_supervisor.py, sgact_modes.py, from sgact_panel.toml). One thread streams a command frame every period
    (the MIT of the supervisor or a heartbeat), one receives the CTL, the rpc run in short threads (one at a time) so that
    the stream never stops: unlike the Speedgoat, here the ack / nak of amc.gtw are read and printed"""

    def __init__(self, args, link, out=print):
        self.args, self.link, self.out = args, link, out
        self.spec = sgact_spec.load(args.spec)
        self.sup = Supervisor(self.spec)
        self.lock, self.rpclock = threading.Lock(), threading.Lock()
        self.ctl, self.rx_at, self.peer, self.warned = None, None, None, 0.0
        self.nsent, self.seq, self.running = 0, args.seq, True
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind((args.ctl_bind, args.ctl_port))
        self.sock.settimeout(0.1)
        self.coll = Collector(link)

    def _rx(self):
        while self.running:
            try:
                data, addr = self.sock.recvfrom(256)
            except socket.timeout:
                continue
            except ConnectionResetError:
                continue                        # Windows: the panel went away (ICMP port unreachable of our last STATUS)
            except OSError:
                return
            c = sgctl.unpack_ctl(data)
            if c is None:
                continue
            now = time.monotonic()
            with self.lock:
                free = self.peer is None or self.rx_at is None or now - self.rx_at > self.spec.keepalive
                if addr != self.peer and not free:
                    if now - self.warned > 5.0:     # a second panel while the first one is alive: only the first one commands
                        self.warned = now
                        self.out(f"ignored CTL from {addr[0]}:{addr[1]}: the panel {self.peer[0]}:{self.peer[1]} is in control")
                    continue
                if free:
                    self.out(f"panel {addr[0]}:{addr[1]} in control" + ("" if c.crc == self.spec.crc else
                             f"  WARNING: it uses another sgact_panel.toml (0x{c.crc:08X}, here 0x{self.spec.crc:08X})"))
                self.ctl, self.rx_at, self.peer = c, now, addr

    def _req(self, now):
        req = req_default(self.spec.max_params)
        with self.lock:
            c, rx_at = self.ctl, self.rx_at
        if c is not None:
            req.update(have=True, alive=(now - rx_at) < self.spec.keepalive, crc_ok=c.crc == self.spec.crc, cnt=c.cnt,
                       id=c.reqid, action=c.action, devices=c.devices, mode=c.mode, duration=c.duration,
                       params=list(c.params)[:self.spec.max_params], mmcfg=list(c.mmcfg), extfault=c.extfault)
        return req

    def _rpc(self, op, arg=b""):
        def body():
            with self.rpclock:
                ok, why = self.link.rpc_run(op, arg=arg)
            name = OP_NAMES.get(op, hex(op)) + (f" {sgctl.CONTROLMODE_NAMES.get(arg[0], arg[0])}" if op == gw.OP_MOTOR_MODE
                                                  else f" {arg[0]}" if op == gw.OP_GW_FAULT else "")
            self.out(f"rpc {name}: {'ack' if ok else 'nak / no reply: ' + str(why)}")
        threading.Thread(target=body, daemon=True).start()

    def _status(self, s, fb):
        return sgctl.pack_status(s["state"], s["mode"], s["inited"], s["err"], s["result"], s["reason"], s["action"],
                                 fb["motor_cubemars_status"] if fb["has_motor_cubemars_status"] else 0, s["reqid"], s["cnt"], self.spec.crc,
                                 s["trun"], fb["motor_cubemars_pos"], fb["motor_cubemars_vel"], fb["motor_cubemars_current"], fb["sensor_torque_mx"],
                                 fb["sensor_torque_my"], fb["sensor_torque_mz"], fb["motor_macro_pos"], fb["motor_macro_vel"],
                                 fb["motor_macro_current"], fb["motor_macro_mode"], fb["motor_macro_configured"],
                                 fb["motor_macro_emulated"], self.nsent, fb["nrecv"], fb["gw"][6], fb["gw"][3],
                                 fb["motor_macro_pwm"], fb["motor_macro_faults"], fb["sensor_position_pos"],
                                 fb["sensor_position_counter"], fb["sensor_position_flags"], fb["gw_extfault"])

    def run(self, stop=None):
        a = self.args
        period = a.period_ms / 1000.0
        self.coll.start()
        threading.Thread(target=self._rx, daemon=True).start()
        self.out(f"daemon: sgact on the PC, waiting for the panel on UDP {a.ctl_port}, amc.gtw {a.board}, "
                 f"{os.path.basename(self.spec.path)} 0x{self.spec.crc:08X} (Ctrl-C to quit)")
        start = time.monotonic()
        n, nextstat, nextlog, laststate = 0, start, start + 5.0, None
        try:
            while self.running and not (stop is not None and stop.is_set()):
                now = time.monotonic()
                with self.coll.lock:
                    st, last_at, nrecv = self.coll.last, self.coll.last_at, self.coll.received
                fb = fb_of(st, (last_at - start) if last_at is not None else 0.0, nrecv)
                req = self._req(now)
                cmd, s = self.sup.step(fb, req, now - start)
                if cmd["rpc_go"]:
                    self._rpc(cmd["rpc_op"], rpc_arg(cmd["rpc_op"], cmd["rpc_arg"], cmd["mmcfg"]))
                mit = target = None
                if cmd["motor_cubemars_mit_valid"]:
                    with self.coll.lock:
                        self.coll.sent[self.seq & 0xFFFFFFFF] = now
                        if len(self.coll.sent) > 2000:
                            self.coll.sent.pop(next(iter(self.coll.sent)))
                    mit = (cmd["motor_cubemars_kp"], cmd["motor_cubemars_kd"], cmd["motor_cubemars_pos"], cmd["motor_cubemars_vel"], cmd["motor_cubemars_torque"], self.seq)
                if cmd["motor_macro_valid"]:
                    value = cmd["motor_macro_target"]
                    if cmd["motor_macro_mode"] == gw.CM_SPEEDVOLTAGE:
                        value = math.degrees(value)         # rad/s -> deg/s: the unit of amc.mot
                    target = (value, cmd["motor_macro_mode"], self.seq)
                if mit is not None or target is not None:
                    self.link.send_command(mit, target)
                    self.seq += 1
                else:
                    self.link.send_empty()
                self.nsent += 1
                key = (s["state"], s["mode"], s["inited"], s["err"], s["result"], s["reqid"])
                if key != laststate:
                    laststate = key
                    mname = self.spec.mode_by_id[s["mode"]].name if s["mode"] in self.spec.mode_by_id else "-"
                    self.out(f"#{s['reqid']} {self.spec.action_names.get(s['action'], '-')}: {sgctl.RESULT_NAMES[s['result']]}"
                             f"{' (' + sgctl.REASON_NAMES[s['reason']] + ')' if s['reason'] else ''}   "
                             f"state {sgctl.STATE_NAMES[s['state']]}  mode {mname}  inited {self.spec.names_of(s['inited'])}"
                             f"{'  ERROR: ' + sgctl.ERR_NAMES[s['err']] if s['err'] else ''}")
                if now >= nextstat:
                    nextstat += 0.1
                    with self.lock:
                        peer = self.peer
                    if peer is not None and s["have"]:
                        try:
                            self.sock.sendto(self._status(s, fb), peer)
                        except OSError:
                            pass
                if now >= nextlog:
                    nextlog += 5.0
                    with self.coll.lock:
                        self.out(f"t {now - start:6.1f}s  sent {self.nsent}  state {self.coll.received}  " + describe(self.coll.last))
                n += 1
                wait = start + n * period - time.monotonic()
                if wait > 0:
                    time.sleep(wait)
                elif wait < -1.0:
                    n = int((time.monotonic() - start) / period)       # Python fell behind: no burst to catch up
        finally:
            inited = self.sup.inited | self.sup.pending
            for dev, op, arg in (("motor_cubemars", gw.OP_CUBE_DISABLE, b""), ("sensor_torque", gw.OP_PAX_DISABLE, b""),
                                 ("motor_macro", gw.OP_MOTOR_MODE, gw.mode_arg(gw.CM_IDLE))):
                if inited & (1 << self.spec.devices[dev]):
                    with self.rpclock:
                        ok, why = self.link.rpc_run(op, arg=arg)
                    self.out(f"rpc {OP_NAMES[op]} at exit: {'ack' if ok else 'nak / no reply: ' + str(why)}")
            self.running = False
            self.coll.running = False
            self.sock.close()


def run_daemon(args, link=None, out=print, stop=None):
    link = link or gw.Gateway(args.board, args.cmd_port, args.rpc_port, args.state_port)
    d = Daemon(args, link, out)
    d.run(stop)
    return d




def main():
    ap = argparse.ArgumentParser(description="sgact on the PC, for the panel: see the top of this file")
    ap.add_argument("--board", default="10.0.1.99", help="IP of amc.gtw")
    ap.add_argument("--cmd-port", type=int, default=gw.PORT_CMD)
    ap.add_argument("--rpc-port", type=int, default=gw.PORT_RPC)
    ap.add_argument("--state-port", type=int, default=gw.PORT_STATE, help="where this PC listens: the board sends the state here")
    ap.add_argument("--period-ms", type=float, default=10.0, help="one command frame every period: 10 ms, or 1 ms")
    ap.add_argument("--seq", type=int, default=1, help="first seq")
    ap.add_argument("--ctl-port", type=int, default=sgctl.PORT_CTL, help="UDP port of the CTL of the panel")
    ap.add_argument("--ctl-bind", default="", help="local address of the CTL socket, all by default")
    ap.add_argument("--spec", default=None, help="sgact_panel.toml. default: $SGACT_SPEC, then host/speedgoat/sgact_panel.toml")
    args = ap.parse_args()
    try:
        run_daemon(args)
    except sgact_spec.SpecError as e:
        print(f"[error] {e}")
        return 1
    except KeyboardInterrupt:
        print("stopped: the board disables the devices when their watchdogs expire")
    return 0


if __name__ == "__main__":
    sys.exit(main())

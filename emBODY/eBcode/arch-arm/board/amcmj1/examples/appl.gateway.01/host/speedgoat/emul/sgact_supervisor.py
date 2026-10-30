"""
sgact_supervisor - the Controller of sgact for the daemon (sgact_daemon.py): the same logic as the Controller of
sgact.slx on the Speedgoat (host/speedgoat/simulink/matlab: sgact_sup_a.m, the standard modes, sgact_sup_b.m, the unit
delay of done; sgact_controller.m is the same in one call), step by step. Keep them aligned (host/speedgoat/simulink/tests checks it).

    sup = Supervisor(spec)                       # spec: sgact_spec.load()
    cmd, status = sup.step(fb, req, t)           # fb, req: dicts with the fields of SGACT_FB, SGACT_REQ; t [s]

requests (each one independent; one is executed when req['id'] changes):
  init(devices)    from idle: enable the devices not yet initialised (sensor_torque, then motor_cubemars, then the motor_macro, which
                   gets its config: OPmotorPIDS, OPmotorSETUP), then check them on the state frame (sensor_torque counter
                   moves, motor_cubemars status 1 or 2, motor_macro configured, sensor_position valid and its counter moves).
                   the motor_cubemars is then disabled again (OPcubeDISABLE): it is enabled only while one of its modes runs
  deinit(devices)  from idle: disable them (motor_cubemars, then sensor_torque, then the motor_macro goes idle)
  start(mode, params, duration)  from idle, if the devices of the mode are initialised and the params are in range.
                   a mode that needs the motor_cubemars enables it (OPcubeENABLE), any other disables it if it is
                   initialised (OPcubeDISABLE). a mode w/ a controlmode then asks it to the motor_macro (OPmotorMODE), any
                   other puts the motor_macro in idle, if it is initialised
  stop             from running: the mode does its own exit, then idle (the motor_macro idle, the motor_cubemars disabled)
EXTFAULT of the requester (sticky in the panel): at once OPgwFAULT 1 (amc.gtw disables the motor_cubemars, idles the
           motor_macro and refuses both until OPgwFAULT 0), no setpoint, the rpc not yet sent dropped; state extfault, every
           request refused; released: OPgwFAULT 0, idle (the devices stay initialised). amc.gtw out of step (fb gw_extfault):
           OPgwFAULT again every FAULT_SYNC s
by itself: requester silent while running -> stop, err 3; duration -> stop; amc.gtw silent -> err 4, all disabled;
           the motor_macro not in the control mode of the mode after CHECK_TIME -> stop, err 5;
           a fault of the motor_macro (external fault, hwfault) while its mode runs -> idle at once, err 6
the gate: a MIT leaves only if the motor_cubemars is initialised and a mode runs or stops;
          a setpoint of the motor_macro only while a mode w/ a controlmode runs or stops
"""

import sgact_modes


class C:
    """as host/speedgoat/simulink/matlab/sgact_const.m"""
    OP_CUBE_ENABLE, OP_CUBE_DISABLE, OP_PAX_ENABLE, OP_PAX_DISABLE = 0xD1, 0xD2, 0xD4, 0xD5
    OP_MOTOR_PIDS, OP_MOTOR_SETUP, OP_MOTOR_MODE, OP_GW_FAULT = 0xD6, 0xD7, 0xD8, 0xD9
    FAULT_SYNC = 0.5
    CM_IDLE, CM_SPEEDVOLTAGE, CM_HWFAULT = 0x00, 0x0A, 0xA0
    FAULT_EXTERNAL = 128
    MM_CONFIGURED = 3
    RPC_QUEUE = 8
    RPC_GAP = 0.02
    ENABLE_SETTLE = 0.05
    CHECK_ENABLE = True
    CHECK_TIME = 1.0
    AMCGTW_TIMEOUT = 1.0
    STOP_MAX = 5.0
    MODE_NONE = 255
    ST_IDLE, ST_INITIALISING, ST_DEINITIALISING, ST_RUNNING, ST_STOPPING, ST_EXTFAULT = 0, 1, 2, 3, 4, 5
    RES_NONE, RES_ACCEPTED, RES_REJECTED, RES_DONE, RES_FAILED = 0, 1, 2, 3, 4
    WHY_STATE, WHY_DEVICES, WHY_PARAMS, WHY_MODE, WHY_SPEC, WHY_ACTION = 1, 2, 3, 4, 5, 6
    ERR_MOTOR_CUBEMARS, ERR_SENSOR_TORQUE, ERR_KEEPALIVE, ERR_AMCGTW, ERR_MOTOR_MACRO, ERR_MMFAULT = 1, 2, 3, 4, 5, 6
    ERR_SENSOR_POSITION = 7
    POS_VALID = 1


def cmd_default():
    return dict(motor_cubemars_mit_valid=False, motor_cubemars_kp=0.0, motor_cubemars_kd=0.0, motor_cubemars_pos=0.0, motor_cubemars_vel=0.0,
                motor_cubemars_torque=0.0, motor_macro_valid=False, motor_macro_mode=0, motor_macro_target=0.0,
                rpc_go=False, rpc_op=0, rpc_arg=0, mmcfg=[0.0] * 14)


def fb_default():
    return dict(valid=False, time=0, has_motor_cubemars_status=False, motor_cubemars_status=0, motor_cubemars_lastcommand=0, motor_cubemars_errors=0,
                motor_cubemars_commands=0, motor_cubemars_watchdogs=0, has_motor_cubemars_state=False, motor_cubemars_pos=0.0, motor_cubemars_vel=0.0,
                motor_cubemars_current=0.0, motor_cubemars_temp=0, motor_cubemars_error=0, motor_cubemars_counter=0, has_sensor_torque=False,
                sensor_torque_mx=0.0, sensor_torque_my=0.0, sensor_torque_mz=0.0, sensor_torque_counter=0, has_motor_macro=False, motor_macro_pos=0.0,
                motor_macro_vel=0.0, motor_macro_current=0.0, motor_macro_pwm=0.0, motor_macro_faults=0, motor_macro_mode=0,
                motor_macro_configured=0, motor_macro_emulated=0, motor_macro_counter=0, gw_extfault=0, has_sensor_position=False,
                sensor_position_pos=0.0, sensor_position_counter=0, sensor_position_errors=0, sensor_position_flags=0,
                gw=[0] * 9, fresh=False, nrecv=0, trx=0.0)


def run_default(max_params=8):
    """as sgact_run_default.m: which mode runs"""
    return dict(mode=255, phase=0, first=False, t=0.0, trun=0.0, p=[0.0] * max_params, inited=0, mmode=0)


def sp_default():
    """as sgact_sp_default.m: what a mode gives"""
    return dict(valid=False, kp=0.0, kd=0.0, pos=0.0, vel=0.0, torque=0.0, mmtarget=0.0, done=False)


def req_default(max_params=8):
    return dict(have=False, alive=False, crc_ok=False, cnt=0, id=0, action=0, devices=0, mode=0, duration=0.0,
                params=[0.0] * max_params, mmcfg=[0.0] * 14, extfault=False)


class Supervisor:
    def __init__(self, spec, speedgoat=False):
        """speedgoat: play the Speedgoat (for the parity test): a mode w/ impl = simulink is accepted and does what the
        subsystem made by sgact_sync_modes does (nothing, done at once), as sgact_controller.m. the daemon refuses it"""
        self.s = spec
        self.speedgoat = speedgoat
        self.dev_cube = 1 << spec.devices["motor_cubemars"]
        self.dev_pax = 1 << spec.devices["sensor_torque"]
        self.dev_macro = 1 << spec.devices["motor_macro"]
        self.dev_pos = 1 << spec.devices["sensor_position"]
        self.dev_all = self.dev_cube | self.dev_pax | self.dev_macro | self.dev_pos
        self.act = spec.actions
        self.state, self.inited, self.err, self.result, self.reason, self.lastaction, self.reqseen = C.ST_IDLE, 0, 0, C.RES_NONE, 0, 0, -1
        self.mode, self.params, self.duration = C.MODE_NONE, [0.0] * spec.max_params, 0.0
        self.t0 = self.trun0 = self.tref = 0.0
        self.rq, self.ri, self.trpc, self.tdone = [], 0, -1e9, -1.0
        self.pending, self.pax0, self.pos0, self.mneeds = 0, 0, 0, 0
        self.tsync = -1e9
        self.mmcfg, self.mmode, self.lastphase = [0.0] * 14, C.CM_IDLE, 0
        self.donez = False                                      # the unit delay of done
        sgact_modes.reset()

    # ---- helpers
    def _params_ok(self, mode, p):
        m = self.s.mode_by_id.get(mode)
        if m is None:
            return False
        return all(pr.min <= p[k] <= pr.max for k, pr in enumerate(m.params))      # a NaN fails too

    def _queue(self, mask, order, append=False):
        """order: ((dev, op, arg), ...): the ops of the devices of mask, in that order. append: after the ops not yet sent
        (else they are dropped). at most RPC_QUEUE ops wait"""
        new = [(op, arg) for dev, op, arg in order if mask & dev]
        self.rq = (self.rq[self.ri:] if append else []) + new
        self.rq = self.rq[:C.RPC_QUEUE]
        self.ri = 0

    # ---- one step of the Controller, as the model: SUP-A, USER CONTROL (the modes), SUP-B, the unit delay of done
    def step(self, fb, req, t):
        run, cmda, sup = self.step_a(fb, req, self.donez, t)
        sp = self.user_control(run, fb)
        cmd = self.step_b(cmda, sup, run, sp)
        self.donez = sp["done"]
        return cmd, sup

    def user_control(self, run, fb):
        """the modes: the standard ones (sgact_modes.py); a mode w/ impl = simulink or speedgoat_only does nothing here
        (its start is refused anyway)"""
        sp = sp_default()
        m = self.s.mode_by_id.get(run["mode"])
        fn = getattr(sgact_modes, f"mode_{m.name}", None) if m else None
        if run["phase"] > 0 and fn is not None and m.impl == "matlab":
            valid, v, done = fn(run["phase"], run["p"], run["t"], fb)
            sp.update(valid=bool(valid), kp=v[0], kd=v[1], pos=v[2], vel=v[3], torque=v[4], mmtarget=v[5], done=bool(done))
        else:
            sp["done"] = True
        return sp

    def step_b(self, cmda, sup, run, sp):
        """SUP-B, the gate (sgact_sup_b.m): the setpoints of the mode that can leave"""
        cmd = dict(cmda)
        active = run["phase"] > 0
        if sup["inited"] & self.dev_cube and active and sp["valid"]:
            cmd.update(motor_cubemars_mit_valid=True, motor_cubemars_kp=sp["kp"], motor_cubemars_kd=sp["kd"], motor_cubemars_pos=sp["pos"],
                       motor_cubemars_vel=sp["vel"], motor_cubemars_torque=sp["torque"])
        if sup["inited"] & self.dev_macro and active and run["mmode"] != 0:
            cmd.update(motor_macro_valid=True, motor_macro_mode=run["mmode"], motor_macro_target=sp["mmtarget"])
        return cmd

    def step_a(self, fb, req, done, t):
        """SUP-A (sgact_sup_a.m): requests, watch, state machine, rpc. done: the one of the previous step"""
        s, a = self.s, self.act
        enable_order = ((self.dev_pax, C.OP_PAX_ENABLE, 0), (self.dev_cube, C.OP_CUBE_ENABLE, 0),
                        (self.dev_macro, C.OP_MOTOR_PIDS, 0), (self.dev_macro, C.OP_MOTOR_SETUP, 0))
        disable_order = ((self.dev_cube, C.OP_CUBE_DISABLE, 0), (self.dev_pax, C.OP_PAX_DISABLE, 0),
                         (self.dev_macro, C.OP_MOTOR_MODE, C.CM_IDLE))
        idle_order = ((self.dev_macro, C.OP_MOTOR_MODE, C.CM_IDLE),)
        cubeoff = ((self.dev_cube, C.OP_CUBE_DISABLE, 0),)
        cubeon = ((self.dev_cube, C.OP_CUBE_ENABLE, 0),)
        faulton, faultoff = ((1, C.OP_GW_FAULT, 1),), ((1, C.OP_GW_FAULT, 0),)

        # the EXTFAULT of the requester, before anything else (its crc does not matter)
        if req["have"] and req["extfault"]:
            if self.state != C.ST_EXTFAULT:
                self._queue(1, faulton)                     # amc.gtw stops both motors
                self.tsync = t
                if self.result == C.RES_ACCEPTED:
                    self.result = C.RES_FAILED
                self.pending, self.mode, self.mmode, self.mneeds, self.tref = 0, C.MODE_NONE, C.CM_IDLE, 0, t
                self.state = C.ST_EXTFAULT
        elif self.state == C.ST_EXTFAULT:
            self.state = C.ST_IDLE                          # released
            self._queue(1, faultoff, True)
            self.tsync = t
        # amc.gtw must hold the EXTFAULT exactly while sgact is in extfault
        if fb["has_motor_macro"] and self.ri >= len(self.rq) and t - self.tsync >= C.FAULT_SYNC and \
                (fb["gw_extfault"] != 0) != (self.state == C.ST_EXTFAULT):
            self._queue(1, faulton if self.state == C.ST_EXTFAULT else faultoff, True)
            self.tsync = t

        # a new request
        if req["have"] and req["id"] != self.reqseen:
            self.reqseen, self.lastaction, self.reason = req["id"], req["action"], 0
            action = req["action"]
            if not req["crc_ok"]:
                self.result, self.reason = C.RES_REJECTED, C.WHY_SPEC
            elif action == a["init"]:
                if self.state != C.ST_IDLE:
                    self.result, self.reason = C.RES_REJECTED, C.WHY_STATE
                else:
                    self.err = 0
                    mask = (req["devices"] & self.dev_all) & ~self.inited
                    if mask == 0:
                        self.result = C.RES_DONE
                    else:
                        if mask & self.dev_macro:
                            self.mmcfg = list(req["mmcfg"])     # the config of the motor_macro: the one of this request
                        self._queue(mask, enable_order, True)
                        self.pending, self.tdone, self.tref, self.pax0 = mask, -1.0, t, fb["sensor_torque_counter"]
                        self.pos0 = fb["sensor_position_counter"]
                        self.state, self.result = C.ST_INITIALISING, C.RES_ACCEPTED
            elif action == a["deinit"]:
                if self.state != C.ST_IDLE:
                    self.result, self.reason = C.RES_REJECTED, C.WHY_STATE
                else:
                    self.err = 0
                    mask = req["devices"] & self.inited
                    if mask == 0:
                        self.result = C.RES_DONE
                    else:
                        self._queue(mask, disable_order, True)
                        self.inited &= ~mask
                        self.tref = t
                        self.state, self.result = C.ST_DEINITIALISING, C.RES_ACCEPTED
            elif action == a["start"]:
                m = s.mode_by_id.get(req["mode"])
                if self.state != C.ST_IDLE:
                    self.result, self.reason = C.RES_REJECTED, C.WHY_STATE
                elif m is None or (m.speedgoat_only and not self.speedgoat and getattr(sgact_modes, f"mode_{m.name}", None) is None):
                    self.result, self.reason = C.RES_REJECTED, C.WHY_MODE
                elif (m.needs_mask & self.inited) != m.needs_mask:
                    self.result, self.reason = C.RES_REJECTED, C.WHY_DEVICES
                elif not self._params_ok(req["mode"], req["params"]):
                    self.result, self.reason = C.RES_REJECTED, C.WHY_PARAMS
                else:
                    self.err = 0
                    self.mode, self.params, self.duration = req["mode"], s.to_si(req["mode"], req["params"]), req["duration"]
                    self.state, self.t0, self.trun0, self.tref = C.ST_RUNNING, t, t, t
                    self.result = C.RES_DONE
                    self.mmode, self.mneeds = m.controlmode, m.needs_mask
                    if self.mneeds & self.dev_cube and m.cube_enable:
                        self._queue(self.dev_cube, cubeon, True)            # the motor_cubemars runs only w/ its modes
                    elif self.inited & self.dev_cube:
                        self._queue(self.dev_cube, cubeoff, True)           # e.g. a mode of the motor_macro: it must not move
                    if self.mmode != C.CM_IDLE:
                        self._queue(self.dev_macro, ((self.dev_macro, C.OP_MOTOR_MODE, self.mmode),), True)
                    elif self.inited & self.dev_macro:
                        self._queue(self.dev_macro, idle_order, True)       # a mode w/out a controlmode: the motor_macro stays still
            elif action == a["stop"]:
                if self.state != C.ST_RUNNING:
                    self.result, self.reason = C.RES_REJECTED, C.WHY_STATE
                else:
                    self.state, self.t0, self.result = C.ST_STOPPING, t, C.RES_ACCEPTED
            else:
                self.result, self.reason = C.RES_REJECTED, C.WHY_ACTION

        # the parameters of the running mode follow the requester
        if self.state == C.ST_RUNNING and req["have"] and req["crc_ok"] and req["mode"] == self.mode:
            if self._params_ok(self.mode, req["params"]):
                self.params, self.duration = self.s.to_si(self.mode, req["params"]), req["duration"]

        # watch: amc.gtw, the requester, the duration
        if (self.inited != 0 or self.state != C.ST_IDLE) and t - max(fb["trx"], self.tref) > C.AMCGTW_TIMEOUT:
            self.err = C.ERR_AMCGTW
            if self.result == C.RES_ACCEPTED:
                self.result = C.RES_FAILED
            self._queue(self.inited | self.pending, disable_order)
            self.inited = self.pending = 0
            self.mode, self.tref, self.mmode = C.MODE_NONE, t, C.CM_IDLE
            if self.state != C.ST_EXTFAULT:
                self.state = C.ST_DEINITIALISING           # (extfault stays until it is released)
        elif self.state in (C.ST_RUNNING, C.ST_STOPPING) and self.mmode != C.CM_IDLE and fb["has_motor_macro"] and \
                ((fb["motor_macro_faults"] & C.FAULT_EXTERNAL) or fb["motor_macro_mode"] == C.CM_HWFAULT):
            self.err = C.ERR_MMFAULT                        # a fault of the motor_macro: no exit of the mode, idle at once
            self._queue(self.dev_macro, idle_order, True)
            self._queue(self.mneeds & self.dev_cube, cubeoff, True)
            self.state, self.mode, self.mmode = C.ST_IDLE, C.MODE_NONE, C.CM_IDLE
            if self.result == C.RES_ACCEPTED:
                self.result = C.RES_FAILED
        elif self.state == C.ST_RUNNING and self.mmode != C.CM_IDLE and t - self.trun0 >= C.CHECK_TIME and \
                fb["motor_macro_mode"] != self.mmode:
            self.err = C.ERR_MOTOR_MACRO                     # the motor_macro did not take the control mode
            self.state, self.t0 = C.ST_STOPPING, t
        elif self.state == C.ST_RUNNING and not req["alive"]:
            self.err = C.ERR_KEEPALIVE
            self.state, self.t0 = C.ST_STOPPING, t
        elif self.state == C.ST_RUNNING and self.duration > 0 and t - self.trun0 >= self.duration:
            self.state, self.t0 = C.ST_STOPPING, t

        # the progress of the state
        if self.state == C.ST_INITIALISING:
            if self.ri >= len(self.rq):
                if self.tdone < 0:
                    self.tdone = t
                if t - self.tdone >= C.ENABLE_SETTLE:
                    okm = self.pending & self.dev_macro
                    if self.pending & self.dev_cube and (not C.CHECK_ENABLE or (fb["has_motor_cubemars_status"] and fb["motor_cubemars_status"] in (1, 2))):
                        okm |= self.dev_cube
                    if self.pending & self.dev_pax and (not C.CHECK_ENABLE or fb["sensor_torque_counter"] != self.pax0):
                        okm |= self.dev_pax
                    if self.pending & self.dev_pos and (not C.CHECK_ENABLE or (fb["has_sensor_position"] and
                            fb["sensor_position_flags"] & C.POS_VALID and fb["sensor_position_counter"] != self.pos0)):
                        okm |= self.dev_pos
                    if self.pending & self.dev_macro and C.CHECK_ENABLE and \
                            not (fb["has_motor_macro"] and fb["motor_macro_configured"] == C.MM_CONFIGURED):
                        okm &= ~self.dev_macro
                    if okm == self.pending:
                        self.inited |= self.pending
                        self._queue(self.pending & self.dev_cube, cubeoff, True)    # checked: off until one of its modes
                        self.pending, self.state, self.result = 0, C.ST_IDLE, C.RES_DONE
                    elif t - self.tdone >= C.CHECK_TIME:
                        failed = self.pending & ~okm
                        self.inited |= okm
                        self.pending = 0
                        self.err = C.ERR_MOTOR_CUBEMARS if failed & self.dev_cube else C.ERR_SENSOR_TORQUE if failed & self.dev_pax \
                            else C.ERR_SENSOR_POSITION if failed & self.dev_pos else C.ERR_MOTOR_MACRO
                        self._queue(okm & self.dev_cube, cubeoff, True)
                        self._queue(failed, disable_order, True)
                        self.tref, self.state, self.result = t, C.ST_DEINITIALISING, C.RES_FAILED
        elif self.state == C.ST_DEINITIALISING:
            if self.ri >= len(self.rq):
                self.state = C.ST_IDLE
                if self.result == C.RES_ACCEPTED:
                    self.result = C.RES_DONE
        elif self.state == C.ST_STOPPING:
            # done is the one of the previous step: it counts only if that step was already in the stop of this mode
            if (done and self.lastphase == 2) or t - self.t0 >= C.STOP_MAX:
                self.state, self.mode = C.ST_IDLE, C.MODE_NONE
                if self.mmode != C.CM_IDLE:
                    self._queue(self.dev_macro, idle_order, True)     # the mode is over: the motor_macro goes idle
                    self.mmode = C.CM_IDLE
                self._queue(self.mneeds & self.dev_cube, cubeoff, True)     # and the motor_cubemars is disabled
                self.mneeds = 0
                if self.lastaction == a["stop"] and self.result == C.RES_ACCEPTED:
                    self.result = C.RES_DONE

        # what goes to amc.gtw: the rpc (the setpoints are added by step_b)
        cmd = cmd_default()
        if self.ri < len(self.rq) and t - self.trpc >= C.RPC_GAP:
            cmd.update(rpc_go=True, rpc_op=self.rq[self.ri][0], rpc_arg=self.rq[self.ri][1])
            self.ri += 1
            self.trpc = t
        cmd["mmcfg"] = list(self.mmcfg)

        # what it tells
        sup = dict(have=req["have"], state=self.state, mode=self.mode, inited=self.inited, err=self.err, result=self.result,
                   reason=self.reason, action=self.lastaction, reqid=max(self.reqseen, 0), cnt=req["cnt"],
                   trun=(t - self.trun0) if self.state in (C.ST_RUNNING, C.ST_STOPPING) else 0.0)

        # which mode runs
        run = run_default(self.s.max_params)
        if self.state in (C.ST_RUNNING, C.ST_STOPPING):
            run.update(mode=self.mode, phase=1 if self.state == C.ST_RUNNING else 2, t=t - self.t0, first=(t - self.t0) == 0,
                       trun=t - self.trun0, p=list(self.params))
        run.update(inited=self.inited, mmode=self.mmode)
        self.lastphase = run["phase"]
        return run, cmd, sup

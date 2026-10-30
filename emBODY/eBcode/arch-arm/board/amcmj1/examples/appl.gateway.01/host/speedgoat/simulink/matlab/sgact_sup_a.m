function [run, cmd, sup] = sgact_sup_a(fb, req, done, t)
%SGACT_SUP_A the first half of the supervisor of sgact (SUP-A): it executes the requests (bus SGACT_REQ) on the devices
%   and the modes, watches amc.gtw (bus SGACT_FB), and tells which mode runs (bus SGACT_RUN), the rpc for amc.gtw (bus
%   SGACT_CMD w/out setpoints) and the status (bus SGACT_SUP). It calls no mode: the modes are in USER CONTROL, which
%   reads run and writes the bus SGACT_SP; SUP-B (sgact_sup_b) lets their setpoints out.
%   done: sp.done of the previous step (a unit delay: no algebraic loop). it ends the stop of the mode.
%   The same logic is in host/speedgoat/emul/sgact_supervisor.py for the daemon: keep them aligned.
%
%   requests (each one independent, none triggers another; one is executed when req.id changes):
%     init(devices)    from idle: enable the devices not yet initialised (sensor_torque, then motor_cubemars, then the motor_macro,
%                      which gets the config of the request: OPmotorPIDS, OPmotorSETUP), then check them on the state
%                      frame (sensor_torque counter moves, motor_cubemars status 1 or 2, motor_macro configured,
%                      sensor_position valid and its counter moves; it has no rpc).
%                      ok: they are initialised, and the motor_cubemars is disabled again (OPcubeDISABLE): it is enabled
%                      only while one of its modes runs. not ok: err 1 / 2 / 5 / 7, the failed ones are disabled again
%     deinit(devices)  from idle: disable them (motor_cubemars, then sensor_torque, then the motor_macro goes idle)
%     start(mode, params, duration)  from idle, if the devices of the mode are initialised and the params in range.
%                      a mode that needs the motor_cubemars enables it (OPcubeENABLE, unless cubemars_enable = false in the
%                      TOML: motor_cubemars_idle); any other disables it, if it is
%                      initialised (OPcubeDISABLE: amc.gtw sends the disable packet). a mode w/ a control mode of the
%                      motor_macro (MODE_CMODE) then asks it (OPmotorMODE); any other puts the motor_macro in idle, if
%                      it is initialised. so the mode idle (no devices) stops all the motors
%                      (checked in the units of the TOML; the mode sees them in SI: deg -> rad, PARAM_SCALE)
%     stop             from running: the mode does its own exit (stopping), then idle (the motor_macro idle, the
%                      motor_cubemars disabled)
%   the EXTFAULT of the requester (req.extfault, sticky in the panel): at once, w/out the exit of the mode, the rpc not yet
%     sent are dropped and amc.gtw gets OPgwFAULT 1: it disables the motor_cubemars, puts the motor_macro in idle, and holds
%     it (it refuses both until OPgwFAULT 0). no setpoint leaves: state extfault, where every request is refused (reason 1).
%     released: OPgwFAULT 0, idle, the devices stay initialised. if amc.gtw says otherwise (fb.gw_extfault: e.g. it
%     restarted, or sgact did) OPgwFAULT is sent again every FAULT_SYNC s
%   by itself:
%     the requester is silent for KEEPALIVE s while running -> stop, err 3
%     the duration expires -> stop
%     amc.gtw sends no state frame for AMCGTW_TIMEOUT s while something is initialised or running -> err 4, all disabled
%     the motor_macro is not in the control mode of the mode after CHECK_TIME -> stop, err 5
%     a fault of the motor_macro (external fault, hwfault) while its mode runs -> idle at once, err 6
%   the gate (which setpoints leave): sgact_sup_b
%
%   states 0 idle, 1 initialising, 2 deinitialising, 3 running, 4 stopping. results, reasons, errors: sgact_const.m

    c = sgact_const();
    s = sgact_spec();
    persistent state inited err result reason lastaction reqseen mode params duration
    persistent t0 trun0 tref rq ra rn ri trpc tdone pending pax0 pos0 mneeds mmcfg mmode lastphase tsync
    if isempty(state)
        state = c.ST_IDLE; inited = 0; err = 0; result = c.RES_NONE; reason = 0; lastaction = 0; reqseen = -1;
        mode = c.MODE_NONE; params = zeros(1, s.MAX_PARAMS); duration = 0;
        t0 = 0; trun0 = 0; tref = 0; rq = zeros(1, c.RPC_QUEUE); ra = zeros(1, c.RPC_QUEUE); rn = 0; ri = 0; trpc = -1e9; tdone = -1;
        pending = 0; pax0 = 0; pos0 = 0; mneeds = 0; mmcfg = zeros(1, 14); mmode = c.CM_IDLE; lastphase = 0;
        tsync = -1e9;
    end
    % the rpc of the devices, in their order: [device op argument]
    enable_order = [s.DEV_SENSOR_TORQUE, c.OP_PAX_ENABLE, 0; s.DEV_MOTOR_CUBEMARS, c.OP_CUBE_ENABLE, 0; ...
                    s.DEV_MOTOR_MACRO, c.OP_MOTOR_PIDS, 0; s.DEV_MOTOR_MACRO, c.OP_MOTOR_SETUP, 0];
    disable_order = [s.DEV_MOTOR_CUBEMARS, c.OP_CUBE_DISABLE, 0; s.DEV_SENSOR_TORQUE, c.OP_PAX_DISABLE, 0; ...
                     s.DEV_MOTOR_MACRO, c.OP_MOTOR_MODE, c.CM_IDLE];
    idle_order = [s.DEV_MOTOR_MACRO, c.OP_MOTOR_MODE, c.CM_IDLE];
    cubeoff = [s.DEV_MOTOR_CUBEMARS, c.OP_CUBE_DISABLE, 0];
    cubeon = [s.DEV_MOTOR_CUBEMARS, c.OP_CUBE_ENABLE, 0];
    faulton = [1, c.OP_GW_FAULT, 1];                         % (device mask 1: always)
    faultoff = [1, c.OP_GW_FAULT, 0];

    % ---- the EXTFAULT of the requester: before anything else. its crc does not matter: a stop is always welcome
    if req.have && req.extfault
        if state ~= c.ST_EXTFAULT
            [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, 1, faulton, false, c.RPC_QUEUE);   % amc.gtw stops both motors
            tsync = t;
            if result == c.RES_ACCEPTED
                result = c.RES_FAILED;
            end
            pending = 0; mode = c.MODE_NONE; mmode = c.CM_IDLE; mneeds = 0; tref = t;
            state = c.ST_EXTFAULT;
        end
    elseif state == c.ST_EXTFAULT
        state = c.ST_IDLE;                                  % released
        [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, 1, faultoff, true, c.RPC_QUEUE);
        tsync = t;
    end
    % amc.gtw must hold the EXTFAULT exactly while sgact is in extfault
    if fb.has_motor_macro && ri >= rn && t - tsync >= c.FAULT_SYNC && (fb.gw_extfault ~= 0) ~= (state == c.ST_EXTFAULT)
        if state == c.ST_EXTFAULT
            [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, 1, faulton, true, c.RPC_QUEUE);
        else
            [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, 1, faultoff, true, c.RPC_QUEUE);
        end
        tsync = t;
    end

    % ---- a new request
    if req.have && req.id ~= reqseen
        reqseen = req.id;
        lastaction = req.action;
        reason = 0;
        if ~req.crc_ok
            result = c.RES_REJECTED; reason = c.WHY_SPEC;
        elseif req.action == s.ACT_INIT
            if state ~= c.ST_IDLE
                result = c.RES_REJECTED; reason = c.WHY_STATE;
            else
                err = 0;
                mask = bitand(req.devices, s.DEV_ALL);
                mask = mask - bitand(mask, inited);             % the ones not yet initialised
                if mask == 0
                    result = c.RES_DONE;
                else
                    if bitand(mask, s.DEV_MOTOR_MACRO) ~= 0
                        mmcfg = req.mmcfg;                      % the config of the motor_macro: the one of this request
                    end
                    [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, mask, enable_order, true, c.RPC_QUEUE);
                    pending = mask; tdone = -1; tref = t; pax0 = fb.sensor_torque_counter; pos0 = fb.sensor_position_counter;
                    state = c.ST_INITIALISING; result = c.RES_ACCEPTED;
                end
            end
        elseif req.action == s.ACT_DEINIT
            if state ~= c.ST_IDLE
                result = c.RES_REJECTED; reason = c.WHY_STATE;
            else
                err = 0;
                mask = bitand(req.devices, inited);
                if mask == 0
                    result = c.RES_DONE;
                else
                    [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, mask, disable_order, true, c.RPC_QUEUE);
                    inited = inited - mask; tref = t;
                    state = c.ST_DEINITIALISING; result = c.RES_ACCEPTED;
                end
            end
        elseif req.action == s.ACT_START
            idx = mode_index(req.mode, s);
            if state ~= c.ST_IDLE
                result = c.RES_REJECTED; reason = c.WHY_STATE;
            elseif idx == 0
                result = c.RES_REJECTED; reason = c.WHY_MODE;
            elseif bitand(s.MODE_NEEDS(idx), inited) ~= s.MODE_NEEDS(idx)
                result = c.RES_REJECTED; reason = c.WHY_DEVICES;
            elseif ~params_ok(idx, req.params, s)
                result = c.RES_REJECTED; reason = c.WHY_PARAMS;
            else
                err = 0;
                mode = req.mode; params = req.params .* s.PARAM_SCALE(idx, :); duration = req.duration;   % the mode sees SI
                state = c.ST_RUNNING; t0 = t; trun0 = t; tref = t;
                result = c.RES_DONE;
                mmode = s.MODE_CMODE(idx);
                mneeds = s.MODE_NEEDS(idx);
                if bitand(mneeds, s.DEV_MOTOR_CUBEMARS) ~= 0 && s.MODE_CUBEON(idx) ~= 0
                    [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, s.DEV_MOTOR_CUBEMARS, cubeon, true, c.RPC_QUEUE);   % only w/ its modes
                elseif bitand(inited, s.DEV_MOTOR_CUBEMARS) ~= 0
                    [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, s.DEV_MOTOR_CUBEMARS, cubeoff, true, c.RPC_QUEUE);  % it must not move
                end
                if mmode ~= c.CM_IDLE
                    [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, s.DEV_MOTOR_MACRO, [s.DEV_MOTOR_MACRO, c.OP_MOTOR_MODE, mmode], true, c.RPC_QUEUE);
                elseif bitand(inited, s.DEV_MOTOR_MACRO) ~= 0
                    [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, s.DEV_MOTOR_MACRO, idle_order, true, c.RPC_QUEUE);   % it stays still
                end
            end
        elseif req.action == s.ACT_STOP
            if state ~= c.ST_RUNNING
                result = c.RES_REJECTED; reason = c.WHY_STATE;
            else
                state = c.ST_STOPPING; t0 = t; result = c.RES_ACCEPTED;
            end
        else
            result = c.RES_REJECTED; reason = c.WHY_ACTION;
        end
    end

    % ---- the parameters of the running mode follow the requester (the same mode, values in range)
    if state == c.ST_RUNNING && req.have && req.crc_ok && req.mode == mode
        idx = mode_index(mode, s);
        if idx > 0 && params_ok(idx, req.params, s)
            params = req.params .* s.PARAM_SCALE(idx, :);
            duration = req.duration;
        end
    end

    % ---- watch: amc.gtw, the requester, the duration
    if (inited ~= 0 || state ~= c.ST_IDLE) && t - max(fb.trx, tref) > c.AMCGTW_TIMEOUT
        err = c.ERR_AMCGTW;                                 % amc.gtw does not answer: everything off
        if result == c.RES_ACCEPTED
            result = c.RES_FAILED;
        end
        [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, bitor(inited, pending), disable_order, false, c.RPC_QUEUE);
        inited = 0; pending = 0; mode = c.MODE_NONE; tref = t; mmode = c.CM_IDLE;
        if state ~= c.ST_EXTFAULT
            state = c.ST_DEINITIALISING;                    % (extfault stays until it is released)
        end
    elseif (state == c.ST_RUNNING || state == c.ST_STOPPING) && mmode ~= c.CM_IDLE && fb.has_motor_macro && ...
            (bitand(fb.motor_macro_faults, c.FAULT_EXTERNAL) ~= 0 || fb.motor_macro_mode == c.CM_HWFAULT)
        err = c.ERR_MMFAULT;                                % a fault of the motor_macro: no exit of the mode, idle at once
        [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, s.DEV_MOTOR_MACRO, idle_order, true, c.RPC_QUEUE);
        [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, bitand(mneeds, s.DEV_MOTOR_CUBEMARS), cubeoff, true, c.RPC_QUEUE);
        state = c.ST_IDLE; mode = c.MODE_NONE; mmode = c.CM_IDLE;
        if result == c.RES_ACCEPTED
            result = c.RES_FAILED;
        end
    elseif state == c.ST_RUNNING && mmode ~= c.CM_IDLE && t - trun0 >= c.CHECK_TIME && fb.motor_macro_mode ~= mmode
        err = c.ERR_MOTOR_MACRO;                             % the motor_macro did not take the control mode
        state = c.ST_STOPPING; t0 = t;
    elseif state == c.ST_RUNNING && ~req.alive
        err = c.ERR_KEEPALIVE;                              % the requester went silent
        state = c.ST_STOPPING; t0 = t;
    elseif state == c.ST_RUNNING && duration > 0 && t - trun0 >= duration
        state = c.ST_STOPPING; t0 = t;
    end

    % ---- the progress of the state
    if state == c.ST_INITIALISING
        if ri >= rn
            if tdone < 0
                tdone = t;
            end
            if t - tdone >= c.ENABLE_SETTLE
                okm = 0;
                if bitand(pending, s.DEV_MOTOR_MACRO) ~= 0 && (~c.CHECK_ENABLE || ...
                        (fb.has_motor_macro && fb.motor_macro_configured == c.MM_CONFIGURED))
                    okm = okm + s.DEV_MOTOR_MACRO;
                end
                if bitand(pending, s.DEV_MOTOR_CUBEMARS) ~= 0 && (~c.CHECK_ENABLE || ...
                        (fb.has_motor_cubemars_status && (fb.motor_cubemars_status == 1 || fb.motor_cubemars_status == 2)))
                    okm = okm + s.DEV_MOTOR_CUBEMARS;
                end
                if bitand(pending, s.DEV_SENSOR_TORQUE) ~= 0 && (~c.CHECK_ENABLE || fb.sensor_torque_counter ~= pax0)
                    okm = okm + s.DEV_SENSOR_TORQUE;
                end
                if bitand(pending, s.DEV_SENSOR_POSITION) ~= 0 && (~c.CHECK_ENABLE || (fb.has_sensor_position && ...
                        bitand(fb.sensor_position_flags, c.POS_VALID) ~= 0 && fb.sensor_position_counter ~= pos0))
                    okm = okm + s.DEV_SENSOR_POSITION;
                end
                if okm == pending
                    inited = bitor(inited, pending);
                    [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, bitand(pending, s.DEV_MOTOR_CUBEMARS), cubeoff, true, c.RPC_QUEUE);
                    pending = 0;
                    state = c.ST_IDLE; result = c.RES_DONE;
                elseif t - tdone >= c.CHECK_TIME
                    failed = pending - okm;
                    inited = bitor(inited, okm); pending = 0;
                    if bitand(failed, s.DEV_MOTOR_CUBEMARS) ~= 0
                        err = c.ERR_MOTOR_CUBEMARS;
                    elseif bitand(failed, s.DEV_SENSOR_TORQUE) ~= 0
                        err = c.ERR_SENSOR_TORQUE;
                    elseif bitand(failed, s.DEV_SENSOR_POSITION) ~= 0
                        err = c.ERR_SENSOR_POSITION;
                    else
                        err = c.ERR_MOTOR_MACRO;
                    end
                    [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, bitand(okm, s.DEV_MOTOR_CUBEMARS), cubeoff, true, c.RPC_QUEUE);
                    [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, failed, disable_order, true, c.RPC_QUEUE);
                    tref = t;
                    state = c.ST_DEINITIALISING; result = c.RES_FAILED;
                end
            end
        end
    elseif state == c.ST_DEINITIALISING
        if ri >= rn
            state = c.ST_IDLE;
            if result == c.RES_ACCEPTED
                result = c.RES_DONE;
            end
        end
    elseif state == c.ST_STOPPING
        % done is the one of the previous step: it counts only if that step was already in the stop of this mode
        if (done && lastphase == 2) || t - t0 >= c.STOP_MAX
            state = c.ST_IDLE; mode = c.MODE_NONE;
            if mmode ~= c.CM_IDLE
                [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, s.DEV_MOTOR_MACRO, idle_order, true, c.RPC_QUEUE);
                mmode = c.CM_IDLE;                          % the mode is over: the motor_macro goes idle
            end
            [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, bitand(mneeds, s.DEV_MOTOR_CUBEMARS), cubeoff, true, c.RPC_QUEUE);
            mneeds = 0;                                     % and the motor_cubemars is disabled
            if lastaction == s.ACT_STOP && result == c.RES_ACCEPTED
                result = c.RES_DONE;
            end
        end
    end

    % ---- what goes to amc.gtw: the rpc (the setpoints are added by sgact_sup_b)
    cmd = sgact_cmd_default();
    if ri < rn && t - trpc >= c.RPC_GAP
        ri = ri + 1;
        cmd.rpc_go = true;
        cmd.rpc_op = rq(ri);
        cmd.rpc_arg = ra(ri);
        trpc = t;
    end
    cmd.mmcfg = mmcfg;

    % ---- what it tells
    sup = sgact_sup_default();
    sup.have = req.have;
    sup.state = state;
    sup.mode = mode;
    sup.inited = inited;
    sup.err = err;
    sup.result = result;
    sup.reason = reason;
    sup.action = lastaction;
    sup.reqid = max(reqseen, 0);
    sup.cnt = req.cnt;
    if state == c.ST_RUNNING || state == c.ST_STOPPING
        sup.trun = t - trun0;
    end

    % ---- which mode runs: for USER CONTROL and SUP-B
    run = sgact_run_default();
    if state == c.ST_RUNNING || state == c.ST_STOPPING
        run.mode = mode;
        if state == c.ST_RUNNING
            run.phase = 1;
        else
            run.phase = 2;
        end
        run.t = t - t0;
        run.first = (t - t0) == 0;
        run.trun = t - trun0;
        run.p = params;
    end
    run.inited = inited;
    run.mmode = mmode;
    lastphase = run.phase;
end


function idx = mode_index(mode, s)
%MODE_INDEX the index of the mode in sgact_spec, 0 if unknown
    idx = 0;
    for i = 1:s.NMODES
        if s.MODE_ID(i) == mode
            idx = i;
        end
    end
end


function ok = params_ok(idx, p, s)
%PARAMS_OK the parameters of the mode are numbers in [min, max] of sgact_panel.toml
    ok = true;
    for k = 1:s.MODE_NPARAMS(idx)
        if ~(p(k) >= s.PARAM_MIN(idx, k) && p(k) <= s.PARAM_MAX(idx, k))     % a NaN fails too
            ok = false;
        end
    end
end


function [rq, ra, rn, ri] = queue_ops(rq, ra, rn, ri, mask, order, append, nmax)
%QUEUE_OPS the rpc operations of order = [device op argument; ...] for the devices of mask, in that order. append: after
%   the ones not yet sent (rq(ri+1:rn)), else they are dropped. at most nmax wait
    q = zeros(1, nmax);
    a = zeros(1, nmax);
    n = 0;
    if append
        for i = (ri + 1):rn
            n = n + 1;
            q(n) = rq(i);
            a(n) = ra(i);
        end
    end
    for i = 1:size(order, 1)
        if bitand(mask, order(i, 1)) ~= 0 && n < nmax
            n = n + 1;
            q(n) = order(i, 2);
            a(n) = order(i, 3);
        end
    end
    rq = q;
    ra = a;
    rn = n;
    ri = 0;
end

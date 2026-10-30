function test_sgact_step()
%TEST_SGACT_STEP the whole chain of the model (sgact_step: panel RX, amc.gtw RX, supervisor + modes, amc.gtw TX,
%   panel TX) with real bytes: CTL v3 packets in, command / rpc frames to a fake amc.gtw, state frames back, STATUS v3
%   out. W/out Simulink (also in Octave). The supervisor in depth: test_parity (with the trace of the Python one).
%     1. no panel: heartbeats, no STATUS
%     2. init motor_cubemars + sensor_torque: rpc sensor_torque then motor_cubemars, STATUS: both initialised, done
%     3. start motor_cubemars_velocity_step 120 deg/s, half 0.5 s, duration 2 s: OPcubeENABLE, MITs of +-120 deg/s, stop by
%        itself, OPcubeDISABLE, no more MIT
%     4. deinit both: rpc motor_cubemars then sensor_torque, heartbeats again
%     5. start w/out devices: rejected (devices); a CTL of another TOML: rejected (spec)
%     6. the motor_macro: init (OPmotorPIDS, OPmotorSETUP w/ the config of the CTL), start motor_macro_velocity 90 deg/s
%        (OPmotorMODE speedvoltage, targets of 90 deg/s in the command frames), stop (ramp, then OPmotorMODE idle)
%   run it from the folder simulink:  sgact_setup; test_sgact_step

    c = sgact_const();
    s = sgact_spec();
    clear sgact_gtw_rx_step sgact_panel_rx_step sgact_gtw_tx_step sgact_panel_tx_step sgact_sup_a sgact_sup_b sgact_controller fake_amcgtw
    opts = struct('paxbroken', false, 'mute', false);
    n = 0; total = 0; tk = 0;
    st = zeros(c.STATE_MAX_SIZE, 1, 'uint8'); stlen = 0;
    ctl = struct('action', 0, 'mode', 0, 'devices', 0, 'id', 0, 'duration', 0, 'params', zeros(1, 8), 'crc', double(s.CRC), 'on', false, ...
                 'mmcfg', s.MMCFG);

    % ---- 1. no panel
    [r, tk, st, stlen] = steps(500, tk, ctl, st, stlen, opts);
    [n, total] = check(n, total, '1. no panel: a heartbeat every CMD_DIV steps, no STATUS', r.nhb == 500 / c.CMD_DIV && r.nmit == 0 && r.nstatus == 0);

    % ---- 2. init cube, pax, pos
    ctl.on = true; ctl.action = s.ACT_INIT; ctl.devices = s.DEV_MOTOR_CUBEMARS + s.DEV_SENSOR_TORQUE + s.DEV_SENSOR_POSITION; ctl.id = 1;
    [r, tk, st, stlen] = steps(1500, tk, ctl, st, stlen, opts);
    [n, total] = check(n, total, '2. init: rpc sensor_torque enable, motor_cubemars enable, then disable', ...
                       isequal(r.ops, [c.OP_PAX_ENABLE, c.OP_CUBE_ENABLE, c.OP_CUBE_DISABLE]));
    [n, total] = check(n, total, '2. STATUS: idle, the three initialised, request 1 done, sensor_position 12.5 deg', r.last(5) == c.ST_IDLE ...
                       && r.last(7) == 11 && r.last(9) == c.RES_DONE && typecast(r.last(13:16), 'uint32') == 1 && r.nmit == 0 ...
                       && abs(double(typecast(r.last(93:96), 'single')) - 12.5 * pi / 180) < 1e-6 && r.last(101) == 3);

    % ---- 3. start velocity_step w/ duration
    ctl.action = s.ACT_START; ctl.mode = 1; ctl.id = 2; ctl.duration = 2; ctl.params = [0, 120, 0, 1, 0, 0.5, 0, 0];      % 120 deg/s
    [r, tk, st, stlen] = steps(3500, tk, ctl, st, stlen, opts);
    [n, total] = check(n, total, '3. STATUS running velocity_step seen, OPcubeENABLE then OPcubeDISABLE', any(r.states == c.ST_RUNNING) ...
                       && any(r.modes == 1) && isequal(r.ops, [c.OP_CUBE_ENABLE, c.OP_CUBE_DISABLE]));
    [n, total] = check(n, total, '3. MITs of +-120 deg/s (2.094 rad/s)', abs(max(r.vels) - 120 * pi / 180) < 1e-6 ...
                       && abs(min(r.vels) + 120 * pi / 180) < 1e-6);
    [n, total] = check(n, total, '3. stop by itself: idle, no more MIT', r.last(5) == c.ST_IDLE && r.last(6) == c.MODE_NONE ...
                       && ~r.lastmit_valid && ~r.info.cuberun);

    % ---- 4. deinit
    ctl.action = s.ACT_DEINIT; ctl.id = 3; ctl.devices = s.DEV_MOTOR_CUBEMARS + s.DEV_SENSOR_TORQUE + s.DEV_SENSOR_POSITION;
    [r, tk, st, stlen] = steps(500, tk, ctl, st, stlen, opts);
    [n, total] = check(n, total, '4. deinit: rpc motor_cubemars disable, then sensor_torque disable', isequal(r.ops, [c.OP_CUBE_DISABLE, c.OP_PAX_DISABLE]));
    [n, total] = check(n, total, '4. nothing initialised, heartbeats again', r.last(7) == 0 && ~r.lastmit_valid && r.nhb > 0);

    % ---- 5. rejections
    ctl.action = s.ACT_START; ctl.mode = 0; ctl.id = 4; ctl.params = [10, 0, 5, 0.5, 0, 0, 0, 0];
    [r, tk, st, stlen] = steps(400, tk, ctl, st, stlen, opts);
    [n, total] = check(n, total, '5. start w/out its devices: rejected, reason 2', r.last(9) == c.RES_REJECTED && r.last(10) == c.WHY_DEVICES);
    ctl.action = s.ACT_INIT; ctl.id = 5; ctl.crc = 1;
    [r, tk, st, stlen] = steps(400, tk, ctl, st, stlen, opts);
    [n, total] = check(n, total, '5. a CTL of another TOML: rejected, reason 5', r.last(9) == c.RES_REJECTED && r.last(10) == c.WHY_SPEC);
    ctl.crc = double(s.CRC);

    % ---- 6. the motor_macro
    ctl.action = s.ACT_INIT; ctl.devices = s.DEV_MOTOR_MACRO; ctl.id = 6;
    [r, tk, st, stlen] = steps(1500, tk, ctl, st, stlen, opts);
    info = r.info;
    [n, total] = check(n, total, '6. init motor_macro: OPmotorPIDS, OPmotorSETUP w/ the config of the CTL, initialised', ...
                       isequal(r.ops, [c.OP_MOTOR_PIDS, c.OP_MOTOR_SETUP]) && max(abs(info.mmcfg - s.MMCFG)) < 1e-6 ...
                       && r.last(7) == s.DEV_MOTOR_MACRO && r.last(9) == c.RES_DONE && r.last(66) == 3);
    ctl.action = s.ACT_START; ctl.mode = 10; ctl.id = 7; ctl.duration = 0; ctl.params = [90, 0, 0, 0, 0, 0, 0, 0];
    [r, tk, st, stlen] = steps(1000, tk, ctl, st, stlen, opts);
    [n, total] = check(n, total, '6. start motor_macro_velocity 90 deg/s: OPmotorMODE speedvoltage, targets of 90 deg/s', ...
                       isequal(r.ops, c.OP_MOTOR_MODE) && r.args(1) == c.CM_SPEEDVOLTAGE && abs(r.lasttarget - 90) < 1e-4 ...
                       && r.lasttmode == c.CM_SPEEDVOLTAGE && r.ntarget > 900);
    [n, total] = check(n, total, '6. STATUS: running, the motor_macro in speedvoltage at 90 deg/s (in rad/s)', r.last(5) == c.ST_RUNNING ...
                       && r.last(65) == c.CM_SPEEDVOLTAGE && abs(double(typecast(r.last(57:60), 'single')) - pi / 2) < 1e-5);
    ctl.action = s.ACT_STOP; ctl.id = 8;
    [r, tk, st, stlen] = steps(1000, tk, ctl, st, stlen, opts);
    [n, total] = check(n, total, '6. stop: ramp of the targets, then OPmotorMODE idle, no more targets', isequal(r.ops, c.OP_MOTOR_MODE) ...
                       && r.args(1) == c.CM_IDLE && min(r.targets) < 90 && r.last(5) == c.ST_IDLE && r.last(65) == c.CM_IDLE && ~r.lasttvalid);

    fprintf('%d/%d chain checks passed\n', n, total);
    if n ~= total
        error('test_sgact_step failed');
    end
end


function [r, tk, st, stlen] = steps(nsteps, tk, ctl, st, stlen, opts)
%STEPS runs the chain and the fake amc.gtw for nsteps steps of 1 ms. the panel sends its CTL every 200 ms.
%   the state frame of amc.gtw arrives at the next step (as on the wire)
    c = sgact_const();
    r = struct('nhb', 0, 'nmit', 0, 'nstatus', 0, 'ops', [], 'args', [], 'states', [], 'modes', [], 'vels', [], 'last', zeros(c.STATUS_SIZE, 1, 'uint8'), ...
               'lastmit_valid', false, 'lastvel', NaN, 'ntarget', 0, 'targets', [], 'lasttarget', NaN, 'lasttmode', 0, 'lasttvalid', false, 'info', []);
    for i = 1:nsteps
        tk = tk + 1;
        t = (tk - 1) * c.TS;
        frame = zeros(c.CTL_SIZE, 1, 'uint8');
        len = 0;
        if ctl.on && mod(tk, 200) == 0
            frame(1:c.CTL_SIZE) = sgact_ctl_frame(ctl.action, ctl.mode, ctl.devices, floor(tk / 200), ctl.id, ctl.crc, ctl.duration, ctl.params, ctl.mmcfg);
            len = c.CTL_SIZE;
        end
        [cmdf, cmd_len, cmd_go, rpc, ~, rpc_go, status, ~, status_go] = sgact_step(frame, len, st, stlen, t);
        [st, stlen, r.info] = fake_amcgtw(cmdf, cmd_len, cmd_go, rpc, rpc_go, opts);
        if cmd_go
            slot = double(cmdf(5));
            num = double(cmdf(6));
            hasmit = false;
            hastarget = false;
            for k = 0:(num - 1)
                off = 16 + k * slot;
                if cmdf(off + 2) == c.ID_CUBE_MIT
                    hasmit = true;
                    r.lastvel = double(typecast(cmdf(off + 4 + (13:16)), 'single'));
                elseif cmdf(off + 2) == c.ID_MOTOR_TARGET
                    hastarget = true;
                    r.lasttarget = double(typecast(cmdf(off + 4 + (1:4)), 'single'));
                    r.lasttmode = double(cmdf(off + 9));
                end
            end
            if hasmit
                r.nmit = r.nmit + 1;
                r.vels(end + 1) = r.lastvel;
            end
            if hastarget
                r.ntarget = r.ntarget + 1;
                r.targets(end + 1) = r.lasttarget;
            end
            if num == 0
                r.nhb = r.nhb + 1;
            end
            r.lastmit_valid = hasmit;
            r.lasttvalid = hastarget;
        end
        if rpc_go
            r.ops(end + 1) = double(rpc(18));
            r.args(end + 1) = double(rpc(21));
        end
        if status_go
            r.nstatus = r.nstatus + 1;
            r.last = status;
            r.states(end + 1) = double(status(5));
            r.modes(end + 1) = double(status(6));
        end
    end
end


function [n, total] = check(n, total, name, cond)
    total = total + 1;
    if cond
        n = n + 1;
        fprintf('  PASS  %s\n', name);
    else
        fprintf('  FAIL  %s\n', name);
    end
end

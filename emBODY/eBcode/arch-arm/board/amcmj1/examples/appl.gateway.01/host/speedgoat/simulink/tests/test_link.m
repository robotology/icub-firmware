function test_link()
%TEST_LINK the four I/O blocks and the buses between them and the Controller, w/out Simulink (also in Octave)
%   1. the structs that the functions produce have exactly the fields, classes and sizes of the default structs, from
%      which sgact_buses.m makes the Simulink buses: if this fails, the model would not compile
%   2. panel RX decodes the CTL v3 (keepalive, crc, config of the motor_macro), amc.gtw RX the state frame (motor_cubemars,
%      sensor_torque, motor_macro, gateway)
%   3. amc.gtw TX sends a frame every CMD_DIV steps (MIT and / or target of the motor_macro, or heartbeat, seq + 1) and
%      the rpc on request (w/ their argument), panel TX the STATUS v3 every STATUS_DIV steps once a requester spoke
%   run it from the folder simulink:  sgact_setup; test_link

    c = sgact_const();
    s = sgact_spec();
    here = fileparts(mfilename('fullpath'));
    g = read_golden(fullfile(here, 'golden.txt'));
    reset();
    n = 0; total = 0;

    % ---- 1 + 2. RX: the panel and amc.gtw
    st = zeros(c.STATE_MAX_SIZE, 1, 'uint8');
    st(1:numel(g.state_cpp)) = g.state_cpp;
    mmcfg = [1, 0.1, 0, 0.5, 0.05, 0, 1, 2, 3, 3, 8, 0, 4096, -5];
    ctl = zeros(c.CTL_SIZE, 1, 'uint8');
    ctl(1:c.CTL_SIZE) = sgact_ctl_frame(3, 1, 3, 9, 1234, s.CRC, 10, [1, 1], mmcfg);
    fb = sgact_gtw_rx_step(st, numel(g.state_cpp), 0.5);
    req = sgact_panel_rx_step(ctl, c.CTL_SIZE, 0.5);
    [n, total] = check(n, total, '1. amc.gtw RX fb has the fields of the bus SGACT_FB', same_shape(fb, sgact_fb_default()));
    [n, total] = check(n, total, '1. panel RX req has the fields of the bus SGACT_REQ', same_shape(req, sgact_req_default()));
    [n, total] = check(n, total, '2. panel RX: start velocity, both, id 1234, alive, crc ok', ...
                       req.have && req.alive && req.crc_ok && req.action == 3 && req.mode == 1 && req.devices == 3 && req.id == 1234 ...
                       && req.cnt == 9 && req.duration == 10 && req.params(1) == 1 && req.params(2) == 1 && max(abs(req.mmcfg - mmcfg)) < 1e-6);
    req2 = sgact_panel_rx_step(ctl, 0, 0.5 + s.KEEPALIVE + 0.01);
    [n, total] = check(n, total, '2. panel RX: not alive after KEEPALIVE s of silence', req2.have && ~req2.alive && req2.id == 1234);
    [n, total] = check(n, total, '2. amc.gtw RX: motor_cubemars, sensor_torque, gateway', ...
                       fb.valid && fb.fresh && fb.nrecv == 1 && fb.trx == 0.5 && fb.motor_cubemars_pos == 1.5 && fb.motor_cubemars_counter == 7 ...
                       && fb.sensor_torque_mz == 3 && fb.sensor_torque_counter == 9 && fb.gw(7) == 2);
    [n, total] = check(n, total, '2. amc.gtw RX: the motor_macro', fb.has_motor_macro && fb.motor_macro_counter == 4 && fb.motor_macro_mode == c.CM_SPEEDVOLTAGE ...
                       && fb.motor_macro_configured == 3 && fb.motor_macro_faults == c.FAULT_EXTERNAL);
    fb2 = sgact_gtw_rx_step(st, 0, 0.6);
    [n, total] = check(n, total, '2. amc.gtw RX keeps the last frame, fresh only when it arrives', fb2.valid && ~fb2.fresh && fb2.nrecv == 1 && fb2.motor_cubemars_pos == 1.5);

    % ---- 1. the Controller writes the buses SGACT_RUN, SGACT_SP, SGACT_CMD and SGACT_SUP
    [cmd, sup, run, sp] = sgact_controller(fb, req, 0.5);
    [n, total] = check(n, total, '1. SUP-B cmd has the fields of the bus SGACT_CMD', same_shape(cmd, sgact_cmd_default()));
    [n, total] = check(n, total, '1. SUP-A sup has the fields of the bus SGACT_SUP', same_shape(sup, sgact_sup_default()));
    [n, total] = check(n, total, '1. SUP-A run has the fields of the bus SGACT_RUN', same_shape(run, sgact_run_default()));
    [n, total] = check(n, total, '1. USER CONTROL sp has the fields of the bus SGACT_SP', same_shape(sp, sgact_sp_default()) ...
                       && same_shape(sgact_std_modes(run, fb), sgact_sp_default()));

    % ---- 3. TX
    reset();
    cmd = sgact_cmd_default();
    cmd.motor_cubemars_mit_valid = true; cmd.motor_cubemars_kp = 5; cmd.motor_cubemars_kd = 0.5; cmd.motor_cubemars_pos = 0.5; cmd.motor_cubemars_vel = 0; cmd.motor_cubemars_torque = 0.1;
    sup = sgact_sup_default();
    sup.have = true; sup.state = 3; sup.mode = 1; sup.inited = 3; sup.result = 3; sup.action = 3; sup.reqid = 1234; sup.cnt = 9; sup.trun = 1.5;
    ngo = 0; okmit = true; okrpc = true; nstatus = 0; okstatus = false;
    for k = 1:(2 * c.STATUS_DIV)
        [cmdf, cmd_len, cmd_go, ~, ~, rpc_go, nsent] = sgact_gtw_tx_step(cmd, 0);
        [status, ~, status_go] = sgact_panel_tx_step(sup, fb, nsent, 0);
        if cmd_go
            ngo = ngo + 1;
            okmit = okmit && cmd_len == 48 && isequal(cmdf(1:48), sgact_mit_frame(5, 0.5, 0.5, 0, 0.1, c.SEQ0 + ngo - 1, 0));
        end
        okrpc = okrpc && ~rpc_go;
        if status_go
            nstatus = nstatus + 1;
            if nstatus == 1
                okstatus = isequal(status, sgact_status_frame(sup, fb, 1)) && isequal(status(1:4).', uint8('SGS4'));
            end
        end
    end
    [n, total] = check(n, total, '3. amc.gtw TX a command frame every CMD_DIV steps', ngo == 2 * c.STATUS_DIV / c.CMD_DIV);
    [n, total] = check(n, total, '3. amc.gtw TX the MIT bytes and seq + 1 at each frame', okmit && okrpc);
    [n, total] = check(n, total, '3. panel TX STATUS v4 every STATUS_DIV steps', nstatus == 2 && okstatus);
    cmd.motor_cubemars_mit_valid = false; cmd.rpc_go = true; cmd.rpc_op = c.OP_PAX_ENABLE;
    reset();
    [cmdf, cmd_len, cmd_go, rpc, rpc_len, rpc_go] = sgact_gtw_tx_step(cmd, 0);
    [n, total] = check(n, total, '3. amc.gtw TX heartbeat w/out MIT, rpc on request', cmd_go && cmd_len == 16 && isequal(cmdf(1:16), g.hb) ...
                       && rpc_go && rpc_len == 24 && isequal(rpc(1:24), g.rpc_pax_enable));
    % the motor_macro: the target in deg/s, the rpc w/ the config
    reset();
    cmd = sgact_cmd_default();
    cmd.motor_cubemars_mit_valid = true; cmd.motor_cubemars_kp = 5; cmd.motor_cubemars_kd = 0.5; cmd.motor_cubemars_pos = 0.5; cmd.motor_cubemars_vel = 0; cmd.motor_cubemars_torque = 0.1;
    cmd.motor_macro_valid = true; cmd.motor_macro_mode = c.CM_SPEEDVOLTAGE; cmd.motor_macro_target = pi / 2;
    cmd.rpc_go = true; cmd.rpc_op = c.OP_MOTOR_PIDS; cmd.mmcfg = mmcfg;
    [cmdf, cmd_len, ~, rpc, rpc_len] = sgact_gtw_tx_step(cmd, 0);
    [f, len] = sgact_cmd_frame(true, [5, 0.5, 0.5, 0, 0.1], c.SEQ0, true, 90, c.CM_SPEEDVOLTAGE, c.SEQ0, 0);
    [n, total] = check(n, total, '3. amc.gtw TX MIT + target of the motor_macro (rad/s -> 90 deg/s), 80 bytes', cmd_len == 80 && len == 80 ...
                       && isequal(cmdf, f) && abs(double(typecast(cmdf(53:56), 'single')) - 90) < 1e-4);
    [n, total] = check(n, total, '3. amc.gtw TX run<OPmotorPIDS> w/ the config, 48 bytes', rpc_len == 48 && isequal(rpc(1:48), g.rpc_motor_pids));
    cmd.rpc_op = c.OP_MOTOR_MODE; cmd.rpc_arg = c.CM_SPEEDVOLTAGE; cmd.motor_cubemars_mit_valid = false;
    [cmdf, cmd_len, ~, rpc, rpc_len] = sgact_gtw_tx_step(cmd, 0);
    [n, total] = check(n, total, '3. amc.gtw TX target alone (32 bytes), run<OPmotorMODE, speedvoltage>', cmd_len == 32 && cmdf(18) == c.ID_MOTOR_TARGET ...
                       && rpc_len == 24 && isequal(rpc(1:24), g.rpc_motor_mode));

    fprintf('%d/%d link checks passed\n', n, total);
    if n ~= total
        error('test_link failed');
    end
end


function reset()
    clear sgact_gtw_rx_step sgact_panel_rx_step sgact_gtw_tx_step sgact_panel_tx_step sgact_sup_a sgact_sup_b sgact_controller
end


function ok = same_shape(a, b)
%SAME_SHAPE the same fields, in the same order, with the same class and size: what a Simulink bus needs
    fa = fieldnames(a);
    fbn = fieldnames(b);
    ok = isequal(fa, fbn);
    for k = 1:numel(fa)
        if ~ok
            break;
        end
        ok = strcmp(class(a.(fa{k})), class(b.(fa{k}))) && isequal(size(a.(fa{k})), size(b.(fa{k})));
        if ~ok
            fprintf('    field %s: %s %s vs %s %s\n', fa{k}, class(a.(fa{k})), mat2str(size(a.(fa{k}))), class(b.(fa{k})), mat2str(size(b.(fa{k}))));
        end
    end
end


function g = read_golden(path)
    g = struct();
    fid = fopen(path, 'r');
    while true
        line = fgetl(fid);
        if ~ischar(line)
            break;
        end
        parts = strsplit(strtrim(line));
        if numel(parts) == 2
            g.(parts{1}) = uint8(hex2dec(reshape(parts{2}, 2, []).'));
        end
    end
    fclose(fid);
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

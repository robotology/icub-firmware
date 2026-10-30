function test_golden()
%TEST_GOLDEN the frames built and decoded in MATLAB must be the bytes of golden.txt, which the Python tools of host/shared
%   (sgctl.py, amc_gtw.py, tethercodec.py) produce w/ tools/make_golden.py; state_cpp comes from the C++ of amc.gtw
%   (appl.gateway.01/test/host). run it from the folder simulink:  sgact_setup; test_golden
%   it runs w/out Simulink (also in Octave)

    here = fileparts(mfilename('fullpath'));
    g = read_golden(fullfile(here, 'golden.txt'));
    c = sgact_const();
    mmcfg = [1, 0.1, 0, 0.5, 0.05, 0, 1, 2, 3, 3, 8, 0, 4096, -5];
    n = 0;
    total = 0;

    [n, total] = add(n, total, same('mit_a', sgact_mit_frame(0, 1, 0, 1, 0, 7, 0), g.mit_a));
    [n, total] = add(n, total, same('mit_b', sgact_mit_frame(5, 0.5, 0.5, 0, 0.1, 123456, 0), g.mit_b));
    [n, total] = add(n, total, same('mit_c', sgact_mit_frame(25, 3, -12.56, -10, -3, 4294967295, 0), g.mit_c));
    [n, total] = add(n, total, same('hb', sgact_hb_frame(0), g.hb));
    [n, total] = add(n, total, same('rpc_enable', sgact_run_frame(209, 1, 0), g.rpc_enable));
    [n, total] = add(n, total, same('rpc_disable', sgact_run_frame(210, 1, 0), g.rpc_disable));
    [n, total] = add(n, total, same('rpc_pax_enable', sgact_run_frame(212, 1, 0), g.rpc_pax_enable));
    [n, total] = add(n, total, same('rpc_pax_disable', sgact_run_frame(213, 1, 0), g.rpc_pax_disable));

    % ---- the motor_macro: the command frame w/ the target, the rpc w/ an argument
    [f, len] = sgact_cmd_frame(true, [5, 0.5, 0.5, 0, 0.1], 7, true, 90, c.CM_SPEEDVOLTAGE, 7, 0);
    [n, total] = add(n, total, same('cmd_mit_target', f(1:len), g.cmd_mit_target));
    [f, len] = sgact_cmd_frame(false, zeros(1, 5), 0, true, 0.5, c.CM_CURRENT, 8, 0);
    [n, total] = add(n, total, same('cmd_target', f(1:len), g.cmd_target));
    [f, len] = sgact_cmd_frame(true, [5, 0.5, 0.5, 0, 0.1], 123456, false, 0, 0, 0, 0);
    [n, total] = add(n, total, same('cmd_frame w/ the MIT only = mit_b', f(1:len), g.mit_b));
    [f, len] = sgact_cmd_frame(false, zeros(1, 5), 0, false, 0, 0, 0, 0);
    [n, total] = add(n, total, same('cmd_frame w/ nothing = hb', f(1:len), g.hb));
    names = {'rpc_motor_pids', 'rpc_motor_setup', 'rpc_motor_mode', 'rpc_pax_enable'};
    ops = [c.OP_MOTOR_PIDS, c.OP_MOTOR_SETUP, c.OP_MOTOR_MODE, c.OP_PAX_ENABLE];
    for k = 1:numel(ops)
        [arg, sz] = sgact_rpc_arg(ops(k), c.CM_SPEEDVOLTAGE, mmcfg);
        [f, len] = sgact_rpc_frame(ops(k), 1, arg, sz, 0);
        [n, total] = add(n, total, same(names{k}, f(1:len), g.(names{k})));
    end

    [arg, sz] = sgact_rpc_arg(c.OP_GW_FAULT, 1, mmcfg);
    [f, len] = sgact_rpc_frame(c.OP_GW_FAULT, 1, arg, sz, 0);
    [n, total] = add(n, total, same('rpc_gw_fault', f(1:len), g.rpc_gw_fault));

    % ---- the panel link: CTL version 3, STATUS version 4
    [n, total] = add(n, total, same('ctl3_a', sgact_ctl_frame(3, 1, 3, 9, 1234, 305419896, 10, [1, 1], mmcfg), g.ctl3_a));
    % the STATUS v4: the same bytes, but for the crc, which is the one of the current sgact_panel.toml
    sup = sgact_sup_default();
    sup.state = 3; sup.mode = 1; sup.inited = 3; sup.err = 0; sup.result = 3; sup.reason = 0; sup.action = 3;
    sup.reqid = 1234; sup.cnt = 9; sup.trun = 1.5;
    fb = sgact_fb_default();
    fb.has_motor_cubemars_status = true; fb.motor_cubemars_status = 2; fb.motor_cubemars_pos = 0.25; fb.motor_cubemars_vel = -0.5; fb.motor_cubemars_current = 0.125;
    fb.sensor_torque_mx = 1; fb.sensor_torque_my = -2; fb.sensor_torque_mz = 3; fb.motor_macro_pos = 0.5; fb.motor_macro_vel = 1.5; fb.motor_macro_current = 0.75;
    fb.motor_macro_mode = c.CM_SPEEDVOLTAGE; fb.motor_macro_configured = 3; fb.motor_macro_emulated = 1; fb.motor_macro_pwm = 12.5;
    fb.motor_macro_faults = c.FAULT_EXTERNAL; fb.nrecv = 99; fb.gw(7) = 1; fb.gw(4) = 2;
    fb.sensor_position_pos = -0.25; fb.sensor_position_counter = 4321; fb.sensor_position_flags = 3; fb.gw_extfault = 1;
    st3 = sgact_status_frame(sup, fb, 100);
    golden = g.status4_a;
    spec = sgact_spec();
    golden(21:24) = sgact_bytes(spec.CRC);
    [n, total] = add(n, total, same('status4_a (crc of the current TOML)', st3, golden));

    % the CTL decoded: the fields of ctl3_a (its crc is not the one of the TOML)
    [ok, r] = sgact_ctl_decode(g.ctl3_a, numel(g.ctl3_a));
    [n, total] = add(n, total, check('ctl3_a decoded', ok && ~r.crc_ok && r.action == 3 && r.mode == 1 && r.devices == 3 && r.cnt == 9 ...
                     && r.id == 1234 && r.duration == 10 && isequal(r.params, [1, 1, 0, 0, 0, 0, 0, 0]) ...
                     && max(abs(r.mmcfg - mmcfg)) < 1e-6));
    [n, total] = add(n, total, same('ctl3_extfault', sgact_ctl_frame(0, 0, 0, 10, 1234, 305419896, 0, [], mmcfg, 1), g.ctl3_extfault));
    [ok, r] = sgact_ctl_decode(g.ctl3_extfault, numel(g.ctl3_extfault));
    [n, total] = add(n, total, check('ctl3_extfault decoded: extfault', ok && r.extfault));
    [ok, r] = sgact_ctl_decode(sgact_ctl_frame(1, 0, 3, 1, 7, spec.CRC, 0, []), c.CTL_SIZE);
    [n, total] = add(n, total, check('a CTL with the crc of the TOML: crc_ok', ok && r.crc_ok && r.action == 1));

    % ---- the state frame of the firmware (made by amcmj1_gwlink.cpp on a PC: test/host/test_gwlink --golden)
    s = sgact_state_decode([g.state_cpp; zeros(100, 1, 'uint8')], numel(g.state_cpp));
    ok = s.valid && s.time == 777 && s.has_motor_cubemars_status && s.motor_cubemars_status == 2 && s.motor_cubemars_commands == 0 && s.motor_cubemars_watchdogs == 1 ...
         && s.motor_cubemars_pos == 1.5 && s.motor_cubemars_vel == 0 && s.motor_cubemars_counter == 7 && s.sensor_torque_mx == 1 && s.sensor_torque_my == -2 && s.sensor_torque_mz == 3 ...
         && s.sensor_torque_counter == 9 && isequal(s.gw, [13, 3, 0, 0, 1, 0, 2, 0, 0]);
    [n, total] = add(n, total, check('state_cpp decoded: motor_cubemars, sensor_torque, gateway', ok));
    ok = s.has_motor_macro && s.motor_macro_current == 0.5 && abs(s.motor_macro_vel - pi / 2) < 1e-12 && abs(s.motor_macro_pos - pi / 4) < 1e-12 ...
         && s.motor_macro_pwm == 12.5 && s.motor_macro_faults == 128 && s.motor_macro_counter == 4 && s.motor_macro_mode == 10 ...
         && s.motor_macro_configured == 3 && s.motor_macro_emulated == 1;
    [n, total] = add(n, total, check('state_cpp decoded: the motor_macro (deg -> rad)', ok));
    ok = s.has_sensor_position && abs(s.sensor_position_pos + 12.5 * pi / 180) < 1e-12 && s.sensor_position_counter == 1000 ...
         && s.sensor_position_errors == 2 && s.sensor_position_flags == 3;
    [n, total] = add(n, total, check('state_cpp decoded: the sensor_position (deg -> rad)', ok));
    bad = g.state_cpp; bad(1) = 0;
    s1 = sgact_state_decode(bad, numel(bad));
    s2 = sgact_state_decode(g.state_cpp, numel(g.state_cpp) - 1);
    s3 = sgact_state_decode(g.state_cpp, 0);
    [n, total] = add(n, total, check('state with a wrong signature is refused', ~s1.valid));
    [n, total] = add(n, total, check('state with a wrong length is refused', ~s2.valid));
    [n, total] = add(n, total, check('nothing received', ~s3.valid));

    fprintf('%d/%d golden checks passed\n', n, total);
    if n ~= total
        error('test_golden failed');
    end
end


function [n, total] = add(n, total, ok)
    n = n + ok;
    total = total + 1;
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
            hex = parts{2};
            g.(parts{1}) = uint8(hex2dec(reshape(hex, 2, []).'));
        end
    end
    fclose(fid);
end


function ok = same(name, got, expected)
    ok = isequal(uint8(got(:)), expected(:));
    if ok
        fprintf('  PASS  %s\n', name);
    else
        fprintf('  FAIL  %s\n    got      %s\n    expected %s\n', name, lower(reshape(dec2hex(got(:)).', 1, [])), lower(reshape(dec2hex(expected(:)).', 1, [])));
    end
end


function ok = check(name, cond)
    ok = logical(cond);
    fprintf('  %s  %s\n', ternary(ok, 'PASS', 'FAIL'), name);
end


function s = ternary(c, a, b)
    if c
        s = a;
    else
        s = b;
    end
end

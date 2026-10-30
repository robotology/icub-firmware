function sub = sl_mode_subsystem(parent, name, kind, pos)
%SL_MODE_SUBSYSTEM the subsystem mode_<name> of a mode w/ impl = simulink, made of Simulink blocks. kind:
%   'template'  what sgact_sync_modes creates for a new mode: it reads run and fb, and it does nothing (valid 0, done 1)
%   'mit'       the parameters p(1:5) = pos vel kp kd torque go to the MIT of the motor_cubemars (motor_cubemars_user)
%   'spark'     as 'mit', plus the motor_macro at 0 (mmtarget, rad/s in speedvoltage), and p(6) forfutureuse; all the
%               feedback the demo needs is already selected from fb (spark_dual_motor_actuation)
%   in all of them the stop is over at once (done 1): then the supervisor disables the motor_cubemars, idles the motor_macro.
%   the control engineer replaces the blocks between the Bus Selectors and "pack sp". "pack sp" makes the bus SGACT_SP
%   from 8 scalars: valid kp kd pos vel torque mmtarget done (see matlab/sgact_sp_default.m)
    sub = sl_enabled_subsystem(parent, ['mode_' name], pos);

    % ---- what the mode reads: the parameters p(1:8) of run (SI, order of the TOML), the feedback of fb
    add_block('simulink/Signal Routing/Bus Selector', [sub '/run sel'], 'Position', [120 90 125 130]);
    set_param([sub '/run sel'], 'OutputSignals', 'p,phase');
    add_line(sub, 'run/1', 'run sel/1', 'autorouting', 'smart');
    add_block('simulink/Signal Routing/Demux', [sub '/p'], 'Outputs', '8', 'Position', [200 60 205 220]);
    add_line(sub, 'run sel/1', 'p/1', 'autorouting', 'smart');
    add_block('simulink/Sinks/Terminator', [sub '/phase not used'], 'Position', [200 240 215 255]);
    add_line(sub, 'run sel/2', 'phase not used/1', 'autorouting', 'smart');
    if strcmp(kind, 'spark')
        fbsig = {'motor_cubemars_pos', 'motor_cubemars_vel', 'motor_cubemars_current', ...
                 'motor_macro_pos', 'motor_macro_vel', 'motor_macro_current', ...
                 'sensor_torque_mx', 'sensor_torque_my', 'sensor_torque_mz', 'sensor_position_pos'};
    else
        fbsig = {'motor_cubemars_pos', 'motor_cubemars_vel'};
    end
    nfb = numel(fbsig);
    add_block('simulink/Signal Routing/Bus Selector', [sub '/fb sel'], 'Position', [120 300 125 300 + 25 * nfb]);
    set_param([sub '/fb sel'], 'OutputSignals', strjoin(fbsig, ','));
    add_line(sub, 'fb/1', 'fb sel/1', 'autorouting', 'smart');
    for k = 1:nfb
        add_block('simulink/Sinks/Terminator', sprintf('%s/%s', sub, fbsig{k}), 'Position', [220 290 + 25 * k 235 305 + 25 * k]);
        add_line(sub, sprintf('fb sel/%d', k), [fbsig{k} '/1'], 'autorouting', 'smart');
    end

    % ---- what the mode gives: pack sp
    pack = [sub '/pack sp'];
    add_block('simulink/User-Defined Functions/MATLAB Function', pack, 'Position', [560 120 680 300]);
    sl_matlab_function(pack, sprintf(['function sp = pack(valid, kp, kd, pos, vel, torque, mmtarget, done)\n', ...
                       '%%#codegen\n', ...
                       '%% the bus SGACT_SP from 8 scalars. valid: a MIT for the motor_cubemars (kp kd pos vel torque, SI);\n', ...
                       '%% mmtarget: the setpoint of the motor_macro (modes w/ a controlmode); done: (stop) the exit is over\n', ...
                       'sp = sgact_sp_default();\n', ...
                       'sp.valid = valid ~= 0;\n', ...
                       'sp.kp = double(kp); sp.kd = double(kd); sp.pos = double(pos); sp.vel = double(vel); sp.torque = double(torque);\n', ...
                       'sp.mmtarget = double(mmtarget);\n', ...
                       'sp.done = done ~= 0;\n']), {'sp', 'Bus: SGACT_SP'});
    add_line(sub, 'pack sp/1', 'sp/1', 'autorouting', 'smart');

    % ---- in between: the control. the parameters: p/1 pos, p/2 vel, p/3 kp, p/4 kd, p/5 torque (p/6 forfutureuse)
    names = {'valid', 'kp', 'kd', 'pos', 'vel', 'torque', 'mmtarget', 'done'};
    switch kind
        case {'mit', 'spark'}
            from = struct('valid', '1', 'kp', 'p/3', 'kd', 'p/4', 'pos', 'p/1', 'vel', 'p/2', 'torque', 'p/5', 'mmtarget', '0', 'done', '1');
            used = 1:5;
        otherwise
            from = struct('valid', '0', 'kp', '0', 'kd', '0', 'pos', '0', 'vel', '0', 'torque', '0', 'mmtarget', '0', 'done', '1');
            used = [];
    end
    for k = 1:8
        src = from.(names{k});
        if any(src == '/')
            add_line(sub, src, sprintf('pack sp/%d', k), 'autorouting', 'smart');
        else
            blk = [names{k} ' ='];
            add_block('simulink/Sources/Constant', [sub '/' blk], 'Value', src, 'Position', [420 110 + 25 * k 450 125 + 25 * k]);
            add_line(sub, [blk '/1'], sprintf('pack sp/%d', k), 'autorouting', 'smart');
        end
    end
    for k = setdiff(1:8, used)
        tname = sprintf('p%d not used', k);
        if strcmp(kind, 'spark') && k == 6
            tname = 'p6 forfutureuse';
        end
        add_block('simulink/Sinks/Terminator', [sub '/' tname], 'Position', [260 40 + 20 * k 275 55 + 20 * k]);
        add_line(sub, sprintf('p/%d', k), [tname '/1'], 'autorouting', 'smart');
    end

    try
        note = Simulink.Annotation([sub '/note']);
        switch kind
            case 'mit'
                note.Text = sprintf(['mode %s: the MIT of the motor_cubemars from the parameters\n', ...
                                     'p(1:5) = pos vel kp kd torque (SI: rad, rad/s). stop: done at once, then the motor_cubemars is disabled.\n', ...
                                     'replace the blocks between the selectors and "pack sp" w/ the control.'], name);
            case 'spark'
                note.Text = sprintf(['mode %s (the demo): as built, the MIT of the motor_cubemars from p(1:5) = pos vel kp kd torque\n', ...
                                     'and the motor_macro at 0 rad/s (speedvoltage: mmtarget in rad/s). p(6): forfutureuse.\n', ...
                                     'fb sel: the feedback of both motors and of the two sensors (rad, rad/s, A, N*m).\n', ...
                                     'stop: done at once, then the motor_cubemars is disabled and the motor_macro goes idle.'], name);
            otherwise
                note.Text = sprintf(['mode %s: made by sgact_sync_modes. put the control between the selectors and "pack sp".\n', ...
                                     'run.p(1:8): the parameters of the TOML in SI; run.phase: 1 run, 2 stop; fb: the feedback.\n', ...
                                     'as it is, it does nothing: valid 0, done 1.'], name);
        end
        note.Position = [40 600 760 660];
    catch
    end
end

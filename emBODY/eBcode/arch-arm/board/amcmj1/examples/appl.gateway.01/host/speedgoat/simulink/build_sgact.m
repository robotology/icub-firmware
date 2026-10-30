function build_sgact(force)
%BUILD_SGACT creates the model sgact.slx ONCE: from then on it is edited by hand.
%
%       [amc.gtw RX] --fb-----+--> [Controller] --cmd----> [amc.gtw TX] --nsent--+
%       [panel RX]   --req----|          |                                       |
%                             |          +-----------sup----> [panel TX] <-------+
%                             +--------------------- fb ---------^
%
%   amc.gtw RX / TX, panel RX / TX : library links to sgact_lib.slx (the protocol: build_sgact_lib.m). Do not edit
%                    them here. W/out the panel: another requester must produce the bus SGACT_REQ.
%   Controller     : SUP-A (matlab/sgact_sup_a.m: requests init / deinit / start / stop, watchdogs, state machine, rpc)
%                    -> run -> USER CONTROL -> sp -> SUP-B (matlab/sgact_sup_b.m: the gate) -> cmd; SUP-A -> sup.
%                    USER CONTROL: "standard modes" (the modes w/ impl = matlab: matlab/sgact_mode_<name>.m) and one
%                    enabled subsystem mode_<name> per mode w/ impl = simulink (the control engineer: Simulink blocks),
%                    merged. done of sp goes back to SUP-A through a unit delay. motor_cubemars_user and
%                    spark_dual_motor_actuation start w/ the MIT of their parameters (tools/sl_mode_kind.m). After a change of the modes of the TOML: build_sgact_lib, then sgact_sync_modes
%                    (it adds the new subsystems and wires them, w/out touching the existing ones).
%   logged signals : state, mode, inited, err, sp_motor_cubemars_kp ... sp_motor_cubemars_torque (the MIT sent to the motor_cubemars),
%                    sp_motor_macro_mode, sp_motor_macro_target (the setpoint of the motor_macro, SI), motor_cubemars_pos vel current,
%                    sensor_torque_mx my mz, motor_macro_pos vel current pwm mode faults, sensor_position_pos (from amc.gtw,
%                    rad and rad/s)
%
%   build_sgact         refuses to overwrite an existing sgact.slx
%   build_sgact(true)   overwrites it (the Controller you edited, and the subsystems of USER CONTROL, are lost)

    if nargin < 1
        force = false;
    end
    mdl = 'sgact';
    lib = 'sgact_lib';
    here = fileparts(mfilename('fullpath'));
    addpath(here, fullfile(here, 'matlab'), fullfile(here, 'tools'));
    target = fullfile(here, [mdl '.slx']);
    if exist(target, 'file') && ~force
        error('build:exists', ['%s exists and it is edited by hand: it is not overwritten.\n' ...
              'To change the protocol run build_sgact_lib. To start again from the example: build_sgact(true)'], target);
    end
    if ~exist(fullfile(here, [lib '.slx']), 'file')
        build_sgact_lib();
    end
    sgact_buses();
    c = sgact_const();
    ts = num2str(c.TS);

    load_system('simulink');
    load_system(lib);
    if bdIsLoaded(mdl)
        close_system(mdl, 0);
    end
    new_system(mdl);
    set_param(mdl, 'SolverType', 'Fixed-step', 'Solver', 'FixedStepDiscrete', 'FixedStep', ts, 'StopTime', 'inf');
    % block parameters inlined: nothing is tuned on the target (the panel drives the modes). It also avoids that SLRT tries
    % to set the bus-typed initial outputs of USER CONTROL at load ("InitialOutput ... Incorrect data type")
    sl_setp(mdl, 'DefaultParameterBehavior', 'Inlined');
    try
        set_param(mdl, 'SystemTargetFile', 'speedgoat.tlc');
    catch e
        warning('build:target', 'cannot select speedgoat.tlc (%s): select it by hand in the Configuration Parameters', e.message);
    end
    set_param(mdl, 'PreLoadFcn', sprintf('addpath(fullfile(fileparts(which(''%s'')), ''matlab'')); sgact_buses;', mdl));

    % ---- the protocol: library links
    add_block([lib '/amc.gtw RX'], [mdl '/amc.gtw RX'], 'Position', [60 80 220 140]);
    add_block([lib '/panel RX'], [mdl '/panel RX'], 'Position', [60 200 220 260]);
    add_block([lib '/amc.gtw TX'], [mdl '/amc.gtw TX'], 'Position', [700 80 860 140]);
    add_block([lib '/panel TX'], [mdl '/panel TX'], 'Position', [700 200 860 280]);

    % ---- the Controller: SUP-A, USER CONTROL, SUP-B, the unit delay of done
    ctrl = [mdl '/Controller'];
    add_block('built-in/Subsystem', ctrl, 'Position', [380 100 540 200], 'BackgroundColor', 'yellow');
    add_block('built-in/Inport', [ctrl '/fb'], 'Position', [40 80 70 94]);
    sl_setp([ctrl '/fb'], 'OutDataTypeStr', 'Bus: SGACT_FB', 'BusOutputAsStruct', 'on');
    add_block('built-in/Inport', [ctrl '/req'], 'Position', [40 160 70 174]);
    sl_setp([ctrl '/req'], 'OutDataTypeStr', 'Bus: SGACT_REQ', 'BusOutputAsStruct', 'on');
    add_block('simulink/Sources/Digital Clock', [ctrl '/Clock'], 'SampleTime', ts, 'Position', [40 300 80 330]);
    add_block('simulink/User-Defined Functions/MATLAB Function', [ctrl '/SUP-A'], 'Position', [180 60 360 300]);
    sl_matlab_function([ctrl '/SUP-A'], sprintf(['function [run, cmda, sup] = supa(fb, req, done, t)\n', ...
                        '%%#codegen\n', ...
                        '%% the requests, the watchdogs, the state machine, the rpc: see matlab/sgact_sup_a.m\n', ...
                        '[run, cmda, sup] = sgact_sup_a(fb, req, done, t);\n']), ...
                       {'fb', 'Bus: SGACT_FB'; 'req', 'Bus: SGACT_REQ'; 'run', 'Bus: SGACT_RUN'; 'cmda', 'Bus: SGACT_CMD'; 'sup', 'Bus: SGACT_SUP'});
    add_block('simulink/User-Defined Functions/MATLAB Function', [ctrl '/SUP-B'], 'Position', [760 60 920 300]);
    sl_matlab_function([ctrl '/SUP-B'], sprintf(['function cmd = supb(cmda, sup, run, sp)\n', ...
                        '%%#codegen\n', ...
                        '%% the gate: which setpoints of the mode leave. see matlab/sgact_sup_b.m\n', ...
                        'cmd = sgact_sup_b(cmda, sup, run, sp);\n']), ...
                       {'cmda', 'Bus: SGACT_CMD'; 'sup', 'Bus: SGACT_SUP'; 'run', 'Bus: SGACT_RUN'; 'sp', 'Bus: SGACT_SP'; 'cmd', 'Bus: SGACT_CMD'});

    % USER CONTROL: the standard modes and one subsystem per mode w/ impl = simulink (the control engineer)
    uc = [ctrl '/USER CONTROL'];
    add_block('built-in/Subsystem', uc, 'Position', [460 320 640 420], 'BackgroundColor', 'cyan');
    add_block('built-in/Inport', [uc '/run'], 'Position', [40 100 70 114]);
    sl_setp([uc '/run'], 'OutDataTypeStr', 'Bus: SGACT_RUN', 'BusOutputAsStruct', 'on');
    add_block('built-in/Inport', [uc '/fb'], 'Position', [40 300 70 314]);
    sl_setp([uc '/fb'], 'OutDataTypeStr', 'Bus: SGACT_FB', 'BusOutputAsStruct', 'on');
    add_block('built-in/Outport', [uc '/sp'], 'Position', [860 80 890 94]);
    std = sl_enabled_subsystem(uc, 'standard modes', [420 90 600 170]);
    add_block('simulink/User-Defined Functions/MATLAB Function', [std '/modes'], 'Position', [300 120 480 300]);
    sl_matlab_function([std '/modes'], sprintf(['function sp = modes(run, fb)\n', ...
                        '%%#codegen\n', ...
                        '%% the modes w/ impl = matlab of sgact_panel.toml: matlab/sgact_mode_<name>.m\n', ...
                        'sp = sgact_std_modes(run, fb);\n']), ...
                       {'run', 'Bus: SGACT_RUN'; 'fb', 'Bus: SGACT_FB'; 'sp', 'Bus: SGACT_SP'});
    add_line(std, 'run/1', 'modes/1', 'autorouting', 'smart');
    add_line(std, 'fb/1', 'modes/2', 'autorouting', 'smart');
    add_line(std, 'modes/1', 'sp/1', 'autorouting', 'smart');
    clear sgact_modes_info
    info = sgact_modes_info();
    sim = info(strcmp({info.impl}, 'simulink'));
    for k = 1:numel(sim)
        sl_mode_subsystem(uc, sim(k).name, sl_mode_kind(sim(k).name), [420 60 600 140]);
    end
    sl_user_control_wire(uc, {sim.name}, [sim.id]);

    % done of the previous step: the unit delay breaks the loop SUP-A -> USER CONTROL -> SUP-A
    add_block('simulink/Signal Routing/Bus Selector', [ctrl '/done sel'], 'Position', [700 440 705 470]);
    set_param([ctrl '/done sel'], 'OutputSignals', 'done');
    add_block('simulink/Discrete/Unit Delay', [ctrl '/done z-1'], 'InitialCondition', '0', 'Position', [560 500 600 530], ...
              'Orientation', 'left');
    add_block('built-in/Outport', [ctrl '/cmd'], 'Position', [1000 100 1030 114]);
    add_block('built-in/Outport', [ctrl '/sup'], 'Position', [1000 220 1030 234]);
    add_line(ctrl, 'fb/1', 'SUP-A/1', 'autorouting', 'smart');
    add_line(ctrl, 'req/1', 'SUP-A/2', 'autorouting', 'smart');
    add_line(ctrl, 'done z-1/1', 'SUP-A/3', 'autorouting', 'smart');
    add_line(ctrl, 'Clock/1', 'SUP-A/4', 'autorouting', 'smart');
    named(add_line(ctrl, 'SUP-A/1', 'USER CONTROL/1', 'autorouting', 'smart'), 'run');
    add_line(ctrl, 'fb/1', 'USER CONTROL/2', 'autorouting', 'smart');
    add_line(ctrl, 'SUP-A/2', 'SUP-B/1', 'autorouting', 'smart');
    add_line(ctrl, 'SUP-A/3', 'SUP-B/2', 'autorouting', 'smart');
    add_line(ctrl, 'SUP-A/1', 'SUP-B/3', 'autorouting', 'smart');
    named(add_line(ctrl, 'USER CONTROL/1', 'SUP-B/4', 'autorouting', 'smart'), 'sp');
    add_line(ctrl, 'USER CONTROL/1', 'done sel/1', 'autorouting', 'smart');
    add_line(ctrl, 'done sel/1', 'done z-1/1', 'autorouting', 'smart');
    add_line(ctrl, 'SUP-B/1', 'cmd/1', 'autorouting', 'smart');
    add_line(ctrl, 'SUP-A/3', 'sup/1', 'autorouting', 'smart');

    % ---- the wiring
    named(add_line(mdl, 'amc.gtw RX/1', 'Controller/1', 'autorouting', 'smart'), 'fb');
    named(add_line(mdl, 'panel RX/1', 'Controller/2', 'autorouting', 'smart'), 'req');
    named(add_line(mdl, 'Controller/1', 'amc.gtw TX/1', 'autorouting', 'smart'), 'cmd');
    named(add_line(mdl, 'Controller/2', 'panel TX/1', 'autorouting', 'smart'), 'sup');
    add_line(mdl, 'amc.gtw RX/1', 'panel TX/2', 'autorouting', 'smart');
    named(add_line(mdl, 'amc.gtw TX/1', 'panel TX/3', 'autorouting', 'smart'), 'nsent');

    % ---- what to look at: named scalar signals, logged
    sl_log_bus(mdl, 'Controller/2', 'log sup', {'state', 'mode', 'inited', 'err'}, '', [1000 300]);
    sl_log_bus(mdl, 'Controller/1', 'log cmd', {'motor_cubemars_kp', 'motor_cubemars_kd', 'motor_cubemars_pos', 'motor_cubemars_vel', 'motor_cubemars_torque', ...
               'motor_macro_mode', 'motor_macro_target'}, 'sp_', [1000 420]);
    sl_log_bus(mdl, 'amc.gtw RX/1', 'log fb', {'motor_cubemars_pos', 'motor_cubemars_vel', 'motor_cubemars_current', 'sensor_torque_mx', 'sensor_torque_my', 'sensor_torque_mz', ...
               'motor_macro_pos', 'motor_macro_vel', 'motor_macro_current', 'motor_macro_pwm', 'motor_macro_mode', 'motor_macro_faults', ...
               'sensor_position_pos'}, '', [380 340]);

    try
        note = Simulink.Annotation([mdl '/note']);
        note.Text = sprintf(['sgact. amc.gtw RX / TX, panel RX / TX: library sgact_lib (protocol, do not edit here: build_sgact_lib).\n', ...
                             'Controller: SUP-A (requests, state machine) -> USER CONTROL (the modes: the control engineer) -> SUP-B (gate).\n', ...
                             'A new mode w/ impl = simulink in sgact_panel.toml: build_sgact_lib, then sgact_sync_modes.\n', ...
                             'The buses: matlab/sgact_buses.m from the default structs (sgact_*_default.m).']);
        note.Position = [60 700 760 760];
    catch
    end

    save_system(mdl, target);
    open_system(mdl);
    fprintf('created %s\n', target);
end


function named(lh, name)
    set_param(lh, 'Name', name);
end

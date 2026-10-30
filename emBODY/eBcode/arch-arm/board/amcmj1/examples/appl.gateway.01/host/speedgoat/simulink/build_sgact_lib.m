function build_sgact_lib()
%BUILD_SGACT_LIB creates the library sgact_lib.slx: the four I/O blocks of sgact, one per peer and direction.
%
%   amc.gtw RX   output  fb      (bus SGACT_FB)      the last state frame of amc.gtw: motor_cubemars, sensor_torque, motor_macro, gateway
%                inside  UDP Receive ETH1:5003, MATLAB Function -> sgact_gtw_rx_step
%   amc.gtw TX   input   cmd     (bus SGACT_CMD)     the MIT for the motor_cubemars, the rpc to send
%                output  nsent                       the command frames sent so far
%                inside  MATLAB Function -> sgact_gtw_tx_step, UDP Send to amc.gtw :5001 (commands) and :5002 (rpc)
%   panel RX     output  req     (bus SGACT_REQ)     the last request of the panel (CTL v3)
%                inside  UDP Receive Host Link:6000, MATLAB Function -> sgact_panel_rx_step
%   panel TX     inputs  sup     (bus SGACT_SUP), fb (bus SGACT_FB), nsent
%                inside  MATLAB Function -> sgact_panel_tx_step, UDP Send to the panel :6001 (STATUS)
%
%   The models use the blocks through library links: run this script again after a change of the protocol or of
%   sgact_netcfg.m (the addresses are written in the UDP blocks) and every model that links them gets the change.
%   It overwrites sgact_lib.slx: never edit the library by hand. W/out the panel, delete panel RX and panel TX.
%   It first generates matlab/sgact_spec.m and sgact_modes_dispatch.m from sgact_panel.toml (sgact_gen_spec).
%
%   usage (from the folder speedgoat):   sgact_setup; build_sgact_lib

    lib = 'sgact_lib';
    here = fileparts(mfilename('fullpath'));
    addpath(fullfile(here, 'matlab'), fullfile(here, 'tools'));
    sgact_gen_spec();
    sgact_buses();
    c = sgact_const();
    n = sgact_netcfg();
    ts = num2str(c.TS);

    load_system('simulink');
    load_system('slrealtimeiplib');
    if bdIsLoaded(lib)
        close_system(lib, 0);
    end
    new_system(lib, 'Library');
    set_param(lib, 'PreLoadFcn', sprintf('addpath(fullfile(fileparts(which(''%s'')), ''matlab'')); sgact_buses;', lib));

    % ---- amc.gtw RX
    blk = [lib '/amc.gtw RX'];
    add_block('built-in/Subsystem', blk, 'Position', [60 60 220 140]);
    add_block('simulink/Sources/Digital Clock', [blk '/Clock'], 'SampleTime', ts, 'Position', [60 30 100 60]);
    add_block('slrealtimeiplib/UDP Receive', [blk '/state from amc.gtw'], 'Position', [40 100 240 180]);
    sl_setp([blk '/state from amc.gtw'], 'ipAddress', n.ETH1_IP, 'localPort', num2str(n.PORT_STATE), 'rcvWidth', num2str(c.STATE_MAX_SIZE), ...
            'rcvFmAny', 'off', 'fmAddress', n.AMCGTW_IP, 'sampleTime', ts);
    sl_setmax([blk '/state from amc.gtw']);
    add_block('simulink/Signal Attributes/Data Type Conversion', [blk '/bytes'], 'OutDataTypeStr', 'uint8', 'Position', [300 105 350 135]);
    add_block('simulink/User-Defined Functions/MATLAB Function', [blk '/rx'], 'Position', [420 40 600 200]);
    sl_matlab_function([blk '/rx'], sprintf(['function fb = rx(st, stlen, t)\n', '%%#codegen\n', ...
                                            '%% the state frame of amc.gtw -> the bus SGACT_FB: see matlab/sgact_gtw_rx_step.m\n', ...
                                            'fb = sgact_gtw_rx_step(st, stlen, t);\n']), {'fb', 'Bus: SGACT_FB'});
    add_block('built-in/Outport', [blk '/fb'], 'Position', [680 113 710 127]);
    add_line(blk, 'state from amc.gtw/1', 'bytes/1', 'autorouting', 'smart');
    add_line(blk, 'bytes/1', 'rx/1', 'autorouting', 'smart');
    add_line(blk, 'state from amc.gtw/2', 'rx/2', 'autorouting', 'smart');
    add_line(blk, 'Clock/1', 'rx/3', 'autorouting', 'smart');
    add_line(blk, 'rx/1', 'fb/1', 'autorouting', 'smart');

    % ---- amc.gtw TX
    blk = [lib '/amc.gtw TX'];
    add_block('built-in/Subsystem', blk, 'Position', [360 60 520 140]);
    add_block('built-in/Inport', [blk '/cmd'], 'Position', [40 120 70 134]);
    sl_setp([blk '/cmd'], 'OutDataTypeStr', 'Bus: SGACT_CMD', 'BusOutputAsStruct', 'on');
    add_block('simulink/Sources/Digital Clock', [blk '/Clock'], 'SampleTime', ts, 'Position', [40 200 80 230]);
    add_block('simulink/User-Defined Functions/MATLAB Function', [blk '/tx'], 'Position', [200 60 400 320]);
    sl_matlab_function([blk '/tx'], sprintf(['function [cmdf, cmd_len, cmd_go, rpc, rpc_len, rpc_go, nsent] = tx(cmd, t)\n', '%%#codegen\n', ...
                                            '%% the bus SGACT_CMD -> the frames to amc.gtw: see matlab/sgact_gtw_tx_step.m\n', ...
                                            '[cmdf, cmd_len, cmd_go, rpc, rpc_len, rpc_go, nsent] = sgact_gtw_tx_step(cmd, t);\n']), ...
                       {'cmd', 'Bus: SGACT_CMD'});
    add_line(blk, 'cmd/1', 'tx/1', 'autorouting', 'smart');
    add_line(blk, 'Clock/1', 'tx/2', 'autorouting', 'smart');
    sl_sender(blk, 'cmd to amc.gtw', [480 60 640 140], n.ETH1_IP, n.LOCAL_CMD, n.AMCGTW_IP, n.PORT_CMD);
    sl_sender(blk, 'rpc to amc.gtw', [480 190 640 270], n.ETH1_IP, n.LOCAL_RPC, n.AMCGTW_IP, n.PORT_RPC);
    names = {'cmd to amc.gtw', 'rpc to amc.gtw'};
    for i = 1:2
        add_line(blk, sprintf('tx/%d', 3 * i - 2), [names{i} '/1'], 'autorouting', 'smart');
        add_line(blk, sprintf('tx/%d', 3 * i - 1), [names{i} '/2'], 'autorouting', 'smart');
        add_line(blk, sprintf('tx/%d', 3 * i), [names{i} '/Enable'], 'autorouting', 'smart');
    end
    add_block('built-in/Outport', [blk '/nsent'], 'Position', [480 313 510 327]);
    add_line(blk, 'tx/7', 'nsent/1', 'autorouting', 'smart');

    % ---- panel RX
    blk = [lib '/panel RX'];
    add_block('built-in/Subsystem', blk, 'Position', [60 200 220 280]);
    add_block('simulink/Sources/Digital Clock', [blk '/Clock'], 'SampleTime', ts, 'Position', [60 30 100 60]);
    add_block('slrealtimeiplib/UDP Receive', [blk '/CTL from panel'], 'Position', [40 100 240 180]);
    sl_setp([blk '/CTL from panel'], 'ipAddress', n.HOSTLINK_IP, 'localPort', num2str(n.PORT_CTL), 'rcvWidth', num2str(c.CTL_SIZE + 28), ...
            'rcvFmAny', 'on', 'sampleTime', ts);
    sl_setmax([blk '/CTL from panel']);
    add_block('simulink/Signal Attributes/Data Type Conversion', [blk '/bytes'], 'OutDataTypeStr', 'uint8', 'Position', [300 105 350 135]);
    add_block('simulink/User-Defined Functions/MATLAB Function', [blk '/rx'], 'Position', [420 40 600 200]);
    sl_matlab_function([blk '/rx'], sprintf(['function req = rx(ctl, ctllen, t)\n', '%%#codegen\n', ...
                                            '%% the CTL v3 of the panel -> the bus SGACT_REQ: see matlab/sgact_panel_rx_step.m\n', ...
                                            'req = sgact_panel_rx_step(ctl, ctllen, t);\n']), {'req', 'Bus: SGACT_REQ'});
    add_block('built-in/Outport', [blk '/req'], 'Position', [680 113 710 127]);
    add_line(blk, 'CTL from panel/1', 'bytes/1', 'autorouting', 'smart');
    add_line(blk, 'bytes/1', 'rx/1', 'autorouting', 'smart');
    add_line(blk, 'CTL from panel/2', 'rx/2', 'autorouting', 'smart');
    add_line(blk, 'Clock/1', 'rx/3', 'autorouting', 'smart');
    add_line(blk, 'rx/1', 'req/1', 'autorouting', 'smart');

    % ---- panel TX
    blk = [lib '/panel TX'];
    add_block('built-in/Subsystem', blk, 'Position', [360 200 520 280]);
    add_block('built-in/Inport', [blk '/sup'], 'Position', [40 80 70 94]);
    sl_setp([blk '/sup'], 'OutDataTypeStr', 'Bus: SGACT_SUP', 'BusOutputAsStruct', 'on');
    add_block('built-in/Inport', [blk '/fb'], 'Position', [40 140 70 154]);
    sl_setp([blk '/fb'], 'OutDataTypeStr', 'Bus: SGACT_FB', 'BusOutputAsStruct', 'on');
    add_block('built-in/Inport', [blk '/nsent'], 'Position', [40 200 70 214]);
    add_block('simulink/Sources/Digital Clock', [blk '/Clock'], 'SampleTime', ts, 'Position', [40 250 80 280]);
    add_block('simulink/User-Defined Functions/MATLAB Function', [blk '/tx'], 'Position', [200 60 400 300]);
    sl_matlab_function([blk '/tx'], sprintf(['function [status, status_len, status_go] = tx(sup, fb, nsent, t)\n', '%%#codegen\n', ...
                                            '%% the buses -> the STATUS v3 to the panel: see matlab/sgact_panel_tx_step.m\n', ...
                                            '[status, status_len, status_go] = sgact_panel_tx_step(sup, fb, nsent, t);\n']), ...
                       {'sup', 'Bus: SGACT_SUP'; 'fb', 'Bus: SGACT_FB'});
    add_line(blk, 'sup/1', 'tx/1', 'autorouting', 'smart');
    add_line(blk, 'fb/1', 'tx/2', 'autorouting', 'smart');
    add_line(blk, 'nsent/1', 'tx/3', 'autorouting', 'smart');
    add_line(blk, 'Clock/1', 'tx/4', 'autorouting', 'smart');
    sl_sender(blk, 'STATUS to panel', [480 120 640 200], n.HOSTLINK_IP, n.LOCAL_STATUS, n.PANEL_IP, n.PORT_STATUS);
    add_line(blk, 'tx/1', 'STATUS to panel/1', 'autorouting', 'smart');
    add_line(blk, 'tx/2', 'STATUS to panel/2', 'autorouting', 'smart');
    add_line(blk, 'tx/3', 'STATUS to panel/Enable', 'autorouting', 'smart');

    try
        note = Simulink.Annotation([lib '/note']);
        note.Text = sprintf(['sgact_lib: made by build_sgact_lib.m, do not edit by hand.\n', ...
                             'amc.gtw (ETH1 %s): commands to %s:%d, rpc :%d, state on %d.   panel (Host Link %s): CTL on %d, STATUS to %s:%d.'], ...
                            n.ETH1_IP, n.AMCGTW_IP, n.PORT_CMD, n.PORT_RPC, n.PORT_STATE, n.HOSTLINK_IP, n.PORT_CTL, n.PANEL_IP, n.PORT_STATUS);
        note.Position = [60 320 700 370];
    catch
    end

    save_system(lib, fullfile(here, [lib '.slx']));
    close_system(lib);
    fprintf('created %s\n', fullfile(here, [lib '.slx']));
end

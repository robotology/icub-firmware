function r = sgact_req_default()
%SGACT_REQ_DEFAULT a request to the supervisor, before any: the template of the bus SGACT_REQ
%   today the requests come from the panel (panel RX decodes its CTL); any other source must produce the same bus
%     have        a requester spoke at least once
%     alive       it spoke in the last KEEPALIVE s (sgact_panel.toml): w/out it the running mode is stopped (err 3)
%     crc_ok      it uses the same sgact_panel.toml (CRC): else every request is rejected (reason 5)
%     cnt         counter of its packets (echoed in the STATUS)
%     id          the id of the request: the supervisor acts when it changes, so a repeated packet does not act twice
%     action      0 none, 1 init, 2 deinit, 3 start, 4 stop (sgact_spec: ACT_*)
%     devices     mask (init, deinit): bit0 motor_cubemars, bit1 sensor_torque, bit2 motor_macro, bit3 sensor_position (sgact_spec: DEV_*)
%     mode        id of the mode (start; while it runs: the same mode with new params updates them)
%     duration    [s] of the run, 0 = until stop
%     params      the parameters of the mode, in the order of sgact_panel.toml
%     extfault    the EXTFAULT of the requester is pressed (flags bit 0 of the CTL): sgact stops all the motors and refuses
%                 every request until it is released, then it goes idle
%     mmcfg       the config of the motor_macro (init of the motor_macro): curkp curki curkd velkp velki velkd nominal peak
%                 overload flags poles entlr rotencres rotindexoff
    s = sgact_spec();
    r = struct('have', false, 'alive', false, 'crc_ok', false, 'cnt', 0, 'id', 0, 'action', 0, 'devices', 0, ...
               'mode', 0, 'duration', 0, 'params', zeros(1, s.MAX_PARAMS), 'mmcfg', zeros(1, 14), 'extfault', false);
end

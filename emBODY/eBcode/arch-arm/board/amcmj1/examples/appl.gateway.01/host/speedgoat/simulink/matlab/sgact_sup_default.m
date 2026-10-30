function u = sgact_sup_default()
%SGACT_SUP_DEFAULT what the supervisor tells: the template of the bus SGACT_SUP (panel TX makes the STATUS with it)
%     have        a requester spoke: send it the STATUS
%     state       0 idle, 1 initialising, 2 deinitialising, 3 running, 4 stopping (sgact_const: ST_*)
%     mode        the mode running or stopping, 255 none
%     inited      mask of the devices initialised
%     err         0 ok, 1 motor_cubemars init failed, 2 sensor_torque init failed, 3 keepalive lost, 4 amc.gtw does not answer,
%                 5 motor_macro init failed or control mode not reached, 6 motor_macro fault (external fault, hwfault)
%     result      of the last request: 0 none, 1 accepted (in progress), 2 rejected, 3 done, 4 failed
%     reason      why it was rejected: 1 wrong state, 2 devices not initialised, 3 parameter out of range,
%                 4 unknown mode, 5 different sgact_panel.toml, 6 unknown action
%     action, reqid   the last request seen
%     cnt         echo of the counter of the requester
%     trun        [s] since the start of the mode (running, stopping)
    u = struct('have', false, 'state', 0, 'mode', 255, 'inited', 0, 'err', 0, 'result', 0, 'reason', 0, ...
               'action', 0, 'reqid', 0, 'cnt', 0, 'trun', 0);
end

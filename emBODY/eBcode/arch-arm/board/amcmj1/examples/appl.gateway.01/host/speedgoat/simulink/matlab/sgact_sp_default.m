function sp = sgact_sp_default()
%SGACT_SP_DEFAULT what a mode gives (USER CONTROL -> SUP-B): the template of the bus SGACT_SP
%     valid                   there is a MIT for the motor_cubemars (false: nothing for the motor_cubemars)
%     kp kd pos vel torque    the MIT: torque_out = kp (pos - p) + kd (vel - v) + torque   [-, -, rad, rad/s, N*m]
%     mmtarget                the setpoint of the motor_macro, for a mode w/ a controlmode [A, rad/s, %]
%     done                    in the stop phase: the exit of the mode is over (SUP-A goes to idle at the next step)
%   SUP-B lets them out only if their device is initialised and the mode runs or stops (the gate)
    sp = struct('valid', false, 'kp', 0, 'kd', 0, 'pos', 0, 'vel', 0, 'torque', 0, 'mmtarget', 0, 'done', false);
end

function cmd = sgact_cmd_default()
%SGACT_CMD_DEFAULT what the controller asks to amc.gtw in a step: the template of the bus SGACT_CMD
%     motor_cubemars_mit_valid      there is a MIT setpoint for the motor_cubemars (false: the command frame is a heartbeat)
%     motor_cubemars_kp kd pos vel torque   the MIT: torque_out = kp (pos - p) + kd (vel - v) + torque   [-, -, rad, rad/s, N*m]
%     motor_macro_valid        there is a setpoint for the motor_macro
%     motor_macro_mode         its control mode (6 current, 10 speedvoltage, 80 openloop): the one asked w/ OPmotorMODE
%     motor_macro_target       the setpoint [A, rad/s, %]: amc.gtw TX sends rad/s as deg/s, the unit of amc.mot
%     rpc_go, rpc_op, rpc_arg send run+ack<rpc_op> on the rpc socket in this step (one at a time):
%                             0xD1 OPcubeENABLE 0xD2 OPcubeDISABLE 0xD4 OPpaxENABLE 0xD5 OPpaxDISABLE
%                             0xD6 OPmotorPIDS, 0xD7 OPmotorSETUP (their argument: mmcfg), 0xD8 OPmotorMODE (argument: rpc_arg)
%     mmcfg                   the config of the motor_macro given at its init (sgact_spec.py: MMCFG_NAMES)
%   the command frame goes every CMD_DIV steps with the setpoints of that step: the controller only says what, not when
    cmd = struct('motor_cubemars_mit_valid', false, 'motor_cubemars_kp', 0, 'motor_cubemars_kd', 0, 'motor_cubemars_pos', 0, 'motor_cubemars_vel', 0, 'motor_cubemars_torque', 0, ...
                 'motor_macro_valid', false, 'motor_macro_mode', 0, 'motor_macro_target', 0, ...
                 'rpc_go', false, 'rpc_op', 0, 'rpc_arg', 0, 'mmcfg', zeros(1, 14));
end

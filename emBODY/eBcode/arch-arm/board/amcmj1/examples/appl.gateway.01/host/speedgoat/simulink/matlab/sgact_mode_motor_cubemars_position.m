function [valid, sp, done] = sgact_mode_motor_cubemars_position(phase, p, t, fb) %#ok<INUSD>
%SGACT_MODE_MOTOR_CUBEMARS_POSITION the mode motor_cubemars_position of sgact_panel.toml: p = [pos vel kp kd torque] (SI):
%   the MIT w/ these values, constant. stop: done at once, then the supervisor disables the motor_cubemars
%   phase 1 run, 2 stop; p: the parameters (in the order of the TOML, in SI: rad, rad/s); t: s since the phase started; fb: the bus SGACT_FB
%   valid: a MIT for the motor_cubemars (sp(1:5)), sp = [kp kd pos vel torque mmtarget]; done: (stop) the mode is over
    valid = true;
    sp = [p(3), p(4), p(1), p(2), p(5), 0];
    done = phase == 2;
end

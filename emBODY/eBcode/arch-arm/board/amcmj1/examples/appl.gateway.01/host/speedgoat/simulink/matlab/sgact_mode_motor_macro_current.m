function [valid, sp, done] = sgact_mode_motor_macro_current(phase, p, t, fb) %#ok<INUSD>
%SGACT_MODE_MOTOR_MACRO_CURRENT the mode motor_macro_current of sgact_panel.toml: p = [current] (A): the motor_macro in
%   control mode current (amc.gtw clamps it to the peak current). stop: 0 at once, then the supervisor asks idle
%   phase 1 run, 2 stop; p: the parameters (in the order of the TOML, in SI); t: s since the phase started; fb: the bus SGACT_FB
%   valid: a MIT for the motor_cubemars (sp(1:5)), sp = [kp kd pos vel torque mmtarget]; done: (stop) the mode is over
    valid = false;
    done = true;
    if phase == 1
        sp = [0, 0, 0, 0, 0, p(1)];
    else
        sp = zeros(1, 6);
    end
end

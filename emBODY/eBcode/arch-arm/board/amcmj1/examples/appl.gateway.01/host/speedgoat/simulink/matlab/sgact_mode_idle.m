function [valid, sp, done] = sgact_mode_idle(phase, p, t, fb) %#ok<INUSD>
%SGACT_MODE_IDLE the mode idle of sgact_panel.toml: all the motors stop. the supervisor does it at the start: it disables
%   the motor_cubemars and puts the motor_macro in idle (the ones initialised). the mode sends nothing. stop: done at once
%   valid: a MIT for the motor_cubemars (sp(1:5)), sp = [kp kd pos vel torque mmtarget]; done: (stop) the mode is over
    valid = false;
    sp = zeros(1, 6);
    done = true;
end

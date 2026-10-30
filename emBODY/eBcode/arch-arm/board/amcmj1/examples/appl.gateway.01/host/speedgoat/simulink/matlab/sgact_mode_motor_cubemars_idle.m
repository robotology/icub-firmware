function [valid, sp, done] = sgact_mode_motor_cubemars_idle(phase, p, t, fb) %#ok<INUSD>
%SGACT_MODE_MOTOR_CUBEMARS_IDLE the mode motor_cubemars_idle of sgact_panel.toml: p = [pos vel kp kd torque], all 0 (the
%   TOML fixes them: the panel shows the MIT of rest). the mode has cubemars_enable = false: at its start the supervisor
%   disables the motor_cubemars (OPcubeDISABLE) and amc.gtw sends the MIT of rest, then the disable packet. it sends no
%   MIT (a MIT would not reach a disabled motor anyway). stop: done at once
%   valid: a MIT for the motor_cubemars (sp(1:5)), sp = [kp kd pos vel torque mmtarget]; done: (stop) the mode is over
    valid = false;
    sp = zeros(1, 6);
    done = true;
end

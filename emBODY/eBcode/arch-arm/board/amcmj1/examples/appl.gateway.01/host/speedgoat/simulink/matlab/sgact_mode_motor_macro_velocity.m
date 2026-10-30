function [valid, sp, done] = sgact_mode_motor_macro_velocity(phase, p, t, fb) %#ok<INUSD>
%SGACT_MODE_MOTOR_MACRO_VELOCITY the mode motor_macro_velocity of sgact_panel.toml: p = [vel] (rad/s): the motor_macro in
%   control mode speedvoltage at the velocity vel. stop: ramp to 0 in c.STOP_RAMP s, then the supervisor asks idle
%   phase 1 run, 2 stop; p: the parameters (in the order of the TOML, in SI: rad, rad/s); t: s since the phase started; fb: the bus SGACT_FB
%   valid: a MIT for the motor_cubemars (sp(1:5)), sp = [kp kd pos vel torque mmtarget]; done: (stop) the mode is over
%   mmtarget: the setpoint of the motor_macro in SI (rad/s here): amc.gtw TX turns it into deg/s
    c = sgact_const();
    valid = false;                      % nothing for the motor_cubemars
    done = true;
    if phase == 1
        sp = [0, 0, 0, 0, 0, p(1)];
    else
        k = max(0, 1 - t / c.STOP_RAMP);
        sp = [0, 0, 0, 0, 0, k * p(1)];
        done = k <= 0;
    end
end

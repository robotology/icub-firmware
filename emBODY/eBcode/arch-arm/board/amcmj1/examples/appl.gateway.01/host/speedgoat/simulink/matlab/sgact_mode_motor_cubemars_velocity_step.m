function [valid, sp, done] = sgact_mode_motor_cubemars_velocity_step(phase, p, t, fb) %#ok<INUSD>
%SGACT_MODE_MOTOR_CUBEMARS_VELOCITY_STEP the mode motor_cubemars_velocity_step of sgact_panel.toml:
%   p = [pos vel kp kd torque half] (SI): the MIT w/ vel = +vel for half s, then -vel for half s, ...; the other values as
%   given. stop: vel 0 for c.STOP_BRAKE s, then done (the supervisor disables the motor_cubemars)
%   phase 1 run, 2 stop; p: the parameters (in the order of the TOML, in SI: rad, rad/s); t: s since the phase started; fb: the bus SGACT_FB
%   valid: a MIT for the motor_cubemars (sp(1:5)), sp = [kp kd pos vel torque mmtarget]; done: (stop) the mode is over
    c = sgact_const();
    valid = true;
    done = true;
    if phase == 1
        sgn = 1;
        if p(6) > 0 && mod(floor(t / p(6)), 2) == 1
            sgn = -1;
        end
        sp = [p(3), p(4), p(1), sgn * p(2), p(5), 0];
    else
        sp = [p(3), p(4), p(1), 0, p(5), 0];
        done = t >= c.STOP_BRAKE;
    end
end

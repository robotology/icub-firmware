function cmd = sgact_sup_b(cmda, sup, run, sp)
%SGACT_SUP_B the second half of the supervisor of sgact (SUP-B): the gate. It adds to the rpc of SUP-A (cmda) the
%   setpoints of the mode (bus SGACT_SP, from USER CONTROL) that can leave:
%     the MIT of the motor_cubemars only if the motor_cubemars is initialised and a mode runs or stops (sp.valid). else
%     nothing: amc.gtw TX sends a heartbeat (and the motor_cubemars is disabled when no mode of it runs)
%     the target of the motor_macro only if the motor_macro is initialised and the mode has a control mode (run.mmode)
%   The same logic is in host/speedgoat/emul/sgact_supervisor.py for the daemon: keep them aligned.
    s = sgact_spec();
    cmd = cmda;
    active = run.phase > 0;
    if bitand(sup.inited, s.DEV_MOTOR_CUBEMARS) ~= 0 && active && sp.valid
        cmd.motor_cubemars_mit_valid = true;
        cmd.motor_cubemars_kp = sp.kp; cmd.motor_cubemars_kd = sp.kd; cmd.motor_cubemars_pos = sp.pos;
        cmd.motor_cubemars_vel = sp.vel; cmd.motor_cubemars_torque = sp.torque;
    end
    if bitand(sup.inited, s.DEV_MOTOR_MACRO) ~= 0 && active && run.mmode ~= 0
        cmd.motor_macro_valid = true;
        cmd.motor_macro_mode = run.mmode;
        cmd.motor_macro_target = sp.mmtarget;
    end
end

function sp = sgact_std_modes(run, fb)
%SGACT_STD_MODES the standard modes: the ones of sgact_panel.toml w/ impl = matlab (sgact_mode_<name>.m). It is the
%   block "standard modes" of USER CONTROL, enabled only while one of them runs or stops (sgact_std_on)
    sp = sgact_sp_default();
    if run.phase > 0
        [valid, v, done] = sgact_modes_dispatch(run.mode, run.phase, run.p, run.t, fb);
        sp.valid = valid;
        sp.kp = v(1); sp.kd = v(2); sp.pos = v(3); sp.vel = v(4); sp.torque = v(5);
        sp.mmtarget = v(6);
        sp.done = done;
    end
end

function r = sgact_run_default()
%SGACT_RUN_DEFAULT which mode runs, as SUP-A tells it to USER CONTROL and to SUP-B: the template of the bus SGACT_RUN
%     mode        the id of the mode of sgact_panel.toml that runs or stops, 255 none
%     phase       0 no mode, 1 run, 2 stop (the mode does its exit, then it says done)
%     first       the first step of the phase: a mode can take here what it needs (e.g. the position where it starts)
%     t           [s] since the start of the phase
%     trun        [s] since the start of the mode
%     p           the parameters of the mode, in the order of sgact_panel.toml and in SI (deg -> rad, deg/s -> rad/s)
%     inited      mask of the devices initialised (sgact_spec: DEV_*)
%     mmode       the control mode of the motor_macro of the mode (0 none, 6 current, 10 speedvoltage, 80 openloop)
    s = sgact_spec();
    r = struct('mode', 255, 'phase', 0, 'first', false, 't', 0, 'trun', 0, 'p', zeros(1, s.MAX_PARAMS), ...
               'inited', 0, 'mmode', 0);
end

function tg = sgact_deploy(target)
%SGACT_DEPLOY builds sgact, loads it on the Speedgoat and starts it
%   sgact_deploy                    the default target of Simulink Real-Time Explorer
%   sgact_deploy('SG-10.0.2.150')   a target by name
%   stop it with: stop(tg)
    mdl = 'sgact';
    sgact_setup();
    if nargin < 1
        tg = slrealtime;
    else
        tg = slrealtime(target);
    end
    if ~isConnected(tg)
        connect(tg);
    end
    sgact_gen_spec(true);                   % sgact_spec.m must come from the current sgact_panel.toml (else a warning)
    slbuild(mdl);
    load(tg, mdl);
    start(tg);
    n = sgact_netcfg();
    fprintf('%s is running: start the panel, from host:  py speedgoat\\panel\\sgact_panel.py --host %s\n', mdl, n.HOSTLINK_IP);
end

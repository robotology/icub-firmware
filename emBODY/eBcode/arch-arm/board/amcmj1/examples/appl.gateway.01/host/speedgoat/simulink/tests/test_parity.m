function test_parity()
%TEST_PARITY the Controller of the Speedgoat (sgact_controller: sgact_sup_a, the standard modes sgact_mode_*.m, sgact_sup_b) and the one of the daemon
%   (host/speedgoat/emul/sgact_supervisor.py + sgact_modes.py) must give the same outputs, step by step.
%   tests/parity_trace.txt.gz is written by host/speedgoat/emul/test_sgact_supervisor.py: the requests, the feedback of a
%   fake amc.gtw and what the Python supervisor answered, 1 ms per line. Here the same inputs go into the MATLAB one.
%   The second line of the file names the columns: req_*, fb_*, cmd_*, sup_*.
%   After a change of the supervisor or of the modes: run the Python test again, then this one.
%   run it from the folder simulink:  sgact_setup; test_parity

    here = fileparts(mfilename('fullpath'));
    tmp = tempname();
    files = gunzip(fullfile(here, 'parity_trace.txt.gz'), tmp);
    path = files{1};
    cleanup = onCleanup(@() rmdir(tmp, 's'));      %#ok<NASGU>
    fid = fopen(path, 'r');
    fgetl(fid);
    names = strsplit(strtrim(strrep(fgetl(fid), '% cols:', '')), ' ');
    fclose(fid);
    col = struct();
    for k = 1:numel(names)
        col.(names{k}) = k;
    end
    m = load(path);                                     % the comment lines are skipped
    s = sgact_spec();
    clear sgact_sup_a sgact_sup_b sgact_controller
    outs = names(strncmp(names, 'cmd_', 4) | strncmp(names, 'sup_', 4));
    nbad = 0;
    for i = 1:size(m, 1)
        r = m(i, :);
        t = r(col.t);
        req = sgact_req_default();
        req = fill(req, r, col, 'req_', {'have', 'alive', 'crc_ok', 'cnt', 'id', 'action', 'devices', 'mode', 'duration', 'extfault'});
        req.have = req.have ~= 0; req.alive = req.alive ~= 0; req.crc_ok = req.crc_ok ~= 0; req.extfault = req.extfault ~= 0;
        for k = 1:s.MAX_PARAMS
            req.params(k) = r(col.(sprintf('req_params%d', k)));
        end
        for k = 1:14
            req.mmcfg(k) = r(col.(sprintf('req_mmcfg%d', k)));
        end
        fb = sgact_fb_default();
        fb = fill(fb, r, col, 'fb_', {'trx', 'has_motor_cubemars_status', 'motor_cubemars_status', 'motor_cubemars_counter', 'sensor_torque_counter', ...
                  'motor_cubemars_vel', 'motor_cubemars_pos', 'has_motor_macro', 'motor_macro_mode', 'motor_macro_configured', ...
                  'motor_macro_faults', 'motor_macro_vel', 'has_sensor_position', 'sensor_position_counter', 'sensor_position_flags', 'gw_extfault'});
        fb.has_motor_cubemars_status = fb.has_motor_cubemars_status ~= 0; fb.has_motor_macro = fb.has_motor_macro ~= 0;
        fb.has_sensor_position = fb.has_sensor_position ~= 0;
        [cmd, sup] = sgact_controller(fb, req, t);
        for k = 1:numel(outs)
            name = outs{k};
            if strncmp(name, 'cmd_', 4)
                got = double(cmd.(name(5:end)));
            else
                got = double(sup.(name(5:end)));
            end
            expected = r(col.(name));
            if abs(got - expected) > 1e-8
                nbad = nbad + 1;
                if nbad <= 5
                    fprintf('  differ at t = %.3f s: %s is %g in MATLAB, %g in Python\n', t, name, got, expected);
                end
                break;
            end
        end
    end
    fprintf('%d/%d steps equal in MATLAB and in Python\n', size(m, 1) - nbad, size(m, 1));
    if nbad > 0
        error('test_parity failed');
    end
end


function x = fill(x, r, col, prefix, fields)
    for k = 1:numel(fields)
        x.(fields{k}) = r(col.([prefix fields{k}]));
    end
end

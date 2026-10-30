function sl_user_control_wire(uc, names, ids)
%SL_USER_CONTROL_WIRE the wiring of USER CONTROL, made again from scratch: the enable of every mode subsystem, the Merge
%   of their outputs, the lines from run and fb. names, ids: the modes w/ impl = simulink (their subsystems mode_<name>
%   must exist). The blocks it owns: "on standard modes", "on mode_<name>", "Merge" and all the lines of this level.
%   It never touches what is inside a mode subsystem.
    % ---- remove the old wiring
    lines = find_system(uc, 'SearchDepth', 1, 'FindAll', 'on', 'Type', 'line');
    for i = 1:numel(lines)
        try
            delete_line(lines(i));
        catch
        end
    end
    owned = [find_system(uc, 'SearchDepth', 1, 'Regexp', 'on', 'Name', '^on '); find_system(uc, 'SearchDepth', 1, 'Name', 'Merge')];
    for i = 1:numel(owned)
        if ~strcmp(owned{i}, uc)
            delete_block(owned{i});
        end
    end

    % ---- the subsystems, in order: standard modes, then the modes of the TOML
    subs = {'standard modes'};
    for k = 1:numel(names)
        subs{end + 1} = ['mode_' names{k}]; %#ok<AGROW>
    end
    n = numel(subs);
    for k = 1:n
        y = 60 + 140 * (k - 1);
        on = ['on ' subs{k}];
        add_block('simulink/User-Defined Functions/MATLAB Function', [uc '/' on], 'Position', [200 y 330 y + 40]);
        if k == 1
            code = sprintf(['function on = f(run)\n%%#codegen\n', ...
                            '%% the block "standard modes" runs while a mode w/ impl = matlab runs or stops\n', ...
                            'on = sgact_std_on(run);\n']);
        else
            code = sprintf(['function on = f(run)\n%%#codegen\n', ...
                            '%% mode_%s (id %d of sgact_panel.toml) runs while that mode runs or stops\n', ...
                            'on = run.phase > 0 && run.mode == %d;\n'], names{k - 1}, ids(k - 1), ids(k - 1));
        end
        sl_matlab_function([uc '/' on], code, {'run', 'Bus: SGACT_RUN'});
        set_param([uc '/' subs{k}], 'Position', [420 y + 30 480 + 120 y + 110]);
        add_line(uc, 'run/1', [on '/1'], 'autorouting', 'smart');
        add_line(uc, [on '/1'], [subs{k} '/Enable'], 'autorouting', 'smart');
        add_line(uc, 'run/1', [subs{k} '/1'], 'autorouting', 'smart');
        add_line(uc, 'fb/1', [subs{k} '/2'], 'autorouting', 'smart');
    end

    % ---- the Merge: the output of the subsystem that ran last
    if n == 1
        add_line(uc, 'standard modes/1', 'sp/1', 'autorouting', 'smart');
    else
        add_block('simulink/Signal Routing/Merge', [uc '/Merge'], 'Inputs', num2str(n), 'Position', [760 60 790 60 + 140 * n]);
        sl_setp([uc '/Merge'], 'InitialOutput', 'sgact_sp_default()');   % a struct of the bus: a plain 0 is refused by SLRT
        for k = 1:n
            add_line(uc, [subs{k} '/1'], sprintf('Merge/%d', k), 'autorouting', 'smart');
        end
        add_line(uc, 'Merge/1', 'sp/1', 'autorouting', 'smart');
    end
    set_param([uc '/sp'], 'Position', [860 80 + 70 * (n - 1) 890 94 + 70 * (n - 1)]);
end

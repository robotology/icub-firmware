function sgact_gen_spec(check_only)
%SGACT_GEN_SPEC sgact_panel.toml -> matlab/sgact_spec.m, sgact_modes_dispatch.m, sgact_modes_info.m, with tools/gen_sgact_spec.py
%   (MATLAB code generation cannot read a TOML file). Then it checks that sgact_spec.m comes from the current TOML (CRC).
%   sgact_gen_spec        generate if Python is there, then check: error if the generated files are old
%   sgact_gen_spec(true)  only check, with a warning
%   build_sgact_lib and sgact_deploy call it. W/out Python: run  py tools/gen_sgact_spec.py  by hand.
    if nargin < 1
        check_only = false;
    end
    here = fileparts(fileparts(mfilename('fullpath')));
    toml = fullfile(fileparts(here), 'sgact_panel.toml');     % host/speedgoat/sgact_panel.toml
    if ~check_only
        gen = fullfile(here, 'tools', 'gen_sgact_spec.py');
        launchers = {'py -3', 'python', 'python3'};
        ran = false;
        for i = 1:numel(launchers)
            [st, out] = system(sprintf('%s "%s" --toml "%s" --out "%s"', launchers{i}, gen, toml, fullfile(here, 'matlab')));
            if st == 0
                fprintf('%s', out);
                ran = true;
                break;
            elseif ~isempty(strfind(out, '[error]'))     %#ok<STREMP> the generator ran and found a problem
                error('sgact:spec', '%s', out);
            end
        end
        if ~ran
            warning('sgact:spec', 'Python not found: matlab/sgact_spec.m is used as it is (run  py tools/gen_sgact_spec.py  by hand)');
        end
        rehash path;
        clear sgact_spec sgact_modes_dispatch sgact_modes_info
    end
    s = sgact_spec();
    crc = sgact_crc32(toml);
    if s.CRC ~= crc
        msg = sprintf(['matlab/sgact_spec.m is not generated from the current sgact_panel.toml (0x%08X vs 0x%08X): ' ...
                       'run  py tools/gen_sgact_spec.py  (or build_sgact_lib)'], double(s.CRC), double(crc));
        if check_only
            warning('sgact:spec', '%s', msg);
        else
            error('sgact:spec', '%s', msg);
        end
    end
end

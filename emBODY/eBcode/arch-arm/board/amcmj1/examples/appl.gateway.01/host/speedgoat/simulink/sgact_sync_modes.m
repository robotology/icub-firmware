function sgact_sync_modes(mdl)
%SGACT_SYNC_MODES brings USER CONTROL of sgact.slx in line w/ sgact_panel.toml, w/out touching the work of the
%   control engineer:
%     - a mode w/ impl = simulink that has no subsystem yet gets one: Controller/USER CONTROL/mode_<name>, w/ its ports
%       (run, fb -> sp) and blocks that do nothing, to be replaced
%     - the wiring of USER CONTROL (the enables, the Merge, the lines of that level) is made again
%     - a subsystem mode_<name> whose mode is not in the TOML any more is left there, w/ a warning: delete it by hand
%   The content of the existing subsystems mode_<name> is never changed. Run it after every change of the modes of
%   the TOML (after build_sgact_lib, which generates matlab/sgact_modes_info.m), then save and deploy.
%   sgact_sync_modes            on sgact.slx
%   sgact_sync_modes('other')   on another model w/ the same structure
    if nargin < 1
        mdl = 'sgact';
    end
    here = fileparts(mfilename('fullpath'));
    addpath(here, fullfile(here, 'matlab'), fullfile(here, 'tools'));
    sgact_gen_spec(true);
    sgact_buses();
    load_system(mdl);
    uc = [mdl '/Controller/USER CONTROL'];
    if isempty(find_system(mdl, 'SearchDepth', 2, 'Name', 'USER CONTROL'))
        error('sync:nouc', '%s has no Controller/USER CONTROL: it was made by an older build_sgact (build_sgact(true) makes a new one)', mdl);
    end
    clear sgact_modes_info
    info = sgact_modes_info();
    sim = info(strcmp({info.impl}, 'simulink'));
    names = {sim.name};
    ids = [sim.id];

    have = find_system(uc, 'SearchDepth', 1, 'BlockType', 'SubSystem', 'Regexp', 'on', 'Name', '^mode_');
    have = cellfun(@(p) get_param(p, 'Name'), have, 'UniformOutput', false);
    for k = 1:numel(names)
        if ~any(strcmp(have, ['mode_' names{k}]))
            sl_mode_subsystem(uc, names{k}, sl_mode_kind(names{k}), [420 60 600 140]);
            fprintf('created %s/mode_%s: fill it w/ the blocks of the mode\n', uc, names{k});
        end
    end
    for k = 1:numel(have)
        if ~any(strcmp(['mode_' names], have{k}))
            warning('sync:orphan', '%s/%s has no mode in sgact_panel.toml: it is not wired. delete it by hand if it is not needed', uc, have{k});
        end
    end
    sl_user_control_wire(uc, names, ids);
    save_system(mdl);
    fprintf('USER CONTROL of %s: standard modes + %d simulink modes (%s)\n', mdl, numel(names), strjoin(names, ', '));
end

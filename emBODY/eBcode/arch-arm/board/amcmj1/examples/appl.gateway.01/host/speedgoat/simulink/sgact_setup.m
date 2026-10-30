function sgact_setup()
%SGACT_SETUP puts the MATLAB functions, the build tools and the tests of the Speedgoat project on the path, checks that
%   matlab/sgact_spec.m comes from the current sgact_panel.toml (a warning if not) and creates the bus objects in the
%   base workspace (only in MATLAB with Simulink: the tests run also w/out it, e.g. in Octave)
    here = fileparts(mfilename('fullpath'));
    addpath(here, fullfile(here, 'matlab'), fullfile(here, 'tools'), fullfile(here, 'tests'));
    sgact_gen_spec(true);           % warns if matlab/sgact_spec.m does not come from the current sgact_panel.toml
    if exist('Simulink.Bus', 'class') == 8
        sgact_buses();
    end
end

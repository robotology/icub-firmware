function [cmd, sup, run, sp] = sgact_controller(fb, req, t)
%SGACT_CONTROLLER the Controller of sgact.slx in one call, as the model runs it: SUP-A, USER CONTROL, SUP-B, and the unit
%   delay of done. The model does not call it: it has the blocks. It is here for the tests (and it is what the daemon
%   does in Python). In USER CONTROL the standard modes run here; a mode w/ impl = simulink is played as the subsystem
%   that sgact_sync_modes creates: it does nothing (valid false, done true).
    persistent donez
    if isempty(donez)
        donez = false;
    end
    [run, cmda, sup] = sgact_sup_a(fb, req, donez, t);
    if sgact_std_on(run)
        sp = sgact_std_modes(run, fb);
    else
        sp = sgact_sp_default();
        sp.done = true;                     % a simulink mode as created, or no mode
    end
    cmd = sgact_sup_b(cmda, sup, run, sp);
    donez = sp.done;
end

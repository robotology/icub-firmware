function [cmdf, cmd_len, cmd_go, rpc, rpc_len, rpc_go, status, status_len, status_go, sup, cmd, fb] = sgact_step(ctl, ctllen, st, stlen, t)
%SGACT_STEP one step of the whole chain, as the model runs it:
%   amc.gtw RX, panel RX -> Controller (SUP-A, USER CONTROL w/ the modes, SUP-B: sgact_controller) -> amc.gtw TX, panel TX
%   The model does not call it: it has the blocks. It is here for the tests (tests/test_sgact_step.m), which run the
%   same functions in the same order w/out Simulink. Outputs: the UDP packets, then the buses sup, cmd and fb.
    fb = sgact_gtw_rx_step(st, stlen, t);
    req = sgact_panel_rx_step(ctl, ctllen, t);
    [cmd, sup] = sgact_controller(fb, req, t);
    [cmdf, cmd_len, cmd_go, rpc, rpc_len, rpc_go, nsent] = sgact_gtw_tx_step(cmd, t);
    [status, status_len, status_go] = sgact_panel_tx_step(sup, fb, nsent, t);
end

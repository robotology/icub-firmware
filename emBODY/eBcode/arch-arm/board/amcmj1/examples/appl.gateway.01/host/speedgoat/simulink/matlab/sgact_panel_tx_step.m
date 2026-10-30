function [status, status_len, status_go] = sgact_panel_tx_step(sup, fb, nsent, t) %#ok<INUSD>
%SGACT_PANEL_TX_STEP the block panel TX: the buses SGACT_SUP and SGACT_FB -> the STATUS v4 to the panel (104 bytes)
%   every c.STATUS_DIV steps once the panel spoke (sup.have). nsent: the command frames sent by amc.gtw TX
%   t is not used today: it is an input to keep the four I/O blocks alike
    c = sgact_const();
    persistent k
    if isempty(k)
        k = 0;
    end
    k = k + 1;

    status = zeros(c.STATUS_SIZE, 1, 'uint8');
    status_len = c.STATUS_SIZE;
    status_go = false;
    if sup.have && mod(k - 1, c.STATUS_DIV) == 0
        status = sgact_status_frame(sup, fb, nsent);
        status_go = true;
    end
end

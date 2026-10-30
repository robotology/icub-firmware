function req = sgact_panel_rx_step(ctl, ctllen, t)
%SGACT_PANEL_RX_STEP the block panel RX: the CTL v3 of the panel (sgctl.py) -> the bus SGACT_REQ
%   ctl, ctllen : the last packet from the panel (port 6000) and its length (0 = nothing new)
%   t           : time [s]
%   req holds the last valid CTL; req.alive goes false after KEEPALIVE s (sgact_panel.toml) w/out a valid one
    s = sgact_spec();
    persistent last have tlast
    if isempty(last)
        last = sgact_req_default();
        have = false;
        tlast = 0;
    end
    [ok, r] = sgact_ctl_decode(ctl, ctllen);
    if ok
        last = r;
        have = true;
        tlast = t;
    end
    req = last;
    req.have = have;
    req.alive = have && (t - tlast) < s.KEEPALIVE;
end

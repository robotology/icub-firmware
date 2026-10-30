function fb = sgact_gtw_rx_step(st, stlen, t)
%SGACT_GTW_RX_STEP the block amc.gtw RX: the state frame of amc.gtw -> the bus SGACT_FB
%   st, stlen : the last state frame from amc.gtw (port 5003) and its length (0 = nothing new)
%   t         : time [s]
%   fb holds the last valid state frame: fb.fresh says if it arrived in this step, fb.nrecv counts them, fb.trx is
%   the time of the last one
    persistent last nrecv tlastrx
    if isempty(last)
        last = sgact_fb_default();
        nrecv = 0;
        tlastrx = 0;
    end

    s = sgact_state_decode(st, stlen);
    if s.valid
        last = s;
        nrecv = nrecv + 1;
        tlastrx = t;
    end
    fb = last;
    fb.fresh = s.valid;
    fb.nrecv = nrecv;
    fb.trx = tlastrx;
end

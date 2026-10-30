function f = sgact_hb_frame(tus)
%SGACT_HB_FRAME the command frame w/out a setpoint (amc_gtw.empty_frame), 16 bytes: it feeds the watchdog of amc.gtw
    f = sgact_tether_header(8, 0, tus);
end

function f = sgact_run_frame(op, ack, tus)
%SGACT_RUN_FRAME the rpc frame run[+ack]<op> w/out argument (amc_gtw.run_frame), 24 bytes
%   op: OPcubeENABLE 0xD1, OPcubeDISABLE 0xD2, OPpaxENABLE 0xD4, OPpaxDISABLE 0xD5
    c = sgact_const();
    f = zeros(24, 1, 'uint8');
    f(1:16) = sgact_tether_header(8, 1, tus);
    opts = 0;
    if ack
        opts = c.OPT_ACK;
    end
    f(17) = uint8(c.CMD_RUN + 16 * opts);
    f(18) = uint8(op);
end

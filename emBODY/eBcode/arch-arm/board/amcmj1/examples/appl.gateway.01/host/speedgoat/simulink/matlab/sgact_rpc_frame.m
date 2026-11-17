function [f, n] = sgact_rpc_frame(op, ack, arg, sz, tus)
%SGACT_RPC_FRAME the rpc frame run[+ack]<op, arg(1:sz)> (port 5002, amc_gtw.run_frame): one ROP of 4 + sz bytes, padded to 4
%   (sz in 0 .. 24 bytes of the uint8 vector arg): 20 bytes w/out argument, 24 w/ 1 byte, 40 w/ 20 (OPmotorSETUP),
%   44 w/ 24 (OPmotorPIDS). no variable-size array: it is code for the Speedgoat.
%   f is c.RPC_FRAME_MAX (44) bytes, n the bytes to send
    c = sgact_const();
    f = zeros(c.RPC_FRAME_MAX, 1, 'uint8');
    f(1:16) = sgact_tether_header(1, tus);
    opts = 0;
    if ack
        opts = c.OPT_ACK;
    end
    f(17) = uint8(c.CMD_RUN + 16 * opts);
    f(18) = uint8(op);
    f(19:20) = sgact_bytes(uint16(sz));
    for i = 1:sz
        f(20 + i) = uint8(arg(i));
    end
    n = 16 + 4 + 4 * ceil(sz / 4);
end

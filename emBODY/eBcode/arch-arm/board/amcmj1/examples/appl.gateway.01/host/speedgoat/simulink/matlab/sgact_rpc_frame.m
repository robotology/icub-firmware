function [f, n] = sgact_rpc_frame(op, ack, arg, sz, tus)
%SGACT_RPC_FRAME the rpc frame run[+ack]<op, arg(1:sz)> (port 5002, amc_gtw.run_frame): one ROP in the smallest slot
%   that holds the argument (sz in 0 .. 24 bytes of the uint8 vector arg): 8 bytes w/ up to 4, 24 w/ 20 (OPmotorSETUP),
%   32 w/ 24 (OPmotorPIDS). no variable-size array: it is code for the Speedgoat.
%   f is c.RPC_FRAME_MAX (48) bytes, n the bytes to send
    c = sgact_const();
    f = zeros(c.RPC_FRAME_MAX, 1, 'uint8');
    slot = 8 * ceil((4 + sz) / 8);
    f(1:16) = sgact_tether_header(slot, 1, tus);
    opts = 0;
    if ack
        opts = c.OPT_ACK;
    end
    f(17) = uint8(c.CMD_RUN + 16 * opts);
    f(18) = uint8(op);
    f(19) = uint8(sz);
    for i = 1:sz
        f(20 + i) = uint8(arg(i));
    end
    n = 16 + slot;
end

function [arg, sz] = sgact_rpc_arg(op, rpcarg, mmcfg)
%SGACT_RPC_ARG the argument of an rpc of the supervisor (amc_gtw.pids_arg, setup_arg, mode_arg), as a 24-byte uint8 column
%   and its size: OPmotorPIDS 24 bytes (6 single), OPmotorSETUP 20 bytes (3 single, flags poles entlr 0, 2 int16),
%   OPmotorMODE 1 byte (the control mode rpcarg), OPgwFAULT 1 byte (1 set, 0 clear), the others none
    c = sgact_const();
    arg = zeros(24, 1, 'uint8');
    sz = 0;
    if op == c.OP_MOTOR_PIDS
        for k = 1:6
            arg(4 * (k - 1) + (1:4)) = sgact_bytes(single(mmcfg(k)));
        end
        sz = 24;
    elseif op == c.OP_MOTOR_SETUP
        for k = 1:3
            arg(4 * (k - 1) + (1:4)) = sgact_bytes(single(mmcfg(6 + k)));
        end
        arg(13) = uint8(mmcfg(10));
        arg(14) = uint8(mmcfg(11));
        arg(15) = uint8(mmcfg(12));
        arg(17:18) = sgact_bytes(int16(mmcfg(13)));
        arg(19:20) = sgact_bytes(int16(mmcfg(14)));
        sz = 20;
    elseif op == c.OP_MOTOR_MODE || op == c.OP_GW_FAULT
        arg(1) = uint8(rpcarg);
        sz = 1;
    end
end

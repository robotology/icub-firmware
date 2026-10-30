function [cmdf, cmd_len, cmd_go, rpc, rpc_len, rpc_go, nsent_out] = sgact_gtw_tx_step(cmd, t)
%SGACT_GTW_TX_STEP the block amc.gtw TX: the bus SGACT_CMD -> the UDP packets to amc.gtw
%   cmdf, cmd_len, cmd_go : the command frame (port 5001), every c.CMD_DIV steps: the MIT of the motor_cubemars if
%                           cmd.motor_cubemars_mit_valid and the target of the motor_macro if cmd.motor_macro_valid (seq + 1 each
%                           time one of them is there), else a heartbeat (16 bytes). up to 80 bytes
%   rpc, rpc_len, rpc_go  : run+ack<cmd.rpc_op, argument> (port 5002) when cmd.rpc_go. up to 48 bytes
%   nsent_out             : command frames sent so far (the panel shows it)
%   the period of the frames, seq and the counter belong to this block: the controller says what, this block says when.
%   the units of amc.mot are made here: the velocity of the motor_macro goes in deg/s
    c = sgact_const();
    persistent k seq nsent
    if isempty(k)
        k = 0;
        seq = c.SEQ0;
        nsent = 0;
    end
    k = k + 1;
    tus = uint64(max(0, round(t * 1e6)));

    % ---- the command frame: always, amc.gtw takes it as its clock and watchdog
    cmdf = zeros(c.CMD_FRAME_MAX, 1, 'uint8');
    cmd_len = 0;
    cmd_go = false;
    if mod(k - 1, c.CMD_DIV) == 0
        tvalue = cmd.motor_macro_target;
        if cmd.motor_macro_mode == c.CM_SPEEDVOLTAGE
            tvalue = tvalue * 180 / pi;             % rad/s -> deg/s
        end
        mit = [cmd.motor_cubemars_kp, cmd.motor_cubemars_kd, cmd.motor_cubemars_pos, cmd.motor_cubemars_vel, cmd.motor_cubemars_torque];
        [cmdf, n] = sgact_cmd_frame(cmd.motor_cubemars_mit_valid, mit, seq, cmd.motor_macro_valid, tvalue, cmd.motor_macro_mode, seq, tus);
        cmd_len = n;
        if cmd.motor_cubemars_mit_valid || cmd.motor_macro_valid
            seq = seq + 1;                          % a heartbeat does not move seq
        end
        cmd_go = true;
        nsent = nsent + 1;
    end

    % ---- the rpc socket
    rpc = zeros(c.RPC_FRAME_MAX, 1, 'uint8');
    rpc_len = 0;
    rpc_go = false;
    if cmd.rpc_go
        [arg, sz] = sgact_rpc_arg(cmd.rpc_op, cmd.rpc_arg, cmd.mmcfg);
        [rpc, rpc_len] = sgact_rpc_frame(cmd.rpc_op, c.RPC_ACK, arg, sz, tus);
        rpc_go = true;
    end
    nsent_out = nsent;
end

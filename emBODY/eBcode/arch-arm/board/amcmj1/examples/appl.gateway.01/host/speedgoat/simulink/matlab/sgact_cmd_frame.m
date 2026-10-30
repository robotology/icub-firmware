function [f, n] = sgact_cmd_frame(mitvalid, mit, mitseq, tvalid, tvalue, tmode, tseq, tus)
%SGACT_CMD_FRAME the command frame (port 5001) w/ what there is (amc_gtw.command_frame):
%   set<IDcubeMIT, {kp, kd, pos, vel, torque, seq}>   if mitvalid, mit = [kp kd pos vel torque]
%   set<IDmotorTARGET, {value, mode, seq}>            if tvalid (value in the unit of amc.mot: A, deg/s, %)
%   all the slots have the size of the biggest ROP: 32 bytes w/ the MIT (24 bytes), 16 w/ the target alone (12 bytes).
%   nothing: a heartbeat of 16 bytes. f is c.CMD_FRAME_MAX (80) bytes, n the bytes to send
    c = sgact_const();
    f = zeros(c.CMD_FRAME_MAX, 1, 'uint8');
    slot = 8;
    num = 0;
    if mitvalid
        slot = 32;
        num = num + 1;
    end
    if tvalid
        slot = max(slot, 16);
        num = num + 1;
    end
    f(1:16) = sgact_tether_header(slot, num, tus);
    off = 16;
    if mitvalid
        f(off + 1) = uint8(c.CMD_SET);
        f(off + 2) = uint8(c.ID_CUBE_MIT);
        f(off + 3) = uint8(24);
        for k = 1:5
            f(off + 4 + 4 * (k - 1) + (1:4)) = sgact_bytes(single(mit(k)));
        end
        f(off + 24 + (1:4)) = sgact_bytes(uint32(mod(mitseq, 4294967296)));
        off = off + slot;
    end
    if tvalid
        f(off + 1) = uint8(c.CMD_SET);
        f(off + 2) = uint8(c.ID_MOTOR_TARGET);
        f(off + 3) = uint8(12);
        f(off + 4 + (1:4)) = sgact_bytes(single(tvalue));
        f(off + 9) = uint8(tmode);
        f(off + 12 + (1:4)) = sgact_bytes(uint32(mod(tseq, 4294967296)));
        off = off + slot;
    end
    n = off;
end

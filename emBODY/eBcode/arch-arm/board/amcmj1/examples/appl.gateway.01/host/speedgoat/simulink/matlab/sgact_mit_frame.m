function f = sgact_mit_frame(kp, kd, pos, vel, torque, seq, tus)
%SGACT_MIT_FRAME the command frame with one set<IDcubeMIT, {kp, kd, pos, vel, torque, seq}> (amc_gtw.mit_frame), 44 bytes
%   the ROP: { opc = set, id = IDcubeMIT, uint16 size = 24 } + kp kd pos vel torque (single), seq (uint32): 24 bytes, no padding
    c = sgact_const();
    f = zeros(c.MIT_FRAME_SIZE, 1, 'uint8');
    f(1:16) = sgact_tether_header(1, tus);
    f(17) = uint8(c.CMD_SET);
    f(18) = uint8(c.ID_CUBE_MIT);
    f(19:20) = sgact_bytes(uint16(24));
    f(21:24) = sgact_bytes(single(kp));
    f(25:28) = sgact_bytes(single(kd));
    f(29:32) = sgact_bytes(single(pos));
    f(33:36) = sgact_bytes(single(vel));
    f(37:40) = sgact_bytes(single(torque));
    f(41:44) = sgact_bytes(uint32(mod(seq, 4294967296)));
end

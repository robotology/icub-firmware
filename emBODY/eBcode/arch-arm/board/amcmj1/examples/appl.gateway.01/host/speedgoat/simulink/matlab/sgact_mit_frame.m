function f = sgact_mit_frame(kp, kd, pos, vel, torque, seq, tus)
%SGACT_MIT_FRAME the command frame with one set<IDcubeMIT, {kp, kd, pos, vel, torque, seq}> (amc_gtw.mit_frame), 48 bytes
%   slot of 32 bytes: { opc = set, id = IDcubeMIT, size = 24, refcmd = 0, kp kd pos vel torque (single), seq (uint32), 4 x 0 }
    c = sgact_const();
    f = zeros(48, 1, 'uint8');
    f(1:16) = sgact_tether_header(32, 1, tus);
    f(17) = uint8(c.CMD_SET);
    f(18) = uint8(c.ID_CUBE_MIT);
    f(19) = uint8(24);
    f(21:24) = sgact_bytes(single(kp));
    f(25:28) = sgact_bytes(single(kd));
    f(29:32) = sgact_bytes(single(pos));
    f(33:36) = sgact_bytes(single(vel));
    f(37:40) = sgact_bytes(single(torque));
    f(41:44) = sgact_bytes(uint32(mod(seq, 4294967296)));
end

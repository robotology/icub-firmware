function f = sgact_status_frame(sup, fb, nsent)
%SGACT_STATUS_FRAME the STATUS v4 to the panel (104 bytes), as sgctl.pack_status
%   'SGS4' state mode inited err result reason action motor_cubemars_status (uint8)
%   reqid cnt crc (uint32) trun (single)
%   motor_cubemars pos vel current, sensor_torque mx my mz, motor_macro pos vel current (single: rad, rad/s, A, N*m)
%   motor_macro mode configured emulated (uint8) gw_extfault (uint8), sent received lost clamped (uint32), motor_macro pwm (single) faults (uint32)
%   sensor_position pos (single: rad) counter (uint32) flags (uint8) 0 0 0
    s = sgact_spec();
    f = zeros(104, 1, 'uint8');
    f(1:4) = uint8('SGS4');
    cubestate = 0;
    if fb.has_motor_cubemars_status
        cubestate = fb.motor_cubemars_status;
    end
    f(5) = uint8(sup.state); f(6) = uint8(sup.mode); f(7) = uint8(sup.inited); f(8) = uint8(sup.err);
    f(9) = uint8(sup.result); f(10) = uint8(sup.reason); f(11) = uint8(sup.action); f(12) = uint8(cubestate);
    f(13:16) = sgact_bytes(uint32(mod(sup.reqid, 4294967296)));
    f(17:20) = sgact_bytes(uint32(mod(sup.cnt, 4294967296)));
    f(21:24) = sgact_bytes(s.CRC);
    f(25:28) = sgact_bytes(single(sup.trun));
    v = [fb.motor_cubemars_pos, fb.motor_cubemars_vel, fb.motor_cubemars_current, fb.sensor_torque_mx, fb.sensor_torque_my, fb.sensor_torque_mz, ...
         fb.motor_macro_pos, fb.motor_macro_vel, fb.motor_macro_current];
    for k = 1:9
        f(28 + 4 * (k - 1) + (1:4)) = sgact_bytes(single(v(k)));
    end
    f(65) = uint8(fb.motor_macro_mode);
    f(66) = uint8(fb.motor_macro_configured);
    f(67) = uint8(fb.motor_macro_emulated);
    f(68) = uint8(fb.gw_extfault);
    u = [nsent, fb.nrecv, fb.gw(7), fb.gw(4)];
    for k = 1:4
        f(68 + 4 * (k - 1) + (1:4)) = sgact_bytes(uint32(mod(u(k), 4294967296)));
    end
    f(85:88) = sgact_bytes(single(fb.motor_macro_pwm));
    f(89:92) = sgact_bytes(uint32(mod(fb.motor_macro_faults, 4294967296)));
    f(93:96) = sgact_bytes(single(fb.sensor_position_pos));
    f(97:100) = sgact_bytes(uint32(mod(fb.sensor_position_counter, 4294967296)));
    f(101) = uint8(fb.sensor_position_flags);
end

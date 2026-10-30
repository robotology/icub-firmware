function [ok, r] = sgact_ctl_decode(raw, len)
%SGACT_CTL_DECODE the CTL v3 of the panel (100 bytes, sgctl.py) -> ok and the fields of the bus SGACT_REQ
%   (have, alive and crc_ok are left to panel RX). ok is false for a wrong size, magic, action or devices
%   'SGC3' action mode devices flags (bit 0: EXTFAULT) cnt reqid crc duration params[8] mmcfg: 9 single, flags poles entlr 0 (uint8),
%   rotencres rotindexoff (int16)
    c = sgact_const();
    s = sgact_spec();
    r = sgact_req_default();
    ok = false;
    if double(len) < c.CTL_SIZE || numel(raw) < c.CTL_SIZE
        return;
    end
    b = uint8(raw(1:c.CTL_SIZE));               % uint8 whatever the type of the input
    if ~(b(1) == uint8('S') && b(2) == uint8('G') && b(3) == uint8('C') && b(4) == uint8('3')) || b(5) > 4 || double(b(7)) > s.DEV_ALL
        return;
    end
    ok = true;
    r.action = double(b(5));
    r.mode = double(b(6));
    r.devices = double(b(7));
    r.extfault = bitand(double(b(8)), c.CTL_EXTFAULT) ~= 0;     % flags
    r.cnt = sgact_rd_u(b, 9, 4);
    r.id = sgact_rd_u(b, 13, 4);
    r.crc_ok = sgact_rd_u(b, 17, 4) == double(s.CRC);
    r.duration = sgact_rd_f32(b, 21);
    for k = 1:s.MAX_PARAMS
        r.params(k) = sgact_rd_f32(b, 24 + 4 * (k - 1) + 1);
    end
    for k = 1:9
        r.mmcfg(k) = sgact_rd_f32(b, 56 + 4 * (k - 1) + 1);
    end
    r.mmcfg(10) = double(b(93));                % flags
    r.mmcfg(11) = double(b(94));                % poles
    r.mmcfg(12) = double(b(95));                % entlr
    r.mmcfg(13) = sgact_rd_i16(b, 97);     % rotencres
    r.mmcfg(14) = sgact_rd_i16(b, 99);    % rotindexoff
end

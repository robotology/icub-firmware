function f = sgact_ctl_frame(action, mode, devices, cnt, reqid, crc, duration, params, mmcfg, flags)
%SGACT_CTL_FRAME the CTL v3 of the panel (100 bytes), as sgctl.pack_ctl: to test w/out the panel
%   'SGC3' action mode devices flags (bit 0: EXTFAULT; missing: 0) cnt reqid crc (uint32) duration params[8] (single)
%   mmcfg: curkp curki curkd velkp velki velkd nominal peak overload (single) flags poles entlr 0 (uint8) rotencres
%   rotindexoff (int16). missing: 0
    if nargin < 9
        mmcfg = [];
    end
    if nargin < 10
        flags = 0;
    end
    f = zeros(100, 1, 'uint8');
    f(1:4) = uint8('SGC3');
    f(5) = uint8(action); f(6) = uint8(mode); f(7) = uint8(devices); f(8) = uint8(flags);
    f(9:12) = sgact_bytes(uint32(mod(cnt, 4294967296)));
    f(13:16) = sgact_bytes(uint32(mod(reqid, 4294967296)));
    f(17:20) = sgact_bytes(uint32(crc));
    f(21:24) = sgact_bytes(single(duration));
    for k = 1:8
        v = 0;
        if k <= numel(params)
            v = params(k);
        end
        f(24 + 4 * (k - 1) + (1:4)) = sgact_bytes(single(v));
    end
    m = zeros(1, 14);
    m(1:min(14, numel(mmcfg))) = mmcfg(1:min(14, numel(mmcfg)));
    for k = 1:9
        f(56 + 4 * (k - 1) + (1:4)) = sgact_bytes(single(m(k)));
    end
    f(93) = uint8(m(10)); f(94) = uint8(m(11)); f(95) = uint8(m(12));
    f(97:98) = sgact_bytes(int16(m(13)));
    f(99:100) = sgact_bytes(int16(m(14)));
end

function [st, stlen, info] = fake_amcgtw(cmd, cmd_len, cmd_go, rpc, rpc_go, opts)
%FAKE_AMCGTW a tiny stand-in of amc.gtw (the gateway of the amcmj1 on the CM4) for test_sgact_step: it takes the frames of the model, answers with a state frame
%   at every command frame, as tGATEWAY does. opts.paxbroken: the Paxini never streams. opts.mute: amc.gtw sends nothing
%   the motor_macro as amcmj1_motdriver.cpp: pids and setup (configured), a control mode only after both, the targets of that mode
%   info: what amc.gtw saw (enables, disables, MITs and the last one, heartbeats, the config of the motor_macro)
    c = sgact_const();
    persistent gwfault cube cuberun posncnt pax cubecnt paxcnt vel pos nmit nhb ops lastmit lastseq mmconf mmmode mmvel mmcnt mmcfg
    if isempty(cube)
        gwfault = 0; cube = false; cuberun = false; posncnt = 0; pax = false; cubecnt = 0; paxcnt = 0; vel = 0; pos = 0; nmit = 0; nhb = 0; ops = zeros(1, 0); lastmit = zeros(1, 5); lastseq = -1;
        mmconf = 0; mmmode = 0; mmvel = 0; mmcnt = 0; mmcfg = zeros(1, 14);
    end
    if rpc_go
        assert(rpc(17) == 23, 'rpc: run+ack expected');
        op = double(rpc(18));
        ops(end + 1) = op;
        switch op
            case 209, cube = true; cuberun = true;     % OPcubeENABLE: running
            case 210, cuberun = false;                 % OPcubeDISABLE: initialised (the disable packet)
            case 212, pax = true;
            case 213, pax = false;
            case 214                    % OPmotorPIDS
                assert(rpc(19) == 24 && rpc(5) == 32, 'OPmotorPIDS: 24 bytes in a slot of 32');
                mmcfg(1:6) = double(typecast(rpc(21:44), 'single'));
                mmconf = bitor(mmconf, 1);
            case 215                    % OPmotorSETUP
                assert(rpc(19) == 20 && rpc(5) == 24, 'OPmotorSETUP: 20 bytes in a slot of 24');
                mmcfg(7:9) = double(typecast(rpc(21:32), 'single'));
                mmcfg(10:12) = double(rpc(33:35));
                mmcfg(13:14) = double(typecast(rpc(37:40), 'int16'));
                mmconf = bitor(mmconf, 2);
            case 217                    % OPgwFAULT
                assert(rpc(19) == 1, 'OPgwFAULT: 1 byte');
                gwfault = double(rpc(21));
                if gwfault ~= 0
                    cuberun = false; mmmode = 0;
                end
            case 216                    % OPmotorMODE
                assert(rpc(19) == 1, 'OPmotorMODE: 1 byte');
                m = double(rpc(21));
                if m == 0 || mmconf == 3
                    mmmode = m;
                end
        end
    end
    st = zeros(512, 1, 'uint8');
    stlen = 0;
    if cmd_go && ~opts.mute
        slot = double(cmd(5));
        num = double(cmd(6));
        assert(cmd_len == 16 + num * slot, 'command frame: wrong length');
        if num == 0
            nhb = nhb + 1;
        end
        for k = 0:(num - 1)
            off = 16 + k * slot;
            assert(cmd(off + 1) == 2, 'command frame: set<> expected');
            if cmd(off + 2) == 13
                assert(cmd(off + 3) == 24 && slot == 32, 'MIT: 24 bytes in a slot of 32');
                v = double(typecast(cmd(off + 4 + (1:20)), 'single')).';
                seq = double(typecast(cmd(off + 4 + (21:24)), 'uint32'));
                assert(lastseq < 0 || seq == lastseq + 1, 'seq must grow by 1');
                lastseq = seq;
                nmit = nmit + 1;
                lastmit = v;
                if cuberun
                    cubecnt = cubecnt + 1;      % the motor answers to the MIT only when enabled
                    vel = v(4);
                    pos = pos + vel * c.TS * c.CMD_DIV;
                end
            elseif cmd(off + 2) == c.ID_MOTOR_TARGET
                assert(cmd(off + 3) == 12, 'target: 12 bytes');
                value = double(typecast(cmd(off + 4 + (1:4)), 'single'));
                mode = double(cmd(off + 9));
                if mode == mmmode && mmmode == c.CM_SPEEDVOLTAGE
                    mmvel = value;              % deg/s
                end
            end
        end
        if mmmode == 0
            mmvel = 0;
        end
        mmcnt = mmcnt + 1;
        if pax && ~opts.paxbroken
            paxcnt = paxcnt + 10;
        end
        posncnt = posncnt + 1;
        f = zeros(16 + 6 * 40, 1, 'uint8');
        f(1:16) = sgact_tether_header(40, 6, 0);
        f = put(f, 0, 11, [uint8([cube + cuberun, 0]), bytes(uint16(0)), bytes(uint32(nmit)), bytes(uint32(0))]);
        f = put(f, 1, 12, [bytes(single(pos)), bytes(single(vel)), bytes(single(0.1 * vel)), bytes(int16(30)), bytes(uint16(0)), bytes(uint32(cubecnt))]);
        f = put(f, 2, 16, [bytes(single(0.5 * pax)), bytes(single(-0.5 * pax)), bytes(single(0.25 * pax)), bytes(uint32(paxcnt))]);
        f = put(f, 3, 17, [bytes(single(0)), bytes(single(mmvel)), bytes(single(0)), bytes(single(0)), bytes(uint32(0)), bytes(uint32(mmcnt)), ...
                           uint8([mmmode, mmconf, 1, gwfault])]);
        f = put(f, 4, 18, bytes(uint32([max(lastseq, 0), nmit + nhb, 0, 0, 0, 0, 0, 0, 0])));
        f = put(f, 5, 24, [bytes(single(12.5)), bytes(uint32(posncnt)), bytes(uint32(0)), bytes(uint16(0)), uint8([3, 0])]);
        st(1:numel(f)) = f;
        stlen = numel(f);
    end
    info = struct('cube', cube, 'cuberun', cuberun, 'pax', pax, 'ops', ops, 'nmit', nmit, 'nhb', nhb, 'lastmit', lastmit, 'paxcnt', paxcnt, ...
                  'mmconf', mmconf, 'mmmode', mmmode, 'mmcfg', mmcfg);
end


function f = put(f, i, id, data)
    off = 16 + 40 * i;
    f(off + 1) = 3;              % sig
    f(off + 2) = id;
    f(off + 3) = numel(data);
    f(off + 4 + (1:numel(data))) = data;
end


function b = bytes(x)
    b = reshape(typecast(x, 'uint8'), 1, []);
end

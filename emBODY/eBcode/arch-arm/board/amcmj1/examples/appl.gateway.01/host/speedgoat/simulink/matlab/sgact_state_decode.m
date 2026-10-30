function s = sgact_state_decode(raw, len)
%SGACT_STATE_DECODE the state frame of amc.gtw (UDP 5003) -> a struct (amc_gtw.decode_state)
%   the frame holds sig<IDcubeSTATUS>, sig<IDcubeSTATE>, sig<IDpaxTORQUE>, sig<IDmotorSTATE>, sig<IDgwSTATS>, sig<IDposSTATE>.
%   s.valid is true only if the frame is correct and holds IDgwSTATS. a ROP that is missing leaves its fields at 0 and has_xxx false
%   raw: uint8 vector (the width of the UDP Receive), len: bytes received (0 = nothing this step)

    c = sgact_const();
    s = struct('valid', false, 'time', 0, ...
               'has_motor_cubemars_status', false, 'motor_cubemars_status', 0, 'motor_cubemars_lastcommand', 0, 'motor_cubemars_errors', 0, 'motor_cubemars_commands', 0, 'motor_cubemars_watchdogs', 0, ...
               'has_motor_cubemars_state', false, 'motor_cubemars_pos', 0, 'motor_cubemars_vel', 0, 'motor_cubemars_current', 0, 'motor_cubemars_temp', 0, 'motor_cubemars_error', 0, 'motor_cubemars_counter', 0, ...
               'has_sensor_torque', false, 'sensor_torque_mx', 0, 'sensor_torque_my', 0, 'sensor_torque_mz', 0, 'sensor_torque_counter', 0, ...
               'has_motor_macro', false, 'motor_macro_pos', 0, 'motor_macro_vel', 0, 'motor_macro_current', 0, 'motor_macro_pwm', 0, ...
               'motor_macro_faults', 0, 'motor_macro_mode', 0, 'motor_macro_configured', 0, 'motor_macro_emulated', 0, 'motor_macro_counter', 0, ...
               'gw_extfault', 0, ...
               'has_sensor_position', false, 'sensor_position_pos', 0, 'sensor_position_counter', 0, 'sensor_position_errors', 0, ...
               'sensor_position_flags', 0, ...
               'gw', zeros(1, 9), ...  % seq packets discarded clamped rejected ignored lost badframes notrunning
               'fresh', false, 'nrecv', 0, 'trx', 0);   % filled by sgact_rx_step: a new frame this step, frames so far, time of the last one
    % NOTE: this struct is also the bus SGACT_FB (sgact_buses.m): a change here changes the bus, keep the order of the fields

    n = double(len);
    if n < c.HEADER_SIZE || numel(raw) < n
        return;
    end
    frame = uint8(raw(:));                      % uint8 whatever the type of the input
    if sgact_rd_u(frame, 1, 4) ~= double(c.SIGNATURE)
        return;
    end
    slot = double(frame(5));
    num = double(frame(6));
    if slot < 4 || num > c.MAX_ROPS || n ~= c.HEADER_SIZE + num * slot
        return;
    end
    s.time = sgact_rd_u(frame, 9, 8);

    hasgw = false;
    for i = 0:(num - 1)
        off = c.HEADER_SIZE + i * slot;            % the slot is frame(off + 1 : off + slot)
        opc = double(frame(off + 1));
        id = double(frame(off + 2));
        sz = double(frame(off + 3));
        if mod(opc, 16) ~= c.CMD_SIG || sz > slot - 4
            continue;
        end
        d = off + 4;                               % the data is frame(d + 1 : d + sz)
        if id == c.ID_CUBE_STATUS && sz == 12       % <BBHII  state lastcommand errors commands watchdogs
            s.has_motor_cubemars_status = true;
            s.motor_cubemars_status = double(frame(d + 1));
            s.motor_cubemars_lastcommand = double(frame(d + 2));
            s.motor_cubemars_errors = sgact_rd_u(frame, d + 3, 2);
            s.motor_cubemars_commands = sgact_rd_u(frame, d + 5, 4);
            s.motor_cubemars_watchdogs = sgact_rd_u(frame, d + 9, 4);
        elseif id == c.ID_CUBE_STATE && sz == 20    % <fffhHI  pos vel current temp error counter
            s.has_motor_cubemars_state = true;
            s.motor_cubemars_pos = sgact_rd_f32(frame, d + 1);
            s.motor_cubemars_vel = sgact_rd_f32(frame, d + 5);
            s.motor_cubemars_current = sgact_rd_f32(frame, d + 9);
            s.motor_cubemars_temp = sgact_rd_i16(frame, d + 13);
            s.motor_cubemars_error = sgact_rd_u(frame, d + 15, 2);
            s.motor_cubemars_counter = sgact_rd_u(frame, d + 17, 4);
        elseif id == c.ID_PAX_TORQUE && sz == 16    % <fffI  Mx My Mz counter
            s.has_sensor_torque = true;
            s.sensor_torque_mx = sgact_rd_f32(frame, d + 1);
            s.sensor_torque_my = sgact_rd_f32(frame, d + 5);
            s.sensor_torque_mz = sgact_rd_f32(frame, d + 9);
            s.sensor_torque_counter = sgact_rd_u(frame, d + 13, 4);
        elseif id == c.ID_MOTOR_STATE && sz == 28   % <ffffIIBBBB  current velocity position pwm faults counter mode configured emulated 0
            s.has_motor_macro = true;            % amc.mot speaks deg and deg/s: the bus speaks rad and rad/s
            s.motor_macro_current = sgact_rd_f32(frame, d + 1);
            s.motor_macro_vel = sgact_rd_f32(frame, d + 5) * pi / 180;
            s.motor_macro_pos = sgact_rd_f32(frame, d + 9) * pi / 180;
            s.motor_macro_pwm = sgact_rd_f32(frame, d + 13);
            s.motor_macro_faults = sgact_rd_u(frame, d + 17, 4);
            s.motor_macro_counter = sgact_rd_u(frame, d + 21, 4);
            s.motor_macro_mode = double(frame(d + 25));
            s.motor_macro_configured = double(frame(d + 26));
            s.motor_macro_emulated = double(frame(d + 27));
            s.gw_extfault = double(frame(d + 28));  % amc.gtw holds the EXTFAULT (OPgwFAULT)
        elseif id == c.ID_POS_STATE && sz == 16     % <fIIHBB  position counter errors raw flags 0
            s.has_sensor_position = true;         % deg on the wire: rad on the bus
            s.sensor_position_pos = sgact_rd_f32(frame, d + 1) * pi / 180;
            s.sensor_position_counter = sgact_rd_u(frame, d + 5, 4);
            s.sensor_position_errors = sgact_rd_u(frame, d + 9, 4);
            s.sensor_position_flags = double(frame(d + 15));
        elseif id == c.ID_GW_STATS && sz == 36      % <9I
            hasgw = true;
            for k = 1:9
                s.gw(k) = sgact_rd_u(frame, d + 4 * (k - 1) + 1, 4);
            end
        end
    end
    s.valid = hasgw;
end

function c = sgact_const()
%SGACT_CONST the constants of sgact, the Speedgoat side of the link to amc.gtw and to the panel
%   keep them aligned with host/shared/sgctl.py and emul/sgact_supervisor.py (panel link, supervisor),
%   host/shared/amc_gtw.py (gateway link) and host/shared/tethercodec.py (TETHER frames).
%   devices, actions, modes and their parameters: sgact_panel.toml -> sgact_spec.m. the network: sgact_netcfg.m

    % ---- timing of the model
    ts = 0.001;                     % (a local first: code generation does not let a field be read while the struct is built)
    c.TS = ts;                      % THE rate of the whole model [s]: the I/O blocks, the supervisor, USER CONTROL. it is chosen
                                    % before the build: change it, then build_sgact_lib and build_sgact (or set FixedStep of sgact.slx)
    c.CMD_DIV = 1;                  % one command frame every CMD_DIV steps: every step (the motor_macro wants a setpoint every 1 ms)
    c.STATUS_DIV = max(1, round(0.1 / ts));       % one STATUS to the panel every 100 ms, whatever TS

    % ---- panel link (sgctl.py, version 3)
    c.CTL_SIZE = 100;               % CTL panel -> sgact: 'SGC3' action mode devices flags cnt reqid crc duration params[8] mmcfg
    c.STATUS_SIZE = 104;            % STATUS sgact -> panel: 'SGS4' ... see sgact_status_frame.m
    c.MMCFG_N = 14;                 % the config of the motor_macro: curkp curki curkd velkp velki velkd nominal peak overload
                                    % flags poles entlr rotencres rotindexoff (sgact_spec.py: MMCFG_NAMES)

    % ---- TETHER frames (tethercodec.py)
    c.SIGNATURE = uint32(4276994270);   % 0xFEEDC0DE
    c.HEADER_SIZE = 16;
    c.CMD_SET = 2; c.CMD_SIG = 3; c.CMD_RUN = 7;
    c.OPT_ACK = 1;
    c.MAX_ROPS = 32;

    % ---- gateway link (amc_gtw.py)
    c.ID_CUBE_STATUS = 11; c.ID_CUBE_STATE = 12; c.ID_CUBE_MIT = 13;
    c.ID_PAX_TORQUE = 16; c.ID_MOTOR_STATE = 17; c.ID_GW_STATS = 18; c.ID_POS_STATE = 24;   % 0x18: the position sensor
    c.ID_MOTOR_TARGET = 22;                             % 0x16: the setpoint of the motor_macro in the command frame
    c.OP_CUBE_ENABLE = 209; c.OP_CUBE_DISABLE = 210;    % 0xD1 0xD2
    c.OP_PAX_ENABLE = 212; c.OP_PAX_DISABLE = 213;      % 0xD4 0xD5
    c.OP_MOTOR_PIDS = 214; c.OP_MOTOR_SETUP = 215; c.OP_MOTOR_MODE = 216;   % 0xD6 0xD7 0xD8, w/ an argument
    c.OP_GW_FAULT = 217;            % 0xD9 w/ uint8 1 / 0: the EXTFAULT held by amc.gtw (it refuses both motors until 0)
    c.FAULT_SYNC = 0.5;             % [s] amc.gtw does not say what sgact wants about the EXTFAULT: OPgwFAULT again
    c.CM_IDLE = 0; c.CM_CURRENT = 6; c.CM_SPEEDVOLTAGE = 10; c.CM_OPENLOOP = 80; c.CM_HWFAULT = 160;   % control modes of amc.mot
    c.FAULT_EXTERNAL = 128;         % bit 7 of the faults of the motor_macro: the external fault button is pressed
    c.MM_CONFIGURED = 3;            % bit 0 pids, bit 1 setup: what amc.gtw gave to amc.mot
    c.MIT_FRAME_SIZE = 48;          % header + one slot of 32 bytes (24 bytes of MIT)
    c.TARGET_FRAME_SIZE = 32;       % header + one slot of 16 bytes (12 bytes of target of the motor_macro)
    c.CMD_FRAME_MAX = 80;           % header + MIT and target in two slots of 32 bytes: the width of the UDP Send of the commands
    c.HB_FRAME_SIZE = 16;           % header only: no setpoint, it feeds the watchdog
    c.RPC_FRAME_SIZE = 24;          % header + one slot of 8 bytes (run<OP> w/ no argument, or w/ 1 byte)
    c.RPC_FRAME_MAX = 48;           % header + one slot of 32 bytes (run<OPmotorPIDS> w/ 24 bytes): the width of the UDP Send of the rpc
    c.STATE_MAX_SIZE = 512;         % width of the UDP Receive of the state (today 16 + 6 x 40 = 256)
    c.SEQ0 = 1;                     % first seq of the MIT

    % ---- the supervisor (sgact_sup_a.m, sgact_sup_b.m)
    c.RPC_ACK = 1;                  % run+ack<OP>, the same bytes as amc_gtw.py. the replies are not read (see README)
    c.RPC_GAP = 0.02;               % [s] between two rpc frames
    c.ENABLE_SETTLE = 0.05;         % [s] after the last enable before the devices are checked
    c.CHECK_ENABLE = true;          % init: check on the state frame that the devices really started
    c.CHECK_TIME = 1.0;             % [s] to see it: sensor_torque counter moves (else err 2), motor_cubemars status 1 or 2 (else err 1),
                                    % motor_macro configured (else err 5); and the control mode of a mode of the motor_macro (else err 5)
    c.RPC_QUEUE = 8;                % rpc operations that can wait
    c.AMCGTW_TIMEOUT = 1.0;         % [s] w/out a state frame while something is initialised or running: err 4
    c.STOP_RAMP = 0.5;              % [s] the ramp down of velocity and ramp at stop
    c.STOP_BRAKE = 0.2;             % [s] the braking of step and sine at stop
    c.STOP_MAX = 5.0;               % [s] a stop that takes longer is ended anyway
    c.MODE_NONE = 255;              % no mode running

    % ---- supervisor states, request results and reasons, errors (the STATUS carries them, the panel names them)
    c.ST_IDLE = 0; c.ST_INITIALISING = 1; c.ST_DEINITIALISING = 2; c.ST_RUNNING = 3; c.ST_STOPPING = 4;
    c.ST_EXTFAULT = 5;              % the EXTFAULT of the panel is pressed: all the motors stopped, every request refused
    c.CTL_EXTFAULT = 1;             % bit of the flags of the CTL: the EXTFAULT of the panel is pressed (sticky in the panel)
    c.RES_NONE = 0; c.RES_ACCEPTED = 1; c.RES_REJECTED = 2; c.RES_DONE = 3; c.RES_FAILED = 4;
    c.WHY_STATE = 1; c.WHY_DEVICES = 2; c.WHY_PARAMS = 3; c.WHY_MODE = 4; c.WHY_SPEC = 5; c.WHY_ACTION = 6;
    c.ERR_MOTOR_CUBEMARS = 1; c.ERR_SENSOR_TORQUE = 2; c.ERR_KEEPALIVE = 3; c.ERR_AMCGTW = 4; c.ERR_MOTOR_MACRO = 5; c.ERR_MMFAULT = 6;
    c.ERR_SENSOR_POSITION = 7;
    c.POS_VALID = 1;                % sensor_position_flags: bit 0 valid, bit 1 emulated
end

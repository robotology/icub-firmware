function fb = sgact_fb_default()
%SGACT_FB_DEFAULT the feedback from amc.gtw as it is before any state frame: the template of the bus SGACT_FB
%   the fields are the ones of sgact_state_decode (+ fresh, nrecv, trx). see sgact_buses.m
%     valid, time                     the last state frame is valid / its time stamp [us of amc.gtw]
%     has_motor_cubemars_status, motor_cubemars_status lastcommand errors commands watchdogs     sig<IDcubeSTATUS>
%                                     (motor_cubemars_status: 0 off, 1 initialised, 2 running, 3 error)
%     has_motor_cubemars_state, motor_cubemars_pos vel current temp error counter                sig<IDcubeSTATE> [rad, rad/s, A, C]
%     has_sensor_torque, sensor_torque_mx my mz counter                                            sig<IDpaxTORQUE> [N*m]
%     has_motor_macro, motor_macro_pos vel current pwm faults mode configured emulated counter
%                                     sig<IDmotorSTATE>: driven by amc.mot (CM7) [rad, rad/s, A, %]; mode: its control mode
%                                     (0 idle, 6 current, 10 speedvoltage, 80 openloop, 160 hwfault); faults: bit 7 external fault;
%                                     configured: bit 0 pids, bit 1 setup; emulated: amc.gtw plays amc.mot (no ICC yet)
%     gw_extfault                     1: amc.gtw holds the EXTFAULT (OPgwFAULT): it refuses both motors (MOTORstate::gwfault)
%     has_sensor_position, sensor_position_pos counter errors flags
%                                     sig<IDposSTATE>: the position sensor of the joint [rad]; flags: bit 0 valid, bit 1 emulated
%     gw(1:9)                         seq packets discarded clamped rejected ignored lost badframes notrunning of amc.gtw
%     fresh, nrecv, trx               a new frame arrived in this step, frames received so far, time of the last one [s]
    fb = sgact_state_decode(zeros(16, 1, 'uint8'), 0);
end

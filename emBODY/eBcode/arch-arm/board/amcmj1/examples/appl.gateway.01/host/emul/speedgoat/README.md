# speedgoat: Speedgoat emulator

speedgoat_emulator.py plays the Speedgoat: it sends the MIT commands (UDP 5001), enables/disables the CubeMars and/or the Paxini (rpc 5002: OPcubeENABLE, OPpaxENABLE ...) and receives the state
(UDP 5003, the board answers to the IP that sends the commands). Run it on Windows, from this folder:

    py speedgoat_emulator.py --info                       # what it does, profiles, limits, examples
    py speedgoat_emulator.py --test                       # automatic checks: hold, velocity, step, position, torque, frames lost
    py speedgoat_emulator.py --test all                   # plus limits and watchdog: only with the emulated motor
    py speedgoat_emulator.py --profile velocity --value 1 --kd 1 --duration 10 --enable cube --disable-at-end   # CubeMars only
    py speedgoat_emulator.py --profile idle --duration 10 --enable pax --disable-at-end                          # Paxini only, no motor
    py speedgoat_emulator.py --profile velocity --value 1 --kd 1 --duration 10 --enable both --disable-at-end   # both (--enable alone = both)
    py speedgoat_emulator.py --test --enable both                                                                  # the checks, with the Paxini too
    py speedgoat_emulator.py --profile position --value 0.5 --kp 5 --kd 0.5 --duration 6 --enable --disable-at-end

Profiles: zero (motor free), velocity, step, sine, ramp, position, torque, idle (heartbeats only). Options: --board, --kp, --kd, --freq, --step-s,
--period-ms (10, then 1), --duration, --seq. The board clamps kp 25, kd 3, |vel| 10 rad/s, |torque| 3 N*m and counts it.
Ctrl-C stops the stream: the board disables the motor after the watchdog (100 ms).

## Panel and daemon: choose the test with a mouse

    py speedgoat_emulator.py --daemon                    # the PC plays the Speedgoat, waits for the panel on UDP 6000 (heartbeats to the board meanwhile)
    py speedgoat_panel.py                                # Tk window: test (ramp, step, sine, velocity, position, torque, zero, idle), device (cube / pax / both),
                                                         # numbers, duration, START / STOP, status, strip charts of velocity, position, Paxini Mz
    py speedgoat_panel.py --host 10.0.2.150              # the same panel against the Simulink model on the real Speedgoat (build 'Control', see ../../../speedgoat/README.md)

Daemon and panel can run on different PCs (`--host`). Two small UDP packets (`sgctl.py`): CTL panel -> daemon (port 6000, 36 bytes, every 200 ms and at every click),
STATUS daemon -> panel (port 6001, 60 bytes, every 100 ms). START enables the devices and plays the profile, STOP disables them; the numbers can change while it runs.
Safety: if the panel closes or the network breaks, the daemon stops (disables) after `--ctl-timeout` (1 s); a duration > 0 in the panel stops it by itself.
Ramp: velocity from 0 to value in `time` s, then it holds. Windows firewall: allow Python to receive UDP on 6000 (daemon) and 6001 (panel).

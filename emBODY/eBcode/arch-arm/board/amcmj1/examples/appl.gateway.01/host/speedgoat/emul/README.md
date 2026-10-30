# emul: the PC that plays the Speedgoat

Two programs, for two different jobs. Run them on Windows, from `host`.

## sgact_daemon.py: sgact on the PC, for the panel

The same supervisor and the same modes w/ impl = matlab as the Simulink model (`sgact_supervisor.py`, `sgact_modes.py`), from the same
`../sgact_panel.toml`. It waits for the panel, streams a command frame every `--period-ms` (the setpoints of the running
mode, else heartbeats), executes init / deinit / start / stop, and sends the STATUS to the panel every 100 ms. Unlike the
Speedgoat it reads the ack / nak of the rpc of amc.gtw, and prints them.

    py speedgoat\emul\sgact_daemon.py                          # amc.gtw at 10.0.1.99, one frame every 10 ms
    py speedgoat\emul\sgact_daemon.py --period-ms 1            # every 1 ms (Python on Windows may not keep it)
    py speedgoat\panel\sgact_panel.py                          # then the panel, in another window

## speedgoat_emulator.py: a streamer straight to amc.gtw

A low-level test of amc.gtw: profiles of MIT setpoints for the motor_cubemars, no supervisor, no panel; with `--mm` also the
motor_macro (its config from the TOML, a control mode, a target in every frame).

    py speedgoat\emul\speedgoat_emulator.py --info              # what it does, profiles, limits, examples
    py speedgoat\emul\speedgoat_emulator.py --test              # automatic checks: hold, velocity, step, position, torque, frames lost
    py speedgoat\emul\speedgoat_emulator.py --test all          # plus limits and watchdog: only with the emulated motor
    py speedgoat\emul\speedgoat_emulator.py --profile velocity --value 1 --kd 1 --duration 10 --enable cube --disable-at-end
    py speedgoat\emul\speedgoat_emulator.py --profile idle --duration 10 --enable pax --disable-at-end         # Paxini only
    py speedgoat\emul\speedgoat_emulator.py --profile idle --mm velocity --mm-value 90 --duration 5           # motor_macro only

Profiles: zero (motor free), velocity, step, sine, ramp, position, torque, idle (no MIT). It speaks rad and rad/s, as the
MIT. Options: --board, --kp, --kd, --freq, --step-s, --period-ms, --duration, --seq, --enable cube|pax|both,
--disable-at-end, --mm velocity|current|voltage, --mm-value (deg/s, A, %). amc.gtw clamps kp 25, kd 3, |vel| 10 rad/s,
|torque| 3 N*m and counts it. Ctrl-C stops the stream: amc.gtw disables the motor_cubemars after its watchdog (100 ms), and puts
the motor_macro in idle after 100 ms w/out targets.

## Files

| file | what it is |
|---|---|
| `sgact_daemon.py` | sgact on the PC |
| `sgact_supervisor.py`, `sgact_modes.py` | the supervisor (step_a, user_control, step_b) and the modes of the daemon, aligned with `../simulink/matlab/sgact_sup_a.m`, `sgact_sup_b.m` and `sgact_mode_*.m`. The modes w/ impl = simulink are Speedgoat only |
| `speedgoat_emulator.py` | the streamer |
| `test_sgact_supervisor.py` | the supervisor against a fake amc.gtw (motor_cubemars, sensor_torque, motor_macro, sensor_position, EXTFAULT w/ OPgwFAULT): 39 checks; it writes `../simulink/tests/parity_trace.txt.gz`, which `test_parity.m` plays again in MATLAB |
| `test_daemon_loopback.py` | daemon + panel client + a fake amc.gtw over UDP on 127.0.0.1: 16 checks. It uses the ports 5001-5003 and 6000-6001 |

The codecs are in `host/shared`: `amc_gtw.py` (the frames of amc.gtw), `sgctl.py` (the link w/ the panel), `sgact_spec.py`
(the TOML), `tethercodec.py`. A new mode: see "A new mode" in `../simulink/README.md`; here it is a `mode_<name>()` in
`sgact_modes.py`.

Windows firewall: allow Python to receive UDP on 5003 (state of amc.gtw) and 6000 (CTL of the panel).

# simulink: sgact on the Speedgoat

The MATLAB / Simulink R2026b project of **sgact**, the Speedgoat actuator. The requests, the motor_macro, the TOML and the link
with the panel are described in `../README.md`; this file is about the model.

```
  PC (Windows)                          Speedgoat SG10                              amcmj1
  sgact_panel.py  --CTL 6000-->    Host Link 10.0.2.150   ETH1 10.0.1.150  --commands 5001-->  amc.gtw (CM4) 10.0.1.99
                  <--STATUS 6001--                                         --rpc 5002------->     |  CAN: motor_cubemars, sensor_torque
                                                                           <--state 5003-----     |  ICC: amc.mot (CM7) -> motor_macro
```

## The model

```
  [amc.gtw RX] --fb-----+--> [Controller] --cmd----> [amc.gtw TX] --nsent--+
  [panel RX]   --req----+          |                                         |
                                   +--------------sup----> [panel TX] <------+
  fb also goes to panel TX
```

Inside the Controller (all at the same rate, `TS` of `matlab/sgact_const.m`, now 1 ms):

```
  fb, req --> [SUP-A] --run---------------------------+----------------------+
                 ^  \--cmda, sup--------------------  |  ------------------> [SUP-B] --cmd-->
                 |                                  v                      ^
                 |      [USER CONTROL] (cyan)                              |
                 |        +-- enable --> [standard modes]  (the .m modes)  |
                 |        +-- enable --> [mode_motor_cubemars_user]        |
                 |        +-- enable --> [mode_spark_dual_motor_actuation] |
                 |        +--> [Merge] --sp--------------------------------+
                 |                         |
                 +------- [z^-1] <-- done -+                 SUP-A --sup-->
```

- **SUP-A** (`matlab/sgact_sup_a.m`): the requests init / deinit / start / stop, the watchdogs, the state machine, the rpc.
  It says which mode runs and in which phase in the bus `run`.
- **USER CONTROL**: the place of the control engineer. One enabled subsystem per mode: `standard modes` holds the modes
  w/ `impl = "matlab"` (`matlab/sgact_mode_<name>.m`), and `mode_<name>` is a mode w/ `impl = "simulink"`, made of blocks.
  Only the subsystem of the running mode is enabled (its states reset at the enable); the Merge gives the bus `sp`.
- **SUP-B** (`matlab/sgact_sup_b.m`): the gate. The MIT reaches the motor_cubemars only if it is initialised, the target the
  motor_macro only while a mode w/ a controlmode runs or stops. When no mode runs, nothing: the motor_cubemars is disabled.
- `done` of `sp` goes back to SUP-A through a unit delay (it breaks the algebraic loop): the stop ends one step later.

| block | what it does | where |
|---|---|---|
| **amc.gtw RX** | state frame (ETH1:5003) → bus `fb` | library `sgact_lib.slx` (`build_sgact_lib.m`) |
| **amc.gtw TX** | bus `cmd` → command frames (:5001) every `CMD_DIV` steps (1 ms: the MIT and / or the target of the motor_macro, else a heartbeat), and the rpc (:5002, with their argument); output `nsent` | library |
| **panel RX** | CTL (Host Link:6000) → bus `req` (also `extfault`) | library |
| **panel TX** | buses `sup`, `fb` and `nsent` → STATUS v4 (:6001), every 100 ms | library |
| **Controller** | SUP-A, USER CONTROL, SUP-B: `req` + `fb` → `cmd` + `sup` | `sgact.slx` (`build_sgact.m`, once; `sgact_sync_modes.m` after a change of the modes) |

The buses are made from the default structs by `sgact_buses.m`:

| bus | content |
|---|---|
| `SGACT_FB` | the last state frame of amc.gtw: `motor_cubemars_*`, `sensor_torque_*`, `motor_macro_*` (pos vel current pwm faults mode configured emulated counter, in rad and rad/s), `sensor_position_*` (pos [rad] counter errors flags), `gw(1:9)`, plus `fresh`, `nrecv`, `trx` |
| `SGACT_REQ` | the request: `have alive crc_ok cnt id action devices mode duration params(1:8) mmcfg(1:14) extfault` |
| `SGACT_CMD` | the motor_cubemars MIT `motor_cubemars_mit_valid kp kd pos vel torque`, the motor_macro `motor_macro_valid mode target` (SI), one rpc `rpc_go rpc_op rpc_arg`, and `mmcfg(1:14)` (the argument of OPmotorPIDS and OPmotorSETUP) |
| `SGACT_SUP` | `have state mode inited err result reason action reqid cnt trun` |
| `SGACT_RUN` | SUP-A → USER CONTROL, SUP-B: `mode phase first t trun p(1:8) inited mmode` (phase 0 none, 1 run, 2 stop; `p` in SI) |
| `SGACT_SP` | USER CONTROL → SUP-B: `valid kp kd pos vel torque mmtarget done` (the MIT of the motor_cubemars, the target of the motor_macro, the end of the stop) |

Another requester can replace the panel, as long as it produces `SGACT_REQ` and reads `SGACT_SUP`.

**The gate**: a MIT reaches the motor_cubemars only if the motor_cubemars is initialised: from the running or stopping mode, or the last
MIT of the last stop, which amc.gtw would repeat anyway. A target reaches the motor_macro only while a mode with a control mode
runs or stops. Otherwise sgact sends heartbeats.

Logged signals (Data Inspector, also while streaming):
- `state mode inited err`;
- `sp_motor_cubemars_*`: the MIT; `sp_motor_macro_mode`, `sp_motor_macro_target`: the setpoint of the motor_macro (SI);
- `motor_cubemars_pos vel current`, `sensor_torque_mx my mz`, `motor_macro_pos vel current pwm mode faults`.

### A new mode

A mode is a section of `../sgact_panel.toml`: `[modes.<name>]` with id, needs, help, params, and `controlmode` if it
drives the motor_macro ("current", "speedvoltage" or "openloop"). Its `impl` says how it is written.

**impl = "simulink"** (blocks, the control engineer; Speedgoat only, the daemon does not offer it):
1. Add the section w/ `impl = "simulink"`.
2. In MATLAB: `build_sgact_lib`, then `sgact_sync_modes`. It creates `Controller/USER CONTROL/mode_<name>` from a template
   and wires its enable and the Merge. The subsystems that exist already are not touched.
3. Open `mode_<name>`: it has `run` and `fb` in, `sp` out. Between the Bus Selectors and "pack sp" put the control:
   - `run.p(1:8)`: the parameters, in the order of the TOML and in SI (deg → rad, deg/s → rad/s); `run.phase` 1 run, 2 stop;
     `run.first` true at the first step of a phase; `run.t` the time since the phase started;
   - "pack sp" wants `valid kp kd pos vel torque mmtarget done`: the MIT of the motor_cubemars (used if `valid`), the target of
     the motor_macro (SI: A, rad/s, %), and `done` (in the stop phase: the exit is over; then SUP-A goes to idle).
   As the template is, it does nothing: valid 0, done 1. `mode_motor_cubemars_user` and
   `mode_spark_dual_motor_actuation` start w/ a working content (`tools/sl_mode_kind.m`): the MIT of the parameters
   `p(1:5) = pos vel kp kd torque`; the second also selects all the feedback (both motors, sensor_torque, sensor_position)
   and gives 0 rad/s to the motor_macro. At the end of the stop SUP-A disables the motor_cubemars and idles the motor_macro.
4. Remove a mode: delete its section in the TOML and its subsystem; `sgact_sync_modes` warns about orphans.

**impl = "matlab"** (the standard modes, run by the subsystem `standard modes`):
1. Add the section (no `impl`, or `impl = "matlab"`).
2. Write `matlab/sgact_mode_<name>.m` (copy one of the others): `[valid, sp, done] = f(phase, p, t, fb)`, with
   `sp = [kp kd pos vel torque mmtarget]`.
3. Write `mode_<name>()` in `../emul/sgact_modes.py` for the daemon, or mark the mode `speedgoat_only = true`.
4. `build_sgact_lib`. The generator stops if one of the two implementations is missing.

The panel shows the new mode without any change.

**The rate**: everything runs at `TS` of `matlab/sgact_const.m` (now 0.001 s). Change it before the build, then
`build_sgact_lib` and `build_sgact(true)`. A slower part of a mode is the control engineer's: a Rate Transition inside its
subsystem.

## Files

| file | what it is |
|---|---|
| `build_sgact_lib.m` | generates `sgact_spec.m`, then creates `sgact_lib.slx` (the four I/O blocks). Run it again at every change of the TOML, of the protocol, or of `sgact_netcfg.m` |
| `build_sgact.m` | creates `sgact.slx` **once**; `build_sgact(true)` overwrites it (and the subsystems of USER CONTROL) |
| `sgact_sync_modes.m` | after a change of the modes w/ impl = simulink: adds the missing `mode_<name>` to USER CONTROL and wires them |
| `sgact_setup.m`, `sgact_deploy.m` | path, check of the spec, bus objects; slbuild + load + start |
| `matlab/sgact_sup_a.m`, `sgact_sup_b.m` | the supervisor, in two: SUP-A (requests, state machine, rpc), SUP-B (the gate) |
| `matlab/sgact_std_modes.m`, `sgact_std_on.m` | the subsystem `standard modes`: the dispatch of the .m modes, and its enable |
| `matlab/sgact_controller.m` | SUP-A + modes + SUP-B in one call, as the Controller, for the tests (a simulink mode is a placeholder there) |
| `matlab/sgact_mode_<name>.m` | the modes w/ impl = matlab: motor_cubemars_position, motor_cubemars_velocity_step, motor_cubemars_idle; motor_macro_velocity, motor_macro_current, motor_macro_voltage; idle |
| `matlab/sgact_spec.m`, `sgact_modes_dispatch.m`, `sgact_modes_info.m` | **generated** from the TOML: do not edit |
| `matlab/sgact_{gtw,panel}_{rx,tx}_step.m` | the code of the four I/O blocks |
| `matlab/sgact_*_default.m`, `sgact_buses.m` | the buses |
| `matlab/sgact_const.m`, `sgact_netcfg.m` | timing, IDs, timeouts, codes; addresses and ports |
| `matlab/sgact_{cmd,mit,hb,run,rpc}_frame.m`, `sgact_rpc_arg.m` | the frames to amc.gtw: commands (MIT, target), rpc (with argument) |
| `matlab/sgact_state_decode.m`, `sgact_ctl_{frame,decode}.m`, `sgact_status_frame.m` | the state frame of amc.gtw (6 slots, w/ IDposSTATE), CTL, STATUS v4 |
| `matlab/sgact_step.m` | the whole chain in one call, for the tests |
| `tools/gen_sgact_spec.py` | the generator: TOML → `matlab/sgact_spec.m`, `sgact_modes_dispatch.m`. It reads the TOML w/ `host/shared/sgact_spec.py` |
| `tools/make_golden.py` | writes `tests/golden.txt` w/ the Python codecs of `host/shared`; `state_cpp` comes from the firmware (`appl.gateway.01/test/host`) |
| `tools/sgact_gen_spec.m`, `sgact_crc32.m`, `sl_*.m` | generator call and CRC check; helpers of the build scripts (`sl_enabled_subsystem`, `sl_mode_subsystem`, `sl_mode_kind`, `sl_user_control_wire`: USER CONTROL) |
| `tests/test_golden.m` | the bytes must match `golden.txt`: MATLAB = Python = C++ |
| `tests/test_link.m` | the I/O blocks and the buses: fields, classes, sizes, decoding, periods |
| `tests/test_sgact_step.m` | the whole chain with real bytes against a fake amc.gtw (`fake_amcgtw.m`), the motor_macro included |
| `tests/test_parity.m` | the MATLAB supervisor plays again `parity_trace.txt.gz`, written by the Python one (`../emul/test_sgact_supervisor.py`): they must give the same outputs |

## Use it

```matlab
cd <repo>/emBODY/eBcode/arch-arm/board/amcmj1/examples/appl.gateway.01/host/speedgoat/simulink
sgact_setup
test_golden; test_link; test_sgact_step; test_parity     % 29/29, 18/18, 14/14, 31000/31000 (also w/out Simulink, in Octave)
build_sgact_lib
build_sgact(true)              % sgact.slx: the new Controller (SUP-A, USER CONTROL, SUP-B); an older sgact.slx does not compile
tg = sgact_deploy;
```

Then on the PC, from `host`:
1. the emulators, if there are no real devices;
2. `py speedgoat\panel\sgact_panel.py --host 10.0.2.150`, then INIT and START.

The panel and the generator find the TOML in `host/speedgoat`. If it is elsewhere, give `--spec <path>` or set `SGACT_SPEC`.

Prerequisites:
- On the Speedgoat:
  - ETH1 (`enp2s0`) at 10.0.1.150 via `eth1-static.service`, and the Host Link at 10.0.2.150;
  - `/etc/sysctl.d/90-two-nics.conf`, because both NICs are on the same switch;
  - if the ETH1 link stays down after plugging the cable: `sudo ip link set enp2s0 down; sudo ip link set enp2s0 up; sudo systemctl restart eth1-static.service`.
- On the PC: Python 3.11 or later (the generator); allow Python to receive UDP 6001 in the Windows firewall. The STATUS goes
  to `PANEL_IP` (10.0.2.104).

## Notes

- **One command frame every 1 ms** (`CMD_DIV = 1` in `sgact_const.m`): the motor_macro wants its setpoint every 1 ms. The
  motor_cubemars gets its MIT at the same rate. To go back to 10 ms, set `CMD_DIV = 10` and run `build_sgact_lib`: amc.gtw sends
  the motor_macro to idle only after 100 ms w/out targets.
- **The rpc replies are not read on the Speedgoat.** A UDP Send and a UDP Receive cannot share a local port there. So init
  is checked on the state frame instead (`CHECK_ENABLE`, `CHECK_TIME` in `sgact_const.m`): the motor_macro must say
  configured, and the control mode of its mode must appear in the state within 1 s. The daemon on the PC does read the
  ack / nak and prints them.
- **The motor_macro is emulated by amc.gtw** until the ICC w/ amc.mot exists: the state says `emulated`, and the panel
  shows it.

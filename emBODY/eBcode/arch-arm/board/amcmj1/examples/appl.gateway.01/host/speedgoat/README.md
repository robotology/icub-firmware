# speedgoat: sgact, the Speedgoat actuator

**sgact** drives the SPARK joint from the Speedgoat. It talks to two peers:
- **amc.gtw**, the gateway application on the CM4 of the amcmj1 board. amc.gtw drives the **motor_cubemars** and reads the
  **sensor_torque** over CAN, reads the **sensor_position** (an SPI encoder, every 1 ms; emulated until it is wired), and
  it talks over the ICC to **amc.mot** on the CM7, which drives the **motor_macro**;
- the **panel**, which sends requests and shows the state.

```
  PC (Windows)                          Speedgoat SG10 (or sgact_daemon.py on the PC)          amcmj1
  panel/sgact_panel.py  --CTL 6000-->   Host Link 10.0.2.150   ETH1 10.0.1.150  --commands 5001-->  amc.gtw (CM4) 10.0.1.99
                        <--STATUS 6001--                                        --rpc 5002------->     |  CAN: motor_cubemars, sensor_torque
                                                                                <--state 5003-----     |  ICC: amc.mot (CM7) -> motor_macro
                                                                                                       |  SPI: sensor_position
```

| folder / file | what it is |
|---|---|
| `sgact_panel.toml` | what can be requested and shown: devices, the config of the motor_macro, actions, modes and their parameters, plots. Read by the three below; its CRC travels in every CTL |
| `panel/` | `sgact_panel.py`: the window, built from the TOML |
| `emul/` | the PC that plays the Speedgoat: `sgact_daemon.py` (sgact on the PC, for the panel) and `speedgoat_emulator.py` (a streamer straight to amc.gtw) |
| `simulink/` | sgact on the Speedgoat: the MATLAB / Simulink project, its build scripts and tests |

## The requests

Four requests, each one independent: none of them triggers another.

| request | from | what the supervisor does |
|---|---|---|
| **init**(devices) | idle | sensor_torque: enable; motor_cubemars: enable; motor_macro: its config (`OPmotorPIDS`, `OPmotorSETUP`, taken from the CTL of this request); sensor_position: nothing to send. Then it checks them on the state frame: the sensor_torque counter moves, the motor_cubemars status is 1 or 2, the motor_macro is configured, the sensor_position is valid and its counter moves. A device that fails: err 1 / 2 / 5 / 7, and it is disabled again. The motor_cubemars, once checked, is disabled again (`OPcubeDISABLE`) |
| **deinit**(devices) | idle | motor_cubemars disable, sensor_torque disable, motor_macro idle |
| **start**(mode, params, duration) | idle | runs the mode, if its devices are initialised and its parameters in range. First: a mode that needs the motor_cubemars enables it (`OPcubeENABLE`), any other disables it (`OPcubeDISABLE`); a mode w/ a control mode asks it (`OPmotorMODE`), any other puts the motor_macro in idle (the devices initialised only) |
| **stop** | running | the mode does its own exit, then idle: the motor_cubemars disabled, the motor_macro idle. The devices stay initialised |

**The motor_cubemars is enabled only while one of its modes runs** (but `motor_cubemars_idle`). Every disable of amc.gtw
(OPcubeDISABLE, its watchdog, OPgwFAULT) sends the MIT of rest (kp kd pos vel torque = 0) and then the disable packet on
CAN: the motor does not drift. A MIT w/ kp = kd = 0 is not a true rest: the 12-bit quantization of the protocol cannot
give 0 for vel and torque (about -0.008 N*m is left), and a motor w/out friction, as the emulator, turns slowly.

**EXTFAULT** (the big button of the panel, sticky; virtual: there is no emergency switch wired): pressed, sgact stops
at once, w/out the exit of the mode: the rpc not yet sent are dropped and amc.gtw gets `run<OPgwFAULT 0xD9, 1>`. amc.gtw
disables the motor_cubemars, puts the motor_macro in idle and **holds the fault**: until `OPgwFAULT 0` it does not use the
MIT and the targets, and it refuses `OPcubeENABLE` and a control mode other than idle. sgact goes to the state `EXTFAULT`
(5), sends no setpoint and refuses every request. Pressed again: `OPgwFAULT 0`, idle, w/ the devices still initialised.
The state frame tells whether amc.gtw holds it (`MOTORstate::gwfault`, `gw_extfault` in the STATUS, shown by the panel); if
amc.gtw and sgact disagree (one of them restarted), sgact sends `OPgwFAULT` again every 0.5 s. The panel sends EXTFAULT
in every CTL (flags bit 0): it stays also if the panel goes silent; a new panel starts w/ it released.

By itself: the panel silent for 1 s while a mode runs: stop, err 3; the duration expires: stop; amc.gtw silent for 1 s:
err 4, everything off; the motor_macro not in the control mode of the mode after 1 s: stop, err 5; a fault of the motor_macro
(external fault pressed, or hwfault) while its mode runs: idle at once, err 6.

## The motor_macro

The motor_macro is driven by amc.mot (CM7). amc.gtw takes the requests of sgact and gives them to amc.mot over the ICC.
**Until the ICC exists, amc.gtw plays amc.mot with a simple model** (`AMCMJ1_MOTDRIVER_EMULATED` in
`src/amcmj1_motdriver.cpp`): the panel shows "EMULATED by amc.gtw".

| what | where | content |
|---|---|---|
| config | rpc 5002, at the init | `run<OPmotorPIDS 0xD6, {curkp curki curkd velkp velki velkd}>` (6 float), `run<OPmotorSETUP 0xD7, {nominal peak overload, flags poles entlr 0, rotencres rotindexoff}>` (3 float [A], 4 uint8, 2 int16: SET_MOTOR_CONFIG 0x77). Only in idle |
| control mode | rpc 5002, at the start and at the end of a mode | `run<OPmotorMODE 0xD8, mode>`: 0x00 idle, 0x06 current, 0x0A speedvoltage, 0x50 openloop. Not idle only after the config |
| setpoint | commands 5001, every 1 ms while a mode of the motor_macro runs | `set<IDmotorTARGET 0x16, {value, mode, seq}>` (float, uint8 + 3 x 0, uint32): current [A], velocity [deg/s], pwm [%]. Used only if mode is the active one. No setpoint for 100 ms: amc.gtw goes idle |
| state | state 5003, in every state frame | `sig<IDmotorSTATE 0x11, {current velocity position pwm faults counter mode configured emulated gwfault}>` (4 float [A, deg/s, deg, %], 2 uint32, 4 uint8). faults bit 7: external fault (of amc.mot); gwfault: amc.gtw holds the EXTFAULT |
| EXTFAULT | rpc 5002, at the EXTFAULT of the panel | `run<OPgwFAULT 0xD9, uint8 1 / 0>`: 1 stops both motors and holds it, 0 clears it |

The config is in `[devices.motor_macro.config]` of the TOML. The panel shows it, it can be changed there before INIT, and it
is sent in every CTL. sgact gives the config of the INIT request to amc.gtw.

Units: on the wire to amc.gtw the motor_macro speaks deg and deg/s, as amc.mot; inside sgact (buses, modes, STATUS) every
angle is in rad and rad/s; the panel shows deg (`[display] angles` of the TOML).

## sgact_panel.toml

- **the panel** builds its window from it: device boxes, the config of the motor_macro, the mode menu, one field per
  parameter, the plots;
- **the daemon** reads it directly (it runs the modes w/ `impl = "matlab"` only);
- **the Speedgoat**: `simulink/tools/gen_sgact_spec.py` turns it into `simulink/matlab/sgact_spec.m`,
  `sgact_modes_dispatch.m` and `sgact_modes_info.m`. `build_sgact_lib` runs the generator by itself.

Every mode has an `impl`: `"matlab"` (default) is a `.m` file, run by the daemon too; `"simulink"` is a subsystem of
blocks in `Controller/USER CONTROL` of sgact.slx, Speedgoat only (`sgact_sync_modes` creates it).

The modes:

| mode | id | needs | impl | what it does |
|---|---|---|---|---|
| `motor_cubemars_position` | 0 | motor_cubemars | matlab | the MIT w/ pos vel kp kd torque, constant |
| `motor_cubemars_velocity_step` | 1 | motor_cubemars | matlab | the MIT w/ vel = +vel, -vel, ... every half s (pos kp kd torque as given) |
| `motor_cubemars_idle` | 2 | motor_cubemars | matlab | disabled at its start (`cubemars_enable = false`): amc.gtw sends the MIT of rest, all 0 (shown in the panel, fixed), then the disable packet |
| `motor_cubemars_user` | 3 | motor_cubemars | simulink | user-defined blocks; as built, the MIT of its parameters |
| `motor_macro_velocity` / `_current` / `_voltage` | 10 / 11 / 12 | motor_macro | matlab | constant velocity [deg/s] / current [A] / pwm [%] |
| `idle` | 30 | - | matlab | all the motors stop (the motor_cubemars disabled, the motor_macro idle); the sensors go on |
| `spark_dual_motor_actuation` | 20 | all four | simulink | the demo: both motors from the feedback of the motors and of the sensors. As built: the MIT of its parameters, the motor_macro at 0 deg/s (speedvoltage); `forfutureuse`: a float for later |

The parameters of the MIT are always in the order pos vel kp kd torque.

The CRC of the TOML travels in every CTL: sgact rejects the requests of a panel that read a different file (reason 5),
and the panel shows it in red.

A new mode: see "A new mode" in `simulink/README.md`.

## The link panel <-> sgact (`host/shared/sgctl.py`, version 4)

- CTL panel → sgact (port 6000, 100 bytes 'SGC3', every 200 ms and at every click): action, mode, devices, flags (bit 0:
  EXTFAULT), cnt, reqid, crc, duration, params[8], the config of the motor_macro;
- STATUS sgact → panel (port 6001, 104 bytes 'SGS4', every 100 ms): state, mode, inited, err, result, reason, the feedback
  of the motor_cubemars, sensor_torque and motor_macro (also its control mode, pwm and faults), gw_extfault, the counters
  of the link, the sensor_position (pos, counter, flags).

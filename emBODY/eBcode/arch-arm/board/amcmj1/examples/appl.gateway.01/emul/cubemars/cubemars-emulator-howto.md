# The CubeMars AK70-9 on amcmj1: MIT and servo over CAN (29 bit)

```
PC 10.0.1.104
+---------------------------------------------+
| cubemars_panel  --backend tether            |
+---------------------+-----------------------+
                      ^
                      |  UDP 6666 (rpc), both directions
                      v
amcmj1 10.0.1.99 (CM4)
+---------------------------------------------+
| theTETHERservice -> SharedVariables         |
|   IDcubeMIT (RW)  IDcubeSERVO  IDcubeSTATE  |
|                                             |
| tCTRL: update() + tick() every 10 ms        |
|   cubedriver - thePROThandler<CubeMars>     |
|   paxdriver  - thePROThandler<PX3Q>         |
|                                             |
| amcmj1::canbus - embot::hw::can             |
+---------------------+-----------------------+
                      ^
                      |  CAN 1 Mbps
                      |  (CubeMars: 29 bit ids)
                      v
+---------------------------------------------+
| CubeMars AK70-9                             |
| (or cubemars_emulator.py on the Vector)     |
+---------------------+-----------------------+
                      |
                      |  UDP :5005 JSON (emulator only)
                      v
+---------------------------------------------+
| cubemars_viewer.py                          |
+---------------------------------------------+
```

## What is new

| where | what |
|---|---|
| `embot_hw_can.h/.cpp`, `embot_hw_can_bsp_amcmj1.cpp` | `Frame::isextended()` / `setextended()` and a 4th constructor argument `extended = false` (the old 3-argument calls are unchanged; 29 bit id), `Config::acceptextended`, an FDCAN extended filter (`ExtFiltersNbr = 1`). `sizeof(Frame)` is still 16. A standard and an extended frame with the same number are different frames on the wire |
| `theprothandler.h` | `CubeMarsProtocol` (MIT, duty, current, brake, velocity, position, origin, pos-vel, disable, feedback config; decode of 0x29, 0x2A, 0x2C). `PX3Qprotocol::parse()` now ignores extended frames |
| `amcmj1_canbus` | owns `embot::hw::can`: it opens it once (`acceptextended = true`), drains the rx queue and gives the frames to the attached drivers in turn. What nobody decodes is counted (`unknown`) |
| `amcmj1_cubedriver` | the setpoint loop, the watchdog, the decoding of the status |
| `amcmj1_paxdriver` | `open(rxsignal)` is now `open()` and `rx(value)` is `take(value)`: the draining moved to `amcmj1_canbus`. Nothing else changed |
| `amcmj1_ctrl.h` | `evtPAXrx` is now `evtCANrx` (it wakes the thread for the frames of both devices), new `evtCUBEinit / deinit / start / stop`. The tick is 10 ms |
| Dictionary | below |
| `host/cubemars/` | emulator, viewer, panel, `cubetether.py` (the panel through the board), tests |

## CAN ids

CubeMars: extended, `(packet << 8) | driverID`, default driverID 0x68: MIT 0x0868, velocity 0x0368, disable 0x0F68; replies 0x2968 (status), 0x2A68, 0x2C68.
PX3Q: standard, 0x180 / 0x200 / 0x280 / 0x380 + id (default 0x7F). No collision: the IDE bit tells them apart, and each decoder checks it.
Mind the CANopen defaults if a Synapticon comes later: they overlap the PX3Q blocks.

## The Dictionary

| ID | name | type | access | notes |
|---|---|---|---|---|
| 0x0A | `IDcubeCFG` | `{canid, repeatms, watchdogms : u32}` | RO | set by `OPcubeINIT` |
| 0x0B | `IDcubeSTATUS` | `{state:u8, lastcommand:u8, errors:u16, commands:u32, watchdogs:u32}` | RO | state: 0 off, 1 initialised, 2 running, 3 error |
| 0x0C | `IDcubeSTATE` | `{pos:f32 rad, vel:f32 rad/s, current:f32 A, temp:i16, error:u16, counter:u32}` | RO | from the 0x29 (and 0x2A) frames; written when frames arrive |
| 0x0D | `IDcubeMIT` | `{kp, kd, pos, vel, torque : f32, seq:u32}` | RW | **the MIT setpoint**. The writer changes `seq` at every write |
| 0x0E | `IDcubeSERVO` | `{packet:u8, a, b, c : f32, seq:u32}` | RO | the last servo setpoint, written by `OPcubeSERVO` |
| 0xCC | `OPcubeINIT` | `{canid, repeatms, watchdogms}` | | canid [1, 255], repeatms [10, 100], watchdogms [2*repeat, 10000] |
| 0xCD / 0xCE / 0xCF | `OPcubeDEINIT / START / STOP` | | | STOP sends the disable packet |
| 0xD0 | `OPcubeSERVO` | `{packet:u8, a, b, c : f32}` | | packet 0 duty (a), 1 current A, 2 brake A, 3 velocity ERPM, 4 position deg, 5 origin (a = 0 / 1, once), 6 pos-vel (deg, ERPM, ERPM/s²) |

MIT ranges (AK70-9): kp [0, 500], kd [0, 5], pos ±12.56 rad, vel ±30 rad/s, torque ±32 N·m. Out of range: `OPcubeSERVO` is a nak; a write of `IDcubeMIT` is ignored by the driver (`txerrors`) and the previous setpoint goes on.

## The loop and the watchdog (no news = stop)

The motor does not ack MIT or servo packets, and the MIT law needs the setpoint again and again. So:

1. `OPcubeINIT` stores the id, `OPcubeSTART` makes the driver `running`. The values that are already in the variables at START are ignored.
2. Every 10 ms tCTRL reads `IDcubeMIT` and `IDcubeSERVO`. When a `seq` changes, that setpoint becomes the active one and goes out at once; then it goes out again every `repeatms`.
3. If no `seq` changes for `watchdogms`, the driver sends the disable packet once, stops sending and counts a `watchdogs`. Write again to move again.
4. `OPcubeSTOP` sends disable. `origin` (packet 5) is a one shot and does not change the active setpoint.

So whoever writes `IDcubeMIT` (the panel today, Simulink later, which repeats it anyway) must keep writing it, with a different `seq` each time. The panel in tether mode does that for you: the last command is written again every 50 ms until DISABLE.

## Try it

PC, with the emulator on the Vector wired to the CAN of the board:

```
py cubemars_emulator.py --interface vector --channel 0 --app-name none --publish 127.0.0.1:5005
py cubemars_viewer.py --port 5005
py cubemars_panel.py --backend tether --board 10.0.1.99        # window with INIT / START / STOP / DEINIT of the board
```

In the panel: INIT (id 0x68, repeat 10, watchdog 100), START, then the MIT tab, e.g. "hold here: Kp 20 Kd 1.5", Send once. The viewer shows the motor. Close the panel without DISABLE: after 100 ms the board disables the motor (`IDcubeSTATUS.watchdogs` counts it). Same from tether.py:

```
> run[+ack]<OPcubeINIT, {canid=104, repeatms=10, watchdogms=100}>
> run[+ack]<OPcubeSTART>
> run[+ack]<OPcubeSERVO, {packet=3, a=2000, b=0, c=0}>      (run it again before 100 ms)
> set<IDcubeMIT, {kp=20, kd=1.5, pos=0, vel=0, torque=0, seq=1}>   (seq=2, 3, ...)
> ask<IDcubeSTATE>; ask<IDcubeSTATUS>
```

`--backend can` is the old panel: a CAN master on a second Vector channel, no board involved (to try the emulator alone).



## To verify on the bench

- The format of the CAN reply to a MIT packet is not in the manual v3.2.0: the driver assumes the same 0x29 (and 0x2A) of the servo mode.
- `rxsignal` fires per frame: with the PX3Q at 1 kHz (2000 frames/s) and the motor, check `canbus` `rxoverflow` and the load of tCTRL. The rx queue holds 64 frames.
- FDCAN: `ExtFiltersNbr = 1` and the extended filter in `embot_hw_can.cpp` are written, not run on the STM32 HAL here.

# host: the PC side of amcmj1.appl.gateway.01

```
host/
├── shared/       the codecs of the links and the reader of sgact_panel.toml, imported by the tools below
├── tether/       TETHER: diagnostics of amc.gtw (rpc 6666, stream 7777)                          -> Linux / WSL2
├── emul/         emulators of the devices on the CAN of the amcmj1 (Vector interface)            -> Windows
│   ├── cubemars/     CubeMars AK70-9: emulator, viewer, bench panel over TETHER
│   └── paxini/       PaXini PX3Q
└── speedgoat/    sgact, the Speedgoat actuator                                                  -> Windows + Speedgoat
    ├── sgact_panel.toml   devices, config of the motor_macro, modes, parameters, plots: one file for the three below
    ├── panel/        sgact_panel.py: the window that drives sgact
    ├── emul/         the PC that plays the Speedgoat: sgact_daemon.py (for the panel), speedgoat_emulator.py (streamer)
    └── simulink/     sgact on the Speedgoat: MATLAB / Simulink R2026b project
```

Who talks to whom:

```
  sgact_panel.py --CTL 6000-->   sgact (the Speedgoat, or sgact_daemon.py)  --commands 5001-->  amc.gtw (CM4)  --CAN-->  motor_cubemars, sensor_torque
                 <--STATUS 6001--                                           --rpc 5002------->         |       --ICC-->  amc.mot (CM7) -> motor_macro
                                                                            <--state 5003-----         |       --SPI-->  sensor_position
  tether.py      --rpc 6666 / stream 7777 (diagnostics, any time)------------------------------------->
```

| file in `shared/` | what it is | used by |
|---|---|---|
| `tethercodec.py` | TETHER frames and ROPs (protocol 5) | tether.py, amc_gtw.py, cubetether.py |
| `amc_gtw.py` | the frames of amc.gtw: commands (MIT, target of the motor_macro), rpc (with argument), state (w/ the position sensor) | sgact_daemon.py, speedgoat_emulator.py, cubetether.py, the tests, make_golden.py |
| `sgctl.py` | the link panel <-> sgact, version 4: CTL (100 bytes, flags bit 0 EXTFAULT), STATUS (104 bytes), the Client of the panel | sgact_panel.py, sgact_daemon.py, the tests, make_golden.py |
| `sgact_spec.py` | the reader of `speedgoat/sgact_panel.toml` | sgact_panel.py, sgact_daemon.py, speedgoat_emulator.py (`--mm`), gen_sgact_spec.py |

Install (once): Python 3.11 or later, then `py -m pip install -r requirements.txt` (Windows) or
`~/.venv/bin/pip install -r requirements.txt` (WSL2). tkinter comes with the Python of python.org.

Typical session (board 10.0.1.99, emulators on the Vector wired to the CAN of the board), from this folder:
1. Windows: `py emul\cubemars\cubemars_emulator.py ...` and `py emul\paxini\px3q_emulator.py ...` (see the README of each folder)
2. power the board
3. the Speedgoat runs sgact (`speedgoat/simulink/README.md`), or the PC plays it: `py speedgoat\emul\sgact_daemon.py`
4. Windows: `py speedgoat\panel\sgact_panel.py --host 10.0.2.150` (the Speedgoat) or `py speedgoat\panel\sgact_panel.py` (the daemon)
5. WSL2, any time: `tether.py rpc / str` (see `tether/README.md`)

Ports of the board: 5001 commands, 5002 rpc, 5003 state (sent to the IP that sends the commands); TETHER 6666 rpc, 7777 stream.
Windows firewall: allow Python to receive UDP on 5003 (daemon, streamer), 6000 (daemon) and 6001 (panel).

Tests w/out hardware:

    py speedgoat\emul\test_sgact_supervisor.py      # the supervisor of the daemon; writes speedgoat/simulink/tests/parity_trace.txt.gz
    py speedgoat\emul\test_daemon_loopback.py       # daemon + panel client + a fake amc.gtw over UDP on 127.0.0.1

and, in MATLAB or Octave, the tests of `speedgoat/simulink/tests`. The firmware of amc.gtw has its own tests on a PC: `../test/host`.

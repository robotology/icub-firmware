# Quick startup

All the commands are run from this folder (`host`), on Windows. Install once: `py -m pip install -r requirements.txt`.

## With the Speedgoat

One window each, in this order:

```
1. py emul\cubemars\cubemars_emulator.py --interface vector --channel 0 --app-name none --publish 127.0.0.1:5005
2. py emul\cubemars\cubemars_viewer.py --port 5005
3. py emul\paxini\px3q_emulator.py --interface vector --channel 0 --app-name none --amplitude 3 3 3 --freq 1 2 3
4. power the amcmj1 board
5. MATLAB, in speedgoat\simulink:  tg = sgact_deploy;     (already loaded: tg.start)
6. py speedgoat\panel\sgact_panel.py --host 10.0.2.150
```

Steps 1, 2 and 3 only w/out the real CubeMars or PX3Q.

In the panel: tick the devices, **INIT**, choose a mode and its parameters, **START** ... **STOP**. **EXTFAULT** stops
all the motors at once (press again to release). **DEINIT** before switching off.

## W/out the Speedgoat (the PC plays it)

As above, but instead of steps 5 and 6:

```
5. py speedgoat\emul\sgact_daemon.py
6. py speedgoat\panel\sgact_panel.py
```

The daemon runs the modes w/ `impl = "matlab"` only, one command frame every 10 ms (`--period-ms`).

## Diagnostics of amc.gtw (WSL2, any time)

```
tether.py rpc      # variables and operations of the board
tether.py str      # stream of variables
```

See `tether/README.md`.

## Addresses and ports

| who | address | ports |
|---|---|---|
| amcmj1 (amc.gtw) | 10.0.1.99 | 5001 commands, 5002 rpc, 5003 state; TETHER 6666 rpc, 7777 stream |
| Speedgoat | ETH1 10.0.1.150 (to the board), Host Link 10.0.2.150 (to the PC) | 6000 CTL from the panel |
| PC (panel) | 10.0.2.104 | 6001 STATUS from sgact |

Windows firewall: allow Python to receive UDP on 5003, 6000 and 6001.

## The tests (no hardware)

```
py speedgoat\emul\test_sgact_supervisor.py
py speedgoat\emul\test_daemon_loopback.py
```

and in MATLAB the four tests of `speedgoat\simulink` (see `speedgoat\simulink\howto.generate.md`).

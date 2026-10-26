# paxini: PaXini PX3Q emulator

px3q_emulator.py answers on the CAN bus like the PX3Q torque sensor (standard ids) and streams Mx My Mz once the board starts it
(init + start from the rpc of tether.py). Run it on Windows, from this folder:

    py px3q_emulator.py --interface vector --channel 0 --app-name none --amplitude 3 3 3 --freq 1 2 3 -v

- `--amplitude MX MY MZ` (three values) and `--freq FX FY FZ` [Hz]: the signal of each axis. `--noise` adds gaussian noise.
- `-v` prints the commands received and the replies. `--sniff` prints every frame of the bus (heavy).
- `--drop` / `--nak` / `--delay-ms`: make some commands fail or wait, to test the board.
- `--channel` / `--app-name` depend on your Vector Hardware Config; `--interface virtual` for a bench without hardware.

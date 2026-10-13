# PX3Q emulator: quick start (Windows + Vector VN1610)

`px3q_emulator.py` emulates the PaXini PX3Q torque sensor on a CAN bus. It answers the commands of the
protocol handler (stop, getVersion, setRate, start, readOnce, ...) and streams the torque frames (A and B).

## 1. Install

1. **Python 3.9+** (python.org). Check with `py --version` (the Windows launcher).
2. **python-can**:
   ```
   py -m pip install python-can
   ```
3. **Vector driver**: install *Vector Driver Setup* from the Vector website (it provides `vxlapi64.dll` and
   *Vector Hardware Config*). Plug the VN1610 in USB.
4. In *Vector Hardware Config* check that the VN1610 shows up. No application assignment is needed with
   `--app-name none`: `--channel` is then the global channel index (`0` = the first Vector channel, i.e. the VN1610).

## 2. Run

With the VN1610 wired to the CAN bus of the board (CAN_H, CAN_L, GND, 120 ohm at both ends, 1 Mbps):

```
py px3q_emulator.py --interface vector --channel 0 --app-name none --verbose
```

| option | meaning |
|---|---|
| `--interface vector` | python-can interface (default) |
| `--channel 0` | with `--app-name none`: global channel index (0 = first Vector channel) |
| `--app-name none` | do not use a Vector application: `--channel` is the global index. Or a name such as `CANalyzer`, to use the channel assigned to it in Vector Hardware Config |
| `--id 0x7F` | device id, 1..127. If missing, or 0, the default 0x7F is used |
| `--bitrate 1000000` | default 1 Mbps |
| `--amplitude`, `--freq`, `--noise` | shape of the emulated Mx, My, Mz |
| `--drop`, `--nak`, `--delay-ms` | fault injection: no reply, negative reply, slow reply |
| `-v`, `--verbose` | print the commands received and the replies (not the stream) |
| `--trace-stream N` | one line every N stream samples; 0 = never (default) |
| `--sniff` | print every frame on the bus (heavy while streaming) |

To list the channels: `py -c "import can; print(can.detect_available_configs('vector'))"`.
With an application name instead (`--app-name CANalyzer`), the channel is the one assigned to that application in Vector Hardware Config.

Quit with `Ctrl+C`. Nothing is printed while it runs, except the start banner, unless `--verbose` is used.

## 3. Check

- Start the board, which runs the init sequence: **stop, getVersion, setRate, start**.
- With `--verbose` you see each command and its reply. Then the emulator streams frames A (`0x280 + id`) and B
  (`0x380 + id`) at the rate set by setRate.
- Without hardware: `py test_emulator.py` runs a self-test on python-can's virtual bus (in one process).

## Notes

- The Vector channel can be used by more than one application only if the Vector configuration allows it:
  close CANalyzer/CANoe, or give each program its own channel.
- Bitrate and termination must be correct on the bus: at least two nodes (the board and the Vector) are needed
  to get the ACK.

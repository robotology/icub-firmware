# cubemars: CubeMars AK70-9 emulator, viewer, panel

Run on Windows, from this folder.

    py cubemars_emulator.py --interface vector --channel 0 --app-name none --publish 127.0.0.1:5005     # the motor (CAN)
    py cubemars_viewer.py --port 5005                                                                   # plots of the motor state
    py cubemars_panel.py --backend tether --board 10.0.1.99                                             # command panel through the board

- cubemars_emulator.py: servo + MIT modes, extended ids `(packet<<8)|0x68`. `--inertia`, `--friction`, `--feedback-hz`, `-v`, `--sniff`.
  The position is saturated at +-3200 deg by the 0x29 frame: restart the emulator before long position tests.
- cubemars_viewer.py: reads the JSON state that the emulator publishes on UDP 5005. `--text` for a console line per second.
- cubemars_panel.py: ENABLE/DISABLE buttons and the MIT/servo commands. `--backend tether` goes through the amcmj1 (commands on 5001,
  rpc on 5002); `--backend can` is a CAN master on a second Vector channel, without the board.
  The panel and the Speedgoat side (the Speedgoat, `speedgoat/emul/sgact_daemon.py` or `speedgoat_emulator.py`) all send the
  commands on 5001: use one at a time.
- cubetether.py: the link of the panel to the board.

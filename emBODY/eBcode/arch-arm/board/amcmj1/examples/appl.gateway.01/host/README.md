# host: PC tools of the amcmj1 gateway

    tether/            generic TETHER client (tether.py) + codec + dictionary          -> WSL2
    emul/paxini/       PaXini PX3Q emulator (CAN, Vector)                              -> Windows
    emul/cubemars/     CubeMars AK70-9 emulator (CAN, Vector) + viewer + command panel -> Windows
    emul/speedgoat/    Speedgoat emulator (streams MIT, --daemon) + Tk panel to choose the test -> Windows
    emul/common/       cubegateway.py: the frames of the gateway (shared by the tools above)
    emul/tests/        self-tests of the tools (no hardware): python emul/tests/test_*.py

Typical session (board 10.0.1.99, emulators on the Vector wired to the CAN of the board):
1. Windows: `py emul\cubemars\cubemars_emulator.py ...` and `py emul\paxini\px3q_emulator.py ...` (see the README of each folder)
2. power the board
3. WSL2: tether rpc / str (see tether/README.md)
4. Windows: `py emul\speedgoat\speedgoat_emulator.py --test`, or with the mouse: `--daemon` and, in another window, `py emul\speedgoat\speedgoat_panel.py`

Board ports: 5001 commands, 5002 rpc, 5003 state (sent to the PC that sends the commands); tether.py: 6666 rpc, 7777 stream.
Windows firewall: allow Python to receive UDP on 5003.

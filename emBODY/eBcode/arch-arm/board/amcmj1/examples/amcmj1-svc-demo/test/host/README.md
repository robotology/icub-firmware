# test/host: TETHER on a PC

The code of TETHER of the board (`embot/svc`: ROP, SharedVariables, theRPCmanager, theSTRmanager) w/ the Dictionary of
this example, compiled on a PC w/ small stubs of the embot headers (`stub/`: no RTOS, no sockets, no hardware):

- `test_rop.cpp`: the wire format of protocol 6, `rop::Head`, `rop::Writer` and `rop::Reader`, w/ valid, malformed, version 5
  and 200000 random frames;
- `test_tether.cpp`: theRPCmanager and theSTRmanager w/ the Dictionary of amcmj1-svc-demo: the browse, the big values (60,
  288 and 400 bytes), the light ack, the room of the reply (nak "no room"), the malformed frames, the budget of the stream;
- `tether_pcboard.cpp`: a board on the PC, the same code on two UDP sockets of linux;
- `test_host.py`: the host tools (`tethercodec.py`, `tether.py`) against `tether_pcboard`, end to end.

On Ubuntu 24.04 / 26.04 (WSL2 or native), once: `sudo apt install g++ python3` (g++ 13 on 24.04, 15 on 26.04; g++ 11
or later is enough). On Windows: in WSL2, or w/ the g++ of MSYS2 (the sanitizers `-fsanitize=...` are not there: leave
them out). From the folder `amcmj1-svc-demo`:

```bash
E=../../../../embot/svc              # emBODY/eBcode/arch-arm/embot/svc
I=../../../../embot/instr            # emBODY/eBcode/arch-arm/embot/instr
SRC="src/dictionary.udpdemo.cpp $E/embot_svc_tether_SharedVariables.cpp $E/embot_svc_tether_theRPCmanager.cpp $E/embot_svc_tether_theSTRmanager.cpp $I/embot_instr.cpp"
g++ -std=c++20 -Wall -Wextra -fsanitize=address,undefined -Itest/host/stub -I$E test/host/test_rop.cpp -o test_rop && ./test_rop
g++ -std=c++20 -Wall -Wextra -fsanitize=address -Itest/host/stub -Isrc -I$E -I$I test/host/test_tether.cpp $SRC -o test_tether && ./test_tether
g++ -std=c++20 -Wall -Wextra -O1 -Itest/host/stub -Isrc -I$E -I$I test/host/tether_pcboard.cpp $SRC -o tether_pcboard
python3 test/host/test_host.py ./tether_pcboard
```

Expected: `40/40 ROP checks passed`, `44/44 TETHER checks passed`, `25/25 host checks passed`.

The Dictionary is compiled w/out `-fsanitize=undefined`: its `TETHER_VALIDATE()` compares function pointers in a constant
expression, which UBSan does not allow.

`tether_pcboard` can also be used by hand, e.g. to try the pages of `embot-tether`:

```bash
./tether_pcboard 16666 17777 7777                       # rpc on 16666, stream from 17777 to the host port 7777
embot-tether rpc --ip 127.0.0.1 --port 16666
embot-tether str --ip 127.0.0.1 --boardport 17777
```

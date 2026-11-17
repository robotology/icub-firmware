# test/host: TETHER on a PC

The code of TETHER of the board (`embot/svc`: ROP, SharedVariables, theRPCmanager, theSTRmanager) w/ the Dictionary of
this example, compiled on a PC w/ small stubs of the embot headers (`stub/`: no RTOS, no sockets, no hardware):

- `test_rop.cpp`: the wire format of protocol 6, `rop::Head`, `rop::Writer` and `rop::Reader`, w/ valid, malformed, version 5
  and 200000 random frames;
- `test_tether.cpp`: theRPCmanager and theSTRmanager w/ the Dictionary of udpdemo-tether: the browse, the big values (60,
  268 and 400 bytes), the light ack, the room of the reply (nak "no room"), the malformed frames, the budget of the stream;
- `tether_pcboard.cpp`: a board on the PC, the same code on two UDP sockets of linux;
- `test_host.py`: the host tools (`tethercodec.py`, `tether.py`) against `tether_pcboard`, end to end.

From the folder `udpdemo-tether` (Linux, WSL or MSYS2, g++ >= 11, python >= 3.11):

```bash
E=../../../../embot/svc              # emBODY/eBcode/arch-arm/embot/svc
SRC="src/dictionary.udpdemo.cpp $E/embot_svc_tether_SharedVariables.cpp $E/embot_svc_tether_theRPCmanager.cpp $E/embot_svc_tether_theSTRmanager.cpp"
g++ -std=c++20 -Wall -Wextra -fsanitize=address,undefined -Itest/host/stub -I$E test/host/test_rop.cpp -o test_rop && ./test_rop
g++ -std=c++20 -Wall -Wextra -fsanitize=address -Itest/host/stub -Isrc -I$E test/host/test_tether.cpp $SRC -o test_tether && ./test_tether
g++ -std=c++20 -Wall -Wextra -O1 -Itest/host/stub -Isrc -I$E test/host/tether_pcboard.cpp $SRC -o tether_pcboard
python3 test/host/test_host.py ./tether_pcboard
```

Expected: `40/40 ROP checks passed`, `44/44 TETHER checks passed`, `25/25 host checks passed`.

The Dictionary is compiled w/out `-fsanitize=undefined`: its `TETHER_VALIDATE()` compares function pointers in a constant
expression, which UBSan does not allow.

`tether_pcboard` can also be used by hand, e.g. to try the pages of `tether.py`:

```bash
./tether_pcboard 16666 17777 7777                       # rpc on 16666, stream from 17777 to the host port 7777
tether rpc --ip 127.0.0.1 --port 16666
tether str --ip 127.0.0.1 --boardport 17777
```

# test/host: the rules of amc.gtw on a PC

The parts of `appl.gateway.01` that have no hardware are compiled and tested on a PC, with small stubs of the embot headers
(`stub/`):
- `amcmj1_gwlink.cpp`: the commands, rpc and state frames of the Speedgoat link;
- `amcmj1_motdriver.cpp`: the macromotor (pids, setup, control mode, setpoints, watchdog, emulation of amc.mot);
- `amcmj1_posdriver.cpp`: the position sensor, in its emulated version (the state frame carries it in its sixth ROP);
- `dictionary.amcmj1.cpp`: only compiled, so that `TETHER_VALIDATE()` checks the Dictionary (IDs, names, sizes).

On Ubuntu 24.04 / 26.04 (WSL2 or native), once: `sudo apt install g++ python3` (g++ 13 on 24.04, 15 on 26.04; g++ 11
or later is enough). On Windows: in WSL2, or w/ the g++ of MSYS2 (the sanitizers `-fsanitize=...` are not there: leave
them out). From the folder `appl.gateway.01`:

```bash
E=../../../../embot/svc              # emBODY/eBcode/arch-arm/embot/svc
g++ -std=c++20 -Wall -Wextra -Itest/host/stub -Isrc -I$E test/host/test_gwlink.cpp src/amcmj1_gwlink.cpp src/amcmj1_motdriver.cpp src/amcmj1_posdriver.cpp -o test_gwlink && ./test_gwlink
g++ -std=c++20 -fsyntax-only -Wall -Itest/host/stub -Isrc -I$E src/dictionary.amcmj1.cpp
```

Expected: `29/29 gateway checks passed`, and no error from the Dictionary.

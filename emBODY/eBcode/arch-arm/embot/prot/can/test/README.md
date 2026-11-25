# Host tests of embot::prot::can

They run on a PC (Linux or WSL, g++ >= 9) and do not need any board.

- `compat.cpp` uses only the legacy API (`load()` / `reply()` / `get()`), which is the one available in robotology/icub-firmware devel,
  to exercise every message of every class. It is built against devel and against the code in `..`, and the two outputs are compared:
  the only differences must be those listed in `expected_compat.diff`.
- `roundtrip.cpp` uses the `Sender` / `Receiver` API of motor polling and motor periodic: for every message it forms a frame from a value,
  parses it back and checks that it gets the same value (request and reply for the GET messages). It also checks that the new API
  produces the same frames as the legacy one.



## How to run

    export ICUB_FW_DEVEL=~/rob/icub-firmware
    export ICUB_FWSH_DEVEL=~/rob/icub-firmware-shared
    export ICUB_FWSH_PR=$ICUB_FWSH_DEVEL
    ./run_tests.sh "$ICUB_FW_DEVEL" "$ICUB_FWSH_DEVEL" "$ICUB_FWSH_PR" c++17 [c++17|c++20]

The executables and the outputs go in `out/`
`stub/embot_hw.h` is an empty header, needed only to build the sources of devel on the PC.

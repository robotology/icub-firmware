# udpdemo-tether

The UDP echo of `udpdemo-embot-net` plus TETHER, on the CM4 of the amcmj1 (boot master). The CM7 will follow.

```
udpdemo-tether/
├── src/
│   ├── udpdemo-tether.cpp       init, theIPservice, theTETHERservice, the echo thread (tECHO)
│   ├── dictionary.udpdemo.h     the catalog of the Dictionary and the typed handles of its variables
│   ├── dictionary.udpdemo.cpp   the variables and the operations: a didactic Dictionary, one example per case
│   ├── udpdemo_types.h          the types of the variables (structs, enum)
│   └── udpdemo_echo.h           the events of the echo thread and echo::signal(), used by the operations
├── cfg/                         BSP config, C startup, scatter file (AC6) and linker script (ATfE)
├── proj/
│   ├── amcmj1.udpdemo.tether.uvprojx / .uvoptx     uVision: target amcmj1.2cm4-master, ULINKpro w/ trace
│   └── amcmj1.udpdemo.tether.csol/                 VS Code: AC6 and ATfE, ST-Link
├── docs/                        the guides: a native variable of the Dictionary, a view of a variable of the application
├── host/                        dictionary.toml of the board, for tether (embot/svc/host/tether, see host/README.md)
└── test/host/                   TETHER on a PC: the wire format, the managers w/ this Dictionary, and a board on the PC
                                 for the host tools (see test/host/README.md)
```

## Guides

- [docs/howto-add-a-native-variable.md](docs/howto-add-a-native-variable.md): a variable that lives in the Dictionary,
  shared by several threads and the host w/ the protection of `SharedVariables` (e.g. `IDechoCONFIG`, `IDechoSTATS`)
- [docs/howto-add-a-view.md](docs/howto-add-a-view.md): a view of a variable that already exists in the application, a
  copy taken on read: a scalar (`IDviewof_wave`, w/ `tetherdebugENABLE_wave`) and a struct protected by a mutex
  (`IDviewof_pwm`, w/ `tetherdebugENABLE_pwm`)

## What it does

| Port | Owner | What |
|------|-------|------|
| 666  | tECHO | replies to each frame w/ the same frame, first byte replaced by a marker (`-` by default) |
| 6666 | TETHER rpc | ask / set / run on the Dictionary, browse, version, stats |
| 7777 | TETHER stream | the variables chosen by the host, sent periodically to the host |
| 9999 | tLOGGER | the lines of `embot::core::print()` to the host, and the text commands that control them (`embot::svc::theLogger`) |

The echo thread shares its data w/ the host through the Dictionary:

- `IDechoCONFIG` (RW): `{marker, enabled, uppercase}`, set by the host, read by tECHO at each frame
- `IDechoSTATS` (RO): rx / tx frames, rx bytes, errors, written by tECHO; `run<OPechoRESET>` clears them
- `IDechoLAST` (RO): IP, port, size and time of the last frame
- `IDticks` (RW): incremented by tECHO every 5 ms, and the host can set it too
- `run<OPechoHELLO>`: the operation (in the rpc thread of TETHER) posts an event to tECHO, which sends `hello from amcmj1`
  to the last sender on port 666

The other variables and operations show the remaining cases of TETHER (scalar, float, array, enum, bool, struct computed
on read, a sine to plot w/ the stream; operations w/ every combination of argument and result). The full catalog is at the
top of `src/dictionary.udpdemo.h`.

The big values (protocol 6 of TETHER: a variable or an argument carries up to 512 bytes, the result of an operation up to
36 bytes, in its ack):

- `IDskinPATCH` (RO): a skin patch of 60 taxels, `uint8_t[60]`, computed on read
- `IDhistogram` (RO): a struct of 268 bytes w/ 128 bins of `uint16_t`, computed on read
- `IDtable` (RW): `float[100]`, 400 bytes, set by the host
- `run<OPloadTable, [100 floats]>`: an argument of 400 bytes; it writes `IDtable` and gives back the sum (4 bytes)

The three of them fit one frame of the stream (16 + 404 + 272 + 64 = 756 bytes of the 1472 of a frame).

## Build

The libraries are not in git: build them before the project. uVision uses the libraries made by the uVision projects
of the libraries (`libs/.../proj/*.uvprojx`), VS Code the ones made by their csolutions (`libs/.../proj/*.csol`, e.g. w/
`build-all.cmd`). All of them go in `lib/` of each library, w/ different names:

| Library | uVision (AC6) | VS Code, Debug (AC6) | VS Code, Debug-atfe (ATfE) |
|---------|---------------|----------------------|----------------------------|
| stm32hal | `stm32hal.h7.amcmj1.2cm4.v1B6.lib` | `stm32hal.csol.h7.amcmj1.2cm4.v1B6.lib` | `stm32hal.csol.atfe.h7.amcmj1.2cm4.v1B6.a` |
| cmsisrtx | `cmsisrtx.debug.cm4.lib` | `cmsisrtx.csol.debug.cm4.lib` | `cmsisrtx.csol.atfe.debug.cm4.a` |
| lwip | `lwip.8udp.tx8k.cm4.lib` (target `lwip-cm4-8udp-tx8k`) | `lwip.csol.8udp.tx8k.cm4.lib` | `lwip.csol.atfe.8udp.tx8k.cm4.a` |

- uVision: open `proj/amcmj1.udpdemo.tether.uvprojx`, target `amcmj1.2cm4-master`, build, load and debug w/ the ULINKpro.
  C++17 (uVision 5.43 offers no C++20).
- VS Code: open the folder `proj/amcmj1.udpdemo.tether.csol`, in Manage Solution choose the target-set
  - `stlink` for AC6 (build-type `Debug`, `.axf`), debug w/ `CM4 STLink@pyOCD AC6 (launch)`
  - `stlink-atfe` for ATfE (build-type `Debug-atfe`, `.elf`), debug w/ `CM4 STLink@pyOCD ATfE (launch)`
  C++20 in both.

## Try it

Board 10.0.1.99, host 10.0.1.104.

The echo:

    echo -n "hello" | nc -u -w1 10.0.1.99 666       # -> "-ello"

The log (the lines of `embot::core::print()`, also on ITM as before). Talk to the board from netcat and press Enter: the
board sends the log to that terminal (it works also from WSL2 w/out mirrored networking):

    ncat -u 10.0.1.99 9999                          # Windows (ncat comes w/ nmap). Linux / WSL2: nc -u 10.0.1.99 9999
    <Enter>                                         # [theLogger] ok: dest 10.0.1.104:54321, on, deco on
    deco off                                        # [theLogger] ok: deco off
    off                                             # [theLogger] ok: off          (the last line until on)
    on                                              # [theLogger] ok: on
    status                                          # [theLogger] status: dest 10.0.1.104:54321, on, deco off, commands on, lines 57, frames 21, dropped 0
    dest 10.0.1.104:9999                            # the log to another ip:port, e.g. a listener:  ncat -ul 9999
    help                                            # the list of the commands, one line each: [theLogger] help: off: ...

At start the log goes to 10.0.1.104:9999, so a listener there receives it w/out any command. The reply to a command goes
to who sent it, also when the log is off; it begins w/ `[theLogger] `, which a print() never does.

TETHER, w/ `tether`, from `host/` (it reads the Dictionary from the board w/ browse and keeps it in
`host/dictionary.toml`). From the folder `host/`:

    tether rpc --ip 10.0.1.99 --cli
    tether str --ip 10.0.1.99

Some commands of the rpc prompt:

    ask<IDechoSTATS>
    set[+ack]<IDechoCONFIG, {marker=42, enabled=true, uppercase=true}>     # then the echo of "hello" is "*ELLO"
    run[+ack]<OPechoHELLO>
    run[+ack]<OPdivide, {a=17, b=0}>                                       # -> nak "division by zero"
    set[+ack]<IDgain, 2.5>                                                 # then stream IDsignal and plot it
    ask<IDhistogram>                                                       # 268 bytes: {samples=.., lo=-1, hi=1, bins=[..128 values..]}
    set[+ack]<IDtable, [1, 2, 3, ... 100 values]>; ask<IDtable>            # a light ack, then the 400 bytes back
    ask<IDtable>; ask<IDtable>; ask<IDtable>; ask<IDtable>                 # 4 x 404 bytes of replies: tether.py sends two frames
    set[+ack]<MGstreamADD, IDhistogram>; set[+ack]<MGstreamADD, IDskinPATCH>; set[+ack]<MGstreamSTART, 20>

The state of the log, seen w/ TETHER (it is configured only w/ its commands, above):

    ask<IDlogSTATUS>                                                     # destination, ON / OFF, decoration, counters

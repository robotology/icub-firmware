# amcmj1-svc-demo

The demo of the services of embot on the CM4 of the amcmj1 (boot master): TETHER w/ a didactic Dictionary, theLogger,
theReporter and theLEDmanager, around the UDP echo of `udpdemo-embot-net`. The CM7 will follow.

The services of embot (TETHER, theLogger, theReporter, theLEDmanager) and where to start: `embot/svc/README.md`.

```
amcmj1-svc-demo/
├── src/
│   ├── svc-demo.cpp             init, theReporter, theIPservice, theTETHERservice, tECHO, tALIVE, HardFault
│   ├── dictionary.udpdemo.h     the catalog of the Dictionary and the typed handles of its variables
│   ├── dictionary.udpdemo.cpp   the variables and the operations: didactic Dictionary, one example per case
│   ├── udpdemo_types.h          the types of the variables (structs, enum)
│   └── udpdemo_echo.h           the events of the echo thread and echo::signal(), used by the operations
├── cfg/                         BSP config, C startup, scatter file (AC6) and linker script (ATfE), w/ RAM that survives a reset
├── proj/
│   ├── amcmj1.svc.demo.uvprojx / .uvoptx    uVision: target amcmj1.2cm4-master, ULINKpro w/ trace
│   └── amcmj1.svc.demo.csol/                VS Code: AC6 and ATfE, ST-Link
├── docs/                        the guides: a variable of the Dictionary, a view of a variable of the appl
├── host/                        TETHER dictionary.toml of the board, see embot/svc/host/README.md
├── test/host/                   test TETHER on a PC
└── quick.startup.md             only the commands, to try it on Windows or Ubuntu
```

## Guides

- [docs/howto-add-a-native-variable.md](docs/howto-add-a-native-variable.md): a variable that lives in the Dictionary,
  shared by several threads and the host w/ the protection of `SharedVariables` (e.g. `IDechoCONFIG`, `IDechoSTATS`)
- [docs/howto-add-a-view.md](docs/howto-add-a-view.md): a view of a variable that already exists in the application, a
  copy taken on read: a scalar (`IDviewof_wave`, w/ `tetherdebugENABLE_wave`), a struct protected by a mutex
  (`IDviewof_pwm`, w/ `tetherdebugENABLE_pwm`) and an object that cannot be copied byte by byte, through its snapshot
  (`IDviewof_latency`)

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
- `IDviewof_latency` (RO): a struct of 288 bytes, `embot::instr::HistogramSnapshot<128>`: the latency of the echo, from
  the frame taken from the socket to its reply given to the socket, in 128 bins of 2 us (`lo` 0, `hi` 256), w/ `total`,
  `below`, `beyond`, `min`, `max`, `mean`. The echo thread fills an `embot::instr::ScopeHisto`, and on read the Dictionary
  copies it in the snapshot (`echo::readlatency()`, under a mutex). It fills while the echo works (see Try it)
- `IDtable` (RW): `float[100]`, 400 bytes, set by the host
- `run<OPloadTable, [100 floats]>`: an argument of 400 bytes; it writes `IDtable` and gives back the sum (4 bytes)

The three of them fit one frame of the stream (16 + 404 + 292 + 64 = 776 bytes of the 1472 of a frame).

The reports of the board go through `embot::svc::theReporter` (see `embot/svc/doc/theReporter.md`): they are printed, so
they are on ITM and on the log (port 9999). The alive thread (tALIVE) reports w/ severity info every 5 s. The record of a
fatal is kept in 256 bytes of RAM that survive a reset (`udpdemo::report::fatalmemory`, section `.bss.noinit`), placed by
the region `RW_NOINIT` (UNINIT) of the scatter file for AC6 and by the section `.noinit` (NOLOAD) of the linker script for
ATfE. A HardFault goes there too: `HardFault_Handler` in `src/svc-demo.cpp` gives the registers to theReporter.

## Build

The libraries are not in git: build them before the project. uVision uses the libraries made by the uVision projects
of the libraries (`libs/.../proj/*.uvprojx`), VS Code the ones made by their csolutions (`libs/.../proj/*.csol`, e.g. w/
`build-all.cmd`). All of them go in `lib/` of each library, w/ different names:

| Library | uVision (AC6) | VS Code, Debug (AC6) | VS Code, Debug-atfe (ATfE) |
|---------|---------------|----------------------|----------------------------|
| stm32hal | `stm32hal.h7.amcmj1.2cm4.v1B6.lib` | `stm32hal.csol.h7.amcmj1.2cm4.v1B6.lib` | `stm32hal.csol.atfe.h7.amcmj1.2cm4.v1B6.a` |
| cmsisrtx | `cmsisrtx.debug.cm4.lib` | `cmsisrtx.csol.debug.cm4.lib` | `cmsisrtx.csol.atfe.debug.cm4.a` |
| lwip | `lwip.8udp.tx8k.cm4.lib` (target `lwip-cm4-8udp-tx8k`) | `lwip.csol.8udp.tx8k.cm4.lib` | `lwip.csol.atfe.8udp.tx8k.cm4.a` |

- uVision: open `proj/amcmj1.svc.demo.uvprojx`, target `amcmj1.2cm4-master`, build, load and debug w/ the ULINKpro.
  C++17 (uVision 5.43 offers no C++20).
- VS Code: open the folder `proj/amcmj1.svc.demo.csol`, in Manage Solution choose the target-set
  - `stlink` for AC6 (build-type `Debug`, `.axf`), debug w/ `CM4 STLink@pyOCD AC6 (launch)`
  - `stlink-atfe` for ATfE (build-type `Debug-atfe`, `.elf`), debug w/ `CM4 STLink@pyOCD ATfE (launch)`
  C++20 in both.

## Try it

Board 10.0.1.99, PC 10.0.1.104. Only the commands: [quick.startup.md](quick.startup.md). The host tools (`embot-tether`,
`embot-logger`) are installed once as in `embot/svc/host/README.md`, for Windows and for Ubuntu.

### The echo (port 666)

Windows, PowerShell (or `ncat -u 10.0.1.99 666` w/ the ncat of nmap, then type lines):

```powershell
$u = New-Object System.Net.Sockets.UdpClient; $u.Client.ReceiveTimeout = 1000
[void]$u.Send([Text.Encoding]::ASCII.GetBytes("hello"), 5, "10.0.1.99", 666)
$ep = New-Object System.Net.IPEndPoint([Net.IPAddress]::Any, 0)
[Text.Encoding]::ASCII.GetString($u.Receive([ref]$ep))      # -> -ello
```

Ubuntu (WSL2 or native):

```bash
echo -n "hello" | nc -u -w1 10.0.1.99 666       # -> -ello
nc -u 10.0.1.99 666                             # then type lines: ccc -> -cc. ctrl-c to stop
```

A continuous traffic, to fill the latency of the echo (`IDviewof_latency`). ctrl-c to stop:

```powershell
$u = New-Object System.Net.Sockets.UdpClient; $b = [byte[]](0x78)
while ($true) { [void]$u.Send($b, 1, "10.0.1.99", 666); Start-Sleep -Milliseconds 10 }
```

```bash
while true; do echo -n x | nc -u -w0 10.0.1.99 666; sleep 0.01; done
```

### The log (port 9999)

The lines of `embot::core::print()`, also on ITM as before. W/ the viewer (the same on both systems; `--no-open` in
WSL2):

    embot-logger --ip 10.0.1.99

Or by hand: talk to the board w/ netcat and press Enter, and the board sends the log to that terminal (it works also
from WSL2 w/out mirrored networking). Windows: `ncat -u 10.0.1.99 9999` (ncat comes w/ nmap). Ubuntu:
`nc -u 10.0.1.99 9999`. Then:

    <Enter>                                         # [theLogger] ok: dest 10.0.1.104:54321, on, deco on
    deco off                                        # [theLogger] ok: deco off
    off                                             # [theLogger] ok: off          (the last line until on)
    on                                              # [theLogger] ok: on
    status                                          # [theLogger] status: dest 10.0.1.104:54321, on, deco off, commands on, lines 57, frames 21, dropped 0
    dest 10.0.1.104:9999                            # the log to another ip:port, e.g. a listener: ncat -ul 9999 / nc -ul 9999
    help                                            # the list of the commands, one line each: [theLogger] help: off: ...

At start the log goes to 10.0.1.104:9999, so a listener there receives it w/out any command. The reply to a command goes
to who sent it, also when the log is off; it begins w/ `[theLogger] `, which a print() never does.

### theReporter: a report every 5 s, a HardFault on demand

In the log (port 9999, or ITM):

    [info] S5:m0:u120 (tALIVE, udpdemo) code 0x00010001 dev 0 addr 0 par16 0x0000 par64 0x0000000000000001: alive #1

To try the fatal and the reset: in `src/svc-demo.cpp` set `udpdemo::report::enableFAULT` to `true`, build and load.
At the 3rd report (15 s) tALIVE executes an undefined instruction: the HardFault handler gives the registers to
theReporter, which writes the record, switches on the three LEDs (its safe state) and resets the board. After the reset the
first report is the fault:

    [fatal] previous run S15:m100:u250 (isr 3, -) code 0x00000000 ... pc 0x081xxxxx ret 0x081xxxxx status 0x... cause 0x00010000 detail 0x40000000 addr 0x00000000: fault

`isr 3` is the HardFault (exception 3), `cause` is the CFSR (0x00010000: UNDEFINSTR, an undefined instruction), `detail` the HFSR (0x40000000: FORCED, escalated
to HardFault), `pc` the address of the instruction: uVision shows it in the Disassembly window (or `fromelf --text -c` on
the `.axf`; ATfE: `llvm-objdump -d` on the `.elf`). tALIVE does not fault again until the next power-on, as the board
has the record of the previous fatal. To stop at the fault w/ the debugger instead: `theReporter::OnFatal::hang` in
`udpdemo::report::config`.

### TETHER (ports 6666, 7777)

From the folder `host/` of this example, w/ the venv active (`embot-tether rpc` reads the Dictionary from the board w/ browse
and keeps it in `host/dictionary.toml`). The same commands on both systems; in WSL2 add `--no-open` to the pages:

    embot-tether rpc --ip 10.0.1.99 --cli          # the prompt
    embot-tether rpc --ip 10.0.1.99                # the page, http://localhost:8080
    embot-tether str --ip 10.0.1.99                # the page of the stream, http://localhost:8081 (another window)

Some commands of the rpc prompt:

    ask<IDechoSTATS>
    set[+ack]<IDechoCONFIG, {marker=42, enabled=true, uppercase=true}>     # then the echo of "hello" is "*ELLO"
    run[+ack]<OPechoHELLO>
    run[+ack]<OPdivide, {a=17, b=0}>                                       # -> nak "division by zero"
    set[+ack]<IDgain, 2.5>                                                 # then stream IDsignal and plot it
    ask<IDviewof_latency>                                                  # 288 bytes: {total=.., mean=.., lo=0, hi=256, bins=[..128..]} (us)
    set[+ack]<IDtable, [1, 2, 3, ... 100 values]>; ask<IDtable>            # a light ack, then the 400 bytes back
    ask<IDtable>; ask<IDtable>; ask<IDtable>; ask<IDtable>                 # 4 x 404 bytes of replies: tether sends two frames
    set[+ack]<MGstreamADD, IDviewof_latency>; set[+ack]<MGstreamADD, IDskinPATCH>; set[+ack]<MGstreamSTART, 20>

The local commands of the prompt: `list` (the Dictionary), `browse` (read it again from the board), `help`, `quit`.

The state of the log, seen w/ TETHER (it is configured only w/ its commands, above):

    ask<IDlogSTATUS>                                                     # destination, ON / OFF, decoration, counters

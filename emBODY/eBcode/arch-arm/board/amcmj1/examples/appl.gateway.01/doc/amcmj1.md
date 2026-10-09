# amcmj1.appl.gateway: the TETHER example application

`amcmj1.appl.gateway.01` (`appl.gateway.01.cpp`) is the example application of **TETHER**, the diagnostic and operations service of`embot/app/eth` (`theTETHERservice`). 

It runs on the **amcmj1** board, opens the TETHER sockets, publishes a Dictionary of
variables and operations, and runs a small control thread that drives a (fake) PaXini PX3Q force / torque sensor.

It is written to be read and copied: its Dictionary contains **every shape of variable and of operation** that TETHER
supports, with the explanations in the comments of `dictionary.amcmj1.cpp`.

For the service itself, the protocol and how to write your own Dictionary, see `TETHER.md`.

## What the application does

```
┌─ HOST  (Linux or WSL) ─────────────────────────────────────────────────────────────────┐
│   tether.py rpc and tether.py str share dictionary.toml and tethercodec.py             │
│                                                                                        │
│   ┌────────────────────────┐    ┌────────────────────────┐    ┌──────────────────────┐ │
│   │ tether.py rpc          │    │ tether.py str          │    │ PlotJuggler 4        │ │
│   │ NiceGUI page  :8080    │    │ NiceGUI page  :8081    ├───►│ UDP Server  :9870    │ │
│   │ variables · operations │    │ plots · csv · pings    │    │ protocol JSON        │ │
│   └────────────────────────┘    └────────────────────────┘    └──────────────────────┘ │
│                                                                                        │
└───────────────┬─────────────────────────────┬──────────────────────────────────────────┘
                ▲                             ▲
                │ UDP :6666                   │ UDP :7777
                │ ask set sig run             │ sig frames
                │ say ack nak                 │ ping every 2 s
┌─ amcmj1 board ┴─────────────────────────────┴──────────────────────────────────────────┐
│               ▼                             ▼                                          │
│   ┌────────────────────────┐    ┌────────────────────────┐                             │
│   │ theRPCmanager          │    │ theSTRmanager          │    theIPservice (lwIP, ETH) │
│   │ rpc thread:            │    │ stream thread + timer  │    carries the two sockets  │
│   │ runs the operations    │    │ keepalive 10 s         │                             │
│   └────────────────────────┘    └────────────────────────┘                             │
│               │                             │                                          │
│               │ read / write                │ read                                     │
│               │                             │                                          │
│               ▼                             ▼                                          │
│   ┌──────────────────────────────────────────────────────┐                             │
│   │ SharedVariables (thread-safe, from the Dictionary)   │    ◄── read / write         │
│   │ lock-free if 1/2/4 B aligned, else a mutex           │        by the ctrl thread   │
│   └──────────────────────────────────────────────────────┘                             │
│   Dictionary: constexpr, in flash.   variables → SharedVariables,   operations → rpc   │
└────────────────────────────────────────────────────────────────────────────────────────┘


```

| piece | what it does |
|---|---|
| `main()` / `initSystem()` | starts the embot OS, the timer and callback managers, the LEDs, `theIPservice`, then `theTETHERservice`, then the ctrl thread |
| `theIPservice` | lwIP and the ETH hardware: board `10.0.1.99`, netmask `255.255.255.0`, gateway `10.0.1.104`, MAC `70:9A:0B:00:00:00`; it carries the two UDP sockets of TETHER |
| `theTETHERservice` | rpc socket `:6666`, stream socket `:7777`, their two threads, the stream timer. It builds the `SharedVariables` from the Dictionary |
| `tCTRL` (ctrl thread) | an event thread with a 100 ms tick. It reads the PAX commands posted by the operations, drives `amcmj1_paxdriver`, publishes `IDpaxVALUE` and `IDpaxSTATUS`, and writes a sine on `IDdummyUINT32` |

The host PC must be at `10.0.1.104` (`amcmj1::net::hostaddress` in `app.gtw.04.cpp`): the service forces an ARP
resolution towards it at start (`Network::arp`), and the stream goes to whichever IP sent the `MGstreamSTART`.

## The files

| file | content |
|---|---|
| `appl.gateway.01.cpp` | `main()`, network constants, `tetherconfig` (a `constexpr theTETHERservice::Config`), IP service, ctrl thread |
| `dictionary.amcmj1.h` | the typed handles `amcmj1::vars::vXxx` (what the firmware uses) and `extern const Dictionary amcmj1::dict::dictionary`; a catalog of IDs in a comment |
| `dictionary.amcmj1.cpp` | the memory of the variables, their fields, the tables `variables[]` and `operations[]`, the operations themselves, `TETHER_VALIDATE`; long explanations of every form |
| `amcmj1_ctrl.h` | the events of the ctrl thread (`evtTick`, `evtPAXinit`, ...) and `signal()`, how the rpc thread wakes it |
| `amcmj1_dummy.h` | `DummySTRUCT`, `DummyMODE`, `DummyCLOCK`: the types of the example variables |
| `amcmj1_paxdriver.h/.cpp` | the fake PX3Q driver: `PAXcfg`, `PAXstatus`, `PAXvalue`, `init / deinit / start / stop / tick` |

## The Dictionary of the amcmj1

```
amcmj1 Dictionary                                       crc32 0x8E2A939B · 9 variables · 12 operations
├── variables  (0x01-0x9F)
│   ├── 0x01  IDtimeoflife    u64                 RO   computed on read                       (onread)
│   ├── 0x02  IDdummyUINT32   u32                 RW   a scalar, a sine written by the ctrl thread
│   ├── 0x03  IDdummySTRUCT   {first:i8[4], second:f32}   RW   a struct with an array
│   ├── 0x04  IDpaxCFG        {par1, par2, par3, txperiodms : u32}   RO   set by OPpaxINIT
│   ├── 0x05  IDpaxSTATUS     {state:u8, lastcommand:u8, errors:u16, commands:u32}   RO   written by ctrl
│   ├── 0x06  IDpaxVALUE      {force:f32[3], torque:f32[3], counter:u32}   RO   Fx Fy Fz Tx Ty Tz
│   ├── 0x07  IDdummyARRAY    f32[3]              RW   an array of scalars
│   ├── 0x08  IDdummyMODE     u8 (enum)           RW   idle / run / test, also set by OPsetMode
│   └── 0x09  IDdummyCLOCK    {microseconds:u64, seconds:u32}   RO   a struct computed on read (onread)
└── operations  (0xA0-0xDF)
    ├── 0xC0  OPmanageLED     {led:u8, on:bool}                 → -
    ├── 0xC1  OPpaxINIT       {par1, par2, par3, txperiodms}    → -
    ├── 0xC2  OPpaxDEINIT     -                                 → -
    ├── 0xC3  OPpaxSTART      -                                 → -
    ├── 0xC4  OPpaxSTOP       -                                 → -
    ├── 0xC5  OPgetUptime     -                                 → seconds:u32
    ├── 0xC6  OPreadRegister  address:u8                        → value:u32
    ├── 0xC7  OPsetMode       mode:u8                           → -
    ├── 0xC8  OPgetClock      -                                 → {microseconds:u64, seconds:u32}
    ├── 0xC9  OPadd           {a:i16, b:i16}                    → sum:i32
    ├── 0xCA  OPdivide        {a:i16, b:i16}                    → {quotient:i32, remainder:i32}
    └── 0xCB  OPaverage       values:f32[3]                     → mean:f32
```

Every row is there to show one thing:

| variable | what it shows |
|---|---|
| `IDtimeoflife` | a scalar computed when read: `onread` refreshes it, no thread updates it |
| `IDdummyUINT32` | a scalar written by the ctrl thread (a sine in [0, 2000], period 3 s) and also settable by the host: a good signal to stream |
| `IDdummySTRUCT` | a struct with an array inside, RW |
| `IDpaxCFG`, `IDpaxSTATUS`, `IDpaxVALUE` | structs of the PX3Q. RO for the host: only the firmware writes them (`IDpaxCFG` through `OPpaxINIT`) |
| `IDdummyARRAY` | an array of scalars, no typed handle: only the host uses it, so the ID is given as a number |
| `IDdummyMODE` | an enum, which is a scalar; the host sees the number. Also set by `OPsetMode` |
| `IDdummyCLOCK` | a struct computed when read, with padding inside (`DummyCLOCK` has 4 bytes of tail padding, described correctly by `TETHER_FIELD`) |

| operation | argument | result | what it shows |
|---|---|---|---|
| `OPpaxDEINIT`, `OPpaxSTART`, `OPpaxSTOP` | none | none | an operation that posts an event to the ctrl thread. `OPpaxSTART` is refused (nak) if the PX3Q is not initialised |
| `OPgetUptime` | none | scalar | `bool f(ResultOf<R> &)` |
| `OPgetClock` | none | struct | the result is a struct: the same fields as `IDdummyCLOCK` |
| `OPsetMode` | scalar (enum) | none | `bool f(const A &, Result &)`; a nak if out of range |
| `OPreadRegister` | scalar | scalar | a nak with a text for a wrong address |
| `OPaverage` | array `f32[3]` | scalar | an array argument |
| `OPmanageLED` | struct `{led, on}` | none | one argument only: a struct holds more |
| `OPpaxINIT` | struct `PAXcfg` | none | validates, stores in `IDpaxCFG`, wakes the ctrl thread |
| `OPadd` | struct `{a, b}` | scalar | struct in, scalar out |
| `OPdivide` | struct `{a, b}` | struct `{quotient, remainder}` | struct in, struct out, a nak for a division by zero |

The IDs of the operations (`0xC0` ...) are inside the operations range `0xA0-0xDF`; variables use `0x01-0x9F`.

### How the firmware reaches the variables

```cpp
auto &sv = embot::app::eth::theTETHERservice::getInstance().sharedvariables();   // in the ctrl thread: amcmj1::ctrl::sharedvars

pax::PAXcfg cfg {};
sv.read(amcmj1::vars::vPaxCfg, cfg);                                         // the whole struct, coherent

sv.write(amcmj1::vars::vDummyUINT32, value, tether::Origin::local);          // lock-free (4 bytes, aligned)

sv.update(amcmj1::vars::vPaxStatus, [](pax::PAXstatus &st) { st.commands++; },
          tether::Origin::local);                                            // read-modify-write in one critical section
```

The typed handle ties the ID to the type: `sv.write(amcmj1::vars::vPaxCfg, someUint32, ...)` does not compile.
`Origin::local` is the firmware: it can write also a variable that is RO for the host.

## What to launch on Linux to see it

You need a Linux shell (or WSL) on the same network as the board, Python 3.11 or later, and nicegui.

**Once**

```bash
sudo apt install python3 python3-venv          # if needed
python3 -m venv ~/.venv
~/.venv/bin/pip install nicegui
cd <the folder with tether.py, tethercodec.py>
```

**Terminal 1: the stream receiver** (start it first: it pings the board, which keeps the stream alive)

```bash
~/.venv/bin/python tether.py str --ip 10.0.1.99 --no-open
```

open `http://localhost:8081`

**Terminal 2: the variables and operations**

```bash
~/.venv/bin/python tether.py rpc --ip 10.0.1.99 --no-open
```

open `http://localhost:8080`. The first time it browses the board and writes `dictionary.toml`
(`board: protocol 5, application 0x01000000, 9 variables, 12 operations, ... crc32 0x8E2A939B`).
`--no-open` avoids the message `gio: ... Operation not supported` that WSL prints when it tries to open a browser: it is harmless, open the address by hand.

**Things to try**

1. *Read.* In `rpc`, press **ASK** on `IDtimeoflife` (it changes every time) and on `IDdummyUINT32` (the sine).
2. *Write.* Set `IDdummyMODE` to `1`, press **SET**, then **ASK**. `IDtimeoflife` is RO: its row has no **SET** button.
3. *Operations.* Press **RUN** on `OPgetUptime`, `OPadd` (`a=30000, b=30000` gives `sum=60000`), `OPdivide` (`a=17, b=5` gives `quotient=3, remainder=2`; `b=0` gives a nak `division by zero`), `OPaverage`.
4. *Stream.* Tick **stream** on `IDdummyUINT32`, press **START** in the *stream* box (0 = default period, 100 ms). In the `str` page tick the signal: a sine appears.
5. *The PX3Q.* Run `OPpaxSTART` first: nak `px3q not initialised`. Then `OPpaxINIT` with `par1=1, par2=2, par3=3, txperiodms=10`, `OPpaxSTART`, tick `IDpaxVALUE` in the stream: `force[0..2]` and `torque[0..2]` are plotted. `IDpaxSTATUS` goes `initialised` then `running`. `OPpaxSTOP` and `OPpaxDEINIT` stop and release it. (The fake driver fails an `OPpaxINIT` with `par3=57005` (0xDEAD), to see the error path: `state=3`.)
6. *Diagnostics.* **VERSION**, **STATS**, **PING** in the *board* box.
7. *PlotJuggler.* see below.

**The same from a prompt** (`--cli`):

```bash
~/.venv/bin/python tether.py rpc --ip 10.0.1.99 --cli
> list
> ask<IDtimeoflife>
> run[+ack]<OPpaxINIT, {par1=1, par2=2, par3=3, txperiodms=10}>
> run[+ack]<OPpaxSTART>
> set[+ack]<MGstreamADD, IDpaxVALUE>; set[+ack]<MGstreamSTART, 0>
> ask<IDpaxSTATUS>; ask<IDpaxVALUE>
> run[+ack]<OPpaxSTOP>; run[+ack]<OPpaxDEINIT>
> set[+ack]<MGstreamSTOP>
```

and `~/.venv/bin/python tether.py str --ip 10.0.1.99 --cli --log stream.csv` for a text summary and a csv.

### The PX3Q session, step by step

```
      host                         rpc thread                      ctrl thread
        │                               │                               │
        │  run[+ack]<OPpaxINIT, cfg>    │                               │
        │──────────────────────────────►│                               │   rpc thread: checks txperiodms in [1, 1000], writes IDpaxCFG
        │                               │  evtPAXinit                   │
        │                               │──────────────────────────────►│   ctrl thread: paxdriver::init(cfg), IDpaxSTATUS = initialised
        │  ack<OPpaxINIT, run>          │                               │
        │◄──────────────────────────────│                               │
        │  run[+ack]<OPpaxSTART>        │                               │
        │──────────────────────────────►│                               │   rpc thread: nak at once if the state is not initialised
        │                               │  evtPAXstart                  │
        │                               │──────────────────────────────►│   ctrl thread: paxdriver::start(), IDpaxSTATUS = running
        │  ack<OPpaxSTART, run>         │                               │
        │◄──────────────────────────────│                               │
        │                               │                               │
        │                               │                               │  every txperiodms: IDpaxVALUE = Fx Fy Fz Tx Ty Tz (ctrl thread)
        │  ask<IDpaxVALUE>  (or the stream)                             │
        │──────────────────────────────►│                               │   rpc thread: reads the variable
        │  say<IDpaxVALUE, {force=[..], torque=[..], counter=..}>       │
        │◄──────────────────────────────│                               │
```

### PlotJuggler 4

1. Install it (see `TETHER.md`) and start it.
2. *Streaming* → **UDP Server** → protocol **JSON**, port **9870** → *Start*.
3. `~/.venv/bin/python tether.py str --ip 10.0.1.99 --no-open --pj 127.0.0.1:9870`
   (or use the *PlotJuggler* box of the `str` page: host:port, then *forward*).
4. Start the stream from `rpc`. The signals appear as `IDpaxVALUE.force[0]`, `IDpaxVALUE.torque[2]`, `IDdummyUINT32`, ...:
   drag them to a plot.

If PlotJuggler runs on Windows and `tether.py` in WSL, use mirrored networking (below) so that `127.0.0.1` reaches it,
or give the Windows address of the PC in `--pj`.

## If something does not work

| symptom | cause and fix |
|---|---|
| `rpc` says the board does not answer | check the IP of the board (`10.0.1.99`), that the PC is on `10.0.1.x` and `ping 10.0.1.99` |
| the stream starts and then stops after a few seconds | nobody pings it: start `tether.py str` (with `--ip 10.0.1.99`). The `rpc` page does not keep the stream alive; the board stops it after 10 s of silence |
| `str` shows no frames | start the stream **after** `str`. With `--ip` the pings start at once; without it, only after the first frame |
| on WSL2 the frames never arrive | the board sends to the IP of the Windows PC, which WSL2 in NAT mode does not forward. Use mirrored networking: in `%UserProfile%\.wslconfig` put `[wsl2]` and `networkingMode=mirrored`, then `wsl --shutdown` |
| the page shows a stale Dictionary | the board was rebuilt with another Dictionary: `rpc` notices the CRC32 and browses again (the old file is kept as `dictionary.toml.bak`); press **BROWSE** to force it |
| `str` was started before any `rpc` | `str` only reads `dictionary.toml`: run `rpc` once |
| `address already in use` | another `rpc` or `str` is running (ports 8080, 8081, UDP 7777); close it or use `--gui-port`, `--localport` |

## Add your own

To add a variable or an operation to this Dictionary: define the C++ object or function, add an entry in `variables[]` or
`operations[]` in `dictionary.amcmj1.cpp` (and, for a variable the firmware uses, a handle in `dictionary.amcmj1.h`), and rebuild.
The comments before each table explain every form. `TETHER_VALIDATE` tells at compile time if something is wrong, and the host
picks up the new entry by itself: no change in `tether.py`.

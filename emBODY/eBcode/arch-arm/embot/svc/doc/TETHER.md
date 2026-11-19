# TETHER

**TETHER** is the diagnostic and operations service of the embot boards (`embot::svc::theTETHERservice`, in `embot/svc`). A host on the Ethernet reads and writes the
board's variables, runs its functions and receives a live stream of its values, with **no code on the host that is specific to
the firmware**: the board describes itself, and the host builds its interface from that description.

This is the general guide. It covers the architecture, **how to create a Dictionary and all its options**, the host tools,
how to install Python, NiceGUI and PlotJuggler 4, and the protocol. `amcmj1.md` describes the example application.

Contents

1. [Overview](#1-overview)
2. [Quick start](#2-quick-start)
3. [Creating a Dictionary](#3-creating-a-dictionary)
4. [Using the variables from the firmware](#4-using-the-variables-from-the-firmware)
5. [Starting the service](#5-starting-the-service)
6. [The host: embot-tether](#6-the-host-embot-tether)
7. [Protocol reference](#7-protocol-reference)
8. [Troubleshooting](#8-troubleshooting)
9. [Limits](#9-limits)

---

## 1. Overview

```
TETHER
├── Transport        UDP over embot::net::eth
│                      rpc socket     :6666   request / reply
│                      stream socket  :7777   board → host: sig   ·   host → board: ping
├── Protocol         ROP v6, little endian
│                      Frame   16-byte header + up to 32 ROPs, at most 1472 bytes
│                      ROP     4-byte head {opc, id, u16 size} + up to 512 bytes of data, padded to 4
│                      CMD     ask · say · set · sig · run · ack · nak · ping
│                      IDs     0x01-0x9F variables · 0xA0-0xDF operations · 0xE0-0xFF management
├── Services         Liveness      ping
│                    Discovery     browse of the Dictionary, with a CRC32
│                    RPC           ask / set / sig / run  →  say / ack / nak
│                    Streaming     periodic sig of the chosen variables, keepalive 10 s
│                    Diagnostics   MGversion, MGstats
├── Dictionary       constexpr, checked at compile time by TETHER_VALIDATE()
│   ├── variables      ID · name · access RO/RW · type or fields        → SharedVariables
│   └── operations     ID · name · argument · result                    → a function bool f(...)
├── SharedVariables  thread-safe store, built by the service from the Dictionary
│                      lock-free for 1, 2, 4 bytes aligned; a mutex for everything else
│                      clients: remote (the host, RO/RW) · local (the threads of the board)
└── Host             embot-tether rpc · embot-tether str · tethercodec.py · dictionary.toml
```

```
┌─ HOST  (Windows or Ubuntu) ────────────────────────────────────────────────────────────┐
│   embot-tether rpc and embot-tether str share dictionary.toml and tethercodec.py       │
│                                                                                        │
│   ┌────────────────────────┐    ┌────────────────────────┐    ┌──────────────────────┐ │
│   │ embot-tether rpc       │    │ embot-tether str       │    │ PlotJuggler 4        │ │
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

Three ideas:

- **Dictionary.** A `constexpr tether::Dictionary` lists the variables and the operations the host can see. You write only what the
  compiler cannot know (ID, access, name); sizes, types and the fields of a struct come from the C++ objects and function
  signatures. It is checked at compile time and lives in flash.
- **SharedVariables.** From the Dictionary the service builds a thread-safe store of the variables. The same variable is read and
  written by the host (through TETHER) and by the threads of the firmware (through typed handles).
- **Discovery.** The host asks the board for its Dictionary (a *browse*), checks a CRC32, and keeps it in `dictionary.toml`.
  Change the Dictionary, rebuild the firmware, and the host updates itself.

```
  your C++ objects                uint32_t x;     struct S {...};     bool f(const A &, Result &)
        │
        ▼
  tether::variable(...)                       tether::operation<f>(...)
  one entry per variable                      one entry per operation
        │                                             │
        └──────────────────────┬──────────────────────┘
                               ▼
        constexpr tether::Dictionary { variables, operations }
        TETHER_VALIDATE(dictionary)       ← IDs, names, sizes, signatures: a mistake is a build error
                               │
                               ▼
        theTETHERservice::Config { dictionary, applicationversion, Network, Stream, threads }
        theTETHERservice::getInstance().initialise(config)
                               │
              ┌────────────────┼──────────────────────┐
              ▼                ▼                      ▼
        SharedVariables   operations table      browse (MGbrowse*)
        (variables)       (theRPCmanager)       what the host reads: names, types,
                                                fields, CRC32  →  dictionary.toml
```

A conversation between the host and the board (see section 7 for the protocol):

```
   host: embot-tether rpc                           board: theRPCmanager
            │                                                 │
            │  ask<IDdummyUINT32>                             │
            │────────────────────────────────────────────────►│
            │  say<IDdummyUINT32, 1583>                       │
            │◄────────────────────────────────────────────────│
            │  set[+ack]<IDdummyMODE, 1>                      │
            │────────────────────────────────────────────────►│   writes the variable
            │  ack<IDdummyMODE, set>                          │
            │◄────────────────────────────────────────────────│
            │  run[+ack]<OPadd, {a=30000, b=30000}>           │
            │────────────────────────────────────────────────►│   add() runs in the rpc thread
            │  ack<OPadd, run, sum=60000>                     │
            │◄────────────────────────────────────────────────│
            │  run[+ack]<OPdivide, {a=17, b=0}>               │
            │────────────────────────────────────────────────►│   divide() fails: result.fail("division by zero")
            │  nak<OPdivide, run, "division by zero">         │
            │◄────────────────────────────────────────────────│
```

---

## 2. Quick start

**Board.** One variable and one operation, in a file of your application:

```cpp
#include "embot_svc_tether.h"
#include "embot_svc_theTETHERservice.h"

namespace tether = embot::svc::tether;
using embot::svc::theTETHERservice;

namespace {
    uint32_t counter {0};
}

bool reset(tether::Result &result) { counter = 0; return true; }

constexpr tether::Variable variables[] {
    tether::variable(0x01, counter, tether::Access::RW, "IDcounter")
};
constexpr tether::Operation operations[] {
    tether::operation<reset>(0xA0, "OPreset")
};

constexpr tether::Dictionary dictionary {variables, operations};
TETHER_VALIDATE(dictionary);

constexpr theTETHERservice::Config config {
    dictionary, 0x01000000,
    theTETHERservice::Network {6666, 7777, 7777, true, embot::net::eth::IPaddress {10, 0, 1, 104}},
    theTETHERservice::Stream {100*embot::core::time1millisec, 1*embot::core::time1millisec, 10*embot::core::time1second, 10*embot::core::time1second},
    embot::os::Thread::Props {embot::os::Priority::abovenorm37, 6*1024},
    embot::os::Thread::Props {embot::os::Priority::abovenorm36, 6*1024}
};

// in the init thread, after theIPservice::initialise():
theTETHERservice::getInstance().initialise(config);
```

This sketch keeps the Dictionary and the `Config` in one file. If they are in different files, declare the Dictionary `extern const` in a header
and define it `constexpr` in its `.cpp` (as the amcmj1 does): the `Config` only needs its address.

**Host.**

```
embot-tether rpc --ip <IP of the board>            # then open http://localhost:8080
```

(`embot-tether` installed once in a venv, on Windows or Ubuntu: `embot/svc/host/quick.startup.md`.)

Press **ASK** on `IDcounter`, **SET** a value, **RUN** `OPreset`. Nothing on the host knows `IDcounter`: it was read from the board.

---

## 3. Creating a Dictionary

### 3.1 The steps

1. **Decide the data.** Which values does the host need to see or set? Which functions should it be able to call?
2. **Give each one an ID and a name.** Variables `0x01-0x9F`, operations `0xA0-0xDF`; names `IDxxx` and `OPxxx` by convention.
3. **Define the C++ objects**: a global for each variable (static storage: the table keeps its address), a function for each operation.
4. **Describe structs** with `TETHER_FIELD`.
5. **Write the two tables** `variables[]` and `operations[]`.
6. **Build the Dictionary** and call `TETHER_VALIDATE(dictionary)`: every mistake is a build error with a message.
7. **Give it to the service** in `theTETHERservice::Config`.

A good place for all of it is one `.cpp`, with a small header for the typed handles (see `dictionary.amcmj1.h/.cpp`).

### 3.2 Variables

Every entry is

```
tether::variable(ID or handle, object, Access, "name" [, fields] [, onread])
```

There are two forms, and the compiler picks the one that fits the type of the object:

```cpp
// a scalar, or an array of scalars
template<typename T>
constexpr Variable variable(ID id, T &object, Access access, const char *name, OnRead onread = nullptr);

// a struct, with its fields
template<typename T, size_t N>
constexpr Variable variable(ID id, T &object, Access access, const char *name, const Field (&fields)[N], OnRead onread = nullptr);
```

Each exists also with a typed handle `tether::Var<T>` in place of the ID.

| parameter | meaning |
|---|---|
| **ID or handle** | `0x01 ... 0x9F`, unique. What the host uses on the wire. A plain number, or a typed handle `constexpr tether::Var<T> vX {0x03};` whose type `T` must be the type of the object (or it does not compile). Use a handle for any variable the firmware reads or writes; use a number for a variable only the host uses |
| **object** | a global (static storage): its address must be a constant. **At most 512 bytes.** The table only points to it, nothing is copied. A scalar (`bool`, `int8_t` ... `int64_t`, `uint8_t` ... `uint64_t`, `float`, `double`, an `enum`), an array of scalars (`float[3]`), or a struct (standard layout, trivially copyable) |
| **Access** | `RO`: the host cannot set it (a `set` gets a nak `read only`). `RW`: it can. The firmware can **always** write it, with `Origin::local`. Any variable, RO or RW, can be added to the stream |
| **"name"** | what the host shows and accepts: `[A-Za-z_][A-Za-z0-9_]*`, at most 28 characters, unique among variables **and** operations |
| **fields** | only for a struct: `constexpr tether::Field f[] { TETHER_FIELD(S, a), TETHER_FIELD(S, b) };`, one per member, in order. Name, type, number of elements and offset come from the real struct, so **padding is described correctly**. A member must be a scalar or an array of scalars (no nested struct) |
| **onread** | optional `void f(tether::ID, void *memory, uint16_t size)`. TETHER calls it just before reading the variable (an `ask`, a stream frame, or `SharedVariables::read`): it refreshes the object, which is then copied. For values computed at read time, with no thread updating them. It runs in the thread that reads (rpc or stream): keep it short. Works for every form |

The forms in practice:

| shape | example |
|---|---|
| scalar | `tether::variable(vDummyUINT32, dummyUINT32, tether::Access::RW, "IDdummyUINT32")` |
| scalar computed on read | `tether::variable(vTimeoflife, timeoflife, tether::Access::RO, "IDtimeoflife", refreshtimeoflife)` |
| enum | `tether::variable(vDummyMODE, dummyMODE, tether::Access::RW, "IDdummyMODE")` (the host shows the number) |
| array of scalars | `tether::variable(vDummyARRAY, dummyARRAY, tether::Access::RW, "IDdummyARRAY")` |
| struct | `tether::variable(vPaxCfg, paxcfg, tether::Access::RO, "IDpaxCFG", paxcfgfields)` |
| struct computed on read | `tether::variable(vDummyCLOCK, dummyCLOCK, tether::Access::RO, "IDdummyCLOCK", dummyCLOCKfields, refreshdummyCLOCK)` |

A struct and its fields:

```cpp
struct PAXvalue {
    float force[3] {0.0f, 0.0f, 0.0f};
    float torque[3] {0.0f, 0.0f, 0.0f};
    uint32_t counter {0};
};

constexpr tether::Field paxvaluefields[] {
    TETHER_FIELD(PAXvalue, force), TETHER_FIELD(PAXvalue, torque), TETHER_FIELD(PAXvalue, counter)
};
```

The host then shows `{force=[..], torque=[..], counter=..}`, and the stream splits it in the signals
`IDpaxVALUE.force[0]`, ..., `IDpaxVALUE.counter`.

**Time.** A frame of the stream has a single time, the board time when the frame was made. It is not the time of the sampling.
A variable that needs its acquisition time carries it: a struct with a `uint64_t timestamp` (microseconds of
`embot::core::now()`). A `uint64_t` is 8-byte aligned, so such a struct holds up to 32 bytes.

### 3.3 Operations

Every entry is

```
tether::operation<function>(ID, "name" [, description of the argument]) [.returns(...)]
```

with three forms, according to the argument of the function:

```cpp
template<auto F> constexpr Operation operation(ID op, const char *name);                                 // no argument
template<auto F> constexpr Operation operation(ID op, const char *name, const char *argname);            // a scalar, or an array of scalars
template<auto F, size_t N> constexpr Operation operation(ID op, const char *name, const Field (&fields)[N]);   // a struct
```

and, if the function gives back a result, the entry is completed with `.returns("name")` or `.returns(fields)`.

**The function** is given as a template argument, so TETHER takes from its signature the type and the size of the argument and
whether it gives back a result. It must be one of:

```cpp
bool f(tether::Result &);                                   // no argument
bool f(const A &arg, tether::Result &);                     // one argument
bool f(tether::ResultOf<R> &);                              // no argument, gives back an R
bool f(const A &arg, tether::ResultOf<R> &);                // one argument, gives back an R
```

It returns `true` for an **ack**, `false` for a **nak**. `result.fail("text")` puts a text (at most 36 characters) in the nak and
returns `false`; without one the nak says `operation failed`. `result.set(value)` puts the value in the ack.
`result.from()` is the IP of the host that asked. An operation takes **one** argument: if it needs more, put them in a struct.

| parameter | meaning |
|---|---|
| **ID** | `0xA0 ... 0xDF`, unique |
| **"name"** | as for a variable |
| **description** | depends on the argument `A`: *none* for no argument; *its name* (`"address"`) for a scalar or an array of scalars; *its fields* (`ledcommandfields`) for a struct. The argument must be described: if the function has one and the description is missing (or the other way round) it does not compile |
| **.returns()** | only for `ResultOf<R>`: it names what the ack carries back. `R` is at most 36 bytes: an ack stays light, a bigger output is a variable that the host reads with `ask`. A scalar or an array: its name. A struct: its fields (a `constexpr tether::Field[]`) |

A struct used as an argument must be standard layout, trivially copyable, **default constructible** (TETHER builds an empty one and
fills it from the ROP) and at most 512 bytes. A struct used as a result must be trivially copyable and at most 36 bytes.

All the combinations, each with an example from the amcmj1:

| argument | result | function | entry |
|---|---|---|---|
| none | none | `paxSTART` | `operation<paxSTART>(0xC3, "OPpaxSTART")` |
| none | scalar | `getUptime` | `operation<getUptime>(0xC5, "OPgetUptime").returns("seconds")` |
| none | struct | `getClock` | `operation<getClock>(0xC8, "OPgetClock").returns(vars::dummyCLOCKfields)` |
| scalar | none | `setMode` | `operation<setMode>(0xC7, "OPsetMode", "mode")` |
| scalar | scalar | `readRegister` | `operation<readRegister>(0xC6, "OPreadRegister", "address").returns("value")` |
| array | scalar | `average` | `operation<average>(0xCB, "OPaverage", "values").returns("mean")` |
| struct | none | `manageLED` | `operation<manageLED>(0xC0, "OPmanageLED", ledcommandfields)` |
| struct | scalar | `add` | `operation<add>(0xC9, "OPadd", operandsfields).returns("sum")` |
| struct | struct | `divide` | `operation<divide>(0xCA, "OPdivide", operandsfields).returns(divisionfields)` |

(scalar → struct is the same as the others: the argument by its name, the result by its fields.)

A complete one:

```cpp
struct Operands { int16_t a {0}; int16_t b {0}; constexpr Operands() = default; constexpr explicit Operands(int16_t x, int16_t y) : a(x), b(y) {} };
struct Division { int32_t quotient {0}; int32_t remainder {0}; constexpr Division() = default; constexpr explicit Division(int32_t q, int32_t r) : quotient(q), remainder(r) {} };

constexpr tether::Field operandsfields[] { TETHER_FIELD(Operands, a), TETHER_FIELD(Operands, b) };
constexpr tether::Field divisionfields[] { TETHER_FIELD(Division, quotient), TETHER_FIELD(Division, remainder) };

bool divide(const Operands &op, tether::ResultOf<Division> &result)
{
    if(0 == op.b) { return result.fail("division by zero"); }          // -> nak<OPdivide, run, "division by zero">
    return result.set(Division {op.a / op.b, op.a % op.b});             // -> ack<OPdivide, run, {quotient=3, remainder=2}>
}

tether::operation<divide>(0xCA, "OPdivide", operandsfields).returns(divisionfields)
```

**Where an operation runs, and what an ack means.** Operations run in the **rpc thread**, so they must be short and must not
block. If the real work belongs to another thread, the operation checks what it can, posts an event to that thread and returns:
**ack means accepted**, not done. Report the outcome in a variable of the Dictionary (as `IDpaxSTATUS` does for the PX3Q
operations). To change a shared variable use `SharedVariables` (section 4).

### 3.4 The limits

| what | limit |
|---|---|
| variables | 159 (`0x01-0x9F`) |
| operations | 64 (`0xA0-0xDF`) |
| size of a variable, of an argument | 512 bytes (the data of a ROP) |
| size of a result | 36 bytes (it travels in the ack) |
| frame | 1472 bytes and 32 ROPs: a stream holds at most 32 variables whose ROPs (4 + size padded to 4) fit 1456 bytes, e.g. two of 512 bytes |
| name | `[A-Za-z_][A-Za-z0-9_]*`, at most 28 characters, unique among variables and operations; also the names of fields, arguments and results, unique within their struct |
| text of a nak | 36 characters |
| a field | a scalar or an array of scalars, not a struct |
| an argument | one: a scalar, an array of scalars or a struct |

### 3.5 What `TETHER_VALIDATE` tells you

| message | what to fix |
|---|---|
| `too many variables (max 159) or operations (max 64)` | the tables are too long |
| `a variable has an ID out of [0x01, 0x9F]` / `an operation has an ID out of [0xA0, 0xDF]` | an ID in the wrong range |
| `an ID is used twice` / `a name is used twice` | duplicates |
| `a name is not [A-Za-z_][A-Za-z0-9_]* w/ at most 28 chars` | a bad name |
| `a variable is not valid: ...` | no memory or name, or fields that are not scalars, overlap, are out of order or go past the size |
| `an operation is not valid: ... did you forget .returns() ?` | an argument or result without its name or fields |
| `a struct needs its fields: use the overload w/ an array of TETHER_FIELD()` | a struct variable without `fields` |
| `a variable must fit a ROP: at most 512 bytes` | too big: split it |
| `a result is at most 36 bytes: for more, write a variable and read it w/ ask` | the result of an operation is too big |
| `a field must be a scalar or an array of scalars` | a nested struct |
| `an operation must be bool f(tether::Result &) ...` | the wrong signature |
| `this operation has no argument: use operation<f>(id, "OPname")` | a description given to a function without argument |

### 3.6 Patterns

- **A status the host should watch**: a struct variable, RO, written by the thread that owns the state (`IDpaxSTATUS`), streamed.
- **A configuration**: a struct variable, RO for the host, set only by an operation that validates it (`OPpaxINIT` writes `IDpaxCFG`).
- **A command to another thread**: an operation that posts an event and returns; the outcome goes in a status variable.
- **A value computed on demand**: `onread` (`IDtimeoflife`, `IDdummyCLOCK`).
- **A fast signal for plotting**: a scalar of 1, 2 or 4 bytes: written lock-free by the control loop, streamed at the period you choose.

---

## 4. Using the variables from the firmware

The threads of the board reach the variables through `SharedVariables`, never through the global itself:

```cpp
auto &sv = embot::svc::theTETHERservice::getInstance().sharedvariables();
```

```cpp
constexpr tether::Var<uint32_t> vDummyUINT32 {0x02};       // a typed handle: the ID and the type go together

uint32_t v {};
sv.read(vDummyUINT32, v);                                  // a coherent copy
sv.write(vDummyUINT32, 1234, tether::Origin::local);       // the firmware is Origin::local
sv.update(vDummyUINT32, [](uint32_t &x) { x++; }, tether::Origin::local);    // read-modify-write, one critical section
sv.writeFromISR(vDummyUINT32, 5);                          // only for 1, 2, 4 bytes (lock-free)
```

| | |
|---|---|
| **lock-free** | a variable of 1, 2 or 4 bytes that is aligned: one atomic load or store. Usable from an ISR with `writeFromISR` |
| **mutex** | everything else (structs, arrays, 64 bits, variables with `onread`): one mutex with priority inheritance, so a struct is always read and written **as a whole** |
| **update** | the callable may run more than once on a lock-free variable (a compare-exchange loop): it must only compute the new value |
| **Origin::local / remote** | the firmware is `local` and can write any variable. The host is `remote` and is bound by RO / RW |
| **the handle** | `sv.write(vPaxCfg, someUint32, ...)` does not compile: the type is tied to the ID |

Do not access the global of a variable directly from two threads: that is what `SharedVariables` is for.

---

## 5. Starting the service

`theTETHERservice` needs `embot::net::eth::theIPservice` (it attaches two UDP sockets to it). Call `initialise()` once, from the init
thread, after `theIPservice::initialise()`. It returns `false` if the Dictionary or the stream periods are not valid, or if already
initialised.

```cpp
constexpr theTETHERservice::Config config {
    dictionary,              // the Dictionary: a reference to a static object, only its address is kept
    applicationversion,      // uint32_t: carried to the host by MGversion (0 = not given)
    theTETHERservice::Network {rpcport, streamport, hostport, arp, arphost},
    theTETHERservice::Stream {defaultperiod, minperiod, maxperiod, keepalive},
    rpcthread,               // embot::os::Thread::Props {priority, stack}
    streamthread
};
```

| field | meaning |
|---|---|
| `Network::rpcport` | UDP port of the rpc socket (default 6666). It replies to the sender |
| `Network::streamport` | UDP port of the stream socket (default 7777). It accepts only `ping` |
| `Network::hostport` | where the host receives the stream (default 7777), on the IP of whoever started it |
| `Network::arp`, `arphost` | if `arp` is true the sockets `connect()` to `arphost` at start, to force the ARP resolution towards the PC |
| `Stream::defaultperiod` | the period for `set<MGstreamSTART, 0>` (default 100 ms) |
| `Stream::minperiod`, `maxperiod` | the host can ask a period in `[min, max]` (1 ms to 10 s) |
| `Stream::keepalive` | the stream stops by itself after this time without a ping from the host (default 10 s; 0 = never) |
| `rpcthread`, `streamthread` | priority and stack of the two threads. The stream thread runs the timer of the stream. Keep `theIPservice` above them |
| `applicationversion` | for example the version of the `Signature` of the application; not part of the CRC32, so changing it does not make the host browse again |

---

## 6. The host: embot-tether

The client of TETHER on a PC is `embot-tether` (`tether.py` and its codec `tethercodec.py`, in `embot/svc/host/tether`):
`rpc` for the variables and the operations, `str` for the stream, a page in the browser or a text prompt, and PlotJuggler
for the plots. It needs nothing of the board but its IP: it reads the Dictionary from the board.

- install, on Windows 11 or Ubuntu 24.04 / 26.04: `embot/svc/host/README.md` (only the commands: `embot/svc/host/quick.startup.md`)
- use: the pages, the prompt and its grammar, the options, the stream, PlotJuggler: `embot/svc/host/tether/README.md`

---

## 7. Protocol reference

**ROP**, Remote Operation Protocol, version 6. Little endian. UDP, one frame per datagram.

A frame is a 16-byte header `{u32 signature 0xFEEDC0DE, u8 layout = 0, u8 numOfROPs, u16 filler, u64 time in µs}` followed by up to
**32** ROPs, one after the other. A ROP is a 4-byte head `{u8 opc, u8 id, u16 size}` plus `size` bytes of data (at most **512**),
padded with zeros to a multiple of 4. The receiver ignores the padding, and accepts a frame only if its ROPs fill it exactly:
a malformed frame is rejected as a whole, nothing of it is executed. `layout` is 0; a frame of version 5 (`layout` 8 ... 40, the
size of its uniform slots) is rejected.
`opc`: low nibble = CMD. High nibble: in `ask`, `set`, `sig`, `run`, `ping` the options, bit 4 = "ack wanted" (`set[+ack]`,
`run[+ack]`); in `ack` and `nak` the CMD they refer to.
A frame is at most **1472** bytes, one datagram on an MTU of 1500 without IP fragmentation. A board may accept and produce
smaller frames: `MGbrowseINFO` tells its maximum of ROPs and of bytes per frame, and the host uses the smaller ones.

All the replies to a frame go in one frame. A ROP that expects a reply (`ask`, `+ack`) is executed only if its reply has room
(before a `set` or a `run`, room for 40 bytes: the biggest ack or nak); else it is not executed and gets `nak<"no room">`, or it is
counted in `rpcdroppedROPs` if not even that fits. The host knows the sizes from the browse and splits its requests so that this
does not happen.

| CMD | | |
|---|---|---|
| ask 0 / say 1 | host asks, board says the value | `ask<ID>` → `say<ID, value>` |
| set 2 | host writes (with `+ack`, the board acknowledges: a light ack, without data) | `set[+ack]<ID, value>` → `ack<ID, set>` |
| sig 3 | a value, no reply: in the stream (board → host) or a write without ack (host → board) | `sig<ID, value>` |
| run 7 | host runs an operation: the argument up to 512 bytes, the result up to 36 | `run[+ack]<OP, arg>` → `ack<OP, run, result>` |
| ack 4 / nak 5 | positive / negative reply. An ack carries data only for the result of a run; a nak may carry a **text** (no NUL, at most 36 characters) | `nak<OP, run, "why">` |
| ping 6 | liveness, on both sockets | `ping<ID>` → `ack<ID, ping>` |

A CMD that is not valid for the range of its ID gets a nak only if the sender expects a reply (`ask` or `+ack`).

**IDs**

| range | | |
|---|---|---|
| `0x00` | none (e.g. `ping<>`) | |
| `0x01-0x9F` | variables | `ask`, `set`, `sig` |
| `0xA0-0xDF` | operations | `run` |
| `0xE0-0xFF` | management, built in | `ask`, `set` |

| ID | name | |
|---|---|---|
| `0xE0` / `0xE1` | MGstreamADD / MGstreamREM | add / remove a variable from the stream |
| `0xE2` | MGstreamLIST | the streamed IDs |
| `0xE3` | MGstreamSTART | `u16` ms: 0 = default, else in [min, max]. The stream goes to the sender. `ask` gives the period applied |
| | | `MGstreamADD` refuses a variable that would make the frame of the stream exceed 32 ROPs or the max bytes of a frame |
| `0xE4` | MGstreamSTOP | |
| `0xE8` / `0xE9` / `0xEA` | MGbrowseINFO / ITEM / FIELD | the Dictionary: counts, the CRC32; one item per variable or operation; the fields |
| `0xF0` | MGversion | protocol and application version |
| `0xF1` | MGstats | nine counters: rpc frames received, wrong frames, ROPs received, replies dropped; stream frames sent, read failures, pings received, frames discarded, keepalive stops |

Standard nak texts: `unknown id`, `read only`, `wrong size`, `wrong size of the argument`, `command not allowed for this id`,
`operation failed`, `read failed`, `no room`, `unknown variable, or stream full`, `not streamed`, `period out of range`, `cannot stop`,
`unknown management id`, `index out of range`; an operation gives its own with `result.fail("...")`.

**Discovery.** `MGbrowseINFO` (12 bytes) gives the number of variables and operations, the maximum of ROPs and of bytes per frame,
the maximum of data of a ROP (512) and a **CRC32** of the whole Dictionary (items and fields, including the argument and the result
of every operation). `MGbrowseITEM i` (40 bytes) gives item `i` (ID, kind, access, type, 16-bit size, and for an operation the
kind and the size of its result, name) and `MGbrowseFIELD id n` (36 bytes) its fields, w/ 16-bit count and offset (the argument
fields first, then the result fields). If the CRC32 of
`dictionary.toml` differs from the board's, the host browses again.

**Time.** The time in the header of a frame is the board time (µs) when the frame was made: not the time of the sampling.

**Keepalive.** The stream stops after `Stream::keepalive` (10 s) without a ping on the stream socket.

---

## 8. Troubleshooting

| symptom | cause and fix |
|---|---|
| `rpc`: the board does not answer | wrong IP, a different subnet, the board not running: `ping <board>`. Check `--ip` and `--port` |
| the stream stops after a few seconds | nobody pings it: run `embot-tether str`. `rpc` does not keep it alive |
| `str` shows no frames | start the stream after `str`; give `--ip <board>` so the pings start at once; on WSL2 use mirrored networking |
| the page shows an old Dictionary | the CRC32 changed: `rpc` browses again by itself; **BROWSE** forces it; the old file is `dictionary.toml.bak` |
| `str`: values shown as raw bytes | `dictionary.toml` is missing or stale: run `rpc` once |
| `a nak: wrong size` on a set | the value does not match the variable (a struct needs all its fields) |
| `address already in use` | another `rpc` or `str` is running: close it, or change `--gui-port` / `--localport` |
| PlotJuggler sees nothing | UDP Server not started, the port is not 9870, or `--pj` points to the wrong host; with `str` open, the *PlotJuggler* box shows the sent and error counters |
| a build error `TETHER: ...` | see 3.5 |

---

## 9. Limits

- A variable and an argument are at most 512 bytes, a result 36 bytes. A single value bigger than a frame is not supported.
- A frame is at most 1472 bytes: a stream of big variables is refused by `MGstreamADD` when it does not fit one frame.
- 159 variables and 64 operations.
- Operations run in the rpc thread: they must be short.
- The frame time is the time of the frame, not of the sampling.
- The communication between the M4 and M7 cores is not covered.
- **No authentication**: any host that reaches the UDP ports can write the RW variables and run the operations. Use it on an isolated network.

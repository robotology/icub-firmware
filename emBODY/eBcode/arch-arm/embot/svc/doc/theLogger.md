# How to use `theLogger`

`embot::svc::theLogger` sends the lines of `embot::core::print()` over UDP to a host, and accepts a few text commands on
the same port (9999 in the examples). The application keeps calling `embot::core::print()` as it always did: after the
init of the logger its lines also go over the network, and the host can read them w/ `embot-logger` or w/ `nc`.

The code: `embot/svc/embot_svc_theLogger.h/.cpp`. In a project: the include path `embot/svc` and
`embot_svc_theLogger.cpp`. It needs:

- `embot::net::eth::theIPservice` (lwIP): the logger attaches one socket to it. So the BSP enables the ETH (on amcmj1:
  `EMBOT_ENABLE_hw_eth`, `EMBOT_ENABLE_hw_eth_LWIP` and `EMBOT_ENABLE_hw_spi_two` for the switch, as in
  `udpdemo-tether/cfg/embot_hw_bsp_amcmj1_custom_config.h`)
- `embot::core::setprint()`, which is in the branch `devel` of `icub-firmware-shared` (not yet in `master`)

Nothing else in the BSP: see "Where print() goes".

## Init

A `constexpr` `Config` and one call from the init thread:

```cpp
#include "embot_svc_theLogger.h"
#include "embot_net_eth_theIPservice.h"

constexpr embot::net::eth::IPaddress localaddress {10, 0, 1, 99};
constexpr embot::net::eth::IPaddress hostaddress {10, 0, 1, 104};
constexpr embot::net::eth::Port portLOG {9999};

constexpr embot::svc::theLogger::Config logger
{
    embot::net::eth::SocketAddress {hostaddress, portLOG},     // destination at start (port 0: nowhere yet)
    localaddress,                                               // the IP shown by the decoration
    portLOG,                                                    // the port of the board
    embot::svc::theLogger::Activation::ON,                      // lines taken from the start
    embot::svc::theLogger::Decoration::ON,                      // each line begins w/ [10.0.1.99 @ time]
    embot::svc::theLogger::Commands::ON,                        // the text commands on portLOG
    true,                                                       // print() also goes to the previous output (ITM / SWO)
    1024,                                                       // bytes of lines in a UDP frame (64 ... 1472)
    embot::os::Thread::Props {embot::os::Priority::low10, 2*1024}
};

void initSystem(embot::os::Thread *t, void* initparam)
{
    embot::svc::theLogger::getInstance().initialise(logger);                    // first: it keeps the lines from now on
    embot::net::eth::theIPservice::getInstance().initialise(ipservice);         // then lwIP
    // ... the other services and threads
    embot::core::print("quitting the INIT thread");
}
```

- `initialise()` can come before `theIPservice::initialise()`: the socket is opened by the thread of the logger, which
  runs after the init thread. The lines printed meanwhile wait in the buffer (`buffercapacity` = 4096 bytes).
- Count the socket of the logger in `theIPservice::Config::numberofattachablesockets`.
- `initialise()` returns false if the `Config` is not valid (see `Config::isvalid()`) or if called twice.

## Print

Nothing new: `embot::core::print()`, from any thread.

```cpp
embot::core::print("motor started");
embot::core::print("speed = " + std::to_string(speed) + " rpm");
```

Each `print()` is one line. With the decoration, the host sees:

```text
[10.0.1.99 @ S12:m345:u678] motor started
```

The time is `embot::core::now()` when the line was taken, in the compact form of `embot::core::TimeFormatter`.

## Where print() goes

`embot::core::print()` calls a function held by `embot::core`. `embot::hw::init()` (called by `embot::os::init()`) sets it
to `embot::hw::bsp::print()`, which calls `embot::hw::bsp::DRIVER::puts()`. By default (STM32 HAL) that writes on the ITM,
i.e. the SWO of the debug probe (uVision: View / Serial Windows / Debug (printf) Viewer). There are two ways to change it.

**In the BSP, at compile time.** Define `EMBOT_REDEFINE_hw_bsp_DRIVER_puts` in the configuration of the BSP of the
application and write the new `puts()` in a `.cpp` of the BSP: the one on the ITM in `embot/hw/embot_hw_bsp.cpp` is no
longer compiled. It is the output of the board from the first `print()`, also before the RTOS starts.

```cpp
// cfg/embot_hw_bsp_amcmj1_custom_config.h
#define EMBOT_REDEFINE_hw_bsp_DRIVER_puts

// a .cpp of the BSP, e.g. bsp/shared/embot_hw_bsp_DRIVER_amcmj1.cpp
#if defined(EMBOT_REDEFINE_hw_bsp_DRIVER_puts)
int embot::hw::bsp::DRIVER::puts(const char* str)
{
    // e.g. the chars of str and a final '\n' on a UART. it returns the number of chars sent
    return 0;
}
#endif
```

**In the application, at run time.** `embot::core::setprint(fp)` makes `print()` call `fp` and returns the function used
until then. So the new function can replace the previous one (it ignores it) or sit beside it (it calls it too):

```cpp
namespace {
    embot::core::fpPrint s_previous {nullptr};      // constant initialised

    int alsohere(const std::string &str)
    {
        // ... the new output of str
        return (nullptr != s_previous) ? s_previous(str) : 0;
    }
}

s_previous = embot::core::setprint(alsohere);         // print() goes to alsohere() and, through it, to the ITM
```

theLogger uses the second way: `initialise()` calls `setprint()`. With `Config::alsoprevious = true` the lines go over
UDP and also to the previous output (the ITM, or the `puts()` of the BSP if it was redefined); w/ `false` only over UDP.
The two ways can be used together.

## From the host

The board sends to the port the host has written from, so it works also behind a NAT (e.g. WSL2 w/out mirrored
networking). The simplest host is `nc`: an empty line (or `hello`) and the lines arrive in the same terminal.

```console
nc -u 10.0.1.99 9999                # Ubuntu (Windows: ncat -u 10.0.1.99 9999, ncat comes w/ nmap)
<Enter>                             # the lines come to this terminal
status<Enter>                       # [theLogger] status: dest 10.0.1.104:54321, on, deco on, ...
```

For a page in the browser, several boards, filters and records: `embot-logger --ip 10.0.1.99`, see
`../host/logger/README.md`.

## The commands

One per UDP frame (at most `maxcommandsize` = 64 bytes). Case and spaces at the ends do not matter. Each one has a reply,
sent to who asked, which begins w/ `[theLogger]` (e.g. `[theLogger] ok: off`), also when the logger is OFF.

| command | what |
|---|---|
| (empty) or `hello` | the lines go to the sender of the command, from now on |
| `on` / `off` | the lines are taken / discarded (`off` also discards the lines not yet sent) |
| `deco on` / `deco off` | the decoration `[<IP> @ <time>]` |
| `dest <ip>:<port>` | the lines go to `<ip>:<port>`, e.g. `dest 10.0.1.104:9999` |
| `status` | one line w/ the destination, the modes and the counters |
| `help` | the list of the commands |

Whoever reaches the port can change the destination: fine in a lab network. `Commands::OFF` in the `Config` disables them.

## From the firmware

The same controls, at any time and from any thread:

```cpp
embot::svc::theLogger &log {embot::svc::theLogger::getInstance()};

log.set(embot::svc::theLogger::Activation::OFF);                                    // discard the lines
log.set(embot::svc::theLogger::Decoration::OFF);                                    // no prefix
log.set(embot::net::eth::SocketAddress {embot::net::eth::IPaddress {10, 0, 1, 104}, 9999});    // a new destination

const embot::svc::theLogger::Status s {log.status()};                              // a coherent snapshot
// s.destination, s.activation, s.decoration, s.commands, s.stats.lines, s.stats.frames, s.stats.droppedlines
```

`udpdemo-tether` shows the `Status` also on TETHER, as the variable `IDlogSTATUS` (computed on read).

## Notes

- `print()` never blocks and never sends: it copies the line into the buffer and wakes up the thread of the logger, which
  sends many lines in a frame. If the buffer is full the line is lost and counted in `Stats::droppedlines`.
- The `'\r'` and `'\n'` at the end of a `print()` are removed, those inside it become spaces. A line longer than
  `maxframesize` is cut and its last character becomes `'~'`.
- A `print()` must not begin w/ `[theLogger]`: the host uses it to tell the replies from the lines.
- Not from an ISR: the buffer is protected by a mutex.
- The thread of the logger does not log itself: a `print()` done inside it (e.g. by `theIPservice` while it sends a frame
  of the logger) goes only to the previous output, if any. It avoids a loop.

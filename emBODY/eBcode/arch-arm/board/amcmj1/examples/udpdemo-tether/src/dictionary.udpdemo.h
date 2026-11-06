/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __DICTIONARY_UDPDEMO_H_
#define __DICTIONARY_UDPDEMO_H_

#if 0

# TETHER dictionary of the udpdemo.tether: catalog

Implemented in `dictionary.udpdemo.cpp`, declared in `dictionary.udpdemo.h`, types in `udpdemo_types.h`.

- Dictionary: `udpdemo::dict::dictionary`
- Typed handles of the variables: `udpdemo::vars::v<Name>` (e.g. `udpdemo::vars::vEchoStats`)

It is a didactic Dictionary: each variable and each operation shows one way of using TETHER, and the echo thread shows
how a thread of the board shares its data w/ the host.

## Variables (ID 0x01-0x9F)

The access is the one seen by the host (RPC). Every variable can be streamed.

| ID   | Name          | Type                 | Access | What it shows                                                  |
|------|---------------|----------------------|--------|----------------------------------------------------------------|
| 0x01 | IDtimeoflife  | `uint64_t`           | RO     | a scalar computed on read (onread)                             |
| 0x02 | IDechoCONFIG  | `types::EchoCONFIG`  | RW     | a struct written by the host and used by the echo thread       |
| 0x03 | IDechoSTATS   | `types::EchoSTATS`   | RO     | a struct written only by a thread of the board (the echo)      |
| 0x04 | IDechoLAST    | `types::EchoLAST`    | RO     | a struct w/ an array, padding and a uint64_t timestamp         |
| 0x05 | IDticks       | `uint32_t`           | RW     | a scalar written by the echo thread (every 5 ms) and by the host |
| 0x06 | IDgain        | `float`              | RW     | a float, used by IDsignal                                      |
| 0x07 | IDvector      | `float[3]`           | RW     | an array of scalars (no fields, no typed handle)               |
| 0x08 | IDmode        | `types::Mode`        | RW     | an enum, also changed by OPsetMode                             |
| 0x09 | IDflag        | `bool`               | RW     | a bool                                                         |
| 0x0A | IDclock       | `types::Clock`       | RO     | a struct computed on read                                      |
| 0x0B | IDsignal      | `float`              | RO     | a sine of amplitude IDgain at 1 Hz computed on read: to plot w/ the stream |
| 0x0C | IDlogSTATUS   | `types::LogSTATUS`   | RO     | the state of embot::net::theLogger, computed on read from theLogger::status() (another module) |
| 0x0D | IDviewof_wave | `float`              | RO     | a view of wave, a variable of the application (udpdemo-tether.cpp): a copy taken on read. only w/ tetherdebugENABLE_wave |
| 0x0E | IDviewof_pwm  | `echo::PWM`          | RO     | a view of pwm, a struct of the application protected by a mutex: a coherent copy taken on read. only w/ tetherdebugENABLE_pwm |

## Operations (ID 0xA0-0xDF)

They run in the rpc thread of TETHER.

| ID   | Name           | Argument                 | Result                           | What it shows                          |
|------|----------------|--------------------------|----------------------------------|----------------------------------------|
| 0xA0 | OPmanageLED    | `LEDcommand {led, on}`   | -                                | struct argument, no result             |
| 0xA1 | OPgetUptime    | -                        | `uint32_t` "seconds"             | no argument, scalar result             |
| 0xA2 | OPreadRegister | `uint8_t` "address"      | `uint32_t` "value"               | scalar argument and result, or a nak   |
| 0xA3 | OPsetMode      | `types::Mode` "mode"     | -                                | enum argument that writes a variable   |
| 0xA4 | OPgetClock     | -                        | `types::Clock`                   | no argument, struct result             |
| 0xA5 | OPadd          | `Operands {a, b}`        | `int32_t` "sum"                  | struct argument, scalar result         |
| 0xA6 | OPdivide       | `Operands {a, b}`        | `Division {quotient, remainder}` | struct argument and result, or a nak   |
| 0xA7 | OPaverage      | `float[3]` "values"      | `float` "mean"                   | array argument, scalar result          |
| 0xA8 | OPechoRESET    | -                        | -                                | it writes a RO variable (IDechoSTATS)  |
| 0xA9 | OPechoHELLO    | -                        | -                                | it asks the echo thread to do something (a frame to the last sender) |

A variable named `IDviewof_<name>` is a view of the variable `<name>` of the application: the application keeps its own
variable where it is, and the Dictionary has a copy of it, taken when the host reads it (onread). it is RO.

`LEDcommand`, `Operands` and `Division` are defined in `dictionary.udpdemo.cpp`.

embot::net::theLogger is configured w/ its own text commands on its port 9999 (see `embot_net_theLogger.h`), not w/
TETHER: TETHER only shows its state w/ IDlogSTATUS.

#endif


#include "embot_core.h"
#include "embot_app_eth_TETHER.h"
#include "embot_app_eth_SharedVariables.h"
#include "udpdemo_types.h"
#include "udpdemo_echo.h"


#if 0

the Dictionary of the udpdemo.tether: the variables and the operations that the host (tether.py) sees, and how the firmware
accesses them. this is the file to include: the Dictionary is defined in dictionary.udpdemo.cpp, where one adds variables
and operations.

- udpdemo::dict::dictionary    the constexpr Dictionary, to give to theTETHERservice::Config
- udpdemo::vars::vXxx          the typed handles of the variables, to read and write them w/ theTETHERservice::sharedvariables():
                                  sv.read(udpdemo::vars::vEchoConfig, cfg);   sv.write(udpdemo::vars::vTicks, n, Origin::local);

#endif


// ---- the typed handles of the variables. their memory, their fields and their table are in dictionary.udpdemo.cpp

namespace udpdemo::vars {

    namespace tether = embot::app::eth::tether;

    // whoever reads or writes a variable w/ theTETHERservice::sharedvariables() gives its handle, so it cannot use a wrong
    // type. the number is the ID the host uses. IDvector has no handle: only the host uses it
    inline constexpr tether::Var<uint64_t> vTimeoflife {0x01};                  // RO, computed on read
    inline constexpr tether::Var<types::EchoCONFIG> vEchoConfig {0x02};         // RW, read by the echo thread
    inline constexpr tether::Var<types::EchoSTATS> vEchoStats {0x03};           // RO for rpc: written by the echo thread
    inline constexpr tether::Var<types::EchoLAST> vEchoLast {0x04};             // RO for rpc: written by the echo thread
    inline constexpr tether::Var<uint32_t> vTicks {0x05};                       // RW, written by the echo thread every 5 ms
    inline constexpr tether::Var<float> vGain {0x06};                           // RW, the amplitude of IDsignal
    inline constexpr tether::Var<types::Mode> vMode {0x08};                     // RW, also changed by OPsetMode
    inline constexpr tether::Var<bool> vFlag {0x09};                            // RW
    inline constexpr tether::Var<types::Clock> vClock {0x0A};                   // RO, computed on read
    inline constexpr tether::Var<float> vSignal {0x0B};                         // RO, computed on read
    inline constexpr tether::Var<types::LogSTATUS> vLogStatus {0x0C};           // RO, computed on read from embot::net::theLogger
#if defined(tetherdebugENABLE_wave)
    inline constexpr tether::Var<float> vViewofWave {0x0D};                     // RO, a view of wave: on read a copy of udpdemo::echo::readwave()
#endif
#if defined(tetherdebugENABLE_pwm)
    inline constexpr tether::Var<echo::PWM> vViewofPwm {0x0E};                  // RO, a view of pwm: on read a copy of udpdemo::echo::readpwm()
#endif

} // namespace udpdemo::vars


// ---- the Dictionary: defined in dictionary.udpdemo.cpp and checked there by TETHER_VALIDATE().
//      theTETHERservice::Config takes its reference: constexpr theTETHERservice::Config c {udpdemo::dict::dictionary, ...}

namespace udpdemo::dict {

    extern const embot::app::eth::tether::Dictionary dictionary;

} // namespace udpdemo::dict


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


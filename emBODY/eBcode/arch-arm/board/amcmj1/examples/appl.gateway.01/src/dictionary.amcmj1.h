

/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __DICTIONARY_AMCMJ1_H_
#define __DICTIONARY_AMCMJ1_H_

#if 0

# TETHER dictionary for amcmj1: catalog

Implemented in `dictionary.amcmj1.cpp`, declared in `dictionary.amcmj1.h`.

- Dictionary: `amcmj1::dict::dictionary`
- Typed handles of the variables: `amcmj1::vars::v<Name>` (e.g. `amcmj1::vars::vPaxCfg`)

## Variables (ID 0x01-0x9F)

The access is the one seen by the host (RPC). Every variable can be streamed.

| ID   | Name            | Type                 | Access | Notes                                                |
|------|-----------------|----------------------|--------|------------------------------------------------------|
| 0x01 | IDtimeoflife    | `uint64_t`           | RO     | computed on read                                     |
| 0x02 | IDdummyUINT32   | `uint32_t`           | RW     | a scalar                                             |
| 0x03 | IDdummySTRUCT   | `dummy::DummySTRUCT` | RW     | a struct with an array                               |
| 0x04 | IDpaxCFG        | `pax::PAXcfg`        | RO     | changes only with OPpaxINIT                          |
| 0x05 | IDpaxSTATUS     | `pax::PAXstatus`     | RO     | written by ctrl thread                               |
| 0x06 | IDpaxVALUE      | `pax::PAXvalue`      | RO     | force / torque of the PX3Q, written by ctrl thread   |
| 0x07 | IDdummyARRAY    | `float[3]`           | RW     | an array of scalars (no typed handle)                |
| 0x08 | IDdummyMODE     | `dummy::DummyMODE`   | RW     | an enum, also changed by OPsetMode                   |
| 0x09 | IDdummyCLOCK    | `dummy::DummyCLOCK`  | RO     | a struct computed on read                            |

## Operations (ID 0xA0-0xDF)

They run in the RPC thread.

| ID   | Name           | Argument                 | Result                          |
|------|----------------|--------------------------|---------------------------------|
| 0xC0 | OPmanageLED    | `LEDcommand {led, on}`   | -                               |
| 0xC1 | OPpaxINIT      | `pax::PAXcfg`            | -                               |
| 0xC2 | OPpaxDEINIT    | -                        | -                               |
| 0xC3 | OPpaxSTART     | -                        | -                               |
| 0xC4 | OPpaxSTOP      | -                        | -                               |
| 0xC5 | OPgetUptime    | -                        | `uint32_t` "seconds"            |
| 0xC6 | OPreadRegister | `uint8_t` "address"      | `uint32_t` "value"              |
| 0xC7 | OPsetMode      | `DummyMODE` "mode"       | -                               |
| 0xC8 | OPgetClock     | -                        | `DummyCLOCK`                    |
| 0xC9 | OPadd          | `Operands {a, b}`        | `int32_t` "sum"                 |
| 0xCA | OPdivide       | `Operands {a, b}`        | `Division {quotient, remainder}`|
| 0xCB | OPaverage      | `float[3]` "values"      | `float` "mean"                  |

`LEDcommand`, `Operands` and `Division` are defined in `dictionary.amcmj1.cpp`.
    
#endif


#include "embot_core.h"
#include "embot_app_eth_TETHER.h"
#include "embot_app_eth_SharedVariables.h"
#include "amcmj1_paxdriver.h"           // amcmj1::pax: PAXcfg, PAXstatus, PAXvalue, State, Command
#include "amcmj1_dummy.h"               // amcmj1::dummy: DummySTRUCT, DummyMODE, DummyCLOCK

#if 0

the Dictionary of amcmj1: the variables and the operations that the host (tether.py) sees, and how the firmware accesses them.
this is the file to include: the Dictionary is defined in dictionary.amcmj1.cpp, where one adds variables and operations.

- amcmj1::dict::dictionary    the constexpr Dictionary, to give to theTETHERservice::Config
- amcmj1::vars::vXxx          the typed handles of the variables, to read and write them w/ theTETHERservice::sharedvariables():
                                 sd.read(amcmj1::vars::vPaxCfg, cfg);   sd.write(amcmj1::vars::vDummyUINT32, v, Origin::local);

#endif


// ---- the typed handles of the variables. the types of the variables are in amcmj1_paxdriver.h (pax) and amcmj1_dummy.h (dummy)
//      the memory of the variables, their fields and their table are in dictionary.amcmj1.cpp

namespace amcmj1::vars {

    namespace tether = embot::app::eth::tether;

    // the typed handles: whoever reads or writes a variable w/ theTETHERservice::sharedvariables() gives its handle, so it cannot
    // use a wrong type, e.g. shared().write(amcmj1::vars::vPaxCfg, cfg, ...). the number is the ID the host uses
    inline constexpr tether::Var<uint64_t> vTimeoflife {0x01};                    // RO, computed on read
    inline constexpr tether::Var<uint32_t> vDummyUINT32 {0x02};                   // RW, a scalar
    inline constexpr tether::Var<dummy::DummySTRUCT> vDummySTRUCT {0x03};         // RW, a struct w/ an array
    inline constexpr tether::Var<pax::PAXcfg> vPaxCfg {0x04};                     // RO for rpc: it changes only w/ OPpaxINIT
    inline constexpr tether::Var<pax::PAXstatus> vPaxStatus {0x05};               // RO for rpc: written by ctrl
    inline constexpr tether::Var<pax::PAXvalue> vPaxValue {0x06};                 // RO for rpc: force / torque of the PX3Q, w/ arrays
    inline constexpr tether::Var<float[3]> vDummyARRAY {0x07};                    // RW, an array of scalars
    inline constexpr tether::Var<dummy::DummyMODE> vDummyMODE {0x08};             // RW, an enum: changed by OPsetMode
    inline constexpr tether::Var<dummy::DummyCLOCK> vDummyCLOCK {0x09};           // RO, a struct computed on read

} // namespace amcmj1::vars


// ---- the Dictionary: defined in dictionary.amcmj1.cpp and checked there by TETHER_VALIDATE().
//      theTETHERservice::Config takes its reference: constexpr theTETHERservice::Config c {amcmj1::dict::dictionary, ...}

namespace amcmj1::dict {

    extern const embot::app::eth::tether::Dictionary dictionary;

} // namespace amcmj1::dict


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


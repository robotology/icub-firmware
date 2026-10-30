

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
| 0x05 | IDpaxSTATUS     | `pax::PAXstatus`     | RO     | written by the gateway thread                               |
| 0x06 | IDpaxVALUE      | `pax::PAXvalue`      | RO     | torque Mx My Mz of the PX3Q, written by the gateway thread  |
| 0x07 | IDdummyARRAY    | `float[3]`           | RW     | an array of scalars (no typed handle)                |
| 0x08 | IDdummyMODE     | `dummy::DummyMODE`   | RW     | an enum, also changed by OPsetMode                   |
| 0x09 | IDdummyCLOCK    | `dummy::DummyCLOCK`  | RO     | a struct computed on read                            |
| 0x0A | IDcubeCFG       | `cube::CUBEcfg`      | RO     | changes only with OPcubeINIT                         |
| 0x0B | IDcubeSTATUS    | `cube::CUBEstatus`   | RO     | written by the gateway thread                               |
| 0x0C | IDcubeSTATE     | `cube::CUBEstate`    | RO     | pos, vel, current, temp, error of the CubeMars       |
| 0x0D | IDcubeMIT       | `cube::CUBEmit`      | RO     | the last MIT accepted (after the limits): a copy of what the command packets carry. it is not a way to write it |
| 0x0E | IDcubeSERVO     | `cube::CUBEservo`    | RO     | the last servo setpoint, written by OPcubeSERVO      |
| 0x0F | IDcubeLIMITS    | `gateway::GWlimits`  | RO     | the limits applied to the MIT of the command packets, changed by OPcubeLIMITS |
| 0x10 | IDpaxTORQUE     | `gateway::PAXtorque` | RO     | the three torques of the PX3Q, sent to the Speedgoat in the state frame |
| 0x11 | IDmotorSTATE    | `mot::MOTORstate`    | RO     | the macromotor (amc.mot on the CM7): current, velocity, position, pwm, faults, control mode. emulated until the ICC |
| 0x12 | IDgwSTATS       | `gateway::GWstats`   | RO     | counters of the command packets: seq echo, clamped, rejected, lost ... |
| 0x13 | IDgwLOAD        | `gateway::GWload`    | RO     | load of tGATEWAY: wakes per second, max CAN frames per drain, longest pass |
| 0x14 | IDmotorPIDS     | `mot::MOTORpids`     | RO     | the current and velocity PIDs given by OPmotorPIDS (rpc socket 5002) |
| 0x15 | IDmotorSETUP    | `mot::MOTORsetup`    | RO     | the current limits and the motor config given by OPmotorSETUP (rpc socket 5002) |
| 0x16 | IDmotorTARGET   | `mot::MOTORtarget`   | RO     | the last setpoint used, from the command packets (set<IDmotorTARGET>) |
| 0x17 | IDmotorSTATUS   | `mot::MOTORstatus`   | RO     | counters of the macromotor: setpoints used and rejected, watchdogs, modes accepted and refused |
| 0x18 | IDposSTATE      | `pos::POSstate`      | RO     | the position sensor of the joint (SPI encoder, read every 1 ms; emulated until it is wired), sent in the state frame |

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
| 0xCC | OPcubeINIT     | `cube::CUBEcfg`          | -                               |
| 0xCD | OPcubeDEINIT   | -                        | -                               |
| 0xCE | OPcubeSTART    | -                        | -                               |
| 0xCF | OPcubeSTOP     | -                        | -                               |
| 0xD0 | OPcubeSERVO    | `cube::ServoCmd`         | -                               |
| 0xD1 | OPcubeENABLE   | -                        | -                               |
| 0xD2 | OPcubeDISABLE  | -                        | -                               |
| 0xD3 | OPcubeLIMITS   | `gateway::GWlimits`      | -                               |
| 0xD4 | OPpaxENABLE    | -                        | -                               |
| 0xD5 | OPpaxDISABLE   | -                        | -                               |

Served only on the rpc socket of the gateway (5002), by tGATEWAY, not by TETHER: they are not in the Dictionary.

| ID   | Name           | Argument                 | Result                          |
|------|----------------|--------------------------|---------------------------------|
| 0xD6 | OPmotorPIDS    | `mot::MOTORpids`         | - (nak if not idle)             |
| 0xD7 | OPmotorSETUP   | `mot::MOTORsetup`        | - (nak if not idle)             |
| 0xD8 | OPmotorMODE    | `uint8_t` control mode   | - (nak w/out pids and setup, or in EXTFAULT if not idle) |
| 0xD9 | OPgwFAULT      | `uint8_t` 1 set, 0 clear | - set: the CubeMars disabled, the macromotor idle, then both refused until clear |

`LEDcommand`, `Operands` and `Division` are defined in `dictionary.amcmj1.cpp`.
    
#endif


#include "embot_core.h"
#include "embot_app_eth_TETHER.h"
#include "embot_app_eth_SharedVariables.h"
#include "amcmj1_paxdriver.h"           // amcmj1::pax: PAXcfg, PAXstatus, PAXvalue, State, Command
#include "amcmj1_cubedriver.h"         // amcmj1::cube: CUBEcfg, CUBEstatus, CUBEstate, CUBEmit, CUBEservo, ServoCmd
#include "amcmj1_gwlink.h"             // amcmj1::gateway: GWlimits, GWstats, GWload, MOTORstate, PAXtorque
#include "amcmj1_motdriver.h"           // amcmj1::mot: MOTORpids, MOTORsetup, MOTORtarget, MOTORstate, MOTORstatus
#include "amcmj1_posdriver.h"           // amcmj1::pos: POSstate
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
    inline constexpr tether::Var<pax::PAXstatus> vPaxStatus {0x05};               // RO for rpc: written by tGATEWAY
    inline constexpr tether::Var<pax::PAXvalue> vPaxValue {0x06};                 // RO for rpc: force / torque of the PX3Q, w/ arrays
    inline constexpr tether::Var<float[3]> vDummyARRAY {0x07};                    // RW, an array of scalars
    inline constexpr tether::Var<dummy::DummyMODE> vDummyMODE {0x08};             // RW, an enum: changed by OPsetMode
    inline constexpr tether::Var<dummy::DummyCLOCK> vDummyCLOCK {0x09};           // RO, a struct computed on read
    inline constexpr tether::Var<cube::CUBEcfg> vCubeCfg {0x0A};                  // RO for rpc: it changes only w/ OPcubeINIT
    inline constexpr tether::Var<cube::CUBEstatus> vCubeStatus {0x0B};            // RO for rpc: written by tGATEWAY
    inline constexpr tether::Var<cube::CUBEstate> vCubeState {0x0C};              // RO for rpc: what the CubeMars tells, written by tGATEWAY
    inline constexpr tether::Var<cube::CUBEmit> vCubeMIT {0x0D};                  // RO for rpc: the last MIT accepted from the command packets
    inline constexpr tether::Var<cube::CUBEservo> vCubeServo {0x0E};              // RO for rpc: written by OPcubeSERVO
    inline constexpr tether::Var<gateway::GWlimits> vCubeLimits {0x0F};           // RO for rpc: changed only w/ OPcubeLIMITS
    inline constexpr tether::Var<gateway::PAXtorque> vPaxTorque {0x10};           // RO for rpc: Mx My Mz of the PX3Q for the state frame
    inline constexpr tether::Var<gateway::MOTORstate> vMotorState {0x11};         // RO for rpc: the macromotor (amc.mot on the CM7)
    inline constexpr tether::Var<gateway::GWstats> vGwStats {0x12};               // RO for rpc: counters of the command packets
    inline constexpr tether::Var<gateway::GWload> vGwLoad {0x13};                 // RO for rpc: load of the gateway thread
    inline constexpr tether::Var<mot::MOTORpids> vMotorPids {0x14};               // RO for rpc: the PIDs of the macromotor (OPmotorPIDS)
    inline constexpr tether::Var<mot::MOTORsetup> vMotorSetup {0x15};             // RO for rpc: its limits and config (OPmotorSETUP)
    inline constexpr tether::Var<mot::MOTORtarget> vMotorTarget {0x16};           // RO for rpc: its last setpoint used
    inline constexpr tether::Var<mot::MOTORstatus> vMotorStatus {0x17};           // RO for rpc: its counters
    inline constexpr tether::Var<pos::POSstate> vPosState {0x18};                 // RO for rpc: the position sensor of the joint

} // namespace amcmj1::vars


// ---- the Dictionary: defined in dictionary.amcmj1.cpp and checked there by TETHER_VALIDATE().
//      theTETHERservice::Config takes its reference: constexpr theTETHERservice::Config c {amcmj1::dict::dictionary, ...}

namespace amcmj1::dict {

    extern const embot::app::eth::tether::Dictionary dictionary;

} // namespace amcmj1::dict


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


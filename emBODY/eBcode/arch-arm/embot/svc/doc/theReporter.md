# How to use `theReporter`

`embot::svc::theReporter` is the place where the board reports what goes wrong, and what is worth knowing. Any module
calls `report()`, also from an ISR, and goes on: it never blocks. The application decides where the reports go
(the sinks): by default they are printed w/ `embot::core::print()`, so they reach the ITM and `theLogger`.

On top of that it merges the reports that repeat, keeps the last ones for TETHER, and keeps the record of a fatal
across the reset.

The code: `embot/svc/embot_svc_theReporter.h/.cpp`. In a project: the include path `embot/svc` and
`embot_svc_theReporter.cpp`. It needs `embot::os` and `embot::os::rtos::interruptid()` (in the `embot/os` of this
branch). It does not need the network.

The header has many options. Most applications need only the first three sections below.

## 1. The minimum: init and report

```cpp
#include "embot_svc_theReporter.h"

// the defaults: print sink ON, queue of 16, merge in windows of 1 s, hang on a fatal, no record across the reset
constexpr embot::svc::theReporter::Config reporterconfig {};

void initSystem(embot::os::Thread *t, void* initparam)
{
    embot::svc::theReporter::getInstance().initialise(reporterconfig);     // first: it does not need the network
    // ... theLogger, theIPservice, the other services and threads
}
```

Then, from any thread:

```cpp
using embot::svc::theReporter;

theReporter::getInstance().report(theReporter::Severity::error, theReporter::Descriptor {0x00030002, 2, 0},
                                  "no reply from the CubeMars", "canhandler");
```

and the print sink writes one line:

```text
[error] S12:m345:u678 (tCAN, canhandler) code 0x00030002 dev 0 addr 0 par16 0x0002 par64 0x0000000000000000: no reply from the CubeMars
```

- `Severity`: `trace`, `info`, `debug`, `warning`, `error`, `fatal`. The values are those of YARP (iCub diagnostics).
- `Descriptor {code, par16, par64}` or `Descriptor {code, sourcedevice, sourceaddress, par16, par64}`: the fields of the
  iCub diagnostics. Use 0 where there is nothing to say. theReporter gives no meaning to the code.
- the text (optional) is copied, at most 79 chars: a longer one ends w/ `~`. The object (optional) must be a literal.
- the time, the thread and (in an ISR) the interrupt are taken by `report()` itself.

## 2. Report from an ISR

The same call. Skip the text: it is not forbidden, but in an ISR the less work the better.

```cpp
void onDMAcomplete()                // an IRQ handler, e.g. the FOC
{
    if(current > limit)
    {
        theReporter::getInstance().report(theReporter::Severity::error, theReporter::Descriptor {codeOVERCURRENT, phase, current});
    }
}
```

The report has no thread and `Report::interrupt` = the exception number (the print sink shows `(isr 37, -)`). It is
delivered later by the thread of the reporter.

## 3. Your own sink

A sink is a function `void f(const theReporter::Report &r, void *param)` w/ a minimum severity. It runs in the thread
of the reporter, one report at a time: it can format, send, blink. It must not call `report()`.

```cpp
namespace amc::diag {

    // e.g. the diagnostics of iCub toward yarprobotinterface
    void toicub(const embot::svc::theReporter::Report &r, void *param)
    {
        // r.severity, r.descriptor (code, sourcedevice, sourceaddress, par16, par64), r.time, r.text
        // r.repeated > 1: it stands for r.repeated same reports (see 4.)
        // r.dropped > 0: so many reports were lost before this one
        // r.previousrun: the fatal of the previous run (see 6.)
    }

}

// in the init thread, after initialise()
uint8_t id {0};
embot::svc::theReporter::getInstance().attach(
    embot::svc::theReporter::Sink {amc::diag::toicub, nullptr, embot::svc::theReporter::Severity::warning}, id);
```

Up to 4 sinks; the print sink takes the first one (id 0) unless `Print::OFF`. `detach(id)` removes one.

## 4. The reports that repeat

An error that happens at every cycle (a CAN timeout at 1 kHz) would flood the sinks. So two reports w/ the same
severity and the same `Descriptor` (the text does not count) are merged within a window (`Config::floodwindow`,
1 s by default):

- the first one is delivered at once
- the same ones within the window are only counted
- at the end of the window one more is delivered, w/ `repeated` = how many were merged

```text
[error] S12:m345:u678 (tCAN, canhandler) code 0x00030002 ... par64 0x0000000000000000: no reply from the CubeMars
[error] S13:m344:u912 (tCAN, canhandler) code 0x00030002 ... par64 0x0000000000000000 x999: no reply from the CubeMars
```

Put what changes at every report (a reading, a counter) in the text, not in `par16` / `par64`, or the reports will
never be the same. `floodwindow = 0` delivers everything.

## 5. The fatal and the safe state

`fatal()` (or `report()` w/ `Severity::fatal`) does not return:

1. it writes the record of the fatal (see 6.)
2. it calls `Config::safestate()`, if given: the PWM off, the motors in idle. No RTOS calls in it
3. from a thread: the sinks have the report at once, in the thread that reports, then it waits `Config::fatalgrace`
   (100 ms) so that theLogger and theIPservice can send it. From an ISR: no sinks
4. `Config::onfatal`: `hang` (forever: the debugger finds it there) or `reset`

```cpp
void motorsoff()
{
    // e.g. disable the PWM timer, w/ registers or w/ embot::hw, w/out the RTOS
}

constexpr embot::svc::theReporter::Config reporterconfig
{
    embot::svc::theReporter::Print::ON,                         // the print sink ...
    embot::svc::theReporter::Severity::trace,                   // ... w/ every report
    16,                                                         // the queue: reports waiting for the thread
    embot::core::time1second,                                   // the window of the merge (0: no merge)
    8,                                                          // different reports followed by the merge
    embot::svc::theReporter::OnFatal::reset,                    // hang or reset
    motorsoff,                                                  // the safe state (nullptr: none)
    100*embot::core::time1millisec,                             // time left to the sinks on a fatal
    embot::core::Data {},                                       // the record across the reset: none (see 6.)
    embot::os::Thread::Props {embot::os::Priority::low12, 2*1024}
};

// somewhere
theReporter::getInstance().fatal(theReporter::Descriptor {codeENCODERLOST, 0, 0}, "the encoder is gone", "motor");
```

## 6. The fatal survives the reset

The record of a fatal goes in a buffer of the application in a RAM that the startup does not clear. At the next boot
the thread of the reporter delivers it as its first report, w/ `Report::previousrun = true`, to all the sinks attached
in the init thread.

The buffer:

```cpp
alignas(8) uint8_t fatalmemory[embot::svc::theReporter::fatalmemorysize] __attribute__((section(".bss.noinit")));
```

and in the `Config`: `embot::core::Data {fatalmemory, sizeof(fatalmemory)}`. In the scatter file (`.sct`) a region
for the section, not initialised: an address and a size (at least 256 bytes) in a RAM of the core, out of the other
regions and not used by the other core:

```text
  RW_NOINIT <address> UNINIT <size> {
    *(.bss.noinit)
  }
```

W/ a linker script (ATfE, GNU ld / ld.lld) the same is a memory region and a section `NOLOAD`. The section must come
before the one of `.bss`: the first rule that matches wins, and `*(.bss .bss.*)` would take `.bss.noinit` and zero it.

```text
MEMORY
{
    RAM         (rwx) : ORIGIN = ..., LENGTH = ...           /* 256 bytes less than before */
    NOINIT      (rw)  : ORIGIN = <address>, LENGTH = 0x100
}

SECTIONS
{
    /* ... .data ... */

    .noinit (NOLOAD) :
    {
        KEEP(*(.bss.noinit))
    } > NOINIT

    .bss (NOLOAD) :
    {
        *(.bss .bss.* COMMON)
    } > RAM
}
```

`board/amcmj1/examples/amcmj1-svc-demo` does both: `cfg/amcmj1.2cm4-lwip-0offset.sct` and `.ld` take the 256 bytes
below the hole of the IPC (0x1003FEC0), and `src/svc-demo.cpp` has the buffer, the HardFault handler and a fault on
demand (`enableFAULT`).

At the boot:

```text
[fatal] previous run S5:m12:u300 (tCTRL, motor) code 0x0000BEEF ...: the encoder is gone
```

`previousfatal(r)` gives it too, at any time. A power-on leaves random bytes in the buffer: a signature and a CRC
tell a true record from garbage.

## 7. The faults of the cpu

A HardFault (a null pointer, a bad access) is a fatal too. The fault handler gives the registers to the static
`fatal(Fault, ...)`, which uses only its arguments (no RTOS, no heap, also before `initialise()`). On Cortex-M (the
startup of the board has `HardFault_Handler` weak: this definition replaces it):

```cpp
extern "C" void HardFault_C(const uint32_t *frame)        // the stacked r0, r1, r2, r3, r12, lr, pc, xpsr
{
    constexpr uintptr_t CFSR {0xE000ED28}, HFSR {0xE000ED2C}, MMFAR {0xE000ED34}, BFAR {0xE000ED38};
    const uint32_t cfsr {*reinterpret_cast<volatile uint32_t *>(CFSR)};
    const uint32_t hfsr {*reinterpret_cast<volatile uint32_t *>(HFSR)};
    const uint32_t address {(0 != (cfsr & 0x0080)) ? *reinterpret_cast<volatile uint32_t *>(MMFAR) :
                            (0 != (cfsr & 0x8000)) ? *reinterpret_cast<volatile uint32_t *>(BFAR) : 0};
    const embot::svc::theReporter::Fault fault {embot::svc::theReporter::CPU::cortexm, frame[6], frame[5], frame[7], cfsr, hfsr, address};
    embot::svc::theReporter::fatal(fault, embot::core::Data {fatalmemory, sizeof(fatalmemory)},
                                   embot::svc::theReporter::OnFatal::reset, motorsoff);
}

extern "C" __attribute__((naked)) void HardFault_Handler()
{   // the frame is on the stack in use when the fault happened: MSP or PSP, as bit 2 of LR (EXC_RETURN) tells
    __asm volatile
    (
        "tst lr, #4         \n"
        "ite eq             \n"
        "mrseq r0, msp      \n"
        "mrsne r0, psp      \n"
        "b HardFault_C      \n"
    );
}
```

At the next boot the report carries the registers: the print sink adds `pc 0x... ret 0x... status 0x... cause 0x...`.
With the `.axf` and the pc, uVision (or `fromelf`) shows the line of code.

## 8. On TETHER

`snapshot(s)` fills a `theReporter::Snapshot`: the counters per severity, how many merged and lost, and the last 8
reports (text cut to 23 chars). It is a plain struct of 488 bytes: it can be the value of a view, like the one of the
latency in amcmj1-svc-demo (`IDviewof_latency`, see `amcmj1-svc-demo/docs/howto-add-a-view.md`). The function that
reads the view calls `theReporter::getInstance().snapshot(s)`.

`stats()` gives only the counters.

## Notes

- `report()` returns false if the reporter is not initialised, or if the queue is full: then the report is lost and
  counted (`Stats::dropped`), and the next report delivered says how many (`Report::dropped`). A queue often full:
  a bigger `queuecapacity` (rounded up to a power of 2), or fewer reports.
- the reports go through the queue also from a thread: a thread of control never pays the cost of a sink.
- the thread of the reporter runs at `low12` by default, above theLogger (`low10`): a printed report reaches the buffer
  of the logger at once.
- the queue uses lock-free atomics: fine on Cortex-M3 and up. A cpu w/out them (a RISC-V w/out the A extension) will
  need a short critical section of `embot::os::rtos` in its place.
- `embot::app::eth::theErrorManager` stays as it is. theReporter is its successor for new code.
- the test on a PC: `test/host/test_reporter.cpp` (how to build it is in its first lines).

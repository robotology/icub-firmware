/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_SVC_THEREPORTER_H_
#define __EMBOT_SVC_THEREPORTER_H_

#include "embot_core.h"
#include "embot_os_Thread.h"
#include <memory>
#include <string>
#include <type_traits>

#if 0

theReporter: the board reports what goes wrong (and what is worth knowing) in one place, and the application decides
where the reports go. it does what embot::app::eth::theErrorManager does (severity, descriptor, text, a function of the
application) and adds what a board in the field needs:

- report() never blocks and can be called also from an ISR
- several sinks, each one w/ its own minimum severity: print() is the default one
- the same report repeated many times is merged into one, w/ the number of repetitions
- the last reports and the counters are kept, and given as a snapshot that can travel on TETHER as a view
- a fatal leaves a record in a RAM that survives the reset: at the next boot it is the first report
- what to do on a fatal is decided in one place: the safe state of the application, then hang or reset

the report
- Severity has the values of YARP (the iCub diagnostics): they cannot change. Descriptor has the fields of the iCub
  diagnostics (eOerrmanDescriptor_t), so a sink toward yarprobotinterface maps it 1:1. theReporter gives no meaning
  to the code: it is used only to merge the repeated reports
- the text is optional. it is copied into the report, at most maxtextlength chars: a longer one is cut and its last
  char becomes '~'. no heap
- the time, the thread (nullptr in an ISR) and the interrupt (0 in a thread) are taken by report() itself. the
  interrupt comes from embot::os::rtos::interruptid(), so theReporter has no code of the cpu: on Cortex-M it is the
  exception number (IPSR), on RISC-V it will be the cause of the interrupt (mcause). the name of the object is
  optional and must be a string that lives forever (a literal)

the path of a report
- report() reserves a slot in a queue (Config::queuecapacity slots, allocated by initialise()), fills it and wakes up
  the thread of the reporter. the queue accepts many producers (threads and ISRs) and uses no mutex, so it can be used
  in an ISR and a thread is never blocked by another. the reservation of a slot is a few instructions: a
  compare-and-swap of an embot::os::rtos::Atomic, lock-free where the cpu can (Cortex-M3 and up, RISC-V w/ the A
  extension), else a few instructions w/ the interrupts off (a single core). if the queue is full the report is lost and counted
  (Stats::dropped); the next report delivered tells how many were lost
- the thread of the reporter (low priority) takes the reports from the queue, merges the repeated ones and gives each
  one to the sinks whose minimum severity it reaches. so the cost of the sinks (formatting, network) is paid by the
  thread of the reporter, never by the thread that reports, nor by an ISR
- a report from before initialise() is lost (it returns false)

the merge of the repeated reports (Config::floodwindow > 0)
- two reports are the same if they have the same severity and the same Descriptor (code, sourcedevice,
  sourceaddress, par16, par64). the text does not count. the first one is delivered at once. the same ones that come within floodwindow from it are only
  counted. at the end of the window one more report is delivered: the last one received, w/ Report::repeated = how
  many were merged. Config::floodkeys is how many different reports are followed at the same time
- a fatal is never merged

the sinks
- a sink is a function of the application w/ its parameter and its minimum severity: up to maxsinks. attach() and
  detach() can be called at any time. a sink runs in the thread of the reporter, one report at a time: it may print,
  send a frame, blink a LED. it must not call report() (it would be lost: the reporter does not report itself)
- the print sink (printsink()) is attached by initialise() if Config::print is ON. it calls embot::core::print(), so
  the reports go where print() goes: ITM, theLogger, ... e.g.
      [error] S12:m345:u678 (tCTRL, canhandler) code 0x00030002 par16 0x0002 par64 0x0000000000000000: no reply ...

the memory of what happened
- stats(): how many reports per severity, how many merged, how many lost
- snapshot(): the counters and the last Snapshot::historycapacity reports delivered, w/ the text cut to
  Snapshot::textcapacity. it is trivially copyable, standard layout and at most 512 bytes: it can be the value of a
  view of TETHER (as in amcmj1-svc-demo: IDviewof_latency)

the fatal
- report(Severity::fatal, ...) or fatal(...) do not return. in this order:
  1. the record of the fatal is written in Config::fatalmemory (if given): signature, time, descriptor, thread, IRQ,
     object, text and, from a fault handler, the registers of the fault (see Fault)
  2. Config::safestate() is called (if given): it must bring the hw in a safe state w/out the RTOS (e.g. PWM off)
  3. from a thread: the report goes directly to the sinks (in the thread that reports), then the reporter waits
     Config::fatalgrace so that the sinks that send over the network can do it. from an ISR or a fault: no sinks
  4. Config::onfatal: hang (forever, w/ the interrupts on: good for the debugger) or reset
- fatal(const Fault &, ...) is for the fault handlers of the cpu (Cortex-M: HardFault, MemManage, BusFault,
  UsageFault; RISC-V: the trap handler of the exceptions): 1, 2 and 4 only. the handler fills Fault w/ the registers
  of its cpu: theReporter only stores them
- at the next boot initialise() finds the record and clears it. the thread of the reporter delivers it as its first
  report (w/ Report::previousrun = true and, in Report::fault, the registers if it came from a fault) when it runs for
  the first time, i.e. after the init thread: so also the sinks attached in the init thread receive it. also
  previousfatal() gives it
- Config::fatalmemory (an embot::core::Data) is a buffer of the application of at least fatalmemorysize bytes, in a region of RAM that is
  not initialised at startup and not used by others (in the scatter file: a section UNINIT). e.g.
      alignas(8) uint8_t fatalmemory[embot::svc::theReporter::fatalmemorysize] __attribute__((section(".bss.noinit")));
  w/ in the .sct:   RW_NOINIT <address> UNINIT <size> { *(.bss.noinit) }
  a power-on leaves random values in it: the signature and a CRC tell a true record from garbage

call initialise() once from the init thread, as early as possible: theReporter does not need the network.

#endif

namespace embot::svc {

    class theReporter
    {
    public:
        static theReporter& getInstance();

        // the values of YARP: they cannot change
        enum class Severity : uint8_t { trace = 0, info = 1, debug = 2, warning = 3, error = 4, fatal = 5 };
        static constexpr size_t numberofseverities {6};

        static constexpr const char * to_cstring(Severity s)
        {
            constexpr const char * names[numberofseverities] {"trace", "info", "debug", "warning", "error", "fatal"};
            return (embot::core::tointegral(s) < numberofseverities) ? names[embot::core::tointegral(s)] : "none";
        }

        enum class Print : uint8_t { OFF = 0, ON = 1 };
        enum class OnFatal : uint8_t { hang = 0, reset = 1 };

        static constexpr size_t maxtextlength {79};             // chars of the text of a report, w/out the final '\0'
        static constexpr size_t maxsinks {4};
        static constexpr size_t fatalmemorysize {256};          // bytes of Config::fatalmemory

        // the fields of the iCub diagnostics (eOerrmanDescriptor_t)
        struct Descriptor
        {
            uint32_t code {0};
            uint8_t sourcedevice {0};
            uint8_t sourceaddress {0};
            uint16_t par16 {0};
            uint64_t par64 {0};

            constexpr Descriptor() = default;
            constexpr explicit Descriptor(uint32_t c, uint16_t p16, uint64_t p64)
                : code(c), sourcedevice(0), sourceaddress(0), par16(p16), par64(p64) {}
            constexpr explicit Descriptor(uint32_t c, uint8_t sd, uint8_t sa, uint16_t p16, uint64_t p64)
                : code(c), sourcedevice(sd), sourceaddress(sa), par16(p16), par64(p64) {}
        };

        // the registers saved by a fault handler. the fields are generic, their meaning depends on the cpu
        enum class CPU : uint8_t { none = 0, cortexm = 1, riscv = 2 };
        struct Fault
        {
            CPU cpu {CPU::none};
            uint32_t pc {0};            // where it happened              Cortex-M: stacked PC     RISC-V: mepc
            uint32_t ret {0};           // the return address             Cortex-M: stacked LR     RISC-V: ra
            uint32_t status {0};        // the state of the cpu           Cortex-M: stacked xPSR   RISC-V: mstatus
            uint32_t cause {0};         // why                            Cortex-M: SCB->CFSR      RISC-V: mcause
            uint32_t detail {0};        // more about why                 Cortex-M: SCB->HFSR      RISC-V: 0
            uint32_t address {0};       // the faulting address, if any   Cortex-M: MMFAR / BFAR   RISC-V: mtval

            constexpr Fault() = default;
            constexpr explicit Fault(CPU c, uint32_t p, uint32_t r, uint32_t s, uint32_t ca, uint32_t d, uint32_t a)
                : cpu(c), pc(p), ret(r), status(s), cause(ca), detail(d), address(a) {}
        };

        // what a sink receives
        struct Report
        {
            embot::core::Time time {0};             // when report() was called
            Severity severity {Severity::trace};
            Descriptor descriptor {};
            const char *thread {nullptr};           // the name of the thread that reported. nullptr: from an ISR
            const char *object {nullptr};           // the object that reported, if given
            uint16_t interrupt {0};                 // 0: from a thread. else embot::os::rtos::interruptid()
            bool previousrun {false};               // true: the fatal that ended the previous run
            uint32_t repeated {1};                  // how many same reports this one stands for (see the merge)
            uint32_t dropped {0};                   // reports lost before this one, because the queue was full
            Fault fault {};                         // only for a fatal from a fault handler
            char text[maxtextlength+1] {0};

            constexpr Report() = default;
        };

        using fpSink = void (*)(const Report &report, void *param);
        // it brings the hw in a safe state. it is called w/ the RTOS maybe not running: no RTOS calls
        using fpSafeState = void (*)();

        struct Sink
        {
            fpSink fn {nullptr};
            void *param {nullptr};
            Severity minimum {Severity::trace};     // it receives the reports w/ severity >= minimum

            constexpr Sink() = default;
            constexpr explicit Sink(fpSink f, void *p, Severity m) : fn(f), param(p), minimum(m) {}
            constexpr bool isvalid() const { return nullptr != fn; }
        };

        struct Config
        {
            Print print {Print::ON};                                        // ON: initialise() attaches printsink()
            Severity printminimum {Severity::trace};                        // ... w/ this minimum severity
            uint8_t queuecapacity {16};                                     // reports waiting for the thread
            embot::core::relTime floodwindow {embot::core::time1second};    // the merge of the repeated reports. 0: none
            uint8_t floodkeys {8};                                          // different reports followed by the merge
            OnFatal onfatal {OnFatal::hang};
            fpSafeState safestate {nullptr};                                // called first on a fatal, if not nullptr
            embot::core::relTime fatalgrace {100*embot::core::time1millisec};   // time for the sinks on a fatal
            embot::core::Data fatalmemory {};                               // {nullptr, 0}: no record of the fatal
            embot::os::Thread::Props thread {embot::os::Priority::low12, 2*1024};

            constexpr Config() = default;
            constexpr explicit Config(Print p, Severity pm, uint8_t qc, embot::core::relTime fw, uint8_t fk, OnFatal of,
                                      fpSafeState ss, embot::core::relTime fg, const embot::core::Data &fm, const embot::os::Thread::Props &t)
                : print(p), printminimum(pm), queuecapacity(qc), floodwindow(fw), floodkeys(fk), onfatal(of),
                  safestate(ss), fatalgrace(fg), fatalmemory(fm), thread(t) {}

            // fatalmemory: either none or at least fatalmemorysize bytes
            constexpr bool isvalid() const
            {
                const bool nomemory {(nullptr == fatalmemory.pointer) && (0 == fatalmemory.capacity)};
                const bool memory {(nullptr != fatalmemory.pointer) && (fatalmemory.capacity >= fatalmemorysize)};
                return (queuecapacity >= 4) && (floodkeys >= 1) && thread.isvalid() && (nomemory || memory);
            }
        };

        struct Stats
        {
            uint32_t reports[numberofseverities] {0};   // received, per severity (also the merged ones)
            uint32_t merged {0};                        // received but not delivered: counted in a later report
            uint32_t dropped {0};                       // lost: the queue was full

            constexpr Stats() = default;
        };

        // the counters and the last reports delivered, in a fixed layout: it can travel on TETHER as a view
        struct Snapshot
        {
            static constexpr size_t historycapacity {8};
            static constexpr size_t textcapacity {24};      // w/ the final '\0'

            struct Entry
            {
                uint64_t time;              // us
                uint64_t par64;
                uint32_t code;
                uint32_t repeated;
                uint16_t par16;
                uint8_t severity;           // Severity
                uint8_t sourcedevice;
                uint8_t sourceaddress;
                uint8_t interrupt;          // 0: from a thread. else Report::interrupt, 255 if bigger
                char text[textcapacity];
            };

            uint32_t reports[numberofseverities];
            uint32_t merged;
            uint32_t dropped;
            uint8_t number;                 // valid entries
            uint8_t reserved[7];
            Entry entries[historycapacity]; // [0] is the most recent
        };

        static_assert(std::is_trivially_copyable_v<Snapshot> && std::is_standard_layout_v<Snapshot>, "Snapshot must be plain");
        static_assert(sizeof(Snapshot) <= 512, "Snapshot must fit a ROP of TETHER");

        // it allocates the queue, creates the thread, attaches printsink() if asked and takes the record of a fatal from
        // Config::fatalmemory, if there is one (the thread delivers it). false if the Config is not valid or if already initialised
        bool initialise(const Config &config);
        bool initialised() const;

        // from a thread or an ISR. it never blocks. false if not initialised or if the queue is full.
        // w/ Severity::fatal it does not return (see fatal())
        bool report(Severity severity, const Descriptor &descriptor, const char *text = nullptr, const char *object = nullptr);
        bool report(Severity severity, const Descriptor &descriptor, const std::string &text, const char *object = nullptr);

        // the fatal: the record, the safe state, the sinks (only from a thread), then Config::onfatal
        [[noreturn]] void fatal(const Descriptor &descriptor, const char *text = nullptr, const char *object = nullptr);
        // from a fault handler: the record (w/ the registers), the safe state, then onfatal. it is static because a
        // fault may happen before initialise() or w/ the heap damaged: it uses only its arguments
        [[noreturn]] static void fatal(const Fault &fault, const embot::core::Data &fatalmemory, OnFatal onfatal, fpSafeState safestate);

        // the sinks. id is in [0, maxsinks). false if no room or not valid
        bool attach(const Sink &sink, uint8_t &id);
        bool detach(uint8_t id);
        // the default sink: one line w/ embot::core::print(). param is not used
        static void printsink(const Report &report, void *param);

        Stats stats() const;
        void snapshot(Snapshot &snapshot) const;
        // the fatal of the previous run, if any (it stays available after initialise() has delivered it)
        bool previousfatal(Report &report) const;

    private:
        theReporter();
        ~theReporter();

    public:
        theReporter(const theReporter&) = delete;
        theReporter& operator=(const theReporter&) = delete;
        theReporter(theReporter&&) = delete;
        theReporter& operator=(theReporter&&) = delete;

        struct Impl;
    private:
        std::unique_ptr<Impl> pImpl;
    };

} // namespace embot::svc


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

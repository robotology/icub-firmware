/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_svc_theReporter.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core_binary.h"
#include "embot_os_rtos.h"
#include "embot_os_theScheduler.h"
#include "embot_hw_sys.h"
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdio>
#include <cstring>

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

namespace embot::svc {

    namespace {

        using R = theReporter;

        // ---- text, w/out heap

        // it copies src into dst (capacity bytes w/ the final '\0'). a longer src is cut and its last char becomes '~'
        void copytext(char *dst, size_t capacity, const char *src)
        {
            if((nullptr == dst) || (0 == capacity))
            {
                return;
            }
            size_t n {0};
            if(nullptr != src)
            {
                while((n < (capacity - 1)) && ('\0' != src[n]))
                {
                    dst[n] = src[n];
                    n++;
                }
                if(('\0' != src[n]) && (n > 0))
                {
                    dst[n-1] = '~';
                }
            }
            dst[n] = '\0';
        }

        // ---- the record of the fatal, in the memory of the application that survives the reset

        // the CRC-32 of IEEE 802.3, bit by bit: the record is small and it is computed once per boot and once per fatal
        uint32_t crc32(const void *data, size_t size)
        {
            const uint8_t *p {static_cast<const uint8_t *>(data)};
            uint32_t crc {0xFFFFFFFFU};
            for(size_t i = 0; i < size; i++)
            {
                crc ^= p[i];
                for(uint8_t b = 0; b < 8; b++)
                {
                    crc = (crc >> 1) ^ ((0 != (crc & 1U)) ? 0xEDB88320U : 0U);
                }
            }
            return ~crc;
        }

        constexpr uint32_t fatalsignature {0xFA7A1EC0};
        constexpr size_t namecapacity {24};

        struct FatalRecord
        {
            uint32_t signature;
            uint32_t crc;                   // of the bytes from time to the end
            uint64_t time;
            uint64_t par64;
            uint32_t code;
            uint16_t par16;
            uint8_t sourcedevice;
            uint8_t sourceaddress;
            uint16_t interrupt;
            uint8_t hasfault;
            uint8_t cpu;
            uint32_t fault[6];              // pc, ret, status, cause, detail, address
            char thread[namecapacity];
            char object[namecapacity];
            char text[R::maxtextlength+1];
        };

        static_assert(std::is_trivially_copyable_v<FatalRecord>, "FatalRecord is copied w/ memcpy");
        static_assert(sizeof(FatalRecord) <= R::fatalmemorysize, "R::fatalmemorysize is too small for FatalRecord");

        constexpr size_t crcoffset {offsetof(FatalRecord, time)};

        // memcpy() to and from the memory of the application: it does not need to be aligned
        void writerecord(const embot::core::Data &memory, const R::Report &report, bool hasfault)
        {
            if((nullptr == memory.pointer) || (memory.capacity < sizeof(FatalRecord)))
            {
                return;
            }
            FatalRecord f {};
            f.signature = fatalsignature;
            f.time = report.time;
            f.par64 = report.descriptor.par64;
            f.code = report.descriptor.code;
            f.par16 = report.descriptor.par16;
            f.sourcedevice = report.descriptor.sourcedevice;
            f.sourceaddress = report.descriptor.sourceaddress;
            f.interrupt = report.interrupt;
            f.hasfault = hasfault ? 1 : 0;
            f.cpu = embot::core::tointegral(report.fault.cpu);
            f.fault[0] = report.fault.pc;
            f.fault[1] = report.fault.ret;
            f.fault[2] = report.fault.status;
            f.fault[3] = report.fault.cause;
            f.fault[4] = report.fault.detail;
            f.fault[5] = report.fault.address;
            copytext(f.thread, sizeof(f.thread), report.thread);
            copytext(f.object, sizeof(f.object), report.object);
            copytext(f.text, sizeof(f.text), report.text);
            const uint8_t *bytes {reinterpret_cast<const uint8_t *>(&f)};
            f.crc = crc32(bytes + crcoffset, sizeof(FatalRecord) - crcoffset);
            std::memcpy(memory.pointer, &f, sizeof(FatalRecord));
        }

        bool readrecord(const embot::core::Data &memory, FatalRecord &f)
        {
            if((nullptr == memory.pointer) || (memory.capacity < sizeof(FatalRecord)))
            {
                return false;
            }
            std::memcpy(&f, memory.pointer, sizeof(FatalRecord));
            if(fatalsignature != f.signature)
            {
                return false;
            }
            const uint8_t *bytes {reinterpret_cast<const uint8_t *>(&f)};
            if(crc32(bytes + crcoffset, sizeof(FatalRecord) - crcoffset) != f.crc)
            {
                return false;
            }
            // the strings come from a RAM that survived a reset: their end is forced
            f.thread[namecapacity-1] = f.object[namecapacity-1] = f.text[R::maxtextlength] = '\0';
            return true;
        }

        void clearrecord(const embot::core::Data &memory)
        {
            if((nullptr != memory.pointer) && (memory.capacity >= sizeof(FatalRecord)))
            {
                std::memset(memory.pointer, 0, sizeof(FatalRecord));
            }
        }

        // the report from the record. the strings point inside f, which must stay alive
        void toreport(const FatalRecord &f, R::Report &r)
        {
            r = R::Report {};
            r.time = f.time;
            r.severity = R::Severity::fatal;
            r.descriptor = R::Descriptor {f.code, f.sourcedevice, f.sourceaddress, f.par16, f.par64};
            r.thread = ('\0' != f.thread[0]) ? f.thread : nullptr;
            r.object = ('\0' != f.object[0]) ? f.object : nullptr;
            r.interrupt = f.interrupt;
            r.previousrun = true;
            if(1 == f.hasfault)
            {
                r.fault = R::Fault {static_cast<R::CPU>(f.cpu), f.fault[0], f.fault[1], f.fault[2], f.fault[3], f.fault[4], f.fault[5]};
            }
            copytext(r.text, sizeof(r.text), f.text);
        }

        [[noreturn]] void endoffatal(R::OnFatal onfatal)
        {
            if(R::OnFatal::reset == onfatal)
            {
                embot::hw::sys::reset();
            }
            volatile uint32_t forever {0};
            for(;;)
            {   // w/ the interrupts on: the debugger can stop here and look at the record
                forever = forever + 1;
            }
        }

        // ---- the queue: bounded, many producers (threads and ISRs), one consumer (the thread of the reporter).
        // the algorithm of D. Vyukov: each slot has a sequence number that tells whether it is free for the producer of
        // position pos (seq == pos) or full for the consumer at position pos (seq == pos + 1). a producer reserves a
        // position w/ a compare-and-swap, writes the slot and then publishes it. no producer ever waits for another

        static_assert(std::atomic<uint32_t>::is_always_lock_free, "theReporter needs lock-free 32-bit atomics: "
                      "for a cpu w/out them, the reservation of a slot needs a critical section of embot::os::rtos");

        class Queue
        {
        public:
            Queue() = default;

            // capacity is rounded up to a power of 2, so that the positions can wrap around at 2^32
            void init(size_t capacity)
            {
                uint32_t c {4};
                while(c < capacity)
                {
                    c <<= 1;
                }
                slots = new Slot[c];
                mask = c - 1;
                for(uint32_t i = 0; i < c; i++)
                {
                    slots[i].sequence.store(i, std::memory_order_relaxed);
                }
            }

            size_t capacity() const { return mask + 1; }

            // fill(R::Report &) writes the slot. false if the queue is full
            template<typename F>
            bool push(F fill)
            {
                uint32_t pos {tail.load(std::memory_order_relaxed)};
                for(;;)
                {
                    Slot &s {slots[pos & mask]};
                    const uint32_t seq {s.sequence.load(std::memory_order_acquire)};
                    const int32_t dif {static_cast<int32_t>(seq - pos)};
                    if(0 == dif)
                    {
                        if(tail.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
                        {
                            fill(s.report);
                            s.sequence.store(pos + 1, std::memory_order_release);
                            return true;
                        }
                        // another producer took pos: compare_exchange_weak() has loaded the new tail in pos
                    }
                    else if(dif < 0)
                    {
                        return false;
                    }
                    else
                    {
                        pos = tail.load(std::memory_order_relaxed);
                    }
                }
            }

            // only the consumer. false if there is no published report at the head
            bool pop(R::Report &report)
            {
                Slot &s {slots[head & mask]};
                const uint32_t seq {s.sequence.load(std::memory_order_acquire)};
                if(static_cast<int32_t>(seq - (head + 1)) < 0)
                {
                    return false;
                }
                report = s.report;
                s.sequence.store(head + mask + 1, std::memory_order_release);
                head++;
                return true;
            }

        private:
            struct Slot
            {
                std::atomic<uint32_t> sequence {0};
                R::Report report {};
            };

            Slot *slots {nullptr};
            uint32_t mask {0};
            std::atomic<uint32_t> tail {0};
            uint32_t head {0};
        };

        // ---- the merge of the repeated reports

        constexpr bool same(const R::Report &a, R::Severity severity, const R::Descriptor &d)
        {
            return (a.severity == severity) && (a.descriptor.code == d.code) && (a.descriptor.sourcedevice == d.sourcedevice) &&
                   (a.descriptor.sourceaddress == d.sourceaddress) && (a.descriptor.par16 == d.par16) && (a.descriptor.par64 == d.par64);
        }

        struct Flood
        {
            bool used {false};
            embot::core::Time start {0};    // when the first one was delivered
            uint32_t merged {0};            // the same ones received after it
            R::Report last {};              // the first one, then the last one merged

            constexpr Flood() = default;
        };

    } // namespace


    struct theReporter::Impl
    {
        static constexpr embot::os::Event evtREPORT {embot::core::binary::mask::pos2mask<embot::os::Event>(0)};
        // the timeout of the thread when there is no merge: it does nothing
        static constexpr embot::core::relTime nowindow {embot::core::reltimeWaitForever};
        static constexpr embot::core::relTime minperiod {10*embot::core::time1millisec};

        Config config {};
        bool initialised {false};

        Queue queue {};
        std::atomic<uint32_t> droppedpending {0};      // lost after the last delivered report
        std::atomic<uint32_t> droppedtotal {0};

        embot::os::rtos::mutex_t *mtx {nullptr};       // it protects sinks, stats and history
        Sink sinks[maxsinks] {};
        Stats stats {};
        Snapshot::Entry history[Snapshot::historycapacity] {};
        uint8_t historynumber {0};

        Flood *flood {nullptr};                         // config.floodkeys of them, used only by the thread

        bool hasprevious {false};
        FatalRecord previous {};
        Report previousreport {};                       // its strings point inside previous

        embot::os::EventThread *thread {nullptr};

        Impl() = default;

        // ---- the report

        void fill(Report &r, Severity severity, const Descriptor &descriptor, const char *text, const char *object) const
        {
            r.time = embot::core::now();
            r.severity = severity;
            r.descriptor = descriptor;
            r.interrupt = embot::os::rtos::interruptid();
            r.thread = nullptr;
            if(0 == r.interrupt)
            {
                const embot::os::Thread *t {embot::os::theScheduler::getInstance().scheduled()};
                r.thread = (nullptr != t) ? t->getName() : nullptr;
            }
            r.object = object;
            r.previousrun = false;
            r.repeated = 1;
            r.dropped = 0;
            r.fault = Fault {};
            copytext(r.text, sizeof(r.text), text);
        }

        bool report(Severity severity, const Descriptor &descriptor, const char *text, const char *object)
        {
            if(Severity::fatal == severity)
            {   // also before initialise(): w/ the default Config (no record, no safe state, hang)
                fatal(descriptor, text, object);
            }
            if(false == initialised)
            {
                return false;
            }
            // the reporter does not report itself (e.g. a sink that calls report()): the report would wait for the thread
            // that is running the sink
            if((0 == embot::os::rtos::interruptid()) && (embot::os::theScheduler::getInstance().scheduled() == thread))
            {
                droppedpending++;
                droppedtotal++;
                return false;
            }
            const bool pushed {queue.push([&](Report &r) { fill(r, severity, descriptor, text, object); })};
            if(false == pushed)
            {
                droppedpending++;
                droppedtotal++;
                return false;
            }
            thread->setEvent(evtREPORT);
            return true;
        }

        [[noreturn]] void fatal(const Descriptor &descriptor, const char *text, const char *object)
        {
            Report r {};
            fill(r, Severity::fatal, descriptor, text, object);
            // 1. the record
            writerecord(config.fatalmemory, r, false);
            // 2. the safe state
            if(nullptr != config.safestate)
            {
                config.safestate();
            }
            // 3. the sinks, only from a thread. no mutex: the thread of the reporter may hold it and never run again
            if(0 == r.interrupt)
            {
                if(true == initialised)
                {
                    stats.reports[embot::core::tointegral(Severity::fatal)]++;
                    r.dropped = droppedpending.load();
                    for(const Sink &s : sinks)
                    {
                        if(true == s.isvalid())
                        {
                            s.fn(r, s.param);
                        }
                    }
                }
                else
                {
                    printsink(r, nullptr);
                }
                if(true == embot::os::rtos::scheduler_isrunning())
                {   // the threads of lower priority (theLogger, theIPservice) can send what the sinks gave them
                    embot::os::rtos::thread_sleep(nullptr, config.fatalgrace);
                }
            }
            // 4. hang or reset
            endoffatal(config.onfatal);
        }

        // ---- the thread

        static void tREPORTER(void *p)
        {
            embot::os::Thread *t {reinterpret_cast<embot::os::Thread *>(p)};
            t->run();
        }

        static void startup(embot::os::Thread *t, void *param)
        {
            static_cast<void>(t);
            Impl *impl {reinterpret_cast<Impl *>(param)};
            // after the init thread: the sinks attached in it are there
            if(true == impl->hasprevious)
            {
                {
                    embot::os::rtos::Lock lock {impl->mtx};
                    impl->stats.reports[embot::core::tointegral(Severity::fatal)]++;
                }
                Report r {impl->previousreport};
                impl->deliver(r);
            }
        }

        static void onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
        {   // at every evtREPORT and at every period(): the reports in the queue, then the windows of the merge that are over
            static_cast<void>(t);
            static_cast<void>(eventmask);
            Impl *impl {reinterpret_cast<Impl *>(param)};
            Report r {};
            while(true == impl->queue.pop(r))
            {
                impl->process(r);
            }
            impl->closewindows(embot::core::now());
        }

        embot::core::relTime period() const
        {
            return (0 == config.floodwindow) ? nowindow : std::max(minperiod, config.floodwindow / 4);
        }

        void process(Report &r)
        {
            {
                embot::os::rtos::Lock lock {mtx};
                stats.reports[embot::core::tointegral(r.severity)]++;
            }

            if(0 == config.floodwindow)
            {
                deliver(r);
                return;
            }

            const embot::core::Time now {embot::core::now()};
            Flood *free {nullptr};
            Flood *oldest {nullptr};
            for(uint8_t i = 0; i < config.floodkeys; i++)
            {
                Flood &f {flood[i]};
                if(false == f.used)
                {
                    free = (nullptr == free) ? &f : free;
                    continue;
                }
                if(true == same(f.last, r.severity, r.descriptor))
                {
                    if((now - f.start) < config.floodwindow)
                    {   // only counted
                        f.merged++;
                        f.last = r;
                        embot::os::rtos::Lock lock {mtx};
                        stats.merged++;
                        return;
                    }
                    // its window is over (the thread was late): it closes and r starts a new one
                    close(f);
                    free = &f;
                    break;
                }
                oldest = ((nullptr == oldest) || (f.start < oldest->start)) ? &f : oldest;
            }

            if(nullptr == free)
            {   // all keys busy w/ other reports: the oldest window closes earlier
                close(*oldest);
                free = oldest;
            }

            deliver(r);
            free->used = true;
            free->start = now;
            free->merged = 0;
            free->last = r;
        }

        void close(Flood &f)
        {
            if(f.merged > 0)
            {
                Report r {f.last};
                r.repeated = f.merged;
                deliver(r);
            }
            f.used = false;
            f.merged = 0;
        }

        void closewindows(embot::core::Time now)
        {
            if(0 == config.floodwindow)
            {
                return;
            }
            for(uint8_t i = 0; i < config.floodkeys; i++)
            {
                Flood &f {flood[i]};
                if((true == f.used) && ((now - f.start) >= config.floodwindow))
                {
                    close(f);
                }
            }
        }

        void deliver(Report &r)
        {
            r.dropped = droppedpending.exchange(0);

            Sink active[maxsinks] {};
            {
                embot::os::rtos::Lock lock {mtx};
                remember(r);
                std::copy(std::begin(sinks), std::end(sinks), std::begin(active));
            }

            for(const Sink &s : active)
            {
                if((true == s.isvalid()) && (embot::core::tointegral(r.severity) >= embot::core::tointegral(s.minimum)))
                {
                    s.fn(r, s.param);
                }
            }
        }

        // w/ mtx taken
        void remember(const Report &r)
        {
            constexpr size_t n {Snapshot::historycapacity};
            std::copy_backward(std::begin(history), std::begin(history) + (n - 1), std::begin(history) + n);
            Snapshot::Entry &e {history[0]};
            e = Snapshot::Entry {};
            e.time = r.time;
            e.par64 = r.descriptor.par64;
            e.code = r.descriptor.code;
            e.repeated = r.repeated;
            e.par16 = r.descriptor.par16;
            e.severity = embot::core::tointegral(r.severity);
            e.sourcedevice = r.descriptor.sourcedevice;
            e.sourceaddress = r.descriptor.sourceaddress;
            e.interrupt = static_cast<uint8_t>(std::min<uint16_t>(r.interrupt, 255));
            copytext(e.text, sizeof(e.text), r.text);
            historynumber = static_cast<uint8_t>(std::min<size_t>(historynumber + 1, n));
        }
    };


    // --------------------------------------------------------------------------------------------------------------------
    // - the class
    // --------------------------------------------------------------------------------------------------------------------

    theReporter &theReporter::getInstance()
    {
        static theReporter *p = new theReporter();
        return *p;
    }

    theReporter::theReporter() : pImpl(new Impl) {}

    theReporter::~theReporter() {}

    bool theReporter::initialise(const Config &config)
    {
        Impl *impl {pImpl.get()};
        if((true == impl->initialised) || (false == config.isvalid()))
        {
            return false;
        }

        impl->config = config;
        impl->mtx = embot::os::rtos::mutex_new();
        impl->queue.init(config.queuecapacity);
        impl->flood = new Flood[config.floodkeys];

        // the fatal of the previous run: kept here, cleared there
        if(true == readrecord(config.fatalmemory, impl->previous))
        {
            impl->hasprevious = true;
            toreport(impl->previous, impl->previousreport);
        }
        clearrecord(config.fatalmemory);

        if(Print::ON == config.print)
        {
            impl->sinks[0] = Sink {printsink, nullptr, config.printminimum};
        }

        const embot::os::EventThread::Config tc
        {
            config.thread.stacksize,
            config.thread.priority,
            Impl::startup,
            impl,
            impl->period(),
            Impl::onevent,
            "tREPORTER"
        };
        impl->thread = new embot::os::EventThread;
        impl->initialised = true;
        impl->thread->start(tc, Impl::tREPORTER);

        return true;
    }

    bool theReporter::initialised() const
    {
        return pImpl->initialised;
    }

    bool theReporter::report(Severity severity, const Descriptor &descriptor, const char *text, const char *object)
    {
        return pImpl->report(severity, descriptor, text, object);
    }

    bool theReporter::report(Severity severity, const Descriptor &descriptor, const std::string &text, const char *object)
    {
        return pImpl->report(severity, descriptor, text.c_str(), object);
    }

    void theReporter::fatal(const Descriptor &descriptor, const char *text, const char *object)
    {
        pImpl->fatal(descriptor, text, object);
    }

    void theReporter::fatal(const Fault &fault, const embot::core::Data &fatalmemory, OnFatal onfatal, fpSafeState safestate)
    {
        // only the arguments: no instance, no heap, no RTOS
        Report r {};
        r.time = embot::core::now();
        r.severity = Severity::fatal;
        r.interrupt = embot::os::rtos::interruptid();
        r.fault = fault;
        copytext(r.text, sizeof(r.text), "fault");
        writerecord(fatalmemory, r, true);
        if(nullptr != safestate)
        {
            safestate();
        }
        endoffatal(onfatal);
    }

    bool theReporter::attach(const Sink &sink, uint8_t &id)
    {
        if((false == pImpl->initialised) || (false == sink.isvalid()))
        {
            return false;
        }
        embot::os::rtos::Lock lock {pImpl->mtx};
        for(uint8_t i = 0; i < maxsinks; i++)
        {
            if(false == pImpl->sinks[i].isvalid())
            {
                pImpl->sinks[i] = sink;
                id = i;
                return true;
            }
        }
        return false;
    }

    bool theReporter::detach(uint8_t id)
    {
        if((false == pImpl->initialised) || (id >= maxsinks))
        {
            return false;
        }
        embot::os::rtos::Lock lock {pImpl->mtx};
        const bool was {pImpl->sinks[id].isvalid()};
        pImpl->sinks[id] = Sink {};
        return was;
    }

    void theReporter::printsink(const Report &report, void *param)
    {
        static_cast<void>(param);
        // [error] S12:m345:u678 (tCTRL, canhandler) code 0x00030002 dev 0 addr 0 par16 0x0002 par64 0x0000000000000000 x5 lost 2: text
        char line[256] {0};
        const embot::core::TimeFormatter tf {report.time};
        char where[48] {0};
        if(nullptr != report.thread)
        {
            std::snprintf(where, sizeof(where), "%s", report.thread);
        }
        else if(0 != report.interrupt)
        {
            std::snprintf(where, sizeof(where), "isr %u", static_cast<unsigned>(report.interrupt));
        }
        else
        {
            std::snprintf(where, sizeof(where), "-");
        }
        int n {std::snprintf(line, sizeof(line), "[%s] %sS%lu:m%u:u%u (%s, %s) code 0x%08lx dev %u addr %u par16 0x%04x par64 0x%08lx%08lx",
                             to_cstring(report.severity), report.previousrun ? "previous run " : "",
                             static_cast<unsigned long>(tf.to_seconds()), static_cast<unsigned>(tf.m), static_cast<unsigned>(tf.u),
                             where, (nullptr != report.object) ? report.object : "-",
                             static_cast<unsigned long>(report.descriptor.code),
                             static_cast<unsigned>(report.descriptor.sourcedevice), static_cast<unsigned>(report.descriptor.sourceaddress),
                             static_cast<unsigned>(report.descriptor.par16),
                             static_cast<unsigned long>(report.descriptor.par64 >> 32), static_cast<unsigned long>(report.descriptor.par64 & 0xFFFFFFFFU))};
        size_t used {(n < 0) ? 0 : std::min<size_t>(static_cast<size_t>(n), sizeof(line) - 1)};
        if(report.repeated > 1)
        {
            n = std::snprintf(line + used, sizeof(line) - used, " x%lu", static_cast<unsigned long>(report.repeated));
            used = (n < 0) ? used : std::min<size_t>(used + static_cast<size_t>(n), sizeof(line) - 1);
        }
        if(report.dropped > 0)
        {
            n = std::snprintf(line + used, sizeof(line) - used, " lost %lu", static_cast<unsigned long>(report.dropped));
            used = (n < 0) ? used : std::min<size_t>(used + static_cast<size_t>(n), sizeof(line) - 1);
        }
        if(CPU::none != report.fault.cpu)
        {
            n = std::snprintf(line + used, sizeof(line) - used, " pc 0x%08lx ret 0x%08lx status 0x%08lx cause 0x%08lx detail 0x%08lx addr 0x%08lx",
                              static_cast<unsigned long>(report.fault.pc), static_cast<unsigned long>(report.fault.ret),
                              static_cast<unsigned long>(report.fault.status), static_cast<unsigned long>(report.fault.cause),
                              static_cast<unsigned long>(report.fault.detail), static_cast<unsigned long>(report.fault.address));
            used = (n < 0) ? used : std::min<size_t>(used + static_cast<size_t>(n), sizeof(line) - 1);
        }
        if('\0' != report.text[0])
        {
            std::snprintf(line + used, sizeof(line) - used, ": %s", report.text);
        }
        embot::core::print(line);
    }

    theReporter::Stats theReporter::stats() const
    {
        Stats s {};
        if(true == pImpl->initialised)
        {
            embot::os::rtos::Lock lock {pImpl->mtx};
            s = pImpl->stats;
        }
        s.dropped = pImpl->droppedtotal.load();
        return s;
    }

    void theReporter::snapshot(Snapshot &snapshot) const
    {
        snapshot = Snapshot {};
        const Stats s {stats()};
        std::copy(std::begin(s.reports), std::end(s.reports), std::begin(snapshot.reports));
        snapshot.merged = s.merged;
        snapshot.dropped = s.dropped;
        if(true == pImpl->initialised)
        {
            embot::os::rtos::Lock lock {pImpl->mtx};
            snapshot.number = pImpl->historynumber;
            std::copy(std::begin(pImpl->history), std::end(pImpl->history), std::begin(snapshot.entries));
        }
    }

    bool theReporter::previousfatal(Report &report) const
    {
        if(false == pImpl->hasprevious)
        {
            return false;
        }
        report = pImpl->previousreport;
        return true;
    }

} // namespace embot::svc


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

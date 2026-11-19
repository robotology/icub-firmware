/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_INSTR_H_
#define __EMBOT_INSTR_H_

// embot::instr holds the instrumentation of the firmware: the objects that observe how the code behaves while it runs.
// this file holds the portable ones, which need only embot_core (embot_instr.cpp in the project):
// - Histogram        it counts values in bins. HistogramSnapshot<N>: a copy w/ a fixed layout, for the wire
// - PeriodValidator  it checks that an activity happens every period. PeriodValidatorSnapshot<N>: a copy for the wire
// - Scope            it marks a piece of code: on() at its start, off() at its end
//                    ScopeDummy (nothing), ScopePrint (a print of the times), ScopeHisto (the durations in a Histogram)
// - Measure          it takes the duration of a piece of code: start() and stop()
//                    MeasureDummy (nothing), MeasureHisto (a histogram w/ a report every so many stop())
// the Scopes that need the board are in their own files, so that a project takes only what it can build:
// - ScopeGPIO    (embot_instr_ScopeGPIO.h)     a pin high / low, for an oscilloscope or a logic analyser. embot::hw
// - ScopeEViewer (embot_instr_ScopeEViewer.h)  a named activity in the Event Viewer of uVision. lib eventviewer
//
// they come from embot::tools (icub-firmware-shared/embot/tools/embot_tools.h), embot::app::scope
// (embot/app/embot_app_scope.h) and the Measure written in the theMBD of amc2c, amcbldc, amcfoc, which stay as they are
// for the projects that use them. old and new are different types and can live in the same project. see README.md

#include "embot_core.h"
#include <cstdint>
#include <cstddef>
#include <vector>
#include <memory>
#include <type_traits>

// - Histogram ---------------------------------------------------------------------------------------------------------

namespace embot::instr {

    // it counts the values given to add() in nsteps() bins of width step that cover [min, max), plus the values below
    // min and the ones equal or beyond max.
    class Histogram
    {
    public:

        struct Config
        {   // there are nsteps() bins of step values each, which fill the range [min, max)
            uint64_t min {0};           // the first value of the first bin
            uint64_t max {0};           // the upper limit of the bins (the last value of the last bin is max-1)
            uint32_t step {0};          // the width of a bin

            constexpr Config() = default;
            constexpr explicit Config(uint64_t mi, uint64_t ma, uint32_t st) : min(mi), max(ma), step(st) {}

            constexpr uint64_t range() const { return max - min; }
            constexpr uint32_t nsteps() const { return static_cast<uint32_t>((range() + step - 1) / step); }
            constexpr bool isvalid() const { return (0 != step) && (min < max); }
        };

        struct Values
        {
            uint64_t total {0};             // below + sum(inside) + beyond
            uint64_t below {0};             // the values in (-INF, min)
            uint64_t beyond {0};            // the values in [max, +INF)
            std::vector<uint64_t> inside {};// inside[i] counts the values in [min + i*step, min + (i+1)*step)
            uint64_t smallest {0};          // the smallest value given to add(), also outside [min, max). 0 if total is 0
            uint64_t largest {0};           // the largest value given to add(), also outside [min, max). 0 if total is 0
            uint64_t sum {0};               // the sum of the values given to add(): the mean is sum / total
        };

        Histogram();
        ~Histogram();

        Histogram(const Histogram &) = delete;
        Histogram &operator=(const Histogram &) = delete;

        // it allocates the bins. false if the config is not valid
        bool init(const Config &config);

        // false if not initialised
        bool add(uint64_t value);

        // it zeroes all the counters, keeping the config
        bool reset();

        const Config &getconfig() const;
        const Values &getvalues() const;

        // the probability of each bin, in a vector of nsteps()+2 items: [0] is below, [nsteps()+1] is beyond.
        // false if there are no values. w/ an integer scale the probability is scaled by it (e.g. 1000 gives per mille)
        bool probabilitydensityfunction(std::vector<uint32_t> &values, uint32_t scale) const;
        bool probabilitydensityfunction(std::vector<double> &values) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };


    // a copy of a Histogram w/ a fixed layout: trivially copyable, no pointers, its size known at compile time. it is
    // what goes on the wire (e.g. a variable of TETHER, w/ a TETHER_FIELD for each member) or in a log. the bins have
    // the same width and cover [lo, hi). the counters saturate: total, below, beyond at 0xffffffff, a bin at 0xffff.
    // 32 + 2*N bytes (288 for 128 bins)
    template<size_t N>
    struct HistogramSnapshot
    {
        static constexpr size_t numberofbins {N};

        uint32_t total {0};         // all the values
        uint32_t below {0};         // the values < lo
        uint32_t beyond {0};        // the values >= hi
        float min {0.0f};           // the smallest value, also outside [lo, hi). 0 w/out values
        float max {0.0f};           // the largest value, also outside [lo, hi). 0 w/out values
        float mean {0.0f};          // the mean of the values. 0 w/out values
        float lo {0.0f};            // the left edge of bins[0]
        float hi {0.0f};            // the right edge of bins[N-1]
        uint16_t bins[N] {0};       // bins[i] counts the values in [lo + i*(hi-lo)/N, lo + (i+1)*(hi-lo)/N)

        constexpr HistogramSnapshot() = default;
    };

    // it fills the snapshot w/ the histogram. if the histogram has fewer than N bins, the bins beyond its last one stay
    // at 0 and hi is the right edge of its last bin. if it has more, the values of its bins from N on go in beyond and
    // hi is the right edge of bins[N-1]
    template<size_t N>
    void snapshot(const Histogram &histo, HistogramSnapshot<N> &snap)
    {
        static_assert(std::is_trivially_copyable_v<HistogramSnapshot<N>> && std::is_standard_layout_v<HistogramSnapshot<N>>,
                      "a HistogramSnapshot is a plain block of bytes");
        constexpr uint64_t max32 {0xffffffff};
        constexpr uint64_t max16 {0xffff};
        const auto sat32 = [](uint64_t v) { return static_cast<uint32_t>((v < max32) ? v : max32); };

        const Histogram::Config &c {histo.getconfig()};
        const Histogram::Values &v {histo.getvalues()};

        const size_t n {(v.inside.size() < N) ? v.inside.size() : N};
        uint64_t beyond {v.beyond};
        for(size_t i = n; i < v.inside.size(); i++)
        {
            beyond += v.inside[i];
        }

        snap.total = sat32(v.total);
        snap.below = sat32(v.below);
        snap.beyond = sat32(beyond);
        snap.min = static_cast<float>(v.smallest);
        snap.max = static_cast<float>(v.largest);
        snap.mean = (0 == v.total) ? 0.0f : static_cast<float>(static_cast<double>(v.sum) / static_cast<double>(v.total));
        snap.lo = static_cast<float>(c.min);
        snap.hi = static_cast<float>(c.min + static_cast<uint64_t>(n) * c.step);
        for(size_t i = 0; i < N; i++)
        {
            snap.bins[i] = (i < n) ? static_cast<uint16_t>((v.inside[i] < max16) ? v.inside[i] : max16) : 0;
        }
    }

} // namespace embot::instr


// - PeriodValidator ---------------------------------------------------------------------------------------------------

namespace embot::instr {

    // it validates an activity that should happen every period: tick() is called at every execution w/ the current
    // time, it gives the time from the previous call, it raises an alert when that time is too long and, if asked, it
    // fills a histogram of the times and says when it is time to report them.
    class PeriodValidator
    {
    public:

        struct Config
        {
            embot::core::Time period {0};           // the expected period
            embot::core::Time alertvalue {0};       // a delta equal or beyond it raises an alert. it must be > period
            embot::core::Time reportinterval {0};   // if not 0, the time between two reports (only w/ a histogram)
            Histogram::Config histoconfig {};       // if valid, the deltas go in a histogram

            constexpr Config() = default;
            constexpr explicit Config(embot::core::Time pe, embot::core::Time al, embot::core::Time ri, const Histogram::Config &hi)
                : period(pe), alertvalue(al), reportinterval(ri), histoconfig(hi) {}

            constexpr bool isvalid() const { return (0 != period) && (period < alertvalue); }
        };

        PeriodValidator();
        ~PeriodValidator();

        PeriodValidator(const PeriodValidator &) = delete;
        PeriodValidator &operator=(const PeriodValidator &) = delete;

        bool init(const Config &config);

        // to be called every Config::period w/ the current time (e.g. embot::core::now()). it gives the time from the
        // previous call (0 at the first call). false if the time goes back
        bool tick(embot::core::Time currtime, embot::core::Time &deltatime);

        // it forgets all the ticks
        bool reset();

        // true when more than Config::reportinterval has passed since the previous report (only w/ a histogram)
        bool report() const;

        // true when the last delta was equal or beyond Config::alertvalue. it gives the last delta
        bool alert(embot::core::Time &deltatime) const;

        // the histogram of the deltas. it is empty if Config::histoconfig is not valid
        const Histogram &histogram() const;

        // what tick() has seen since init() or reset()
        struct Stats
        {
            uint64_t ticks {0};                 // the deltas computed (the calls of tick() after the first)
            uint64_t alerts {0};                // the deltas equal or beyond Config::alertvalue
            embot::core::Time lastdelta {0};
            embot::core::Time maxdelta {0};
        };

        const Config &getconfig() const;
        const Stats &stats() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };


    // a copy of a PeriodValidator w/ a fixed layout, as HistogramSnapshot: the times in the unit of embot::core::Time
    // (us), the counters saturated at 0xffffffff. 24 + 32 + 2*N bytes (312 for 128 bins)
    template<size_t N>
    struct PeriodValidatorSnapshot
    {
        uint32_t period {0};                    // the expected period
        uint32_t alertvalue {0};                // a delta equal or beyond it is an alert
        uint32_t lastdelta {0};
        uint32_t maxdelta {0};                  // since init() or reset()
        uint32_t ticks {0};                     // the deltas computed
        uint32_t alerts {0};                    // the deltas equal or beyond alertvalue
        HistogramSnapshot<N> histogram {};      // the deltas. all 0 if the PeriodValidator has no histogram

        constexpr PeriodValidatorSnapshot() = default;
    };

    template<size_t N>
    void snapshot(const PeriodValidator &pv, PeriodValidatorSnapshot<N> &snap)
    {
        static_assert(std::is_trivially_copyable_v<PeriodValidatorSnapshot<N>> && std::is_standard_layout_v<PeriodValidatorSnapshot<N>>,
                      "a PeriodValidatorSnapshot is a plain block of bytes");
        constexpr uint64_t max32 {0xffffffff};
        const auto sat32 = [](uint64_t v) { return static_cast<uint32_t>((v < max32) ? v : max32); };

        const PeriodValidator::Config &c {pv.getconfig()};
        const PeriodValidator::Stats &s {pv.stats()};
        snap.period = sat32(c.period);
        snap.alertvalue = sat32(c.alertvalue);
        snap.lastdelta = sat32(s.lastdelta);
        snap.maxdelta = sat32(s.maxdelta);
        snap.ticks = sat32(s.ticks);
        snap.alerts = sat32(s.alerts);
        snapshot(pv.histogram(), snap.histogram);
    }

} // namespace embot::instr


// - Scope -------------------------------------------------------------------------------------------------------------

namespace embot::instr {

    // the interface
    class Scope
    {
    public:
        virtual void on() = 0;
        virtual void off() = 0;

    protected:
        virtual ~Scope() = default;     // a Scope is destroyed from its implementation, not from the interface
    };


    // it does nothing
    class ScopeDummy : public Scope
    {
    public:
        constexpr ScopeDummy() = default;
        ~ScopeDummy() override = default;

        void on() override {}
        void off() override {}
    };


    // at off() it calls a function w/ the times of on() and off() and the duration. the default function prints them
    // w/ embot::core::print()
    class ScopePrint : public Scope
    {
    public:

        using fpOnOff = void (*)(const char *tag, embot::core::Time ontime, embot::core::Time offtime, embot::core::relTime duration);

        static void defaultonoff(const char *tag, embot::core::Time ontime, embot::core::Time offtime, embot::core::relTime duration);

        struct Config
        {
            const char *tag {"tag"};            // what tells this ScopePrint from the others. it must be a static string
            fpOnOff onoff {defaultonoff};       // called at off(). nullptr: defaultonoff

            constexpr Config() = default;
            constexpr explicit Config(const char *t, fpOnOff oo) : tag(t), onoff(oo) {}

            constexpr bool isvalid() const { return nullptr != tag; }
        };

        explicit ScopePrint(const Config &cfg);
        ~ScopePrint() override = default;

        void on() override;
        void off() override;

    private:
        Config config {};
        embot::core::Time start {0};
    };


    // it puts the duration between on() and off() in a Histogram. durations of 600 s or more are discarded
    class ScopeHisto : public Scope
    {
    public:

        struct Config
        {
            Histogram::Config hcfg {0, 8, 1};

            constexpr Config() = default;
            // the bins cover [minduration, minduration + range) w/ a width of step
            constexpr explicit Config(embot::core::relTime minduration, embot::core::relTime range, embot::core::relTime step)
                : hcfg(minduration, minduration + range, step) {}

            constexpr bool isvalid() const { return hcfg.isvalid(); }
        };

        explicit ScopeHisto(const Config &cfg);
        ~ScopeHisto() override = default;

        ScopeHisto(const ScopeHisto &) = delete;
        ScopeHisto &operator=(const ScopeHisto &) = delete;

        void on() override;
        void off() override;

        const Histogram &histogram() const;

    private:
        static constexpr embot::core::relTime maxduration {600*embot::core::time1second};
        Histogram histo {};
        embot::core::Time start {0};
    };

} // namespace embot::instr


// - Measure -----------------------------------------------------------------------------------------------------------

// how to use MeasureHisto:
// 1. create it w/ a Config: the histogram covers [minduration, maxduration) w/ bins of resolution
// 2. call start() and stop() around the code to measure
// 3. every reportevery stop(), report() is called. w/ Report::snapshot it copies the histogram in snapshot[] and
//    beyond, which are volatile: put a breakpoint in report() and look at them. w/ Report::print it also prints them.
//    report() can also be called at any time.

namespace embot::instr {

    // the interface
    class Measure
    {
    public:
        virtual void start() = 0;
        virtual void stop() = 0;

    protected:
        virtual ~Measure() = default;   // a Measure is destroyed from its implementation, not from the interface
    };


    // it does nothing
    class MeasureDummy : public Measure
    {
    public:
        constexpr MeasureDummy() = default;
        ~MeasureDummy() override = default;

        void start() override {}
        void stop() override {}
    };


    class MeasureHisto : public Measure
    {
    public:

        enum class Report : uint8_t { none = 0, snapshot = 1, print = 2 };

        struct Config
        {
            const char *name {"measure"};                                       // a static string
            embot::core::relTime minduration {0};
            embot::core::relTime maxduration {100*embot::core::time1microsec};
            embot::core::relTime resolution {embot::core::time1microsec};
            Report report {Report::snapshot};
            uint32_t reportevery {10000};                                       // number of stop(). 0: report() only by hand

            constexpr Config() = default;
            constexpr explicit Config(const char *n, embot::core::relTime min, embot::core::relTime max, embot::core::relTime res,
                                      Report r, uint32_t re)
                : name(n), minduration(min), maxduration(max), resolution(res), report(r), reportevery(re) {}

            constexpr bool isvalid() const { return (nullptr != name) && (0 != resolution) && (minduration < maxduration); }
        };

        // the bins copied by a snapshot: more bins than these are not copied (but they are in histogram())
        static constexpr size_t snapshotcapacity {512};

        explicit MeasureHisto(const Config &cfg);
        ~MeasureHisto() override = default;

        MeasureHisto(const MeasureHisto &) = delete;
        MeasureHisto &operator=(const MeasureHisto &) = delete;

        void start() override;
        void stop() override;

        // it copies the histogram in snapshot[] and beyond and, w/ Report::print, it prints them
        void report();

        const Histogram &histogram() const;

        // the copy made by report(), to be seen w/ the debugger
        volatile uint64_t snapshot[snapshotcapacity] {0};
        volatile uint64_t beyond {0};

    private:
        Config config {};
        ScopeHisto scope;
        uint32_t counter {0};
    };

} // namespace embot::instr


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

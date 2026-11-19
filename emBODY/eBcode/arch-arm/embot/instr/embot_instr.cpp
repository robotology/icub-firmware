/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_instr.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <string>


// ====================================================================================================================
// - Histogram
// ====================================================================================================================

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::instr::Histogram::Impl
{
    Config config {};
    Values values {};

    Impl() = default;

    bool init(const Config &cfg)
    {
        if(false == cfg.isvalid())
        {
            return false;
        }
        config = cfg;
        values.inside.assign(config.nsteps(), 0);
        values.total = values.below = values.beyond = 0;
        values.smallest = values.largest = values.sum = 0;
        return true;
    }

    bool add(uint64_t val)
    {
        if(false == config.isvalid())
        {   // not yet initialised
            return false;
        }

        if(val < config.min)
        {
            values.below++;
        }
        else if(val < config.max)
        {
            const uint64_t index {(val - config.min) / config.step};
            if(index >= values.inside.size())
            {
                return false;
            }
            values.inside[static_cast<size_t>(index)]++;
        }
        else
        {
            values.beyond++;
        }

        values.smallest = (0 == values.total) ? val : std::min(values.smallest, val);
        values.largest = (0 == values.total) ? val : std::max(values.largest, val);
        values.sum += val;
        values.total++;
        return true;
    }

    bool reset()
    {
        std::fill(values.inside.begin(), values.inside.end(), 0);
        values.total = values.below = values.beyond = 0;
        values.smallest = values.largest = values.sum = 0;
        return true;
    }
};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

embot::instr::Histogram::Histogram()
    : pImpl(std::make_unique<Impl>())
{
}

embot::instr::Histogram::~Histogram() = default;

bool embot::instr::Histogram::init(const Config &config)
{
    return pImpl->init(config);
}

bool embot::instr::Histogram::add(uint64_t value)
{
    return pImpl->add(value);
}

bool embot::instr::Histogram::reset()
{
    return pImpl->reset();
}

const embot::instr::Histogram::Config &embot::instr::Histogram::getconfig() const
{
    return pImpl->config;
}

const embot::instr::Histogram::Values &embot::instr::Histogram::getvalues() const
{
    return pImpl->values;
}

bool embot::instr::Histogram::probabilitydensityfunction(std::vector<double> &probs) const
{
    const Values &v {pImpl->values};
    if(0 == v.total)
    {
        probs.clear();
        return false;
    }

    const double total {static_cast<double>(v.total)};
    probs.resize(v.inside.size() + 2);
    probs.front() = static_cast<double>(v.below) / total;
    for(size_t i = 0; i < v.inside.size(); i++)
    {
        probs[i+1] = static_cast<double>(v.inside[i]) / total;
    }
    probs.back() = static_cast<double>(v.beyond) / total;
    return true;
}

bool embot::instr::Histogram::probabilitydensityfunction(std::vector<uint32_t> &probs, uint32_t scale) const
{
    const Values &v {pImpl->values};
    if(0 == v.total)
    {
        probs.clear();
        return false;
    }

    probs.resize(v.inside.size() + 2);
    probs.front() = static_cast<uint32_t>(scale * v.below / v.total);
    for(size_t i = 0; i < v.inside.size(); i++)
    {
        probs[i+1] = static_cast<uint32_t>(scale * v.inside[i] / v.total);
    }
    probs.back() = static_cast<uint32_t>(scale * v.beyond / v.total);
    return true;
}


// ====================================================================================================================
// - PeriodValidator
// ====================================================================================================================

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::instr::PeriodValidator::Impl
{
    Config config {};
    Histogram histo {};
    bool usehisto {false};

    embot::core::Time previous {0};
    embot::core::Time delta {0};
    embot::core::Time prevreport {0};
    bool enabledReport {false};
    bool enabledAlert {false};
    Stats stats {};

    Impl() = default;

    bool init(const Config &cfg)
    {
        if(false == cfg.isvalid())
        {
            return false;
        }
        config = cfg;
        usehisto = config.histoconfig.isvalid() && histo.init(config.histoconfig);
        return true;
    }

    bool tick(embot::core::Time currtime, embot::core::Time &deltatime)
    {
        if(0 == previous)
        {
            previous = currtime;
            prevreport = currtime;
            delta = 0;
            deltatime = delta;
            return true;
        }

        if(currtime < previous)
        {
            return false;
        }

        delta = currtime - previous;
        previous = currtime;

        if(true == usehisto)
        {
            histo.add(delta);
        }

        enabledAlert = (delta >= config.alertvalue);
        enabledReport = false;

        stats.ticks++;
        stats.lastdelta = delta;
        stats.maxdelta = std::max(stats.maxdelta, delta);
        if(true == enabledAlert)
        {
            stats.alerts++;
        }

        if((true == usehisto) && ((currtime - prevreport) > config.reportinterval))
        {
            prevreport = currtime;
            enabledReport = (config.reportinterval > 0);
        }

        deltatime = delta;
        return true;
    }

    bool reset()
    {
        previous = 0;
        delta = 0;
        prevreport = 0;
        enabledReport = false;
        enabledAlert = false;
        stats = Stats {};
        histo.reset();
        return true;
    }
};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

embot::instr::PeriodValidator::PeriodValidator()
    : pImpl(std::make_unique<Impl>())
{
}

embot::instr::PeriodValidator::~PeriodValidator() = default;

bool embot::instr::PeriodValidator::init(const Config &config)
{
    return pImpl->init(config);
}

bool embot::instr::PeriodValidator::tick(embot::core::Time currtime, embot::core::Time &deltatime)
{
    return pImpl->tick(currtime, deltatime);
}

bool embot::instr::PeriodValidator::reset()
{
    return pImpl->reset();
}

bool embot::instr::PeriodValidator::report() const
{
    return pImpl->enabledReport;
}

bool embot::instr::PeriodValidator::alert(embot::core::Time &deltatime) const
{
    deltatime = pImpl->delta;
    return pImpl->enabledAlert;
}

const embot::instr::Histogram &embot::instr::PeriodValidator::histogram() const
{
    return pImpl->histo;
}

const embot::instr::PeriodValidator::Config &embot::instr::PeriodValidator::getconfig() const
{
    return pImpl->config;
}

const embot::instr::PeriodValidator::Stats &embot::instr::PeriodValidator::stats() const
{
    return pImpl->stats;
}


// ====================================================================================================================
// - Scope
// ====================================================================================================================

// --------------------------------------------------------------------------------------------------------------------
// - ScopePrint
// --------------------------------------------------------------------------------------------------------------------

void embot::instr::ScopePrint::defaultonoff(const char *tag, embot::core::Time ontime, embot::core::Time offtime, embot::core::relTime duration)
{
    embot::core::print(std::string(tag) + ": [" + embot::core::TimeFormatter(ontime).to_string() + ", " +
                       embot::core::TimeFormatter(offtime).to_string() + "] -> " +
                       embot::core::TimeFormatter(duration).to_string());
}

embot::instr::ScopePrint::ScopePrint(const Config &cfg)
    : config(cfg)
{
    if(nullptr == config.tag)
    {
        config.tag = "tag";
    }
    if(nullptr == config.onoff)
    {
        config.onoff = defaultonoff;
    }
}

void embot::instr::ScopePrint::on()
{
    start = embot::core::now();
}

void embot::instr::ScopePrint::off()
{
    const embot::core::Time stop {embot::core::now()};
    config.onoff(config.tag, start, stop, static_cast<embot::core::relTime>(stop - start));
}

// --------------------------------------------------------------------------------------------------------------------
// - ScopeHisto
// --------------------------------------------------------------------------------------------------------------------

embot::instr::ScopeHisto::ScopeHisto(const Config &cfg)
{
    histo.init(cfg.hcfg);
}

void embot::instr::ScopeHisto::on()
{
    start = embot::core::now();
}

void embot::instr::ScopeHisto::off()
{
    const embot::core::Time duration {embot::core::now() - start};
    if(duration < maxduration)
    {
        histo.add(duration);
    }
}

const embot::instr::Histogram &embot::instr::ScopeHisto::histogram() const
{
    return histo;
}


// ====================================================================================================================
// - Measure
// ====================================================================================================================

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

embot::instr::MeasureHisto::MeasureHisto(const Config &cfg)
    : config(cfg), scope(ScopeHisto::Config(cfg.minduration, cfg.maxduration - cfg.minduration, cfg.resolution))
{
    if(nullptr == config.name)
    {
        config.name = "measure";
    }
}

void embot::instr::MeasureHisto::start()
{
    scope.on();
}

void embot::instr::MeasureHisto::stop()
{
    scope.off();

    if((Report::none != config.report) && (0 != config.reportevery))
    {
        if(++counter >= config.reportevery)
        {
            counter = 0;
            report();
        }
    }
}

void embot::instr::MeasureHisto::report()
{
    const Histogram::Values &v {scope.histogram().getvalues()};

    beyond = v.beyond;
    const size_t n {(v.inside.size() < snapshotcapacity) ? v.inside.size() : snapshotcapacity};
    for(size_t i = 0; i < n; i++)
    {
        snapshot[i] = v.inside[i];
    }

    // put a breakpoint here and look at beyond and snapshot[]

    if(Report::print == config.report)
    {
        std::string str {std::string("name, beyond, histo = ") + config.name + ", " + std::to_string(v.beyond)};
        for(size_t i = 0; i < n; i++)
        {
            str += ", ";
            str += std::to_string(v.inside[i]);
        }
        embot::core::print(str);
    }
}

const embot::instr::Histogram &embot::instr::MeasureHisto::histogram() const
{
    return scope.histogram();
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

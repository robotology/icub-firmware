
#include "amcmj1_gwlink.h"

#include "embot_svc_tether_ROP.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace {

    namespace rop = embot::svc::tether::rop;
    using rop::CMD;
    using rop::Head;
    using amcmj1::cube::CUBEmit;

    amcmj1::gateway::GWlimits lim {};
    amcmj1::gateway::GWstats counters {};
    bool haslastseq {false};
    uint32_t lastseq {0};
    CUBEmit accepted {};                // the last MIT after the limits
    bool newaccepted {false};
    bool faulted {false};               // OPgwFAULT: the EXTFAULT of the Speedgoat

    constexpr float clamp(float x, float lo, float hi)
    {
        return (x < lo) ? lo : ((x > hi) ? hi : x);
    }

    bool finite(const CUBEmit &m)
    {
        return std::isfinite(m.kp) && std::isfinite(m.kd) && std::isfinite(m.pos) && std::isfinite(m.vel) && std::isfinite(m.torque);
    }

    // true if something was changed
    bool applylimits(CUBEmit &m, const amcmj1::gateway::GWlimits &l)
    {
        const CUBEmit before {m};
        m.kp = clamp(m.kp, 0.0f, l.kp);
        m.kd = clamp(m.kd, 0.0f, l.kd);
        m.pos = clamp(m.pos, -l.pos, l.pos);
        m.vel = clamp(m.vel, -l.vel, l.vel);
        m.torque = clamp(m.torque, -l.torque, l.torque);
        return (m.kp != before.kp) || (m.kd != before.kd) || (m.pos != before.pos) || (m.vel != before.vel) || (m.torque != before.torque);
    }

    // a sig<id, value> appended to the frame
    template<typename T>
    bool sig(rop::Writer &w, uint8_t id, const T &value)
    {
        return nullptr != w.add(Head::request(CMD::sig, rop::OPTnone, id, static_cast<uint16_t>(sizeof(T))), &value);
    }

} // namespace


namespace amcmj1::gateway {

    void setlimits(const GWlimits &limits)
    {
        if(true == valid(limits))
        {
            lim = limits;
        }
    }

    GWlimits limits() { return lim; }

    void setfault(bool on) { faulted = on; }
    bool fault() { return faulted; }

    Frame oncommands(const void *payload, size_t size, embot::core::Time now)
    {
        // the whole frame is validated first: a malformed one is not used at all
        rop::Reader reader {payload, size, rop::maxsizeofframe};
        if(rop::Error::none != reader.error())
        {
            counters.badframes++;
            return Frame::bad;
        }
        counters.packets++;

        bool hasmit {false};
        CUBEmit mit {};
        rop::View r {};
        while(reader.next(r))
        {
            const bool carries {(CMD::set == r.cmd()) || (CMD::sig == r.cmd())};
            if((true == carries) && (ids::motorTARGET == r.id()))
            {
                if(true == faulted)
                {
                    counters.rejected++;                // EXTFAULT: the macromotor stays idle
                }
                else if(sizeof(amcmj1::mot::MOTORtarget) == r.size())
                {
                    amcmj1::motdriver::target(r.value<amcmj1::mot::MOTORtarget>(), now);   // it counts what it does not use
                }
                else
                {
                    counters.rejected++;
                }
                continue;
            }
            if((false == carries) || (ids::cubeMIT != r.id()))
            {
                counters.ignored++;
                continue;
            }
            if(sizeof(CUBEmit) != r.size())
            {
                counters.rejected++;
                continue;
            }
            CUBEmit m {r.value<CUBEmit>()};
            if(false == finite(m))
            {
                counters.rejected++;
                continue;
            }
            mit = m;                                // the last valid one of the frame wins
            hasmit = true;
        }

        if(false == hasmit)
        {
            amcmj1::cubedriver::heartbeat(now);
            return Frame::withoutmit;
        }
        if(true == faulted)
        {
            counters.notrunning++;                  // EXTFAULT: the MIT is not used, the CubeMars stays disabled
            amcmj1::cubedriver::heartbeat(now);
            return Frame::withoutmit;
        }

        // the gaps of seq: packets lost on the way. going backwards (the Speedgoat restarted) is not a loss
        if(true == haslastseq)
        {
            const uint32_t d {mit.seq - lastseq};
            if((d > 1) && (d < 0x80000000u))
            {
                counters.lost += d - 1;
            }
        }
        haslastseq = true;
        lastseq = mit.seq;
        counters.seq = mit.seq;

        if(true == applylimits(mit, lim))
        {
            counters.clamped++;
        }

        accepted = mit;
        newaccepted = true;

        const amcmj1::cubedriver::Result res {amcmj1::cubedriver::command(mit, now)};
        if(amcmj1::cubedriver::Result::notrunning == res)
        {
            counters.notrunning++;
        }
        else if(amcmj1::cubedriver::Result::discarded == res)
        {
            counters.discarded++;
        }
        return Frame::withmit;
    }

    bool buildstate(void *buffer, size_t capacity, size_t &size, embot::core::Time now, const amcmj1::cube::CUBEstatus &status,
                    const amcmj1::cube::CUBEstate &state, const PAXtorque &torque, const MOTORstate &motor,
                    const amcmj1::pos::POSstate &position)
    {
        size = 0;
        if((nullptr == buffer) || (capacity < statesize))
        {
            return false;
        }
        rop::Writer w {buffer, capacity};
        MOTORstate m {motor};
        m.gwfault = faulted ? 1 : 0;
        const bool ok {sig(w, ids::cubeSTATUS, status) && sig(w, ids::cubeSTATE, state) && sig(w, ids::paxTORQUE, torque) &&
                       sig(w, ids::motorSTATE, m) && sig(w, ids::gwSTATS, counters) && sig(w, ids::posSTATE, position)};
        if(false == ok)
        {
            return false;
        }
        size = w.close(static_cast<uint64_t>(now));
        return true;
    }

    bool takelastmit(amcmj1::cube::CUBEmit &mit)
    {
        if(false == newaccepted)
        {
            return false;
        }
        mit = accepted;
        newaccepted = false;
        return true;
    }

    GWstats stats() { return counters; }

    void resetstats()
    {
        counters = GWstats {};
        haslastseq = false;
        lastseq = 0;
        newaccepted = false;
    }

    Rpc onrpc(const void *payload, size_t size)
    {
        Rpc r {};
        rop::Reader reader {payload, size, rop::maxsizeofframe};
        rop::View first {};
        if((rop::Error::none != reader.error()) || (false == reader.next(first)))
        {
            return r;
        }
        r.id = first.id();
        r.ackrequired = first.ackrequired();
        if(CMD::run != first.cmd())
        {
            r.request = Request::unknown;
        }
        else if(ids::OPcubeENABLE == first.id())
        {
            r.request = Request::enable;
        }
        else if(ids::OPcubeDISABLE == first.id())
        {
            r.request = Request::disable;
        }
        else if(ids::OPpaxENABLE == first.id())
        {
            r.request = Request::paxenable;
        }
        else if(ids::OPpaxDISABLE == first.id())
        {
            r.request = Request::paxdisable;
        }
        else if(ids::OPmotorPIDS == first.id())
        {
            r.request = (sizeof(r.pids) == first.size()) ? Request::motorpids : Request::badargument;
            if(Request::motorpids == r.request) { r.pids = first.value<amcmj1::mot::MOTORpids>(); }
        }
        else if(ids::OPmotorSETUP == first.id())
        {
            r.request = (sizeof(r.setup) == first.size()) ? Request::motorsetup : Request::badargument;
            if(Request::motorsetup == r.request) { r.setup = first.value<amcmj1::mot::MOTORsetup>(); }
        }
        else if(ids::OPgwFAULT == first.id())
        {
            r.request = (1 == first.size()) ? Request::gwfault : Request::badargument;
            if(Request::gwfault == r.request) { r.fault = (0 != first.data[0]); }
        }
        else if(ids::OPmotorMODE == first.id())
        {
            r.request = (1 == first.size()) ? Request::motormode : Request::badargument;
            if(Request::motormode == r.request) { r.mode = static_cast<amcmj1::mot::ControlMode>(first.data[0]); }
        }
        else
        {
            r.request = Request::unknown;
        }
        return r;
    }

    bool buildreply(void *buffer, size_t capacity, size_t &size, embot::core::Time now, const Rpc &rpc, bool done, const char *why)
    {
        size = 0;
        if((Request::none == rpc.request) || (false == rpc.ackrequired) || (nullptr == buffer))
        {
            return false;
        }
        rop::Writer w {buffer, capacity};
        bool ok {false};
        if(true == done)
        {   // light: no data
            ok = (nullptr != w.add(Head::reply(CMD::ack, rpc.id, CMD::run, 0), nullptr));
        }
        else
        {
            const char *text {(nullptr != why) ? why : "failed"};
            const size_t n {std::min<size_t>(std::strlen(text), rop::maxtextsize)};
            ok = (nullptr != w.add(Head::reply(CMD::nak, rpc.id, CMD::run, static_cast<uint16_t>(n)), text));
        }
        if(false == ok)
        {
            return false;
        }
        size = w.close(static_cast<uint64_t>(now));
        return true;
    }

} // namespace amcmj1::gateway

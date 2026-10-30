
#include "amcmj1_gwlink.h"

#include "embot_app_eth_ROP.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace {

    using embot::app::eth::rop::ROP;
    using embot::app::eth::rop::FrameHeader;
    namespace rop = embot::app::eth::rop;
    using amcmj1::cube::CUBEmit;

    constexpr size_t slotstate {ROP::maxsizeofROP};            // 40: all the sig<> of the state frame have the same slot

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

    // one slot of the frame: the first sizeofROP bytes of the ROP
    void putslot(uint8_t *frame, size_t sizeofROP, size_t n, const ROP &r)
    {
        std::memcpy(frame + sizeof(FrameHeader) + n * sizeofROP, &r, sizeofROP);
    }

    template<typename T>
    ROP sig(uint8_t id, const T &value)
    {
        return ROP(ROP::CMD::sig, ROP::OPTnone, id, static_cast<uint8_t>(sizeof(T)), &value, ROP::CMD::none);
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
        FrameHeader header {};
        if(rop::Error::none != rop::checkframe(payload, size, header))
        {
            counters.badframes++;
            return Frame::bad;
        }
        counters.packets++;

        bool hasmit {false};
        CUBEmit mit {};
        for(uint8_t n = 0; n < header.numOfROPs; ++n)
        {
            ROP r {};
            rop::extract(payload, header, n, r);
            if(r.size > (header.sizeofROP - ROP::headersize))
            {
                counters.badframes++;               // the ROP claims more data than its slot holds
                continue;
            }
            const bool carries {(ROP::CMD::set == r.cmd()) || (ROP::CMD::sig == r.cmd())};
            if((true == carries) && (ids::motorTARGET == r.id))
            {
                if(true == faulted)
                {
                    counters.rejected++;                // EXTFAULT: the macromotor stays idle
                }
                else if(sizeof(amcmj1::mot::MOTORtarget) == r.size)
                {
                    amcmj1::motdriver::target(r.value<amcmj1::mot::MOTORtarget>(), now);   // it counts what it does not use
                }
                else
                {
                    counters.rejected++;
                }
                continue;
            }
            if((false == carries) || (ids::cubeMIT != r.id))
            {
                counters.ignored++;
                continue;
            }
            if(sizeof(CUBEmit) != r.size)
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
        uint8_t *frame {static_cast<uint8_t *>(buffer)};
        const FrameHeader header {static_cast<uint8_t>(slotstate), 6, static_cast<uint64_t>(now)};
        std::memcpy(frame, &header, sizeof(FrameHeader));
        putslot(frame, slotstate, 0, sig(ids::cubeSTATUS, status));
        putslot(frame, slotstate, 1, sig(ids::cubeSTATE, state));
        putslot(frame, slotstate, 2, sig(ids::paxTORQUE, torque));
        MOTORstate m {motor};
        m.gwfault = faulted ? 1 : 0;
        putslot(frame, slotstate, 3, sig(ids::motorSTATE, m));
        putslot(frame, slotstate, 4, sig(ids::gwSTATS, counters));
        putslot(frame, slotstate, 5, sig(ids::posSTATE, position));
        size = statesize;
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
        FrameHeader header {};
        if((rop::Error::none != rop::checkframe(payload, size, header)) || (0 == header.numOfROPs))
        {
            return r;
        }
        ROP first {};
        rop::extract(payload, header, 0, first);
        r.id = first.id;
        r.ackrequired = first.ackrequired();
        if(ROP::CMD::run != first.cmd())
        {
            r.request = Request::unknown;
        }
        else if(ids::OPcubeENABLE == first.id)
        {
            r.request = Request::enable;
        }
        else if(ids::OPcubeDISABLE == first.id)
        {
            r.request = Request::disable;
        }
        else if(ids::OPpaxENABLE == first.id)
        {
            r.request = Request::paxenable;
        }
        else if(ids::OPpaxDISABLE == first.id)
        {
            r.request = Request::paxdisable;
        }
        else if(ids::OPmotorPIDS == first.id)
        {
            r.request = (sizeof(r.pids) == first.size) ? Request::motorpids : Request::badargument;
            if(Request::motorpids == r.request) { r.pids = first.value<amcmj1::mot::MOTORpids>(); }
        }
        else if(ids::OPmotorSETUP == first.id)
        {
            r.request = (sizeof(r.setup) == first.size) ? Request::motorsetup : Request::badargument;
            if(Request::motorsetup == r.request) { r.setup = first.value<amcmj1::mot::MOTORsetup>(); }
        }
        else if(ids::OPgwFAULT == first.id)
        {
            r.request = (1 == first.size) ? Request::gwfault : Request::badargument;
            if(Request::gwfault == r.request) { r.fault = (0 != first.data[0]); }
        }
        else if(ids::OPmotorMODE == first.id)
        {
            r.request = (1 == first.size) ? Request::motormode : Request::badargument;
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
        ROP r {};
        if(true == done)
        {
            r.load(ROP::CMD::ack, ROP::OPTnone, rpc.id, 0, nullptr, ROP::CMD::run);
        }
        else
        {
            const char *text {(nullptr != why) ? why : "failed"};
            const size_t n {std::min<size_t>(std::strlen(text), ROP::datasize)};
            r.load(ROP::CMD::nak, ROP::OPTnone, rpc.id, static_cast<uint8_t>(n), text, ROP::CMD::run);
        }
        const size_t slot {ROP::slotsize(r.size) < ROP::minsizeofROP ? ROP::minsizeofROP : ROP::slotsize(r.size)};
        if(capacity < (sizeof(FrameHeader) + slot))
        {
            return false;
        }
        uint8_t *frame {static_cast<uint8_t *>(buffer)};
        const FrameHeader header {static_cast<uint8_t>(slot), 1, static_cast<uint64_t>(now)};
        std::memcpy(frame, &header, sizeof(FrameHeader));
        putslot(frame, slot, 0, r);
        size = sizeof(FrameHeader) + slot;
        return true;
    }

} // namespace amcmj1::gateway


#include "amcmj1_paxdriver.h"

#include "thePROThandler.h"

#include <cstring>
#include <vector>

namespace {

    using embot::app::can::PX3Qprotocol;
    using embot::app::can::thePROThandler;
    using embot::app::can::Callback;
    using Op = PX3Qprotocol::Opcode;
    using amcmj1::pax::State;

    constexpr embot::core::relTime acktimeout {100 * embot::core::time1millisec};   // TODO: tune it on the bench

    // ---- the sequence of commands in flight: each one waits for its ack before the next one is sent

    struct Sequence
    {
        static constexpr std::size_t capacity {4};
        Op ops[capacity] {};
        std::size_t size {0};
        std::size_t index {0};
        State target {State::off};          // the state when all the acks have arrived
        embot::core::Time deadline {0};     // 0: no ack is expected

        constexpr Sequence() = default;

        bool waiting() const { return 0 != deadline; }
        Op current() const { return ops[index]; }
        void reset() { size = 0; index = 0; deadline = 0; }
    };

    // ---- all the state of the driver lives in Impl, which is created by bind(). the only static is the pointer to it
    //      (constant-initialised, trivial): no static object with a ctor or a dtor, so nothing runs at startup or at exit

    struct Impl
    {
        // where the frames to transmit are pushed back: the output vector of the ctrl thread
        std::vector<embot::hw::can::Frame> *txout;
        State paxstate {State::off};
        Sequence sequence {};
        amcmj1::pax::PAXcfg config {};

        amcmj1::pax::PAXvalue latest {};
        bool newvalue {false};
        std::uint32_t counter {0};

        char versionstring[8] {0};

        // the callbacks of the protocol have this Impl as owner. they run inside parser().parse(), so in the ctrl thread
        thePROThandler<PX3Qprotocol> px3q;

        explicit Impl(std::vector<embot::hw::can::Frame> &out)
            : txout(&out),
              px3q(PX3Qprotocol::Config(0x7F),
                   PX3Qprotocol::Callbacks(Callback<PX3Qprotocol::Torque>(onTorque, this),
                                           Callback<PX3Qprotocol::Reply>(onReply, this)))
        {}

        // they only copy the data into the memory of the driver or advance the sequence: any frame to send goes into *txout
        static void onTorque(const PX3Qprotocol::Torque &t, void *owner);
        static void onReply(const PX3Qprotocol::Reply &r, void *owner);

        void fail()
        {
            sequence.reset();
            paxstate = State::error;
        }

        bool encode(Op op, std::vector<embot::hw::can::Frame> &out)
        {
            switch(op)
            {
                case Op::getVersion:    return px3q.encode(PX3Qprotocol::CmdGetVersion(), out);
                case Op::setRate:       return px3q.encode(PX3Qprotocol::CmdSetRate(static_cast<std::uint16_t>(config.ratehz)), out);
                case Op::start:         return px3q.encode(PX3Qprotocol::CmdStart(), out);
                case Op::stop:          return px3q.encode(PX3Qprotocol::CmdStop(), out);
                default:                return false;   // the driver never sends the other commands
            }
        }

        // it pushes the frame of the current command of the sequence into the output vector (tCTRL transmits it at the
        // end of the event) and arms the timeout of its ack
        bool sendcurrent()
        {
            if(false == encode(sequence.current(), *txout))
            {
                return false;
            }
            sequence.deadline = embot::core::now() + acktimeout;
            return true;
        }

        bool begin(State target, std::initializer_list<Op> ops)
        {
            sequence.reset();
            for(Op op : ops)
            {
                if(sequence.size < Sequence::capacity) { sequence.ops[sequence.size++] = op; }
            }
            sequence.target = target;
            paxstate = State::busy;
            if(false == sendcurrent())
            {
                fail();
                return false;
            }
            return true;
        }
    };

    Impl *pImpl {nullptr};

    void Impl::onTorque(const PX3Qprotocol::Torque &t, void *owner)
    {
        Impl *i {static_cast<Impl*>(owner)};
        i->latest.torque[0] = t.mx;
        i->latest.torque[1] = t.my;
        i->latest.torque[2] = t.mz;
        i->latest.counter = ++i->counter;
        i->newvalue = true;
    }

    void Impl::onReply(const PX3Qprotocol::Reply &r, void *owner)
    {
        Impl *i {static_cast<Impl*>(owner)};
        Sequence &sequence {i->sequence};
        if((false == sequence.waiting()) || (r.cmd != sequence.current()))
        {
            return;                         // not expected: e.g. a late ack after a timeout
        }
        if(false == r.ok())
        {
            i->fail();
            return;
        }
        if(Op::getVersion == sequence.current())
        {
            std::memcpy(i->versionstring, r.data, 6);
            i->versionstring[6] = 0;
        }
        if(++sequence.index < sequence.size)
        {
            if(false == i->sendcurrent()) { i->fail(); }
            return;
        }
        i->paxstate = sequence.target;
        sequence.reset();
    }

} // namespace


namespace amcmj1::paxdriver {

    void bind(std::vector<embot::hw::can::Frame> &out)
    {
        if(nullptr == pImpl)
        {
            pImpl = new Impl(out);          // once, at startup: it is never destroyed
        }
        else
        {
            pImpl->txout = &out;
        }
    }

    embot::app::can::Parser * parser()
    {
        return (nullptr == pImpl) ? nullptr : &pImpl->px3q;
    }

    bool init(const amcmj1::pax::PAXcfg &cfg)
    {
        if((nullptr == pImpl) || (false == amcmj1::pax::valid(cfg)) ||
           (State::running == pImpl->paxstate) || (State::busy == pImpl->paxstate))
        {
            return false;                   // running: stop it first
        }
        // canid 0 = not specified: the default one. config holds the id that is really used
        pImpl->config = amcmj1::pax::PAXcfg(amcmj1::pax::effectivecanid(cfg.canid), cfg.ratehz);
        pImpl->px3q.protocol().configure(PX3Qprotocol::Config(static_cast<std::uint8_t>(pImpl->config.canid)));
        pImpl->versionstring[0] = 0;
        // stop first: the PX3Q may still be streaming, e.g. after a reset of the board
        return pImpl->begin(State::initialised, {Op::stop, Op::getVersion, Op::setRate});
    }

    bool start()
    {
        if((nullptr == pImpl) || (State::initialised != pImpl->paxstate))
        {
            return false;
        }
        pImpl->px3q.protocol().resync();
        return pImpl->begin(State::running, {Op::start});
    }

    bool stop()
    {
        if((nullptr == pImpl) || (State::running != pImpl->paxstate))
        {
            return false;
        }
        return pImpl->begin(State::initialised, {Op::stop});
    }

    bool deinit()
    {
        if(nullptr == pImpl)
        {
            return false;
        }
        if((State::running == pImpl->paxstate) || (State::busy == pImpl->paxstate) || (State::error == pImpl->paxstate))
        {
            return pImpl->begin(State::off, {Op::stop});   // stop it anyway: we may not know what the PX3Q is doing
        }
        pImpl->sequence.reset();
        pImpl->paxstate = State::off;
        return true;
    }

    bool take(amcmj1::pax::PAXvalue &value)
    {
        if((nullptr == pImpl) || (false == pImpl->newvalue))
        {
            return false;
        }
        value = pImpl->latest;
        pImpl->newvalue = false;
        return true;
    }

    void tick(embot::core::Time now)
    {
        if((nullptr != pImpl) && pImpl->sequence.waiting() && (now > pImpl->sequence.deadline))
        {
            pImpl->fail();                  // the PX3Q did not answer
        }
    }

    amcmj1::pax::State state()
    {
        return (nullptr == pImpl) ? State::off : pImpl->paxstate;
    }

    Stats stats()
    {
        Stats r {};
        if(nullptr != pImpl)
        {
            const auto &s {pImpl->px3q.protocol().stats()};
            r.samples = s.samples;
            r.lostMz = s.lostMz;
            r.orphanMz = s.orphanMz;
            r.malformed = s.malformed;
        }
        return r;
    }

    const char * version()
    {
        return (nullptr == pImpl) ? "" : pImpl->versionstring;
    }

} // namespace amcmj1::paxdriver

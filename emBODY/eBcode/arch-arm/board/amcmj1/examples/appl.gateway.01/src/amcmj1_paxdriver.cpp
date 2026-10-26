
#include "amcmj1_paxdriver.h"

#include "amcmj1_canbus.h"
#include "embot_hw_can.h"
#include "theprothandler.h"

#include <cstring>
#include <vector>

namespace {

    using embot::app::can::PX3Qprotocol;
    using embot::app::can::thePROThandler;
    using embot::app::can::Callback;
    using Op = PX3Qprotocol::Opcode;
    using amcmj1::pax::State;

    constexpr embot::hw::CAN canbus {amcmj1::canbus::bus};
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

    bool opened {false};
    State paxstate {State::off};
    Sequence sequence {};
    amcmj1::pax::PAXcfg config {};

    amcmj1::pax::PAXvalue latest {};
    bool newvalue {false};
    std::uint32_t counter {0};

    char versionstring[8] {0};

    std::vector<embot::hw::can::Frame> txframes {};

    // ---- the callbacks of the protocol: they run inside rx(), so in the gateway thread

    void onTorque(const PX3Qprotocol::Torque &t, void *)
    {
        latest.torque[0] = t.mx;
        latest.torque[1] = t.my;
        latest.torque[2] = t.mz;
        latest.counter = ++counter;
        newvalue = true;
    }

    void onReply(const PX3Qprotocol::Reply &r, void *);

    thePROThandler<PX3Qprotocol> px3q
    {
        PX3Qprotocol::Config(0x7F),
        PX3Qprotocol::Callbacks(Callback<PX3Qprotocol::Torque>(onTorque, nullptr),
                                Callback<PX3Qprotocol::Reply>(onReply, nullptr))
    };

    // ---- the sequence

    // the consumer given to amcmj1::canbus: it decodes the PX3Q frames and erases them from the vector
    void consume(amcmj1::canbus::Frames &frames)
    {
        px3q.process(frames);
    }

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

    // it sends the current command of the sequence and arms the timeout of its ack
    bool sendcurrent()
    {
        txframes.clear();
        if(false == encode(sequence.current(), txframes))
        {
            return false;
        }
        for(const auto &f : txframes)
        {
            if(embot::hw::resOK != embot::hw::can::put(canbus, f))
            {
                return false;
            }
        }
        if(embot::hw::resOK != embot::hw::can::transmit(canbus))
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

    void onReply(const PX3Qprotocol::Reply &r, void *)
    {
        if((false == sequence.waiting()) || (r.cmd != static_cast<std::uint8_t>(sequence.current())))
        {
            return;                         // not expected: e.g. a late ack after a timeout
        }
        if(false == r.ok())
        {
            fail();
            return;
        }
        if(Op::getVersion == sequence.current())
        {
            std::memcpy(versionstring, r.data, 6);
            versionstring[6] = 0;
        }
        if(++sequence.index < sequence.size)
        {
            if(false == sendcurrent()) { fail(); }
            return;
        }
        paxstate = sequence.target;
        sequence.reset();
    }

} // namespace


namespace amcmj1::paxdriver {

    bool open()
    {
        if(true == opened)
        {
            return true;
        }
        if(false == amcmj1::canbus::attach(consume))
        {
            return false;
        }
        txframes.reserve(4);
        opened = true;
        return true;
    }

    bool init(const amcmj1::pax::PAXcfg &cfg)
    {
        if((false == opened) || (false == amcmj1::pax::valid(cfg)) ||
           (State::running == paxstate) || (State::busy == paxstate))
        {
            return false;                   // running: stop it first
        }
        config = cfg;
        px3q.protocol().configure(PX3Qprotocol::Config(static_cast<std::uint8_t>(cfg.canid)));
        versionstring[0] = 0;
        // stop first: the PX3Q may still be streaming, e.g. after a reset of the board
        return begin(State::initialised, {Op::stop, Op::getVersion, Op::setRate});
    }

    bool start()
    {
        if((false == opened) || (State::initialised != paxstate))
        {
            return false;
        }
        px3q.protocol().resync();
        return begin(State::running, {Op::start});
    }

    bool stop()
    {
        if((false == opened) || (State::running != paxstate))
        {
            return false;
        }
        return begin(State::initialised, {Op::stop});
    }

    bool deinit()
    {
        if(false == opened)
        {
            return false;
        }
        if((State::running == paxstate) || (State::busy == paxstate) || (State::error == paxstate))
        {
            return begin(State::off, {Op::stop});   // stop it anyway: we may not know what the PX3Q is doing
        }
        sequence.reset();
        paxstate = State::off;
        return true;
    }

    bool take(amcmj1::pax::PAXvalue &value)
    {
        if((false == opened) || (false == newvalue))
        {
            return false;
        }
        value = latest;
        newvalue = false;
        return true;
    }

    void tick(embot::core::Time now)
    {
        if(sequence.waiting() && (now > sequence.deadline))
        {
            fail();                         // the PX3Q did not answer
        }
    }

    amcmj1::pax::State state() { return paxstate; }

    Stats stats()
    {
        const auto &s {px3q.protocol().stats()};
        Stats r {};
        r.samples = s.samples;
        r.lostMz = s.lostMz;
        r.orphanMz = s.orphanMz;
        r.malformed = s.malformed;
        return r;
    }

    const char * version() { return versionstring; }

} // namespace amcmj1::paxdriver

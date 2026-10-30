
#include "amcmj1_cubedriver.h"

#include "amcmj1_canbus.h"
#include "embot_hw_can.h"
#include "theprothandler.h"

#include <vector>

namespace {

    using embot::app::can::CubeMarsProtocol;
    using embot::app::can::thePROThandler;
    using embot::app::can::Callback;
    using amcmj1::cube::State;
    using amcmj1::cube::Servo;

    constexpr embot::hw::CAN canbus {amcmj1::canbus::bus};
    constexpr float pi {3.14159265359f};
    constexpr float deg2rad {pi / 180.0f};
    constexpr embot::core::relTime slack {2 * embot::core::time1millisec};     // the tick of the gateway thread has jitter

    enum class Active : uint8_t { none, mit, servo };

    bool opened {false};
    State cubestate {State::off};
    amcmj1::cube::CUBEcfg config {};

    Active active {Active::none};
    amcmj1::cube::CUBEmit mit {};
    amcmj1::cube::CUBEservo servo {};
    bool servosynced {false};                   // false after start(): the first seq seen is only memorised
    std::uint32_t servoseq {0};
    std::uint32_t discard {0};                  // MIT setpoints to drop: 1 after start()
    embot::core::Time lastarrival {0};          // when the active setpoint arrived: the watchdog
    embot::core::Time lastsent {0};
    bool due {false};                           // a new servo setpoint: send it at the next tick

    amcmj1::cube::CUBEstate latest {};
    bool newstate {false};
    bool haspos32 {false};                      // the 0x2A frames give a better position: the 0x29 one is not used then

    amcmj1::cubedriver::Stats counters {};
    std::vector<embot::hw::can::Frame> txframes {};

    void onStatus(const CubeMarsProtocol::Status &s, void *)
    {
        if(true == s.disableack)
        {
            return;                             // the answer to the disable packet: the error byte is not an error
        }
        if(false == haspos32)
        {
            latest.pos = s.posdeg * deg2rad;
        }
        latest.vel = s.speederpm * amcmj1::cube::erpm2rads;
        latest.current = s.currentA;
        latest.temp = s.temp;
        latest.error = s.error;
        latest.counter++;
        newstate = true;
    }

    void onPosition32(const CubeMarsProtocol::Position32 &p, void *)
    {
        latest.pos = p.posdeg * deg2rad;
        haspos32 = true;
    }

    thePROThandler<CubeMarsProtocol> motor
    {
        CubeMarsProtocol::Config(0x68),
        CubeMarsProtocol::Callbacks(Callback<CubeMarsProtocol::Status>(onStatus, nullptr),
                                    Callback<CubeMarsProtocol::Position32>(onPosition32, nullptr))
    };

    // the consumer given to amcmj1::canbus: it decodes the CubeMars frames and erases them from the vector
    void consume(amcmj1::canbus::Frames &frames)
    {
        motor.process(frames);
    }

    bool send()
    {
        bool ok {true};
        for(const auto &f : txframes)
        {
            if(embot::hw::resOK != embot::hw::can::put(canbus, f)) { ok = false; counters.txerrors++; }
            else                                                     { counters.txframes++; }
        }
        if(embot::hw::resOK != embot::hw::can::transmit(canbus)) { ok = false; counters.txerrors++; }
        return ok;
    }

    bool sendMIT()
    {
        txframes.clear();
        if(false == motor.encode(CubeMarsProtocol::CmdMIT(mit.kp, mit.kd, mit.pos, mit.vel, mit.torque), txframes))
        {
            counters.txerrors++;
            return false;
        }
        return send();
    }

    bool encodeservo(const amcmj1::cube::CUBEservo &s)
    {
        txframes.clear();
        switch(s.packet)
        {
            case Servo::duty:       return motor.encode(CubeMarsProtocol::CmdDuty(s.a), txframes);
            case Servo::current:    return motor.encode(CubeMarsProtocol::CmdCurrent(s.a), txframes);
            case Servo::brake:      return motor.encode(CubeMarsProtocol::CmdBrake(s.a), txframes);
            case Servo::velocity:   return motor.encode(CubeMarsProtocol::CmdVelocity(s.a), txframes);
            case Servo::position:   return motor.encode(CubeMarsProtocol::CmdPosition(s.a), txframes);
            case Servo::origin:     return motor.encode(CubeMarsProtocol::CmdOrigin(s.a >= 0.5f), txframes);
            case Servo::posvel:     return motor.encode(CubeMarsProtocol::CmdPosVel(s.a, s.b, s.c), txframes);
            default:                return false;
        }
    }

    bool sendServo()
    {
        if(false == encodeservo(servo))
        {
            counters.txerrors++;
            return false;
        }
        return send();
    }

    // the disable packet. after a MIT (rest = true) it is preceded by the MIT of rest (kp kd pos vel torque = 0): the motor
    // forgets the last setpoint before it leaves the motor mode. the two frames go out in this order
    bool sendDisable(bool rest)
    {
        txframes.clear();
        if(true == rest)
        {
            motor.encode(CubeMarsProtocol::CmdMIT(0.0f, 0.0f, 0.0f, 0.0f, 0.0f), txframes);
        }
        motor.encode(CubeMarsProtocol::CmdDisable(), txframes);
        return send();
    }

} // namespace


namespace amcmj1::cubedriver {

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
        txframes.reserve(2);
        opened = true;
        return true;
    }

    bool init(const amcmj1::cube::CUBEcfg &cfg)
    {
        if((false == opened) || (false == amcmj1::cube::valid(cfg)) || (State::running == cubestate))
        {
            return false;                       // running: stop it first
        }
        config = cfg;
        motor.protocol().configure(CubeMarsProtocol::Config(static_cast<std::uint8_t>(cfg.canid)));
        active = Active::none;
        haspos32 = false;
        cubestate = State::initialised;
        return true;
    }

    bool start()
    {
        if((false == opened) || (State::initialised != cubestate))
        {
            return false;
        }
        active = Active::none;
        discard = 1;                            // the first MIT may be older than the enable
        servosynced = false;                    // the servo setpoint already in the variable is not for us
        cubestate = State::running;
        return true;
    }

    bool stop()
    {
        if((false == opened) || (State::running != cubestate))
        {
            return false;
        }
        const bool rest {Active::servo != active};    // a MIT, or nothing yet: the MIT of rest first
        active = Active::none;
        cubestate = State::initialised;
        return sendDisable(rest);
    }

    bool deinit()
    {
        if(false == opened)
        {
            return false;
        }
        bool ok {true};
        if((State::running == cubestate) || (State::error == cubestate))
        {
            ok = sendDisable(Active::servo != active);    // stop it anyway: we may not know what the motor is doing
        }
        active = Active::none;
        cubestate = State::off;
        return ok;
    }

    Result command(const amcmj1::cube::CUBEmit &m, embot::core::Time now)
    {
        if(State::running != cubestate)
        {
            return Result::notrunning;
        }
        if(discard > 0)
        {
            discard--;
            counters.discarded++;
            return Result::discarded;
        }
        if(false == amcmj1::cube::valid(m))
        {
            counters.txerrors++;                // the gateway clamps before: this is a bug of the caller. the previous setpoint goes on
            return Result::failed;
        }
        mit = m;
        active = Active::mit;
        lastarrival = now;
        lastsent = now;
        if(false == sendMIT())
        {
            cubestate = State::error;
            active = Active::none;
            return Result::failed;
        }
        return Result::sent;
    }

    bool heartbeat(embot::core::Time now)
    {
        if((State::running != cubestate) || (Active::mit != active))
        {
            return false;                       // nothing to send again: the watchdog is not fed either
        }
        lastarrival = now;
        lastsent = now;
        if(false == sendMIT())
        {
            cubestate = State::error;
            active = Active::none;
            return false;
        }
        return true;
    }

    void update(const amcmj1::cube::CUBEservo &s, embot::core::Time now)
    {
        if(State::running != cubestate)
        {
            return;
        }
        if((true == servosynced) && (s.seq != servoseq))
        {
            servo = s;                          // OPcubeSERVO has already checked it
            if(Servo::origin == s.packet)
            {
                sendServo();                    // a one shot: the active setpoint, if any, goes on
            }
            else
            {
                active = Active::servo;
                lastarrival = now;
                due = true;
            }
        }
        servoseq = s.seq;
        servosynced = true;
    }

    void tick(embot::core::Time now)
    {
        if((State::running != cubestate) || (Active::none == active))
        {
            return;
        }
        if((now - lastarrival) > static_cast<embot::core::Time>(config.watchdogms) * embot::core::time1millisec)
        {
            sendDisable(Active::servo != active);   // no news = stop
            active = Active::none;
            counters.watchdogs++;
            return;
        }
        if(Active::servo != active)
        {
            return;                             // a MIT is sent when its packet arrives
        }
        const embot::core::Time period {static_cast<embot::core::Time>(config.repeatms) * embot::core::time1millisec};
        if((true == due) || ((now - lastsent) + slack >= period))
        {
            const bool ok {sendServo()};
            lastsent = now;
            due = false;
            if(false == ok) { cubestate = State::error; active = Active::none; }
        }
    }

    bool take(amcmj1::cube::CUBEstate &state)
    {
        if((false == opened) || (false == newstate))
        {
            return false;
        }
        state = latest;
        newstate = false;
        return true;
    }

    amcmj1::cube::State state() { return cubestate; }

    Stats stats()
    {
        const auto &s {motor.protocol().stats()};
        Stats r {counters};
        r.status = s.status;
        r.position32 = s.position32;
        r.starts = s.starts;
        r.malformed = s.malformed;
        return r;
    }

} // namespace amcmj1::cubedriver

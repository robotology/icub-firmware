/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// the Dictionary of the udpdemo.tether: the variables and the operations that the host (tether.py) sees.
//
// it is a didactic Dictionary: every form of variable and every combination of argument and result of an operation
// appears once. to add a variable or an operation, read the explanations before the tables variables[] and operations[].
// the types are in udpdemo_types.h, the typed handles and udpdemo::dict::dictionary are declared in dictionary.udpdemo.h

// api
#include "dictionary.udpdemo.h"

// other dependencies
#include "udpdemo_echo.h"
#include "embot_app_eth_theTETHERservice.h"
#include "embot_app_theLEDmanager.h"
#include "embot_hw_led.h"
#include "embot_net_theLogger.h"
#include <cmath>
#include <cstdint>

namespace tether = embot::app::eth::tether;


// ---- the variables. only ID, access and name are written by hand: size, type and fields come from the C++ objects

namespace udpdemo::vars {

    // the memory. it is accessed only through theTETHERservice::sharedvariables(), w/ the typed handles of
    // dictionary.udpdemo.h: this is why it is not visible outside this file. all of them are constant initialised: no
    // constructor runs before main()
    namespace {

        uint64_t timeoflife {0};
        types::EchoCONFIG echoconfig {};
        types::EchoSTATS echostats {};
        types::EchoLAST echolast {};
        uint32_t ticks {0};
        float gain {1.0f};
        float vectorvalue[3] {0.0f, 0.0f, 0.0f};
        types::Mode mode {types::Mode::idle};
        bool flag {false};
        types::Clock clockvalue {};
        float signalvalue {0.0f};
        types::LogSTATUS logstatus {};
#if defined(tetherdebugENABLE_wave)
        float viewof_wave {0.0f};
#endif
#if defined(tetherdebugENABLE_pwm)
        echo::PWM viewof_pwm {};
#endif

    } // namespace

    // ---- the functions called on read (onread): they refresh the object just before it is read, for an ask or for a
    //      frame of the stream. they run in the thread that reads (rpc or stream of TETHER): keep them short

    // a scalar computed on read
    void refreshtimeoflife(tether::ID, void *memory, uint8_t)
    {
        *static_cast<uint64_t *>(memory) = static_cast<uint64_t>(embot::core::now());
    }

    // a struct computed on read: it fills all the members that must be fresh
    void refreshclock(tether::ID, void *memory, uint8_t)
    {
        types::Clock *c {static_cast<types::Clock *>(memory)};
        const embot::core::Time now {embot::core::now()};
        c->microseconds = static_cast<uint64_t>(now);
        c->seconds = static_cast<uint32_t>(now / embot::core::time1second);
    }

    // a sine at 1 Hz w/ amplitude IDgain: start the stream w/ IDsignal and plot it. it reads the object gain directly and
    // not w/ sharedvariables(): a float aligned to 4 bytes is read in one access (and sharedvariables() is busy w/ the
    // read of IDsignal, which called this function)
    void refreshsignal(tether::ID, void *memory, uint8_t)
    {
        constexpr float twopi {6.2831853f};
        const float seconds {static_cast<float>(embot::core::now() % embot::core::time1second) / static_cast<float>(embot::core::time1second)};
        *static_cast<float *>(memory) = gain * std::sin(twopi * seconds);
    }

    // the state of another module: the variable is only a window on embot::net::theLogger, filled when it is read. the
    // status belongs to theLogger (theLogger::Status, a coherent snapshot); types::LogSTATUS is how it travels on TETHER.
    // theLogger is configured w/ its own text commands on its port (see embot_net_theLogger.h), not w/ TETHER
    void refreshlogstatus(tether::ID, void *memory, uint8_t)
    {
        const embot::net::theLogger::Status s {embot::net::theLogger::getInstance().status()};
        const uint32_t ip {s.destination.addr.value()};
        *static_cast<types::LogSTATUS *>(memory) = types::LogSTATUS
        {
            static_cast<uint8_t>(ip & 0xff), static_cast<uint8_t>((ip >> 8) & 0xff),
            static_cast<uint8_t>((ip >> 16) & 0xff), static_cast<uint8_t>((ip >> 24) & 0xff), s.destination.port,
            embot::net::theLogger::Activation::ON == s.activation, embot::net::theLogger::Decoration::ON == s.decoration,
            s.stats.lines, s.stats.frames, s.stats.droppedlines
        };
    }

#if defined(tetherdebugENABLE_wave)
    // IDviewof_wave: a view of wave, a variable of the application that lives in udpdemo-tether.cpp (the echo thread
    // computes it) and is not in the Dictionary. the view is a copy, taken when the host reads it (ask, or a frame of
    // the stream)
    void refreshviewof_wave(tether::ID, void *memory, uint8_t)
    {
        *static_cast<float *>(memory) = echo::readwave();
    }
#endif

#if defined(tetherdebugENABLE_pwm)
    // IDviewof_pwm: a view of pwm, a struct of the application that lives in udpdemo-tether.cpp and is protected by a mutex
    // of its owner. readpwm() gives a coherent copy (u, v, w of the same moment). it is called w/ the mutex of the
    // SharedVariables taken, and it takes the mutex of pwm: the owner never does the opposite
    void refreshviewof_pwm(tether::ID, void *memory, uint8_t)
    {
        *static_cast<echo::PWM *>(memory) = echo::readpwm();
    }
#endif

    // ---- the fields of the structs: one TETHER_FIELD() for each member, in order

#if defined(tetherdebugENABLE_pwm)
    constexpr tether::Field viewofpwmfields[]
    {
        TETHER_FIELD(echo::PWM, u), TETHER_FIELD(echo::PWM, v), TETHER_FIELD(echo::PWM, w)
    };
#endif

    constexpr tether::Field echoconfigfields[]
    {
        TETHER_FIELD(types::EchoCONFIG, marker), TETHER_FIELD(types::EchoCONFIG, enabled), TETHER_FIELD(types::EchoCONFIG, uppercase)
    };

    constexpr tether::Field echostatsfields[]
    {
        TETHER_FIELD(types::EchoSTATS, rxframes), TETHER_FIELD(types::EchoSTATS, txframes),
        TETHER_FIELD(types::EchoSTATS, rxbytes), TETHER_FIELD(types::EchoSTATS, errors)
    };

    constexpr tether::Field echolastfields[]
    {
        TETHER_FIELD(types::EchoLAST, ip), TETHER_FIELD(types::EchoLAST, port), TETHER_FIELD(types::EchoLAST, size),
        TETHER_FIELD(types::EchoLAST, time)
    };

    constexpr tether::Field clockfields[]
    {
        TETHER_FIELD(types::Clock, microseconds), TETHER_FIELD(types::Clock, seconds)
    };

    constexpr tether::Field logstatusfields[]
    {
        TETHER_FIELD(types::LogSTATUS, ip), TETHER_FIELD(types::LogSTATUS, port), TETHER_FIELD(types::LogSTATUS, enabled),
        TETHER_FIELD(types::LogSTATUS, decorated), TETHER_FIELD(types::LogSTATUS, lines), TETHER_FIELD(types::LogSTATUS, frames),
        TETHER_FIELD(types::LogSTATUS, dropped)
    };


 #if 0

 ---- the table of the variables

 every entry is   tether::variable(ID or handle, object, Access, "name" [, fields] [, onread])

 there are two forms. the compiler picks the one that fits the type of the object:

   a scalar, or an array of scalars:
     template<typename T>
     constexpr Variable variable(ID id, T &object, Access access, const char *name, OnRead onread = nullptr)

   a struct, w/ its fields:
     template<typename T, size_t N>
     constexpr Variable variable(ID id, T &object, Access access, const char *name, const Field (&fields)[N], OnRead onread = nullptr)

 and each one exists also w/ a typed handle tether::Var<T> in place of the ID.

 - ID or handle   0x01 ... 0x9F, unique. it is what the host uses on the wire. two ways to give it:
                    a typed handle  constexpr tether::Var<T> vX {0x03};  then variable(vX, object, ...)
                                    the type T of the handle must be the type of the object, or it does not compile.
                                    the firmware that reads or writes the variable w/ sharedvariables() gives the handle
                                    too, so it cannot use a wrong type: shared().write(vX, value, ...)
                    a number        variable(0x07, object, ...): no handle, fine for a variable that only the host uses

 - object         a global (static storage): its address must be a constant. at most 36 bytes. the table only points to it

 - Access         RO  the host cannot set it (set -> nak). the firmware can always write it (Origin::local)
                  RW  the host can set it too.  any variable, RO or RW, can be added to the stream

 - "name"         the name the host uses: ask<IDname>, set[+ack]<IDname, value>. [A-Za-z_][A-Za-z0-9_]*, max 28 chars, unique

 - fields         only for a struct: a constexpr tether::Field[] built w/ TETHER_FIELD(Struct, member), one for each member,
                  in order. name, type, number of elements and offset come from the real struct, so padding is described
                  correctly and the host shows the value as {member=.., member=..}. a member is a scalar or an array of
                  scalars (no nested struct)

 - onread         optional. void f(tether::ID, void *memory, uint8_t size), called just before the variable is read for an
                  ask or for a frame of the stream: it refreshes the object, which is then copied

 the forms of this file:
   scalar, onread             variable(vTimeoflife, timeoflife, Access::RO, "IDtimeoflife", refreshtimeoflife)
   struct, RW                 variable(vEchoConfig, echoconfig, Access::RW, "IDechoCONFIG", echoconfigfields)
   struct, RO                 variable(vEchoStats,  echostats,  Access::RO, "IDechoSTATS",  echostatsfields)
   struct w/ array, padding   variable(vEchoLast,   echolast,   Access::RO, "IDechoLAST",   echolastfields)
   scalar uint32_t            variable(vTicks,      ticks,      Access::RW, "IDticks")
   scalar float               variable(vGain,       gain,       Access::RW, "IDgain")
   array of scalars, by ID    variable(0x07,        vectorvalue, Access::RW, "IDvector")
   enum (a scalar)            variable(vMode,       mode,       Access::RW, "IDmode")
   bool                       variable(vFlag,       flag,       Access::RW, "IDflag")
   struct, onread             variable(vClock,      clockvalue, Access::RO, "IDclock", clockfields, refreshclock)
   float, onread              variable(vSignal,     signalvalue, Access::RO, "IDsignal", refreshsignal)
   struct of another module   variable(vLogStatus,  logstatus,  Access::RO, "IDlogSTATUS", logstatusfields, refreshlogstatus)
   view of an app variable    variable(vViewofWave, viewof_wave, Access::RO, "IDviewof_wave", refreshviewof_wave)   w/ tetherdebugENABLE_wave
   view of an app struct      variable(vViewofPwm,  viewof_pwm,  Access::RO, "IDviewof_pwm", viewofpwmfields, refreshviewof_pwm)   w/ tetherdebugENABLE_pwm

 #endif

    constexpr tether::Variable variables[]
    {
        tether::variable(vTimeoflife, timeoflife,  tether::Access::RO, "IDtimeoflife", refreshtimeoflife),
        tether::variable(vEchoConfig, echoconfig,  tether::Access::RW, "IDechoCONFIG", echoconfigfields),
        tether::variable(vEchoStats,  echostats,   tether::Access::RO, "IDechoSTATS",  echostatsfields),
        tether::variable(vEchoLast,   echolast,    tether::Access::RO, "IDechoLAST",   echolastfields),
        tether::variable(vTicks,      ticks,       tether::Access::RW, "IDticks"),
        tether::variable(vGain,       gain,        tether::Access::RW, "IDgain"),
        tether::variable(0x07,        vectorvalue, tether::Access::RW, "IDvector"),
        tether::variable(vMode,       mode,        tether::Access::RW, "IDmode"),
        tether::variable(vFlag,       flag,        tether::Access::RW, "IDflag"),
        tether::variable(vClock,      clockvalue,  tether::Access::RO, "IDclock",      clockfields, refreshclock),
        tether::variable(vSignal,     signalvalue, tether::Access::RO, "IDsignal",     refreshsignal),
        tether::variable(vLogStatus,  logstatus,   tether::Access::RO, "IDlogSTATUS",  logstatusfields, refreshlogstatus),
#if defined(tetherdebugENABLE_wave)
        tether::variable(vViewofWave, viewof_wave, tether::Access::RO, "IDviewof_wave", refreshviewof_wave),
#endif
#if defined(tetherdebugENABLE_pwm)
        tether::variable(vViewofPwm,  viewof_pwm,  tether::Access::RO, "IDviewof_pwm",  viewofpwmfields, refreshviewof_pwm),
#endif
    };

} // namespace udpdemo::vars


// ---- the operations. the type of the argument and of the result come from the signature of the function, their names
//      (or the fields of their struct) from the table: tether.py shows them w/ list, so the user knows what to give.
//      an operation that fails tells why: result.fail("text") is the text of the nak

namespace udpdemo::ops {

    // the SharedVariables of TETHER: the operations use it to read and write the variables thread-safely
    embot::app::eth::SharedVariables &shared() { return embot::app::eth::theTETHERservice::getInstance().sharedvariables(); }

    // ---- OPmanageLED: a struct argument, no result
    // an operation takes ONE argument only, so the led and the on/off go in a struct
    struct LEDcommand
    {
        embot::hw::LED led {embot::hw::LED::one};
        bool on {false};

        constexpr LEDcommand() = default;       // needed: TETHER builds an empty one and fills it from the ROP
        constexpr explicit LEDcommand(embot::hw::LED l, bool o) : led(l), on(o) {}
    };

    constexpr tether::Field ledcommandfields[]
    {
        TETHER_FIELD(LEDcommand, led), TETHER_FIELD(LEDcommand, on)
    };

    // run[+ack]<OPmanageLED, {led=1, on=true}>
    bool manageLED(const LEDcommand &cmd, tether::Result &result)
    {
        embot::app::theLEDmanager &leds {embot::app::theLEDmanager::getInstance()};
        if(false == leds.initialised(cmd.led))
        {
            return result.fail("this led is not available");        // -> nak, nothing done
        }
        if(cmd.on) { leds.get(cmd.led).on(); } else { leds.get(cmd.led).off(); }
        return true;
    }

    // ---- OPgetUptime: no argument, a scalar result. result.set() puts the value in the ack
    // run[+ack]<OPgetUptime>   ->   ack<OPgetUptime, run, seconds=1234>
    bool getUptime(tether::ResultOf<uint32_t> &result)
    {
        return result.set(static_cast<uint32_t>(embot::core::now() / embot::core::time1second));
    }

    // ---- OPreadRegister: a scalar argument, a scalar result, or a nak
    // a fake device w/ 8 registers, constant: it lives in flash
    constexpr uint32_t registers[8] {0x00000000, 0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666, 0x77777777};

    // run[+ack]<OPreadRegister, 3>   ->   ack<OPreadRegister, run, value=858993459>
    // run[+ack]<OPreadRegister, 9>   ->   nak<OPreadRegister, run, "address out of [0, 7]">
    bool readRegister(const uint8_t &address, tether::ResultOf<uint32_t> &result)
    {
        if(address >= 8)
        {
            return result.fail("address out of [0, 7]");
        }
        return result.set(registers[address]);
    }

    // ---- OPsetMode: an enum argument, no result. it writes a variable, so it uses shared().write()
    // run[+ack]<OPsetMode, 1>   ->   ack<OPsetMode, run>     then ask<IDmode> gives 1
    bool setMode(const types::Mode &m, tether::Result &result)
    {
        if(m > types::Mode::test)
        {
            return result.fail("mode out of [0, 2]");
        }
        shared().write(vars::vMode, m, tether::Origin::local);
        return true;
    }

    // ---- OPgetClock: no argument, a struct result. it reads the variable IDclock: shared().read() calls its onread
    // run[+ack]<OPgetClock>   ->   ack<OPgetClock, run, {microseconds=.., seconds=..}>
    bool getClock(tether::ResultOf<types::Clock> &result)
    {
        types::Clock clk {};
        shared().read(vars::vClock, clk);
        return result.set(clk);
    }

    // ---- OPadd and OPdivide: two numbers go in a struct
    struct Operands
    {
        int16_t a {0};
        int16_t b {0};

        constexpr Operands() = default;         // needed: TETHER builds an empty one and fills it from the ROP
        constexpr explicit Operands(int16_t x, int16_t y) : a(x), b(y) {}
    };

    constexpr tether::Field operandsfields[]
    {
        TETHER_FIELD(Operands, a), TETHER_FIELD(Operands, b)
    };

    // a struct argument, a scalar result
    // run[+ack]<OPadd, {a=30000, b=30000}>   ->   ack<OPadd, run, sum=60000>
    bool add(const Operands &op, tether::ResultOf<int32_t> &result)
    {
        return result.set(static_cast<int32_t>(op.a) + static_cast<int32_t>(op.b));
    }

    struct Division
    {
        int32_t quotient {0};
        int32_t remainder {0};

        constexpr Division() = default;         // needed: ResultOf<> builds an empty one
        constexpr explicit Division(int32_t q, int32_t r) : quotient(q), remainder(r) {}
    };

    constexpr tether::Field divisionfields[]
    {
        TETHER_FIELD(Division, quotient), TETHER_FIELD(Division, remainder)
    };

    // a struct argument, a struct result, or a nak
    // run[+ack]<OPdivide, {a=17, b=5}>   ->   ack<OPdivide, run, {quotient=3, remainder=2}>
    // run[+ack]<OPdivide, {a=17, b=0}>   ->   nak<OPdivide, run, "division by zero">
    bool divide(const Operands &op, tether::ResultOf<Division> &result)
    {
        if(0 == op.b)
        {
            return result.fail("division by zero");
        }
        return result.set(Division {op.a / op.b, op.a % op.b});
    }

    // ---- OPaverage: an array argument (an array of scalars is a scalar form), a scalar result
    // run[+ack]<OPaverage, [1, 2, 6]>   ->   ack<OPaverage, run, mean=3>
    bool average(const float (&values)[3], tether::ResultOf<float> &result)
    {
        return result.set((values[0] + values[1] + values[2]) / 3.0f);
    }

    // ---- OPechoRESET: no argument, no result. it writes a variable that is RO for the host: an operation is the firmware,
    // so it writes w/ Origin::local. the echo thread goes on counting from zero
    // run[+ack]<OPechoRESET>   then ask<IDechoSTATS> gives {rxframes=0, ...}
    bool echoRESET(tether::Result &)
    {
        shared().write(vars::vEchoStats, types::EchoSTATS {}, tether::Origin::local);
        return true;
    }

    // ---- OPechoHELLO: no argument, no result. the socket belongs to the echo thread, so the operation only posts an event to
    // it and returns: the ack means request accepted. the echo thread sends "hello from amcmj1" to the last sender
    // run[+ack]<OPechoHELLO>   ->   the last host that used the echo port receives the frame
    bool echoHELLO(tether::Result &result)
    {
        types::EchoLAST last {};
        shared().read(vars::vEchoLast, last);
        if(0 == last.port)
        {
            return result.fail("no frame received yet on port 666");
        }
        return echo::signal(echo::evtHELLO) ? true : result.fail("echo thread not running");
    }


 #if 0

 ---- the table of the operations

 every entry is   tether::operation<function>(ID, "name" [, description of the argument]) [.returns(...)]

 - <function>   the operation itself, as a template argument. its signature is one of:
                  bool f(tether::Result &)                              no argument
                  bool f(const A &arg, tether::Result &)                one argument
                  bool f(tether::ResultOf<R> &)                         no argument, gives back an R
                  bool f(const A &arg, tether::ResultOf<R> &)           one argument, gives back an R
                it returns true -> ack, false -> nak. result.fail("text") puts the text in the nak (max 36 chars).
                an operation takes ONE argument: if it needs more, put them in a struct (see LEDcommand, Operands)

 - ID           0xA0 ... 0xDF, unique. it is what the host uses on the wire

 - "name"       the name the host uses: run[+ack]<OPname, ...>. [A-Za-z_][A-Za-z0-9_]*, max 28 chars, unique

 - description  of the argument A:  none -> nothing;  a scalar or an array -> its name;  a struct -> its fields
                (a constexpr tether::Field[] of TETHER_FIELD()). the struct must have a default constructor

 - .returns()   only w/ ResultOf<R>: the name of R (a scalar or an array) or its fields (a struct)

 all the combinations, one each in this file:
                  argument       result       function       entry
                  struct         none         manageLED      operation<manageLED>(0xA0, "OPmanageLED", ledcommandfields)
                  none           scalar       getUptime      operation<getUptime>(0xA1, "OPgetUptime").returns("seconds")
                  scalar         scalar       readRegister   operation<readRegister>(0xA2, "OPreadRegister", "address").returns("value")
                  enum           none         setMode        operation<setMode>(0xA3, "OPsetMode", "mode")
                  none           struct       getClock       operation<getClock>(0xA4, "OPgetClock").returns(vars::clockfields)
                  struct         scalar       add            operation<add>(0xA5, "OPadd", operandsfields).returns("sum")
                  struct         struct       divide         operation<divide>(0xA6, "OPdivide", operandsfields).returns(divisionfields)
                  array          scalar       average        operation<average>(0xA7, "OPaverage", "values").returns("mean")
                  none           none         echoRESET      operation<echoRESET>(0xA8, "OPechoRESET")
                  none           none         echoHELLO      operation<echoHELLO>(0xA9, "OPechoHELLO")

 the host (tether.py) needs nothing else: it reads names, types and fields from the board w/ browse and shows how to call
 every operation w/ the command `list`, e.g.  run[+ack]<OPmanageLED, {led=.., on=..}>

 #endif

    constexpr tether::Operation operations[]
    {
        tether::operation<manageLED>(0xA0, "OPmanageLED", ledcommandfields),
        tether::operation<getUptime>(0xA1, "OPgetUptime").returns("seconds"),
        tether::operation<readRegister>(0xA2, "OPreadRegister", "address").returns("value"),
        tether::operation<setMode>(0xA3, "OPsetMode", "mode"),
        tether::operation<getClock>(0xA4, "OPgetClock").returns(vars::clockfields),
        tether::operation<add>(0xA5, "OPadd", operandsfields).returns("sum"),
        tether::operation<divide>(0xA6, "OPdivide", operandsfields).returns(divisionfields),
        tether::operation<average>(0xA7, "OPaverage", "values").returns("mean"),
        tether::operation<echoRESET>(0xA8, "OPechoRESET"),
        tether::operation<echoHELLO>(0xA9, "OPechoHELLO"),
    };

} // namespace udpdemo::ops


// ---- the Dictionary. it is declared extern in dictionary.udpdemo.h, so that other files can take its address in a
//      constexpr (e.g. theTETHERservice::Config). here it is constexpr, and TETHER_VALIDATE() checks it at compile time

namespace udpdemo::dict {

    constexpr tether::Dictionary dictionary
    {
        udpdemo::vars::variables,
        udpdemo::ops::operations
    };

    TETHER_VALIDATE(dictionary);

} // namespace udpdemo::dict


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


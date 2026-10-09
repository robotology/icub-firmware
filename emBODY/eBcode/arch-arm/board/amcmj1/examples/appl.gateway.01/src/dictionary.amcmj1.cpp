

/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// the dictionary of amcmj1: the variables and the operations that the host (tether.py) sees.
//
// to add a variable or an operation, read the explanations before the tables variables[] and operations[]
// the types, the typed handles and amcmj1::dict::dictionary are declared in dictionary.amcmj1.h

// api
#include "dictionary.amcmj1.h"

// other dependencies
#include "amcmj1_ctrl.h"
#include "amcmj1_dummy.h"
#include "embot_app_eth_theTETHERservice.h"
#include "embot_app_theLEDmanager.h"
#include "embot_hw_led.h"
#include <cstdint>

namespace tether = embot::app::eth::tether;


// ---- the shared variables of amcmj1. only ID, access and name are written by hand: size, type and fields
//      come from the C++ objects
//
//      in here we make an example of how to add some variables in the Dictionary:
//      - uint64_t timeoflife: because it can be useful
//      - uint32_t dummyUINT32: as an example for a scalar value modified by the control thread
//      - dummy::DummySTRUCT dummySTRUCT: as an example for a struct value w/ an array inside
//      - float dummyARRAY[3]: as an example for an array of scalars
//      - dummy::DummyMODE dummyMODE: as an example for an enum, which is a scalar
//      - dummy::DummyCLOCK dummyCLOCK: as an example for a struct computed when it is read, w/ some padding inside
//      - three structs used by the PAXini sensor: PAXcfg, PAXstatus, PAXvalue

namespace amcmj1::vars {

    // the memory. it is accessed only through theTETHERservice::sharedvariables(), w/ the typed handles of dictionary.amcmj1.h:
    // this is why it is not visible outside this file
    namespace {

        uint64_t timeoflife {0};
        uint32_t dummyUINT32 {0};
        dummy::DummySTRUCT dummySTRUCT {};
        float dummyARRAY[3] {0.0f, 0.0f, 0.0f};
        dummy::DummyMODE dummyMODE {dummy::DummyMODE::idle};
        dummy::DummyCLOCK dummyCLOCK {};
        pax::PAXcfg paxcfg {};
        pax::PAXstatus paxstatus {};
        pax::PAXvalue paxvalue {};

    } // namespace

    // called when the variable timeoflife is accessed in reading through theTETHERservice::sharedvariables() via vTimeoflife
    void refreshtimeoflife(tether::ID, void *memory, uint8_t size)
    {
        *static_cast<uint64_t *>(memory) = static_cast<uint64_t>(embot::core::now());
    }

    // same as above but for a struct: it fills all the members that must be fresh
    void refreshdummyCLOCK(tether::ID, void *memory, uint8_t size)
    {
        dummy::DummyCLOCK *c {static_cast<dummy::DummyCLOCK *>(memory)};
        const embot::core::Time now {embot::core::now()};
        c->microseconds = static_cast<uint64_t>(now);
        c->seconds = static_cast<uint32_t>(now / embot::core::time1second);
    }

    constexpr tether::Field dummySTRUCTfields[]
    {
        TETHER_FIELD(dummy::DummySTRUCT, first), TETHER_FIELD(dummy::DummySTRUCT, second)
    };

    constexpr tether::Field dummyCLOCKfields[]
    {
        TETHER_FIELD(dummy::DummyCLOCK, microseconds), TETHER_FIELD(dummy::DummyCLOCK, seconds)
    };

    constexpr tether::Field paxcfgfields[]
    {
        TETHER_FIELD(pax::PAXcfg, par1), TETHER_FIELD(pax::PAXcfg, par2), TETHER_FIELD(pax::PAXcfg, par3),
        TETHER_FIELD(pax::PAXcfg, txperiodms)
    };

    constexpr tether::Field paxstatusfields[]
    {
        TETHER_FIELD(pax::PAXstatus, state), TETHER_FIELD(pax::PAXstatus, lastcommand),
        TETHER_FIELD(pax::PAXstatus, errors), TETHER_FIELD(pax::PAXstatus, commands)
    };

    constexpr tether::Field paxvaluefields[]
    {
        TETHER_FIELD(pax::PAXvalue, force), TETHER_FIELD(pax::PAXvalue, torque), TETHER_FIELD(pax::PAXvalue, counter)
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

 and each one exists also w/ a typed handle tether::Var<T> in place of the ID (see below).

 - ID or handle   0x01 ... 0x9F, unique. it is what the host uses on the wire. two ways to give it:
                    a typed handle  constexpr tether::Var<T> vX {0x03};  then variable(vX, object, ...)
                                    the type T of the handle must be the type of the object, or it does not compile.
                                    the firmware that reads or writes the variable w/ sharedvariables() gives the handle too,
                                    so it cannot use a wrong type: shared().write(vX, value, ...)
                    a number        variable(0x07, object, ...)
                                    no handle: fine for a variable that only the host uses.

 - object         a global (static storage): its address must be a constant. at most 36 bytes. the table only points
                  to it. the type of the object decides the form above:
                    a scalar         bool, int8_t ... int64_t, uint8_t ... uint64_t, float, double, an enum
                    an array         of any scalar, e.g. float[3]: it is a scalar form, no fields are needed
                    a struct         standard layout, trivially copyable. it needs fields.

 - Access         RO  the host cannot set it (set -> nak). the firmware can always write it.
                  RW  the host can set it too.
                  any variable, RO or RW, can be added to the stream.

 - "name"         the name the host uses: ask<IDname>, set[+ack]<IDname, value>. [A-Za-z_][A-Za-z0-9_]*, max 28 chars, unique.

 - fields         only for a struct: a constexpr tether::Field[] built w/ TETHER_FIELD(Struct, member), one for each member,
                  in order. name, type, number of elements and offset come from the real struct, so padding is described
                  correctly and the host shows the value as {member=.., member=..}. a member must be a scalar or an array
                  of scalars (no nested struct).

 - onread         optional. a function void f(tether::ID, void *memory, uint8_t size) that TETHER calls just before reading
                  the variable for an ask or for a frame of the stream: it refreshes the object, which is then copied.
                  use it for a value that is computed when it is read: no thread has to update it. it runs in the thread
                  that reads (rpc or stream): keep it short. it works for every form, a scalar, an array or a struct.

 how the memory is accessed: a scalar of 1, 2 or 4 bytes that is aligned is lock-free, everything else is protected by a
 mutex. you never choose it: shared().read(), write(), update() do it. the same for the ctrl thread and for the host.

 the six forms of this file:
   scalar, onread                 vTimeoflife   variable(vTimeoflife,  timeoflife,   Access::RO, "IDtimeoflife", refreshtimeoflife)
   scalar, no onread              vDummyUINT32  variable(vDummyUINT32, dummyUINT32,  Access::RW, "IDdummyUINT32")
   enum (a scalar)                vDummyMODE    variable(vDummyMODE,   dummyMODE,    Access::RW, "IDdummyMODE")
   array of scalars               vDummyARRAY   variable(vDummyARRAY,  dummyARRAY,   Access::RW, "IDdummyARRAY")
   struct                         vDummySTRUCT  variable(vDummySTRUCT, dummySTRUCT,  Access::RW, "IDdummySTRUCT", dummySTRUCTfields)
   struct, onread                 vDummyCLOCK   variable(vDummyCLOCK,  dummyCLOCK,   Access::RO, "IDdummyCLOCK",  dummyCLOCKfields, refreshdummyCLOCK)

 #endif

    constexpr tether::Variable variables[]
    {
        // timeoflife is a scalar object, so we use:
        // template<typename T>
        // constexpr Variable variable(ID id, T &object, Access access, const char *name, OnRead onread = nullptr)
        // note that onread = refreshtimeoflife != nullptr is called to refresh the value of timeolife when the variable is read
        tether::variable(vTimeoflife,  timeoflife,   tether::Access::RO, "IDtimeoflife", refreshtimeoflife),
        // dummyUINT32 is also a scalr so we use the above, but we dont have any onread
        tether::variable(vDummyUINT32, dummyUINT32,  tether::Access::RW, "IDdummyUINT32"),
        // dummySTRUCT is a struct, so we use:
        // template<typename T, size_t N>
        // constexpr Variable variable(ID id, T &object, Access access, const char *name, const Field (&fields)[N], OnRead onread = nullptr)
        // in here there is also const Field (&fields)[N] thta is needed to correctly access the fields         
        tether::variable(vDummySTRUCT, dummySTRUCT,  tether::Access::RW, "IDdummySTRUCT", dummySTRUCTfields),
        // similarly for the other variables that are struct 
        tether::variable(vPaxCfg,      paxcfg,       tether::Access::RO, "IDpaxCFG",      paxcfgfields),
        tether::variable(vPaxStatus,   paxstatus,    tether::Access::RO, "IDpaxSTATUS",   paxstatusfields),
        tether::variable(vPaxValue,    paxvalue,     tether::Access::RO, "IDpaxVALUE",    paxvaluefields),
        // dummyARRAY is an array of scalars, so we use the scalar form: no fields
        tether::variable(vDummyARRAY,  dummyARRAY,   tether::Access::RW, "IDdummyARRAY"),
        // dummyMODE is an enum, which is a scalar. the host sees the number
        tether::variable(vDummyMODE,   dummyMODE,    tether::Access::RW, "IDdummyMODE"),
        // dummyCLOCK is a struct w/ an onread: it is refreshed when read
        tether::variable(vDummyCLOCK,  dummyCLOCK,   tether::Access::RO, "IDdummyCLOCK",  dummyCLOCKfields, refreshdummyCLOCK),
    };

} // namespace amcmj1::vars


// ---- the operations of amcmj1. the type of the argument comes from the signature of the function, its name (or
//      the fields of its struct) from the table: tether.py shows them w/ list, so the user knows what to give.
//      an operation that fails tells why: result.fail("text") is the text of the nak
//
//      - OPmanageLED({led, on}): an example of an operation w/ a struct argument (one argument only: a struct holds more)
//      - OPgetUptime: an example of an operation w/ no argument that gives back a result
//      - OPreadRegister(address): an example of an operation w/ a scalar argument that gives back a result, or a nak
//      - OPsetMode(mode): an example of an operation w/ a scalar (an enum) argument that gives back nothing
//      - OPgetClock: an example of an operation w/ no argument that gives back a struct
//      - OPadd({a, b}): an example of an operation w/ a struct argument that gives back a scalar
//      - OPdivide({a, b}): an example of an operation w/ a struct argument that gives back a struct, or a nak
//      - OPaverage(values): an example of an operation w/ an array argument that gives back a scalar
//      - OPpaxINIT, OPpaxDEINIT, OPpaxSTART, OPpaxSTOP: they configure and run the PX3Q. they run in the rpc thread: they post
//        an event to the thread that owns the CAN and return. the ack means: request accepted

namespace amcmj1::ops {

    // the argument of OPmanageLED: an operation takes ONE argument only, so the led and the on/off go in a struct
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

    // manageLED() is executed by the rpc thread at reception of following ROP. It does not use shared data
    // run[+ack]<OPmanageLED, {led=0, on=true}>
    bool manageLED(const LEDcommand &cmd, tether::Result &result)
    { 
        // we dont use shared data
        embot::app::theLEDmanager &leds {embot::app::theLEDmanager::getInstance()};
        if(false == leds.initialised(cmd.led))
        {
            return result.fail("this led is not available");        // -> nak, nothing done
        }

        if(cmd.on) { leds.get(cmd.led).on(); } else { leds.get(cmd.led).off(); }
        return true;
    }

    // neeeded to access shared data.
    embot::app::eth::SharedVariables &shared() { return embot::app::eth::theTETHERservice::getInstance().sharedvariables(); }

    // it posts the event to the ctrl thread. false (-> nak) if the ctrl thread is not running
    bool signalctrl(embot::os::Event evt, tether::Result &result)
    {
        return ctrl::signal(evt) ? true : result.fail("ctrl thread not running");
    }

    // paxINIT() is executed by the rpc thread at reception of following ROP. 
    // it changes shared data, so we do it w/ shared().write() that uses the required protection, in this case a mutex.
    // run[+ack]<OPpaxINIT, {par1=1, par2=2, par3=3, txperiodms=10}>
    bool paxINIT(const pax::PAXcfg &cfg, tether::Result &result)
    {
        static_assert((1 == pax::txperiodmin) && (1000 == pax::txperiodmax), "update the text of the nak below");
        if((cfg.txperiodms < pax::txperiodmin) || (cfg.txperiodms > pax::txperiodmax))
        {
            return result.fail("txperiodms out of [1, 1000]");      // -> nak, nothing stored
        }
        // in here we access shared data to modify it. shared().write() use the required protection, in this case a mutex 
        shared().write(vars::vPaxCfg, cfg, tether::Origin::local);  // the operation is the firmware: local
        // and then we alert the ctrl thread so that can use PaxCfg
        return signalctrl(ctrl::evtPAXinit, result);
    }

    bool paxDEINIT(tether::Result &result) { return signalctrl(ctrl::evtPAXdeinit, result); }

    // nak at once if the PX3Q is not initialised: the ctrl thread checks it again
    bool paxSTART(tether::Result &result)
    {
        pax::PAXstatus st {};
        shared().read(vars::vPaxStatus, st);
        if(pax::State::initialised != st.state)
        {
            return result.fail("px3q not initialised");
        }
        return signalctrl(ctrl::evtPAXstart, result);
    }

    bool paxSTOP(tether::Result &result) 
    { 
        return signalctrl(ctrl::evtPAXstop, result); 
    }

    // getUptime() has no argument and gives back a result: bool f(tether::ResultOf<R> &)
    // result.set() puts the value in the ack. it does not use shared data
    // run[+ack]<OPgetUptime>   ->   ack<OPgetUptime, run, seconds=1234>
    bool getUptime(tether::ResultOf<uint32_t> &result)
    {
        return result.set(static_cast<uint32_t>(embot::core::now() / embot::core::time1second));
    }

    // a fake device w/ 8 registers, just to give readRegister() something to read
    uint32_t registers[8] {0, 1, 2, 3, 4, 5, 6, 7};

    // readRegister() has a scalar argument and gives back a result: bool f(const A &arg, tether::ResultOf<R> &)
    // run[+ack]<OPreadRegister, 3>   ->   ack<OPreadRegister, run, value=3>
    bool readRegister(const uint8_t &address, tether::ResultOf<uint32_t> &result)
    {
        if(address >= 8)
        {
            return result.fail("address out of [0, 7]");           // -> nak<OPreadRegister, run, "address out of [0, 7]">
        }
        return result.set(registers[address]);                      // -> ack<OPreadRegister, run, value=...>
    }

    // setMode() has a scalar argument (an enum) and gives back nothing: bool f(const A &arg, tether::Result &)
    // it changes shared data, so we use shared().write(). the mode is also a variable the host can ask
    // run[+ack]<OPsetMode, 1>   ->   ack<OPsetMode, run>
    bool setMode(const dummy::DummyMODE &mode, tether::Result &result)
    {
        if(mode > dummy::DummyMODE::test)
        {
            return result.fail("mode out of [0, 2]");              // -> nak, nothing stored
        }
        shared().write(vars::vDummyMODE, mode, tether::Origin::local);
        return true;
    }

    // getClock() has no argument and gives back a struct: bool f(tether::ResultOf<R> &), w/ R a struct.
    // it reads the same variable the host can ask w/ ask<IDdummyCLOCK>: shared().read() calls its onread
    // run[+ack]<OPgetClock>   ->   ack<OPgetClock, run, {microseconds=.., seconds=..}>
    bool getClock(tether::ResultOf<dummy::DummyCLOCK> &result)
    {
        dummy::DummyCLOCK clk {};
        shared().read(vars::vDummyCLOCK, clk);
        return result.set(clk);
    }

    // the argument of OPadd and OPdivide: two numbers go in a struct
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

    // add() has a struct argument and gives back a scalar: bool f(const A &arg, tether::ResultOf<R> &)
    // run[+ack]<OPadd, {a=30000, b=30000}>   ->   ack<OPadd, run, sum=60000>
    bool add(const Operands &op, tether::ResultOf<int32_t> &result)
    {
        return result.set(static_cast<int32_t>(op.a) + static_cast<int32_t>(op.b));
    }

    // the result of OPdivide: a struct
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

    // divide() has a struct argument and gives back a struct, or a nak
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

    // average() has an array argument (an array of scalars is a scalar form) and gives back a scalar
    // run[+ack]<OPaverage, [1, 2, 6]>   ->   ack<OPaverage, run, mean=3>
    bool average(const float (&values)[3], tether::ResultOf<float> &result)
    {
        return result.set((values[0] + values[1] + values[2]) / 3.0f);
    }


 #if 0
    
 ---- the table of the operations

 every entry is   tether::operation<function>(ID, "name" [, description of the argument]) [.returns(...)]

 there are three forms, according to the argument of <function>:

   no argument:
     template<auto F>
     constexpr Operation operation(ID op, const char *name)

   a scalar argument, or an array of scalars:
     template<auto F>
     constexpr Operation operation(ID op, const char *name, const char *argname)

   a struct argument:
     template<auto F, size_t N>
     constexpr Operation operation(ID op, const char *name, const Field (&fields)[N])

 and if <function> gives back a result, the entry is completed w/ .returns(...) (see below).

 - <function>   the operation itself, given as a template argument: TETHER takes from its signature the type and
                the size of the argument and whether it gives back a result. the signature must be one of:
                  bool f(tether::Result &)                              no argument
                  bool f(const A &arg, tether::Result &)                one argument
                  bool f(tether::ResultOf<R> &)                         no argument, gives back an R
                  bool f(const A &arg, tether::ResultOf<R> &)           one argument, gives back an R
                it returns true -> ack, false -> nak. result.fail("text") puts the text in the nak (max 36 chars).
                an operation takes ONE argument: if it needs more, put them in a struct (see LEDcommand, Operands).

 - ID           0xA0 ... 0xDF, unique. it is what the host uses on the wire.

 - "name"       the name the host uses: run[+ack]<OPname, ...>. [A-Za-z_][A-Za-z0-9_]*, max 28 chars, unique.

 - description  depends on the argument A of <function>:
                  no argument       nothing:                  operation<paxSTART>(0xC3, "OPpaxSTART")
                  a scalar          its name, a const char*:  operation<readRegister>(0xC6, "OPreadRegister", "address")
                                    (bool, int8_t ... int64_t, float, double, an enum)
                  an array          of scalars: the same, its name. A is const float (&)[3] for instance:
                                      operation<average>(0xCB, "OPaverage", "values")
                  a struct          its fields, a constexpr tether::Field[] built w/ TETHER_FIELD(Struct, member),
                                    one for each member, in order: name, type and offset come from the real struct
                                    (so padding is described correctly):
                                      operation<manageLED>(0xC0, "OPmanageLED", ledcommandfields)
                                    the struct must be standard layout, trivially copyable and default
                                    constructible (so it needs a default constructor), at most 36 bytes.
                the argument must be described: if <function> has an argument and the description is missing (or the
                other way around) it does not compile.

 - .returns()   only if <function> uses ResultOf<R>: it names what the ack carries back. R is at most 36 bytes
                  R is a scalar    its name:                 operation<getUptime>(0xC5, "OPgetUptime").returns("seconds")
                                                              operation<readRegister>(0xC6, "OPreadRegister", "address").returns("value")
                  R is an array    of scalars: the same, its name
                  R is a struct    its fields, a constexpr tether::Field[] as for an argument:
                                                              operation<getClock>(0xC8, "OPgetClock").returns(vars::dummyCLOCKfields)
                the host then shows   ack<OPgetUptime, run, seconds=1234>   or   ack<OPgetClock, run, {microseconds=.., seconds=..}>
                if the function fails, result.fail("text") sends nak<OPreadRegister, run, "text"> instead (max 36 chars)
                and no result is sent.

 all the combinations, w/ an example of each in this file:
                  argument       result       function       entry
                  none           none         paxSTART       operation<paxSTART>(0xC3, "OPpaxSTART")
                  none           scalar       getUptime      operation<getUptime>(0xC5, "OPgetUptime").returns("seconds")
                  none           struct       getClock       operation<getClock>(0xC8, "OPgetClock").returns(vars::dummyCLOCKfields)
                  scalar         none         setMode        operation<setMode>(0xC7, "OPsetMode", "mode")
                  scalar         scalar       readRegister   operation<readRegister>(0xC6, "OPreadRegister", "address").returns("value")
                  array          scalar       average        operation<average>(0xCB, "OPaverage", "values").returns("mean")
                  struct         none         manageLED      operation<manageLED>(0xC0, "OPmanageLED", ledcommandfields)
                  struct         scalar       add            operation<add>(0xC9, "OPadd", operandsfields).returns("sum")
                  struct         struct       divide         operation<divide>(0xCA, "OPdivide", operandsfields).returns(divisionfields)
                  (scalar          struct       is the same as the others: the argument by its name, the result by its fields)

 the host (tether.py) needs nothing else: it reads names, types and fields from the board w/ browse and shows
 how to call every operation w/ the command `list`, e.g.  run[+ack]<OPmanageLED, {led=.., on=..}>

 here:
   OPmanageLED    argument: struct LEDcommand {led, on}   -> fields ledcommandfields
   OPpaxINIT      argument: struct pax::PAXcfg            -> fields vars::paxcfgfields (the same of IDpaxCFG)
   OPpaxDEINIT    no argument
   OPpaxSTART     no argument
   OPpaxSTOP      no argument
   OPgetUptime    no argument, gives back a uint32_t       -> .returns("seconds")
   OPreadRegister argument: uint8_t "address", gives back a uint32_t  -> .returns("value")
   OPsetMode      argument: enum dummy::DummyMODE "mode", gives back nothing
   OPgetClock     no argument, gives back a struct dummy::DummyCLOCK -> .returns(vars::dummyCLOCKfields) (the same of IDdummyCLOCK)
   OPadd          argument: struct Operands {a, b}, gives back an int32_t -> .returns("sum")
   OPdivide       argument: struct Operands {a, b}, gives back a struct Division -> .returns(divisionfields)
   OPaverage      argument: float[3] "values", gives back a float -> .returns("mean")
   
#endif

    constexpr tether::Operation operations[]
    {
        tether::operation<manageLED>(0xC0, "OPmanageLED", ledcommandfields),
        tether::operation<paxINIT>(0xC1, "OPpaxINIT", vars::paxcfgfields),
        tether::operation<paxDEINIT>(0xC2, "OPpaxDEINIT"),
        tether::operation<paxSTART>(0xC3, "OPpaxSTART"),
        tether::operation<paxSTOP>(0xC4, "OPpaxSTOP"),
        tether::operation<getUptime>(0xC5, "OPgetUptime").returns("seconds"),
        tether::operation<readRegister>(0xC6, "OPreadRegister", "address").returns("value"),
        tether::operation<setMode>(0xC7, "OPsetMode", "mode"),
        tether::operation<getClock>(0xC8, "OPgetClock").returns(vars::dummyCLOCKfields),
        tether::operation<add>(0xC9, "OPadd", operandsfields).returns("sum"),
        tether::operation<divide>(0xCA, "OPdivide", operandsfields).returns(divisionfields),
        tether::operation<average>(0xCB, "OPaverage", "values").returns("mean"),
    };

} // namespace amcmj1::ops


// ---- the dictionary. it is declared extern in dictionary.amcmj1.h, so that other files can take its address
//      in a constexpr (e.g. theTETHERservice::Config). here it is constexpr, and TETHER_VALIDATE() checks it at compile time

namespace amcmj1::dict {

    constexpr tether::Dictionary dictionary
    {
        amcmj1::vars::variables,
        amcmj1::ops::operations
    };

    TETHER_VALIDATE(dictionary);

} // namespace amcmj1::dict


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THERPCMANAGER_H_
#define __EMBOT_APP_ETH_THERPCMANAGER_H_

#include "embot_core.h"
#include "embot_app_eth_ROP.h"
#include "embot_app_eth_SharedVariables.h"
#include "embot_app_eth_theSTRmanager.h"
#include <array>


namespace embot::app::eth::rop {

    // the manager of the rpc socket. it is used only by the rpc thread.
    // it executes the received ROPs and forms the frame w/ the replies. the range of the ID decides the target:
    //   ROP::isvariable(id) -> the SharedVariables
    //   ROP::isop(id)       -> the operations given in Config
    //   ROP::ismg(id)       -> theRPCmanager itself: streaming (via theSTRmanager), browse, version, stats
    //
    //   ping<ID>            -> ack<ID, ping>, for any ID
    //   ask<ID>             -> say<ID, value> or nak<ID, ask, "why">
    //   set[+ack]<ID, v>    -> ack<ID, set, value read back> or nak<ID, set, "why">
    //   set<ID, v>          -> nothing
    //   sig<ID, v>          -> nothing (same as set w/out +ack)
    //   run[+ack]<OP, arg>  -> ack<OP, run, result> or nak<OP, run, "why">
    //   run<OP, arg>        -> nothing
    //   a CMD not valid for the range of the ID -> nak if the sender expects a reply (ask or +ack), nothing otherwise
    //
    // the text of a nak: the one given by the operation (Result::fail), else a standard one of the manager, e.g.
    // "unknown id", "read only", "wrong size", "operation failed". at most ROP::datasize characters
    //
    // browse: the variables and the operations describe themselves (name, size, access, type, fields of a struct;
    // for an operation also the argument and the result),
    // so the host does not need a table kept aligned by hand. their CRC32 tells the host when to browse again.
    //
    // the operations run inside the rpc thread: a long one (e.g. a CAN configuration that waits for replies)
    // delays the following ROPs. if it must run elsewhere, its callback can just post an event.
    class theRPCmanager
    {
    public:

        static theRPCmanager &getInstance();

        using Type = embot::app::eth::SharedVariables::Type;

        // what an operation receives and can give back
        struct OPcall
        {
            ID op {0};
            const uint8_t *arg {nullptr};           // the data of the run ROP
            uint8_t argsize {0};                    // == OPentry::argsize, already checked
            embot::net::eth::IPaddress from {};          // who asked for it
            uint8_t result[ROP::datasize] {0};      // optional output, carried by the ack
            uint8_t resultsize {0};
            char error[ROP::datasize] {0};          // optional text, carried by the nak if the operation fails
            uint8_t errorsize {0};

            OPcall() = default;
            explicit OPcall(ID o, const uint8_t *a, uint8_t s, const embot::net::eth::IPaddress &f) : op(o), arg(a), argsize(s), from(f) {}
        };

        // it returns true if the operation succeeded (-> ack), false otherwise (-> nak, w/ the text in call.error if any)
        using fpRUN = bool (*)(OPcall &call, void *param);

        // an operation, its argument and its result, described for the browse so that the host shows how to call it
        // and what it gives back. the argument and the result have the same description:
        // - none:                 size 0, Type::raw, no name, no fields
        // - a scalar or an array: the type is the scalar (bool, u8 ... i64, f32, f64), the name is its name (e.g. "periodms")
        // - a struct:             Type::structure, its fields (as the ones of a variable)
        // - raw bytes:            Type::raw w/ size > 0, w/out a name. only for who builds the table by hand
        struct OPentry
        {
            using Field = embot::app::eth::SharedVariables::Field;

            ID op {0};
            uint8_t argsize {0};                    // a run w/ a different size gets a nak w/out calling run
            Type argtype {Type::raw};
            const char *argname {nullptr};          // only for a scalar (or an array) argument
            const Field *argfields {nullptr};       // only for a struct argument
            uint8_t numberofargfields {0};
            uint8_t resultsize {0};                 // what Result::set() gives back w/ the ack
            Type resulttype {Type::raw};
            const char *resultname {nullptr};       // only for a scalar (or an array) result
            const Field *resultfields {nullptr};    // only for a struct result
            uint8_t numberofresultfields {0};
            fpRUN run {nullptr};
            void *param {nullptr};
            const char *name {nullptr};             // e.g. "OPsensorTRQinit". [A-Za-z_][A-Za-z0-9_]*, max 28 chars

            constexpr OPentry() = default;
            constexpr explicit OPentry(ID o, uint8_t as, Type at, const char *an, const Field *af, uint8_t naf,
                                       uint8_t rs, Type rt, const char *rn, const Field *rf, uint8_t nrf,
                                       fpRUN r, void *p, const char *n)
                : op(o), argsize(as), argtype(at), argname(an), argfields(af), numberofargfields(naf),
                  resultsize(rs), resulttype(rt), resultname(rn), resultfields(rf), numberofresultfields(nrf),
                  run(r), param(p), name(n) {}

            // the same operation w/ the name of its result (a scalar or an array), or the fields of its result (a struct).
            // the type and the size of the result come from the signature of the function
            constexpr OPentry returns(const char *rn) const
            {
                OPentry e {*this};
                e.resultname = rn;
                return e;
            }

            template<size_t N>
            constexpr OPentry returns(const Field (&rf)[N]) const
            {
                OPentry e {*this};
                e.resultfields = rf;
                e.numberofresultfields = static_cast<uint8_t>(N);
                return e;
            }

            // the fields given by the browse: 1 for a named scalar (or array), the fields of a struct, else 0
            constexpr uint8_t argfieldscount() const { return count(argtype, argname, numberofargfields); }
            constexpr uint8_t resultfieldscount() const { return count(resulttype, resultname, numberofresultfields); }
            // all of them: first the ones of the argument, then the ones of the result
            constexpr uint8_t fieldscount() const { return static_cast<uint8_t>(argfieldscount() + resultfieldscount()); }

            constexpr bool argfield(uint8_t n, Field &f) const { return fieldof(argtype, argsize, argname, argfields, argfieldscount(), n, f); }
            constexpr bool resultfield(uint8_t n, Field &f) const { return fieldof(resulttype, resultsize, resultname, resultfields, resultfieldscount(), n, f); }
            constexpr bool field(uint8_t n, Field &f) const
            {
                return (n < argfieldscount()) ? argfield(n, f) : resultfield(static_cast<uint8_t>(n - argfieldscount()), f);
            }

            // also at compile time (see TETHER_VALIDATE). the names are checked apart
            constexpr bool isvalid() const
            {
                if((false == ROP::isop(op)) || (nullptr == run) || (argsize > ROP::datasize) || (resultsize > ROP::datasize))
                {
                    return false;
                }
                return validtype(argtype, argsize, argname, argfields, numberofargfields) &&
                       validtype(resulttype, resultsize, resultname, resultfields, numberofresultfields);
            }

        private:
            static constexpr uint8_t count(Type t, const char *name, uint8_t nfields)
            {
                return (Type::structure == t) ? nfields : ((nullptr != name) ? 1 : 0);
            }

            static constexpr bool fieldof(Type t, uint8_t size, const char *name, const Field *fields, uint8_t n, uint8_t i, Field &f)
            {
                using DIC = embot::app::eth::SharedVariables;
                if(i >= n)
                {
                    return false;
                }
                if(Type::structure == t)
                {
                    f = fields[i];
                }
                else
                {
                    f = Field {name, t, static_cast<uint8_t>(size / DIC::sizeoftype(t)), 0};
                }
                return true;
            }

            static constexpr bool validtype(Type t, uint8_t size, const char *name, const Field *fields, uint8_t nfields)
            {
                using DIC = embot::app::eth::SharedVariables;
                using LUT = embot::app::eth::SharedVariablesLUT;
                if(Type::structure == t)
                {
                    return (nullptr == name) && LUT::isvalid(fields, nfields, size);
                }
                if((nullptr != fields) || (0 != nfields))
                {
                    return false;
                }
                if(Type::raw == t)
                {
                    return nullptr == name;
                }
                return DIC::isscalar(t) && (size > 0) && (0 == (size % DIC::sizeoftype(t))) && (nullptr != name);
            }
        };

        struct Config
        {
            embot::app::eth::SharedVariables *sharedvariables {nullptr};
            theSTRmanager *streamer {nullptr};      // if nullptr the MGstreamXXX get a nak
            const OPentry *ops {nullptr};           // table of the operations. it must outlive theRPCmanager
            size_t numberofops {0};
            fpRUN fallback {nullptr};               // optional: called for an OP not in the table. not visible by browse
            void *fallbackparam {nullptr};
            uint32_t applicationversion {0};        // the version of the application, given by MGversion

            Config() = default;
            explicit Config(embot::app::eth::SharedVariables *v, theSTRmanager *s, const OPentry *o, size_t no,
                            fpRUN fb, void *fbp, uint32_t av)
                : sharedvariables(v), streamer(s), ops(o), numberofops(no), fallback(fb), fallbackparam(fbp), applicationversion(av) {}
            bool isvalid() const { return (nullptr != sharedvariables) && ((0 == numberofops) || (nullptr != ops)); }
        };

        struct Stats
        {
            uint32_t rxframes {0};
            uint32_t rxwrongframes {0};
            uint32_t rxROPs {0};
            uint32_t droppedROPs {0};     // replies not added to the tx payload because it was full

            Stats() = default;
        };

        // it validates the sharedvariables and computes its CRC32. it fails if:
        // - a variable is not in [ROP::VARfirst, ROP::VARlast] or is bigger than ROP::datasize
        // - an operation is not in [ROP::OPfirst, ROP::OPlast], is duplicated or is not valid
        // - a name is not [A-Za-z_][A-Za-z0-9_]* w/ at most 28 chars, or is used twice
        bool initialise(const Config &config);

        // it parses a received UDP payload, executes each ROP and prepares the replies.
        // from is the IP of the sender: a ROP that starts the stream sends it there, and any valid frame refreshes
        // the keepalive of the stream
        Error parse(const void *payload, size_t size, const embot::net::eth::IPaddress &from);

        // it gives the reply, w/ the slots of the smallest size and the header stamped w/ embot::core::now().
        // false if there is nothing to reply. memory valid until the next parse()
        bool get(const void *&payload, size_t &size);

        uint32_t crc32() const { return _crc32; }
        const Stats &stats() const { return _stats; }

    private:
        theRPCmanager() = default;
        ~theRPCmanager() = default;

    public:
        theRPCmanager(const theRPCmanager &) = delete;
        theRPCmanager &operator=(const theRPCmanager &) = delete;
        theRPCmanager(theRPCmanager &&) = delete;
        theRPCmanager &operator=(theRPCmanager &&) = delete;

    private:
        void executevariable(const ROP &rx, ROP &reply);
        void executeop(const ROP &rx, ROP &reply, const embot::net::eth::IPaddress &from);
        void executemg(const ROP &rx, ROP &reply, const embot::net::eth::IPaddress &from);
        bool describeitem(uint8_t index, BrowseItem &item) const;
        bool getfield(ID id, uint8_t n, embot::app::eth::SharedVariables::Field &field) const;   // of a variable or of an argument
        bool describefield(ID id, uint8_t n, BrowseField &field) const;
        uint32_t computecrc32() const;
        bool add(const ROP &rop);

        Config _config {};
        bool _initialised {false};
        Stats _stats {};
        std::array<int8_t, ROP::OPlast - ROP::OPfirst + 1> _opindex {};     // op -> index in _config.ops, -1 if none
        uint8_t _numberofvariables {0};
        uint32_t _crc32 {0};

        // the replies are added at the max slot size and compacted to the smallest one by get()
        uint8_t _numberofROPs {0};
        bool _compacted {false};
        size_t _txsize {0};
        alignas(8) std::array<uint8_t, maxsizeofframe> _txbuffer {};
    };

} // namespace embot::app::eth::rop


#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

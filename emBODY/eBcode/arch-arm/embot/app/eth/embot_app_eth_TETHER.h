
/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_TETHER_H_
#define __EMBOT_APP_ETH_TETHER_H_

#include "embot_core.h"
#include "embot_app_eth_ROP.h"
#include "embot_app_eth_SharedVariables.h"
#include "embot_app_eth_theRPCmanager.h"
#include <cstddef>
#include <cstring>
#include <type_traits>


// TETHER: the constexpr Dictionary of a board, i.e. the description of its variables and operations.
//
// you write only what the compiler cannot know: the ID, the access and the name. the rest, size and type, comes from
// the C++ objects, the fields of a struct from TETHER_FIELD(), the type of the argument and of the result of an
// operation from the signature of its function. the argument and the result are a scalar, an array of scalars or a
// struct: a scalar (or an array) has a name, a struct its fields, and the host shows them, so the user knows what
// to give and what to expect. TETHER_VALIDATE() checks the whole Dictionary at compile time.
//
//   uint32_t dummyUINT32 {0};
//   DummySTRUCT dummySTRUCT {};
//
//   constexpr tether::Var<uint32_t> vDummyUINT32 {0x02};       // optional: a typed handle for the ctrl thread
//
//   constexpr tether::Field dummySTRUCTfields[] { TETHER_FIELD(DummySTRUCT, a), TETHER_FIELD(DummySTRUCT, b) };
//
//   constexpr tether::Variable variables[] {
//       tether::variable(vDummyUINT32, dummyUINT32, tether::Access::RW, "IDdummyUINT32"),
//       tether::variable(0x03, dummySTRUCT, tether::Access::RW, "IDdummySTRUCT", dummySTRUCTfields)
//   };
//
//   bool dummyECHO(const uint32_t &arg, tether::ResultOf<uint32_t> &result) { return result.set(arg + 1); }
//   bool sensorTRQinit(tether::Result &result) { ... return true; }
//   bool sensorTRQstartTX(const TXconfig &cfg, tether::Result &result)
//   {
//       if(0 == cfg.periodms) { return result.fail("periodms must be > 0"); }       // the text of the nak
//       ...
//       return true;
//   }
//
//   constexpr tether::Field txconfigfields[] { TETHER_FIELD(TXconfig, periodms), TETHER_FIELD(TXconfig, channels) };
//
//   constexpr tether::Operation operations[] {
//       tether::operation<dummyECHO>(0xC0, "OPdummyECHO", "value").returns("echo"),      // a scalar: its name. it gives back "echo"
//       tether::operation<sensorTRQinit>(0xC1, "OPsensorTRQinit"),                       // no argument, no result
//       tether::operation<sensorTRQstartTX>(0xC3, "OPsensorTRQstartTX", txconfigfields)  // a struct: its fields
//   };
//
//   the host then shows:  run[+ack]<OPdummyECHO, value> -> echo   and   run[+ack]<OPsensorTRQstartTX, {periodms=.., channels=..}>
//
//   constexpr tether::Dictionary dictionary {variables, operations};
//   TETHER_VALIDATE(dictionary);
//
// the acquisition time of a value: a frame of the stream has a single time, taken when the Dictionary is read. a
// variable that needs its own acquisition time carries it: a struct w/ a uint64_t timestamp (8 bytes, in microseconds
// of embot::core::now()). mind that a uint64_t is aligned to 8 bytes, so such a struct is at most 32 bytes.

namespace embot::app::eth::tether {

    using ID = embot::app::eth::rop::ID;
    using Type = embot::app::eth::SharedVariables::Type;
    using Access = embot::app::eth::SharedVariables::Access;
    using Origin = embot::app::eth::SharedVariables::Origin;
    using Field = embot::app::eth::SharedVariables::Field;
    using Variable = embot::app::eth::SharedVariablesLUT::Entry;
    using OnRead = embot::app::eth::SharedVariablesLUT::Entry::fpOnRead;
    using Operation = embot::app::eth::rop::theRPCmanager::OPentry;
    template<typename T> using Var = embot::app::eth::SharedVariables::Handle<T>;

    constexpr size_t datasize {embot::app::eth::rop::ROP::datasize};

    namespace detail {

        // the element type and the number of elements of T or T[N]
        template<typename T> struct element { using type = T; static constexpr size_t count {1}; };
        template<typename T, size_t N> struct element<T[N]> { using type = T; static constexpr size_t count {N}; };

        // the TETHER type of a C++ scalar, by size and signedness so that it does not depend on how the platform
        // spells uint32_t. Type::raw if T is not a scalar
        template<typename T>
        constexpr Type scalartype()
        {
            using U = std::remove_cv_t<T>;
            if constexpr (std::is_same_v<U, bool>)
            {
                return Type::boolean;
            }
            else if constexpr (std::is_enum_v<U>)
            {
                return scalartype<std::underlying_type_t<U>>();
            }
            else if constexpr (std::is_integral_v<U>)
            {
                constexpr bool s {std::is_signed_v<U>};
                if constexpr (1 == sizeof(U)) { return s ? Type::i8 : Type::u8; }
                else if constexpr (2 == sizeof(U)) { return s ? Type::i16 : Type::u16; }
                else if constexpr (4 == sizeof(U)) { return s ? Type::i32 : Type::u32; }
                else if constexpr (8 == sizeof(U)) { return s ? Type::i64 : Type::u64; }
                else { return Type::raw; }
            }
            else if constexpr (std::is_floating_point_v<U>)
            {
                if constexpr (4 == sizeof(U)) { return Type::f32; }
                else if constexpr (8 == sizeof(U)) { return Type::f64; }
                else { return Type::raw; }
            }
            else
            {
                return Type::raw;
            }
        }

        template<typename T>
        constexpr bool isscalar {Type::raw != scalartype<typename element<std::remove_cv_t<T>>::type>()};

    } // namespace detail


    // ---- fields

    // use TETHER_FIELD(S, m) instead: it gives name and offset
    template<typename M>
    constexpr Field field(const char *name, size_t offset)
    {
        using E = typename detail::element<std::remove_cv_t<M>>::type;
        constexpr size_t count {detail::element<std::remove_cv_t<M>>::count};
        static_assert(Type::raw != detail::scalartype<E>(), "TETHER: a field must be a scalar or an array of scalars");
        static_assert(count * sizeof(E) <= datasize, "TETHER: a field must fit a ROP: at most 36 bytes");
        return Field {name, detail::scalartype<E>(), static_cast<uint8_t>(count), static_cast<uint8_t>(offset)};
    }

    // the name, the type, the number of elements and the offset of member m of struct S
    #define TETHER_FIELD(S, m) ::embot::app::eth::tether::field<decltype(S::m)>(#m, offsetof(S, m))


    // ---- variables: the object must have static storage (e.g. a global), so its address is a constant

    // a scalar, or an array of scalars
    template<typename T>
    constexpr Variable variable(ID id, T &object, Access access, const char *name, OnRead onread = nullptr)
    {
        static_assert(sizeof(T) <= datasize, "TETHER: a variable must fit a ROP: at most 36 bytes");
        static_assert(detail::isscalar<T>, "TETHER: a struct needs its fields: use the overload w/ an array of TETHER_FIELD()");
        using E = typename detail::element<std::remove_cv_t<T>>::type;
        return Variable {id, static_cast<uint8_t>(sizeof(T)), &object, access, detail::scalartype<E>(), nullptr, 0, onread, name};
    }

    // a struct, w/ its fields
    template<typename T, size_t N>
    constexpr Variable variable(ID id, T &object, Access access, const char *name, const Field (&fields)[N], OnRead onread = nullptr)
    {
        static_assert(std::is_class_v<T>, "TETHER: only a struct has fields");
        static_assert(sizeof(T) <= datasize, "TETHER: a variable must fit a ROP: at most 36 bytes");
        static_assert(std::is_standard_layout_v<T> && std::is_trivially_copyable_v<T>,
                      "TETHER: a struct must be standard layout and trivially copyable");
        static_assert(N <= 255, "TETHER: too many fields");
        return Variable {id, static_cast<uint8_t>(sizeof(T)), &object, access, Type::structure, fields, static_cast<uint8_t>(N), onread, name};
    }

    // the same w/ a typed handle: the type of the object must be the type of the handle
    template<typename T>
    constexpr Variable variable(Var<T> var, T &object, Access access, const char *name, OnRead onread = nullptr)
    {
        return variable(var.id, object, access, name, onread);
    }

    template<typename T, size_t N>
    constexpr Variable variable(Var<T> var, T &object, Access access, const char *name, const Field (&fields)[N], OnRead onread = nullptr)
    {
        return variable(var.id, object, access, name, fields, onread);
    }


    // ---- operations

    // what an operation can do with its call: tell why it failed, and know who asked for it. this is the Result of an
    // operation that gives nothing back:  bool f(tether::Result &)
    class Result
    {
    public:
        using OPcall = embot::app::eth::rop::theRPCmanager::OPcall;

        explicit Result(OPcall &call) : _call(call) {}

        // the text of the nak, at most 36 characters (the others are cut). it returns false, so an operation can end w/
        //   return result.fail("txperiodms out of [1, 1000]");
        bool fail(const char *text)
        {
            uint8_t n {0};
            while((nullptr != text) && (0 != text[n]) && (n < datasize))
            {
                _call.error[n] = text[n];
                n++;
            }
            _call.errorsize = n;
            return false;
        }

        // the IP of who asked for the operation
        embot::net::eth::IPaddress from() const { return _call.from; }

    protected:
        OPcall &_call;
    };

    // the Result of an operation that gives back an R with its ack:  bool f(tether::ResultOf<R> &)
    // R is a scalar, an array of scalars or a struct. its description (the name, or the fields) is given to the table
    // w/ operation<f>(...).returns(...), so the host knows what the ack carries
    template<typename R>
    class ResultOf : public Result
    {
    public:
        static_assert(std::is_trivially_copyable_v<R>, "TETHER: a result must be trivially copyable");
        static_assert(sizeof(R) <= datasize, "TETHER: a result must fit a ROP: at most 36 bytes");

        explicit ResultOf(OPcall &call) : Result(call) {}

        bool set(const R &value)
        {
            std::memcpy(_call.result, &value, sizeof(R));
            _call.resultsize = static_cast<uint8_t>(sizeof(R));
            return true;
        }
    };

    namespace detail {

        // an operation is  bool f(Result &),  bool f(ResultOf<R> &),  bool f(const A &arg, Result &)  or
        // bool f(const A &arg, ResultOf<R> &)
        template<typename F> struct optraits
        {
            static_assert(sizeof(F) == 0, "TETHER: an operation must be bool f(tether::Result &) or bool f(const A &arg, tether::Result &), or w/ a tether::ResultOf<R> & to give back an R");
        };

        template<> struct optraits<bool (*)(Result &)>
        {
            static constexpr bool hasarg {false};
            static constexpr bool hasresult {false};
            using arg = void;
            using res = void;
            using result = Result;
        };

        template<typename R> struct optraits<bool (*)(ResultOf<R> &)>
        {
            static constexpr bool hasarg {false};
            static constexpr bool hasresult {true};
            using arg = void;
            using res = R;
            using result = ResultOf<R>;
        };

        template<typename A> struct optraits<bool (*)(const A &, Result &)>
        {
            static constexpr bool hasarg {true};
            static constexpr bool hasresult {false};
            using arg = A;
            using res = void;
            using result = Result;
        };

        template<typename A, typename R> struct optraits<bool (*)(const A &, ResultOf<R> &)>
        {
            static constexpr bool hasarg {true};
            static constexpr bool hasresult {true};
            using arg = A;
            using res = R;
            using result = ResultOf<R>;
        };

        // the adapter called by theRPCmanager, generated at compile time for each operation. theRPCmanager has
        // already checked that the size of the argument is sizeof(A)
        template<auto F>
        bool adapter(embot::app::eth::rop::theRPCmanager::OPcall &call, void *)
        {
            using traits = optraits<decltype(F)>;
            typename traits::result result {call};
            if constexpr (traits::hasarg)
            {
                typename traits::arg arg {};
                std::memcpy(&arg, call.arg, sizeof(arg));
                return F(arg, result);
            }
            else
            {
                return F(result);
            }
        }

        // the size and the type of the result, from the signature of the function. 0 and raw if it gives nothing back.
        // the name of a scalar result, or the fields of a struct one, are given by .returns()
        template<typename F>
        constexpr uint8_t resultsize()
        {
            if constexpr (optraits<F>::hasresult)
            {
                using R = std::remove_cv_t<typename optraits<F>::res>;
                static_assert(isscalar<R> || std::is_class_v<R>, "TETHER: the result of an operation is a scalar, an array of scalars or a struct");
                static_assert(std::is_class_v<R> == false || (std::is_standard_layout_v<R> && std::is_trivially_copyable_v<R>),
                              "TETHER: a struct result must be standard layout and trivially copyable");
                return static_cast<uint8_t>(sizeof(R));
            }
            else
            {
                return 0;
            }
        }

        template<typename F>
        constexpr Type resulttype()
        {
            if constexpr (optraits<F>::hasresult)
            {
                using R = std::remove_cv_t<typename optraits<F>::res>;
                if constexpr (std::is_class_v<R>)
                {
                    return Type::structure;
                }
                else
                {
                    return scalartype<typename element<R>::type>();
                }
            }
            else
            {
                return Type::raw;
            }
        }

    } // namespace detail

    // an operation w/out argument:  bool f(tether::Result &)  or  bool f(tether::ResultOf<R> &)
    template<auto F>
    constexpr Operation operation(ID op, const char *name)
    {
        using traits = detail::optraits<decltype(F)>;
        static_assert(false == traits::hasarg,
                      "TETHER: name the argument: operation<f>(id, \"OPname\", \"argname\"), or give the fields of its struct");
        return Operation {op, 0, Type::raw, nullptr, nullptr, 0,
                          detail::resultsize<decltype(F)>(), detail::resulttype<decltype(F)>(), nullptr, nullptr, 0,
                          &detail::adapter<F>, nullptr, name};
    }

    // an operation w/ a scalar argument (bool, uint8_t ... int64_t, float, double, an enum) or an array of them:
    //   bool f(const A &argname, tether::Result &)
    template<auto F>
    constexpr Operation operation(ID op, const char *name, const char *argname)
    {
        using traits = detail::optraits<decltype(F)>;
        static_assert(traits::hasarg, "TETHER: this operation has no argument: use operation<f>(id, \"OPname\")");
        using A = std::remove_cv_t<typename traits::arg>;
        static_assert(false == std::is_class_v<A>,
                      "TETHER: the argument is a struct: give its fields, operation<f>(id, \"OPname\", fields)");
        static_assert(detail::isscalar<A>, "TETHER: the argument of an operation is a scalar, an array of scalars or a struct");
        static_assert(sizeof(A) <= datasize, "TETHER: the argument of an operation must fit a ROP: at most 36 bytes");
        using E = typename detail::element<A>::type;
        return Operation {op, static_cast<uint8_t>(sizeof(A)), detail::scalartype<E>(), argname, nullptr, 0,
                          detail::resultsize<decltype(F)>(), detail::resulttype<decltype(F)>(), nullptr, nullptr, 0,
                          &detail::adapter<F>, nullptr, name};
    }

    // an operation w/ a struct argument:  bool f(const S &arg, tether::Result &), w/ the TETHER_FIELD()s of S
    template<auto F, size_t N>
    constexpr Operation operation(ID op, const char *name, const Field (&fields)[N])
    {
        using traits = detail::optraits<decltype(F)>;
        static_assert(traits::hasarg, "TETHER: this operation has no argument: use operation<f>(id, \"OPname\")");
        using A = typename traits::arg;
        static_assert(std::is_class_v<A>, "TETHER: only a struct argument has fields: give its name instead");
        static_assert(sizeof(A) <= datasize, "TETHER: the argument of an operation must fit a ROP: at most 36 bytes");
        static_assert(std::is_standard_layout_v<A> && std::is_trivially_copyable_v<A> && std::is_default_constructible_v<A>,
                      "TETHER: a struct argument must be standard layout, trivially copyable and default constructible");
        static_assert(N <= 255, "TETHER: too many fields");
        return Operation {op, static_cast<uint8_t>(sizeof(A)), Type::structure, nullptr, fields, static_cast<uint8_t>(N),
                          detail::resultsize<decltype(F)>(), detail::resulttype<decltype(F)>(), nullptr, nullptr, 0,
                          &detail::adapter<F>, nullptr, name};
    }

    // an operation that gives back a result is completed w/ .returns(), which names it:
    //   tether::operation<dummyECHO>(0xC0, "OPdummyECHO", "value").returns("echo")      a scalar (or an array): its name
    //   tether::operation<status>(0xC9, "OPstatus").returns(statusfields)               a struct: its TETHER_FIELD()s


    // ---- the dictionary

    struct Dictionary
    {
        const Variable *variables {nullptr};
        size_t numberofvariables {0};
        const Operation *operations {nullptr};
        size_t numberofoperations {0};
        constexpr Dictionary() = default;

        template<size_t NV, size_t NO>
        constexpr explicit Dictionary(const Variable (&v)[NV], const Operation (&o)[NO])
            : variables(v), numberofvariables(NV), operations(o), numberofoperations(NO) {}

        template<size_t NV>
        constexpr explicit Dictionary(const Variable (&v)[NV])
            : variables(v), numberofvariables(NV), operations(nullptr), numberofoperations(0) {}
    };


    // ---- the checks of TETHER_VALIDATE(). theRPCmanager::initialise() repeats them at runtime

    namespace check {

        using embot::app::eth::rop::ROP;

        constexpr bool count(const Dictionary &d)
        {
            return (d.numberofvariables <= (ROP::VARlast - ROP::VARfirst + 1u)) &&
                   (d.numberofoperations <= (ROP::OPlast - ROP::OPfirst + 1u)) &&
                   ((0 == d.numberofvariables) || (nullptr != d.variables)) &&
                   ((0 == d.numberofoperations) || (nullptr != d.operations));
        }

        constexpr bool variableids(const Dictionary &d)
        {
            for(size_t i = 0; i < d.numberofvariables; i++)
            {
                if(false == ROP::isvariable(d.variables[i].id)) { return false; }
            }
            return true;
        }

        constexpr bool variables(const Dictionary &d)
        {
            for(size_t i = 0; i < d.numberofvariables; i++)
            {
                const Variable &v = d.variables[i];
                if((false == embot::app::eth::SharedVariablesLUT::isvalid(v)) || (v.size > datasize)) { return false; }
            }
            return true;
        }

        constexpr bool operationids(const Dictionary &d)
        {
            for(size_t i = 0; i < d.numberofoperations; i++)
            {
                if(false == ROP::isop(d.operations[i].op)) { return false; }
            }
            return true;
        }

        constexpr bool operations(const Dictionary &d)
        {
            for(size_t i = 0; i < d.numberofoperations; i++)
            {
                if(false == d.operations[i].isvalid()) { return false; }
            }
            return true;
        }

        constexpr bool uniqueids(const Dictionary &d)
        {
            for(size_t i = 0; i < d.numberofvariables; i++)
            {
                for(size_t j = i + 1; j < d.numberofvariables; j++)
                {
                    if(d.variables[i].id == d.variables[j].id) { return false; }
                }
            }
            for(size_t i = 0; i < d.numberofoperations; i++)
            {
                for(size_t j = i + 1; j < d.numberofoperations; j++)
                {
                    if(d.operations[i].op == d.operations[j].op) { return false; }
                }
            }
            return true;
        }

        constexpr const char *nameat(const Dictionary &d, size_t i)
        {
            return (i < d.numberofvariables) ? d.variables[i].name : d.operations[i - d.numberofvariables].name;
        }

        constexpr bool names(const Dictionary &d)
        {
            for(size_t i = 0; i < (d.numberofvariables + d.numberofoperations); i++)
            {
                if(false == embot::app::eth::rop::isvalidname(nameat(d, i))) { return false; }
            }
            return true;
        }

        constexpr bool uniquenames(const Dictionary &d)
        {
            const size_t n {d.numberofvariables + d.numberofoperations};
            for(size_t i = 0; i < n; i++)
            {
                for(size_t j = i + 1; j < n; j++)
                {
                    if(embot::app::eth::rop::samename(nameat(d, i), nameat(d, j))) { return false; }
                }
            }
            return true;
        }

        // group 0: the fields of a struct variable, or the argument of an operation. group 1: the result of an operation
        constexpr size_t numberoffieldsat(const Dictionary &d, size_t i, int group)
        {
            if(i < d.numberofvariables)
            {
                const Variable &v = d.variables[i];
                return ((0 == group) && (Type::structure == v.type)) ? v.numberoffields : 0;
            }
            const Operation &o = d.operations[i - d.numberofvariables];
            return (0 == group) ? o.argfieldscount() : o.resultfieldscount();
        }

        constexpr Field fieldat(const Dictionary &d, size_t i, int group, size_t n)
        {
            if(i < d.numberofvariables)
            {
                return d.variables[i].fields[n];
            }
            Field f {};
            const Operation &o = d.operations[i - d.numberofvariables];
            if(0 == group)
            {
                o.argfield(static_cast<uint8_t>(n), f);
            }
            else
            {
                o.resultfield(static_cast<uint8_t>(n), f);
            }
            return f;
        }

        // the names of the fields of the structs, of the arguments and of the results: valid, and not duplicated in the
        // same group of the same item
        constexpr bool fieldnames(const Dictionary &d)
        {
            for(size_t i = 0; i < (d.numberofvariables + d.numberofoperations); i++)
            {
                for(int group = 0; group < 2; group++)
                {
                    for(size_t n = 0; n < numberoffieldsat(d, i, group); n++)
                    {
                        const Field f {fieldat(d, i, group, n)};
                        if(false == embot::app::eth::rop::isvalidname(f.name)) { return false; }
                        for(size_t m = 0; m < n; m++)
                        {
                            if(embot::app::eth::rop::samename(f.name, fieldat(d, i, group, m).name)) { return false; }
                        }
                    }
                }
            }
            return true;
        }

    } // namespace check

} // namespace embot::app::eth::tether


// it checks a constexpr tether::Dictionary at compile time. every check has its own message
#define TETHER_VALIDATE(d) \
    static_assert(::embot::app::eth::tether::check::count(d), "TETHER: too many variables (max 159) or operations (max 64)"); \
    static_assert(::embot::app::eth::tether::check::variableids(d), "TETHER: a variable has an ID out of [0x01, 0x9F]"); \
    static_assert(::embot::app::eth::tether::check::variables(d), "TETHER: a variable is not valid: no memory or name, or its fields are not scalars, overlap, are out of order or go past its size"); \
    static_assert(::embot::app::eth::tether::check::operationids(d), "TETHER: an operation has an ID out of [0xA0, 0xDF]"); \
    static_assert(::embot::app::eth::tether::check::operations(d), "TETHER: an operation is not valid: no function, or an argument or a result that is not a scalar (or an array) w/ a name or a struct w/ valid fields: did you forget .returns() ?"); \
    static_assert(::embot::app::eth::tether::check::uniqueids(d), "TETHER: an ID is used twice"); \
    static_assert(::embot::app::eth::tether::check::names(d), "TETHER: a name is not [A-Za-z_][A-Za-z0-9_]* w/ at most 28 chars"); \
    static_assert(::embot::app::eth::tether::check::uniquenames(d), "TETHER: a name is used twice"); \
    static_assert(::embot::app::eth::tether::check::fieldnames(d), "TETHER: the name of a field, of an argument or of a result is not [A-Za-z_][A-Za-z0-9_]* w/ at most 28 chars, or two fields of the same struct have the same name")


#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

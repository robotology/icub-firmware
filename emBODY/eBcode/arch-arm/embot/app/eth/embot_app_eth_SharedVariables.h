
/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_SHAREDVARIABLES_H_
#define __EMBOT_APP_ETH_SHAREDVARIABLES_H_

#include "embot_core.h"
#include "embot_os_rtos.h"
#include <array>
#include <cstring>
#include <type_traits>

// SharedVariables: the values of the variables of a board, shared in a thread-safe way by its threads and by TETHER.
// it is built from a tether::Dictionary, which describes the variables (and the operations): the Dictionary is the fixed
// description, constexpr; SharedVariables is what lets the threads of the board and the host use the variables safely.
// - a thread of the board reads and writes them w/ typed handles: SharedVariables::read(), write(), update(), writeFromISR()
// - TETHER (the host) reads and writes them w/ Origin::remote: a variable that is RO for the host can still be written
//   by the board (Origin::local)
// - the description of every variable (ID, name, size, type, fields, access) is constant and is what the host browses
// - the operations of the board are not here: they are in the tether::Dictionary, and they run in the rpc thread

namespace embot::app::eth {

    namespace sharedvariables_detail {
        template<typename T> struct Identity { using type = T; };       // it blocks the deduction of T from a value
    }

    // abstract interface to the variables shared by the rpc thread (RW), the streaming thread (RO)
    // and the user threads (RW). the only key is the ID.
    // - the description of the variables (size, access, type, fields, name) is constant: reading it needs no lock
    // - every access to a value is atomic w.r.t. the others, also for variables that are structs
    // - a variable may be lock-free (see lockfree()): its accesses never block, and it can be written from an ISR
    
    class SharedVariables
    {
    public:

        using ID = uint8_t;

        // RO / RW refer to the remote access (Origin::remote, i.e. rpc). the local owner (Origin::local, e.g. the
        // user thread) can always write.
        enum class Access : uint8_t { RO = 0, RW = 1 };
        enum class Origin : uint8_t { remote = 0, local = 1 };

        // the type of a variable, of a field, or of the argument of an operation.
        // the values are visible on the wire (browse of the ROP protocol): do not change them
        enum class Type : uint8_t
        {
            // bytes w/out a known type:
            raw = 0,                                                                        
            // scalar types:
            boolean = 1, u8 = 2, i8 = 3, u16 = 4, i16 = 5, u32 = 6, i32 = 7, u64 = 8, i64 = 9, f32 = 10, f64 = 11,
            // described by its fields:
            structure = 16                                                                  
        };

        static constexpr bool isscalar(Type t) { return (t >= Type::boolean) && (t <= Type::f64); }

        // the size of a scalar. 1 for raw (a byte), 0 for structure
        static constexpr uint8_t sizeoftype(Type t)
        {
            switch(t)
            {
                case Type::raw:
                case Type::boolean:
                case Type::u8:
                case Type::i8:      return 1;
                case Type::u16:
                case Type::i16:     return 2;
                case Type::u32:
                case Type::i32:
                case Type::f32:     return 4;
                case Type::u64:
                case Type::i64:
                case Type::f64:     return 8;
                default:            return 0;
            }
        }

        // a field of a variable of Type::structure. give the offset w/ offsetof(), so the padding of the
        // struct is described exactly
        struct Field
        {
            const char *name {nullptr};
            Type type {Type::raw};          // a scalar
            uint8_t count {1};              // > 1 for an array
            uint8_t offset {0};             // from the start of the variable

            constexpr Field() = default;
            constexpr explicit Field(const char *n, Type t, uint8_t c, uint8_t o) : name(n), type(t), count(c), offset(o) {}
        };

        // the description of a variable
        struct Info
        {
            ID id {0};
            uint8_t size {0};
            Access access {Access::RO};
            Type type {Type::raw};
            uint8_t numberoffields {0};     // > 0 only for Type::structure
            const char *name {nullptr};

            constexpr Info() = default;
            constexpr explicit Info(ID i, uint8_t s, Access a, Type t, uint8_t nf, const char *n)
                : id(i), size(s), access(a), type(t), numberoffields(nf), name(n) {}
        };

        // one element of a coherent multiple read
        struct Item
        {
            ID id {0};
            void *data {nullptr};           // where to copy the value
            uint8_t capacity {0};           // bytes available at data
            uint8_t size {0};               // filled by get(): bytes copied, 0 if the read failed

            Item() = default;
            explicit Item(ID i, void *d, uint8_t c) : id(i), data(d), capacity(c), size(0) {}
        };

        // a typed handle of a variable: w/ it, read() and write() check the type at compile time.
        // e.g.  constexpr Handle<uint32_t> ticks {0x04};  ...  shared.write(ticks, n, Origin::local);
        template<typename T>
        struct Handle
        {
            ID id {0};

            constexpr explicit Handle(ID i) : id(i) {}
        };

        virtual ~SharedVariables() = default;

        // the description. constant after initialisation
        virtual size_t numberofvariables() const = 0;
        // index in [0, numberofvariables())
        virtual bool info(size_t index, Info &info) const = 0;
        // n in [0, Info::numberoffields)
        virtual bool field(ID id, uint8_t n, Field &field) const = 0;
        // 0 if the ID is unknown
        virtual uint8_t size(ID id) const = 0;
        virtual Access access(ID id) const = 0;

        // the values
        // it copies the value into data. false if the ID is unknown or capacity < size(id)
        virtual bool get(ID id, void *data, uint8_t capacity) const = 0;

        // it reads n variables inside a single critical section, so the values are coherent with each other.
        // it returns true only if all items are read
        virtual bool get(Item *items, size_t n) const = 0;

        // it writes the value. false if the ID is unknown, if size != size(id), or if origin is remote and the
        // variable is RO
        virtual bool set(ID id, const void *data, uint8_t size, Origin origin) = 0;

        // read-modify-write inside a single critical section: nobody else (e.g. a set from rpc) can write in between.
        // fn receives the memory of the variable (size bytes) and changes it in place. it returns false if it did not
        // change it (update() below works on a copy, so a false leaves the variable untouched).
        // false if the ID is unknown, if size != size(id), or if origin is remote and the variable is RO.
        // fn runs w/ the mutex taken: keep it short and do not access the variables from inside it
        using fpModify = bool (*)(void *value, uint8_t size, void *param);
        virtual bool modify(ID id, uint8_t size, fpModify fn, void *param, Origin origin) = 0;

        // true if the accesses to the variable are lock-free (a single atomic load / store, no mutex): they never
        // block. on a lock-free variable modify() may call fn more than once, so fn must only compute the new value
        virtual bool lockfree(ID id) const = 0;

        // as set() w/ Origin::local, but it never blocks: false if the variable is not lock-free. usable from an ISR
        virtual bool setlockfree(ID id, const void *data, uint8_t size) = 0;

        // typed helpers
        template<typename T>
        bool read(ID id, T &value) const
        {
            static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
            return (sizeof(T) == size(id)) && get(id, &value, sizeof(T));
        }

        template<typename T>
        bool write(ID id, const T &value, Origin origin)
        {
            static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
            return set(id, &value, sizeof(T), origin);
        }

        // typed helpers w/ a Handle: the value is converted to T as for any function argument
        template<typename T>
        bool read(Handle<T> handle, T &value) const
        {
            return read(handle.id, value);
        }

        template<typename T>
        bool write(Handle<T> handle, const typename sharedvariables_detail::Identity<T>::type &value, Origin origin)
        {
            return write(handle.id, value, origin);
        }

        // it writes from an ISR: it compiles only for a T that can be lock-free, and it fails (w/out blocking) if the
        // variable is not lock-free in the table (e.g. misaligned, or w/ an onread)
        template<typename T>
        bool writeFromISR(Handle<T> handle, const typename sharedvariables_detail::Identity<T>::type &value)
        {
            static_assert(std::is_trivially_copyable_v<T> && (1 == sizeof(T) || 2 == sizeof(T) || 4 == sizeof(T)),
                          "only a variable of 1, 2 or 4 bytes can be lock-free");
            return setlockfree(handle.id, &value, sizeof(T));
        }

        // read-modify-write w/ a Handle and any callable taking T &, e.g. a lambda (also w/ captures):
        //   shared.update(vDummyUINT32, [](uint32_t &v) { v++; }, Origin::local);
        // the callable may return void (always written back) or bool (written back only if true).
        // on a lock-free variable the callable may run more than once (compare and swap): no side effects in it
        template<typename T, typename F>
        bool update(Handle<T> handle, F &&fn, Origin origin)
        {
            static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
            using C = std::remove_reference_t<F>;
            static_assert(std::is_invocable_v<C &, T &>, "the callable must take a T &");
            const fpModify call = [](void *value, uint8_t, void *param) -> bool
            {
                T v {};
                std::memcpy(&v, value, sizeof(T));
                C &f {*static_cast<C *>(param)};
                if constexpr (std::is_same_v<bool, std::invoke_result_t<C &, T &>>)
                {
                    if(false == f(v))
                    {
                        return false;
                    }
                }
                else
                {
                    f(v);
                }
                std::memcpy(value, &v, sizeof(T));
                return true;
            };
            return modify(handle.id, sizeof(T), call, const_cast<void *>(static_cast<const void *>(&fn)), origin);
        }
    };


    // implementation w/ a look-up table of entries.
    // - lock-free variables: a scalar of 1, 2 or 4 bytes (bool, u8, i8, u16, i16, u32, i32, f32, not an array),
    //   aligned to its size, w/out onread(). they use a single atomic load / store (ldr / str + dmb on Cortex-M4/M7)
    //   and modify() uses compare and swap (ldrex / strex)
    // - all the others (structs, arrays, 64 bits, raw, w/ onread()): one mutex (w/ priority inheritance) for the whole
    //   table. the critical sections are short: a memcpy of size(id) bytes, plus the optional onread()
    // the variables shared w/ another core (e.g. CM7 <-> CM4) are not covered: they need non-cacheable memory and HSEM
    
    class SharedVariablesLUT : public SharedVariables
    {
    public:

        struct Entry
        {
            // called inside the critical section just before a read, e.g. to refresh a computed value in memory
            using fpOnRead = void (*)(ID id, void *memory, uint8_t size);

            ID id {0};
            uint8_t size {0};
            void *memory {nullptr};
            Access access {Access::RO};
            Type type {Type::raw};
            const Field *fields {nullptr};      // only for Type::structure, in order of offset
            uint8_t numberoffields {0};
            fpOnRead onread {nullptr};
            const char *name {nullptr};

            constexpr Entry() = default;
            constexpr explicit Entry(ID i, uint8_t s, void *m, Access a, Type t, const Field *f, uint8_t nf, fpOnRead r, const char *n)
                : id(i), size(s), memory(m), access(a), type(t), fields(f), numberoffields(nf), onread(r), name(n) {}
        };

        // the check of a single entry, also at compile time (see TETHER_VALIDATE): null memory or name, zero size, ID 0,
        // a size that is not a multiple of the size of the type, fields of a structure that are not scalars, overlap
        // each other, are not in order of offset or go past the size
        // the fields of a struct of size bytes: named scalars (or arrays), in order of offset, not overlapping, inside
        // the struct. also for the struct argument of an operation
        
        static constexpr bool isvalid(const Field *fields, uint8_t numberoffields, size_t size)
        {
            if((nullptr == fields) || (0 == numberoffields))
            {
                return false;
            }
            size_t end {0};
            for(uint8_t i = 0; i < numberoffields; i++)
            {
                const Field &f = fields[i];
                if((nullptr == f.name) || (false == isscalar(f.type)) || (0 == f.count) || (f.offset < end))
                {
                    return false;
                }
                end = f.offset + static_cast<size_t>(f.count) * sizeoftype(f.type);
                if(end > size)
                {
                    return false;
                }
            }
            return true;
        }

        static constexpr bool isvalid(const Entry &e)
        {
            if((0 == e.id) || (0 == e.size) || (nullptr == e.memory) || (nullptr == e.name))
            {
                return false;
            }
            if(Type::structure == e.type)
            {
                return isvalid(e.fields, e.numberoffields, e.size);
            }
            if((nullptr != e.fields) || (0 != e.numberoffields))
            {
                return false;
            }
            // raw: any size. a scalar: a whole number of them (an array)
            return (Type::raw == e.type) || (isscalar(e.type) && (0 == (e.size % sizeoftype(e.type))));
        }

        // entries (and their fields) must stay alive for the lifetime of the object
        explicit SharedVariablesLUT(const Entry *entries, size_t number);
        
        ~SharedVariablesLUT() override;

        SharedVariablesLUT(const SharedVariablesLUT &) = delete;
        SharedVariablesLUT &operator=(const SharedVariablesLUT &) = delete;
        SharedVariablesLUT(SharedVariablesLUT &&) = delete;
        SharedVariablesLUT &operator=(SharedVariablesLUT &&) = delete;

        // it validates the entries and creates the mutex: call it from the init thread, after the RTOS has started
        // and before any other use. it fails on: null memory or name, zero size, ID 0, duplicated IDs, a size that is
        // not a multiple of the size of the type, fields of a structure that are not scalars, overlap each other,
        // are not in order of offset or go past the size. invalidentry() tells which entry failed.
        
        bool initialise();
        int16_t invalidentry() const { return _invalid; }

        size_t numberofvariables() const override;
        bool info(size_t index, Info &info) const override;
        bool field(ID id, uint8_t n, Field &field) const override;
        uint8_t size(ID id) const override;
        Access access(ID id) const override;
        bool get(ID id, void *data, uint8_t capacity) const override;
        bool get(Item *items, size_t n) const override;
        bool set(ID id, const void *data, uint8_t size, Origin origin) override;
        bool modify(ID id, uint8_t size, fpModify fn, void *param, Origin origin) override;
        bool lockfree(ID id) const override;
        bool setlockfree(ID id, const void *data, uint8_t size) override;

        // the rule, also at compile time: a scalar of 1, 2 or 4 bytes (not an array), w/out onread(). the alignment
        // of the memory is checked by initialise()
        static constexpr bool canbelockfree(const Entry &e)
        {
            return isscalar(e.type) && (e.size == sizeoftype(e.type)) && ((1 == e.size) || (2 == e.size) || (4 == e.size)) &&
                   (nullptr == e.onread);
        }

    private:
        static constexpr int16_t none {-1};

        const Entry *find(ID id) const;
        bool copyout(const Entry *e, void *data, uint8_t capacity) const;   // to be called w/ the mutex taken

        const Entry *_entries {nullptr};
        size_t _number {0};
        std::array<int16_t, 256> _index {};
        bool _initialised {false};
        int16_t _invalid {none};
        mutable embot::os::rtos::mutex_t *_mtx {nullptr};
        std::array<uint32_t, 8> _lockfree {};       // one bit per ID, set by initialise()

        bool islockfree(ID id) const { return 0 != (_lockfree[id >> 5] & (1u << (id & 31u))); }
    };

} // namespace embot::app::eth


#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

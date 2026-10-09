
/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_app_eth_SharedVariables.h"
#include <cstdint>
#include <cstring>


namespace {


    // the atomic accesses to the memory of a lock-free variable. the memory is a plain object (e.g. a float or an
    // int32_t): the may_alias types tell the compiler that it is accessed through an integer of the same size.
    // on Cortex-M4/M7: ldr + dmb, dmb + str, and ldrex / strex for the compare and swap

    using a8 = uint8_t __attribute__((__may_alias__));
    using a16 = uint16_t __attribute__((__may_alias__));
    using a32 = uint32_t __attribute__((__may_alias__));

    template<typename U>
    void atomicload(const void *memory, void *data)
    {
        const U v {__atomic_load_n(static_cast<const U *>(memory), __ATOMIC_ACQUIRE)};
        std::memcpy(data, &v, sizeof(U));
    }

    template<typename U>
    void atomicstore(void *memory, const void *data)
    {
        U v {0};
        std::memcpy(&v, data, sizeof(U));
        __atomic_store_n(static_cast<U *>(memory), v, __ATOMIC_RELEASE);
    }

    // fn works on a copy: the copy is written back only if nobody wrote the variable in between, else fn runs again
    template<typename U>
    bool atomicmodify(void *memory, embot::app::eth::SharedVariables::fpModify fn, void *param)
    {
        U *m {static_cast<U *>(memory)};
        U expected {__atomic_load_n(m, __ATOMIC_ACQUIRE)};
        for(;;)
        {
            U desired {expected};
            if(false == fn(&desired, sizeof(U), param))
            {
                return false;
            }
            if(__atomic_compare_exchange_n(m, &expected, desired, true, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE))
            {
                return true;
            }
            // expected now holds the value written by the other one: try again on it
        }
    }

    void atomicload(const void *memory, uint8_t size, void *data)
    {
        switch(size)
        {
            case 1: atomicload<a8>(memory, data); break;
            case 2: atomicload<a16>(memory, data); break;
            default: atomicload<a32>(memory, data); break;
        }
    }

    void atomicstore(void *memory, uint8_t size, const void *data)
    {
        switch(size)
        {
            case 1: atomicstore<a8>(memory, data); break;
            case 2: atomicstore<a16>(memory, data); break;
            default: atomicstore<a32>(memory, data); break;
        }
    }

    bool atomicmodify(void *memory, uint8_t size, embot::app::eth::SharedVariables::fpModify fn, void *param)
    {
        switch(size)
        {
            case 1: return atomicmodify<a8>(memory, fn, param);
            case 2: return atomicmodify<a16>(memory, fn, param);
            default: return atomicmodify<a32>(memory, fn, param);
        }
    }

    static_assert(__atomic_always_lock_free(1, 0) && __atomic_always_lock_free(2, 0) && __atomic_always_lock_free(4, 0),
                  "the lock-free path needs atomic accesses of 1, 2 and 4 bytes");

}


namespace embot::app::eth {

    SharedVariablesLUT::SharedVariablesLUT(const Entry *entries, size_t number)
        : _entries(entries), _number(number)
    {
        _index.fill(none);
    }

    SharedVariablesLUT::~SharedVariablesLUT()
    {
        if(nullptr != _mtx)
        {
            embot::os::rtos::mutex_delete(_mtx);
        }
    }

    bool SharedVariablesLUT::initialise()
    {
        if(_initialised)
        {
            return true;
        }

        _invalid = none;
        if((nullptr == _entries) || (_number > 255))
        {
            return false;
        }

        _index.fill(none);
        _lockfree.fill(0);
        for(size_t i = 0; i < _number; i++)
        {
            const Entry &e = _entries[i];
            if((false == isvalid(e)) || (none != _index[e.id]))
            {
                _index.fill(none);
                _lockfree.fill(0);
                _invalid = static_cast<int16_t>(i);
                return false;
            }
            _index[e.id] = static_cast<int16_t>(i);
            // lock-free only if also aligned to its size: a misaligned ldr / str is not atomic
            const bool aligned {0 == (reinterpret_cast<uintptr_t>(e.memory) % e.size)};
            if(canbelockfree(e) && aligned)
            {
                _lockfree[e.id >> 5] |= (1u << (e.id & 31u));
            }
        }

        _mtx = embot::os::rtos::mutex_new();
        _initialised = (nullptr != _mtx);
        return _initialised;
    }

    const SharedVariablesLUT::Entry *SharedVariablesLUT::find(ID id) const
    {
        const int16_t i {_index[id]};
        return (none == i) ? nullptr : &_entries[i];
    }

    // the description is constant after initialise(): no mutex

    size_t SharedVariablesLUT::numberofvariables() const
    {
        return _initialised ? _number : 0;
    }

    bool SharedVariablesLUT::info(size_t index, Info &info) const
    {
        if((false == _initialised) || (index >= _number))
        {
            return false;
        }
        const Entry &e = _entries[index];
        info = Info {e.id, e.size, e.access, e.type, e.numberoffields, e.name};
        return true;
    }

    bool SharedVariablesLUT::field(ID id, uint8_t n, Field &field) const
    {
        const Entry *e {_initialised ? find(id) : nullptr};
        if((nullptr == e) || (n >= e->numberoffields))
        {
            return false;
        }
        field = e->fields[n];
        return true;
    }

    uint8_t SharedVariablesLUT::size(ID id) const
    {
        const Entry *e {find(id)};
        return (nullptr == e) ? 0 : e->size;
    }

    SharedVariables::Access SharedVariablesLUT::access(ID id) const
    {
        const Entry *e {find(id)};
        return (nullptr == e) ? Access::RO : e->access;
    }

    // the values: a lock-free variable w/ an atomic access, all the others w/ the mutex

    bool SharedVariablesLUT::lockfree(ID id) const
    {
        return _initialised && islockfree(id);
    }

    bool SharedVariablesLUT::copyout(const Entry *e, void *data, uint8_t capacity) const
    {
        if((nullptr == e) || (nullptr == data) || (capacity < e->size))
        {
            return false;
        }
        if(islockfree(e->id))
        {
            atomicload(e->memory, e->size, data);
            return true;
        }
        // here the mutex is taken
        if(nullptr != e->onread)
        {
            e->onread(e->id, e->memory, e->size);
        }
        std::memcpy(data, e->memory, e->size);
        return true;
    }

    bool SharedVariablesLUT::get(ID id, void *data, uint8_t capacity) const
    {
        if(false == _initialised)
        {
            return false;
        }
        const Entry *e {find(id)};
        embot::os::rtos::LockIf lock {_mtx, (nullptr != e) && (false == islockfree(id))};
        return copyout(e, data, capacity);
    }

    bool SharedVariablesLUT::get(Item *items, size_t n) const
    {
        if((false == _initialised) || (nullptr == items))
        {
            return false;
        }

        // the mutex only if some variable needs it. the variables that use the mutex are coherent w/ each other, the
        // lock-free ones are each read atomically
        bool needed {false};
        for(size_t i = 0; (i < n) && (false == needed); i++)
        {
            needed = (nullptr != find(items[i].id)) && (false == islockfree(items[i].id));
        }

        bool all {true};
        embot::os::rtos::LockIf lock {_mtx, needed};
        for(size_t i = 0; i < n; i++)
        {
            const Entry *e {find(items[i].id)};
            const bool ok {copyout(e, items[i].data, items[i].capacity)};
            items[i].size = ok ? e->size : 0;
            all = all && ok;
        }
        return all;
    }

    bool SharedVariablesLUT::set(ID id, const void *data, uint8_t size, Origin origin)
    {
        if((false == _initialised) || (nullptr == data))
        {
            return false;
        }

        const Entry *e {find(id)};
        if((nullptr == e) || (size != e->size))
        {
            return false;
        }
        if((Origin::remote == origin) && (Access::RW != e->access))
        {
            return false;
        }

        if(islockfree(id))
        {
            atomicstore(e->memory, size, data);
            return true;
        }

        embot::os::rtos::Lock lock {_mtx};;
        std::memcpy(e->memory, data, size);
        return true;
    }

    bool SharedVariablesLUT::setlockfree(ID id, const void *data, uint8_t size)
    {
        if((false == _initialised) || (nullptr == data) || (false == islockfree(id)))
        {
            return false;
        }
        const Entry *e {find(id)};
        if((nullptr == e) || (size != e->size))
        {
            return false;
        }
        atomicstore(e->memory, size, data);
        return true;
    }

    bool SharedVariablesLUT::modify(ID id, uint8_t size, fpModify fn, void *param, Origin origin)
    {
        if((false == _initialised) || (nullptr == fn))
        {
            return false;
        }

        const Entry *e {find(id)};
        if((nullptr == e) || (size != e->size))
        {
            return false;
        }
        if((Origin::remote == origin) && (Access::RW != e->access))
        {
            return false;
        }

        if(islockfree(id))
        {   // compare and swap: fn may run more than once
            return atomicmodify(e->memory, size, fn, param);
        }

        embot::os::rtos::Lock lock {_mtx};;
        if(nullptr != e->onread)
        {
            e->onread(e->id, e->memory, e->size);
        }
        return fn(e->memory, size, param);
    }

} // namespace embot::app::eth

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

// stub for the host tests of TETHER (test/host): a mutex that counts, single threaded
#pragma once
#include "embot_core.h"
namespace embot::os::rtos {
    struct mutex_t { int taken {0}; };
    inline mutex_t *mutex_new() { return new mutex_t; }
    inline bool mutex_take(mutex_t *m, embot::core::relTime) { if(nullptr == m) { return false; } m->taken++; return true; }
    inline void mutex_release(mutex_t *m) { if(nullptr != m) { m->taken--; } }
    inline void mutex_delete(mutex_t *m) { delete m; }
    class Lock
    {
    public:
        explicit Lock(mutex_t *m, embot::core::relTime tout = embot::core::reltimeWaitForever) : m_(m), taken_(mutex_take(m_, tout)) {}
        ~Lock() { if(taken_) { mutex_release(m_); } }
        Lock(const Lock &) = delete;
        Lock &operator=(const Lock &) = delete;
    private:
        mutex_t *m_ {nullptr};
        bool taken_ {false};
    };
    class LockIf
    {
    public:
        explicit LockIf(mutex_t *m, bool needed, embot::core::relTime tout = embot::core::reltimeWaitForever) : m_(m), taken_(needed && mutex_take(m_, tout)) {}
        ~LockIf() { if(taken_) { mutex_release(m_); } }
        LockIf(const LockIf &) = delete;
        LockIf &operator=(const LockIf &) = delete;
    private:
        mutex_t *m_ {nullptr};
        bool taken_ {false};
    };
}

// stub for the host tests of TETHER (test/host): only what embot/svc/embot_svc_tether{, _ROP, _SharedVariables,
// _theRPCmanager, _theSTRmanager}.h/.cpp and the Dictionary of udpdemo-tether need
#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
#include <type_traits>
namespace embot::core {
    using Time = std::uint64_t;
    using relTime = std::uint32_t;
    constexpr relTime time1microsec {1};
    constexpr relTime time1millisec {1000};
    constexpr relTime time1second {1000000};
    constexpr relTime reltimeWaitForever {0xffffffff};
    template<typename E> constexpr auto tointegral(E e) { return static_cast<std::underlying_type_t<E>>(e); }
    // the tests move the time by hand
    inline Time &testclock() { static Time t {0}; return t; }
    inline Time now() { return testclock(); }
    inline void print(const std::string &) {}
}

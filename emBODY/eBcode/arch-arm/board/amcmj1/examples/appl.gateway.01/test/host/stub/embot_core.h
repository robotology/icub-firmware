// stub for the host tests (test/host): only what amcmj1_gwlink, amcmj1_motdriver and dictionary.amcmj1 need
#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
namespace embot::core {
    using Time = std::uint64_t;
    using relTime = std::uint32_t;
    constexpr relTime time1microsec {1};
    constexpr relTime time1millisec {1000};
    constexpr relTime time1second {1000000};
    template<typename E> constexpr auto tointegral(E e) { return static_cast<std::underlying_type_t<E>>(e); }
    inline Time now() { return 0; }
    inline void print(const std::string &) {}
}

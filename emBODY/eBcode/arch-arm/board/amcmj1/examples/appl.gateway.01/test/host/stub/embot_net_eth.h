// stub for the host tests (test/host): only what amcmj1_gwlink, amcmj1_motdriver and dictionary.amcmj1 need
#pragma once
#include <cstdint>
namespace embot::net::eth {
    struct IPaddress { uint32_t v {0}; constexpr IPaddress() = default; constexpr IPaddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d) : v(a | (b << 8) | (c << 16) | (uint32_t(d) << 24)) {} constexpr explicit IPaddress(uint32_t x) : v(x) {} };
    constexpr IPaddress IPany {0, 0, 0, 0};
    using Port = uint16_t;
    class theIPservice;
}

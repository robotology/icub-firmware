// stub for the host tests of TETHER (test/host)
#pragma once
#include <cstdint>
#include <cstddef>
namespace embot::net::eth {
    struct IPaddress
    {
        uint32_t v {0};
        IPaddress() = default;
        constexpr IPaddress(uint32_t i) : v(i) {}
        constexpr IPaddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d) : v((uint32_t(d) << 24) | (c << 16) | (b << 8) | a) {}
        constexpr uint32_t value() const { return v; }
    };
    constexpr IPaddress IPany {0, 0, 0, 0};
    using Port = uint16_t;
    struct SocketAddress
    {
        IPaddress addr {0, 0, 0, 0};
        Port port {0};
        constexpr SocketAddress() = default;
        constexpr SocketAddress(const IPaddress &a, const Port &p) : addr(a), port(p) {}
    };
}

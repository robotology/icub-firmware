// stub for the host tests of TETHER (test/host)
#pragma once
#include "embot_net_eth.h"
namespace embot::svc {
    class theLogger
    {
    public:
        enum class Activation : uint8_t { OFF = 0, ON = 1 };
        enum class Decoration : uint8_t { OFF = 0, ON = 1 };
        struct Stats { uint32_t lines {0}; uint32_t frames {0}; uint32_t droppedlines {0}; };
        struct Status { embot::net::eth::SocketAddress destination {}; Activation activation {Activation::OFF}; Decoration decoration {Decoration::OFF}; Stats stats {}; };
        static theLogger &getInstance() { static theLogger l {}; return l; }
        Status status() const { return Status {}; }
    };
}

/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __UDPDEMO_TYPES_H_
#define __UDPDEMO_TYPES_H_

#include <cstdint>

#if 0

the types of the variables of the Dictionary of the udpdemo.tether. each one is an example of a kind of variable that
TETHER can carry:

- EchoCONFIG    a struct w/ different scalars (a uint8_t, two bool), written by the host (RW) and read by the echo thread
- EchoSTATS     a struct of counters, written only by the echo thread (RO for the host)
- EchoLAST      a struct w/ an array, a uint16_t and a uint64_t timestamp: it has padding, which TETHER_FIELD() describes
- Mode          an enum class: a scalar for TETHER, the host sees its number
- Clock         a struct computed when it is read (onread), w/ 4 bytes of padding at the end
- LogSTATUS     what embot::net::theLogger is doing, computed on read from theLogger::status()

they are standard layout and trivially copyable, at most 36 bytes (the data of a ROP). each one has a constexpr default
constructor, which TETHER needs to build an empty one, and an explicit constructor w/ all the members.

#endif

namespace udpdemo::types {

    // how the echo thread replies: the host changes it w/ set<IDechoCONFIG, {...}>
    struct EchoCONFIG
    {
        uint8_t marker {'-'};       // the reply has its first byte replaced by marker
        bool enabled {true};        // false: the echo thread counts the frames but does not reply
        bool uppercase {false};     // true: the letters of the reply are changed to uppercase

        constexpr EchoCONFIG() = default;
        constexpr explicit EchoCONFIG(uint8_t m, bool e, bool u) : marker(m), enabled(e), uppercase(u) {}
    };

    // the counters of the echo socket, written by the echo thread
    struct EchoSTATS
    {
        uint32_t rxframes {0};      // frames received on the echo port
        uint32_t txframes {0};      // replies transmitted (ontx of the socket)
        uint32_t rxbytes {0};       // bytes received
        uint32_t errors {0};        // errors signalled by the socket (onerr)

        constexpr EchoSTATS() = default;
        constexpr explicit EchoSTATS(uint32_t rf, uint32_t tf, uint32_t rb, uint32_t er)
            : rxframes(rf), txframes(tf), rxbytes(rb), errors(er) {}
    };

    // who sent the last frame on the echo port, and when
    struct EchoLAST
    {
        uint8_t ip[4] {0, 0, 0, 0}; // the IP address of the sender, w/ ip[0] the first number: 10.0.1.104 is {10, 0, 1, 104}
        uint16_t port {0};          // its UDP port
        uint16_t size {0};          // the size of the frame
        uint64_t time {0};          // when it was received, in microseconds of embot::core::now()

        constexpr EchoLAST() = default;
        constexpr explicit EchoLAST(uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3, uint16_t p, uint16_t s, uint64_t t)
            : ip{i0, i1, i2, i3}, port(p), size(s), time(t) {}
    };

    // an enum: for TETHER it is a scalar of its underlying type
    enum class Mode : uint8_t { idle = 0, run = 1, test = 2 };

    // a value computed when it is read. 4 bytes of padding at the end: TETHER_FIELD() takes the real offsets
    struct Clock
    {
        uint64_t microseconds {0};
        uint32_t seconds {0};

        constexpr Clock() = default;
        constexpr explicit Clock(uint64_t us, uint32_t s) : microseconds(us), seconds(s) {}
    };

    // the state of embot::net::theLogger (theLogger::Status) as it travels on TETHER: its destination, its modes and its
    // counters
    struct LogSTATUS
    {
        uint8_t ip[4] {0, 0, 0, 0}; // the destination of the lines, w/ ip[0] the first number: 10.0.1.104 is {10, 0, 1, 104}
        uint16_t port {0};          // its UDP port
        bool enabled {false};       // Activation::ON
        bool decorated {false};     // Decoration::ON: the lines begin w/ [10.0.1.99 @ time]
        uint32_t lines {0};         // lines taken
        uint32_t frames {0};        // UDP frames sent
        uint32_t dropped {0};       // lines lost (buffer full)

        constexpr LogSTATUS() = default;
        constexpr explicit LogSTATUS(uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3, uint16_t p, bool e, bool d, uint32_t l, uint32_t f, uint32_t dr)
            : ip{i0, i1, i2, i3}, port(p), enabled(e), decorated(d), lines(l), frames(f), dropped(dr) {}
    };

} // namespace udpdemo::types


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


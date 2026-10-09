

/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __AMCMJ1_DUMMY_H_
#define __AMCMJ1_DUMMY_H_

#include <cstdint>

#if 0

the types of the dummy variables of the Dictionary of amcmj1: they are examples of how to add a variable w/ a certain type.
they are used by dictionary.amcmj1.h (the typed handles) and by dictionary.amcmj1.cpp (the variables and the operations)

#endif

namespace amcmj1::dummy {

    struct DummySTRUCT
    {
        int8_t first[4] {0};    
        float second {0.0f};

        constexpr DummySTRUCT() = default;      // needed: TETHER builds an empty one and fills it from the ROP
        constexpr explicit DummySTRUCT(int8_t f0, int8_t f1, int8_t f2, int8_t f3, float s)
            : first{f0, f1, f2, f3}, second(s) {}
    };

    enum class DummyMODE : uint8_t { idle = 0, run = 1, test = 2 };

    // 4 bytes of padding at the end: TETHER_FIELD() takes the real offsets, so the host sees them
    struct DummyCLOCK
    {
        uint64_t microseconds {0};
        uint32_t seconds {0};

        constexpr DummyCLOCK() = default;       // needed: TETHER builds an empty one and fills it from the ROP
        constexpr explicit DummyCLOCK(uint64_t us, uint32_t s) : microseconds(us), seconds(s) {}
    };

} // namespace amcmj1::dummy


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


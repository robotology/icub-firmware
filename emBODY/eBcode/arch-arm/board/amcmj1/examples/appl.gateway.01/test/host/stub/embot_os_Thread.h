// stub for the host tests (test/host): only what amcmj1_gwlink, amcmj1_motdriver and dictionary.amcmj1 need
#pragma once
#include <cstdint>
#include "embot_core.h"
namespace embot::os {
    using Event = uint32_t; using EventMask = uint32_t;
    enum class Priority : uint8_t { normal24 = 24, abovenorm36 = 36, abovenorm37 = 37, high40 = 40, system50 = 50 };
    class Thread { public: struct Props { Priority p; uint32_t s; constexpr Props(Priority a, uint32_t b) : p(a), s(b) {} }; void setEvent(Event) {} };
    class EventThread : public Thread {};
}

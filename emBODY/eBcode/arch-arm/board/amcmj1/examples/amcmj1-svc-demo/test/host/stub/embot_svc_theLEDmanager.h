// stub for the host tests of TETHER (test/host): led one and two exist
#pragma once
#include "embot_hw_led.h"
namespace embot::svc {
    struct LEDh { void on() {} void off() {} };
    class theLEDmanager
    {
    public:
        static theLEDmanager &getInstance() { static theLEDmanager l {}; return l; }
        bool initialised(embot::hw::LED l) const { return embot::hw::LED::three != l; }
        LEDh &get(embot::hw::LED) { return led; }
    private:
        LEDh led {};
    };
}

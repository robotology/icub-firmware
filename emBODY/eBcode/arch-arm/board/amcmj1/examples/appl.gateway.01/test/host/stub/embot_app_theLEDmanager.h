// stub for the host tests (test/host): only what amcmj1_gwlink, amcmj1_motdriver and dictionary.amcmj1 need
#pragma once
#include "embot_hw_led.h"
namespace embot::app { struct LEDh { void on() {} void off() {} }; class theLEDmanager { public: static theLEDmanager &getInstance(); bool initialised(embot::hw::LED) const; LEDh &get(embot::hw::LED); }; }

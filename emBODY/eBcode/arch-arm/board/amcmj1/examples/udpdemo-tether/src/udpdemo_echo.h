/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __UDPDEMO_ECHO_H_
#define __UDPDEMO_ECHO_H_

#include "embot_core.h"
#include "embot_core_binary.h"
#include "embot_os_Thread.h"
#include "embot_os_rtos.h"

#if 0

the interface of the echo thread (tECHO) of the udpdemo.tether w/ the other threads.

- the echo thread (udpdemo-tether.cpp) owns the socket of the echo port: its callbacks only set an event of the thread
- the operations of the Dictionary (dictionary.udpdemo.cpp) run in the rpc thread of TETHER: when they need the echo thread
  (e.g. OPechoHELLO, which sends a frame from the echo socket) they call echo::signal() w/ one of its events and return.
  the ack of such an operation means: request accepted
- the echo thread owns two variables of the application that are not in the Dictionary: wave (a float) and pwm (a
  PWM). readwave() and readpwm() give their value to any thread. w/ tetherdebugENABLE_wave and tetherdebugENABLE_pwm the
  Dictionary has a view of them (IDviewof_wave, IDviewof_pwm)

#endif

namespace udpdemo::echo {

    using embot::core::binary::mask::pos2mask;

    constexpr uint16_t port {666};      // the echo port of the board

    constexpr embot::os::Event evtRXframe {pos2mask<embot::os::Event>(0)};     // the socket has received a frame
    constexpr embot::os::Event evtTXframe {pos2mask<embot::os::Event>(1)};     // the socket has transmitted a frame
    constexpr embot::os::Event evtERframe {pos2mask<embot::os::Event>(2)};     // the socket has signalled an error
    constexpr embot::os::Event evtHELLO {pos2mask<embot::os::Event>(3)};       // OPechoHELLO: send a hello to the last sender

    // set by the startup of the echo thread. nullptr until then
    inline embot::os::EventThread *thread {nullptr};

    // the value now of wave, a variable of the application owned by the echo thread (a sine of period 1 s and amplitude
    // 10.0, computed every 5 ms). it can be called from any thread (defined in udpdemo-tether.cpp)
    float readwave();

    // the type of pwm, a variable of the application owned by the echo thread: three values, e.g. the duty cycles of the
    // three phases of a motor. here three sines of period 1 s and amplitude 10.0, 120 degrees apart
    struct PWM
    {
        float u {0.0f};
        float v {0.0f};
        float w {0.0f};

        constexpr PWM() = default;
        constexpr explicit PWM(float uu, float vv, float ww) : u(uu), v(vv), w(ww) {}
    };

    // a coherent copy of pwm (u, v, w of the same moment), taken under the mutex of its owner. it can be called from any
    // thread, not from an ISR (defined in udpdemo-tether.cpp)
    PWM readpwm();

    // it wakes up the echo thread w/ an event. false if the thread is not running yet
    inline bool signal(embot::os::Event evt)
    {
        if(nullptr == thread)
        {
            return false;
        }
        thread->setEvent(evt);
        return true;
    }

} // namespace udpdemo::echo


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


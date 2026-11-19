# How to use `theLEDmanager`

`embot::svc::theLEDmanager` drives the LEDs of the board, each one on its own. The application asks (on, blink, a
waveform) and goes on: the timing runs on a timer of the RTOS, which calls back in the thread of
`embot::os::theCallbackManager`. So it is used after `embot::os` has started (e.g. in the init of the application).

The code: `embot/svc/embot_svc_theLEDmanager.h/.cpp`. In a project: the include path `embot/svc`, and
`embot_svc_theLEDmanager.cpp` (the old projects compile `embot/app/embot_app_theLEDmanager.cpp`, which is the same
code w/ the old names `embot::app::theLEDmanager`, ...).

## Init

Once, w/ the LEDs to use (the ones of the BSP of the board: `embot::hw::LED::one`, `two`, ...):

```cpp
#include "embot_svc_theLEDmanager.h"

embot::svc::theLEDmanager &leds {embot::svc::theLEDmanager::getInstance()};
leds.init({embot::hw::LED::one, embot::hw::LED::two});
```

Then `leds.get(led)` gives the `LEDhandle` of a LED. For a LED that was not initialised (or that the board does not
have) it gives a handle that does nothing: `get(led).supported()` tells which.

## On, off, toggle

```cpp
leds.get(embot::hw::LED::one).on();
leds.get(embot::hw::LED::one).off();
leds.get(embot::hw::LED::one).toggle();
```

## Blink at 1 Hz

`pulse(period, times)`: the LED toggles every `period / 2`, so `period` is the period of the blink. `times` is the
number of toggles: 0 is forever.

```cpp
leds.get(embot::hw::LED::one).pulse(embot::core::time1second);          // 1 Hz, forever: the "I am alive"
leds.get(embot::hw::LED::two).pulse(200*embot::core::time1millisec, 6); // 5 Hz, three flashes, then off
leds.get(embot::hw::LED::one).stop();                                   // stops and switches it off
```

## A waveform

`wave(spec, times)`: a sequence of steps of the same duration, each one on or off. `LEDwaveSpec` describes it with
constants only: the duration of a step, the number of steps (up to `LEDwaveSpec::maxlength` = 64) and the bits, where
bit `i` is the step `i` (1 = on). `times` is the number of repetitions of the whole waveform: 0 is forever. A
`LEDwaveSpec` has no virtual functions and a trivial destructor, so it can be `constexpr`: no code runs at startup.

A heartbeat: steps of 100 ms, 10 steps (1 s), on in steps 0 and 2: two short flashes, then 700 ms off.

```cpp
constexpr embot::svc::LEDwaveSpec heartbeat {100*embot::core::time1millisec, 10, 0b0000000101};
constexpr embot::svc::LEDwaveSpec sos {150*embot::core::time1millisec, 34, 0b0000000101010001110111011100010101};

leds.get(embot::hw::LED::two).wave(heartbeat);                    // forever
leds.get(embot::hw::LED::one).wave(sos, 3);                       // S O S, in Morse Code for three times
```

The bits written as a binary number are read from the right: the rightmost digit is step 0. In `sos` a dot is 1 step
on, a dash 3 steps on, 1 step off between the signs of a letter, 3 between letters and 7 at the end before it repeats:
`10101` (S), `000`, `11101110111` (O), `000`, `10101` (S), `0000000`.

`wave()` copies the waveform inside the manager, so the spec can also be a local object. The older form
`wave(const LEDwave *lw, times)` is still there: it takes a `LEDwaveT<C>` (a `std::bitset<C>` of `C` steps) or any
object derived from `LEDwave`, e.g. one that computes its steps. Those are polymorphic types, so they cannot be
`constexpr`.

`stop()` ends a waveform too, and switches the LED off. A new `pulse()` or `wave()` on a LED replaces the one it was
doing. `on()`, `off()` and `toggle()` do not stop them (the timer would change the LED again soon after): `stop()` first.

## Notes

- One `pulse()` or `wave()` per LED at a time; different LEDs run on their own.
- The step of a waveform and the half period of a pulse are timers of the RTOS: a few ms are fine, a few us are not.
- The old names (`embot::app::theLEDmanager`, `embot::app::LEDwaveT`, `embot::app::LEDwaveSpec`, ...) are the same types: old and new code can mix
  them.

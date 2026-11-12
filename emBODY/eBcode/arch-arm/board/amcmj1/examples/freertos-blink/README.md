# freertos-blink

A small `embot::os` application on the CM4 of the amcmj1 (boot master) to compare **FreeRTOS** and **RTX**. The code is
plain `embot::os` and the same for both: the project has two targets that differ only in the RTOS.

| target | define | lib |
|--------|--------|-----|
| `amcmj1.2cm4-freertos` | `-DEMBOT_USE_rtos_freertos` | `libs/highlevel/abslayer/freertos/lib/freertos.debug.cm4.lib` |
| `amcmj1.2cm4-cmsisrtx` | `-DEMBOT_USE_rtos_cmsisrtx` | `libs/highlevel/abslayer/cmsisrtx/lib/cmsisrtx.debug.cm4.lib` |

Both libs are the "debug" flavour (stack check and asserts on, bridge to the Event Viewer on), so the comparison is fair.
Both targets list the two libs; each one excludes the lib of the other RTOS (Options for File, "Include in Target Build").
They have their own `obj\<rtos>\` and `lst\<rtos>\` folders.

```
freertos-blink/
├── src/freertos-blink.cpp       init thread, tBLINK (periodic), tEVENT (event based), tPING / tPONG (benchmark), main()
├── cfg/                         BSP config (only the minimal BSP: dualcore, gpio, led, flash), C startup, scatter file (AC6)
└── proj/
    └── amcmj1.freertos.blink.uvprojx / .uvoptx      uVision: two targets, ULINKpro w/ trace (as udpdemo-tether)
```

## What it does

| Who | What |
|-----|------|
| `theLEDmanager` | LED one pulses at 1 Hz (an `embot::os::Timer` inside `theTimerManager`) |
| `tBLINK` | `embot::os::PeriodicThread` at 500 ms (priority `normal24`): it toggles LED two, and every 4 periods it sends `evtFROMBLINK` to tEVENT |
| `tEVENT` | `embot::os::EventThread` (priority `high40`): `evtTICK` every second from an `embot::os::Timer`, and `evtFROMBLINK`; it prints the time and the counters, and at every `evtTICK` it starts a burst of the benchmark |
| `tPING`, `tPONG` | the benchmark: two `embot::os::EventThread` (priorities `high41` and `high42`) that exchange events for 1000 round trips per burst |

The prints go to `embot::core::print()`: in uVision, Debug (printf) Viewer.

## The benchmark
tEVENT sends `evtGO` to tPING; tPING sends `evtPING` to tPONG, which answers with `evtPONG`, 1000 times. The DWT cycle
counter of the CM4 measures:
- **ping -> pong (preemption)**: from `setEvent()` in tPING to the start of `onevent()` in tPONG. tPONG has the higher
  priority, so it is the event plus one context switch;
- **pong -> ping (on wait)**: from `setEvent()` in tPONG to the start of `onevent()` in tPING. tPING has the lower priority,
  so it runs when tPONG waits again: the event, the end of `onevent()`, the wait and the context switch;
- **round trip**: one ping and its pong.

Both directions include what `embot::os::EventThread` adds (the return from the wait and the call of `onevent()`). At the
end of each burst tPING prints, e.g.:
```
bench[FreeRTOS] #7: 1000 round trips in 3120 us
bench[FreeRTOS]   ping -> pong (preemption): min / avg / max cycles = min / avg / max ns
bench[FreeRTOS]   pong -> ping (on wait):    ...
bench[FreeRTOS]   round trip:                ...
```
The `max` also catches the interrupts (SysTick, the timers of embot) that fall inside a burst. The first burst can be
slower (cold start): look at the following ones. The Event Viewer costs one ITM
write per context switch in both libs; to measure the RTOSes without it, use `freertos.fast.cm4.lib` and
`cmsisrtx.fast.cm4.lib` (and remove `eventviewer.c` if neither lib uses it).

## Build

The libraries are not in git: build them before the project, with their uVision projects.

| Library | project | lib used here |
|---------|---------|---------------|
| stm32hal | `libs/lowlevel/stm32hal/proj/...` | `stm32hal.h7.amcmj1.2cm4.v1B6.lib` |
| freertos | `libs/highlevel/abslayer/freertos/proj/freertos.uvprojx`, target `cm4` | `freertos.debug.cm4.lib` |
| cmsisrtx | `libs/highlevel/abslayer/cmsisrtx/proj/cmsisrtx.uvprojx`, target `debug.cm4` | `cmsisrtx.debug.cm4.lib` |

Then build the targets of `proj/amcmj1.freertos.blink.uvprojx` (Project / Batch Build builds both). The project is the one
of `udpdemo-tether` without lwIP, ETH, TETHER, `embot::net` and `embot::app::scope`. It keeps `eventviewer.c` (group rtos),
needed by the Event Viewer bridge of both libs.

## What to check on the board
- LED one at 1 Hz, LED two toggling every 500 ms;
- in the Debug (printf) Viewer, `evtTICK #n` once per second, `evtFROMBLINK after 4k blinks` every 2 seconds, and the four
  lines of the benchmark once per second;
- in Debug / OS Support / Event Viewer (ULINKpro, trace on): `sysIDLE` most of the time, `tBLINK` every 500 ms, `tEVENT`
  every second followed by the burst of `tPING` / `tPONG` (zoom in to see the single switches and compare their timing
  between the two targets), `sysTIMER` when the timers expire, `sysINIT` at the start, plus the threads of
  `theTimerManager` and `theCallbackManager`. The threads are in namespaces: uVision may show them with the namespace,
  e.g. `blink::bench::tPING`. With FreeRTOS, if a name is missing, look at `freertos_ev_diag` in the Watch window
  (`notready` > 0: the ITM is not enabled; `unnamed` > 0: a task without a name);
- if FreeRTOS stops in `freertos_assert_failed()` or `freertos_error_notify()`, the reason is printed first. The usual
  suspects are in the README of the freertos lib (ISR priority 0 calling the RTOS, vector table).

## Differences w/ RTX to keep in mind
See "What is different from RTX" in `libs/highlevel/abslayer/freertos/README.md`. For this example none applies: no ISR
of the application calls the RTOS and no RTOS object is created before `embot::os::start()`.

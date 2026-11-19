# embot::instr

The instrumentation of the firmware: objects that observe how the code behaves while it runs (durations, periods,
signals for an oscilloscope or the Event Viewer). They are not services: no singleton, no thread.

| file | namespace `embot::instr` | needs |
|---|---|---|
| `embot_instr.h/.cpp` | `Histogram` and `HistogramSnapshot<N>`, `PeriodValidator` and `PeriodValidatorSnapshot<N>`, `Scope` (the interface), `ScopeDummy`, `ScopePrint`, `ScopeHisto`, `Measure` (the interface), `MeasureDummy`, `MeasureHisto` | `embot_core` |
| `embot_instr_ScopeGPIO.h/.cpp` | `ScopeGPIO` | `embot::hw::gpio` |
| `embot_instr_ScopeEViewer.h/.cpp` | `ScopeEViewer` | the library `eventviewer` (`libs/midware/eventviewer`) |

`embot_instr.h` is portable: it builds also on a PC (see `test/host/test_instr.cpp`). The two Scopes that need the
board are in their own files, so a project adds only what it can build.

In a project: add `..\embot\instr` (w/ the right number of `..\`) to the include path and `embot_instr.cpp`, plus
`embot_instr_ScopeGPIO.cpp` for a `ScopeGPIO` and `embot_instr_ScopeEViewer.cpp` for a `ScopeEViewer`.

```cpp
#include "embot_instr.h"

constexpr embot::instr::MeasureHisto::Config cfgFOC {"foc", 0, 64*embot::core::time1microsec, embot::core::time1microsec,
                                                     embot::instr::MeasureHisto::Report::snapshot, 100*1000};
embot::instr::Measure *measureFOC {nullptr};

// in the initialisation
measureFOC = new embot::instr::MeasureHisto(cfgFOC);    // or a MeasureDummy, to leave the calls at no cost

// around the code to measure
measureFOC->start();
// ... the code to measure
measureFOC->stop();
```

## Snapshots: what goes on the wire

`Histogram` and `PeriodValidator` hold a `std::vector`: they cannot be sent as they are (e.g. as a variable of TETHER,
which copies bytes). `HistogramSnapshot<N>` and `PeriodValidatorSnapshot<N>` are copies of them w/ a fixed layout
(trivially copyable, standard layout, saturated counters), filled by `embot::instr::snapshot()`: a variable of TETHER
of that type, w/ a `TETHER_FIELD` for each member, is read by the host as any struct. The owner of the histogram takes
the snapshot under its mutex, when the host reads (see `amcmj1-svc-demo`: the latency of the echo, `IDviewof_latency`, and its `docs/howto-add-a-view.md`).

## Where they come from

They replace, for new code, objects that stay as they are for the projects that use them:

| old | new |
|---|---|
| `embot::tools::Histogram`, `embot::tools::PeriodValidator` (icub-firmware-shared `embot/tools/embot_tools.h`) | `embot::instr::Histogram`, `embot::instr::PeriodValidator` |
| `embot::app::scope::Signal`, `SignalDummy`, `SignalPrint`, `SignalHisto` (`embot/app/embot_app_scope.h`) | `embot::instr::Scope`, `ScopeDummy`, `ScopePrint`, `ScopeHisto` |
| `embot::app::scope::SignalGPIO`, `SignalEViewer` | `embot::instr::ScopeGPIO`, `ScopeEViewer` |
| `Measure`, `MeasureDummy`, `MeasureHisto` copied in the theMBD of amc2c, amcbldc, amcfoc | `embot::instr::Measure`, `MeasureDummy`, `MeasureHisto` |

The old and the new are different types, so they can live in the same project. When no project uses the old ones any
more, `embot/app/embot_app_scope.*` and `embot/tools` of icub-firmware-shared can go.

What changes in the new ones:

- `Histogram::getconfig()` / `getvalues()` and `ScopeHisto::histogram()` return a reference, not a pointer. Histogram,
  PeriodValidator, ScopeHisto and MeasureHisto cannot be copied (the old Histogram could, and its copy deleted the same
  memory twice).
- `ScopeHisto` owns its Histogram (`SignalHisto` allocated it and never freed it).
- `ScopePrint::Config::tag` is a `const char *` (a static string), so the Config can be `constexpr`.
- `ScopeGPIO` uses `Config::gpiocfg` (`SignalGPIO` ignored it), and does nothing if the pin cannot be initialised.
- `ScopeEViewer::Config::LABEL` has `uint8_t` as underlying type; w/ a Config not valid `on()` / `off()` do nothing.
- `MeasureHisto::Config` has `Report::none`, `snapshot` or `print` in place of `enablereport` and the
  `#define PRINT_REPORT`, and `maxduration` is the end of the range (the old one passed it as the width of the range:
  the same when `minduration` is 0).
- No `#define EMBOT_APP_SCOPE_core`: a project that has no `embot::hw` simply does not add `embot_instr_ScopeGPIO.cpp`.
- The constructors w/ arguments are `explicit`, the Configs are `constexpr`.

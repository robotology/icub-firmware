# How to see a variable of the application w/ TETHER: a view

The typical case: the application already has a variable, somewhere in its code, that it uses for its own purposes (a
counter, a measure, a state). You want to see it from the host w/ `tether.py`: read it, stream it, plot it. You do not
move it into the Dictionary and you do not change how the application uses it. You add to the Dictionary a **view** of it:
a variable w/ a new ID, named `IDviewof_<name>`, which holds a copy of it. TETHER takes the copy every time the host
reads the view. The Dictionary does not own the variable, and the application does not know that the view exists.

If instead the value must be shared by several threads and the host (e.g. a configuration that the host changes),
make it a native variable of the Dictionary: see [howto-add-a-native-variable.md](howto-add-a-native-variable.md).

This guide does it step by step on `udpdemo-tether`. The same steps work for any board that uses TETHER.

The example:

| | |
|---|---|
| the variable of the application | `wave` in `src/udpdemo-tether.cpp`: the echo thread computes it every 5 ms, a sine w/ a period of 1 second and an amplitude of 10.0 |
| its view in the Dictionary | `IDviewof_wave`, ID `0x0D`, `float`, RO (read only: the host can read it and stream it, not set it) |
| the copy | taken by a function called on read (onread), just before TETHER reads `IDviewof_wave` for the host |
| enabled by | the macro `tetherdebugENABLE_wave`: without it, the view does not exist (the application keeps its variable) |

and, at the end of the guide, a second example: the view of a struct protected by a mutex, `IDviewof_pwm` (ID `0x0E`,
macro `tetherdebugENABLE_pwm`), of `pwm`, three floats `u`, `v`, `w`.

The sources of the example already contain the code of this guide, w/ the macro not defined. To see it work, do step 2
(define the macro) and then steps 6-8. To learn by doing, show another variable of the application in the same way:
e.g. a variable of your own, w/ its view `IDviewof_<name>`.

The convention of the names:

| | name | example |
|---|---|---|
| the variable of the application | whatever it is | `wave` |
| the function of its owner that gives its value | `read<name>()` | `udpdemo::echo::readwave()` |
| the view, for the host | `IDviewof_<name>` | `IDviewof_wave` |
| its handle | `vViewof<Name>` | `vViewofWave` |
| its memory in the Dictionary | `viewof_<name>` | `viewof_wave` |
| its onread function | `refreshviewof_<name>()` | `refreshviewof_wave()` |


## 1. Choose the ID

The IDs of the variables go from `0x01` to `0x9F` (the operations use `0xA0` - `0xDF`). Look at the catalog at the top of
`src/dictionary.udpdemo.h` and take the first free one. Here the last variable is `0x0C` (`IDlogSTATUS`), so `IDviewof_wave`
takes `0x0D`.

Two variables w/ the same ID, or w/ the same name, do not compile: `TETHER_VALIDATE()` checks the Dictionary at compile
time.


## 2. Define the macro in the project

The macro must be seen by all the files, so define it in the project and not inside one file.

- uVision: Project > Options for Target > C/C++ (AC6) > Preprocessor Symbols > Define: add `tetherdebugENABLE_wave` to
  the list (the symbols are separated by spaces or commas), then Rebuild (uVision does not rebuild by itself when the
  defines change).
- VS Code: in `proj/amcmj1.udpdemo.tether.csol/cm4/amcmj1-2cm4.cproject.yml`, add it to the list `define:` of the
  project:

  ```yaml
  define:
    - EMBOT_USE_rtos_cmsisrtx
    # ... the other defines ...
    - tetherdebugENABLE_wave        # IDviewof_wave: a view of the variable wave of the echo thread
  ```

To remove the view later, remove the macro: nothing else changes.


## 3. Let the Dictionary read the variable of the application

The variable of the application is not visible from `dictionary.udpdemo.cpp`, and it must not become a global that
anybody can change. Its owner gives a function that returns its value, declared in a header that both files include.

In `src/udpdemo-tether.cpp` the variable and the function are in `namespace udpdemo::echo`. `wave` is a plain `float`,
as in the existing code:

```cpp
    float wave {0.0f};

    float readwave()
    {
        return wave;
    }
```

and the echo thread writes it, in the timeout of its loop (`onevent()` w/ `eventmask` equal to 0):

```cpp
            // the variables of the application: sines of period 1 s and amplitude 10.0
            constexpr float amplitude {10.0f};
            constexpr float twopi {6.2831853f};
            const float phase {static_cast<float>(embot::core::now() % embot::core::time1second) / static_cast<float>(embot::core::time1second)};
            wave = amplitude * std::sin(twopi * phase);
```

The function is declared in `src/udpdemo_echo.h`, the header of the echo thread:

```cpp
    // the value now of wave, a variable of the application owned by the echo thread (a sine of period 1 s and amplitude
    // 10.0, computed every 5 ms). it can be called from any thread (defined in udpdemo-tether.cpp)
    float readwave();
```

No header declares `wave`: the other files can only read it, w/ `readwave()`. It is not in an anonymous namespace, so a
debugger finds it by its name, `udpdemo::echo::wave`.

With a variable of your application, this is the only change on its side: if it does not have one yet, add the function
that returns its value. Three things to check.

**Another thread writes it.** The onread function runs in a thread of TETHER (rpc or stream), while the owner may be
writing the variable. What you can do depends on the variable:

| the variable | what to do |
|---|---|
| a scalar of up to 4 bytes (`float`, `int32_t`, `uint32_t`, `bool`, an enum), aligned to its size | read it as it is, as `readwave()` does. On Cortex-M4 / M7 it is written and read w/ one instruction (`STR` / `LDR`): a reader never sees half of it |
| the same, but you want the guarantee of C++ | keep a copy in a `std::atomic` (see below) |
| a `uint64_t`, a `double`, a struct, an array | they are written w/ more than one instruction: the owner protects them w/ a lock. See the second example, `IDviewof_pwm` |

The first row works because of the hardware, not because of C++: for C++ two threads that access the same variable
w/out synchronisation are a data race. It is what the existing code of the board already does, and it is fine for a view
that you use to look at the variable. If you want the guarantee of the language, the owner keeps a copy of the variable
in a `std::atomic`, w/ one line where it changes the variable, and the view reads the copy:

```cpp
    float wave {0.0f};                              // the variable of the application, as it is
    std::atomic<float> atomic_wave {0.0f};          // its copy for the view (constexpr constructor: no code at startup)

    float readwave()
    {
        return atomic_wave.load(std::memory_order_relaxed);
    }

            // where the owner changes wave
            wave = amplitude * std::sin(twopi * phase);
            atomic_wave.store(wave, std::memory_order_relaxed);
```

`wave` stays a plain `float`: the code that uses it does not change, and the Logic Analyzer of uVision still sees it.

**It is written in an ISR.** The same rules hold: a scalar of 4 bytes can be read as it is. A struct cannot be protected
w/ a mutex in an ISR: see the second example.

**The function must be quick**: no waits and no prints. It runs while TETHER reads the Dictionary, w/ the mutex of the
`SharedVariables` taken.


## 4. The typed handle, in `dictionary.udpdemo.h`

A handle (`tether::Var<T>`) ties the ID to the C++ type of the variable: the table of the Dictionary uses it, and so
does any code of the board that reads the variable w/ `theTETHERservice::sharedvariables()`. The ID is written in one
place only.

Add it at the end of `namespace udpdemo::vars`, after `vLogStatus`:

```cpp
#if defined(tetherdebugENABLE_wave)
    inline constexpr tether::Var<float> vViewofWave {0x0D};                     // RO, a view of wave: on read a copy of udpdemo::echo::readwave()
#endif
```

and add a row to the catalog of the variables, in the comment at the top of the same file:

```
| 0x0D | IDviewof_wave | `float`              | RO     | a view of wave, a variable of the application (udpdemo-tether.cpp): a copy taken on read. only w/ tetherdebugENABLE_wave |
```


## 5. The view, in `dictionary.udpdemo.cpp`

### 5.1 Its memory: where the copy goes

Every variable of the Dictionary has its own C++ object. For a view it holds the copy: TETHER reads it, the application
does not know it exists. Add it at the end of the anonymous namespace inside `namespace udpdemo::vars`, after `logstatus`:

```cpp
#if defined(tetherdebugENABLE_wave)
        float viewof_wave {0.0f};
#endif
```

It is constant initialised (`{0.0f}`): no constructor runs before `main()`. It has the same type as the variable of the
application (for a struct: the same struct, or a struct made for TETHER, standard layout and at most 36 bytes, like
`types::LogSTATUS`).

### 5.2 The function that takes the copy (onread)

TETHER calls the onread function of a variable just before it reads it: for an `ask` of the host, and for every frame
of the stream. The function writes in the memory of the variable the value of now. Add it after `refreshlogstatus()`:

```cpp
#if defined(tetherdebugENABLE_wave)
    // IDviewof_wave: a view of wave, a variable of the application that lives in udpdemo-tether.cpp (the echo thread
    // computes it) and is not in the Dictionary. the view is a copy, taken when the host reads it (ask, or a frame of
    // the stream)
    void refreshviewof_wave(tether::ID, void *memory, uint8_t)
    {
        *static_cast<float *>(memory) = echo::readwave();
    }
#endif
```

- the signature is always `void f(tether::ID id, void *memory, uint8_t size)`. `memory` points to the object of the
  variable (here `viewof_wave`), `size` is its size in bytes (here 4)
- `udpdemo_echo.h` is already included by the file, so `echo::readwave()` is known
- the view costs nothing when nobody reads it: no thread copies the value periodically

### 5.3 Its entry in the table

The table `variables[]` is the list that TETHER shows to the host. Each entry is

```
tether::variable(handle or ID, object, access, "name" [, fields] [, onread])
```

size and type come from the object, so you write only the ID (w/ the handle), the access and the name. Add the entry at
the end of the table, after `IDlogSTATUS`:

```cpp
#if defined(tetherdebugENABLE_wave)
        tether::variable(vViewofWave, viewof_wave, tether::Access::RO, "IDviewof_wave", refreshviewof_wave),
#endif
```

The comma at the end is fine also when the entry is the last one. A `float` is a scalar, so it has no fields; a struct
needs its fields (see `logstatusfields`).

RO is the right access for a view: a `set` from the host would change only the copy, not the variable of the
application, and the next read would overwrite it.


## 6. Build and run

Build, load and run, as usual (uVision, or VS Code w/ the target-set `stlink` or `stlink-atfe`).


## 7. Read it from the host

Start `tether.py rpc`. At startup it asks the board for the CRC32 of its Dictionary: the Dictionary has changed, so
`tether.py` reads the new one from the board (browse) and rewrites `dictionary.toml` (the old one is kept as `.bak`). If
the rpc was already running, type `browse`.

From the folder `host/` of the example:

    python tether.py rpc --ip 10.0.1.99 --cli

then:

    > ask<IDviewof_wave>
    say<IDviewof_wave, 7.3412>
    > ask<IDviewof_wave>
    say<IDviewof_wave, -9.1037>

Each `ask` gives the value of that moment. A `set<IDviewof_wave, 1.0>` gives a nak: the variable is RO.


## 8. Plot it w/ the stream

From the rpc prompt, add `IDviewof_wave` to the stream and start it w/ a period of 10 ms (100 samples per period of the sine):

    > set+ack<MGstreamADD, IDviewof_wave>; set+ack<MGstreamSTART, 10>

and, in another terminal on the same PC (the stream goes to the PC that started it), the page of the plots:

    python tether.py str --ip 10.0.1.99

tick `IDviewof_wave`: a sine between -10 and +10, one period per second. To stop:

    > set+ack<MGstreamSTOP>

Two things about what you see:

- the stream takes the copy at its own period, the echo thread updates the variable every 5 ms: each sample is the last
  value computed, up to 5 ms old. The time of a frame is the time of the board when the frame was made, not when the
  variable was computed. If you need that, the variable must carry its own timestamp (a struct w/ a `uint64_t`)
- w/ the default period of the stream (100 ms) you get only 10 samples per period: a coarse sine. With 1000 ms you get
  one sample per period, always at about the same phase: an almost flat line (aliasing)


## A second example: the view of a struct protected by a mutex

`pwm` is a variable of the application of type `echo::PWM`, three floats: e.g. the duty cycles of the three phases of a
motor. Here the echo thread writes three sines of period 1 s and amplitude 10.0, 120 degrees apart. A reader that copies
it w/out a lock could take `u` new and `v`, `w` old: a triple that never existed. So the owner protects it w/ a mutex.

| | name |
|---|---|
| the variable of the application | `udpdemo::echo::pwm`, type `udpdemo::echo::PWM {float u; float v; float w;}` |
| the function of its owner | `udpdemo::echo::readpwm()`: a copy taken under the mutex |
| the view | `IDviewof_pwm`, ID `0x0E`, RO, 12 bytes, three fields |
| enabled by | `tetherdebugENABLE_pwm` |

### The owner, in `src/udpdemo_echo.h` and `src/udpdemo-tether.cpp`

The type, and the function that the others use (`udpdemo_echo.h`):

```cpp
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
```

The variable, its mutex and the function (`udpdemo-tether.cpp`):

```cpp
    PWM pwm {};
    embot::os::rtos::mutex_t *pwmmutex {nullptr};

    PWM readpwm()
    {
        if(nullptr == pwmmutex)
        {
            return pwm;                                 // the echo thread has not started: nobody writes it yet
        }
        embot::os::rtos::Lock lock {pwmmutex};
        return pwm;
    }
```

The echo thread creates the mutex in its `startup()`, before the first write:

```cpp
        pwmmutex = embot::os::rtos::mutex_new();
```

and writes the variable w/ the mutex taken, in the timeout of its loop:

```cpp
            const PWM p
            {
                amplitude * std::sin(twopi * phase),
                amplitude * std::sin(twopi * phase - twopi / 3.0f),
                amplitude * std::sin(twopi * phase - 2.0f * twopi / 3.0f)
            };
            {
                embot::os::rtos::Lock lock {pwmmutex};
                pwm = p;
            }
```

The values are computed before the lock: the mutex is held only for the copy of 12 bytes.

**One rule, to avoid a deadlock**: the view calls `readpwm()` w/ the mutex of the `SharedVariables` taken, and
`readpwm()` takes `pwmmutex`. So the owner must never do the opposite: no `sharedvariables()` (read, write, update) w/
`pwmmutex` taken.

**If an ISR writes the struct** (the typical case of the PWM of a motor), a mutex cannot be used. Two ways:

- the reader copies the struct w/ the interrupts disabled for the time of the copy (a few cycles): the ISR cannot run in
  the middle. it adds that time to the latency of all the interrupts
- a double buffer: the ISR writes the buffer that nobody reads, then switches an index (a `uint8_t`, written w/ one
  instruction) to it; the reader copies the buffer of the index. the ISR is never delayed

### The view, in `src/dictionary.udpdemo.h` and `src/dictionary.udpdemo.cpp`

The same steps of `IDviewof_wave`, plus the fields, because it is a struct.

The handle (`dictionary.udpdemo.h`, which includes `udpdemo_echo.h` for the type):

```cpp
#if defined(tetherdebugENABLE_pwm)
    inline constexpr tether::Var<echo::PWM> vViewofPwm {0x0E};                  // RO, a view of pwm: on read a copy of udpdemo::echo::readpwm()
#endif
```

The memory, the onread function, the fields and the entry in the table (`dictionary.udpdemo.cpp`):

```cpp
#if defined(tetherdebugENABLE_pwm)
        echo::PWM viewof_pwm {};
#endif
```

```cpp
#if defined(tetherdebugENABLE_pwm)
    void refreshviewof_pwm(tether::ID, void *memory, uint8_t)
    {
        *static_cast<echo::PWM *>(memory) = echo::readpwm();
    }
#endif
```

```cpp
#if defined(tetherdebugENABLE_pwm)
    constexpr tether::Field viewofpwmfields[]
    {
        TETHER_FIELD(echo::PWM, u), TETHER_FIELD(echo::PWM, v), TETHER_FIELD(echo::PWM, w)
    };
#endif
```

```cpp
#if defined(tetherdebugENABLE_pwm)
        tether::variable(vViewofPwm,  viewof_pwm,  tether::Access::RO, "IDviewof_pwm",  viewofpwmfields, refreshviewof_pwm),
#endif
```

The type of the view is the type of the application: it can be used as it is because it is standard layout, trivially
copyable and at most 36 bytes. If the type of your application is not (e.g. it has a pointer, or it is bigger), make a
struct for the view w/ the members that you want to see, and fill it in the onread function.

### From the host

    > ask<IDviewof_pwm>
    say<IDviewof_pwm, {u=8.6603, v=-8.6603, w=0.0}>
    > set+ack<MGstreamADD, IDviewof_pwm>; set+ack<MGstreamSTART, 10>

and in `tether.py str` tick `IDviewof_pwm.u`, `.v`, `.w`: three sines 120 degrees apart. In every sample `u + v + w` is
0 (up to the rounding): the three values come from the same moment.


## The view and the debugger

The variables of the application are always there, also w/out the macros: a debugger can show them by their names,
`udpdemo::echo::wave` and `udpdemo::echo::pwm`.

| tool | how | while the board runs | plot |
|---|---|---|---|
| uVision + ULINKpro | Watch window: add `udpdemo::echo::wave`, `udpdemo::echo::pwm`. View > Periodic Window Update to see them change while running | yes | Logic Analyzer: `udpdemo::echo::wave`, `udpdemo::echo::pwm.u`, `.v`, `.w` |
| VS Code (CMSIS Debugger) | Run and Debug view > Watch: add the same names | the Watch is refreshed when the core halts (a breakpoint, pause). a Live Watch, if your version of the extension has it, reads them while running | no |
| TETHER (`IDviewof_wave`, `IDviewof_pwm`) | `ask<...>`, or the stream and `tether.py str` | yes, w/out a debugger and w/out stopping the core | yes |

The Logic Analyzer of uVision does not read the variables: the trace hardware of the core (the DWT) reports every write
to them, w/ its value and its time, through the SWO. Every write of a scalar of 4 bytes arrives whole, so the Logic
Analyzer shows `wave`, and `pwm.u`, `pwm.v`, `pwm.w` each as its own signal, correctly, w/out any mutex or disabling of
the interrupts. Two limits: it needs plain scalars of up to 4 bytes at a fixed address (a global or static variable, or
a member of one; not a `std::atomic`), and the DWT has 4 comparators, so at most 4 signals at a time (`u`, `v`, `w` take
3). If the variables are written very often (e.g. in an ISR at 20 kHz) the SWO may not keep up and some samples are lost.

The mutex of `pwm` is for the readers that copy the whole struct, like the view: it makes the three values of a copy
come from the same moment. The Logic Analyzer does not copy the struct, so it does not need it.

The view is the way to see the variables from VS Code while the board runs, to plot them, and to see them on a board
that has no probe attached.


## Another way: the owner writes a native variable of the Dictionary

Instead of a copy taken on read, the thread that owns the variable can write it into the Dictionary itself, w/
`theTETHERservice::getInstance().sharedvariables().write(handle, value, tether::Origin::local)`, every time it changes
it. The echo thread does so w/ `IDechoSTATS` and `IDticks`. Then the Dictionary variable has no onread: it is a native variable, see
[howto-add-a-native-variable.md](howto-add-a-native-variable.md).

| | copy on read (onread) | written by the owner |
|---|---|---|
| cost | only when the host reads | at every change, also when nobody reads |
| code on the side of the owner | a function that returns the value | a call to `write()` (or `update()`) where the value changes |
| what the host sees | the value at the time of the read | the last value written |
| good for | a value that changes often (every ms) and is read rarely | a value that changes rarely, or that the owner wants to publish at a precise moment (e.g. all the counters of a frame together) |


## If something does not work

| what you see | why |
|---|---|
| it does not compile: an error from `TETHER_VALIDATE()` | the ID or the name is already used, or the ID is outside 0x01-0x9F |
| it does not compile: `vViewofWave` or `viewof_wave` not declared | the macro is defined in one file only, or one of the `#if` blocks is missing |
| it does not link: `udpdemo::echo::readwave()` undefined | the function is declared in the header but not defined in the file of its owner |
| `ask<IDviewof_wave>`: tether.py does not know `IDviewof_wave` | its `dictionary.toml` is old: type `browse` at the rpc prompt |
| `ask<0x0D>` gives a nak | the firmware on the board was built w/out `tetherdebugENABLE_wave` (uVision: Rebuild after changing the defines) |
| the value is always 0.0 | the owner never writes its variable, or `readwave()` returns another one |
| the plot is flat or jagged | the period of the stream is too long for a 1 Hz sine: use 10 ms |
| the board stops when the host reads `IDviewof_pwm` | a deadlock: the owner accesses the `SharedVariables` w/ `pwmmutex` taken |
| `u + v + w` is not 0 in some samples | the copy is not protected: `readpwm()` or the writer does not take `pwmmutex` |

# How to add a native variable to the Dictionary of TETHER

There are two ways to see a variable w/ TETHER:

| | native variable (this guide) | view (see [howto-add-a-view.md](howto-add-a-view.md)) |
|---|---|---|
| where the value lives | in the Dictionary | in the application, where it already is |
| who reads and writes it | the threads of the board w/ `sharedvariables()`, and the host w/ TETHER | the application, as it likes. TETHER only takes a copy on read |
| protection between threads | built in: every access goes through `SharedVariables` (atomic or under its mutex) | up to the owner (`std::atomic`, its own lock) |
| the host can write it (RW) | yes: the threads see the new value at their next read | no: a view is RO |
| good for | data shared by several threads and the host: a configuration that the host changes, counters written by a thread and reset by an operation, the state of a module that many read | a variable that already exists and that you only want to look at |

A native variable is the right choice when the value is **shared**: written by one thread and read by another, written
by the host, or changed by more than one thread. You do not write any lock: `SharedVariables` does it.

This guide follows two native variables of `udpdemo-tether`, which show the two typical cases:

- `IDechoCONFIG` (RW): a configuration that the host writes and the echo thread reads at every frame
- `IDechoSTATS` (RO): counters that the echo thread updates and that the operation `OPechoRESET` (in the rpc thread of
  TETHER) clears: two threads write the same variable

To add a new native variable, do the same steps w/ your type, your ID and your name.


## 1. Choose the ID and the access

The IDs of the variables go from `0x01` to `0x9F` (the operations use `0xA0` - `0xDF`): take the first free one in the
catalog at the top of `src/dictionary.udpdemo.h`.

The access is the one of the host (rpc):

- RO: the host can read and stream it. The board writes it (w/ `tether::Origin::local`)
- RW: the host can also set it. The board can write it too

`IDechoCONFIG` is `0x02` RW, `IDechoSTATS` is `0x03` RO.


## 2. The type, in `src/udpdemo_types.h`

A scalar (`uint32_t`, `float`, `bool`, an `enum class`) needs no type of its own. A struct needs one, w/ three rules:

- standard layout and trivially copyable (plain members, no virtual functions, no pointers to the heap)
- at most 36 bytes (the data of a ROP)
- a `constexpr` default constructor (TETHER builds an empty one) and an explicit constructor w/ all the members

```cpp
    // how the echo thread replies: the host changes it w/ set<IDechoCONFIG, {...}>
    struct EchoCONFIG
    {
        uint8_t marker {'-'};       // the reply has its first byte replaced by marker
        bool enabled {true};        // false: the echo thread counts the frames but does not reply
        bool uppercase {false};     // true: the letters of the reply are changed to uppercase

        constexpr EchoCONFIG() = default;
        constexpr explicit EchoCONFIG(uint8_t m, bool e, bool u) : marker(m), enabled(e), uppercase(u) {}
    };
```

The default values of the members are the values at startup.


## 3. The typed handle, in `src/dictionary.udpdemo.h`

The handle ties the ID to the type. Every access of the board goes through it, so the board cannot read or write the
variable w/ a wrong type, and the ID is written in one place only. Add it in `namespace udpdemo::vars`:

```cpp
    inline constexpr tether::Var<types::EchoCONFIG> vEchoConfig {0x02};         // RW, read by the echo thread
    inline constexpr tether::Var<types::EchoSTATS> vEchoStats {0x03};           // RO for rpc: written by the echo thread
```

and a row in the catalog (the comment at the top of the file):

```
| 0x02 | IDechoCONFIG  | `types::EchoCONFIG`  | RW     | a struct written by the host and used by the echo thread       |
| 0x03 | IDechoSTATS   | `types::EchoSTATS`   | RO     | a struct written only by a thread of the board (the echo)      |
```


## 4. The variable, in `src/dictionary.udpdemo.cpp`

### 4.1 Its memory

In the anonymous namespace inside `namespace udpdemo::vars`:

```cpp
        types::EchoCONFIG echoconfig {};
        types::EchoSTATS echostats {};
```

It is constant initialised (no constructor runs before `main()`), and it is in the anonymous namespace: no other file
can touch it. Everybody goes through `sharedvariables()` w/ the handle, which is what protects it.

### 4.2 Its fields (only for a struct)

The host needs the name, type and offset of every member: one `TETHER_FIELD()` per member, in order. Size and offset
come from the C++ type, also w/ padding.

```cpp
    constexpr tether::Field echoconfigfields[]
    {
        TETHER_FIELD(types::EchoCONFIG, marker), TETHER_FIELD(types::EchoCONFIG, enabled), TETHER_FIELD(types::EchoCONFIG, uppercase)
    };
```

### 4.3 Its entry in the table

```cpp
        tether::variable(vEchoConfig, echoconfig,  tether::Access::RW, "IDechoCONFIG", echoconfigfields),
        tether::variable(vEchoStats,  echostats,   tether::Access::RO, "IDechoSTATS",  echostatsfields),
```

A native variable has no onread: its value is in its memory. (A variable w/ onread, like a view, is computed when it is
read.)


## 5. Use it from the threads of the board

Every thread uses the same object, the `SharedVariables` of `theTETHERservice`. In `udpdemo-tether.cpp` the echo thread
has a short function for it:

```cpp
    embot::app::eth::SharedVariables &shared() { return theTETHERservice::getInstance().sharedvariables(); }
```

### read: the echo thread reads the configuration that the host has set, at every frame

```cpp
        types::EchoCONFIG cfg {};
        shared().read(vars::vEchoConfig, cfg);
```

`cfg` is a coherent copy: all its members come from the same `set` of the host, also if the host writes while the
thread reads.

### update: the echo thread increments the counters (read, modify, write in one step)

```cpp
        shared().update(vars::vEchoStats, [size](types::EchoSTATS &st)
        {
            st.rxframes++;
            st.rxbytes += static_cast<uint32_t>(size);
        }, tether::Origin::local);
```

The lambda runs w/ the variable locked: no other thread (and no `set` of the host) can change it in the middle. Keep it
short, and do not access other variables from inside it.

### write: an operation clears the counters, from another thread

```cpp
    bool echoRESET(tether::Result &)
    {
        shared().write(vars::vEchoStats, types::EchoSTATS {}, tether::Origin::local);
        return true;
    }
```

`OPechoRESET` runs in the rpc thread of TETHER, the counters are updated by the echo thread: two threads write the same
variable, and no increment is lost or half written, because both go through `SharedVariables`.

### Origin

- `tether::Origin::local`: the board writes. It can write also a variable that is RO for the host
- `tether::Origin::remote`: used by TETHER for the `set` of the host. It is refused on a RO variable

### How it protects

| variable | how |
|---|---|
| a scalar of 1, 2 or 4 bytes (`bool`, `uint8_t` ... `uint32_t`, `int32_t`, `float`, an enum), aligned, w/out onread | lock-free: a single atomic load or store; `update()` w/ compare and swap |
| anything else (structs, arrays, 64 bits, w/ onread) | one mutex for the whole table, w/ priority inheritance. the critical section is a copy of the bytes of the variable |

So a struct is always read and written whole. On a lock-free variable the lambda of `update()` may run more than once
(compare and swap): no side effects in it. A lock-free variable can also be written from an ISR, w/ `writeFromISR()`.

Variables shared between the CM4 and the CM7 are not covered: they need non-cacheable memory and the HSEM.


## 6. Build, run, and use it from the host

Build, load and run. Start `tether.py rpc`: the Dictionary has changed, so it reads the new one from the board (browse)
and rewrites `dictionary.toml`. From the folder `host/` of the example:

    python tether.py rpc --ip 10.0.1.99 --cli

then:

    > ask<IDechoCONFIG>
    say<IDechoCONFIG, {marker=45, enabled=true, uppercase=false}>
    > set+ack<IDechoCONFIG, {marker=42, enabled=true, uppercase=true}>
    ack<IDechoCONFIG, set, {marker=42, enabled=true, uppercase=true}>

and the next echo of `hello` is `*ELLO`: the echo thread has read the new configuration at the next frame.

    > ask<IDechoSTATS>
    say<IDechoSTATS, {rxframes=3, txframes=3, rxbytes=15, errors=0}>
    > run+ack<OPechoRESET>
    > ask<IDechoSTATS>
    say<IDechoSTATS, {rxframes=0, txframes=0, rxbytes=0, errors=0}>
    > set+ack<IDechoSTATS, {rxframes=1, txframes=1, rxbytes=1, errors=1}>
    nak<IDechoSTATS, set, "...">                       RO for the host

A struct must be set w/ all its fields.

The stream works as for every variable: `set+ack<MGstreamADD, IDechoSTATS>; set+ack<MGstreamSTART, 100>` and
`tether.py str`.


## If something does not work

| what you see | why |
|---|---|
| it does not compile: an error from `TETHER_VALIDATE()` | the ID or the name is already used, the ID is outside 0x01-0x9F, or the fields do not match the struct |
| it does not compile: a `static_assert` on the type | the struct is not standard layout or trivially copyable, or it is bigger than 36 bytes |
| `read()` or `write()` returns false | the ID of the handle is not in the table, or `theTETHERservice` is not initialised yet |
| the host sets the variable but the board does not change behaviour | the thread reads it only once at startup: read it where you use it, e.g. at every frame |
| `set` gives a nak | the variable is RO, or the host did not give all the fields of a struct |

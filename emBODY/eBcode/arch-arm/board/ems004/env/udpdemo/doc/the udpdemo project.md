# The `udpdemo` project



This project offers the services of `EOsocketDatagram` in a minimal way. We use one instance of EOsocketDatagram to listen to port 3333, we print on the trace port any UDP frame received from a host and we send back a reply to it. 

The project uses the trace port but also signalling LEDs and the uVision timing diagram.

It uses the `embot::os` environment because we want to be able to run it on modern boards where we use the `stm32hal` + `embot`  paradigm. However, as the `EOsocketDatagram`  originally run on the `embobj` C environment, some dependencies from it are still required. 

[TOC]


## Description of the software components

In here we describe the structure of the code starting from what we need most: `EOsocketDatagram`. 

### The group `eo-ipnet` 

The `EOsocketDatagram` belongs to the group `eo-ipnet`. It is the highest level object of this group and offers funtionalities for transmission and reception of datagram sockets with FIFO buffering. 

Basically one can:

-  create a datagram socket specifying the required buffering requirements, 
- open it for listening on a given port, 
- specify a callback on reception of a UDP frame and on completion of its transmission, 
- connect to a host, 
- get the UDP frame from the rx FIFO queue,
- put frames into the tx FIFO queue. 

The `EOsocketDatagram` is the UDP specialization of the base object `EOsocket`.  Any EOsocket-derived object (anche hence also `EOsocketDatagram` ) uses the interfaces offered by the virtual object `EOVtheIPnet`. This latter requires however a real object and its multithreading implementation is `EOMtheIPnet`.

The object `EOMtheIPnet` serves the requests of a maximum number of `EOsocket` objects which is configured at its startup. It does that by means of two threads, the `tIPNETproc` and `tIPNETtick`,  which manage the calls to the underlying embedded TCP/IP stack, which is contained inside the IPAL libray `ipal.lib`.

### Dependencies from `embobj`

The objects inside the group `eo-ipnet` need some infrastructure. In particular, they need what is inside the groups `eo-core`, `eom-embot` and the abstraction layers towards the HW, the RTOS and the TCP/IP stack contained inside the group `abstraction-layer`.

#### The group `eo-core` 

The group `eo-core` contains code which is cross-platform and does not depend on any abstration layer. It contains:

- type definitions such as the `eOresult_t`, the `eOreltime_t` expressed in usec, the `eOipv4addr_t`, etc. ;
- utility objects such as `EOfifo`, `EOlist`, `EOvector`, etc.;
- platform-neutral object such as the`EOtheErrorManager`, the `EOtheMemoryPool`, the `EOtimer`, the `EOaction` executed at the timer expiry,  etc.;
- simple interfaces towards services which require and RTOS such as the `EOVmutex`,  the `EOVtask`, the `EOVtheTimerManager`, the `EOVtheCallbackManager` and the `EOVtheSystem`. 

#### The group `eom-embot::os`

The group `eom-embot` contains the implementation counterpart of the `EOV` objects inside `eo-core`. They are `EOMmutex`, `EOMtask`, `EOMtheTimerManager`, `EOMtheCallbackManager`. 

The object `EOMtheSystem` was not required in this projects because I attempted to remove as much as possible `embobj` modules and 

- I implemented in another way the functionalities offered by `EOVtheSystem` with its direct initialization by means of the call `eov_sys_hid_Initialise()` in file `main-embot.cpp`;
- I start the system inside `main()` using  `embot::os::start()` .

The other `EOM` objects are instead implemented with a wrap to the corresponding objects in`embot::os`.



### The group `abstraction-layer`.

In the original `ems` projects, the interface versus the HW, the RTOS and the TCP/IP stack was given by three monolitic libraries: `HAL`, `OSAL` and `IPAL`.  In this project I have kept them, but I have tried to use them only if necessary.  

#### The `RTOS` layer

In the original `ems` projects the RTOS service are offered by `osal.lib` used directly inside `EOMtask` etc.  

In the more recent `embot`  projects instead we use the `embot::os::rtos` namespace which links to either `osal.lib` or to the more modern  `cmsisos2.lib` RTOS.  The `embot::os::rtos` calls are then used in the various `embot::os::Thread` ,  `embot::os::Timer`, `embot::os::theTimerManager`, etc.

In here we have kept the `embot` approach: we use the `embot::os` namespace and we have reimplemented just some `EOM` object (see the previous group `eom-embot::os`).

#### The `TCP/IP` layer

In the `ems` projects we use the `ipal.lib` library and that is unchanged also in the project.

The library `ipal.lib`  exposes a set of API which for instance manage `ARP`, `UDP`, `IGMP`, TCP etc. So far we use only the first three.  The library is implemented around `TCPnet`. An IPAL version with `lwIP` exists since 2014 but has not been used. The `ipal.lib` is HW agnostic because the required calls to the ETH peripheral are done through function pointers which are defined at the initialization of the library. In this mode, we hopefully can maintain the `ipal.lib` when we move to another MPU.

#### The `HW` layer

In the `ems` project we use a the monolitic `hal2` library compile for the `ems4rd` target: `hal2.ems4rd.lib`. This library uses the drivers pruduced by ST Microelectornics and adds to it exta functionalities which expose thrugh a unchenged API. 

That has protected vs changes of the API coming from ST but has produced a difficult update of the library vs the continuous releases of new drivers from ST. 

For this reason, for more recent boards we have decided a different approach. We keep what ST gives us, we organise that as it is inside a library called `stm32hal.brd.ver.lib`  and use the API from ST inside C++ source code encapsulated inside the `embot::hw` namespace. In such a way we can easily add extra pieces (eg, a `embot::hw::bno055` driver) with much more flexibility. We can move the drivers to work on different boards just linking the `stm32.brd.ver.lib` for that board. By the way, `brd` stands for the board name and `ver` expresses the version of the ST HAL driver. More documentation on the stm32hal approach is available in the part of this repository which hold its code. 

In this `udpdemo` project I kept the `hal2` because we dont have the relevant `stm32hal` library.

But I have used `hal2` calls in limited places:

- in file `ipal_cfg.c` inside group where we need the `hal_eth*` functions to pass to the ipal;
- in file `stm32hal_hal2.c` where we implement the function `stm32hal_init()` which gives the basic initialization of the MPU (clock tree, etc.);
- in file `embot_hw_hal2.cpp` where we implement some minimal `embot::hw`  functions used by the `embot` environment. Normally we use direct calls to ST HAL but in here we use what we have, hence `hal_*` calls.  As an example, the trace function`embot::hw::sys::puts(puts(const char* str))` uses `hal_trace_puts(str)` , `embot::hw::led::on(LED l)` uses `hal_led_on()` etc.



### The group `abstraction-layer-config`

It contains configuration of the abstraction layer.



### The group `stm32hal`

It does not contain the full stm32hal library but just the implementation of the function  `stm32hal_init()` which is called by `embot::hw::init()` which is in turn called by `embt::os::init()` inside `main()`.



### The group `embot::hw`

It contains what of embot::hw we have kept. in here there is `embot::hw::init()`.



### The group `embot::hw:: [impl ...]`

It contains what of `embot::hw` we needed and we have implemented using `hal2`. For instance the drivers for the LEDs.



### The group `embot::core`

It contains the core part of `embot`.



### The group `embot::os`

It contains the OS part of `embot`. In particular:

- access to the RTOS via `embot::os::rtos`;
- start of the system using `embot::os::init()` and `embot::os:;start()`.
- utility objects: `embot::os::theScheduler`, various types of `embot::os::Thread`, etc.
- 

### The group `embot::app`

It contains utility objects. So far only the `embot::app::theLEDmanager`.



### The group `app`

It contains  a single `main-embot.cpp` file which runs the `main()`.



## Description of the application

The `main()` launches a classical `embot::os` system. 

The `tINIT` thread starts the `embot::os::theTimerManager`, the `embot::os::theCallbackManager` and the `embot::app::theLEDmanager`.  

After the usual business, it launches the service `IPnet` with address `10.0.1.1` and netmask `255.255.255.0`. It does that by following theses steps:

- it launches some `embobj` services which are pre-requisite to `IPnet`: 
  - the `EOVtheSystem` is initialized to offer interfaces to services  provided by `embot::core`;
  - the `EOMtheTimerManager` and the `EOMtheTimerManager` in the `embot` implementation which are shadows of the `embot::os::theTimerManager` and the `embot::os::theCallbackManager`;
- it calls `eom_ipnet_Initialise()` which launches the `EOMtheIPnet`.

The `tINIT` thread starts also a user-defined thread `tUDPserver` which opens a socket listening to port `3333`.

The `tUDPserver` threads does two things:

- regularly attempts / check ARP connection with the remote address `10.0.1.104`. If connected it switches the red LED on (`embot::hw::LED::one`). And if it detects a disconnection it switches it off.
- when it receives a UDP frame on port `3333` it prints some info on the trace port and sends back a reply.



## The tests done on the board



The setup is formed by an `ems` board and a Linux PC with ubuntu 20.04 configured w/ a fixed IP address `10.0.1.104` and netmask `255.255.255.0`.

On the PC we use `netcat` to start a UDP server listening on port `3333` any message from the `ems` board as  in **Listing 1**.



```bash
shell-2 $ netcat -u -l 3333
```
**Listing 1**. This command must be invoked on a shell of the Linux PC w/ static IPaddress `10.0.1.104` and netmask `255.255.255.0` which is attached to the same network as the `ems`. It starts the UDP server which receives any UDP frame sent by the `ems`  to `10.0.1.104:3333`.



Then we start the `udpdemo` on the `ems`. The project traces all its actions on the relevant window.  As first thing it will use ARP to connect to address '10.0.1.104`.  If successful it switches the red LED on. It will periodically attempt ARP connection so that it will be possible to connect even if the RJ45 is plugged after bootstrap. At the same time, if we unplug it, after some seconds the red LED switches off. And then on again if we plug again.




We can now start start a UDP client on the PC as in **Listing 2**.

```bash
shell-1 $ netcat -u 10.0.1.1 -p 11111 3333
```
**Listing 2**. This command must be invoked on a separate shell of the Linux PC. It starts a communication channel towards the ems board w/ address `10.0.1.1` and sends UDP frames to port 3333. The content of the frames are the characters digited before hitting `Enter`.  



We can send a frame to the `ems` just digiting a text on the shell of the UDP client and hitting `Return`.

The `ems` receives the text, it prints it on trace and replies back to the PC by sending a UDP frame to `10.0.1.104:3333` with some information (time of transmission, frame length, ...). 



The UDP server on Linux receives the frame and prints it on the shell.




It is also possible to transmit bursts of UDP frames with the following command. 

```bash
shell-1b $ for i in {1..10}; do echo "test-$i"| netcat -w 0 -u 10.0.1.1 -p 31111 3333; sleep 0.1; done
```
**Listing 3**. This command must be invoked on a separate shell of the Linux PC. It sends ten UDP frames to `10.0.1.1:3333`, one every 100 ms.  
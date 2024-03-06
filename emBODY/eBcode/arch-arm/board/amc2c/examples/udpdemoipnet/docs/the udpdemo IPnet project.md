# The `IPNet` project



The aim of this project is twofold:  

- to demonstrate UDP communication on the `amc`/`amc2c` board and 
- to mix together legacy C code already available for the  `ems` board with the more modern C++ code based on the the `stm32hal` + `embot`  paradigm.

[TOC]





## Description of the application

The `main()` launches a classical `EOMtheSystem` system as used by the `ems` which runs over `embot::os`. 

The system starts the usual services (timer manager and callback manager) and then it starts:

- `EOMtheIPNET`with its two threads which manage standard UDP communication plus a number of `EOsocketDatagram` sockets;
- one `embot::os::Thread` called `tUDPsrvr` dedicated to act as a UDP server which opens a `EOsocketDatagram` listening on port `3333` and attempt connection it to address `10.0.1.104`;
- one periodic `embot::os::Thread` called`tETHmon` dedicated to monitor ETH sanity (links up / down + CRC errors) .



The `tUDPsrvr` thread does two things:

- regularly attempts / check ARP connection with the remote address `10.0.1.104`. When connected it switches the LED on.
- when it receives a UDP frame on port `3333` it prints some info on the trace port and sends back a reply.

The `tETHmon` threads does two things:

- checks the link status of connectors `J6` and `J7` by querying the chip `embot::hw::chip::KSZ8563` over SPI and prints the variations over the TRACE port. It also lights on/off the associated LED;
- checks the CRC errors on the above links and prints information on the TRACE port.



## How to compile and run it on the `amc2c` board

Follow these steps:

- make sure the stm32hal is compiled with macro `STM32HAL_BOARD_AMC2C_V1A0_enableETH` enabled,
- compile IPAL to produce `ipal.lwip.h7.cm4.lib`
- flash on the `amc` the `eLoader` compiled with macros `DEBUG_stayinhere` and `DEBUG_startCM4now` all enabled.
- compiled the project and run it.

Note that on `amc2c` the `KSZ8563` chip is not supported yet.



## The tests done on the board

It is possible to perform test related to IP communication and also to diagnostics provided by the chip KSZ8563. 



### The setup

The setup is formed by an `amc` board and a Linux PC with ubuntu 20.04 configured w/ a fixed IP address `10.0.1.104` and netmask `255.255.255.0`. The `amc` board has IP address `10.0.1.99`.

### IP communication tests

They are ping and UDP socket communication.

#### Ping

The first thing to do is just to ping the `amc` board as in **Listing 1**.

```bash
shell-1 $ ping 10.0.1.99
```

**Listing 1**. If connected w/  a cable the `amc` board shall reply.  



#### Socket

On the PC we use `netcat` to start a UDP client/server as in **Listing 2**.

```bash
shell-1a $ netcat -u 10.0.1.99 -p 11111 3333
```
**Listing 2**. This command must be invoked on a shell of the Linux PC. It starts a communication channel towards the ETH board w/ address `10.0.1.99` and sends UDP frames to port 3333. The content of the frames are the characters digited before hitting `Enter`.  

We can send a frame to the `amc` just digiting a text on the shell of the UDP client and hitting `Return`.

The `amc` receives the text, it prints it on trace and replies back to the PC by sending a UDP frame to `10.0.1.104:3333` with some information (time of transmission, frame length, ...). 

The UDP server on Linux receives the frame and prints it on the shell.


It is also possible to transmit bursts of UDP frames with the following command. 

```bash
shell-1b $ for i in {1..10}; do echo "test-$i"| netcat -w 0 -u 10.0.1.99 -p 31111 3333; sleep 0.1; done
```
**Listing 3**. This command must be invoked on a separate shell of the Linux PC. It sends ten UDP frames to `10.0.1.1:3333`, one every 100 ms.  



### Diagnostics tests

We can simply attach / detach a cable to connectors `J6` / `J7` and observe the link LEDs close to the connectors to see if they are lighted on or off.

By running the debugger we can also observe the prints on the trace port.


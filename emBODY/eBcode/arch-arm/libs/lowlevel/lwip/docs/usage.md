

This library just wraps what in lwip is used for a ETH network interface. You must provide a proper  `lwipopts.h` file that in our case enables:

- UDP up to 6 sockets;
- 12K bytes for packets, that holds up to 9 packets maximum size before you process and release them

Obviously, you must then provide for an implementation of a HW network interface and for the calls of`lwip`.



However, the above is already done so that it can used in an `embot` project. There are functions in `embot::hw::lwip` that allow a simplified use of `lwip` and in `embot::net::lwip` there is a network interface for `lwip` that works w/ latest HAL versions of a STM32H7 MPU. You also need a proper `embot::hw::eth::bsp` that does low level ETH initialization for the board. You can refer to the one for the `amcmj1`.

So, to use all that you must:

- include `lib/lwip.6udp.9pkt.cm7.lib` or its `cm4` variant in a project
- add the following paths (sadly lwip dos not allow to have a single inclusion path with a single `lwip.h` file):
  - `LwIP\src\include`;
  - `LwIP\system`;
  - `cfg` to see `lwipopts.h`.
- add the files `embot_net_lwip.cpp` and `embot_hw_lwip.cpp` and the `embot::hw::eth::bsp` low level ETH initialization for the board you are using.






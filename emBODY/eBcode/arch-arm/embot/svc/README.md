# embot::svc: the services of an embot board

Services that a board starts once and that run on their own, at low priority, beside the application.

## theTETHERservice

It exposes to an external client two UDP channels: rpc (port 6666) and stream (port 7777). The client is bound only
to the protocol (ROP): the description of the variables and of the operations lives only on the board, in its
Dictionary. On the rpc channel the client browses the variables and operations that the board offers, reads them,
writes the ones that can be written and runs the operations. On the stream channel it watches how the variables it
has chosen change over time. A new variable in the firmware needs no change on the host.

- the client: `embot-tether` (a page in the browser, or a text prompt), see `host/tether/README.md`
- the developer: how to give a board its Dictionary and start the service, see `doc/TETHER.md`
- an example: `board/amcmj1/examples/udpdemo-tether`

## theLogger

It sends to an external client what the board writes w/ `embot::core::print()`, over UDP (port 9999). The client can
be a page in the browser (`embot-logger`), or any reader of a UDP socket, such as `nc`. A few text commands sent to
the same port switch the log on and off, add a decoration (time, IP of the board) and change its destination.

- the client: `embot-logger`, see `host/logger/README.md`
- the developer: `doc/theLogger.md` (init, print, commands), and `embot_svc_theLogger.h`

## theReporter

The one place where the board reports what goes wrong. `report()` never blocks and works also from an ISR; the
application chooses where the reports go (by default `print()`, so the ITM and theLogger; then its own sinks, e.g. the
iCub diagnostics). The reports that repeat are merged, the last ones are kept for TETHER, and a fatal leaves a record
that is reported at the next boot, also when it comes from a HardFault.

- the developer: `doc/theReporter.md` (from the minimum to the fault handler), and `embot_svc_theReporter.h`

## theLEDmanager

It drives the LEDs of the board, each one on its own: on, off, toggle, a pulse repeated n times or forever, and a
waveform of your own (a pattern of on / off steps). The timing runs on a timer of the RTOS, so the application only
asks and goes on.

- the developer: `doc/theLEDmanager.md` (init, blink, waveform), and `embot_svc_theLEDmanager.h`

## Where to start

| you are | open |
|---|---|
| using the tools on a PC | `host/README.md` (only the commands: `host/quick.startup.md`) |
| developing firmware w/ TETHER | `doc/TETHER.md` |
| trying an example | `board/amcmj1/examples/udpdemo-tether/README.md` |
| working on the SPARK gateway | `board/amcmj1/examples/appl.gateway.01/host/README.md` |

The paths of `board/...` start from `emBODY/eBcode/arch-arm`.

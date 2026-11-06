# lwip library in VS Code (Arm Keil Studio)

The folder `lwip.csol/` holds a CMSIS solution equivalent to `lwip.uvprojx`, which is not modified.

## Layout

```
proj/
    lwip.uvprojx                        uVision project (unchanged)
    lwip.csol/                     open this folder in VS Code
        lwip.csolution.yml              device, DFP, compiler, target-types (cores), build-type
        vcpkg-configuration.json        tools fetched by Keil Studio: CMSIS-Toolbox, CMake, Ninja, armclang, ATfE
        README.vscode.md                this file
        build-all.cmd                   builds all the contexts from the command line
        lwip/lwip.cproject.yml          the library: sources, include paths, compiler options
```

## Contexts

| uVision target | csolution context | output |
|---|---|---|
| lwip-cm4 | `lwip.8udp-tx8k+cm4` | `../../lib/lwip.csol.8udp.tx8k.cm4.lib` |
| lwip-cm7 | `lwip.8udp-tx8k+cm7` | `../../lib/lwip.csol.8udp.tx8k.cm7.lib` |
| - (ATfE) | `lwip.8udp-tx8k-atfe+cm4` | `../../lib/lwip.csol.atfe.8udp.tx8k.cm4.a` |
| - (ATfE) | `lwip.8udp-tx8k-atfe+cm7` | `../../lib/lwip.csol.atfe.8udp.tx8k.cm7.a` |

The build-types w/out `-atfe` use Arm Compiler 6 (as uVision), the ones w/ `-atfe` the Arm Toolchain for Embedded (ATfE).

## Configuration: 8udp.tx8k

The same `lwipopts.h` of uVision (`lwip.6udp.9pkt.<core>.lib`), w/ three values redefined by the build-type (`define:` of
the csolution; `lwipopts.h` has them under `#ifndef`):

| define | csolution | lwipopts.h | why |
|---|---|---|---|
| `MEMP_NUM_UDP_PCB` | 8 | 6 | 8udp: up to 8 UDP sockets. 32 bytes each |
| `MEM_SIZE` | 8192 | 14336 | tx8k: the heap of lwIP, used to transmit (`PBUF_RAM`). 5 frames of 1500 bytes; the ETH driver sends one at a time (`HAL_ETH_Transmit()` is blocking) |
| `PBUF_POOL_SIZE` | 2 | 16 (default of opt.h) | the pool is not used: the ETH driver receives w/ its own pbufs (`RX_POOL` of `embot_hw_lwip.cpp`). about 21 KB less |

To check them on the board, set `LWIP_STATS 1` (and `MEM_STATS`, `MEMP_STATS`) in `lwipopts.h` and look in the debugger at
`lwip_stats.mem.max` (bytes of heap used at most) and `lwip_stats.memp[MEMP_PBUF_POOL]->max` (pbufs of the pool used at most).

The build makes `out/lwip/<core>/8udp-tx8k/lwip.csol.8udp.tx8k.<core>.lib`; the `executes:` step of the project copies it
to `../../lib`, both with the build button of VS Code and with `cbuild` / `build-all.cmd`.
The names begin with `lwip.csol.` so that they never overwrite the uVision ones. `lwip.uvprojx` has the same
configuration in its targets `lwip-cm4-8udp-tx8k` and `lwip-cm7-8udp-tx8k` (-> `lwip.8udp.tx8k.<core>.lib`), next to
`lwip-cm4` and `lwip-cm7` (-> `lwip.6udp.9pkt.<core>.lib`, the values of `lwipopts.h`).

From the command line, all contexts: `build-all.cmd` (or `build-all.cmd rebuild`, or `build-all.cmd sizes` to also list
the sizes of each object with `armar --sizes`). It needs the tools in the PATH: a new terminal of VS Code with the solution
open, or `vcpkg activate` in this folder.

## Mapping from the .uvprojx

| uVision | csolution | Note |
|---|---|---|
| Device STM32H747AIIx:CM4 / :CM7 | target-types `cm4` / `cm7` | |
| `STM32HAL_BOARD_AMCMJ1_2CM4` / `_1CM7` | in the target-types | |
| `USE_STM32HAL STM32HAL_DRIVER_V1B6 STM32H747xx` | project `define` | |
| Include `..\api;..\cfg;..\LwIP\src\include;..\LwIP\system` | project `add-path` | |
| `-Os balanced`, C99 | `misc: C-CPP: -Os`, `language-C: c99` | |
| Short enums / wchar, one ELF section per function | `-fshort-enums -fshort-wchar -ffunction-sections` | |
| AC5-like warnings | not reproduced | toolbox default warnings; the code is the same |
| Groups lwip-api, lwip-core, lwip-core-ipv4, lwip-netif | same groups and files | the TCP files are built too, as in uVision: if lwipopts.h has LWIP_TCP = 0 they compile to nothing |
| Group ::CMSIS excluded, no auto includes | no `components` | |
| Create library, After Build copy | `output: type: lib`, `executes:` with `cmake -E copy` | |
| Scatter file of the targets | not used | a library is not linked |

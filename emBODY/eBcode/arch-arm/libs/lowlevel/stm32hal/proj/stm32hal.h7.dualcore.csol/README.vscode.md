# stm32hal library (STM32H7 dual core) in VS Code (Arm Keil Studio)

The folder `stm32hal.h7.dualcore.csol/` holds a CMSIS solution equivalent to two targets of `stm32hal.h7.dualcore.uvprojx`, which is
not modified.

## Layout

```
proj/
    stm32hal.h7.dualcore.uvprojx            uVision project (unchanged)
    stm32hal.h7.dualcore.csol/         open this folder in VS Code
        stm32hal-h7.csolution.yml           device, DFP, compiler, target-types (board+core), build-types (driver)
        vcpkg-configuration.json            tools fetched by Keil Studio: CMSIS-Toolbox, CMake, Ninja, armclang, ATfE
        README.vscode.md                    this file
        build-all.cmd                       builds all the contexts from the command line
        stm32hal-h7/stm32hal-h7.cproject.yml  the library: sources, include paths, compiler options
```

## Contexts

| uVision target | csolution context | output |
|---|---|---|
| amcmj1.2cm4-v1B6 | `stm32hal-h7.v1B6+amcmj1-2cm4` | `../../lib/stm32hal.csol.h7.amcmj1.2cm4.v1B6.lib` |
| amcmj1.1cm7-v1B6 | `stm32hal-h7.v1B6+amcmj1-1cm7` | `../../lib/stm32hal.csol.h7.amcmj1.1cm7.v1B6.lib` |
| - (ATfE) | `stm32hal-h7.v1B6-atfe+amcmj1-2cm4` | `../../lib/stm32hal.csol.atfe.h7.amcmj1.2cm4.v1B6.a` |
| - (ATfE) | `stm32hal-h7.v1B6-atfe+amcmj1-1cm7` | `../../lib/stm32hal.csol.atfe.h7.amcmj1.1cm7.v1B6.a` |

The build-types w/out `-atfe` use Arm Compiler 6 (as uVision), the ones w/ `-atfe` the Arm Toolchain for Embedded (ATfE).

The build makes `out/stm32hal-h7/<target-type>/<build-type>/stm32hal.csol.h7.amcmj1-2cm4.v1B6.lib` (a target-type
name cannot contain `.`); the `executes:` step of the project copies it to `../../lib` with the dotted name, both with
the build button of VS Code and with `cbuild` / `build-all.cmd`. The names begin with
`stm32hal.csol.` so that they never overwrite the uVision ones (`stm32hal.h7.amcmj1.2cm4.v1B6.lib`).

From the command line, all contexts: `build-all.cmd` (or `build-all.cmd rebuild`, or `build-all.cmd sizes` to also list
the sizes of each object with `armar --sizes`). It runs `cbuild stm32hal-h7.csolution.yml --packs` and needs the tools in
the PATH: a terminal of VS Code with the solution open, or `vcpkg activate` in this folder.

## Mapping from the .uvprojx

| uVision | csolution | Note |
|---|---|---|
| Device STM32H747AIIx:CM4 / :CM7 | target-types `amcmj1-2cm4` / `amcmj1-1cm7` | |
| `STM32HAL_BOARD_AMCMJ1_2CM4` / `_1CM7` | in the target-types | |
| `STM32HAL_DRIVER_V1B6`, include `..\src\driver\stm32h7-v1B6\inc`, group driver-stm32h7-v1B6 | build-type `v1B6` | |
| `USE_STM32HAL STM32H747xx`, Misc `-DSTM32HAL_USE_LOWLEVEL_DELAY` | project `define` | |
| `-Os balanced`, C99 | `misc: C-CPP: -Os`, `language-C: c99` | |
| Short enums / wchar, one ELF section per function | `-fshort-enums -fshort-wchar -ffunction-sections` | |
| AC5-like warnings | not reproduced | toolbox default warnings; the code is the same |
| `lowlevel.c` at `-O0` | `optimize: none` on the file | |
| CM7 only: `stm32h7xx_ll_usb.c` excluded | `for-context: +amcmj1-2cm4` on that file | |
| HAL templates `stm32h7xx_hal_msp_template.c`, `stm32h7xx_hal_timebase_*_template.c` | not listed | they must not be built (the uVision CM4 target built them) |
| Groups v1A0, v1B5, ::CMSIS excluded | not listed | |
| No auto includes, CMSIS:CORE component excluded | no `components` | |
| Create library, After Build copy | `output: type: lib`, `executes:` with `cmake -E copy` | |

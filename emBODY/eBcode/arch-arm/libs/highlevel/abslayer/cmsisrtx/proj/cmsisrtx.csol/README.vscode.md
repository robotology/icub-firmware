# cmsisrtx library in VS Code (Arm Keil Studio)

The folder `cmsisrtx.csol/` holds a CMSIS solution equivalent to `cmsisrtx.uvprojx`. The `.uvprojx` is not modified.

## Layout

```
proj/
    cmsisrtx.uvprojx                    uVision project (unchanged)
    cmsisrtx.csol/                      open this folder in VS Code
        cmsisrtx.csolution.yml          device, DFP, compiler, target-types (cores), build-types (flavours)
        vcpkg-configuration.json        tools fetched by Keil Studio: CMSIS-Toolbox, CMake, Ninja, armclang, ATfE
        README.vscode.md                this file
        build-all.cmd                   builds all the contexts from the command line
        cmsisrtx/cmsisrtx.cproject.yml  the library: sources, include paths, compiler options
```

## Contexts

| uVision target | csolution context | output |
|---|---|---|
| debug.cm4 | `cmsisrtx.debug+cm4` | `../../lib/cmsisrtx.csol.debug.cm4.lib` |
| fast.cm4 | `cmsisrtx.fast+cm4` | `../../lib/cmsisrtx.csol.fast.cm4.lib` |
| debug.cm7 | `cmsisrtx.debug+cm7` | `../../lib/cmsisrtx.csol.debug.cm7.lib` |
| fast.cm7 | `cmsisrtx.fast+cm7` | `../../lib/cmsisrtx.csol.fast.cm7.lib` |
| - (ATfE) | `cmsisrtx.debug-atfe+<core>` | `../../lib/cmsisrtx.csol.atfe.debug.<core>.a` |
| - (ATfE) | `cmsisrtx.fast-atfe+<core>` | `../../lib/cmsisrtx.csol.atfe.fast.<core>.a` |

The build-types w/out `-atfe` use Arm Compiler 6 (as uVision), the ones w/ `-atfe` the Arm Toolchain for Embedded (ATfE).

From the command line, all of them: `build-all.cmd` (or `build-all.cmd rebuild`, or `build-all.cmd sizes` to also list
the sizes of each object with `armar --sizes`). It runs `cbuild cmsisrtx.csolution.yml --packs` and needs the tools in the
PATH: a terminal of VS Code with the solution open, or `vcpkg activate` in this folder.

The build makes `out/cmsisrtx/<core>/<flavour>/cmsisrtx.csol.<flavour>.<core>.lib`; the `executes:` step of the project
copies it to `../../lib`, both with the build button of VS Code and with `cbuild` / `build-all.cmd`. The names begin with `cmsisrtx.csol.` so that they never overwrite the uVision ones
(`cmsisrtx.debug.cm4.lib`).

## Mapping from the .uvprojx

| uVision | csolution | Note |
|---|---|---|
| Device STM32H747AIIx:CM4 / :CM7 | `target-types` with `device: ...:CM4` / `:CM7` | |
| Defines `CMSISRTX_CM4` / `CMSISRTX_CM7` | in the target-types | |
| Defines `CMSISRTX_EVENTVIEWER` / `CMSISRTX_FAST` | in the build-types `debug` / `fast` | |
| Defines `EVR_RTX_DISABLE _RTE_` | project `define` | uVision does not pass `EVR_RTX_DISABLE` to the assembler; harmless |
| `-Os balanced` | `misc: C-CPP: -Os` | `optimize: balanced` would give `-O2` |
| All Warnings, C99 | `warnings: all`, `language-C: c99` | |
| Short enums / wchar, one ELF section per function | `-fshort-enums -fshort-wchar -ffunction-sections` | |
| `cmsisrtx_irq.c` with `-x assembler-with-cpp` | `-xassembler-with-cpp` in the `misc` of the file | joined: with the space CMake passes one quoted argument |
| Create library | `output: type: lib` | |
| After Build: copy to `..\lib` | `executes:` with `cmake -E copy` | see above |

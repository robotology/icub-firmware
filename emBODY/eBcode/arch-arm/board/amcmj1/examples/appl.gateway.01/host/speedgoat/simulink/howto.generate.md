# How to generate the Simulink files

From MATLAB R2026b, in this folder (`host/speedgoat/simulink`). Python 3.11 or later must be on the PATH (the generator).

## The first time, or to start again from scratch

```matlab
sgact_setup              % path, check of sgact_panel.toml, bus objects
build_sgact_lib          % sgact_spec.m, sgact_modes_dispatch.m, sgact_modes_info.m from the TOML, then sgact_lib.slx
build_sgact(true)        % sgact.slx: the whole model, USER CONTROL included
tg = sgact_deploy;       % build, load and start on the Speedgoat
```

`build_sgact(true)` overwrites `sgact.slx`: everything drawn in the subsystems of USER CONTROL is lost. Run it only
when there is nothing to keep.

## After a change of sgact_panel.toml

| what changed | run |
|---|---|
| parameters, help, plots, the config of the motor_macro | `build_sgact_lib`, then `sgact_deploy` |
| a mode w/ `impl = "matlab"` (and its `matlab/sgact_mode_<name>.m`) | `build_sgact_lib`, then `sgact_deploy` |
| a mode w/ `impl = "simulink"` added, removed or renamed | `build_sgact_lib`, then `sgact_sync_modes`, then `sgact_deploy` |

`sgact_sync_modes` adds the subsystems of the new modes and rewires USER CONTROL. It never touches what is inside the
existing subsystems. To rename a mode w/ `impl = "simulink"`, first rename its subsystem `mode_<name>` by hand.

After a change of the TOML the panel must read the same file: its CRC travels in every request.

## After a change of the protocol or of the network

`sgact_netcfg.m` (addresses, ports), `sgact_const.m` (timing, IDs), the frames: `build_sgact_lib`, then `sgact_deploy`.
`sgact_const.m` holds `TS`, the rate of the whole model (1 ms): after a change of `TS` also `build_sgact(true)`.

## The tests (no hardware, no Simulink: also in Octave)

```matlab
test_golden; test_link; test_sgact_step; test_parity
```

| test | what it checks |
|---|---|
| `test_golden` | MATLAB writes and reads the same bytes as Python (panel, daemon) and as the C++ of amc.gtw (`tests/golden.txt`) |
| `test_link` | the four I/O blocks and the buses |
| `test_sgact_step` | the whole chain w/ real bytes against a fake amc.gtw |
| `test_parity` | the MATLAB supervisor gives the same outputs as the Python one, step by step |

Run them after every change of the protocol, of the supervisor or of a mode in `.m`.

## If the build fails

- Clean and build again:
  ```matlab
  bdclose all
  if isfolder('slprj'), rmdir('slprj', 's'); end
  if isfolder('sgact_sg_rtw'), rmdir('sgact_sg_rtw', 's'); end
  delete *.slxc
  clear all; rehash path
  ```
- "shadowing" warnings: another copy of sgact is on the MATLAB path. `which -all sgact_sup_a` shows it; remove it w/
  `rmpath`, then `savepath`.

The generated and build files (`sgact_lib.slx`, `slprj/`, `sgact_sg_rtw/`, `codegen/`, `*.slxc`, `*.mldatx`) are not in
git (`host/.gitignore`). `sgact.slx` is: it holds the work of the control engineer.

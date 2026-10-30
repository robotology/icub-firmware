# panel: the window of sgact

`sgact_panel.py` drives **sgact**: the Simulink model on the Speedgoat, or the PC that plays it (`../emul/sgact_daemon.py`).
The window is built from `../sgact_panel.toml`: devices, the config of the motor_macro, modes and their parameters with units
and limits, plots. Run it on Windows, from `host`:

    py speedgoat\panel\sgact_panel.py --host 10.0.2.150        # against sgact on the Speedgoat
    py speedgoat\panel\sgact_panel.py                          # against sgact_daemon.py on the same PC (127.0.0.1)
    py speedgoat\panel\sgact_panel.py --spec other.toml        # another TOML (or set SGACT_SPEC)

The window: link and EXTFAULT on top; devices (the help of each device on the mouse; the config of the motor_macro
folded, "motor_macro config ▸" opens it), mode and its parameters side by side, status on the right; the plots below,
in two columns.

What you do:
- **devices**: tick motor_cubemars, sensor_torque, motor_macro, sensor_position, then **INIT** or **DEINIT**. The config of the motor_macro comes from the
  TOML: change it in its fields before INIT if you want (red: not valid), *TOML values* puts back the ones of the file.
- **mode**: choose it, set its parameters and a duration (0 = until stop), then **START** and **STOP**. The parameters of
  the running mode can be changed while it runs (Enter, or leaving the field). A parameter w/ min = max in the TOML is
  shown and cannot be changed (e.g. the MIT of motor_cubemars_idle).
- **EXTFAULT** (the big red button, sticky): pressed, sgact stops all the motors at once and amc.gtw holds the fault
  (motor_cubemars disabled, motor_macro idle, both refused), every request is refused; pressed again, sgact goes idle.
  It goes in every CTL (flags bit 0). The motor_macro row says "amc.gtw holds EXTFAULT" while amc.gtw holds it.

What you see:
- the state of the supervisor (idle, initialising, deinitialising, running, stopping, EXTFAULT), the initialised devices, the result
  of the last request (done, rejected and why, failed) and the errors;
- the feedback: motor_cubemars, sensor_torque, motor_macro (control mode, configured, emulated, external fault, pos vel current pwm),
  sensor_position (pos, readings, valid, emulated);
- the plots of the TOML. Angles in deg, or rad: `[display] angles` of the TOML.

The panel sends a CTL every 200 ms: if it closes or the network breaks, the running mode stops after 1 s (err 3). The
devices stay initialised.

It needs `host/shared` (sgctl.py, sgact_spec.py) and tkinter. Windows firewall: allow Python to receive UDP on 6001.

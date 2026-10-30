"""
sgact_modes - the modes of sgact_panel.toml for the daemon (sgact_daemon.py): the same as
host/speedgoat/simulink/matlab/sgact_mode_<name>.m on the Speedgoat. Keep them aligned.

    valid, sp, done = mode_<name>(phase, p, t, fb)
      phase 1 run, 2 stop; p: the parameters in the order of the TOML, in SI (deg -> rad); t: s since the phase started; fb: dict (SGACT_FB)
      valid: a MIT for the motor_cubemars (sp[0:5]), sp = [kp kd pos vel torque mmtarget]; done: (stop) the mode is over.
      mmtarget: the setpoint of the motor_macro in SI (A, rad/s, %), used by the modes w/ a controlmode

A mode marked speedgoat_only = true in the TOML needs no function here.
"""

import math

STOP_RAMP = 0.5         # [s] as sgact_const.m
STOP_BRAKE = 0.2

def reset():
    """forget the memory of the modes (as clearing the persistent variables in MATLAB): none has memory now"""


def _mit(p, vel):
    """the MIT of a mode of the motor_cubemars, p = [pos vel kp kd torque ...] -> sp w/ the velocity vel"""
    return [p[2], p[3], p[0], vel, p[4], 0.0]


def mode_motor_cubemars_position(phase, p, t, fb):
    """p = [pos vel kp kd torque]: the MIT, constant. stop: done at once (then the supervisor disables the motor)"""
    return True, _mit(p, p[1]), phase == 2


def mode_motor_cubemars_velocity_step(phase, p, t, fb):
    """p = [pos vel kp kd torque half]: vel changes sign every half s. stop: vel 0 for STOP_BRAKE s"""
    if phase == 1:
        sgn = -1.0 if p[5] > 0 and math.floor(t / p[5]) % 2 == 1 else 1.0
        return True, _mit(p, sgn * p[1]), True
    return True, _mit(p, 0.0), t >= STOP_BRAKE


def mode_motor_cubemars_idle(phase, p, t, fb):
    """p = [pos vel kp kd torque], all 0 (the MIT of rest, shown). cubemars_enable = false: the supervisor disables the
    motor at the start (amc.gtw: MIT of rest, then the disable packet). no MIT. stop: done at once"""
    return False, [0.0] * 6, True


def mode_motor_macro_velocity(phase, p, t, fb):
    """p = [vel] rad/s: control mode speedvoltage. stop: ramp to 0 in STOP_RAMP s"""
    if phase == 1:
        return False, [0.0, 0.0, 0.0, 0.0, 0.0, p[0]], True
    k = max(0.0, 1.0 - t / STOP_RAMP)
    return False, [0.0, 0.0, 0.0, 0.0, 0.0, k * p[0]], k <= 0


def mode_motor_macro_current(phase, p, t, fb):
    """p = [current] A: control mode current. stop: 0 at once"""
    return False, [0.0, 0.0, 0.0, 0.0, 0.0, p[0] if phase == 1 else 0.0], True


def mode_motor_macro_voltage(phase, p, t, fb):
    """p = [pwm] %: control mode openloop. stop: 0 at once"""
    return False, [0.0, 0.0, 0.0, 0.0, 0.0, p[0] if phase == 1 else 0.0], True


def mode_idle(phase, p, t, fb):
    """all the motors stop: the supervisor disables the motor_cubemars and idles the motor_macro at the start. nothing sent"""
    return False, [0.0, 0.0, 0.0, 0.0, 0.0, 0.0], True

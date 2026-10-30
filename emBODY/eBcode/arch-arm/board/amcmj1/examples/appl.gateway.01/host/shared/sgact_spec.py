"""
sgact_spec - the reader of sgact_panel.toml: what the panel can ask to sgact (devices, actions, modes and their
parameters) and what it plots. Used by sgact_panel.py, sgact_daemon.py and speedgoat/simulink/tools/gen_sgact_spec.py.

    spec = sgact_spec.load()              # finds the file: see find()
    spec.crc                              # CRC32 of the file (CRLF counted as LF): it travels in every CTL
    spec.devices                          # {'motor_cubemars': 0, 'sensor_torque': 1, 'motor_macro': 2, 'sensor_position': 3}: bit of the mask
    spec.actions                          # {'init': 1, 'deinit': 2, 'start': 3, 'stop': 4}
    spec.modes                            # [Mode], sorted by id. Mode: name id needs needs_mask help params speedgoat_only controlmode impl
                                          #   cube_enable: the mode enables the motor_cubemars at its start (cubemars_enable, default true)
    spec.mmcfg                            # the config of the motor_macro: 14 numbers in the order of MMCFG_NAMES (the CTL carries them)
    spec.mode_by_id[1].params[1].name     # 'vel'
    spec.mode_by_id[1].params[1].scale    # pi/180: the value in the TOML unit (deg/s) times scale is the one in SI (rad/s)
    spec.angles                           # 'deg' or 'rad': how the panel shows the angles of the feedback
    spec.plots                            # [Plot]: title, fields

It lives in host/shared: the panel, the daemon and the generator of the Simulink side import it from here.
"""

import math
import os
import re
import tomllib
import zlib
from collections import namedtuple

SPEC_NAME = "sgact_panel.toml"
DEVICE_NAMES = ("motor_cubemars", "sensor_torque", "motor_macro", "sensor_position")          # the supervisor knows these: a new device needs code too
ACTION_NAMES = ("init", "deinit", "start", "stop")
STATUS_FIELDS = ("motor_cubemars_pos", "motor_cubemars_vel", "motor_cubemars_current", "sensor_torque_mx", "sensor_torque_my", "sensor_torque_mz",
                 "motor_macro_pos", "motor_macro_vel", "motor_macro_current", "motor_macro_pwm",
                 "sensor_position_pos")
# the control modes of amc.mot that a mode can ask (amcmj1_motdriver.h). 0: the mode does not use the motor_macro
CONTROLMODES = {"current": 0x06, "speedvoltage": 0x0A, "openloop": 0x50}
# the config of the motor_macro, in the order of the CTL: (section, key, kind, min, max). kind f: float, i: integer
MMCFG_LAYOUT = (("current_pid", "kp", "f", None, None), ("current_pid", "ki", "f", None, None), ("current_pid", "kd", "f", None, None),
                ("velocity_pid", "kp", "f", None, None), ("velocity_pid", "ki", "f", None, None), ("velocity_pid", "kd", "f", None, None),
                ("current_limits", "nominal", "f", 0.0, None), ("current_limits", "peak", "f", 0.0, None),
                ("current_limits", "overload", "f", 0.0, None),
                ("motor", "flags", "i", 0, 255), ("motor", "poles", "i", 1, 255), ("motor", "entlr", "i", 0, 255),
                ("motor", "rotencres", "i", -32768, 32767), ("motor", "rotindexoff", "i", -32768, 32767))
MMCFG_NAMES = tuple(f"{a}.{b}" for a, b, _, _, _ in MMCFG_LAYOUT)

ANGLE_FIELDS = ("motor_cubemars_pos", "motor_cubemars_vel", "motor_macro_pos", "motor_macro_vel", "sensor_position_pos")    # rad or rad/s in the STATUS
# the unit of a parameter -> the factor to SI, which is what the modes see. Any other unit: factor 1
UNIT_SCALE = {"deg": math.pi / 180.0, "deg/s": math.pi / 180.0, "deg/s^2": math.pi / 180.0}

Param = namedtuple("Param", "name unit default min max scale")
Mode = namedtuple("Mode", "name id needs needs_mask help params speedgoat_only controlmode impl cube_enable")
IMPLS = ("matlab", "simulink")      # matlab: sgact_mode_<name>.m (+ mode_<name>() for the daemon); simulink: a subsystem of USER CONTROL
Plot = namedtuple("Plot", "title fields")
_NAME = re.compile(r"[A-Za-z_]\w*$")


class SpecError(Exception):
    pass


class Spec:
    def __init__(self, path, crc, version, keepalive, max_params, devices, device_help, actions, modes, plots, angles, mmcfg):
        self.path = path
        self.mmcfg = mmcfg
        self.angles = angles
        self.crc = crc
        self.version = version
        self.keepalive = keepalive
        self.max_params = max_params
        self.devices = devices
        self.device_help = device_help
        self.actions = actions
        self.action_names = {v: k for k, v in actions.items()}
        self.modes = modes
        self.mode_by_id = {m.id: m for m in modes}
        self.mode_by_name = {m.name: m for m in modes}
        self.plots = plots

    def mask(self, names):
        """the device mask of a list of device names"""
        return sum(1 << self.devices[n] for n in names)

    def names_of(self, mask):
        return [n for n, b in self.devices.items() if mask & (1 << b)]

    def to_si(self, mode, params):
        """the parameters of the mode with that id, from the units of the TOML to SI (what the modes see)"""
        m = self.mode_by_id.get(mode)
        scales = [p.scale for p in m.params] if m else []
        return [x * (scales[k] if k < len(scales) else 1.0) for k, x in enumerate(params)]

    def angle_factor(self):
        """rad -> the unit of the panel"""
        return 180.0 / math.pi if self.angles == "deg" else 1.0


def crc_of(path):
    """CRC32 of the file with CRLF as LF: the same on Windows and on Linux, whatever git did to the line ends"""
    with open(path, "rb") as f:
        return zlib.crc32(f.read().replace(b"\r\n", b"\n")) & 0xFFFFFFFF


def find(path=None):
    """the file: the given path, else $SGACT_SPEC, else next to this module, else host/speedgoat/sgact_panel.toml"""
    here = os.path.dirname(os.path.abspath(__file__))
    candidates = [path, os.environ.get("SGACT_SPEC"), os.path.join(here, SPEC_NAME),
                  os.path.join(here, "..", "speedgoat", SPEC_NAME)]
    for c in candidates:
        if c and os.path.isfile(c):
            return os.path.abspath(c)
    raise SpecError(f"{SPEC_NAME} not found: give its path (--spec) or set SGACT_SPEC")


def load(path=None):
    path = find(path)
    with open(path, "rb") as f:
        try:
            t = tomllib.load(f)
        except tomllib.TOMLDecodeError as e:
            raise SpecError(f"{path}: {e}")

    link = t.get("link", {})
    version = int(link.get("version", 0))
    keepalive = float(link.get("keepalive_s", 1.0))
    max_params = int(link.get("max_params", 8))
    if version != 4:
        raise SpecError(f"{path}: link.version is {version}, these tools speak version 4")

    devices, device_help = {}, {}
    for name, d in t.get("devices", {}).items():
        if name not in DEVICE_NAMES:
            raise SpecError(f"device {name}: unknown to the supervisor (known: {', '.join(DEVICE_NAMES)})")
        bit = int(d["bit"])
        if not 0 <= bit <= 7 or bit in devices.values():
            raise SpecError(f"device {name}: bit {bit} is out of 0..7 or used twice")
        devices[name], device_help[name] = bit, d.get("help", "")

    mmcfg = [0.0] * len(MMCFG_LAYOUT)
    if "motor_macro" in devices:
        cfg = t["devices"]["motor_macro"].get("config")
        if not isinstance(cfg, dict):
            raise SpecError("devices.motor_macro needs its [devices.motor_macro.config]")
        for k, (sec, key, kind, lo, hi) in enumerate(MMCFG_LAYOUT):
            try:
                v = cfg[sec][key]
            except (KeyError, TypeError):
                raise SpecError(f"devices.motor_macro.config: {sec}.{key} is missing")
            if kind == "i" and not (isinstance(v, int) and not isinstance(v, bool)):
                raise SpecError(f"devices.motor_macro.config: {sec}.{key} must be an integer")
            if not isinstance(v, (int, float)) or isinstance(v, bool) or (lo is not None and v < lo) or (hi is not None and v > hi):
                raise SpecError(f"devices.motor_macro.config: {sec}.{key} = {v} is not valid")
            mmcfg[k] = float(v)
        if mmcfg[7] < mmcfg[6]:
            raise SpecError("devices.motor_macro.config: current_limits.peak must be >= nominal")

    actions = {k: int(v) for k, v in t.get("actions", {}).items()}
    if sorted(actions) != sorted(ACTION_NAMES) or len(set(actions.values())) != 4 or not all(1 <= v <= 254 for v in actions.values()):
        raise SpecError(f"actions must be {', '.join(ACTION_NAMES)} with different ids in 1..254")

    modes = []
    for name, m in t.get("modes", {}).items():
        if not _NAME.match(name):
            raise SpecError(f"mode {name}: not a valid name")
        mid = int(m["id"])
        if not 0 <= mid <= 254 or any(x.id == mid for x in modes):
            raise SpecError(f"mode {name}: id {mid} is out of 0..254 or used twice")
        needs = list(m.get("needs", []))
        for n in needs:
            if n not in devices:
                raise SpecError(f"mode {name}: needs the unknown device {n}")
        params = []
        for p in m.get("params", []):
            unit = p.get("unit", "")
            pr = Param(p["name"], unit, float(p["default"]), float(p["min"]), float(p["max"]), UNIT_SCALE.get(unit, 1.0))
            if not _NAME.match(pr.name) or any(x.name == pr.name for x in params):
                raise SpecError(f"mode {name}: parameter {pr.name} is not a valid name or is used twice")
            if not pr.min <= pr.default <= pr.max:
                raise SpecError(f"mode {name}: parameter {pr.name}: default {pr.default} is out of [{pr.min}, {pr.max}]")
            params.append(pr)
        if len(params) > max_params:
            raise SpecError(f"mode {name}: {len(params)} parameters, at most {max_params}")
        impl = m.get("impl", "matlab")
        if impl not in IMPLS:
            raise SpecError(f"mode {name}: impl {impl} is not one of {', '.join(IMPLS)}")
        cm = m.get("controlmode")
        if cm is not None and cm not in CONTROLMODES:
            raise SpecError(f"mode {name}: controlmode {cm} is not one of {', '.join(CONTROLMODES)}")
        if cm is not None and "motor_macro" not in needs:
            raise SpecError(f"mode {name}: a controlmode needs the motor_macro in needs")
        cube_enable = bool(m.get("cubemars_enable", True)) and "motor_cubemars" in needs
        if "cubemars_enable" in m and "motor_cubemars" not in needs:
            raise SpecError(f"mode {name}: cubemars_enable needs the motor_cubemars in needs")
        modes.append(Mode(name, mid, needs, sum(1 << devices[n] for n in needs), m.get("help", ""), params,
                          bool(m.get("speedgoat_only", False)) or impl == "simulink", CONTROLMODES.get(cm, 0), impl,
                          cube_enable))
    if not modes:
        raise SpecError("no modes")
    modes.sort(key=lambda m: m.id)

    angles = t.get("display", {}).get("angles", "rad")
    if angles not in ("deg", "rad"):
        raise SpecError(f"display.angles is {angles}: it must be deg or rad")

    plots = []
    for p in t.get("plots", []):
        for f in p.get("fields", []):
            if f not in STATUS_FIELDS:
                raise SpecError(f"plot {p.get('title')}: unknown field {f} (known: {', '.join(STATUS_FIELDS)})")
        plots.append(Plot(p.get("title", ""), list(p.get("fields", []))))

    return Spec(path, crc_of(path), version, keepalive, max_params, devices, device_help, actions, modes, plots, angles, mmcfg)

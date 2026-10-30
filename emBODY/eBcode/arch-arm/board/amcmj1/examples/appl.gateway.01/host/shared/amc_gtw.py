"""
The Speedgoat link of amcmj1 seen from the PC: the frames of the commands, rpc and state sockets (amcmj1_gwlink.h).

    streamer / panel  --UDP 5001 commands-->  amcmj1 (tGATEWAY)  --CAN-->  CubeMars (or cubemars_emulator.py)
    streamer / panel  <--UDP 5003 state-----  amcmj1
    streamer / panel  --UDP 5002 rpc-------->  amcmj1           run<OPcubeENABLE> ... run<OPmotorMODE, mode>
                                               amcmj1  --ICC-->  amc.mot (CM7) -> macromotor

The frames are TETHER frames (tethercodec.py, next to this file): the same ones that the Simulink model of the Speedgoat sends.

  commands   set<IDcubeMIT, {kp, kd, pos, vel, torque, seq}>      a frame at every step, also when it has nothing to say
             set<IDmotorTARGET, {value, mode, seq}>              the setpoint of the macromotor, while its control mode is not idle
  rpc        run[+ack]<OPcubeENABLE>, <OPcubeDISABLE>, <OPpaxENABLE>, <OPpaxDISABLE>
             run[+ack]<OPmotorPIDS, {curkp curki curkd velkp velki velkd}>
             run[+ack]<OPmotorSETUP, {nominal peak overload flags poles entlr rotencres rotindexoff}>
             run[+ack]<OPmotorMODE, mode>                         0x00 idle, 0x06 current, 0x0A speedvoltage, 0x50 openloop
  state      sig<IDcubeSTATUS>, sig<IDcubeSTATE>, sig<IDpaxTORQUE>, sig<IDmotorSTATE>, sig<IDgwSTATS>, sig<IDposSTATE>
             (slots of 40 bytes)

The units of the macromotor are the ones of amc.mot: current [A], velocity [deg/s], position [deg], pwm [%].
"""

import os
import socket
import struct
import sys
import time
from collections import namedtuple

_HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, _HERE)                           # tethercodec.py is next to this file
import tethercodec as rc                            # noqa: E402

PORT_CMD, PORT_RPC, PORT_STATE = 5001, 5002, 5003   # the same of appl.gateway.01.cpp (amcmj1::net)

ID_CUBE_STATUS, ID_CUBE_STATE, ID_CUBE_MIT = 0x0B, 0x0C, 0x0D
ID_PAX_TORQUE, ID_MOTOR_STATE, ID_GW_STATS = 0x10, 0x11, 0x12
ID_MOTOR_PIDS, ID_MOTOR_SETUP, ID_MOTOR_TARGET, ID_MOTOR_STATUS = 0x14, 0x15, 0x16, 0x17
ID_POS_STATE = 0x18                                 # the position sensor of the joint (amcmj1_posdriver.h)
OP_CUBE_ENABLE, OP_CUBE_DISABLE = 0xD1, 0xD2
OP_PAX_ENABLE, OP_PAX_DISABLE = 0xD4, 0xD5
OP_MOTOR_PIDS, OP_MOTOR_SETUP, OP_MOTOR_MODE = 0xD6, 0xD7, 0xD8
OP_GW_FAULT = 0xD9                                  # the EXTFAULT held by amc.gtw: argument uint8 1 set, 0 clear

# the control modes of amc.mot (amcmj1_motdriver.h). hwfault is only reported
CM_IDLE, CM_CURRENT, CM_SPEEDVOLTAGE, CM_OPENLOOP, CM_HWFAULT = 0x00, 0x06, 0x0A, 0x50, 0xA0
CM_NAMES = {CM_IDLE: "idle", CM_CURRENT: "current", CM_SPEEDVOLTAGE: "speedvoltage", CM_OPENLOOP: "openloop", CM_HWFAULT: "hwfault"}
FAULT_EXTERNAL = 1 << 7                             # bit of MotorState.faults: the external fault button is pressed
DEVICES = ("cube", "pax", "both")

# the layouts of amcmj1_cubedriver.h and amcmj1_gwlink.h, little endian, no padding (the static_asserts of the firmware)
MIT_FMT = "<fffffI"                                 # kp kd pos vel torque seq               24 bytes
STATUS_FMT = "<BBHII"                               # state lastcommand errors commands watchdogs   12 bytes
STATE_FMT = "<fffhHI"                               # pos vel current temp error counter     20 bytes
TORQUE_FMT = "<fffI"                                # Mx My Mz counter                       16 bytes
MOTOR_FMT = "<ffffIIBBBB"                           # current velocity position pwm faults counter controlmode configured emulated gwfault  28 bytes
STATS_FMT = "<9I"                                   # seq packets discarded clamped rejected ignored lost badframes notrunning  36 bytes
PIDS_FMT = "<6f"                                    # curkp curki curkd velkp velki velkd     24 bytes
SETUP_FMT = "<fffBBBBhh"                            # nominal peak overload flags poles entlr reserved rotencres rotindexoff  20 bytes
TARGET_FMT = "<fB3xI"                               # value mode seq                         12 bytes
POS_FMT = "<fIIHBB"                                 # position counter errors raw flags reserved   16 bytes
POS_VALID, POS_EMULATED = 0x01, 0x02                # PosState.flags
for _f, _n in ((MIT_FMT, 24), (STATUS_FMT, 12), (STATE_FMT, 20), (TORQUE_FMT, 16), (MOTOR_FMT, 28), (STATS_FMT, 36),
               (PIDS_FMT, 24), (SETUP_FMT, 20), (TARGET_FMT, 12), (POS_FMT, 16)):
    assert struct.calcsize(_f) == _n

CubeStatus = namedtuple("CubeStatus", "state lastcommand errors commands watchdogs")
CubeState = namedtuple("CubeState", "pos vel current temp error counter")
PaxTorque = namedtuple("PaxTorque", "mx my mz counter")
MotorState = namedtuple("MotorState", "current velocity position pwm faults counter controlmode configured emulated gwfault")
GwStats = namedtuple("GwStats", "seq packets discarded clamped rejected ignored lost badframes notrunning")
PosState = namedtuple("PosState", "position counter errors raw flags reserved")
State = namedtuple("State", "time cube_status cube_state pax_torque motor gw pos", defaults=(None,))

CUBE_STATES = {0: "off", 1: "initialised", 2: "running", 3: "error"}

_CODEC = rc.Codec(rc.Protocol())                    # only to build and decode frames: no Dictionary needed


def mit_frame(kp, kd, pos, vel, torque, seq):
    """the command frame: one set<IDcubeMIT, ...>. the slot is the one that holds the 24 bytes of the struct"""
    return command_frame((kp, kd, pos, vel, torque, seq), None)


def command_frame(mit=None, target=None):
    """the command frame w/ what there is: mit = (kp, kd, pos, vel, torque, seq) and / or target = (value, mode, seq).
    all the slots of a frame have the size of the biggest ROP: 32 bytes w/ the MIT, 16 w/ the target alone"""
    rops = []
    if mit is not None:
        kp, kd, pos, vel, torque, seq = mit
        rops.append(rc.make_rop(rc.CMD.set, ID_CUBE_MIT, struct.pack(MIT_FMT, kp, kd, pos, vel, torque, seq & 0xFFFFFFFF)))
    if target is not None:
        value, mode, seq = target
        rops.append(rc.make_rop(rc.CMD.set, ID_MOTOR_TARGET, struct.pack(TARGET_FMT, value, mode, seq & 0xFFFFFFFF)))
    return _CODEC.build_frame(rops)


def pids_arg(curkp, curki, curkd, velkp, velki, velkd):
    """the argument of OPmotorPIDS"""
    return struct.pack(PIDS_FMT, curkp, curki, curkd, velkp, velki, velkd)


def setup_arg(nominal, peak, overload, flags, poles, entlr, rotencres, rotindexoff):
    """the argument of OPmotorSETUP"""
    return struct.pack(SETUP_FMT, nominal, peak, overload, int(flags), int(poles), int(entlr), 0, int(rotencres), int(rotindexoff))


def mode_arg(mode):
    """the argument of OPmotorMODE"""
    return struct.pack("<B", int(mode))


def fault_arg(on):
    """the argument of OPgwFAULT: 1 set (amc.gtw stops both motors and refuses them), 0 clear"""
    return struct.pack("<B", 1 if on else 0)


def empty_frame():
    """a command frame w/out a MIT: the board sends the last one again. it still feeds the watchdog"""
    return _CODEC.build_frame([])


def run_frame(op, ack=True, arg=b""):
    """the rpc frame run[+ack]<op, arg>: one ROP, in the smallest slot that holds arg"""
    return _CODEC.build_frame([rc.make_rop(rc.CMD.run, op, bytes(arg), rc.OPT_ACK if ack else 0)])


def decode_state(data):
    """a state frame -> State, or None if it is not one. a missing ROP is None"""
    try:
        fr = _CODEC.decode_frame(data)
    except rc.ProtocolError:
        return None
    got = {}
    for r in fr.rops:
        if r.cmd != rc.CMD.sig:
            continue
        spec = {ID_CUBE_STATUS: (STATUS_FMT, CubeStatus), ID_CUBE_STATE: (STATE_FMT, CubeState), ID_PAX_TORQUE: (TORQUE_FMT, PaxTorque),
                ID_MOTOR_STATE: (MOTOR_FMT, MotorState), ID_GW_STATS: (STATS_FMT, GwStats), ID_POS_STATE: (POS_FMT, PosState)}.get(r.id)
        if spec and r.size == struct.calcsize(spec[0]):
            got[r.id] = spec[1](*struct.unpack(spec[0], r.data[:r.size]))
    if ID_GW_STATS not in got:
        return None
    return State(fr.time, got.get(ID_CUBE_STATUS), got.get(ID_CUBE_STATE), got.get(ID_PAX_TORQUE), got.get(ID_MOTOR_STATE), got[ID_GW_STATS],
                 got.get(ID_POS_STATE))


def decode_reply(data, op):
    """the reply of the rpc socket: (True, "") for the ack of op, (False, text) for the nak, None if it is not a reply to op"""
    try:
        fr = _CODEC.decode_frame(data)
    except rc.ProtocolError:
        return None
    for r in fr.rops:
        if r.id == op and r.cmd == rc.CMD.ack:
            return True, ""
        if r.id == op and r.cmd == rc.CMD.nak:
            return False, rc.format_text(r.data) or "nak"
    return None


class Gateway:
    """the three sockets of the PC side. cmd and rpc are connected to the board, state listens on the port where the board sends"""

    def __init__(self, board="10.0.1.99", cmdport=PORT_CMD, rpcport=PORT_RPC, stateport=PORT_STATE, bind=""):
        self.cmd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.cmd.connect((board, cmdport))
        self.rpc = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.rpc.connect((board, rpcport))
        self.state = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.state.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.state.bind((bind, stateport))
        self.state.settimeout(0.05)

    def send_mit(self, kp, kd, pos, vel, torque, seq):
        self.cmd.send(mit_frame(kp, kd, pos, vel, torque, seq))

    def send_empty(self):
        self.cmd.send(empty_frame())

    def send_command(self, mit=None, target=None):
        """a command frame w/ the MIT and / or the target of the macromotor (see command_frame)"""
        self.cmd.send(command_frame(mit, target))

    def recv_state(self, timeout=0.05):
        """the next State, or None on timeout"""
        self.state.settimeout(timeout)
        try:
            data = self.state.recv(2048)
        except (socket.timeout, OSError):
            return None
        return decode_state(data)

    def rpc_run(self, op, timeout=0.5, arg=b""):
        """run+ack<op, arg>: (True, "") ack, (False, text) nak, (None, "no reply")"""
        self.rpc.settimeout(0.001)
        try:
            while True:
                self.rpc.recv(2048)                 # late replies
        except (socket.timeout, OSError):
            pass
        self.rpc.send(run_frame(op, True, arg))
        deadline = time.monotonic() + timeout
        while (left := deadline - time.monotonic()) > 0:
            self.rpc.settimeout(left)
            try:
                res = decode_reply(self.rpc.recv(2048), op)
            except (socket.timeout, OSError):
                break
            if res is not None:
                return res
        return None, "no reply"

    def enable(self, devices="cube"):
        """OPcubeENABLE and/or OPpaxENABLE ("cube", "pax", "both"): (True, "") if all acked, else (False, text of the first nak / "no reply").
        the PX3Q is first: it acks its commands later, the CubeMars needs the MIT that follows anyway"""
        return self._run_devices(devices, ((OP_PAX_ENABLE, "pax"), (OP_CUBE_ENABLE, "cube")))

    def disable(self, devices="cube"):
        """the CubeMars is first: the motor stops before the sensor"""
        return self._run_devices(devices, ((OP_CUBE_DISABLE, "cube"), (OP_PAX_DISABLE, "pax")))

    def _run_devices(self, devices, ops):
        if devices not in DEVICES:
            raise ValueError(f"devices must be one of {DEVICES}")
        for op, dev in ops:
            if devices in (dev, "both"):
                ok, why = self.rpc_run(op)
                if ok is not True:
                    return ok, f"{dev}: {why}" if why else dev
        return True, ""

    def close(self):
        for s in (self.cmd, self.rpc, self.state):
            s.close()

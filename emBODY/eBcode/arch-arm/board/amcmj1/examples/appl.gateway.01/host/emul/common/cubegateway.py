"""
The Speedgoat link of amcmj1 seen from the PC: the frames of the commands, rpc and state sockets (amcmj1_gwlink.h).

    streamer / panel  --UDP 5001 commands-->  amcmj1 (tGATEWAY)  --CAN-->  CubeMars (or cubemars_emulator.py)
    streamer / panel  <--UDP 5003 state-----  amcmj1
    streamer / panel  --UDP 5002 rpc-------->  amcmj1           run<OPcubeENABLE>, run<OPcubeDISABLE>

The frames are TETHER frames (tethercodec.py, in the folder above): the same ones that the Simulink model of the Speedgoat sends.

  commands   set<IDcubeMIT, {kp, kd, pos, vel, torque, seq}>      a frame at every step, also when it has nothing to say
  state      sig<IDcubeSTATUS>, sig<IDcubeSTATE>, sig<IDpaxTORQUE>, sig<IDmotorSTATE>, sig<IDgwSTATS>   (slots of 40 bytes)
"""

import os
import socket
import struct
import sys
import time
from collections import namedtuple

_HERE = os.path.dirname(os.path.abspath(__file__))
_HOST = os.path.dirname(os.path.dirname(_HERE))        # host/: the folders below hold the modules shared by the tools
for _d in ("tether", "emul/common", "emul/cubemars", "emul/paxini", "emul/speedgoat"):
    sys.path.insert(0, os.path.join(_HOST, _d))
import tethercodec as rc                            # noqa: E402

PORT_CMD, PORT_RPC, PORT_STATE = 5001, 5002, 5003   # the same of appl.gateway.01.cpp (amcmj1::net)

ID_CUBE_STATUS, ID_CUBE_STATE, ID_CUBE_MIT = 0x0B, 0x0C, 0x0D
ID_PAX_TORQUE, ID_MOTOR_STATE, ID_GW_STATS = 0x10, 0x11, 0x12
OP_CUBE_ENABLE, OP_CUBE_DISABLE = 0xD1, 0xD2
OP_PAX_ENABLE, OP_PAX_DISABLE = 0xD4, 0xD5
DEVICES = ("cube", "pax", "both")

# the layouts of amcmj1_cubedriver.h and amcmj1_gwlink.h, little endian, no padding (the static_asserts of the firmware)
MIT_FMT = "<fffffI"                                 # kp kd pos vel torque seq               24 bytes
STATUS_FMT = "<BBHII"                               # state lastcommand errors commands watchdogs   12 bytes
STATE_FMT = "<fffhHI"                               # pos vel current temp error counter     20 bytes
TORQUE_FMT = "<fffI"                                # Mx My Mz counter                       16 bytes
MOTOR_FMT = "<fffBBHI"                              # pos vel current state error reserved counter  20 bytes
STATS_FMT = "<9I"                                   # seq packets discarded clamped rejected ignored lost badframes notrunning  36 bytes
for _f, _n in ((MIT_FMT, 24), (STATUS_FMT, 12), (STATE_FMT, 20), (TORQUE_FMT, 16), (MOTOR_FMT, 20), (STATS_FMT, 36)):
    assert struct.calcsize(_f) == _n

CubeStatus = namedtuple("CubeStatus", "state lastcommand errors commands watchdogs")
CubeState = namedtuple("CubeState", "pos vel current temp error counter")
PaxTorque = namedtuple("PaxTorque", "mx my mz counter")
MotorState = namedtuple("MotorState", "pos vel current state error reserved counter")
GwStats = namedtuple("GwStats", "seq packets discarded clamped rejected ignored lost badframes notrunning")
State = namedtuple("State", "time cube_status cube_state pax_torque motor gw")

CUBE_STATES = {0: "off", 1: "initialised", 2: "running", 3: "error"}

_CODEC = rc.Codec(rc.Protocol())                    # only to build and decode frames: no Dictionary needed


def mit_frame(kp, kd, pos, vel, torque, seq):
    """the command frame: one set<IDcubeMIT, ...>. the slot is the one that holds the 24 bytes of the struct"""
    data = struct.pack(MIT_FMT, kp, kd, pos, vel, torque, seq & 0xFFFFFFFF)
    return _CODEC.build_frame([rc.make_rop(rc.CMD.set, ID_CUBE_MIT, data)])


def empty_frame():
    """a command frame w/out a MIT: the board sends the last one again. it still feeds the watchdog"""
    return _CODEC.build_frame([])


def run_frame(op, ack=True):
    return _CODEC.build_frame([rc.make_rop(rc.CMD.run, op, b"", rc.OPT_ACK if ack else 0)])


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
                ID_MOTOR_STATE: (MOTOR_FMT, MotorState), ID_GW_STATS: (STATS_FMT, GwStats)}.get(r.id)
        if spec and r.size == struct.calcsize(spec[0]):
            got[r.id] = spec[1](*struct.unpack(spec[0], r.data[:r.size]))
    if ID_GW_STATS not in got:
        return None
    return State(fr.time, got.get(ID_CUBE_STATUS), got.get(ID_CUBE_STATE), got.get(ID_PAX_TORQUE), got.get(ID_MOTOR_STATE), got[ID_GW_STATS])


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

    def recv_state(self, timeout=0.05):
        """the next State, or None on timeout"""
        self.state.settimeout(timeout)
        try:
            data = self.state.recv(2048)
        except (socket.timeout, OSError):
            return None
        return decode_state(data)

    def rpc_run(self, op, timeout=0.5):
        """run+ack<op>: (True, "") ack, (False, text) nak, (None, "no reply")"""
        self.rpc.settimeout(0.001)
        try:
            while True:
                self.rpc.recv(2048)                 # late replies
        except (socket.timeout, OSError):
            pass
        self.rpc.send(run_frame(op, True))
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

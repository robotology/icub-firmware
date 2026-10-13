# self-test on the virtual bus: a master that does what amcmj1_paxdriver does (stop, getVersion, setRate, start)
import struct, sys, threading, time, can
import px3q_emulator as pe

ch = "t1"
emubus = can.Bus(interface="virtual", channel=ch)
emu = pe.PX3Q(emubus, verbose=False)
th = threading.Thread(target=emu.run, daemon=True); th.start()
m = can.Bus(interface="virtual", channel=ch)

def cmd(op, arg=0):
    m.send(can.Message(arbitration_id=0x200 + 0x7F, data=[op, arg, 0, 0, 0, 0, 0, 0], is_extended_id=False))
    t = time.time()
    while time.time() - t < 0.5:
        r = m.recv(0.1)
        if r and r.arbitration_id == 0x180 + 0x7F and r.data[1] == op:
            return r
    return None

assert cmd(pe.OP_STOP).data[0] == 0
v = cmd(pe.OP_GETVERSION); assert bytes(v.data[2:8]) == b"v0.1.1"
assert cmd(pe.OP_SETRATE, 25).data[0] == 0 and emu.rate_hz == 100
assert cmd(pe.OP_SETRATE, 1).data[0] == 0 and emu.rate_hz == 4        # the lowest: 4 Hz
assert cmd(pe.OP_SETRATE, 0).data[0] == 1 and emu.rate_hz == 4        # byte 0 (0 Hz) refused, the rate is not changed
assert cmd(pe.OP_SETRATE, 250).data[0] == 0 and emu.rate_hz == 1000
assert cmd(pe.OP_SETRATE, 251).data[0] == 1                      # 1004 Hz refused
assert cmd(pe.OP_SETRATE, 25).data[0] == 0
assert cmd(0x55).data[0] == 1                                    # unknown opcode
assert cmd(pe.OP_START).data[0] == 0
frames = []; t = time.time()
while time.time() - t < 1.0:
    r = m.recv(0.05)
    if r: frames.append(r)
A = [f for f in frames if f.arbitration_id == 0x280 + 0x7F]; B = [f for f in frames if f.arbitration_id == 0x380 + 0x7F]
print("100 Hz for 1 s:", len(A), "A,", len(B), "B")
assert 90 <= len(A) <= 105 and abs(len(A) - len(B)) <= 1
ids = [f.arbitration_id for f in frames]
assert all(ids[i] == 0x280 + 0x7F and ids[i + 1] == 0x380 + 0x7F for i in range(0, len(ids) - 1, 2))   # A then B
mx, my = struct.unpack("<ff", bytes(A[5].data)); mz, _ = struct.unpack("<ff", bytes(B[5].data))
assert abs(mx) <= 1.01 and abs(my) <= 2.01 and abs(mz) <= 3.01
assert cmd(pe.OP_STOP).data[0] == 0
time.sleep(0.1)
while m.recv(0.05): pass
assert m.recv(0.3) is None                                       # no stream after stop
# drop / nak
emu.drop = {pe.OP_GETVERSION}; assert cmd(pe.OP_GETVERSION) is None
emu.drop = set(); emu.nak = {pe.OP_START}; assert cmd(pe.OP_START).data[0] == 1
emu.nak = set(); assert cmd(pe.OP_READONCE).data[0] == 0
time.sleep(0.1); got = {f.arbitration_id for f in iter(lambda: m.recv(0.1), None)}
assert {0x280 + 0x7F, 0x380 + 0x7F} <= got
# device id: not specified or 0 -> the default 0x7F; a wrong one is refused
assert pe.PX3Q(can.Bus(interface="virtual", channel="t2")).devid == 0x7F
assert pe.PX3Q(can.Bus(interface="virtual", channel="t3"), devid=0).devid == 0x7F
assert pe.PX3Q(can.Bus(interface="virtual", channel="t4"), devid=None).devid == 0x7F
assert pe.PX3Q(can.Bus(interface="virtual", channel="t5"), devid=0x10).id_cmd == 0x210
for bad in (128, -1):
    try:
        pe.PX3Q(can.Bus(interface="virtual", channel="t6"), devid=bad); assert False
    except ValueError:
        pass
# printing is optional: nothing printed while streaming by default, one line every N samples with trace_stream
import io, contextlib
def streamed_output(trace, seconds=1.0):
    emu.trace_stream = trace
    assert cmd(pe.OP_SETRATE, 25).data[0] == 0
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        assert cmd(pe.OP_START).data[0] == 0
        time.sleep(seconds)
        assert cmd(pe.OP_STOP).data[0] == 0
    return buf.getvalue().splitlines()
assert streamed_output(0) == []                                  # default: silent
lines = streamed_output(10)
print("trace every 10 samples at 100 Hz for 1 s:", len(lines), "lines", file=sys.__stdout__)
assert 8 <= len(lines) <= 12 and "Mx=" in lines[0]
emu.trace_stream = 0
print("OK")

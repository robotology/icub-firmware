#!/usr/bin/env python3
"""
PaXini PX3Q torque sensor emulator (CAN grammar of the manual, sec. 5.3), for python-can.

Bus: classic CAN, 1 Mbps, 11-bit IDs, DLC 8. ID11 = deviceID + (TYPE << 7). Default device id 0x7F:
if the id is not specified, or it is 0, the default 0x7F is used (as the amcmj1 firmware does).
  RX  command   0x200 + id   TYPE 0b0100   data[0] = cmd, data[1..7] = args
  TX  reply     0x180 + id   TYPE 0b0011   data[0] = status (0 = ok), data[1] = cmd, data[2..7] = payload
  TX  stream A  0x280 + id   TYPE 0b0101   Mx, My (float32 little endian)
  TX  stream B  0x380 + id   TYPE 0b0111   Mz (float32 little endian), reserved

Printing. By default nothing is printed but the start banner. The messages are optional, because at 1 kHz printing is heavy:
  -v / --verbose        the commands received and the replies sent (a few lines per run, not the stream)
  --trace-stream N      one line every N stream samples (A+B). 0 = never (default). e.g. --trace-stream 100
  --sniff               every frame on the bus, ours or not: very heavy while streaming

Example with a Vector VN1610 (channel 0 of the Vector hardware config):
  python px3q_emulator.py --interface vector --channel 0 --app-name CANalyzer
  python px3q_emulator.py --interface vector --channel 2 --app-name none     (global channel index of the VN1610)
Without hardware (two terminals or a test), the virtual bus:
  python px3q_emulator.py --interface virtual --channel test
"""

import argparse
import math
import random
import struct
import sys
import threading
import time

import can

OP_SETID, OP_SETRATE, OP_START, OP_STOP, OP_READONCE = 0x01, 0x02, 0x03, 0x04, 0x05
OP_GETVERSION, OP_CALIBRATE, OP_SETPORT = 0x07, 0x10, 0x11
OPNAMES = {OP_SETID: "setID", OP_SETRATE: "setRate", OP_START: "start", OP_STOP: "stop", OP_READONCE: "readOnce",
           OP_GETVERSION: "getVersion", OP_CALIBRATE: "calibrate", OP_SETPORT: "setPort"}
OPCODES = {v: k for k, v in OPNAMES.items()}

DEFAULT_ID = 0x7F                               # used when the device id is not specified (or it is 0)


class PX3Q:
    def __init__(self, bus, devid=0x7F, version="v0.1.1", amplitude=(1.0, 2.0, 3.0),
                 freq=(0.5, 0.25, 0.1), noise=0.0, drop=(), nak=(), delay_ms=0.0, verbose=False, sniff=False,
                 trace_stream=0):
        self.bus = bus
        if devid is None or devid == 0:
            devid = DEFAULT_ID                  # not specified: the default one, as the firmware does
        if not 1 <= devid <= 0x7F:
            raise ValueError(f"device id must be 0 (default 0x7F) or in [1, 127], not {devid}")
        self.devid = devid
        self.version = version.encode("ascii")[:6].ljust(6, b"\0")
        self.amplitude, self.freq, self.noise = amplitude, freq, noise
        self.drop, self.nak = set(drop), set(nak)   # commands that get no reply / a negative reply
        self.delay = delay_ms / 1000.0
        self.verbose = verbose                  # print the commands received and the replies sent
        self.trace_stream = max(0, int(trace_stream))   # print one stream sample every N (0 = never)
        self.sniff = sniff                      # print every frame seen on the bus, ours or not
        self.seen = 0
        self.seen_errors = 0
        self.rate_hz = 100.0
        self.streaming = False
        self.offset = [0.0, 0.0, 0.0]           # set by calibrate
        self.t0 = time.monotonic()
        self.lock = threading.Lock()
        self.stop_evt = threading.Event()
        self.samples = 0
        self.thread = threading.Thread(target=self._stream, daemon=True)

    # ids
    @property
    def id_cmd(self): return 0x200 + self.devid
    @property
    def id_reply(self): return 0x180 + self.devid
    @property
    def id_a(self): return 0x280 + self.devid
    @property
    def id_b(self): return 0x380 + self.devid

    def log(self, *a):
        if self.verbose:
            print(f"[{time.monotonic() - self.t0:9.3f}] ", *a, flush=True)

    def send(self, arbid, data):
        self.bus.send(can.Message(arbitration_id=arbid, data=bytes(data), is_extended_id=False, is_fd=False))

    # the signal: a sine per axis, plus noise and the offset of calibrate
    def torque(self):
        t = time.monotonic() - self.t0
        return [self.amplitude[i] * math.sin(2 * math.pi * self.freq[i] * t) + random.gauss(0, self.noise) - self.offset[i]
                for i in range(3)]

    def send_sample(self):
        mx, my, mz = self.torque()
        self.send(self.id_a, struct.pack("<ff", mx, my))
        self.send(self.id_b, struct.pack("<ff", mz, 0.0))
        self.samples += 1
        if self.trace_stream and (0 == self.samples % self.trace_stream):
            print(f"[{time.monotonic() - self.t0:9.3f}] sample {self.samples}: "
                  f"Mx={mx:+.4f} My={my:+.4f} Mz={mz:+.4f}", flush=True)
        return mx, my, mz

    def reply(self, cmd, status=0, payload=b""):
        if cmd in self.drop:
            self.log(f"  (reply to {OPNAMES.get(cmd, hex(cmd))} dropped on purpose)")
            return
        if self.delay:
            time.sleep(self.delay)
        data = bytes([status, cmd]) + bytes(payload).ljust(6, b"\0")[:6]
        self.send(self.id_reply, data)
        self.log(f"TX reply {OPNAMES.get(cmd, hex(cmd))} status={status} data={data.hex(' ')}")

    # a command from the host
    def on_message(self, msg):
        if msg.is_extended_id or msg.arbitration_id != self.id_cmd:
            return
        if msg.dlc < 1:
            return
        cmd = msg.data[0]
        arg = msg.data[1] if msg.dlc > 1 else 0
        name = OPNAMES.get(cmd, f"unknown 0x{cmd:02X}")
        self.log(f"RX {name} arg={arg} data={bytes(msg.data).hex(' ')}")
        if cmd in self.nak:
            return self.reply(cmd, status=1)

        with self.lock:
            if cmd == OP_SETID:
                if arg > 0x7F:
                    return self.reply(cmd, status=1)
                self.log(f"  new id 0x{arg:02X} takes effect after a power cycle: the emulator keeps 0x{self.devid:02X}")
                self.reply(cmd)
            elif cmd == OP_SETRATE:
                hz = arg * 4.0                  # the argument is Hz / 4: byte 0 (0 Hz) and above 1000 Hz are refused
                if hz <= 0 or hz > 1000:
                    return self.reply(cmd, status=1)
                self.rate_hz = hz
                self.log(f"  rate {hz:g} Hz")
                self.reply(cmd)
            elif cmd == OP_START:
                self.reply(cmd)                 # the ack first, then the stream
                self.streaming = True
                self.log(f"  streaming started at {self.rate_hz:g} Hz")
            elif cmd == OP_STOP:
                self.streaming = False
                self.reply(cmd)
                self.log(f"  streaming stopped: {self.samples} samples sent so far")
            elif cmd == OP_READONCE:
                self.reply(cmd)
                self.send_sample()
            elif cmd == OP_GETVERSION:
                self.reply(cmd, payload=self.version)
            elif cmd == OP_CALIBRATE:
                self.offset = [0.0, 0.0, 0.0]
                self.offset = self.torque()
                self.reply(cmd)
            elif cmd == OP_SETPORT:
                self.reply(cmd)
            else:
                self.reply(cmd, status=1)

    # the stream: absolute schedule, so no drift. A then B back to back
    def _stream(self):
        nxt = time.monotonic()
        while not self.stop_evt.is_set():
            with self.lock:
                on, period = self.streaming, 1.0 / self.rate_hz
            now = time.monotonic()
            if not on:
                nxt = now + 0.002
                time.sleep(0.002)
                continue
            if now >= nxt:
                with self.lock:
                    if self.streaming:
                        self.send_sample()
                nxt += period
                if nxt < now - 0.1:             # fell far behind (e.g. a blocked bus): do not burst
                    nxt = now + period
            else:
                time.sleep(min(nxt - now, 0.001))

    def run(self):
        self.thread.start()
        self.log(f"PX3Q emulator on id 0x{self.devid:02X}: cmd 0x{self.id_cmd:03X}, reply 0x{self.id_reply:03X}, "
                 f"streams 0x{self.id_a:03X}/0x{self.id_b:03X}")
        try:
            lastbeat = time.monotonic()
            while True:
                msg = self.bus.recv(timeout=0.1)
                if msg is not None:
                    self.seen += 1
                    if msg.is_error_frame:
                        self.seen_errors += 1
                    if self.sniff:
                        kind = "ERROR-FRAME" if msg.is_error_frame else ("ext" if msg.is_extended_id else "std")
                        print(f"[{time.monotonic() - self.t0:9.3f}] SNIFF {kind} id=0x{msg.arbitration_id:X} dlc={msg.dlc} "
                              f"data={bytes(msg.data).hex(' ')}", flush=True)
                    self.on_message(msg)
                if self.sniff and time.monotonic() - lastbeat >= 2.0:
                    lastbeat = time.monotonic()
                    print(f"[{time.monotonic() - self.t0:9.3f}] alive: {self.seen} frames seen ({self.seen_errors} error frames), "
                          f"waiting for id 0x{self.id_cmd:03X}, streaming={self.streaming}", flush=True)
        except KeyboardInterrupt:
            pass
        finally:
            self.stop_evt.set()
            self.thread.join(timeout=1)


def main(argv=None):
    ap = argparse.ArgumentParser(description="PaXini PX3Q emulator")
    ap.add_argument("--interface", default="vector", help="python-can interface: vector, virtual, socketcan, pcan ...")
    ap.add_argument("--channel", default="0", help="vector: the application channel (0 = first channel of the VN1610)")
    ap.add_argument("--app-name", default="CANalyzer",
                    help="vector: name of the application in Vector Hardware Config; 'none' = --channel is the global channel "
                         "index listed by can.detect_available_configs('vector')")
    ap.add_argument("--bitrate", type=int, default=1000000)
    ap.add_argument("--id", type=lambda s: int(s, 0), default=None,
                    help="device id in [1, 127]. If not specified, or 0, the default 0x7F is used")
    ap.add_argument("--version", default="v0.1.1", help="version string (6 chars) of getVersion")
    ap.add_argument("--amplitude", type=float, nargs=3, default=(1.0, 2.0, 3.0), metavar=("MX", "MY", "MZ"))
    ap.add_argument("--freq", type=float, nargs=3, default=(0.5, 0.25, 0.1), metavar=("FX", "FY", "FZ"), help="Hz")
    ap.add_argument("--noise", type=float, default=0.0, help="sigma of the gaussian noise")
    ap.add_argument("--drop", nargs="*", default=[], choices=sorted(OPCODES), help="commands that get no reply (test the timeout)")
    ap.add_argument("--nak", nargs="*", default=[], choices=sorted(OPCODES), help="commands that get a negative reply")
    ap.add_argument("--delay-ms", type=float, default=0.0, help="delay of every reply")
    ap.add_argument("-v", "--verbose", action="store_true",
                    help="print the commands received and the replies sent (not the stream). Off by default")
    ap.add_argument("--trace-stream", type=int, default=0, metavar="N",
                    help="print one line every N stream samples (A+B). 0 = never, the default: printing is heavy at high rates")
    ap.add_argument("--sniff", action="store_true", help="debug: print every frame seen on the bus (any id, error frames too) "
                    "and a heartbeat every 2 s. Very heavy while streaming")
    a = ap.parse_args(argv)
    if a.id is not None and not 0 <= a.id <= 0x7F:
        ap.error(f"--id must be 0 (default 0x7F) or in [1, 127], not {a.id}")

    kw = {"interface": a.interface, "bitrate": a.bitrate}
    if a.interface == "vector":
        kw.update(channel=int(a.channel), app_name=None if a.app_name.lower() == "none" else a.app_name, fd=False)
    else:
        kw["channel"] = a.channel
    try:
        bus = can.Bus(**kw)
    except Exception as e:                      # noqa
        print(f"cannot open the bus {kw}: {e}", file=sys.stderr)
        return 1
    emu = PX3Q(bus, devid=a.id, version=a.version, amplitude=a.amplitude, freq=a.freq,
               noise=a.noise, drop={OPCODES[n] for n in a.drop}, nak={OPCODES[n] for n in a.nak},
               delay_ms=a.delay_ms, verbose=a.verbose, sniff=a.sniff, trace_stream=a.trace_stream)
    print(f"PX3Q emulator on {a.interface}:{a.channel} id=0x{emu.devid:02X} (Ctrl+C to quit)", flush=True)
    emu.run()
    bus.shutdown()
    return 0


if __name__ == "__main__":
    sys.exit(main())

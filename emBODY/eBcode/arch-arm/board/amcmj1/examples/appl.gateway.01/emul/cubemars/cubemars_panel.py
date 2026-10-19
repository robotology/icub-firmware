#!/usr/bin/env python3
"""
Command panel for the CubeMars emulator (or for a real AK actuator): a CAN master with a GUI.

    cubemars_panel.py  <--CAN-->  cubemars_emulator.py  --UDP-->  cubemars_viewer.py

The panel sends the commands of the manual (servo and MIT) as extended CAN frames and shows the last status frame (0x29) it
receives back. What the motor is doing is shown by cubemars_viewer.py. Needs python-can and tkinter. It uses the encoders of
cubemars_emulator.py (same folder), so the frames are the same as in the manual.

The panel and the emulator are two programs on the same CAN bus: with a Vector VN1610 use the two channels wired together
(emulator on channel 0, panel on channel 1), or the same channel if your Vector configuration allows two applications on it.

  python cubemars_panel.py --interface vector --channel 1 --app-name CANalyzer --id 0x68
  python cubemars_panel.py --interface virtual --channel test        (with the emulator on the same virtual channel, same process only)

MIT mode needs the command to be re-sent continuously (the law is evaluated with the last command): use "repeat" in the MIT tab.
Nothing is printed on the console. The log window of the panel is optional (check "log tx").
"""

import argparse
import struct
import sys
import threading
import time

import can

import cubemars_emulator as ce


# ---------------------------------------------------------------- encoders (no GUI): (packet id, 8 or fewer bytes)
def _chk(name, x, lo, hi):
    if not lo <= x <= hi:
        raise ValueError(f"{name} {x:g} out of range [{lo:g}, {hi:g}]")
    return x


def enc_duty(duty):
    return ce.P_DUTY, struct.pack(">i", round(_chk("duty", duty, -1.0, 1.0) * 100000.0))


def enc_current(amps):
    return ce.P_CURRENT, struct.pack(">i", round(_chk("current [A]", amps, -60.0, 60.0) * 1000.0))


def enc_brake(amps):
    return ce.P_BRAKE, struct.pack(">i", round(_chk("brake current [A]", amps, 0.0, 60.0) * 1000.0))


def enc_rpm(erpm):
    return ce.P_RPM, struct.pack(">i", round(_chk("speed [ERPM]", erpm, -100000.0, 100000.0)))


def enc_pos(deg):
    return ce.P_POS, struct.pack(">i", round(_chk("position [deg]", deg, -36000.0, 36000.0) * 10000.0))


def enc_origin(permanent):
    return ce.P_ORIGIN, bytes([1 if permanent else 0])


def enc_posspd(deg, erpm, accel):
    return ce.P_POSSPD, (struct.pack(">i", round(_chk("position [deg]", deg, -36000.0, 36000.0) * 10000.0))
                         + struct.pack(">h", round(_chk("speed [ERPM]", erpm, -327680.0, 327670.0) / 10.0))
                         + struct.pack(">h", round(_chk("acceleration [ERPM/s^2]", accel, 0.0, 327670.0) / 10.0)))


def enc_mit(kp, kd, pos, vel, torque):
    return ce.P_MIT, ce.pack_mit(_chk("Kp", kp, 0.0, ce.KP_MAX), _chk("Kd", kd, 0.0, ce.KD_MAX), _chk("position [rad]", pos, -ce.PMAX, ce.PMAX),
                                 _chk("speed [rad/s]", vel, -ce.VMAX, ce.VMAX), _chk("torque [N.m]", torque, -ce.TMAX, ce.TMAX))


def enc_disable():
    return ce.P_DISABLE, b""


def enc_framecfg(pos32, single_turn):
    flags = (0x8000 if pos32 else 0) | (0x4000 if single_turn else 0)
    return ce.P_FRAMECFG, bytes(6) + struct.pack(">H", flags)


def dec_status(data):
    """0x29 payload -> (pos deg, speed ERPM, current A, temp, error)."""
    p, s, c = struct.unpack(">hhh", bytes(data[:6]))
    return p / 10.0, s * 10, c / 100.0, data[6], data[7]


# ---------------------------------------------------------------- the CAN side
class Link:
    def __init__(self, bus, devid):
        self.bus, self.devid = bus, devid
        self.lock = threading.Lock()
        self.status = None                      # (t, decoded) of the last 0x29
        self.status_count = 0
        self.pos32 = None
        self.start_frames = 0
        self.flag77 = 0
        self.tx_count = 0
        self.running = True
        self.log = None                         # callable(str) or None
        self.thread = threading.Thread(target=self._rx, daemon=True)
        self.thread.start()

    def send(self, packet, data):
        msg = can.Message(arbitration_id=(packet << 8) | self.devid, data=bytes(data), is_extended_id=True)
        self.bus.send(msg)
        self.tx_count += 1
        if self.log:
            self.log(f"tx {ce.PNAMES.get(packet, packet)} id=0x{msg.arbitration_id:08X} data={bytes(data).hex(' ')}")

    def _rx(self):
        while self.running:
            try:
                m = self.bus.recv(0.1)
            except can.CanError:
                continue
            if m is None or not m.is_extended_id or (m.arbitration_id & 0xFF) != self.devid:
                continue
            fid = (m.arbitration_id >> 8) & 0x1FFFFF
            with self.lock:
                if fid == ce.F_STATUS and len(m.data) == 8:
                    self.status = (time.time(), dec_status(m.data))
                    self.status_count += 1
                    if m.data[7] == 0x77:
                        self.flag77 += 1
                elif fid == ce.F_POS32 and len(m.data) >= 4:
                    self.pos32 = struct.unpack(">i", bytes(m.data[:4]))[0] / 10000.0
                elif fid == ce.F_START:
                    self.start_frames += 1
            if self.log and fid in (ce.F_START,):
                self.log(f"rx start frame {bytes(m.data).hex(' ')}")

    def close(self):
        self.running = False
        self.thread.join(0.5)


# ---------------------------------------------------------------- the GUI
def run_gui(link, devid, on_ready=None, geometry="880x660"):
    import tkinter as tk
    from tkinter import ttk

    root = tk.Tk()
    root.title(f"CubeMars panel - driver id 0x{devid:02X}")
    root.geometry(geometry)

    status = tk.StringVar(value="ready")
    fb = tk.StringVar(value="no feedback yet")

    def do(enc, *args):
        try:
            link.send(*enc(*args))
            status.set("sent: " + ce.PNAMES[enc(*args)[0]])
        except ValueError as e:
            status.set(f"NOT sent: {e}")
        except can.CanError as e:
            status.set(f"CAN error: {e}")

    def entry_row(parent, row, label, default, unit, enc, extra=()):
        ttk.Label(parent, text=label, width=22).grid(row=row, column=0, sticky="w", pady=3)
        vars_ = []
        for i, d in enumerate([default] + list(extra)):
            v = tk.StringVar(value=str(d))
            ttk.Entry(parent, textvariable=v, width=10).grid(row=row, column=1 + i, padx=2)
            vars_.append(v)
        ttk.Label(parent, text=unit, width=32).grid(row=row, column=4, sticky="w")

        def go():
            try:
                vals = [float(v.get()) for v in vars_]
            except ValueError:
                status.set("NOT sent: not a number")
                return
            do(enc, *vals)
        ttk.Button(parent, text="Send", width=8, command=go).grid(row=row, column=5, padx=4)
        return go

    nb = ttk.Notebook(root)
    nb.pack(fill="both", expand=True, padx=8, pady=8)

    # ---- servo
    sv = ttk.Frame(nb, padding=10)
    nb.add(sv, text="Servo")
    buttons = {}
    buttons["duty"] = entry_row(sv, 0, "duty cycle", 0.1, "-1 .. 1 (approximated in the emulator)", enc_duty)
    buttons["current"] = entry_row(sv, 1, "current loop", 2.0, "A, +-60", enc_current)
    buttons["brake"] = entry_row(sv, 2, "current brake", 2.0, "A, 0..60 (holds the position)", enc_brake)
    buttons["rpm"] = entry_row(sv, 3, "velocity", 5000, "ERPM electrical (/126 = output rpm)", enc_rpm)
    buttons["pos"] = entry_row(sv, 4, "position", 90.0, "deg, +-36000 (max speed/accel)", enc_pos)
    buttons["posspd"] = entry_row(sv, 5, "position-velocity", 180.0, "deg | ERPM | ERPM/s^2", enc_posspd, extra=(5000, 30000))
    ttk.Label(sv, text="set origin", width=22).grid(row=6, column=0, sticky="w", pady=3)
    ttk.Button(sv, text="temporary", command=lambda: do(enc_origin, False)).grid(row=6, column=1, columnspan=2, sticky="we", padx=2)
    ttk.Button(sv, text="permanent", command=lambda: do(enc_origin, True)).grid(row=6, column=3, columnspan=2, sticky="we", padx=2)
    ttk.Separator(sv).grid(row=7, column=0, columnspan=6, sticky="we", pady=8)
    pos32, single = tk.BooleanVar(value=False), tk.BooleanVar(value=False)
    ttk.Label(sv, text="feedback config", width=22).grid(row=8, column=0, sticky="w")
    ttk.Checkbutton(sv, text="add 0x2A (int32 position)", variable=pos32).grid(row=8, column=1, columnspan=3, sticky="w")
    ttk.Checkbutton(sv, text="single turn (0..360)", variable=single).grid(row=9, column=1, columnspan=3, sticky="w")
    ttk.Button(sv, text="Send", width=8, command=lambda: do(enc_framecfg, pos32.get(), single.get())).grid(row=8, column=5)
    ttk.Label(sv, text="(writes the flash of a real motor: do not send often)", foreground="#a05000").grid(row=10, column=1, columnspan=4, sticky="w")

    # ---- MIT
    mt = ttk.Frame(nb, padding=10)
    nb.add(mt, text="MIT")
    mit = {}
    specs = [("Kp", 0.0, ce.KP_MAX, 0.0, "N.m/rad"), ("Kd", 0.0, ce.KD_MAX, 0.0, "N.m.s/rad"),
             ("position", -ce.PMAX, ce.PMAX, 0.0, "rad"), ("speed", -ce.VMAX, ce.VMAX, 0.0, "rad/s"),
             ("torque (ff)", -ce.TMAX, ce.TMAX, 0.0, "N.m")]
    for i, (name, lo, hi, d, unit) in enumerate(specs):
        v = tk.DoubleVar(value=d)
        ttk.Label(mt, text=name, width=12).grid(row=i, column=0, sticky="w", pady=4)
        ttk.Scale(mt, from_=lo, to=hi, variable=v, length=360).grid(row=i, column=1, padx=6)
        e = ttk.Entry(mt, width=9)
        e.grid(row=i, column=2)
        ttk.Label(mt, text=unit, width=10).grid(row=i, column=3, sticky="w")
        mit[name] = v

        def sync(v=v, e=e):
            e.delete(0, "end")
            e.insert(0, f"{v.get():.3f}")
        v.trace_add("write", lambda *_a, sync=sync: sync())
        sync()

        def typed(_ev=None, v=v, e=e):
            try:
                v.set(float(e.get()))
            except ValueError:
                pass
        e.bind("<Return>", typed)

    def mit_vals():
        return [mit[n[0]].get() for n in specs]

    def send_mit():
        do(enc_mit, *mit_vals())

    def preset(kp, kd, p, v, t):
        for (n, *_), x in zip(specs, (kp, kd, p, v, t)):
            mit[n].set(x)

    bar = ttk.Frame(mt)
    bar.grid(row=6, column=0, columnspan=4, sticky="w", pady=10)
    ttk.Button(bar, text="Send once", command=send_mit).pack(side="left", padx=2)
    rep, rate = tk.BooleanVar(value=False), tk.StringVar(value="50")
    ttk.Checkbutton(bar, text="repeat at", variable=rep).pack(side="left", padx=(14, 2))
    ttk.Entry(bar, textvariable=rate, width=5).pack(side="left")
    ttk.Label(bar, text="Hz").pack(side="left")
    pre = ttk.Frame(mt)
    pre.grid(row=7, column=0, columnspan=4, sticky="w")
    ttk.Label(pre, text="presets:").pack(side="left")
    ttk.Button(pre, text="free (all 0)", command=lambda: preset(0, 0, 0, 0, 0)).pack(side="left", padx=2)
    ttk.Button(pre, text="hold here: Kp 20 Kd 1.5", command=lambda: preset(20, 1.5, 0, 0, 0)).pack(side="left", padx=2)
    ttk.Button(pre, text="speed 6 rad/s: Kd 2", command=lambda: preset(0, 2, 0, 6, 0)).pack(side="left", padx=2)
    ttk.Button(pre, text="torque 2 N.m", command=lambda: preset(0, 0, 0, 0, 2)).pack(side="left", padx=2)
    ttk.Label(mt, text="Kp and Kd are zero at start: with Kp = Kd = 0 only the feed-forward torque acts.\n"
                       "The command must be sent continuously: tick \"repeat\".", foreground="#555").grid(row=8, column=0, columnspan=4, sticky="w", pady=8)

    def mit_tick():
        if rep.get():
            send_mit()
        try:
            hz = max(1.0, min(1000.0, float(rate.get())))
        except ValueError:
            hz = 50.0
        root.after(int(1000.0 / hz), mit_tick)
    mit_tick()

    # ---- common
    low = ttk.Frame(root, padding=(8, 0, 8, 8))
    low.pack(fill="x")
    ttk.Button(low, text="DISABLE MOTOR", command=lambda: (rep.set(False), do(enc_disable))).pack(side="left")
    ttk.Label(low, textvariable=status).pack(side="left", padx=14)
    fbl = ttk.LabelFrame(root, text="last status frame received (0x29)", padding=6)
    fbl.pack(fill="x", padx=8, pady=(0, 6))
    ttk.Label(fbl, textvariable=fb, font=("Courier", 9), wraplength=860, justify="left").pack(anchor="w")
    logw = tk.Text(root, height=7, state="disabled", font=("Courier", 9))
    logon = tk.BooleanVar(value=False)
    ttk.Checkbutton(root, text="log tx (and the start frames)", variable=logon).pack(anchor="w", padx=8)
    logw.pack(fill="x", padx=8, pady=(0, 8))

    def log(line):
        if logon.get():
            root.after(0, lambda: (logw.configure(state="normal"), logw.insert("end", line + "\n"), logw.see("end"),
                                   logw.configure(state="disabled")))
    link.log = log

    def poll():
        with link.lock:
            st, n, s77, sf = link.status, link.status_count, link.flag77, link.start_frames
            p32 = link.pos32
        if st is None:
            fb.set("no feedback yet" + (f"   start frames: {sf}" if sf else ""))
        else:
            t, (p, s, c, tmp, err) = st
            fb.set(f"pos {p:9.1f} deg   speed {s:8d} ERPM   I {c:7.2f} A   T {tmp:4d} C   err {err}   "
                   f"[{n} frames, last {time.time() - t:4.1f} s ago]"
                   + (f"\npos32 {p32:.4f} deg" if p32 is not None else "") + (f"   disable ack: {s77}" if s77 else ""))
        root.after(100, poll)
    poll()

    root.protocol("WM_DELETE_WINDOW", root.destroy)
    if on_ready is not None:                    # for the tests: called once the window is built
        ctx = {"buttons": buttons, "mit": mit, "repeat": rep, "rate": rate, "status": status, "fb": fb, "send_mit": send_mit,
               "preset": preset, "disable": lambda: (rep.set(False), do(enc_disable))}
        root.after(300, lambda: on_ready(root, ctx))
    try:
        root.mainloop()
    except KeyboardInterrupt:
        pass
    return root


def main(argv=None):
    ap = argparse.ArgumentParser(description="CubeMars command panel (CAN master)")
    ap.add_argument("--interface", default="vector", help="python-can interface: vector, virtual, socketcan, pcan ...")
    ap.add_argument("--channel", default="1", help="vector: the application channel (the emulator uses another one, wired to it)")
    ap.add_argument("--app-name", default="CANalyzer", help="vector: application name in Vector Hardware Config; 'none' = global index")
    ap.add_argument("--bitrate", type=int, default=1000000)
    ap.add_argument("--id", type=lambda s: int(s, 0), default=ce.DEFAULT_ID, help=f"driver id [0, 255], default 0x{ce.DEFAULT_ID:02X}")
    a = ap.parse_args(argv)
    if not 0 <= a.id <= 0xFF:
        ap.error("--id must be in [0, 255]")
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
    link = Link(bus, a.id)
    run_gui(link, a.id)
    link.close()
    bus.shutdown()
    return 0


if __name__ == "__main__":
    sys.exit(main())

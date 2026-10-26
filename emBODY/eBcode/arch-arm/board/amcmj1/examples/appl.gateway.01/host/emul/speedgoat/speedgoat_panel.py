#!/usr/bin/env python3
"""
A small control panel (Tk) for whoever plays the Speedgoat: speedgoat_emulator.py --daemon on the PC, or the Simulink model on the real Speedgoat.
It talks UDP with the two small packets of sgctl.py (CTL to port 6000, STATUS back to port 6001). Choose the test, the device(s) and the numbers,
press Start: the other side enables the devices and plays the profile, Stop disables them.

    py speedgoat_emulator.py --daemon                      (the PC plays the Speedgoat)       py speedgoat_panel.py
    py speedgoat_panel.py --host 10.0.2.150                (the Speedgoat runs the Simulink model with the control link)

The panel sends the CTL every 200 ms: if it dies or the network breaks, the other side stops by itself (1 s). The numbers can be changed while it runs.
"""

import argparse
import os
import sys
import time
import tkinter as tk
from collections import deque
from tkinter import ttk

_HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, _HERE)
import sgctl  # noqa: E402

# mode -> (what "value" is, the entries that matter, the defaults value kp kd freq tparam, a line of help)
TESTS = {
    "ramp":     ("target velocity [rad/s]", ("value", "kd", "tparam"), (2.0, 0.0, 1.0, 0.5, 5.0), "velocity from 0 to value in 'time' s, then it holds"),
    "step":     ("velocity [rad/s]", ("value", "kd", "tparam"), (1.0, 0.0, 1.0, 0.5, 3.0), "velocity +value, -value, +value ... the sign changes every 'time' s"),
    "sine":     ("amplitude [rad/s]", ("value", "kd", "freq"), (2.0, 0.0, 1.0, 0.5, 3.0), "velocity = value sin(2 pi freq t)"),
    "velocity": ("velocity [rad/s]", ("value", "kd"), (1.0, 0.0, 1.0, 0.5, 3.0), "constant velocity (kd brakes: value 0 holds still)"),
    "position": ("position [rad]", ("value", "kp", "kd"), (0.5, 5.0, 0.5, 0.5, 3.0), "position setpoint, kp kd > 0 (+-12.56 rad)"),
    "torque":   ("torque [N*m]", ("value",), (0.05, 0.0, 0.0, 0.5, 3.0), "constant torque, kp kd 0: the motor accelerates, keep it short"),
    "zero":     ("-", (), (0.0, 0.0, 0.0, 0.5, 3.0), "kp kd torque 0: the motor is free"),
    "idle":     ("-", (), (0.0, 0.0, 0.0, 0.5, 3.0), "heartbeats only, no setpoint (e.g. to look at the Paxini)"),
}
FIELDS = (("value", "value"), ("kp", "kp"), ("kd", "kd"), ("freq", "freq [Hz]"), ("tparam", "time [s]"), ("duration", "duration [s] (0 = until Stop)"))
RUN_COLOR = {0: "#555555", 1: "#b8860b", 2: "#1a7f37", 3: "#b8860b"}


class Trace(tk.Canvas):
    """a strip chart of the last seconds, one or more series on the same scale"""

    def __init__(self, master, title, colors, labels=None, seconds=15.0, **kw):
        super().__init__(master, height=70, bg="white", highlightthickness=1, highlightbackground="#bbbbbb", **kw)
        self.title, self.seconds, self.data = title, seconds, deque()
        self.colors = [colors] if isinstance(colors, str) else list(colors)
        self.labels = labels or [""] * len(self.colors)

    def add(self, t, *values):
        self.data.append((t, values))
        while self.data and t - self.data[0][0] > self.seconds:
            self.data.popleft()

    def clear(self):
        self.data.clear()

    def draw(self):
        self.delete("all")
        w, h = max(self.winfo_width(), 50), max(self.winfo_height(), 50)
        self.create_text(6, 4, anchor="nw", text=self.title, fill="#444444", font=("TkDefaultFont", 8))
        if len(self.data) < 2:
            return
        t1 = self.data[-1][0]
        vs = [v for _, vals in self.data for v in vals]
        lo, hi = min(vs), max(vs)
        if hi - lo < 1e-3:
            lo, hi = lo - 0.5, hi + 0.5
        pad = 0.1 * (hi - lo)
        lo, hi = lo - pad, hi + pad
        for k, color in enumerate(self.colors):
            pts = []
            for t, vals in self.data:
                pts += [w - (t1 - t) / self.seconds * (w - 50) - 4, h - 6 - (vals[k] - lo) / (hi - lo) * (h - 20)]
            self.create_line(*pts, fill=color, width=2)
        last = self.data[-1][1]
        txt = "   ".join(f"{self.labels[k]} {last[k]:+.3f}".strip() for k in range(len(self.colors)))
        self.create_text(w - 4, 4, anchor="ne", text=f"{txt}   [{lo:+.2f} .. {hi:+.2f}]", fill="#333333", font=("TkDefaultFont", 8))


class Panel:
    def __init__(self, root, client):
        self.root, self.client = root, client
        self.running = False                    # what the panel asks (the Start / Stop buttons)
        self.started_at = 0.0
        root.title("Speedgoat panel - amcmj1")
        root.protocol("WM_DELETE_WINDOW", self.close)
        top = ttk.Frame(root, padding=8)
        top.pack(fill="both", expand=True)

        con = ttk.LabelFrame(top, text="link", padding=6)
        con.grid(row=0, column=0, columnspan=2, sticky="ew")
        ttk.Label(con, text="Speedgoat / daemon IP").grid(row=0, column=0)
        self.host = tk.StringVar(value=client.dest[0])
        e = ttk.Entry(con, textvariable=self.host, width=16)
        e.grid(row=0, column=1, padx=4)
        e.bind("<Return>", lambda _e: self.apply_host())
        ttk.Button(con, text="apply", command=self.apply_host).grid(row=0, column=2)
        self.linkv = tk.StringVar(value="no status yet")
        self.linklabel = ttk.Label(con, textvariable=self.linkv)
        self.linklabel.grid(row=0, column=3, padx=12)

        tst = ttk.LabelFrame(top, text="test", padding=6)
        tst.grid(row=1, column=0, sticky="nsew", pady=6)
        ttk.Label(tst, text="test").grid(row=0, column=0, sticky="w")
        self.mode = tk.StringVar(value="ramp")
        self.combo = ttk.Combobox(tst, textvariable=self.mode, values=list(TESTS), state="readonly", width=12)
        self.combo.grid(row=0, column=1, sticky="w", padx=4)
        self.combo.bind("<<ComboboxSelected>>", lambda _e: self.on_mode(True))
        self.helpv = tk.StringVar()
        ttk.Label(tst, textvariable=self.helpv, foreground="#555555", wraplength=330).grid(row=1, column=0, columnspan=2, sticky="w", pady=2)
        ttk.Label(tst, text="device").grid(row=2, column=0, sticky="w")
        self.dev = tk.StringVar(value="cube")
        df = ttk.Frame(tst)
        df.grid(row=2, column=1, sticky="w")
        self.devbuttons = [ttk.Radiobutton(df, text=d, value=d, variable=self.dev, command=self.send) for d in sgctl.DEVS]
        for b in self.devbuttons:
            b.pack(side="left", padx=2)
        self.vars, self.entries, self.labels = {}, {}, {}
        for i, (key, text) in enumerate(FIELDS):
            self.labels[key] = ttk.Label(tst, text=text)
            self.labels[key].grid(row=3 + i, column=0, sticky="w")
            self.vars[key] = tk.StringVar()
            self.entries[key] = ttk.Entry(tst, textvariable=self.vars[key], width=10)
            self.entries[key].grid(row=3 + i, column=1, sticky="w", padx=4, pady=1)
            self.entries[key].bind("<Return>", lambda _e: self.send())
            self.entries[key].bind("<FocusOut>", lambda _e: self.send())
        self.vars["duration"].set("0")
        bf = ttk.Frame(tst)
        bf.grid(row=10, column=0, columnspan=2, pady=8, sticky="ew")
        self.start = tk.Button(bf, text="START", bg="#2e9e4f", fg="white", font=("TkDefaultFont", 11, "bold"), width=10, command=self.do_start)
        self.stop = tk.Button(bf, text="STOP", bg="#c0392b", fg="white", font=("TkDefaultFont", 11, "bold"), width=10, command=self.do_stop)
        self.start.pack(side="left", padx=4)
        self.stop.pack(side="left", padx=4)
        self.msgv = tk.StringVar()
        ttk.Label(tst, textvariable=self.msgv, foreground="#b03030", wraplength=330).grid(row=11, column=0, columnspan=2, sticky="w")

        st = ttk.LabelFrame(top, text="status", padding=6)
        st.grid(row=1, column=1, sticky="nsew", padx=8, pady=6)
        self.statev = tk.StringVar(value="-")
        self.statelabel = tk.Label(st, textvariable=self.statev, font=("TkDefaultFont", 14, "bold"), fg=RUN_COLOR[0])
        self.statelabel.grid(row=0, column=0, columnspan=2, sticky="w")
        self.rows = {}
        for i, key in enumerate(("time", "CubeMars", "position", "velocity", "current", "Paxini Mx My Mz", "frames sent / state back", "lost / clamped")):
            ttk.Label(st, text=key).grid(row=1 + i, column=0, sticky="w", padx=(0, 10))
            self.rows[key] = tk.StringVar(value="-")
            ttk.Label(st, textvariable=self.rows[key], font=("TkFixedFont", 10)).grid(row=1 + i, column=1, sticky="w")
        self.errv = tk.StringVar()
        tk.Label(st, textvariable=self.errv, fg="#c00000", wraplength=330, justify="left", font=("TkDefaultFont", 10, "bold")).grid(row=10, column=0, columnspan=2, sticky="w", pady=(8, 0))
        self.tv = Trace(top, "velocity [rad/s]", "#1f5fbf")
        self.tp = Trace(top, "position [rad]", "#b5651d")
        self.tmx = Trace(top, "Paxini Mx [N*m]", "#d62728")
        self.tmy = Trace(top, "Paxini My [N*m]", "#2ca02c")
        self.tmz = Trace(top, "Paxini Mz [N*m]", "#8a2be2")
        self.traces = (self.tv, self.tp, self.tmx, self.tmy, self.tmz)
        for r, tr in enumerate(self.traces):
            tr.grid(row=2 + r, column=0, columnspan=2, sticky="ew", pady=2)
        top.columnconfigure(0, weight=1)
        top.columnconfigure(1, weight=1)
        self.on_mode(True)
        self.tick()

    # ---- the entries
    def on_mode(self, defaults=False):
        label, used, d, help_ = TESTS[self.mode.get()]
        if defaults:
            for k, v in zip(("value", "kp", "kd", "freq", "tparam"), d):
                self.vars[k].set(f"{v:g}")
        self.helpv.set(help_)
        self.labels["value"].config(text=label)
        for k in ("value", "kp", "kd", "freq", "tparam"):
            self.entries[k].config(state="normal" if k in used else "disabled")
        self.send()

    def numbers(self):
        out, bad = {}, []
        for k, _ in FIELDS:
            try:
                out[k] = float(self.vars[k].get().replace(",", "."))
            except ValueError:
                bad.append(k)
        return out, bad

    def send(self):
        nums, bad = self.numbers()
        for k, _ in FIELDS:
            self.entries[k].config(foreground="#c00000" if k in bad else "black")
        if bad:
            self.msgv.set("not a number: " + ", ".join(bad) + " (the last valid values stay)")
            return False
        self.msgv.set("")
        used = TESTS[self.mode.get()][1]
        for k in ("value", "kp", "kd", "freq", "tparam"):
            if k not in used:
                nums[k] = 0.0
        self.client.set(run=int(self.running), mode=sgctl.MODES[self.mode.get()], dev=sgctl.DEVS[self.dev.get()], **nums)
        return True

    # ---- the buttons
    def do_start(self):
        self.running, self.started_at = True, time.monotonic()
        for tr in self.traces:
            tr.clear()
        if not self.send():
            self.running = False
            self.send()

    def do_stop(self):
        self.running = False
        self.send()

    def apply_host(self):
        self.client.set_host(self.host.get().strip() or "127.0.0.1")

    def close(self):
        self.client.close(stop=True)
        self.root.destroy()

    # ---- the status
    def tick(self):
        st, age = self.client.status()
        if st is None or age is None or age > 1.0:
            self.linkv.set("NO STATUS: is the daemon / the Speedgoat running? (IP, UDP 6000 / 6001, firewall)")
            self.statev.set("no link")
            self.statelabel.config(fg="#b03030")
        else:
            self.linkv.set(f"status ok ({age * 1000:.0f} ms ago)")
            name = sgctl.STATE_NAMES.get(st.state, "?")
            self.statev.set(f"{name}  ({sgctl.MODE_NAMES.get(st.mode, '?')}, {sgctl.DEV_NAMES.get(st.dev, '?')})" if st.state else "idle")
            self.statelabel.config(fg=RUN_COLOR.get(st.state, "black"))
            r = self.rows
            r["time"].set(f"{st.t:7.2f} s")
            r["CubeMars"].set(sgctl.CUBE_NAMES.get(st.cube, "?"))
            r["position"].set(f"{st.pos:+9.4f} rad")
            r["velocity"].set(f"{st.vel:+9.4f} rad/s")
            r["current"].set(f"{st.current:+9.3f} A")
            r["Paxini Mx My Mz"].set(f"{st.mx:+.3f} {st.my:+.3f} {st.mz:+.3f}")
            r["frames sent / state back"].set(f"{st.sent} / {st.received}")
            r["lost / clamped"].set(f"{st.lost} / {st.clamped}")
            now = time.monotonic()
            self.tv.add(now, st.vel)
            self.tp.add(now, st.pos)
            self.tmx.add(now, st.mx)
            self.tmy.add(now, st.my)
            self.tmz.add(now, st.mz)
            self.errv.set(("ERROR: " + sgctl.ERR_NAMES.get(st.err, str(st.err)) + "\n(the reason is in the console of the daemon / of the Speedgoat)") if st.err else "")
            # the other side stopped on its own (duration expired, enable failed, keepalive): the buttons follow it
            if self.running and st.state == 0 and time.monotonic() - self.started_at > 1.5:
                self.running = False
                self.send()
        self.start.config(state="disabled" if self.running else "normal")
        self.stop.config(state="normal" if self.running else "disabled")
        for b in self.devbuttons:
            b.config(state="disabled" if self.running else "normal")
        for tr in self.traces:
            tr.draw()
        self.root.after(100, self.tick)


def main():
    ap = argparse.ArgumentParser(description="control panel for the Speedgoat emulator (--daemon) or for the Speedgoat model: see the top of this file")
    ap.add_argument("--host", default="127.0.0.1", help="IP of the PC with speedgoat_emulator.py --daemon, or of the Speedgoat")
    ap.add_argument("--ctl-port", type=int, default=sgctl.PORT_CTL)
    ap.add_argument("--status-port", type=int, default=sgctl.PORT_STATUS, help="where this panel listens")
    args = ap.parse_args()
    try:
        client = sgctl.Client(args.host, args.ctl_port, args.status_port)
    except OSError as e:
        print(e)
        return 1
    root = tk.Tk()
    Panel(root, client)
    root.mainloop()


if __name__ == "__main__":
    main()

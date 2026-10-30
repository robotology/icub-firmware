#!/usr/bin/env python3
"""
The control panel (Tk) of sgact: the Simulink model on the Speedgoat, or sgact_daemon.py on the PC.
The window is built from sgact_panel.toml (sgact_spec.py): the devices, the modes and their parameters, the plots.
It sends requests (sgctl.py, version 4), each one independent:

    devices:  [x] motor_cubemars [x] sensor_torque [ ] motor_macro [x] sensor_position     INIT  DEINIT
              motor_macro config: pids, current limits, motor config (from the TOML, it can be changed before INIT)
    mode:     motor_cubemars_position  pos [deg] 30  vel [deg/s] 0  kp 5  kd 0.5  torque [N*m] 0   duration [s] 0   START  STOP

and it shows the state of the supervisor, the result of the last request and the feedback.
EXTFAULT is sticky: pressed, sgact stops all the motors at once (motor_cubemars disabled, motor_macro idle) and refuses every
request; pressed again, it goes idle. It is in every CTL (flags bit 0): the keepalive keeps it, a new panel starts released.

    py sgact_panel.py --host 10.0.2.150                (the Speedgoat runs sgact)
    py sgact_daemon.py                      then   py sgact_panel.py   (the PC plays sgact)
    py sgact_panel.py --spec ..\\sgact_panel.toml

The panel sends a CTL every 200 ms: if it dies or the network breaks, the running mode stops by itself (keepalive).
The parameters of the running mode can be changed while it runs (Enter or leaving the field): they apply at once.
The parameters are in the units of the TOML (deg, deg/s, ...); the angles of the feedback as in its [display] angles.
"""

import argparse
import os
import sys
import time
import tkinter as tk
from collections import deque
from tkinter import ttk

_HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(_HERE)), "shared"))     # host/shared
import sgact_spec  # noqa: E402
import sgctl  # noqa: E402

STATE_COLOR = {0: "#555555", 1: "#b8860b", 2: "#b8860b", 3: "#1a7f37", 4: "#b8860b", 5: "#c00000"}
SERIES_COLORS = ("#1f5fbf", "#d62728", "#2ca02c", "#8a2be2", "#b5651d", "#17becf")


class Trace(tk.Canvas):
    """a strip chart of the last seconds, one or more series on the same scale"""

    def __init__(self, master, title, labels, seconds=15.0, **kw):
        super().__init__(master, height=70, bg="white", highlightthickness=1, highlightbackground="#bbbbbb", **kw)
        self.title, self.labels, self.seconds, self.data = title, list(labels), seconds, deque()
        self.colors = [SERIES_COLORS[k % len(SERIES_COLORS)] for k in range(len(self.labels))]

    def add(self, t, values):
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
        txt = "   ".join(f"{self.labels[k]} {last[k]:+.3f}" for k in range(len(self.labels)))
        self.create_text(w - 4, 4, anchor="ne", text=f"{txt}   [{lo:+.2f} .. {hi:+.2f}]", fill="#333333", font=("TkDefaultFont", 8))


class Tooltip:
    """a small window w/ a text, shown while the mouse is on the widget"""

    def __init__(self, widget, text):
        self.widget, self.text, self.tip = widget, text, None
        if text:
            widget.bind("<Enter>", self.show)
            widget.bind("<Leave>", self.hide)

    def show(self, _e=None):
        if self.tip is not None:
            return
        x, y = self.widget.winfo_rootx() + 20, self.widget.winfo_rooty() + self.widget.winfo_height() + 2
        self.tip = tk.Toplevel(self.widget)
        self.tip.wm_overrideredirect(True)
        self.tip.wm_geometry(f"+{x}+{y}")
        tk.Label(self.tip, text=self.text, background="#ffffe0", relief="solid", borderwidth=1, wraplength=360,
                 justify="left").pack()

    def hide(self, _e=None):
        if self.tip is not None:
            self.tip.destroy()
            self.tip = None


class Panel:
    def __init__(self, root, client, spec):
        self.root, self.client, self.spec = root, client, spec
        self.mode_names = [m.name for m in spec.modes]
        self.lastid = None                                  # the id of the last request sent
        root.title(f"sgact panel - {os.path.basename(spec.path)} (crc 0x{spec.crc:08X})")
        root.protocol("WM_DELETE_WINDOW", self.close)
        top = ttk.Frame(root, padding=8)
        top.pack(fill="both", expand=True)

        # ---- row 0: link | EXTFAULT
        con = ttk.LabelFrame(top, text="link", padding=6)
        con.grid(row=0, column=0, sticky="nsew")
        ttk.Label(con, text="sgact IP").grid(row=0, column=0)
        self.host = tk.StringVar(value=client.dest[0])
        e = ttk.Entry(con, textvariable=self.host, width=16)
        e.grid(row=0, column=1, padx=4)
        e.bind("<Return>", lambda _e: self.apply_host())
        ttk.Button(con, text="apply", command=self.apply_host).grid(row=0, column=2)
        self.linkv = tk.StringVar(value="no status yet")
        self.linklabel = tk.Label(con, textvariable=self.linkv, wraplength=560, justify="left")
        self.linklabel.grid(row=0, column=3, padx=12, sticky="w")
        # EXTFAULT: sticky. pressed: sgact stops all the motors at once and amc.gtw holds it; every request is refused.
        # pressed again: idle
        ff = ttk.LabelFrame(top, text="EXTFAULT", padding=6)
        ff.grid(row=0, column=1, sticky="nsew", padx=(8, 0))
        self.extfault = False
        self.bfault = tk.Button(ff, text="EXTFAULT", bg="#8b0000", fg="white", activebackground="#ff2020",
                                font=("TkDefaultFont", 14, "bold"), relief="raised", bd=4, command=self.do_extfault)
        self.bfault.pack(fill="both", expand=True)

        # ---- row 1, left: devices, then mode | parameters
        left = ttk.Frame(top)
        left.grid(row=1, column=0, sticky="nsew", pady=6)
        dev = ttk.LabelFrame(left, text="devices", padding=6)
        dev.pack(fill="x")
        self.devvars, self.devstate = {}, {}
        for i, (name, bit) in enumerate(sorted(spec.devices.items(), key=lambda kv: kv[1])):
            self.devvars[name] = tk.BooleanVar(value=(name != "motor_macro"))
            cb = ttk.Checkbutton(dev, text=name, variable=self.devvars[name])
            cb.grid(row=i, column=0, sticky="w")
            self.devstate[name] = tk.Label(dev, text="-", width=12, anchor="w")
            self.devstate[name].grid(row=i, column=1, sticky="w")
            Tooltip(cb, spec.device_help.get(name, ""))         # the help of the TOML: on the mouse
        row = len(spec.devices)
        # the config of the motor_macro: from the TOML, sent in every CTL, taken by sgact at the init of the motor_macro.
        # folded: it is open only when one wants to see or change it
        self.mmvars, self.mmentries = [], []
        if "motor_macro" in spec.devices:
            self.cfopen = False
            self.cftoggle = ttk.Button(dev, text="motor_macro config \u25b8", command=self.toggle_mmcfg)
            self.cftoggle.grid(row=row, column=0, columnspan=2, sticky="w", pady=(4, 0))
            self.cf = cf = ttk.Frame(dev, padding=(4, 2))
            for k, name in enumerate(sgact_spec.MMCFG_NAMES):
                sec, key = name.split(".")
                short = {"current_pid": "cur", "velocity_pid": "vel", "current_limits": "I", "motor": ""}[sec]
                ttk.Label(cf, text=f"{short} {key}".strip()).grid(row=k // 5, column=2 * (k % 5), sticky="e", padx=(6, 1))
                v = tk.StringVar(value=f"{spec.mmcfg[k]:g}")
                e = ttk.Entry(cf, textvariable=v, width=7)
                e.grid(row=k // 5, column=2 * (k % 5) + 1, sticky="w")
                e.bind("<Return>", lambda _e: self.send_mmcfg())
                e.bind("<FocusOut>", lambda _e: self.send_mmcfg())
                self.mmvars.append(v)
                self.mmentries.append(e)
            ttk.Button(cf, text="TOML values", command=self.reset_mmcfg).grid(row=2, column=8, columnspan=2, sticky="e")
            self.cfrow = row + 1
        bf = ttk.Frame(dev)
        bf.grid(row=row + 2, column=0, columnspan=3, pady=(6, 0), sticky="w")
        self.binit = tk.Button(bf, text="INIT", bg="#2e6f9e", fg="white", width=9, command=self.do_init)
        self.bdeinit = tk.Button(bf, text="DEINIT", bg="#6b6b6b", fg="white", width=9, command=self.do_deinit)
        self.binit.pack(side="left", padx=4)
        self.bdeinit.pack(side="left", padx=4)

        mp = ttk.Frame(left)
        mp.pack(fill="both", expand=True, pady=(6, 0))
        mf = ttk.LabelFrame(mp, text="mode", padding=6)
        mf.grid(row=0, column=0, sticky="nsew")
        self.mode = tk.StringVar(value=self.mode_names[0])
        self.combo = ttk.Combobox(mf, textvariable=self.mode, values=self.mode_names, state="readonly",
                                  width=max(len(n) for n in self.mode_names) + 2)     # the whole name of every mode
        self.combo.grid(row=0, column=0, sticky="w")
        self.combo.bind("<<ComboboxSelected>>", lambda _e: self.on_mode())
        self.helpv = tk.StringVar()
        ttk.Label(mf, textvariable=self.helpv, foreground="#555555", wraplength=300, justify="left") \
            .grid(row=1, column=0, sticky="w", pady=2)
        df = ttk.Frame(mf)
        df.grid(row=2, column=0, sticky="w", pady=(4, 0))
        ttk.Label(df, text="duration [s] (0 = until stop)").pack(side="left")
        self.duration = tk.StringVar(value="0")
        de = ttk.Entry(df, textvariable=self.duration, width=8)
        de.pack(side="left", padx=4)
        de.bind("<Return>", lambda _e: self.send_live())
        de.bind("<FocusOut>", lambda _e: self.send_live())
        sf = ttk.Frame(mf)
        sf.grid(row=3, column=0, pady=(8, 0), sticky="w")
        self.bstart = tk.Button(sf, text="START", bg="#2e9e4f", fg="white", font=("TkDefaultFont", 11, "bold"), width=9, command=self.do_start)
        self.bstop = tk.Button(sf, text="STOP", bg="#c0392b", fg="white", font=("TkDefaultFont", 11, "bold"), width=9, command=self.do_stop)
        self.bstart.pack(side="left", padx=4)
        self.bstop.pack(side="left", padx=4)
        pf = ttk.LabelFrame(mp, text="parameters", padding=6)
        pf.grid(row=0, column=1, sticky="nsew", padx=(8, 0))
        self.pframe = ttk.Frame(pf)
        self.pframe.pack(fill="both", expand=True)
        self.pvars, self.pentries = [], []
        mp.columnconfigure(1, weight=1)
        self.msgv = tk.StringVar()
        ttk.Label(left, textvariable=self.msgv, foreground="#b03030", wraplength=560).pack(fill="x")

        # ---- row 1, right: status
        st = ttk.LabelFrame(top, text="status", padding=6)
        st.grid(row=1, column=1, sticky="nsew", padx=(8, 0), pady=6)
        self.statev = tk.StringVar(value="-")
        self.statelabel = tk.Label(st, textvariable=self.statev, font=("TkDefaultFont", 14, "bold"), fg=STATE_COLOR[0])
        self.statelabel.grid(row=0, column=0, columnspan=2, sticky="w")
        self.rows = {}
        self.af = spec.angle_factor()                       # rad -> the unit of the panel
        u = spec.angles
        self.k_cube = f"motor_cubemars pos [{u}] vel [{u}/s] I [A]"
        self.k_macro = f"motor_macro pos [{u}] vel [{u}/s] I [A] pwm [%]"
        self.k_pos = f"sensor_position pos [{u}]"
        keys = ("time", "last request", "motor_cubemars", self.k_cube, "sensor_torque Mx My Mz [N*m]", "motor_macro", self.k_macro,
                self.k_pos, "frames sent / state back", "lost / clamped")
        for i, key in enumerate(keys):
            ttk.Label(st, text=key).grid(row=1 + i, column=0, sticky="w", padx=(0, 10))
            self.rows[key] = tk.StringVar(value="-")
            ttk.Label(st, textvariable=self.rows[key], font=("TkFixedFont", 10)).grid(row=1 + i, column=1, sticky="w")
        self.errv = tk.StringVar()
        tk.Label(st, textvariable=self.errv, fg="#c00000", wraplength=420, justify="left", font=("TkDefaultFont", 10, "bold")) \
            .grid(row=1 + len(keys), column=0, columnspan=2, sticky="w", pady=(8, 0))

        # ---- row 2: the plots, in two columns
        pl = ttk.Frame(top)
        pl.grid(row=2, column=0, columnspan=2, sticky="nsew")
        self.traces = []
        for r, p in enumerate(spec.plots):
            tr = Trace(pl, p.title, [f.rsplit("_", 1)[1] for f in p.fields])
            tr.fields = p.fields
            tr.grid(row=r // 2, column=r % 2, sticky="ew", padx=(0 if r % 2 == 0 else 4, 0), pady=2)
            self.traces.append(tr)
        pl.columnconfigure(0, weight=1)
        pl.columnconfigure(1, weight=1)
        top.columnconfigure(0, weight=1)
        top.columnconfigure(1, weight=1)
        self.on_mode()
        self.send_mmcfg()
        self.tick()

    def toggle_mmcfg(self):
        """the config of the motor_macro: open / folded"""
        self.cfopen = not self.cfopen
        if self.cfopen:
            self.cf.grid(row=self.cfrow, column=0, columnspan=3, sticky="w")
        else:
            self.cf.grid_remove()
        self.cftoggle.config(text="motor_macro config " + ("\u25be" if self.cfopen else "\u25b8"))

    # ---- the config of the motor_macro
    def mmcfg(self):
        """the 14 numbers of the fields, or None (the wrong fields go red)"""
        vals, good = [], True
        for k, (v, e) in enumerate(zip(self.mmvars, self.mmentries)):
            _, _, kind, lo, hi = sgact_spec.MMCFG_LAYOUT[k]
            try:
                x = float(v.get().replace(",", "."))
                ok = (lo is None or x >= lo) and (hi is None or x <= hi) and (kind == "f" or x == int(x))
            except ValueError:
                x, ok = 0.0, False
            e.config(foreground="black" if ok else "#c00000")
            good = good and ok
            vals.append(x)
        return vals if good else None

    def send_mmcfg(self):
        if not self.mmvars:
            return
        v = self.mmcfg()
        if v is None:
            self.msgv.set("motor_macro config not valid: the last valid one is sent")
        else:
            self.client.live(mmcfg=v)

    def reset_mmcfg(self):
        for v, x in zip(self.mmvars, self.spec.mmcfg):
            v.set(f"{x:g}")
        self.send_mmcfg()

    # ---- the mode and its parameters
    def current_mode(self):
        return self.spec.mode_by_name[self.mode.get()]

    def on_mode(self):
        m = self.current_mode()
        self.helpv.set(m.help + ("   (Speedgoat only)" if m.speedgoat_only else ""))
        for w in self.pframe.winfo_children():
            w.destroy()
        self.pvars, self.pentries = [], []
        for k, p in enumerate(m.params):
            ttk.Label(self.pframe, text=f"{p.name} [{p.unit}]" if p.unit else p.name).grid(row=k, column=0, sticky="w")
            v = tk.StringVar(value=f"{p.default:g}")
            e = ttk.Entry(self.pframe, textvariable=v, width=10, state="readonly" if p.min == p.max else "normal")
            e.grid(row=k, column=1, sticky="w", padx=4, pady=1)      # min = max: shown, it cannot be changed
            e.bind("<Return>", lambda _e: self.send_live())
            e.bind("<FocusOut>", lambda _e: self.send_live())
            ttk.Label(self.pframe, text=f"[{p.min:g} .. {p.max:g}]", foreground="#777777").grid(row=k, column=2, sticky="w")
            self.pvars.append(v)
            self.pentries.append(e)
        if not m.params:
            ttk.Label(self.pframe, text="(no parameters)", foreground="#777777").grid(row=0, column=0, sticky="w")
        self.send_live()

    def numbers(self):
        """(params, duration) or None, with the message in the panel"""
        m = self.current_mode()
        vals, bad = [], []
        for p, v, e in zip(m.params, self.pvars, self.pentries):
            try:
                x = float(v.get().replace(",", "."))
                ok = p.min <= x <= p.max
            except ValueError:
                x, ok = 0.0, False
            e.config(foreground="black" if ok else "#c00000")
            vals.append(x)
            if not ok:
                bad.append(f"{p.name} ({p.min:g} .. {p.max:g})")
        try:
            duration = float(self.duration.get().replace(",", "."))
            if duration < 0:
                raise ValueError
        except ValueError:
            bad.append("duration (>= 0)")
            duration = 0.0
        if bad:
            self.msgv.set("not valid: " + ", ".join(bad))
            return None
        self.msgv.set("")
        return vals, duration

    def mask(self):
        return self.spec.mask([n for n, v in self.devvars.items() if v.get()])

    # ---- the requests
    def send_live(self):
        """mode, parameters and duration go with the next packets: sgact applies them if this mode is running"""
        nd = self.numbers()
        if nd is not None:
            self.client.live(mode=self.current_mode().id, params=nd[0], duration=nd[1])

    def do_init(self):
        self.send_mmcfg()
        self.lastid = self.client.request(self.spec.actions["init"], devices=self.mask())

    def do_deinit(self):
        self.lastid = self.client.request(self.spec.actions["deinit"], devices=self.mask())

    def do_start(self):
        nd = self.numbers()
        if nd is None:
            return
        for tr in self.traces:
            tr.clear()
        self.lastid = self.client.request(self.spec.actions["start"], mode=self.current_mode().id, params=nd[0], duration=nd[1])

    def do_extfault(self):
        """the EXTFAULT latches: it goes in every CTL until it is pressed again"""
        self.extfault = not self.extfault
        self.bfault.config(text="EXTFAULT PRESSED: all motors stopped\n(press again to release)" if self.extfault else "EXTFAULT",
                           bg="#ff2020" if self.extfault else "#8b0000", relief="sunken" if self.extfault else "raised")
        self.client.live(extfault=self.extfault)

    def do_stop(self):
        self.lastid = self.client.request(self.spec.actions["stop"])

    def apply_host(self):
        self.client.set_host(self.host.get().strip() or "127.0.0.1")

    def close(self):
        self.client.close()
        self.root.destroy()

    # ---- the status
    def tick(self):
        st, age = self.client.status()
        spec = self.spec
        buttons = (self.binit, self.bdeinit, self.bstart, self.bstop)
        if st is None or age is None or age > 1.0:
            other, oage = self.client.foreign()
            if other is not None and oage is not None and oage < 1.0:
                self.linkv.set(f"NO STATUS from {self.client.dest[0]}: the STATUS comes from {other}, which does not get our CTL. wrong IP?")
            else:
                self.linkv.set(f"NO STATUS from {self.client.dest[0]}: is sgact running? (IP, UDP 6000 / 6001, firewall)")
            self.linklabel.config(fg="#b03030")
            self.statev.set("no link")
            self.statelabel.config(fg="#b03030")
            for b in buttons:
                b.config(state="disabled")
        else:
            same = st.crc == spec.crc
            self.linkv.set(f"status ok ({age * 1000:.0f} ms)   sgact_panel.toml: " +
                           ("the same" if same else f"DIFFERENT (sgact 0x{st.crc:08X}, panel 0x{spec.crc:08X}): requests refused"))
            self.linklabel.config(fg="black" if same else "#c00000")
            mname = spec.mode_by_id[st.mode].name if st.mode in spec.mode_by_id else ""
            self.statev.set(f"{sgctl.STATE_NAMES.get(st.state, '?')}  {mname}")
            self.statelabel.config(fg=STATE_COLOR.get(st.state, "black"))
            for name, bit in spec.devices.items():
                on = bool(st.inited & (1 << bit))
                self.devstate[name].config(text="initialised" if on else "-", fg="#1a7f37" if on else "#777777")
            r = self.rows
            r["time"].set(f"{st.trun:7.2f} s")
            res = sgctl.RESULT_NAMES.get(st.result, "?")
            why = f" ({sgctl.REASON_NAMES.get(st.reason, st.reason)})" if st.reason else ""
            mine = "" if self.lastid is None or st.reqid == self.lastid else "  (waiting for mine)"
            r["last request"].set(f"#{st.reqid} {spec.action_names.get(st.action, '-')}: {res}{why}{mine}")
            r["motor_cubemars"].set(sgctl.CUBE_NAMES.get(st.motor_cubemars_status, "?"))
            af = self.af
            r[self.k_cube].set(f"{st.motor_cubemars_pos * af:+.3f} {st.motor_cubemars_vel * af:+.3f} {st.motor_cubemars_current:+.3f}")
            r["sensor_torque Mx My Mz [N*m]"].set(f"{st.sensor_torque_mx:+.3f} {st.sensor_torque_my:+.3f} {st.sensor_torque_mz:+.3f}")
            r[self.k_macro].set(f"{st.motor_macro_pos * af:+.3f} {st.motor_macro_vel * af:+.3f} {st.motor_macro_current:+.3f} {st.motor_macro_pwm:+.1f}")
            conf = {0: "not configured", 1: "pids only", 2: "setup only", 3: "configured"}.get(st.motor_macro_configured, "?")
            r["motor_macro"].set(f"{sgctl.CONTROLMODE_NAMES.get(st.motor_macro_mode, hex(st.motor_macro_mode))}, {conf}"
                                f"{', EMULATED by amc.gtw' if st.motor_macro_emulated else ''}"
                                f"{', EXTERNAL FAULT' if st.motor_macro_faults & sgctl.FAULT_EXTERNAL else ''}"
                                f"{', amc.gtw holds EXTFAULT' if st.gw_extfault else ''}")
            pf = st.sensor_position_flags
            r[self.k_pos].set(f"{st.sensor_position_pos * af:+.3f}  #{st.sensor_position_counter}" +
                              ("" if pf & 1 else "  not valid") + ("  EMULATED by amc.gtw" if pf & 2 else ""))
            r["frames sent / state back"].set(f"{st.sent} / {st.received}")
            r["lost / clamped"].set(f"{st.lost} / {st.clamped}")
            self.errv.set(("ERROR: " + sgctl.ERR_NAMES.get(st.err, str(st.err))) if st.err else "")
            now = time.monotonic()
            for tr in self.traces:
                tr.add(now, [getattr(st, f) * (self.af if f in sgact_spec.ANGLE_FIELDS else 1.0) for f in tr.fields])
            idle, running = st.state == 0, st.state == 3
            self.binit.config(state="normal" if idle and same else "disabled")
            self.bdeinit.config(state="normal" if idle and same else "disabled")
            self.bstart.config(state="normal" if idle and same else "disabled")
            self.bstop.config(state="normal" if running else "disabled")
            self.combo.config(state="disabled" if st.state in (3, 4) else "readonly")
        for tr in self.traces:
            tr.draw()
        self.root.after(100, self.tick)


def main():
    ap = argparse.ArgumentParser(description="the control panel of sgact: see the top of this file")
    ap.add_argument("--host", default="127.0.0.1", help="IP of sgact: the Speedgoat, or the PC with sgact_daemon.py")
    ap.add_argument("--spec", default=None, help="sgact_panel.toml (default: $SGACT_SPEC, then host/speedgoat/sgact_panel.toml)")
    ap.add_argument("--ctl-port", type=int, default=sgctl.PORT_CTL)
    ap.add_argument("--status-port", type=int, default=sgctl.PORT_STATUS, help="where this panel listens")
    args = ap.parse_args()
    try:
        spec = sgact_spec.load(args.spec)
    except sgact_spec.SpecError as e:
        print(f"[error] {e}")
        return 1
    try:
        client = sgctl.Client(spec.crc, args.host, args.ctl_port, args.status_port)
    except OSError as e:
        print(e)
        return 1
    root = tk.Tk()
    Panel(root, client, spec)
    root.mainloop()
    return 0


if __name__ == "__main__":
    sys.exit(main())

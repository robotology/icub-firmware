#!/usr/bin/env python3
"""
Viewer of the state of the CubeMars emulator (cubemars_emulator.py): the third program of

    cubemars_panel.py  <--CAN-->  cubemars_emulator.py  --UDP-->  cubemars_viewer.py

It only listens to the UDP datagrams (JSON) published by the emulator and shows them: angle (dial + multi-turn value), speed,
torque, current, temperature, error, mode, the target, the MIT parameters, the counters, and rolling plots of the last seconds.
It sends nothing and does not touch the CAN bus. Standard library only (tkinter).

  python cubemars_viewer.py                      listens on 0.0.0.0:5005 (the default of the emulator: 127.0.0.1:5005)
  python cubemars_viewer.py --port 5006 --window 20
  python cubemars_viewer.py --text               no GUI, one line per second on the console (a check of the stream)
"""

import argparse
import collections
import json
import math
import socket
import sys
import threading
import time

ERRORS = {0: "none", 1: "motor over-temperature", 2: "over-current", 3: "over-voltage", 4: "under-voltage", 5: "encoder",
          6: "MOSFET over-temperature", 7: "motor lock-up"}

BG, FG, DIM, GRID = "#14181d", "#e6e9ee", "#7d8794", "#2a313a"
C_POS, C_SPD, C_TRQ, C_CUR, C_TGT, C_BAD, C_OK = "#4fc3f7", "#81c784", "#ffb74d", "#ce93d8", "#fff176", "#ef5350", "#66bb6a"


class Receiver:
    """UDP listener: keeps the last state and a history of (t_local, state)."""

    def __init__(self, host, port, window_s):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind((host, port))
        self.sock.settimeout(0.2)
        self.lock = threading.Lock()
        self.last = None
        self.last_rx = 0.0
        self.count = 0
        self.history = collections.deque()
        self.window = window_s
        self.running = True
        self.thread = threading.Thread(target=self._run, daemon=True)
        self.thread.start()

    def _run(self):
        while self.running:
            try:
                data, _ = self.sock.recvfrom(65535)
            except socket.timeout:
                continue
            except OSError:
                break
            try:
                st = json.loads(data.decode("utf-8"))
            except ValueError:
                continue
            now = time.time()
            with self.lock:
                self.last, self.last_rx = st, now
                self.count += 1
                self.history.append((now, st))
                while self.history and now - self.history[0][0] > self.window:
                    self.history.popleft()

    def get(self):
        with self.lock:
            return self.last, self.last_rx, self.count, list(self.history)

    def close(self):
        self.running = False
        self.sock.close()


def run_text(rx):
    try:
        while True:
            time.sleep(1.0)
            st, t_rx, n, _ = rx.get()
            if st is None:
                print("no data yet", flush=True)
                continue
            print(f"[{n} datagrams] id=0x{st['id']:02X} {st['mode']:<12} pos={st['pos_deg']:9.2f} deg  speed={st['speed_rads']:7.3f} rad/s "
                  f"torque={st['torque_nm']:7.3f} N.m  I={st['current_a']:6.2f} A  T={st['temp_c']:.1f} C  err={st['error']}", flush=True)
    except KeyboardInterrupt:
        pass


def run_gui(rx, window_s):
    import tkinter as tk

    root = tk.Tk()
    root.title("CubeMars AK70-9 emulator - viewer")
    root.configure(bg=BG)
    W, H = 980, 640
    root.geometry(f"{W}x{H}")
    cv = tk.Canvas(root, width=W, height=H, bg=BG, highlightthickness=0)
    cv.pack(fill="both", expand=True)

    def text(x, y, s, color=FG, size=10, anchor="w", bold=False):
        cv.create_text(x, y, text=s, fill=color, anchor=anchor, font=("Helvetica", size, "bold" if bold else "normal"))

    def dial(cx, cy, r, st):
        cv.create_oval(cx - r, cy - r, cx + r, cy + r, outline=GRID, width=2)
        for k in range(36):
            a = math.radians(k * 10)
            r1 = r - (14 if k % 3 == 0 else 7)
            cv.create_line(cx + r1 * math.sin(a), cy - r1 * math.cos(a), cx + r * math.sin(a), cy - r * math.cos(a), fill=GRID)
        for k, lbl in ((0, "0"), (9, "90"), (18, "180"), (27, "270")):
            a = math.radians(k * 10)
            text(cx + (r + 16) * math.sin(a), cy - (r + 16) * math.cos(a), lbl, DIM, 9, "center")
        pos = st["pos_deg"]
        a = math.radians(pos)
        cv.create_line(cx, cy, cx + (r - 10) * math.sin(a), cy - (r - 10) * math.cos(a), fill=C_POS, width=5, capstyle="round")
        cv.create_oval(cx - 6, cy - 6, cx + 6, cy + 6, fill=C_POS, outline="")
        if st["mode"] in ("position", "posVelocity"):
            ta = math.radians(st["target"])
            cv.create_line(cx + (r - 22) * math.sin(ta), cy - (r - 22) * math.cos(ta), cx + (r + 2) * math.sin(ta), cy - (r + 2) * math.cos(ta),
                           fill=C_TGT, width=4)
        elif st["mode"] == "MIT":
            ta = math.degrees(st["mit"]["pos_rad"])
            ta = math.radians(ta)
            cv.create_line(cx + (r - 22) * math.sin(ta), cy - (r - 22) * math.cos(ta), cx + (r + 2) * math.sin(ta), cy - (r + 2) * math.cos(ta),
                           fill=C_TGT, width=4)
        text(cx, cy + r + 38, f"{pos:+.2f} deg", C_POS, 18, "center", True)
        text(cx, cy + r + 62, f"{pos / 360.0:+.3f} turns    {math.radians(pos):+.3f} rad", DIM, 10, "center")

    def bar(x, y, w, h, value, vmax, color, label, unit, fmt="{:+.3f}"):
        text(x, y - 14, label, DIM, 10)
        text(x + w, y - 14, (fmt + " {}").format(value, unit), FG, 11, "e", True)
        cv.create_rectangle(x, y, x + w, y + h, outline=GRID)
        mid = x + w / 2
        f = max(-1.0, min(1.0, value / vmax)) if vmax else 0.0
        cv.create_rectangle(mid, y + 2, mid + f * w / 2, y + h - 2, fill=color, outline="")
        cv.create_line(mid, y - 3, mid, y + h + 3, fill=DIM)
        text(x, y + h + 10, f"{-vmax:g}", DIM, 8)
        text(x + w, y + h + 10, f"{vmax:g}", DIM, 8, "e")

    def plot(x, y, w, h, hist, key, vmax, color, label, unit):
        cv.create_rectangle(x, y, x + w, y + h, outline=GRID)
        cv.create_line(x, y + h / 2, x + w, y + h / 2, fill=GRID, dash=(2, 4))
        text(x + 6, y + 10, f"{label} [{unit}]", color, 9, "w", True)
        text(x + w - 4, y + 10, f"+-{vmax:g}", DIM, 8, "e")
        if len(hist) < 2:
            return
        now = hist[-1][0]
        pts = []
        for t, st in hist:
            v = key(st)
            px = x + w - (now - t) / window_s * w
            py = y + h / 2 - max(-1.0, min(1.0, v / vmax)) * (h / 2 - 3)
            pts += [px, py]
        cv.create_line(*pts, fill=color, width=1.5)

    def redraw():
        st, t_rx, n, hist = rx.get()
        cv.delete("all")
        age = time.time() - t_rx
        live = st is not None and age < 1.0
        text(16, 18, "CubeMars AK70-9 KV60 - emulator state", FG, 13, "w", True)
        if st is None:
            text(W / 2, H / 2, "waiting for the emulator (UDP)...", DIM, 16, "center")
            root.after(50, redraw)
            return
        text(W - 16, 18, "LIVE" if live else f"NO DATA for {age:.1f} s", C_OK if live else C_BAD, 12, "e", True)
        lim = st["limits"]

        dial(170, 175, 95, st)

        bx, bw = 360, 330
        bar(bx, 90, bw, 18, st["speed_rads"], lim["vmax"], C_SPD, "speed", "rad/s")
        bar(bx, 150, bw, 18, st["torque_nm"], lim["tmax"], C_TRQ, "torque", "N.m")
        bar(bx, 210, bw, 18, st["current_a"], lim["imax"], C_CUR, "current", "A", "{:+.2f}")
        text(bx, 262, f"speed {st['speed_erpm']:+.0f} ERPM (electrical)", DIM, 10)

        rx_x = 730
        err = st["error"]
        rows = [("mode", st["mode"], C_TGT), ("driver id", f"0x{st['id']:02X}", FG), ("temperature", f"{st['temp_c']:.1f} C", FG),
                ("error", f"{err}: {ERRORS.get(err, '?')}", C_OK if err == 0 else C_BAD),
                ("CAN commands", f"{st['cmds']}  (last: {st['last_cmd']})", FG),
                ("CAN feedback", ("on query" if st["query_reply"] else f"{st['fb_hz']} Hz") + f", {st['fb_tx']} sent", FG),
                ("0x2A frame / single turn", f"{'on' if st['pos32'] else 'off'} / {'on' if st['single_turn'] else 'off'}", FG),
                ("origin", "permanent" if st["origin_permanent"] else "temporary", FG),
                ("datagrams", f"{n}  (emulator t = {st['t']:.1f} s)", DIM)]
        for i, (k, v, c) in enumerate(rows):
            text(rx_x, 78 + i * 30, k, DIM, 9)
            text(rx_x, 78 + i * 30 + 14, v, c, 11, "w", True)

        m = st["mit"]
        text(bx, 296, "target / MIT", DIM, 10)
        if st["mode"] == "MIT":
            text(bx, 318, f"Kp {m['kp']:.1f}   Kd {m['kd']:.2f}", C_TGT, 11)
            text(bx, 336, f"p {m['pos_rad']:+.3f} rad   v {m['vel_rads']:+.2f} rad/s   t {m['torque_nm']:+.2f} N.m", C_TGT, 11)
        elif st["mode"] in ("position", "posVelocity"):
            text(bx, 318, f"target position {st['target']:+.2f} deg", C_TGT, 11)
        elif st["mode"] in ("velocity", "duty"):
            text(bx, 318, f"target speed {st['target']:+.3f} rad/s", C_TGT, 11)
        elif st["mode"] in ("current", "currentBrake"):
            text(bx, 318, f"target current {st['target']:+.3f} A", C_TGT, 11)
        else:
            text(bx, 318, "-", DIM, 11)

        py, ph, pw = 368, 70, W - 48
        plot(24, py, pw, ph, hist, lambda s: s["pos_deg"], max(10.0, max(abs(s["pos_deg"]) for _, s in hist)), C_POS, "position", "deg")
        plot(24, py + ph + 8, pw, ph, hist, lambda s: s["speed_rads"], lim["vmax"], C_SPD, "speed", "rad/s")
        plot(24, py + 2 * (ph + 8), pw, ph, hist, lambda s: s["torque_nm"], lim["tmax"], C_TRQ, "torque", "N.m")
        text(24, H - 8, f"last {window_s:g} s", DIM, 8)
        root.after(40, redraw)

    redraw()
    try:
        root.mainloop()
    except KeyboardInterrupt:
        pass


def main(argv=None):
    ap = argparse.ArgumentParser(description="viewer of the CubeMars emulator state (UDP/JSON)")
    ap.add_argument("--host", default="0.0.0.0", help="local address to bind (default 0.0.0.0)")
    ap.add_argument("--port", type=int, default=5005, help="UDP port (default 5005, the default of the emulator)")
    ap.add_argument("--window", type=float, default=10.0, help="seconds shown by the plots (default 10)")
    ap.add_argument("--text", action="store_true", help="no GUI: one line per second on the console")
    a = ap.parse_args(argv)
    try:
        rx = Receiver(a.host, a.port, a.window)
    except OSError as e:
        print(f"cannot listen on {a.host}:{a.port}: {e}", file=sys.stderr)
        return 1
    print(f"viewer listening on UDP {a.host}:{a.port}", flush=True)
    if a.text:
        run_text(rx)
    else:
        run_gui(rx, a.window)
    rx.close()
    return 0


if __name__ == "__main__":
    sys.exit(main())

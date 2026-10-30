#!/usr/bin/env python3
"""
make_golden.py - writes tests/golden.txt: the bytes of the frames as the Python tools of host/shared make them
(amc_gtw.py, sgctl.py, tethercodec.py). test_golden.m checks that the MATLAB functions of the Speedgoat make the same bytes.

    py tools/make_golden.py                                     (from the folder simulink)
    py tools/make_golden.py --state-cpp state_cpp.txt           the state frame made by the firmware on a PC:
                                                                appl.gateway.01/test/host: ./test_gwlink --golden > state_cpp.txt

w/out --state-cpp the line state_cpp of the current golden.txt is kept: it comes from the C++ code of amc.gtw, not from Python.
The header of the TETHER frames carries the time 0.
"""

import argparse
import os
import sys

_HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(_HERE))), "shared"))   # host/shared
import amc_gtw as gw        # noqa: E402
import sgctl                # noqa: E402

MMCFG = [1.0, 0.1, 0.0, 0.5, 0.05, 0.0, 1.0, 2.0, 3.0, 3, 8, 0, 4096, -5]


def frames():
    gw._CODEC.now_us = lambda: 0            # the time of the header: 0, as the MATLAB tests give it
    g = {}
    g["mit_a"] = gw.mit_frame(0, 1, 0, 1, 0, 7)
    g["mit_b"] = gw.mit_frame(5, 0.5, 0.5, 0, 0.1, 123456)
    g["mit_c"] = gw.mit_frame(25, 3, -12.56, -10, -3, 4294967295)
    g["hb"] = gw.empty_frame()
    g["rpc_enable"] = gw.run_frame(gw.OP_CUBE_ENABLE)
    g["rpc_disable"] = gw.run_frame(gw.OP_CUBE_DISABLE)
    g["rpc_pax_enable"] = gw.run_frame(gw.OP_PAX_ENABLE)
    g["rpc_pax_disable"] = gw.run_frame(gw.OP_PAX_DISABLE)
    # the motor_macro
    g["cmd_mit_target"] = gw.command_frame((5, 0.5, 0.5, 0, 0.1, 7), (90.0, gw.CM_SPEEDVOLTAGE, 7))
    g["cmd_target"] = gw.command_frame(None, (0.5, gw.CM_CURRENT, 8))
    g["rpc_motor_pids"] = gw.run_frame(gw.OP_MOTOR_PIDS, True, gw.pids_arg(*MMCFG[0:6]))
    g["rpc_motor_setup"] = gw.run_frame(gw.OP_MOTOR_SETUP, True, gw.setup_arg(*MMCFG[6:14]))
    g["rpc_motor_mode"] = gw.run_frame(gw.OP_MOTOR_MODE, True, gw.mode_arg(gw.CM_SPEEDVOLTAGE))
    # the panel link: CTL version 3, STATUS version 4
    g["ctl3_a"] = sgctl.pack_ctl(3, 1, 3, 9, 1234, 0x12345678, 10, [1, 1], MMCFG)
    g["ctl3_extfault"] = sgctl.pack_ctl(0, 0, 0, 10, 1234, 0x12345678, 0, [], MMCFG, extfault=True)
    g["status4_a"] = sgctl.pack_status(3, 1, 3, 0, 3, 0, 3, 2, 1234, 9, 0x12345678, 1.5, 0.25, -0.5, 0.125, 1, -2, 3,
                                       0.5, 1.5, 0.75, gw.CM_SPEEDVOLTAGE, 3, 1, 100, 99, 1, 2, 12.5, gw.FAULT_EXTERNAL,
                                       -0.25, 4321, gw.POS_VALID | gw.POS_EMULATED, 1)
    g["rpc_gw_fault"] = gw.run_frame(gw.OP_GW_FAULT, True, gw.fault_arg(True))
    return g


def main():
    ap = argparse.ArgumentParser(description="writes tests/golden.txt")
    ap.add_argument("--out", default=os.path.join(os.path.dirname(_HERE), "tests", "golden.txt"))
    ap.add_argument("--state-cpp", default=None, help="the output of test_gwlink --golden (a line: state_cpp <hex>)")
    args = ap.parse_args()
    g = {k: v.hex() for k, v in frames().items()}
    src = args.state_cpp or args.out
    state = None
    if os.path.isfile(src):
        for line in open(src):
            p = line.split()
            if len(p) == 2 and p[0] == "state_cpp":
                state = p[1]
    if state is None:
        print("[error] no state_cpp: give --state-cpp")
        return 1
    if gw.decode_state(bytes.fromhex(state)) is None:
        print("[error] state_cpp is not a state frame for amc_gtw.py")
        return 1
    g["state_cpp"] = state
    with open(args.out, "w", newline="\n") as f:
        for k, v in g.items():
            f.write(f"{k} {v}\n")
    print(f"written {args.out}: {len(g)} frames")
    return 0


if __name__ == "__main__":
    sys.exit(main())

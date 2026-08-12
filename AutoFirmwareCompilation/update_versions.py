#!/usr/bin/env python3
"""
iCub Firmware Version Manager
==============================
Reads firmware_versions.yaml, shows current versions, lets the user
update one or more boards interactively, patches the source files,
and writes the new values back to the YAML.

Usage:
    python update_versions.py [--build] [--yaml PATH]

Options:
    --build     Launch buildAllBoards.bat automatically after updating.
    --yaml PATH Use an alternate YAML registry (default: firmware_versions.yaml
                in the same directory as this script).

Requirements:
    pip install pyyaml
"""

import os
import re
import sys
import shutil
import subprocess
import argparse
from datetime import datetime
from pathlib import Path

try:
    import yaml
except ImportError:
    print("ERROR: PyYAML is required. Run:  pip install pyyaml")
    sys.exit(1)


# ── Month / Day name tables (used by AMC and AMC2C source formats) ────────────

MONTH_NAME = {
    1: 'Jan', 2: 'Feb', 3: 'Mar',  4: 'Apr',  5: 'May',  6: 'Jun',
    7: 'Jul', 8: 'Aug', 9: 'Sep', 10: 'Oct', 11: 'Nov', 12: 'Dec',
}
MONTH_FROM_NAME = {v: k for k, v in MONTH_NAME.items()}

DAY_NAME = {
    1: 'one',        2: 'two',       3: 'three',     4: 'four',
    5: 'five',       6: 'six',       7: 'seven',     8: 'eight',
    9: 'nine',      10: 'ten',      11: 'eleven',   12: 'twelve',
   13: 'thirteen',  14: 'fourteen', 15: 'fifteen',  16: 'sixteen',
   17: 'seventeen', 18: 'eighteen', 19: 'nineteen', 20: 'twenty',
   21: 'twentyone', 22: 'twentytwo',23: 'twentythree', 24: 'twentyfour',
   25: 'twentyfive',26: 'twentysix',27: 'twentyseven',28: 'twentyeight',
   29: 'twentynine',30: 'thirty',   31: 'thirtyone',
}
DAY_FROM_NAME = {v: k for k, v in DAY_NAME.items()}


# ── Path helpers ──────────────────────────────────────────────────────────────

def workspace_root(script_dir: Path) -> Path:
    """
    Return the workspace root used by paths stored in firmware_versions.yaml.

    Supported layouts:

        WORKSPACE/
        ├── AutoFirmwareCompilation/
        └── icub-firmware/

    and:

        WORKSPACE/
        └── icub-firmware/
            └── AutoFirmwareCompilation/

    The YAML paths already include the "icub-firmware/" prefix, therefore
    when this script lives inside icub-firmware we must go up two levels.
    """
    parent = script_dir.parent

    if parent.name.lower() == "icub-firmware":
        return parent.parent

    return parent


def version_file_path(board: dict, root: Path) -> Path:
    rel = board['version_file'].replace('/', os.sep)
    return root / rel


# ── File I/O ──────────────────────────────────────────────────────────────────

def read_file(path: Path) -> str:
    for enc in ('utf-8', 'utf-8-sig', 'latin-1'):
        try:
            return path.read_text(encoding=enc)
        except Exception:
            continue
    raise OSError(f"Cannot read {path}")


def write_file(path: Path, content: str) -> None:
    path.write_text(content, encoding='utf-8')


def backup(path: Path) -> None:
    shutil.copy2(path, str(path) + '.bak')


# ── Version data model ────────────────────────────────────────────────────────

def ver_str(v: dict) -> str:
    """Pretty-print a version/build_date dict."""
    vv = v.get('version', {})
    dd = v.get('build_date', {})
    maj = vv.get('major', '?')
    min_ = vv.get('minor', '?')
    y = dd.get('year', '????')
    mo = dd.get('month', 0) or 0
    d = dd.get('day',   0) or 0
    h = dd.get('hour',  0) or 0
    mi = dd.get('minute', 0) or 0
    return f"v{maj}.{min_}  ({y}-{mo:02d}-{d:02d} {h:02d}:{mi:02d})"


def yaml_current(board: dict) -> dict:
    return {
        'version':    dict(board['version']),
        'build_date': dict(board['build_date']),
    }


# ── Source-file readers (extract version from actual C/C++ file) ──────────────

def _read_c_defines(content: str, board: dict):
    p = board.get('define_prefix', 'EOMTHEEMSAPPLCFG')
    has_off = board.get('has_offset', False)

    if has_off:
        m = re.search(rf'#define {p}_VERSION_MAJOR\s+\(VERSION_MAJOR_OFFSET\+(\d+)\)', content)
    else:
        m = re.search(rf'#define {p}_VERSION_MAJOR\s+(\d+)', content)
    major = int(m.group(1)) if m else None

    def def_int(key):
        m2 = re.search(rf'#define {p}_{key}\s+(\d+)', content)
        return int(m2.group(1)) if m2 else None

    return {
        'version':    {'major': major, 'minor': def_int('VERSION_MINOR')},
        'build_date': {
            'year':   def_int('BUILDDATE_YEAR'),
            'month':  def_int('BUILDDATE_MONTH'),
            'day':    def_int('BUILDDATE_DAY'),
            'hour':   def_int('BUILDDATE_HOUR'),
            'minute': def_int('BUILDDATE_MIN'),
        },
    }


def _read_cpp_theapplication(content: str, _board: dict):
    m = re.search(r'Process::eApplication,\s*\n\s*\{(\d+),\s*(\d+)\}', content)
    major = int(m.group(1)) if m else None
    minor = int(m.group(2)) if m else None

    m2 = re.search(r'\{(\d+),\s*Month::(\w+),\s*Day::(\w+),\s*(\d+),\s*(\d+)\}', content)
    if m2:
        year   = int(m2.group(1))
        month  = MONTH_FROM_NAME.get(m2.group(2))
        day    = DAY_FROM_NAME.get(m2.group(3))
        hour   = int(m2.group(4))
        minute = int(m2.group(5))
    else:
        year = month = day = hour = minute = None

    return {
        'version':    {'major': major, 'minor': minor},
        'build_date': {'year': year, 'month': month, 'day': day,
                       'hour': hour, 'minute': minute},
    }


def _read_cpp_amc2c_sig(content: str, _board: dict):
    m = re.search(r'\{(\d+),\s*(\d+),\s*\d+,\s*\d+\},?\s*//\s*application version', content)
    major = int(m.group(1)) if m else None
    minor = int(m.group(2)) if m else None

    m2 = re.search(
        r'\{(\d+),\s*embot::app::eth::Month::(\w+),\s*embot::app::eth::Day::(\w+),\s*(\d+),\s*(\d+)\}',
        content)
    if m2:
        year   = int(m2.group(1))
        month  = MONTH_FROM_NAME.get(m2.group(2))
        day    = DAY_FROM_NAME.get(m2.group(3))
        hour   = int(m2.group(4))
        minute = int(m2.group(5))
    else:
        year = month = day = hour = minute = None

    return {
        'version':    {'major': major, 'minor': minor},
        'build_date': {'year': year, 'month': month, 'day': day,
                       'hour': hour, 'minute': minute},
    }


_READERS = {
    'c_defines':          _read_c_defines,
    'cpp_theapplication': _read_cpp_theapplication,
    'cpp_amc2c_sig':      _read_cpp_amc2c_sig,
}


def read_version_from_file(board: dict, file_path: Path):
    """Returns version dict read from the source file, or None on failure."""
    try:
        content = read_file(file_path)
        reader = _READERS.get(board['format'])
        return reader(content, board) if reader else None
    except Exception:
        return None


# ── Source-file updaters ──────────────────────────────────────────────────────

def _update_c_defines(content: str, board: dict, ver: dict) -> str:
    p     = board.get('define_prefix', 'EOMTHEEMSAPPLCFG')
    has_off = board.get('has_offset', False)
    v  = ver['version']
    dd = ver['build_date']
    maj, min_ = v['major'], v['minor']
    y, mo, d, h, mi = dd['year'], dd['month'], dd['day'], dd['hour'], dd['minute']

    if has_off:
        content = re.sub(
            rf'(#define {p}_VERSION_MAJOR\s+\(VERSION_MAJOR_OFFSET\+)\d+(\))',
            lambda m: f'{m.group(1)}{maj}{m.group(2)}',
            content)
    else:
        content = re.sub(rf'(#define {p}_VERSION_MAJOR\s+)\d+',
                         f'\\g<1>{maj}', content)

    content = re.sub(rf'(#define {p}_VERSION_MINOR\s+)\d+',   f'\\g<1>{min_}', content)
    content = re.sub(rf'(#define {p}_BUILDDATE_YEAR\s+)\d+',  f'\\g<1>{y}',    content)
    content = re.sub(rf'(#define {p}_BUILDDATE_MONTH\s+)\d+', f'\\g<1>{mo}',   content)
    content = re.sub(rf'(#define {p}_BUILDDATE_DAY\s+)\d+',   f'\\g<1>{d}',    content)
    content = re.sub(rf'(#define {p}_BUILDDATE_HOUR\s+)\d+',  f'\\g<1>{h}',    content)
    content = re.sub(rf'(#define {p}_BUILDDATE_MIN\s+)\d+',   f'\\g<1>{mi}',   content)
    return content


def _update_cpp_theapplication(content: str, _board: dict, ver: dict) -> str:
    v  = ver['version']
    dd = ver['build_date']
    maj, min_ = v['major'], v['minor']
    y, mo, d, h, mi = dd['year'], dd['month'], dd['day'], dd['hour'], dd['minute']
    mname = MONTH_NAME[mo]
    dname = DAY_NAME[d]

    # {major, minor} right after Process::eApplication,
    content = re.sub(
        r'(Process::eApplication,\s*\n\s*)\{\d+,\s*\d+\}',
        f'\\g<1>{{{maj}, {min_}}}',
        content)

    # date line: {year, Month::xxx, Day::xxx, hour, minute}
    content = re.sub(
        r'\{\d+,\s*Month::\w+,\s*Day::\w+,\s*\d+,\s*\d+\}',
        f'{{{y}, Month::{mname}, Day::{dname}, {h}, {mi}}}',
        content)
    return content


def _update_cpp_amc2c_sig(content: str, _board: dict, ver: dict) -> str:
    v  = ver['version']
    dd = ver['build_date']
    maj, min_ = v['major'], v['minor']
    y, mo, d, h, mi = dd['year'], dd['month'], dd['day'], dd['hour'], dd['minute']
    mname = MONTH_NAME[mo]
    dname = DAY_NAME[d]

    # {major, minor, 0, 0},  // application version  (update all occurrences = both #ifdef branches)
    content = re.sub(
        r'\{\d+,\s*\d+,\s*\d+,\s*\d+\}(,?\s*//\s*application version)',
        f'{{{maj}, {min_}, 0, 0}}\\1',
        content)

    # date with embot::app::eth:: prefix (all occurrences)
    content = re.sub(
        r'\{\d+,\s*embot::app::eth::Month::\w+,\s*embot::app::eth::Day::\w+,\s*\d+,\s*\d+\}',
        f'{{{y}, embot::app::eth::Month::{mname}, embot::app::eth::Day::{dname}, {h}, {mi}}}',
        content)
    return content


_UPDATERS = {
    'c_defines':          _update_c_defines,
    'cpp_theapplication': _update_cpp_theapplication,
    'cpp_amc2c_sig':      _update_cpp_amc2c_sig,
}


def apply_update(board: dict, new_ver: dict, file_path: Path) -> bool:
    """Patches the source file. Returns True on success."""
    updater = _UPDATERS.get(board['format'])
    if not updater:
        print(f"  ERROR: unknown format '{board['format']}' for board {board['name']}")
        return False
    try:
        content = read_file(file_path)
        new_content = updater(content, board, new_ver)
        if new_content == content:
            print(f"  WARNING: no changes applied to {file_path.name} — check regex patterns")
            return False
        backup(file_path)
        write_file(file_path, new_content)
        print(f"  ✓ Patched: {file_path}")
        return True
    except Exception as e:
        print(f"  ERROR: could not update {file_path}: {e}")
        return False


# ── Interactive prompt ────────────────────────────────────────────────────────

def prompt_int(label: str, default: int) -> int:
    val = input(f"    {label} [{default}]: ").strip()
    if val == '':
        return default
    try:
        return int(val)
    except ValueError:
        print(f"    Invalid input, keeping {default}")
        return default


def prompt_board(board: dict, current: dict):
    """
    Interactively asks the user for new version values.
    Returns the new version dict, or None if nothing changed / user cancelled.
    """
    print(f"\n  ╔══ {board['name']} — {board.get('description', '')} ══")
    print(f"  ║  Current: {ver_str(current)}")

    today = datetime.now()

    maj  = prompt_int("Version major", current['version']['major'])
    min_ = prompt_int("Version minor", current['version']['minor'])

    use_today = input(
        f"    Use today's date ({today.strftime('%Y-%m-%d %H:%M')})? [y/N]: "
    ).strip().lower()

    if use_today == 'y':
        y, mo, d, h, mi = today.year, today.month, today.day, today.hour, today.minute
    else:
        dd = current['build_date']
        y  = prompt_int("Year",   dd['year'])
        mo = prompt_int("Month",  dd['month'])
        d  = prompt_int("Day",    dd['day'])
        h  = prompt_int("Hour",   dd['hour'])
        mi = prompt_int("Minute", dd['minute'])

    # Basic date validation
    if not (1 <= mo <= 12 and 1 <= d <= 31 and 0 <= h <= 23 and 0 <= mi <= 59):
        print("  ERROR: invalid date/time values. Skipping this board.")
        return None

    new_ver = {
        'version':    {'major': maj, 'minor': min_},
        'build_date': {'year': y, 'month': mo, 'day': d, 'hour': h, 'minute': mi},
    }

    if new_ver == current:
        print("  ║  No changes.")
        return None

    print(f"  ║  New:     {ver_str(new_ver)}")
    confirm = input("  ╚═ Apply? [Y/n]: ").strip().lower()
    return new_ver if confirm != 'n' else None


# ── YAML write-back ───────────────────────────────────────────────────────────

def update_yaml_entry(board: dict, new_ver: dict) -> None:
    board['version']['major']    = new_ver['version']['major']
    board['version']['minor']    = new_ver['version']['minor']
    board['build_date']['year']   = new_ver['build_date']['year']
    board['build_date']['month']  = new_ver['build_date']['month']
    board['build_date']['day']    = new_ver['build_date']['day']
    board['build_date']['hour']   = new_ver['build_date']['hour']
    board['build_date']['minute'] = new_ver['build_date']['minute']


# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(
        description='iCub firmware version manager — patches source files from a YAML registry.')
    parser.add_argument('--build', action='store_true',
                        help='Launch buildAllBoards.bat after updating versions.')
    parser.add_argument('--yaml', default=None,
                        help='Path to the YAML registry (default: firmware_versions.yaml '
                             'in the same folder as this script).')
    args = parser.parse_args()

    script_dir   = Path(__file__).parent.resolve()
    yaml_path    = Path(args.yaml).resolve() if args.yaml else script_dir / 'firmware_versions.yaml'
    root         = workspace_root(script_dir)

    print()
    print("╔══════════════════════════════════════════════════════════╗")
    print("║          iCub Firmware Version Manager                   ║")
    print("╚══════════════════════════════════════════════════════════╝")
    print(f"  Workspace : {root}")
    print(f"  Registry  : {yaml_path}")
    print()

    with open(yaml_path, encoding='utf-8') as f:
        config = yaml.safe_load(f)
    boards = config['boards']

    # ── Read current versions from source files ────────────────────────────
    file_versions = {}
    for b in boards:
        fp = version_file_path(b, root)
        file_versions[b['id']] = read_version_from_file(b, fp)

    # ── Display table ──────────────────────────────────────────────────────
    print("  Current firmware versions (read from source files):")
    print()
    for i, b in enumerate(boards, 1):
        fv = file_versions.get(b['id'])
        if fv:
            vs = ver_str(fv)
            yv = yaml_current(b)
            mismatch = (fv['version'] != yv['version'] or
                        fv['build_date'] != yv['build_date'])
            flag = "  ← YAML out of sync!" if mismatch else ""
        else:
            vs = "[file not found]"
            flag = ""
        print(f"  [{i}] {b['name']:<10} {vs}{flag}")

    print()
    print("  [A] Update ALL boards")
    print("  [Q] Quit without changes")
    print()
    sel = input("  Select boards to update (e.g. 1,2  or  A  or  Q): ").strip().upper()

    if not sel or sel == 'Q':
        print("\n  No changes made. Bye.")
        return

    if sel == 'A':
        chosen = [b['id'] for b in boards]
    else:
        chosen = []
        for token in sel.split(','):
            token = token.strip()
            try:
                idx = int(token) - 1
                if 0 <= idx < len(boards):
                    chosen.append(boards[idx]['id'])
                else:
                    print(f"  WARNING: index {token} out of range, skipped.")
            except ValueError:
                print(f"  WARNING: '{token}' is not a valid number, skipped.")

    if not chosen:
        print("\n  Nothing selected. Bye.")
        return

    # ── Process each selected board ────────────────────────────────────────
    yaml_dirty   = False
    files_patched = 0

    for bid in chosen:
        board = next((b for b in boards if b['id'] == bid), None)
        if board is None:
            continue

        # Use the version read from the file as the "current" baseline;
        # fall back to YAML if the file could not be parsed.
        current = file_versions.get(bid) or yaml_current(board)

        new_ver = prompt_board(board, current)
        if new_ver is None:
            continue

        fp = version_file_path(board, root)
        if apply_update(board, new_ver, fp):
            update_yaml_entry(board, new_ver)
            yaml_dirty = True
            files_patched += 1

    # ── Write YAML if changed ─────────────────────────────────────────────
    if yaml_dirty:
        with open(yaml_path, 'w', encoding='utf-8') as f:
            yaml.dump(config, f, default_flow_style=False,
                      allow_unicode=True, sort_keys=False)
        print(f"\n  ✓ Registry updated: {yaml_path.name}")

    if files_patched == 0:
        print("\n  No files were modified.")
        return

    # ── Optionally launch the build ────────────────────────────────────────
    if args.build:
        launch = 'y'
    else:
        print()
        launch = input("  Launch buildAllBoards.bat now? [y/N]: ").strip().lower()

    if launch == 'y':
        bat = script_dir / 'buildAllBoards.bat'
        if bat.exists():
            print(f"\n  Launching {bat.name} …\n")
            subprocess.run([str(bat)], shell=True)
        else:
            print(f"  ERROR: {bat} not found.")

    print("\n  Done.\n")


if __name__ == '__main__':
    main()

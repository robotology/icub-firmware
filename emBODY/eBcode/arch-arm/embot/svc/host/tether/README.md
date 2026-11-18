# embot-tether: the client of TETHER

`embot-tether` talks to `embot::svc::theTETHERservice` on a board: `rpc` (UDP 6666) reads, writes and runs what the
Dictionary of the board offers, `str` (UDP 7777) receives the stream of the variables. It is `tether.py` w/ its codec
`tethercodec.py`, in this folder. Install: `../README.md` (Windows 11, Ubuntu 24.04 / 26.04); only the commands:
`../quick.startup.md`. What TETHER is and how a board offers it: `../../doc/TETHER.md`.

The commands are the same on Windows and Ubuntu; in WSL2 add `--no-open` and open the pages in the browser of Windows.

```
embot-tether rpc --ip <board>                 page: variables and operations          http://localhost:8080
embot-tether rpc --ip <board> --cli           the same, from a prompt
embot-tether str [--ip <board>]               page: plots of the stream               http://localhost:8081
embot-tether str --cli [--log stream.csv]     the same, as a text summary and a csv file
embot-tether --version                        which files run (see ../README.md, one venv per clone)
```

**The Dictionary.** At start `rpc` asks the board for the CRC32 of its Dictionary. If `dictionary.toml` does not exist, or was made from
another Dictionary or protocol, it browses the board and rewrites the file (the old one is kept as `.bak`). `--no-browse` works
offline with the file as it is. `str` only reads the file (the streaming socket does not support the browse): run `rpc` once.
`str` checks the file every second and loads it again when `rpc` rewrites it. `dictionary.toml` is generated, do not edit
it by hand. It is read from the current folder (the folder of the example of the board), or from `--dictionary`.

## Options

| `rpc` | |
|---|---|
| `--ip` (required) | IP of the board |
| `--port` | rpc port of the board (6666) |
| `--no-browse` | do not ask the board for its Dictionary |
| `--dictionary` | the file (`./dictionary.toml`) |
| `--cli`, `--no-open` | text interface; do not open the browser |
| `--localport`, `--timeout`, `--tries` | local UDP port (any); seconds to wait for a reply (0.5); transmissions of ask / set (3) |
| `--verbose` | `--cli`: hex dump of the frames |
| `--gui-port`, `--native` | page port (8080); a window of its own (needs pywebview) |

| `str` | |
|---|---|
| `--ip` | IP of the board: other IPs are ignored and pings go there (default: the source of the first frame) |
| `--localport`, `--boardport` | where the board streams (7777); stream port of the board (7777) |
| `--ping` | seconds between the keepalive pings (2; 0 = none) |
| `--window`, `--history`, `--maxpoints`, `--fps` | page: seconds plotted (10), samples kept per signal (200000), points drawn per signal (3000), refreshes per second (10) |
| `--pj HOST:PORT` | page: forward to PlotJuggler from the start (`127.0.0.1:9870`) |
| `--every`, `--all`, `--quiet`, `--rtt`, `--log` | `--cli`: seconds between summaries, every frame, quiet, round trip time of the pings, csv file |
| `--gui-port`, `--dictionary`, `--cli`, `--no-open` | as above (8081) |

## The rpc page

Variables (one row each: ID, name, RO/RW, size and type, an editor per field, **ASK**, **SET** if RW, a **stream** checkbox),
operations (an editor for the argument and a **RUN** button, with the last result), the *stream* box (period, **START**, **STOP**,
state), the *board* box (**VERSION**, **STATS**, **PING**, *auto ask the RO variables*), a command line and the log. **BROWSE** reads the
Dictionary again.

## The str page

The streamed signals appear as the board sends them; tick them to plot (one chart, or one per signal), *pause*, *clear data*,
*record* to a csv, and the *PlotJuggler* box (host:port, *forward*). The header shows the frame rate and whether the pings are answered.

## The command line (`rpc --cli`)

ROPs separated by `;`. A line goes in as many frames as needed: at most 32 ROPs per frame, and frames whose replies fit
the frames of the board (`MGbrowseINFO` tells its maximum, 1472 bytes):

```
ask<IDname>                       say<IDname, value> or nak<IDname, ask, "why">
set<IDname, value>                no reply
set[+ack]<IDname, value>          ack<IDname, set> or nak<IDname, set, "why">: the ack is light, ask the value if needed
sig<IDname, value>                no reply
run<OPname[, arg]>                no reply
run[+ack]<OPname[, arg]>          ack<OPname, run[, result of up to 36 bytes]> or nak<OPname, run, "why the operation failed">
ping<> or ping<name>              ack<name, ping>
```

Values: `42`, `-3`, `0x1F`, `1.5`, `true`, `[1, 2, 3]`, `{a = 1, b = -2, c = 1.5}`, an `IDname`. A struct needs all its fields.

### The local commands

Besides the ROPs, the prompt has a few local commands, which do not talk to the board unless said:

| command | what it does |
|---|---|
| `list` | every variable and operation of the Dictionary: name, ID, size, type, and how to use it (e.g. `run[+ack]<OPadd, {a, b}>`) |
| `browse` | browses the board again (MGbrowseINFO, then MGbrowseITEM and MGbrowseFIELD for every item) and rewrites `dictionary.toml` |
| `help` | the grammar of a line and the management variables |
| `quit` | exits (also Ctrl-D, or Ctrl-Z then Enter on Windows) |

### The management variables

```
set[+ack]<MGstreamADD, IDname>   set[+ack]<MGstreamREM, IDname>   ask<MGstreamLIST>
set[+ack]<MGstreamSTART, ms>     0 = the default period; ask<MGstreamSTART> gives the period applied
set[+ack]<MGstreamSTOP>          ask<MGstreamSTART>  (the period, or stopped)
ask<MGstats>   ask<MGversion>   ask<MGbrowseINFO>   ask<MGbrowseITEM, index>   ask<MGbrowseFIELD, IDname, n>
```

## Streaming

Tick the variables (`MGstreamADD`) and start with `MGstreamSTART`. The board sends one frame per period with a `sig` per
streamed variable to the IP that started it, UDP port 7777. **Start `embot-tether str` first**: it receives the frames and **pings
the board every 2 s**; the board stops the stream after 10 s without a ping. The `rpc` page does not ping.

A long array (more than 16 items: a skin patch, the bins of a histogram, a table) is drawn as a bar chart: in the `str`
page with the values of the last frame, one chart per array, each one shown or hidden w/ its tick in the box *long
arrays*; in the `rpc` page beside its text box, at every `ask`. In the `str` page its items are listed as signals, to plot
them against time, only if *list the items of the long arrays* is on. The bins of a histogram (an array `bins` in a
struct w/ `lo` and `hi`, e.g. an `embot::instr::HistogramSnapshot`) have on the x axis the left edge of each bin, and the
title tells the width of the bins and the values below and beyond.

```
    embot-tether rpc                           embot-tether str                             board
          │                                       │                                     │
          │  set[+ack]<MGstreamADD, IDpaxVALUE>   │                                     │
          │───────────────────────────────────────┼────────────────────────────────────►│
          │  set[+ack]<MGstreamSTART, 100>        │                                     │
          │───────────────────────────────────────┼────────────────────────────────────►│   the stream goes to the IP that sent this ROP
          │  ack<MGstreamSTART, set>              │                                     │
          │◄──────────────────────────────────────┼─────────────────────────────────────│   port 7777
          │                                       │  sig<IDpaxVALUE, {...}>             │
          │                                       │◄────────────────────────────────────│   every 100 ms
          │                                       │  ping                               │
          │                                       │────────────────────────────────────►│   every 2 s: keeps the stream alive
          │                                       │  ack<ping>                          │
          │                                       │◄────────────────────────────────────│
          │  ... if no ping or frame arrives for 10 s, the board stops the stream (strkeepalivestops + 1)
          │                                       │                                     │
```

## How the browse works

The Dictionary of the board is described by three management variables, which `embot-tether` asks by itself at start (and
on `browse`); they can also be asked by hand:

- `ask<MGbrowseINFO>`: 12 bytes, `{nv, no, maxrops, reserved, maxframesize, maxdatasize, crc}`: the number of variables
  and operations, the most ROPs in a frame, the biggest frame and the biggest value of the board, and the CRC32 of the
  Dictionary. If the CRC is the one in `dictionary.toml`, `embot-tether` uses the file as it is.
- `ask<MGbrowseITEM, index>`: 40 bytes, the item `index` (0 ... nv + no - 1; variables first, then operations): its ID,
  kind (variable or operation), access, type, number of fields, size, the size and kind of the result of an operation,
  and its name (up to 28 chars).
- `ask<MGbrowseFIELD, IDname, n>`: 36 bytes, the field `n` of a struct variable, or of the argument and then the result
  of an operation: its type, count (> 1 for an array), offset and name.

## PlotJuggler 4

`embot-tether str` forwards every frame to the **UDP Server** streaming plugin of PlotJuggler, as one flat JSON object per frame:

```json
{"timestamp": 12.503, "IDpaxVALUE.force[0]": 0.0, "IDpaxVALUE.torque[2]": -0.5, "IDdummyUINT32": 1583}
```

`timestamp` is the board time in seconds; the names are the variable name (scalar), `name[k]` (array item), `name.field` (struct field).

1. Start PlotJuggler → **Streaming** → **UDP Server** → protocol **JSON**, port **9870** → **Start**. If the dialog offers to take the time from a field of the message, use `timestamp`.
2. `embot-tether str --ip <board> --pj 127.0.0.1:9870`, or open the page and use the *PlotJuggler* box.
3. Start the stream from `rpc`. The signals appear in the list of PlotJuggler: drag them onto a plot.

### Install PlotJuggler

Optional: it plots the stream live. Get it from the official GitHub repository (`PlotJuggler/PlotJuggler`, *Releases*).

| platform | how |
|---|---|
| Linux, Ubuntu | `sudo snap install plotjuggler` |
| Linux, any | the **AppImage** from the Releases page: `chmod +x PlotJuggler-*.AppImage && ./PlotJuggler-*.AppImage` |
| Linux, ROS | `sudo apt install ros-$ROS_DISTRO-plotjuggler-ros`, then `ros2 run plotjuggler plotjuggler` |
| Windows | the x64 installer from the Releases page |

In WSL2 with WSLg a Linux PlotJuggler opens in its own window. Or install the **Windows** version and keep `embot-tether` in WSL:
with mirrored networking `127.0.0.1` reaches both; in NAT mode give `--pj` the address of the Windows PC.

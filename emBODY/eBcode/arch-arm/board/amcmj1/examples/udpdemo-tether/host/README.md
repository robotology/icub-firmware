# host

The host of TETHER for this example is the generic client `tether` (`tether.py` and its codec `tethercodec.py`), which
lives w/ the service in `embot/svc/host/tether`: see the `README.md` there for the install (Windows, WSL2). This folder
holds only the Dictionary of the board, `dictionary.toml`, which `tether rpc` writes after the browse of the board
(it is not committed: the first `tether rpc` makes it).

Run it from this folder, so that it reads and writes `./dictionary.toml`:

    tether rpc --ip 10.0.1.99 --cli         # a prompt: ask<IDechoSTATS>, set[+ack]<IDechoCONFIG, {...}>, run[+ack]<OPadd, {a=1, b=2}>
    tether rpc --ip 10.0.1.99               # the same, as a page in the browser
    tether str --ip 10.0.1.99               # the plots of the stream (start it from rpc: set[+ack]<MGstreamSTART, 10>)

`rpc` and `str` must run on the same PC: the board sends the stream to the PC that started it. The full grammar of the
prompt is at the top of `embot/svc/host/tether/tether.py`.

## The prompt of `tether rpc --cli`

A line holds one or more ROPs separated by `;` (e.g. `ask<IDechoSTATS>; ask<MGstats>`). Besides the ROPs, the prompt
has a few local commands, which do not talk to the board unless said:

| command | what it does |
|---|---|
| `list` | every variable and operation of the Dictionary: name, ID, size, type, and how to use it (e.g. `run[+ack]<OPadd, {a, b}>`) |
| `browse` | browses the board again (MGbrowseINFO, then MGbrowseITEM and MGbrowseFIELD for every item) and rewrites `dictionary.toml` |
| `help` | the grammar of a line and the management variables |
| `quit` | exits (also Ctrl-D, or Ctrl-Z then Enter on Windows) |

## How the browse works

The Dictionary of the board is described by three management variables, which `tether` asks by itself at start (and
on `browse`); they can also be asked by hand:

- `ask<MGbrowseINFO>`: 12 bytes, `{nv, no, maxrops, reserved, maxframesize, maxdatasize, crc}`: the number of variables
  and operations, the most ROPs in a frame, the biggest frame and the biggest value of the board, and the CRC32 of the
  Dictionary. If the CRC is the one in `dictionary.toml`, `tether` uses the file as it is.
- `ask<MGbrowseITEM, index>`: 40 bytes, the item `index` (0 ... nv + no - 1; variables first, then operations): its ID,
  kind (variable or operation), access, type, number of fields, size, the size and kind of the result of an operation,
  and its name (up to 28 chars).
- `ask<MGbrowseFIELD, IDname, n>`: 36 bytes, the field `n` of a struct variable, or of the argument and then the result
  of an operation: its type, count (> 1 for an array), offset and name.

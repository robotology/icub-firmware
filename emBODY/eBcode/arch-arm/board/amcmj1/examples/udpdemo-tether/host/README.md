# host

The host of TETHER for this example is the generic client of `appl.gateway.01/host`: `tether/tether.py` and the one codec of
the ROP protocol (version 6), `shared/tethercodec.py`. the `tether.py` of this folder only runs it, w/ the Dictionary of
this folder.

| file | what |
|---|---|
| `tether.py` | it runs `../../appl.gateway.01/host/tether/tether.py`: `rpc` (ask / set / run on the Dictionary, port 6666) and `str` (the stream, port 7777) |
| `requirements.txt` | the Python packages: only `nicegui`, for the graphical pages. the text interface (`--cli`) needs nothing |
| `dictionary.toml` | the Dictionary of the board, written by `tether.py rpc` after the browse of the board |

Run it from this folder, so that it reads and writes `./dictionary.toml`:

    python tether.py rpc --ip 10.0.1.99 --cli         # a prompt: ask<IDechoSTATS>, set[+ack]<IDechoCONFIG, {...}>, run[+ack]<OPadd, {a=1, b=2}>
    python tether.py rpc --ip 10.0.1.99               # the same, as a page in the browser
    python tether.py str --ip 10.0.1.99               # the plots of the stream (start it from rpc: set[+ack]<MGstreamSTART, 10>)

On Windows `python` is the Python w/ nicegui installed (e.g. `py -3.13`, or a venv); in WSL2 e.g. `~/.venv/bin/python`.
`rpc` and `str` must run on the same PC: the board sends the stream to the PC that started it. The full grammar of the
prompt is at the top of `appl.gateway.01/host/tether/tether.py`.

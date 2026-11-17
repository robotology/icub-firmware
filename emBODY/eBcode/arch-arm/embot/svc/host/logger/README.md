# logger: the viewer of theLogger

`theLogger` (`embot/svc/embot_svc_theLogger.h`) sends the lines of `embot::core::print()` of a board over UDP and accepts
a few text commands on the same port (9999). `logger.py` shows the lines of one or more boards, in a page of the browser
or in the terminal. Nothing changes on the board: the viewer uses the commands that theLogger already has.

| file | what |
|---|---|
| `logger.py` | the viewer: the page (nicegui, the default) or `--cli` |
| `logger_emulator.py` | a board w/ theLogger on the PC, to try the viewer w/out a board |

## Install and run

Once, in the venv of `tether` (see `../README.md` for Windows and WSL2): `pip install -e .` from this folder. It gives the
commands `embot-logger` and `embot-logger-emulator` (or run `python logger.py ...` from here, w/out installing).

```console
embot-logger --ip 10.0.1.99                             # the page, at http://localhost:8090
embot-logger --ip 10.0.1.99 --ip 10.0.1.98              # two boards, one colour each
embot-logger --ip 10.0.1.99 --cli                       # in the terminal
embot-logger --localport 9999                           # no board given: it receives the boards that log to this PC:9999
```

At start the viewer sends `hello` to each board of `--ip`: from then on the board sends its lines to the viewer (`--no-hello`
leaves the destination of the boards as it is). A board that sends to the port of the viewer w/out being asked is added
by itself: with `--localport 9999` the viewer receives the boards whose destination is this PC:9999, as at their start.

The page needs `nicegui` (installed w/ it, the same of `tether`). `--cli` needs nothing.

## The page

- **boards**: add a board w/ `ip[:port]`. Each board has its colour, a tick to show or hide its lines, the buttons `hello`
  (its lines come here), `status`, `help`, the switches `on` and `deco`, and the last status: where its lines go, the lines
  taken, the frames sent and the lines lost (`dropped`). The status is asked every `--poll` s (default 3) w/out showing the
  reply. A warning says when the lines of a board go elsewhere (another viewer, an `nc`) or when it is silent.
- **the lines**: the time of the PC, the board, the time of the board and the text. The time of the board, in seconds, comes
  from the decoration (`deco on`: `[10.0.1.99 @ S12:m345:u678]`), which is removed from the text; w/out it, `-`.
  The replies to the commands (`[theLogger] ...`) are in italics and can be hidden.
- **filter**: a text, or a regular expression (e.g. `error|warn`), w/ or w/out case. It applies also to the lines already
  received (the last `--history` lines are kept, default 100000).
- **view**: `pause` stops the view but not the reception: `resume` shows also what arrived meanwhile. `clear`.
- **record**: every line received (also the filtered ones) to a `.txt` (as shown) or a `.csv` (pc time, board, board
  time in us, kind, text). `--record file` records from the start.

## The cli

The lines as they arrive, one colour per board. A line typed is a command for all the boards (e.g. `deco on`, `status`),
or for one board w/ `<ip>: <command>` (e.g. `10.0.1.99: off`). `quit` exits.

## Try it w/out a board

```console
embot-logger-emulator --port 19999 --name 10.0.1.99 --deco &
embot-logger-emulator --port 19998 --name 10.0.1.98 &
embot-logger --ip 127.0.0.1:19999 --ip 127.0.0.1:19998
```

The emulator follows `embot_svc_theLogger.cpp`: the same commands, replies and decoration; the lines are made up.

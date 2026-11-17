# embot/svc/host: the host tools of the embot services

| folder | service on the board | command | what |
|---|---|---|---|
| `tether/` | `embot::svc::theTETHERservice` (rpc 6666, stream 7777) | `tether` | ask / set / run on the Dictionary of a board, the plots of its stream |
| `logger/` | `embot::svc::theLogger` (9999) | `embot-logger` | the lines of `embot::core::print()` of one or more boards |

Each folder is a small Python package (`pyproject.toml`). It is installed once in a venv w/ `pip install -e` (editable):
the venv points to the files of the repo, so a `git pull` updates the tools w/out installing again. The firmware of the
boards does not change: the tools find out the Dictionary of a board by themselves.

The Dictionary of a board (`dictionary.toml`) is not here: it stays w/ the example of the board (e.g.
`board/amcmj1/examples/appl.gateway.01/host/tether`), and `tether` reads it from the current folder.

## Windows (PowerShell)

### 1. Python, once per PC

Python 3.11 or later (3.13 is fine) from python.org, w/ the launcher `py` (the default of the installer). Or:

```powershell
winget install Python.Python.3.13
py -0                                  # lists the installed Pythons: 3.13 must be there
```

### 2. A venv, once per PC

Outside the repo, so that git does not see it. Here `$HOME\venvs\embot` (i.e. `C:\Users\<you>\venvs\embot`):

```powershell
py -3.13 -m venv $HOME\venvs\embot
& $HOME\venvs\embot\Scripts\Activate.ps1           # the prompt starts w/ (embot)
```

If PowerShell says that running scripts is disabled, allow the local scripts once and activate again:

```powershell
Set-ExecutionPolicy -Scope CurrentUser RemoteSigned
```

### 3. The tools, once per venv

From the clone of the repo (here `C:\work\icub-firmware`; use yours):

```powershell
cd C:\work\icub-firmware\emBODY\eBcode\arch-arm\embot\svc\host
python -m pip install --upgrade pip
pip install -e .\tether -e .\logger
tether --help
embot-logger --help
```

`pip install -e` also installs `nicegui` (the pages). Install again only if a `pyproject.toml` changes (a new command).

### 4. Every time

Open a PowerShell, activate the venv, go to the folder of the board, run:

```powershell
& $HOME\venvs\embot\Scripts\Activate.ps1
cd C:\work\icub-firmware\emBODY\eBcode\arch-arm\board\amcmj1\examples\appl.gateway.01\host\tether

tether rpc --ip 10.0.1.99                  # page of variables and operations   -> http://localhost:8080
tether str --ip 10.0.1.99                  # page of the stream (another PowerShell) -> http://localhost:8081
tether rpc --ip 10.0.1.99 --cli            # the prompt: list, help, ask<...>, set[+ack]<...>, run[+ack]<...>, quit
```

For udpdemo-tether the folder is `board\amcmj1\examples\udpdemo-tether\host`. From any folder:
`tether rpc --ip 10.0.1.99 --dictionary <path>\dictionary.toml`.

W/out activating the venv: `& $HOME\venvs\embot\Scripts\tether.exe rpc --ip 10.0.1.99`.

The logger (its own PowerShell; on Windows `&` at the end does not put a command in the background):

```powershell
embot-logger --ip 10.0.1.99                # page -> http://localhost:8090
embot-logger --ip 10.0.1.99 --cli          # in the terminal
```

### 5. Network and firewall

- The PC needs an address in the network of the board, e.g. the Ethernet adapter w/ the static IP `10.0.1.104`, mask
  `255.255.255.0` (board `10.0.1.99`). Check w/ `ping 10.0.1.99`.
- The first time a tool receives UDP, Windows asks whether to allow Python: allow it on **private** networks. If the
  adapter of the board is classified *public* (an unidentified network), allow it there too, or set it private
  (Settings > Network > Ethernet > Network profile type). The stream (UDP 7777) and the logger (9999 w/ `--localport 9999`)
  do not arrive w/out it.
- `rpc` and `str` on the same PC: the board sends the stream to the PC that started it.

## WSL2 (Ubuntu)

```bash
sudo apt install python3 python3-venv
python3 -m venv ~/.venv
~/.venv/bin/pip install -e <repo>/emBODY/eBcode/arch-arm/embot/svc/host/tether -e <repo>/emBODY/eBcode/arch-arm/embot/svc/host/logger
source ~/.venv/bin/activate
cd <repo>/emBODY/eBcode/arch-arm/board/amcmj1/examples/appl.gateway.01/host/tether
tether rpc --ip 10.0.1.99 --no-open        # --no-open: no gio message; open http://localhost:8080 by hand
```

The stream reaches WSL2 only w/ mirrored networking: `networkingMode=mirrored` in `%UserProfile%\.wslconfig` (section
`[wsl2]`), then `wsl --shutdown`.

## Other tools that use the codec

The Speedgoat tools of `board/amcmj1/examples/appl.gateway.01/host` (`amc_gtw.py`, the daemon, the tests, `make_golden.py`,
`cubetether.py`) import `tethercodec.py`: from the venv if `tether` is installed there, else straight from
`embot/svc/host/tether` in the repo. They need no install of their own besides their `requirements.txt`.

## W/out installing

`python tether.py rpc --ip 10.0.1.99` from `tether\` works too (Python finds `tethercodec.py` next to it), but then the
Dictionary is read from and written to `tether\`: give `--dictionary`.

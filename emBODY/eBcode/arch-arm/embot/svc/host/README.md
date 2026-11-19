# embot/svc/host: the host tools of the embot services

The services and where to start, for every kind of reader: [../README.md](../README.md).

| folder | service on the board | command | what |
|---|---|---|---|
| `tether/` | `embot::svc::theTETHERservice` (rpc 6666, stream 7777) | `embot-tether` | ask / set / run on the Dictionary of a board, the plots of its stream |
| `logger/` | `embot::svc::theLogger` (9999) | `embot-logger` | the lines of `embot::core::print()` of one or more boards |

Only the commands, to copy and paste: [quick.startup.md](quick.startup.md). How to use each tool: `tether/README.md`,
`logger/README.md`.

Each folder is a small Python package (`pyproject.toml`). It is installed once in a venv w/ `pip install -e` (editable):
the venv points to the files of the clone of the repo, so an edit of a `.py` or a `git pull` takes effect at the next
run, w/out installing again. The firmware of the boards does not change: the tools find out the Dictionary of a board
by themselves. To be sure of which `.py` runs (e.g. w/ more clones of the repo), see
[Which files run](#which-files-run-one-venv-per-clone): one venv per clone, and `embot-tether --version`.

`embot-tether` reads the Dictionary of the board from `dictionary.toml` in the current folder, which is the folder of the
example of the board (e.g. `board/amcmj1/examples/appl.gateway.01/host/tether`), or from `--dictionary`. If the file is
not there, or it does not match the board, `embot-tether rpc` browses the board and writes it; `embot-tether str` only reads it,
and loads it again when `rpc` rewrites it.

The guide is for two systems, w/ the same steps:

- [Windows 11, PowerShell](#windows-11-powershell)
- [Ubuntu 24.04 or 26.04, in WSL2 or native](#ubuntu-2404-or-2604-in-wsl2-or-native)

The examples use the clone `C:\work\icub-firmware` (Windows) or `~/work/icub-firmware` (Ubuntu), the board `10.0.1.99`
and the PC `10.0.1.104`: use yours.

## Windows 11, PowerShell

### 1. Python, once per PC

Python 3.11 or later (3.13 is fine) from python.org, w/ the launcher `py` (the default of the installer). Or:

```powershell
winget install Python.Python.3.13
py -0                                  # lists the installed Pythons: 3.13 must be there
```

### 2. A venv, once per clone of the repo

Outside the repo, so that git does not see it, and named as the clone: the clone `C:\work\icub-firmware` gets the venv
`$HOME\venvs\icub-firmware` (i.e. `C:\Users\<you>\venvs\icub-firmware`). A second clone gets a second venv: see
[Which files run](#which-files-run-one-venv-per-clone).

```powershell
py -3.13 -m venv $HOME\venvs\icub-firmware
& $HOME\venvs\icub-firmware\Scripts\Activate.ps1   # the prompt starts w/ (icub-firmware)
```

If PowerShell says that running scripts is disabled, allow the local scripts once and activate again:

```powershell
Set-ExecutionPolicy -Scope CurrentUser RemoteSigned
```

### 3. The tools, once per venv

```powershell
cd C:\work\icub-firmware\emBODY\eBcode\arch-arm\embot\svc\host
python -m pip install --upgrade pip
pip install -e .\tether -e .\logger
embot-tether --version                            # the files that run: they must be in C:\work\icub-firmware
embot-logger --version
```

`pip install -e` also installs `nicegui` (the pages). Install again only if a `pyproject.toml` changes (a new command,
a new module) or if the clone moves to another folder.

### 4. Every time

Every PowerShell window needs the venv: activate it in each one (the prompt starts w/ `(icub-firmware)`).

```powershell
& $HOME\venvs\icub-firmware\Scripts\Activate.ps1
cd C:\work\icub-firmware\emBODY\eBcode\arch-arm\board\amcmj1\examples\appl.gateway.01\host\tether

embot-tether rpc --ip 10.0.1.99                  # page of variables and operations   -> http://localhost:8080
embot-tether str --ip 10.0.1.99                  # page of the stream (another window) -> http://localhost:8081
embot-tether rpc --ip 10.0.1.99 --cli            # the prompt: list, help, ask<...>, set[+ack]<...>, run[+ack]<...>, quit
```

For amcmj1-svc-demo the folder is `board\amcmj1\examples\amcmj1-svc-demo\host`. From any folder:
`embot-tether rpc --ip 10.0.1.99 --dictionary <path>\dictionary.toml`.

W/out activating the venv: `& $HOME\venvs\icub-firmware\Scripts\embot-tether.exe rpc --ip 10.0.1.99`.

The logger, in its own window (in PowerShell `&` at the end does not put a command in the background):

```powershell
embot-logger --ip 10.0.1.99                # page -> http://localhost:8090
embot-logger --ip 10.0.1.99 --cli          # in the terminal
```

### 5. Network and firewall

- The PC needs an address in the network of the board, e.g. the Ethernet adapter w/ the static IP `10.0.1.104`, mask
  `255.255.255.0` (board `10.0.1.99`). Check w/ `ping 10.0.1.99`.
- The first time a tool receives UDP, Windows asks whether to allow Python: allow it on **private** networks. If the
  adapter of the board is classified *public* (an unidentified network), allow it there too, or set it private
  (Settings > Network & internet > Ethernet > Network profile type). The stream (UDP 7777) and the logger (UDP 9999)
  do not arrive w/out it.
- `rpc` and `str` on the same PC: the board sends the stream to the PC that started it.

## Ubuntu 24.04 or 26.04, in WSL2 or native

### 1. Python, once per PC

Ubuntu 24.04 has Python 3.12, Ubuntu 26.04 Python 3.14: both fine (the tools need 3.11 or later). Neither allows
`pip install` outside a venv (PEP 668), so the venv is required. The commands are the same on both.

```bash
sudo apt update
sudo apt install python3 python3-venv python3-pip
python3 --version                          # 3.12 on 24.04, 3.14 on 26.04
```

In WSL2, `wsl --list --online` (in PowerShell) shows the names of the Ubuntu releases to give to `wsl --install -d`.

### 2. A venv, once per clone of the repo

Outside the repo, named as the clone: the clone `~/work/icub-firmware` gets the venv `~/venvs/icub-firmware`.

```bash
python3 -m venv ~/venvs/icub-firmware
source ~/venvs/icub-firmware/bin/activate   # the prompt starts w/ (icub-firmware)
```

In WSL2 the clone can be the one of Windows, under `/mnt/c`: e.g. `C:\ace\!mar\icub-firmware` is
`/mnt/c/ace/!mar/icub-firmware`. A `!` in a path must be in single quotes in bash (`cd '/mnt/c/ace/!mar/icub-firmware'`),
else bash takes it as a command of its history.

### 3. The tools, once per venv

```bash
cd ~/work/icub-firmware/emBODY/eBcode/arch-arm/embot/svc/host
pip install --upgrade pip
pip install -e ./tether -e ./logger
embot-tether --version                            # the files that run: they must be in ~/work/icub-firmware
embot-logger --version
```

Install again only if a `pyproject.toml` changes or if the clone moves to another folder.

### 4. Every time

Every terminal needs the venv: activate it in each one.

```bash
source ~/venvs/icub-firmware/bin/activate
cd ~/work/icub-firmware/emBODY/eBcode/arch-arm/board/amcmj1/examples/appl.gateway.01/host/tether

embot-tether rpc --ip 10.0.1.99 --no-open        # page -> http://localhost:8080, open it in the browser of Windows
embot-tether str --ip 10.0.1.99 --no-open        # another terminal -> http://localhost:8081
embot-tether rpc --ip 10.0.1.99 --cli            # the prompt
```

`--no-open`: WSL2 has no browser of its own (and w/out it a harmless `gio: ... Operation not supported` appears); the
browser of Windows reaches the pages at `localhost`. On a native Ubuntu w/ a desktop, leave it out.

W/out activating the venv: `~/venvs/icub-firmware/bin/embot-tether rpc --ip 10.0.1.99`.

The logger:

```bash
embot-logger --ip 10.0.1.99 --no-open      # page -> http://localhost:8090
embot-logger --ip 10.0.1.99 --cli          # in the terminal
```

### 5. Network

- Native Ubuntu: the PC needs an address in the network of the board (e.g. `10.0.1.104/24`). Check w/ `ping 10.0.1.99`.
- WSL2 uses the network of Windows (see the Windows firewall above, for the Python of WSL2 too). `rpc` works w/ the
  default networking, but the stream (UDP 7777) and the logger (UDP 9999) reach WSL2 only w/ **mirrored** networking: in
  `%UserProfile%\.wslconfig` of Windows

  ```ini
  [wsl2]
  networkingMode=mirrored
  ```

  then `wsl --shutdown` in PowerShell, and open Ubuntu again.

## Which files run: one venv per clone

`pip install -e` links a venv to the folder of one clone. `embot-tether` and `embot-logger` run the `.py` of that clone, as
they are on disk now: an edit or a `git pull` there is used at the next run, an edit in another clone is not.

So: one venv per clone, named as the clone, and activate the one of the clone you work in. E.g. for a second clone
`icub-firmware-000`:

```powershell
# Windows, once
py -3.13 -m venv $HOME\venvs\icub-firmware-000
& $HOME\venvs\icub-firmware-000\Scripts\Activate.ps1
cd C:\work\icub-firmware-000\emBODY\eBcode\arch-arm\embot\svc\host
pip install -e .\tether -e .\logger
# Windows, every day: the venv of the clone of the day
& $HOME\venvs\icub-firmware-000\Scripts\Activate.ps1
```

```bash
# Ubuntu, once
python3 -m venv ~/venvs/icub-firmware-000
source ~/venvs/icub-firmware-000/bin/activate
cd ~/work/icub-firmware-000/emBODY/eBcode/arch-arm/embot/svc/host
pip install -e ./tether -e ./logger
# Ubuntu, every day: the venv of the clone of the day
source ~/venvs/icub-firmware-000/bin/activate
```

To check, any time (same on both):

```
embot-tether --version                    # the full path of tether.py and tethercodec.py, and the protocol
embot-logger --version              # the full path of logger.py
pip show embot-tether               # "Editable project location": the folder of the clone
```

Every run also prints the path of the files in its first line (`[info] tether.py .../tether.py, ...`).

What to do when:

| change | what to do |
|---|---|
| an edit of a `.py`, a `git pull`, a checkout of another branch in the same clone | nothing: stop the tool and start it again (and reload the page in the browser) |
| work in another clone | activate the venv of that clone (create it once, as above) |
| a `pyproject.toml` changes (a new command, a new `.py` module, a new dependency) | `pip install -e` again, in the venv of that clone |
| the clone moves to another folder | `pip install -e` again from the new folder (or a new venv) |
| `embot-tether` says `No module named 'tether'` | the venv points to a folder that is not there any more: `pip install -e` again |

`dictionary.toml` does not depend on the venv: `embot-tether` reads it from the folder where it runs (or from `--dictionary`).

## Other tools that use the codec

The Speedgoat tools of `board/amcmj1/examples/appl.gateway.01/host` (`amc_gtw.py`, the daemon, the tests, `make_golden.py`,
`cubetether.py`) import `tethercodec.py`: from the venv if `embot-tether` is installed there, else straight from
`embot/svc/host/tether` in the repo. They need no install of their own besides their `requirements.txt`.

## W/out installing

`python tether.py rpc --ip 10.0.1.99` (Windows: `py tether.py ...`) from the folder `tether` works too: Python finds
`tethercodec.py` next to it, and the pages need `nicegui` in that Python. But then the Dictionary is read from and
written to the folder `tether`: give `--dictionary`.

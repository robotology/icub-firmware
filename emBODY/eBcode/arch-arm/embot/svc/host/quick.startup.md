# Quick startup: embot-tether and embot-logger

Only the commands. The why, the network and the firewall: [README.md](README.md). Change the folder of the clone
(`C:\work\icub-firmware`, `~/work/icub-firmware`) and the IP of the board (`10.0.1.99`) to yours.

## Windows 11, PowerShell

Once per PC:

```powershell
winget install Python.Python.3.13
Set-ExecutionPolicy -Scope CurrentUser RemoteSigned
```

Once per clone:

```powershell
py -3.13 -m venv $HOME\venvs\icub-firmware
& $HOME\venvs\icub-firmware\Scripts\Activate.ps1
cd C:\work\icub-firmware\emBODY\eBcode\arch-arm\embot\svc\host
python -m pip install --upgrade pip
pip install -e .\tether -e .\logger
embot-tether --version
```

Every time, one window each:

```powershell
# window 1: rpc
& $HOME\venvs\icub-firmware\Scripts\Activate.ps1
cd C:\work\icub-firmware\emBODY\eBcode\arch-arm\board\amcmj1\examples\udpdemo-tether\host
embot-tether rpc --ip 10.0.1.99
```

```powershell
# window 2: the stream
& $HOME\venvs\icub-firmware\Scripts\Activate.ps1
cd C:\work\icub-firmware\emBODY\eBcode\arch-arm\board\amcmj1\examples\udpdemo-tether\host
embot-tether str --ip 10.0.1.99
```

```powershell
# window 3: the log
& $HOME\venvs\icub-firmware\Scripts\Activate.ps1
embot-logger --ip 10.0.1.99
```

Pages: rpc http://localhost:8080, stream http://localhost:8081, log http://localhost:8090.

## Ubuntu 24.04 or 26.04, WSL2 or native

Once per PC:

```bash
sudo apt update
sudo apt install python3 python3-venv python3-pip
```

WSL2 only, for the stream and the log: in `%UserProfile%\.wslconfig` of Windows put the two lines below, then
`wsl --shutdown` in PowerShell and open Ubuntu again.

```ini
[wsl2]
networkingMode=mirrored
```

Once per clone:

```bash
python3 -m venv ~/venvs/icub-firmware
source ~/venvs/icub-firmware/bin/activate
cd ~/work/icub-firmware/emBODY/eBcode/arch-arm/embot/svc/host
pip install --upgrade pip
pip install -e ./tether -e ./logger
embot-tether --version
```

Every time, one terminal each:

```bash
# terminal 1: rpc
source ~/venvs/icub-firmware/bin/activate
cd ~/work/icub-firmware/emBODY/eBcode/arch-arm/board/amcmj1/examples/udpdemo-tether/host
embot-tether rpc --ip 10.0.1.99 --no-open
```

```bash
# terminal 2: the stream
source ~/venvs/icub-firmware/bin/activate
cd ~/work/icub-firmware/emBODY/eBcode/arch-arm/board/amcmj1/examples/udpdemo-tether/host
embot-tether str --ip 10.0.1.99 --no-open
```

```bash
# terminal 3: the log
source ~/venvs/icub-firmware/bin/activate
embot-logger --ip 10.0.1.99 --no-open
```

Pages, in the browser (of Windows, for WSL2): rpc http://localhost:8080, stream http://localhost:8081, log
http://localhost:8090. On a native Ubuntu w/ a desktop, leave out `--no-open` and the browser opens by itself.

## The gateway

The same, from the folder of its Dictionary: `...\appl.gateway.01\host\tether` (Windows),
`.../appl.gateway.01/host/tether` (Ubuntu).

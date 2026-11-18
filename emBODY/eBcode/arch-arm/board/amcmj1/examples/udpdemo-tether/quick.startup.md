# Quick startup: udpdemo-tether

Only the commands. The why: [README.md](README.md). Board `10.0.1.99`, PC `10.0.1.104`. Change the folder of the clone
(`C:\work\icub-firmware`, `~/work/icub-firmware`) to yours.

Before: the board w/ `amcmj1.udpdemo.tether` (uVision: `proj/amcmj1.udpdemo.tether.uvprojx`, target
`amcmj1.2cm4-master`), and the host tools installed once (`embot/svc/host/quick.startup.md`).

## Windows 11, PowerShell

```powershell
# window 1: TETHER rpc -> http://localhost:8080
& $HOME\venvs\icub-firmware\Scripts\Activate.ps1
cd C:\work\icub-firmware\emBODY\eBcode\arch-arm\board\amcmj1\examples\udpdemo-tether\host
embot-tether rpc --ip 10.0.1.99
```

```powershell
# window 2: TETHER stream -> http://localhost:8081
& $HOME\venvs\icub-firmware\Scripts\Activate.ps1
cd C:\work\icub-firmware\emBODY\eBcode\arch-arm\board\amcmj1\examples\udpdemo-tether\host
embot-tether str --ip 10.0.1.99
```

```powershell
# window 3: the log -> http://localhost:8090
& $HOME\venvs\icub-firmware\Scripts\Activate.ps1
embot-logger --ip 10.0.1.99
```

```powershell
# window 4: one echo, then traffic on the echo (ctrl-c to stop) to fill IDviewof_latency
$u = New-Object System.Net.Sockets.UdpClient; $u.Client.ReceiveTimeout = 1000
[void]$u.Send([Text.Encoding]::ASCII.GetBytes("hello"), 5, "10.0.1.99", 666)
$ep = New-Object System.Net.IPEndPoint([Net.IPAddress]::Any, 0)
[Text.Encoding]::ASCII.GetString($u.Receive([ref]$ep))
$b = [byte[]](0x78)
while ($true) { [void]$u.Send($b, 1, "10.0.1.99", 666); Start-Sleep -Milliseconds 10 }
```

## Ubuntu 24.04 or 26.04, WSL2 or native

```bash
# terminal 1: TETHER rpc -> http://localhost:8080
source ~/venvs/icub-firmware/bin/activate
cd ~/work/icub-firmware/emBODY/eBcode/arch-arm/board/amcmj1/examples/udpdemo-tether/host
embot-tether rpc --ip 10.0.1.99 --no-open
```

```bash
# terminal 2: TETHER stream -> http://localhost:8081
source ~/venvs/icub-firmware/bin/activate
cd ~/work/icub-firmware/emBODY/eBcode/arch-arm/board/amcmj1/examples/udpdemo-tether/host
embot-tether str --ip 10.0.1.99 --no-open
```

```bash
# terminal 3: the log -> http://localhost:8090
source ~/venvs/icub-firmware/bin/activate
embot-logger --ip 10.0.1.99 --no-open
```

```bash
# terminal 4: one echo, then traffic on the echo (ctrl-c to stop) to fill IDviewof_latency
echo -n "hello" | nc -u -w1 10.0.1.99 666
while true; do echo -n x | nc -u -w0 10.0.1.99 666; sleep 0.01; done
```

WSL2: the stream and the log need mirrored networking (`embot/svc/host/quick.startup.md`).

## In the page of rpc (or w/ `embot-tether rpc --ip 10.0.1.99 --cli`)

```
ask<IDechoSTATS>
ask<IDviewof_latency>
set[+ack]<IDechoCONFIG, {marker=42, enabled=true, uppercase=true}>
set[+ack]<MGstreamADD, IDsignal>; set[+ack]<MGstreamADD, IDviewof_latency>; set[+ack]<MGstreamSTART, 20>
```

In the page: **ASK** on a variable, **SET** on a RW one, **RUN** on an operation, tick **stream** and **START** to see
the variables in the page of the stream.

# tether: the Dictionary of amc.gtw

`dictionary.toml` holds the variables and operations of amc.gtw, as `embot-tether` browses them from the board. The tool itself,
`tether.py` w/ its codec `tethercodec.py`, is the host of the TETHER service and lives w/ it, in `embot/svc/host/tether`:
see the `README.md` there for the install (Windows 11, Ubuntu 24.04 / 26.04) and the commands.

Run it from this folder, so that it reads and writes this `dictionary.toml`, w/ the venv of the tools active.

Windows 11, PowerShell:

    cd <repo>\emBODY\eBcode\arch-arm\board\amcmj1\examples\appl.gateway.01\host\tether
    embot-tether rpc --ip 10.0.1.99            # ask / set / run: variables and operations (port 6666)
    embot-tether str --ip 10.0.1.99            # the stream of the variables (port 7777), another window

Ubuntu 24.04 or 26.04 (WSL2 or native):

    cd <repo>/emBODY/eBcode/arch-arm/board/amcmj1/examples/appl.gateway.01/host/tether
    embot-tether rpc --ip 10.0.1.99 --no-open  # then http://localhost:8080
    embot-tether str --ip 10.0.1.99 --no-open  # another terminal, then http://localhost:8081

If `dictionary.toml` does not match the board, `embot-tether rpc` browses the board and rewrites it (the old one is kept as
`.bak`): commit the new one when the Dictionary of the firmware changes.

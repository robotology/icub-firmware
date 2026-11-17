# tether: the Dictionary of amc.gtw

`dictionary.toml` holds the variables and operations of amc.gtw, as `tether` browses them from the board. The tool itself,
`tether.py` w/ its codec `tethercodec.py`, is the host of the TETHER service and lives w/ it, in `embot/svc/host/tether`:
see the `README.md` there for the install (Windows, WSL2) and the commands.

Run it from this folder, so that it reads and writes this `dictionary.toml`:

    cd <repo>\emBODY\eBcode\arch-arm\board\amcmj1\examples\appl.gateway.01\host\tether
    tether rpc --ip 10.0.1.99            # ask / set / run: variables and operations (port 6666)
    tether str --ip 10.0.1.99            # the stream of the variables (port 7777)

If `dictionary.toml` does not match the board, `tether rpc` browses the board and rewrites it (the old one is kept as
`.bak`): commit the new one when the Dictionary of the firmware changes.

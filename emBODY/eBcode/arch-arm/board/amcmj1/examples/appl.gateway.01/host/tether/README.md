# tether: the generic TETHER client

tether.py (rpc, str), tethercodec.py (the protocol) and dictionary.toml (the variables and operations of the board).
Run it in WSL2, from this folder (the dictionary is read from ./dictionary.toml; if it does not match the board, the board's own is browsed and written here):

    ~/.venv/bin/python tether.py rpc --ip 10.0.1.99 --no-open     # ask / set / run: variables and operations (port 6666)
    ~/.venv/bin/python tether.py str --ip 10.0.1.99 --no-open     # streaming of variables (port 7777)

Independent of the other folders: the emulators import tethercodec.py from here.

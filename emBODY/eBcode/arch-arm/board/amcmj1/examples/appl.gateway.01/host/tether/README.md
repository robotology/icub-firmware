# tether: the generic TETHER client

tether.py (rpc, str) and dictionary.toml (the variables and operations of the board). The protocol, tethercodec.py, is in
`../shared`: tether.py finds it there, or in its own folder if the two files are copied together.
Run it in WSL2, from this folder (the dictionary is read from ./dictionary.toml; if it does not match the board, the board's own is browsed and written here):

    ~/.venv/bin/python tether.py rpc --ip 10.0.1.99 --no-open     # ask / set / run: variables and operations (port 6666)
    ~/.venv/bin/python tether.py str --ip 10.0.1.99 --no-open     # streaming of variables (port 7777)

It needs only `../shared/tethercodec.py`, which the other tools of `host` import too.

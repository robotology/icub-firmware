#!/usr/bin/env python3
# tether.py of udpdemo-tether: it runs the generic client of TETHER, appl.gateway.01/host/tether/tether.py, which uses the
# one tethercodec.py of appl.gateway.01/host/shared. it is here so that the commands of README.md work from this folder,
# w/ ./dictionary.toml as the Dictionary of this board. all the options are the ones of the generic client.

import os
import runpy
import sys

_TOOL = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                      '..', '..', 'appl.gateway.01', 'host', 'tether', 'tether.py'))

if __name__ == '__main__':
    if not os.path.isfile(_TOOL):
        sys.exit(f'[error] the client of TETHER is not at {_TOOL}')
    sys.argv[0] = _TOOL
    runpy.run_path(_TOOL, run_name='__main__')

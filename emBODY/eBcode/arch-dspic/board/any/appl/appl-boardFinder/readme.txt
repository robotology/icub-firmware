app-boardfinder is a very simple application.

It recognizes only these messages of canLoader calss of icub-can-protocol:
	- CMD_BROADCAST: it returns boardtype=2foc and version=5, Release=5 and build=5 in order to be reconized by human user by CanLoader application
	- CMD_BOARD: usual meaning
	- CMD_FINDBOARD: this message is new and is mangaed by this application only. it folows icub-canprotocol  class Canloader fomattinf rules.

The command CMD_FINDBOARD:

  - id = 0x70F or 0x700|(BOARD_ID); payload_data[0]= CMD_FINDBOARD = 0xFE, payload_data[1] = 0 ==> switch off led yellow; led red switch on.

  - id = 0x70F or 0x700|(BOARD_ID); payload_data[0]= CMD_FINDBOARD = 0xFE, payload_data[1] = 1 ==> led yellow and red link very slowly.

This command can be send by canReal.




Pay attention: this application is not tested on 6sg and prbably it should port on 6sg painless.
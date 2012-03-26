
-----------------------------------------------------------------------------------------------------------------------
- Hardware Abstraction Layer
-----------------------------------------------------------------------------------------------------------------------


The HAL is an abstraction layer to the HW.


-----------------------------------------------------------------------------------------------------------------------
- HOW TO INSTALL
-----------------------------------------------------------------------------------------------------------------------



-----------------------------------------------------------------------------------------------------------------------
- HOW TO USE
-----------------------------------------------------------------------------------------------------------------------

Include the following into your project:
- files hal.lib, hal_startup.s
- a section hal-cfg with files: hal_cfg.c, hal_cfg.h

Ethernet:
if you are interested to use ethernet, then you have to set ethernet enable in hal config.
You can plug Ethernet cable when you want: you can start with no cables connected and during run time you can plug and
unplug cables. 
The driver tries to use autonegotiation to establish link capability.
If the autonegationtion is not completed, then the link is forced to 100Mb full duplex
----------------------------------------------------------------------------------------------------------------------
- FUTURE DEVELOPMENT
----------------------------------------------------------------------------------------------------------------------

tbd




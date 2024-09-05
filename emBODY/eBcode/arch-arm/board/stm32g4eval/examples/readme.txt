The projects are:
- basic: 
  contains the simplest project using bare metal approach and inclusion of stm32hal library.
  it has a stlink target and a ulink pro target
- can-btl + can-app:
  they work together, being are placed in flash in different addresses
  btl executes at reset and after 5 seconds attempts a jump to app. they use can to process CAN frames (dont remember which pins are CAN)
- embot-os:
  it is a demo project w/ embot::os. it has one event based thread activated by a tick event sent by a embot::os:Timer (and one peridiodic thread).
  it also has several HW tests. the active one is about the TLV reading over I2C. That needs a FAP board connected to some pins of the connecteor 
- embot-os2:
  test project for usage of cmsisos2. it may not compile   
  
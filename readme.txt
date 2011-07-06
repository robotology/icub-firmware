This folder contains software for all boards on iCub.

The iCub board software is composed of the firmware layer, colled also Hardware Abstraction Layer, and application layer.
The first layer provides services (such as peripherical management, ..) to the second one;
the application layer implements motor control or sensors reading.


This folder is structured as follows:

  - build --> contains output files to flash on boards
  - build_testing --> contains testing output files to flash on boards

  - loader56f807 --> boot loader for boards with DSP56f807
  - loader56f807NoSleep --> boot loader for boards with DSP56f807 without sleep of 5 seconds
  - loaderDspic30f4011 --> boot loader for boards with DSPic30f4011 (skin (i.e. mtb3) )
  - loaderDspic30f4013 --> boot loader for boards with DSPic30f4013 (strain, mais)

  - libDsp56f807 --> libraries (source code, progect files and .lib) that supply services to motor controller applications (folder MotorControllerDsp56f807)
  - libDspic30f4013-HAL --> libraries (source code, progect file and .lib) that supply services to sensor applications (Mais, Strain)

  - MotorControllerDsp56f807 --> motor controller applications, one for each motor-board.

  - sensorReaderDspic30f4013 --> sensor reader application: Mais and Strain

  - mtb3 --> "skin - application"





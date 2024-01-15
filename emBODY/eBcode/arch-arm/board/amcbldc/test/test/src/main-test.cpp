
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_app_skeleton_os_bootloader.h"

// --------------------------------------------------------------------------------------------------------------------
// bootloader info

constexpr uint8_t minor = 
#if defined(STM32HAL_DRIVER_V120)
    1;
#else
    2;
#endif


constexpr std::uint8_t defADDRESS = 1;
constexpr embot::app::theCANboardInfo::bootloaderInfo btlInfo 
{ 
    embot::prot::can::Board::amcbldc, 
    embot::prot::can::versionOfBOOTLOADER {1, minor}, 
    defADDRESS,                                                  
    "I am an amcbldc - test FW" 
};

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    embot::app::skeleton::os::bootloader::run(btlInfo);
    for(;;);    
}
   
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------






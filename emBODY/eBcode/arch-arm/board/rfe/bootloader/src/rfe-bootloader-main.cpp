
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_app_skeleton_os_bootloader.h"

// --------------------------------------------------------------------------------------------------------------------
// bootloader info

constexpr std::uint8_t defADDRESS = 1;
constexpr embot::app::theCANboardInfo::bootloaderInfo btlInfo 
{ 
    embot::prot::can::Board::rfe, 
    embot::prot::can::versionOfBOOTLOADER {1, 8}, 
    defADDRESS,                                                  
    "I am a beautiful rfe" 
};

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    embot::app::skeleton::os::bootloader::run(btlInfo);
    for(;;);    
}
   
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------






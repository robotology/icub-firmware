
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_code_bootloader.h"

// --------------------------------------------------------------------------------------------------------------------
// bootloader info

constexpr std::uint8_t defADDRESS = 1;
constexpr embot::app::theCANboardInfo::bootloaderInfo btlInfo 
{ 
    embot::prot::can::Board::psc, 
    embot::prot::can::versionOfBOOTLOADER {1, 4}, 
    defADDRESS,                                                       
    "I am a psc" 
};

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    embot::code::bootloader::run(btlInfo);
    for(;;);    
}
   
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


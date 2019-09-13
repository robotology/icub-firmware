
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_code_bootloader.h"

// --------------------------------------------------------------------------------------------------------------------
// bootloader info

constexpr std::uint8_t defADDRESS = 12;
constexpr embot::app::theCANboardInfo::bootloaderInfo btlInfo 
{ 
    embot::app::canprotocol::Board::sg3, 
    embot::app::canprotocol::versionOfBOOTLOADER {1, 3}, 
    defADDRESS,                                                  
    "I am a sg3" 
};

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    embot::code::bootloader::run(btlInfo);
    for(;;);    
}
   
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

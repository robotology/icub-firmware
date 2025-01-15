
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#ifndef __EMBOT_APP_BOARDS_H_
#define __EMBOT_APP_BOARDS_H_


#include "embot_core.h"


namespace embot::app::boards {
  
    enum class Board : uint8_t { 
    
        mtb = 5, strain = 6, mais = 7, mtb4 = 11, strain2 = 12, rfe = 13, sg3 = 14, psc = 15, mtb4w = 16, 
        pmc = 17, amcbldc = 18, bms = 19, mtb4c = 20, amc2c = 21, strain2c = 22, bat = 23, amcfoc2c = 24,

        ems4 = 32, mc4plus = 33, mc2plus = 34, amc = 35, amcfoc = 36,
        
        none = 254,
        unknown = 255
    };
    
    
    constexpr const char * to_constchar(Board b)
    {
        const char *c = "unknown"; 
        switch(b)
        {
            case Board::mtb:        { c = "mtb"; } break;
            case Board::strain:     { c = "strain"; } break;
            case Board::mais:       { c = "mais"; } break;
            case Board::mtb4:       { c = "mtb4"; } break;
            case Board::strain2:    { c = "strain2"; } break;
            case Board::rfe:        { c = "rfe"; } break;
            case Board::sg3:        { c = "sg3"; } break;
            case Board::psc:        { c = "psc"; } break;
            case Board::mtb4w:      { c = "mtb4w"; } break;
            case Board::pmc:        { c = "pmc"; } break;
            case Board::amcbldc:    { c = "amcbldc"; } break;
            case Board::mtb4c:      { c = "mtb4c"; } break;
            case Board::amc2c:      { c = "amc2c"; } break;
            case Board::strain2c:   { c = "strain2c"; } break;
            case Board::bat:        { c = "bat"; } break;
            case Board::amcfoc2c:   { c = "amcfoc2c"; } break;
            case Board::ems4:       { c = "ems4"; } break;
            case Board::mc4plus:    { c = "mc4plus"; } break;
            case Board::mc2plus:    { c = "mc2plus"; } break;
            case Board::amc:        { c = "amc"; } break;
            case Board::amcfoc:     { c = "amcfoc"; } break;
            case Board::none:       { c = "none"; } break;
            
            default:                { c = "unknown"; } break;            
        }
        return c;
    }
    
}

 
#endif  // include-guard



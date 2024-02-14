
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------


#include "embot_msg.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - implementation 
// --------------------------------------------------------------------------------------------------------------------
 
 
namespace embot::msg {

//    void fill(Location &loc, const eObrd_location_t &l)
//    {
//        uint8_t place {l.any.place};
//        if((eobrd_place_can == place) || (eobrd_place_extcan == place))
//        {   // i decide to lose the .index (0 or 1) because it is barely used             
//            eOlocation_t ll {l.can.port, l.can.addr};
//            loc = {&ll};              
//        }
//        else if(eobrd_place_eth == place)
//        {   // id is 6 bits, so in range [0, 63].   
//            eOlocation_t ll {eobus_local, l.eth.id};
//            loc = {&ll};             
//        }         
//    }
    
    void fill(Location &loc, const eOlocation_t &l)
    {
        loc = {&l};
    }  

    Location transform(const eOlocation_t &l)
    {
        Location r {&l};
        return r;
    }   

//    eOlocation_t transform(const Location &l)
//    {
//        eOlocation_t r {};
//        r.bus = embot::core::tointegral(l.getbus()); 
//        r.adr = l.getadr();
//        r.ffu = l.ffu;
//        return r;
//    }    
     
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



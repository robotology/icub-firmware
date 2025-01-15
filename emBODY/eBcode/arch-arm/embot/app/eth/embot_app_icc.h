
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#ifndef __EMBOT_APP_ICC_H_
#define __EMBOT_APP_ICC_H_


#include "embot_core.h"
#include "embot_app_msg.h"
#include "embot_app_boards.h"
#include "embot_app_eth.h"


namespace embot::app::icc {
  
    // it is not the most general data structure
    struct Signature
    {
        embot::app::boards::Board board {embot::app::boards::Board::none};
        embot::app::msg::Location location {embot::app::msg::BUS::none, 0};
        embot::app::eth::Version2 application {0, 0, 0, 0};
        embot::app::eth::Version protocol {0, 0};
        embot::app::eth::Date date {2022, embot::app::eth::Month::Apr, embot::app::eth::Day::thirteen, 16, 40};
        constexpr Signature() = default;
        constexpr Signature(embot::app::boards::Board b, 
                            const embot::app::msg::Location &l, 
                            const embot::app::eth::Version2 &a, 
                            const embot::app::eth::Version &p,
                            const embot::app::eth::Date &d)
                            : board(b), location(l), application(a), protocol(p), date(d) {};
                                
        std::string to_string() const
        {
            return std::string(embot::app::boards::to_constchar(board)) + " @ " + location.to_string() 
                + ", vers " + application.to_string() + ", prot " + protocol.to_string() + ", date " + date.to_string();
        }                                  
    };
    
    static_assert(sizeof(Signature) == 16, "Signature is not of correct size");
    
}

 
#endif  // include-guard





/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_ICC_ITEMCANFRAME_H_
#define __EMBOT_APP_ETH_ICC_ITEMCANFRAME_H_

#include "embot_core.h"
#include "embot_app_eth.h"
#include "embot_app_eth_theICCservice.h"

#include "embot_app_msg.h"
#include "embot_prot_can.h" // to see embot::prot::can::Frame


namespace embot::app::eth::icc {
     
    
    struct ItemCANframe : public Item
    {
        embot::app::msg::Location des {};
        embot::prot::can::Frame frame {};
            
        constexpr ItemCANframe() = default;
            
        constexpr ItemCANframe(const embot::app::msg::Location &d, const embot::prot::can::Frame &f) : des(d), frame(f)
        {
            type = ItemType::CANframe;
            std::memset(filler, 0, sizeof(filler));
            std::memmove(&data[0], &des, sizeof(des));
            std::memmove(&data[4], &frame, sizeof(frame));
        } 
        
        constexpr ItemCANframe(const Item &i)
        {
            load(i);                 
        } 
        
        void load(const Item &i)
        {
            if(ItemType::CANframe == i.type)
            {
                type = ItemType::CANframe;
                std::memmove(filler, i.filler, sizeof(filler));
                std::memmove(data, i.data, sizeof(data));
                // and now des and frame
                std::memmove(&des, &data[0], sizeof(des));
                std::memmove(&frame, &data[4], sizeof(frame));  
            }              
        }
                    
        Item item() const
        {
            Item i {};
            i.type = type;
            std::memmove(i.filler, filler, sizeof(filler));
            std::memmove(i.data, data, sizeof(data));
            return i;                    
        }

    };        


} // namespace embot::app::eth:icc {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_APP_APPLICATION_THECANPARSERSTRAIN_H_
#define _EMBOT_APP_APPLICATION_THECANPARSERSTRAIN_H_

#include <vector>
#include <memory>
#include "embot_core.h"
#include "embot_prot_can.h"
#include "embot_prot_can_analog_polling.h"
#include "embot_prot_can_analog_periodic.h"


namespace embot { namespace app { namespace application {
    
    class CANagentSTRAIN
    {
    public:
        // interface
        virtual bool set(const embot::prot::can::analog::polling::Message_SET_TXMODE::Info &info) = 0;
        virtual bool set(const embot::prot::can::analog::polling::Message_SET_SERIAL_NO::Info &info) = 0;  
        virtual bool set(const embot::prot::can::analog::polling::Message_SET_CANDATARATE::Info &info) = 0; 
        virtual bool set(const embot::prot::can::analog::polling::Message_SET_FULL_SCALES::Info &info) = 0; 
        virtual bool set(const embot::prot::can::analog::polling::Message_SAVE2EE::Info &info) = 0; 
        virtual bool set(const embot::prot::can::analog::polling::Message_SET_CH_DAC::Info &info) = 0; 
        virtual bool set(const embot::prot::can::analog::polling::Message_SET_MATRIX_RC::Info &info) = 0; 
        virtual bool set(const embot::prot::can::analog::polling::Message_SET_MATRIX_G::Info &info) = 0; 
        virtual bool set(const embot::prot::can::analog::polling::Message_SET_CALIB_TARE::Info &info) = 0; 
        virtual bool set(const embot::prot::can::analog::polling::Message_SET_CURR_TARE::Info &info) = 0; 
        virtual bool set(const embot::prot::can::analog::polling::Message_AMPLIFIER_RESET::Info &info) = 0; 
        virtual bool set(const embot::prot::can::analog::polling::Message_AMPLIFIER_PGA308_CFG1_SET::Info &info) = 0; 
        virtual bool set(const embot::prot::can::analog::polling::Message_AMPLIFIER_OFFSET_AUTOCALIB::Info &info, embot::prot::can::analog::polling::Message_AMPLIFIER_OFFSET_AUTOCALIB::ReplyInfo &replyinfo) = 0; 
        virtual bool set(const embot::prot::can::analog::polling::Message_AMPLIFIER_GAINOFFSET_SET::Info &info) = 0; 
        virtual bool set(const embot::prot::can::analog::polling::Message_REGULATIONSET_SET::Info &info) = 0; 
  
        virtual bool get(const embot::prot::can::analog::polling::Message_GET_SERIAL_NO::Info &info, embot::prot::can::analog::polling::Message_GET_SERIAL_NO::ReplyInfo &replyinfo) = 0;
        virtual bool get(const embot::prot::can::analog::polling::Message_GET_FULL_SCALES::Info &info, embot::prot::can::analog::polling::Message_GET_FULL_SCALES::ReplyInfo &replyinfo) = 0;
        virtual bool get(const embot::prot::can::analog::polling::Message_GET_EEPROM_STATUS::Info &info, embot::prot::can::analog::polling::Message_GET_EEPROM_STATUS::ReplyInfo &replyinfo) = 0;
        virtual bool get(const embot::prot::can::analog::polling::Message_GET_CH_ADC::Info &info, embot::prot::can::analog::polling::Message_GET_CH_ADC::ReplyInfo &replyinfo) = 0;
        virtual bool get(const embot::prot::can::analog::polling::Message_GET_CH_DAC::Info &info, embot::prot::can::analog::polling::Message_GET_CH_DAC::ReplyInfo &replyinfo) = 0;
        virtual bool get(const embot::prot::can::analog::polling::Message_GET_MATRIX_RC::Info &info, embot::prot::can::analog::polling::Message_GET_MATRIX_RC::ReplyInfo &replyinfo) = 0;
        virtual bool get(const embot::prot::can::analog::polling::Message_GET_MATRIX_G::Info &info, embot::prot::can::analog::polling::Message_GET_MATRIX_G::ReplyInfo &replyinfo) = 0;
        virtual bool get(const embot::prot::can::analog::polling::Message_GET_CALIB_TARE::Info &info, embot::prot::can::analog::polling::Message_GET_CALIB_TARE::ReplyInfo &replyinfo) = 0;
        virtual bool get(const embot::prot::can::analog::polling::Message_GET_CURR_TARE::Info &info, embot::prot::can::analog::polling::Message_GET_CURR_TARE::ReplyInfo &replyinfo) = 0;
        virtual bool get(const embot::prot::can::analog::polling::Message_AMPLIFIER_PGA308_CFG1_GET::Info &info, embot::prot::can::analog::polling::Message_AMPLIFIER_PGA308_CFG1_GET::ReplyInfo &replyinfo) = 0;
        virtual bool get(const embot::prot::can::analog::polling::Message_AMPLIFIER_GAINOFFSET_GET::Info &info, embot::prot::can::analog::polling::Message_AMPLIFIER_GAINOFFSET_GET::ReplyInfo &replyinfo) = 0;
        virtual bool get(const embot::prot::can::analog::polling::Message_AMPLIFIER_RANGE_OF_GAIN_GET::Info &info, embot::prot::can::analog::polling::Message_AMPLIFIER_RANGE_OF_GAIN_GET::ReplyInfo &replyinfo) = 0;
        virtual bool get(const embot::prot::can::analog::polling::Message_AMPLIFIER_RANGE_OF_OFFSET_GET::Info &info, embot::prot::can::analog::polling::Message_AMPLIFIER_RANGE_OF_OFFSET_GET::ReplyInfo &replyinfo) = 0;
        virtual bool get(const embot::prot::can::analog::polling::Message_REGULATIONSET_GET::Info &info, embot::prot::can::analog::polling::Message_REGULATIONSET_GET::ReplyInfo &replyinfo) = 0;
       
    public:
        virtual ~CANagentSTRAIN() {};         
    };
           
    class theCANparserSTRAIN
    {
    public:
        static theCANparserSTRAIN& getInstance();
        
        
    public:

        struct Config
        {
            CANagentSTRAIN* agent {nullptr};
            Config() = default;
            Config(CANagentSTRAIN* a) : agent(a) {}
            bool isvalid() const { return (nullptr == agent) ? false : true; }
        }; 
        
        
        bool initialise(const Config &config); 
        
        // returns true if the canframe has been recognised. if so, any reply is sent if replies.size() > 0
        bool process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);

    private:
        theCANparserSTRAIN(); 
        ~theCANparserSTRAIN();


    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;    
    };       


}}} // namespace embot { namespace app { namespace application


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

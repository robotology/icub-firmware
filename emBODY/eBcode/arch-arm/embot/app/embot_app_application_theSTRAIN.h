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

#ifndef _EMBOT_APP_APPLICATION_THESTRAIN_H_
#define _EMBOT_APP_APPLICATION_THESTRAIN_H_

#include "embot_common.h"
#include "embot_hw.h"
#include "embot_sys.h"
#include "embot_app_application_theCANparserSTRAIN.h"
#include "embot_hw_pga308.h"
#include <vector>
#include <memory>

namespace embot { namespace app { namespace application {
           
    class theSTRAIN : public CANagentSTRAIN
    {
    public:
        static theSTRAIN& getInstance();
        
        
    public:
        struct Config
        {
            embot::common::Event        tickevent;
            embot::common::Event        datareadyevent;
            embot::sys::Task*           totask;
            Config() : tickevent(0), datareadyevent(0), totask(nullptr) {}
            Config(embot::common::Event _te, embot::common::Event _de, embot::sys::Task* _ts) : 
                tickevent(_te), datareadyevent(_de), totask(_ts) 
            { }                
        }; 
        
        
        bool initialise(Config &config);          
        
        bool autocalib(const embot::app::canprotocol::analog::polling::Message_AMPLIFIER_OFFSET_AUTOCALIB::Info &info, std::uint8_t &noisychannelmask, std::uint8_t &algorithmOKmask, std::uint8_t &finalmeasureOKmask, std::uint16_t &mae);
        
        bool start(const embot::app::canprotocol::analog::polling::Message_SET_TXMODE::StrainMode mode);
        bool stop();        
        bool tick(std::vector<embot::hw::can::Frame> &replies);
        bool processdata(std::vector<embot::hw::can::Frame> &replies);
        
        
        // interface to CANagentSTRAIN
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_TXMODE::Info &info);
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_SERIAL_NO::Info &info);  
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_CANDATARATE::Info &info); 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_FULL_SCALES::Info &info); 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SAVE2EE::Info &info); 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_CH_DAC::Info &info); 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_MATRIX_RC::Info &info); 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_MATRIX_G::Info &info); 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE::Info &info); 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE::Info &info); 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RESET::Info &info); 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_AMPLIFIER_PGA308_CFG1_SET::Info &info); 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_AMPLIFIER_OFFSET_AUTOCALIB::Info &info, embot::app::canprotocol::analog::polling::Message_AMPLIFIER_OFFSET_AUTOCALIB::ReplyInfo &replyinfo); 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_AMPLIFIER_GAINOFFSET_SET::Info &info); 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_REGULATIONSET_SET::Info &info); 
  
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_SERIAL_NO::Info &info, embot::app::canprotocol::analog::polling::Message_GET_SERIAL_NO::ReplyInfo &replyinfo);
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_FULL_SCALES::Info &info, embot::app::canprotocol::analog::polling::Message_GET_FULL_SCALES::ReplyInfo &replyinfo);
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_EEPROM_STATUS::Info &info, embot::app::canprotocol::analog::polling::Message_GET_EEPROM_STATUS::ReplyInfo &replyinfo);
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_CH_ADC::Info &info, embot::app::canprotocol::analog::polling::Message_GET_CH_ADC::ReplyInfo &replyinfo);
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_CH_DAC::Info &info, embot::app::canprotocol::analog::polling::Message_GET_CH_DAC::ReplyInfo &replyinfo);
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_MATRIX_RC::Info &info, embot::app::canprotocol::analog::polling::Message_GET_MATRIX_RC::ReplyInfo &replyinfo);
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_MATRIX_G::Info &info, embot::app::canprotocol::analog::polling::Message_GET_MATRIX_G::ReplyInfo &replyinfo);
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_CALIB_TARE::Info &info, embot::app::canprotocol::analog::polling::Message_GET_CALIB_TARE::ReplyInfo &replyinfo);
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_CURR_TARE::Info &info, embot::app::canprotocol::analog::polling::Message_GET_CURR_TARE::ReplyInfo &replyinfo);
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_AMPLIFIER_PGA308_CFG1_GET::Info &info, embot::app::canprotocol::analog::polling::Message_AMPLIFIER_PGA308_CFG1_GET::ReplyInfo &replyinfo);
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_AMPLIFIER_GAINOFFSET_GET::Info &info, embot::app::canprotocol::analog::polling::Message_AMPLIFIER_GAINOFFSET_GET::ReplyInfo &replyinfo);
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_GAIN_GET::Info &info, embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_GAIN_GET::ReplyInfo &replyinfo);
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_OFFSET_GET::Info &info, embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_OFFSET_GET::ReplyInfo &replyinfo);
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_REGULATIONSET_GET::Info &info, embot::app::canprotocol::analog::polling::Message_REGULATIONSET_GET::ReplyInfo &replyinfo);


    private:
        theSTRAIN(); 
        ~theSTRAIN(); 


    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;         
    };       


}}} // namespace embot { namespace app { namespace application


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

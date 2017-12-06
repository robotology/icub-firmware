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

#include "embot_app_canprotocol.h"
#include "embot_app_canprotocol_analog_periodic.h"
#include "embot_app_canprotocol_analog_polling.h"

#include "embot_hw_pga308.h"


#include <vector>

namespace embot { namespace app { namespace application {
           
    class theSTRAIN
    {
    public:
        static theSTRAIN& getInstance()
        {
            static theSTRAIN* p = new theSTRAIN();
            return *p;
        }
        
        
    public:
        struct Config
        {
//            embot::hw::PGA308::Config   pgaconfig[6];   
            embot::common::Event        tickevent;
            embot::common::Event        datareadyevent;
            embot::sys::Task*           totask;
            Config() : tickevent(0), datareadyevent(0), totask(nullptr) {}
            Config(embot::common::Event _te, embot::common::Event _de, embot::sys::Task* _ts) : 
                tickevent(_te), datareadyevent(_de), totask(_ts) 
            { }                
//            Config(const embot::hw::PGA308::Config * _pc, embot::common::Event _te, embot::common::Event _de, embot::sys::Task* _ts) : 
//                tickevent(_te), datareadyevent(_de), totask(_ts) 
//            { 
//                if(nullptr != _pc)  std::memmove(pgaconfig, _pc, sizeof(pgaconfig)); 
//            }
        }; 
        
        
        bool initialise(Config &config);   

        bool configure(embot::common::Time txperiod);        
        bool configure(embot::app::canprotocol::analog::polling::Message_SET_SERIAL_NO::Info &info);
        bool configure(embot::app::canprotocol::analog::polling::Message_SET_FULL_SCALES::Info &info);
        bool configure(embot::app::canprotocol::analog::polling::Message_SET_CH_DAC::Info &info);
        bool set(embot::app::canprotocol::analog::polling::Message_SET_MATRIX_RC::Info &info);
        bool set(embot::app::canprotocol::analog::polling::Message_SET_MATRIX_G::Info &info);
        bool set(embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE::Info &info);
        bool set(embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE::Info &info);
        bool resetamplifier(embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RESET::Info &info);
        bool set(embot::app::canprotocol::analog::polling::Message_AMPLIFIER_PGA308_CFG1_SET::Info &info);
        bool set(embot::app::canprotocol::analog::polling::Message_AMPLIFIER_GAINOFFSET_SET::Info &info, float &alpha, float &beta);
        
        bool save2eeprom();
        
        bool get_serial(embot::app::canprotocol::analog::polling::Message_GET_SERIAL_NO::ReplyInfo &replyinfo);
        bool get_fullscale(std::uint8_t channel, std::uint16_t &value);
        bool get_eepromstatus(bool &saved);
        bool get_adc(embot::app::canprotocol::analog::polling::Message_GET_CH_ADC::ReplyInfo &replyinfo);
        bool get_offset(std::uint8_t channel, std::uint16_t &value);
        bool get(embot::app::canprotocol::analog::polling::Message_GET_MATRIX_RC::ReplyInfo &replyinfo);
        bool get(embot::app::canprotocol::analog::polling::Message_GET_MATRIX_G::ReplyInfo &replyinfo);
        bool get(embot::app::canprotocol::analog::polling::Message_GET_CALIB_TARE::ReplyInfo &replyinfo);
        bool get(embot::app::canprotocol::analog::polling::Message_GET_CURR_TARE::ReplyInfo &replyinfo);
        bool get(embot::app::canprotocol::analog::polling::Message_AMPLIFIER_PGA308_CFG1_GET::ReplyInfo &replyinfo);
        bool get(embot::app::canprotocol::analog::polling::Message_AMPLIFIER_GAINOFFSET_GET::ReplyInfo &replyinfo);
        bool get(embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_GAIN_GET::ReplyInfo &replyinfo);
        bool get(embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_OFFSET_GET::ReplyInfo &replyinfo);
        
        bool autocalib(embot::app::canprotocol::analog::polling::Message_AMPLIFIER_OFFSET_AUTOCALIB::Info &info, std::uint8_t &noisychannelmask, std::uint8_t &algorithmOKmask, std::uint8_t &finalmeasureOKmask, std::uint16_t &mae);
        
        bool start(embot::app::canprotocol::analog::polling::Message_SET_TXMODE::StrainMode &mode);
        bool stop();        
        bool tick(std::vector<embot::hw::can::Frame> &replies);
        bool processdata(std::vector<embot::hw::can::Frame> &replies);

    private:
        theSTRAIN(); 

    public:
        // remove copy constructors and copy assignment operators
        theSTRAIN(const theSTRAIN&) = delete;
        theSTRAIN(theSTRAIN&) = delete;
        void operator=(const theSTRAIN&) = delete;
        void operator=(theSTRAIN&) = delete;

    private:    
        struct Impl;
        Impl *pImpl;        
    };       


}}} // namespace embot { namespace app { namespace application


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

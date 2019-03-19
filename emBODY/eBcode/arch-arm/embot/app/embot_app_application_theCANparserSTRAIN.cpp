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


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_application_theCANparserSTRAIN.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot.h"
#include <new>
#include "embot_hw.h"
#include "embot_app_theCANboardInfo.h"



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::application::theCANparserSTRAIN::Impl
{    

    class dummyCANagentSTRAIN : public CANagentSTRAIN 
    {
    public:
        dummyCANagentSTRAIN() {}
        virtual ~dummyCANagentSTRAIN() {}
            
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_TXMODE::Info &info) { return true; }
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_SERIAL_NO::Info &info) { return true; }  
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_CANDATARATE::Info &info) { return true; } 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_FULL_SCALES::Info &info) { return true; } 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SAVE2EE::Info &info) { return true; } 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_CH_DAC::Info &info) { return true; } 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_MATRIX_RC::Info &info) { return true; } 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_MATRIX_G::Info &info) { return true; } 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE::Info &info) { return true; } 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE::Info &info) { return true; } 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RESET::Info &info) { return true; } 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_AMPLIFIER_PGA308_CFG1_SET::Info &info) { return true; } 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_AMPLIFIER_OFFSET_AUTOCALIB::Info &info, embot::app::canprotocol::analog::polling::Message_AMPLIFIER_OFFSET_AUTOCALIB::ReplyInfo &replyinfo) { return true; } 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_AMPLIFIER_GAINOFFSET_SET::Info &info) { return true; } 
        virtual bool set(const embot::app::canprotocol::analog::polling::Message_REGULATIONSET_SET::Info &info) { return true; } 

    
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_SERIAL_NO::Info &info, embot::app::canprotocol::analog::polling::Message_GET_SERIAL_NO::ReplyInfo &replyinfo) { return true; }
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_FULL_SCALES::Info &info, embot::app::canprotocol::analog::polling::Message_GET_FULL_SCALES::ReplyInfo &replyinfo) { return true; }
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_EEPROM_STATUS::Info &info, embot::app::canprotocol::analog::polling::Message_GET_EEPROM_STATUS::ReplyInfo &replyinfo) { return true; }
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_CH_ADC::Info &info, embot::app::canprotocol::analog::polling::Message_GET_CH_ADC::ReplyInfo &replyinfo) { return true; }
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_CH_DAC::Info &info, embot::app::canprotocol::analog::polling::Message_GET_CH_DAC::ReplyInfo &replyinfo) { return true; }
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_MATRIX_RC::Info &info, embot::app::canprotocol::analog::polling::Message_GET_MATRIX_RC::ReplyInfo &replyinfo) { return true; }
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_MATRIX_G::Info &info, embot::app::canprotocol::analog::polling::Message_GET_MATRIX_G::ReplyInfo &replyinfo) { return true; }
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_CALIB_TARE::Info &info, embot::app::canprotocol::analog::polling::Message_GET_CALIB_TARE::ReplyInfo &replyinfo) { return true; }
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_GET_CURR_TARE::Info &info, embot::app::canprotocol::analog::polling::Message_GET_CURR_TARE::ReplyInfo &replyinfo) { return true; }
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_AMPLIFIER_PGA308_CFG1_GET::Info &info, embot::app::canprotocol::analog::polling::Message_AMPLIFIER_PGA308_CFG1_GET::ReplyInfo &replyinfo) { return true; }
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_AMPLIFIER_GAINOFFSET_GET::Info &info, embot::app::canprotocol::analog::polling::Message_AMPLIFIER_GAINOFFSET_GET::ReplyInfo &replyinfo) { return true; }
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_GAIN_GET::Info &info, embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_GAIN_GET::ReplyInfo &replyinfo) { return true; }
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_OFFSET_GET::Info &info, embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_OFFSET_GET::ReplyInfo &replyinfo) { return true; }
        virtual bool get(const embot::app::canprotocol::analog::polling::Message_REGULATIONSET_GET::Info &info, embot::app::canprotocol::analog::polling::Message_REGULATIONSET_GET::ReplyInfo &replyinfo) { return true; }       
    };
    
    dummyCANagentSTRAIN dummyagent;    
    
    Config config;
        
    bool txframe;
    bool recognised;
    
    embot::app::canprotocol::Clas cls;
    std::uint8_t cmd;    
        
    embot::hw::can::Frame reply;
    

    Impl() 
    { 
        config.agent = &dummyagent;          
        recognised = false;        
        txframe = false;
        cls = embot::app::canprotocol::Clas::none;
        cmd = 0;               
    }
    
   
    bool process(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    
        
    bool process_set_txmode(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_set_serial(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_get_serial(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_set_candatarate(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_set_fullscales(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_get_fullscales(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_get_eeprom_status(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);    
    bool process_save2ee(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_get_ch_adc(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_set_offset(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_get_offset(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);       
    bool process_set_matrix_value(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_get_matrix_value(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);     
    bool process_set_matrix_gain(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_get_matrix_gain(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_set_calib_bias(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_get_calib_bias(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies); 
    bool process_set_curr_bias(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_get_curr_bias(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies); 
    bool process_reset_strain2_amplifier(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);    
    bool process_set_strain2_amplifier_cfg1(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_get_strain2_amplifier_cfg1(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);   
    bool process_set_strain2_amplifier_autocalib(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies); 
    bool process_set_strain2_amplifier_gainoffset(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_get_strain2_amplifier_gainoffset(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_get_strain2_amplifier_gainlimits(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_get_strain2_amplifier_offsetlimits(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);  
    bool process_set_strain2_regulationset(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);
    bool process_get_strain2_regulationset(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies);    
};


bool embot::app::application::theCANparserSTRAIN::Impl::process(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    txframe = false;
    recognised = false;
    
    if(false == embot::app::canprotocol::frameis4board(frame, embot::app::theCANboardInfo::getInstance().cachedCANaddress()))
    {
        recognised = false;
        return recognised;
    }
        
    // now get cls and cmd
    cls = embot::app::canprotocol::frame2clas(frame);
    cmd = embot::app::canprotocol::frame2cmd(frame);
    
    
    // the basic can handle only some messages ...
    
    switch(cls)
    {
        
        case embot::app::canprotocol::Clas::pollingAnalogSensor:
        {        
            if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::SET_TXMODE) == cmd)
            {
                txframe = process_set_txmode(frame, replies);
                recognised = true;
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::SET_SERIAL_NO) == cmd)
            { 
                txframe = process_set_serial(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::GET_SERIAL_NO) == cmd)
            { 
                txframe = process_get_serial(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::SET_CANDATARATE) == cmd)
            { 
                txframe = process_set_candatarate(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::GET_FULL_SCALES) == cmd)
            { 
                txframe = process_get_fullscales(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::SET_FULL_SCALES) == cmd)
            { 
                txframe = process_set_fullscales(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::GET_EEPROM_STATUS) == cmd)
            { 
                txframe = process_get_eeprom_status(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::SAVE2EE) == cmd)
            { 
                txframe = process_save2ee(frame, replies);
                recognised = true;                
            } 
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::GET_CH_ADC) == cmd)
            { 
                txframe = process_get_ch_adc(frame, replies);
                recognised = true;                
            }  
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::GET_CH_DAC) == cmd)
            { 
                txframe = process_get_offset(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::SET_CH_DAC) == cmd)
            { 
                txframe = process_set_offset(frame, replies);
                recognised = true;                
            }    
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::GET_MATRIX_RC) == cmd)
            { 
                txframe = process_get_matrix_value(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::SET_MATRIX_RC) == cmd)
            { 
                txframe = process_set_matrix_value(frame, replies);
                recognised = true;                
            } 
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::GET_MATRIX_G) == cmd)
            { 
                txframe = process_get_matrix_gain(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::SET_MATRIX_G) == cmd)
            { 
                txframe = process_set_matrix_gain(frame, replies);
                recognised = true;                
            }    
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::GET_CALIB_TARE) == cmd)
            { 
                txframe = process_get_calib_bias(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::SET_CALIB_TARE) == cmd)
            { 
                txframe = process_set_calib_bias(frame, replies);
                recognised = true;                
            }              
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::GET_CURR_TARE) == cmd)
            { 
                txframe = process_get_curr_bias(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::SET_CURR_TARE) == cmd)
            { 
                txframe = process_set_curr_bias(frame, replies);
                recognised = true;                
            } 
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::AMPLIFIER_RESET) == cmd)
            { 
                txframe = process_reset_strain2_amplifier(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::AMPLIFIER_PGA308_CFG1_GET) == cmd)
            { 
                txframe = process_get_strain2_amplifier_cfg1(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::AMPLIFIER_PGA308_CFG1_SET) == cmd)
            { 
                txframe = process_set_strain2_amplifier_cfg1(frame, replies);
                recognised = true;                
            }  
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::AMPLIFIER_OFFSET_AUTOCALIB) == cmd)
            { 
                txframe = process_set_strain2_amplifier_autocalib(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::AMPLIFIER_GAINOFFSET_GET) == cmd)
            { 
                txframe = process_get_strain2_amplifier_gainoffset(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::AMPLIFIER_GAINOFFSET_SET) == cmd)
            { 
                txframe = process_set_strain2_amplifier_gainoffset(frame, replies);
                recognised = true;                
            } 
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::AMPLIFIER_RANGE_OF_GAIN_GET) == cmd)
            { 
                txframe = process_get_strain2_amplifier_gainlimits(frame, replies);
                recognised = true;                
            }  
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::AMPLIFIER_RANGE_OF_OFFSET_GET) == cmd)
            { 
                txframe = process_get_strain2_amplifier_offsetlimits(frame, replies);
                recognised = true;                
            }  
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::REGULATIONSET_GET) == cmd)
            { 
                txframe = process_get_strain2_regulationset(frame, replies);
                recognised = true;                
            }  
            else if(static_cast<std::uint8_t>(embot::app::canprotocol::analog::polling::CMD::REGULATIONSET_SET) == cmd)
            { 
                txframe = process_set_strain2_regulationset(frame, replies);
                recognised = true;                
            }             
        } break;

        
        default:
        {
            txframe = false;
            recognised = false;
        } break;
    }    
    
    
    return recognised;
}



bool embot::app::application::theCANparserSTRAIN::Impl::process_set_txmode(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_SET_TXMODE msg(embot::app::canprotocol::Board::strain2);
    msg.load(frame);
    config.agent->set(msg.info);
                  
    return msg.reply();        
}


bool embot::app::application::theCANparserSTRAIN::Impl::process_set_serial(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_SET_SERIAL_NO msg;
    msg.load(frame);
    config.agent->set(msg.info);
                  
    return msg.reply();        
}

bool embot::app::application::theCANparserSTRAIN::Impl::process_get_serial(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_GET_SERIAL_NO msg;
    msg.load(frame);    
    embot::app::canprotocol::analog::polling::Message_GET_SERIAL_NO::ReplyInfo replyinfo;
    config.agent->get(msg.info, replyinfo);
        
    embot::hw::can::Frame frame0;
    if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }        

    return false;    
}


bool embot::app::application::theCANparserSTRAIN::Impl::process_set_candatarate(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_SET_CANDATARATE msg;
    msg.load(frame);
    config.agent->set(msg.info);
                  
    return msg.reply();        
}

bool embot::app::application::theCANparserSTRAIN::Impl::process_set_fullscales(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_SET_FULL_SCALES msg;
    msg.load(frame);
    config.agent->set(msg.info);
    
    return msg.reply();        
}

bool embot::app::application::theCANparserSTRAIN::Impl::process_get_fullscales(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_GET_FULL_SCALES msg;
    msg.load(frame);    
    embot::app::canprotocol::analog::polling::Message_GET_FULL_SCALES::ReplyInfo replyinfo;
    config.agent->get(msg.info, replyinfo);
        
    embot::hw::can::Frame frame0;
    if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }        

    return false;    
}

bool embot::app::application::theCANparserSTRAIN::Impl::process_get_eeprom_status(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_GET_EEPROM_STATUS msg;
    msg.load(frame);    
    embot::app::canprotocol::analog::polling::Message_GET_EEPROM_STATUS::ReplyInfo replyinfo;   
    config.agent->get(msg.info, replyinfo);
        
    embot::hw::can::Frame frame0;
    if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }        

    return false;    
}


bool embot::app::application::theCANparserSTRAIN::Impl::process_save2ee(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_SAVE2EE msg;
    msg.load(frame);
    embot::app::canprotocol::analog::polling::Message_SAVE2EE::ReplyInfo replyinfo;
    replyinfo.ok = config.agent->set(msg.info);
                  
    embot::hw::can::Frame frame0;
    if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }        

    return false;       
}

bool embot::app::application::theCANparserSTRAIN::Impl::process_get_ch_adc(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_GET_CH_ADC msg;
    msg.load(frame);    
    embot::app::canprotocol::analog::polling::Message_GET_CH_ADC::ReplyInfo replyinfo;    
    config.agent->get(msg.info, replyinfo);
    
    embot::hw::can::Frame frame0;
    if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }        

    return false;    
}


bool embot::app::application::theCANparserSTRAIN::Impl::process_set_offset(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_SET_CH_DAC msg;
    msg.load(frame);
    config.agent->set(msg.info);
                  
    return msg.reply();        
}


bool embot::app::application::theCANparserSTRAIN::Impl::process_get_offset(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_GET_CH_DAC msg;
    msg.load(frame);    
    embot::app::canprotocol::analog::polling::Message_GET_CH_DAC::ReplyInfo replyinfo;
    config.agent->get(msg.info, replyinfo);
        
    embot::hw::can::Frame frame0;
    if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }        

    return false;    
}


bool embot::app::application::theCANparserSTRAIN::Impl::process_set_matrix_value(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_SET_MATRIX_RC msg;
    msg.load(frame);
    config.agent->set(msg.info);
                  
    return msg.reply();        
}

bool embot::app::application::theCANparserSTRAIN::Impl::process_get_matrix_value(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_GET_MATRIX_RC msg;
    msg.load(frame);    
    embot::app::canprotocol::analog::polling::Message_GET_MATRIX_RC::ReplyInfo replyinfo;
    config.agent->get(msg.info, replyinfo);
        
    embot::hw::can::Frame frame0;
    if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }        

    return false;    
}

bool embot::app::application::theCANparserSTRAIN::Impl::process_set_matrix_gain(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_SET_MATRIX_G msg;
    msg.load(frame);
    config.agent->set(msg.info);
    
    return msg.reply();        
}

bool embot::app::application::theCANparserSTRAIN::Impl::process_get_matrix_gain(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_GET_MATRIX_G msg;
    msg.load(frame);
    
    embot::app::canprotocol::analog::polling::Message_GET_MATRIX_G::ReplyInfo replyinfo;
    config.agent->get(msg.info, replyinfo);
        
    embot::hw::can::Frame frame0;
    if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }        

    return false;    
}


bool embot::app::application::theCANparserSTRAIN::Impl::process_set_calib_bias(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE msg;
    msg.load(frame);
    config.agent->set(msg.info);
                  
    return msg.reply();        
}

bool embot::app::application::theCANparserSTRAIN::Impl::process_get_calib_bias(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_GET_CALIB_TARE msg;
    msg.load(frame);    
    embot::app::canprotocol::analog::polling::Message_GET_CALIB_TARE::ReplyInfo replyinfo;
    config.agent->get(msg.info, replyinfo);
        
    embot::hw::can::Frame frame0;
    if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }        

    return false;    
}


bool embot::app::application::theCANparserSTRAIN::Impl::process_set_curr_bias(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE msg;
    msg.load(frame);
    config.agent->set(msg.info);    
    
    return msg.reply();        
}

bool embot::app::application::theCANparserSTRAIN::Impl::process_get_curr_bias(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_GET_CURR_TARE msg;
    msg.load(frame);    
    embot::app::canprotocol::analog::polling::Message_GET_CURR_TARE::ReplyInfo replyinfo;
    config.agent->get(msg.info, replyinfo);
    
    embot::hw::can::Frame frame0;
    if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }        

    return false;    
}



bool embot::app::application::theCANparserSTRAIN::Impl::process_reset_strain2_amplifier(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RESET msg;
    msg.load(frame);
    config.agent->set(msg.info); 
    
    return msg.reply();        
}

bool embot::app::application::theCANparserSTRAIN::Impl::process_set_strain2_amplifier_cfg1(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_AMPLIFIER_PGA308_CFG1_SET msg;
    msg.load(frame);
    config.agent->set(msg.info);
                  
    return msg.reply();        
}

bool embot::app::application::theCANparserSTRAIN::Impl::process_get_strain2_amplifier_cfg1(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_AMPLIFIER_PGA308_CFG1_GET msg;
    msg.load(frame);
    
    embot::app::canprotocol::analog::polling::Message_AMPLIFIER_PGA308_CFG1_GET::ReplyInfo replyinfo;
        
    if(0xf == msg.info.channel)
    {
        embot::app::canprotocol::analog::polling::Message_AMPLIFIER_PGA308_CFG1_GET::Info info = msg.info;
        // we send back for all channels
        for(std::uint8_t i=0; i<6; i++)
        {
            info.channel = i; // same info, different channel            
            config.agent->get(info, replyinfo);
            
            embot::hw::can::Frame frame0;
            if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
            {
                replies.push_back(frame0);
            }
            else
            {
                return false;
            }    
            
        }

        return true;        
    }
    else
    {    
        config.agent->get(msg.info, replyinfo);
            
        embot::hw::can::Frame frame0;
        if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
        {
            replies.push_back(frame0);
            return true;
        } 
    }    

    return false;    
}


bool embot::app::application::theCANparserSTRAIN::Impl::process_set_strain2_amplifier_autocalib(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_AMPLIFIER_OFFSET_AUTOCALIB msg;
    msg.load(frame);
        
    embot::app::canprotocol::analog::polling::Message_AMPLIFIER_OFFSET_AUTOCALIB::ReplyInfo replyinfo;
    config.agent->set(msg.info, replyinfo);
    
    embot::hw::can::Frame frame0;
    if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }        

    return false;     
}


bool embot::app::application::theCANparserSTRAIN::Impl::process_set_strain2_amplifier_gainoffset(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_AMPLIFIER_GAINOFFSET_SET msg;
    msg.load(frame);
    config.agent->set(msg.info);
                  
    return msg.reply();        
}

bool embot::app::application::theCANparserSTRAIN::Impl::process_get_strain2_amplifier_gainoffset(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_AMPLIFIER_GAINOFFSET_GET msg;
    msg.load(frame);
    
    embot::app::canprotocol::analog::polling::Message_AMPLIFIER_GAINOFFSET_GET::ReplyInfo replyinfo;
    
    
    if(0xf == msg.info.channel)
    {
        embot::app::canprotocol::analog::polling::Message_AMPLIFIER_GAINOFFSET_GET::Info info = msg.info;
        
        // we send back for all channels
        for(std::uint8_t i=0; i<6; i++)
        {
            info.channel = i;
            config.agent->get(info, replyinfo);  // same info, different channel 
                
            embot::hw::can::Frame frame0;
            if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
            {
                replies.push_back(frame0);
            }
            else
            {
                return false;
            }    
            
        }

        return true;        
    }
    else
    {    
        config.agent->get(msg.info, replyinfo);
            
        embot::hw::can::Frame frame0;
        if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
        {
            replies.push_back(frame0);
            return true;
        } 
    }    

    return false;    
}

bool embot::app::application::theCANparserSTRAIN::Impl::process_get_strain2_amplifier_gainlimits(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_GAIN_GET msg;
    msg.load(frame);
    
    embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_GAIN_GET::ReplyInfo replyinfo;
        
    if(0xf == msg.info.channel)
    {
        embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_GAIN_GET::Info info = msg.info;
        // we send back for all channels
        for(std::uint8_t i=0; i<6; i++)
        {
            info.channel = i;  // same info, different channel 
            config.agent->get(info, replyinfo);
                
            embot::hw::can::Frame frame0;
            if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
            {
                replies.push_back(frame0);
            }
            else
            {
                return false;
            }    
            
        }

        return true;        
    }
    else
    {    
        config.agent->get(msg.info, replyinfo);
            
        embot::hw::can::Frame frame0;
        if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
        {
            replies.push_back(frame0);
            return true;
        } 
    }    

    return false;    
}

bool embot::app::application::theCANparserSTRAIN::Impl::process_get_strain2_amplifier_offsetlimits(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_OFFSET_GET msg;
    msg.load(frame);
    
    embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_OFFSET_GET::ReplyInfo replyinfo; 
    
    if(0xf == msg.info.channel)
    {
        embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_OFFSET_GET::Info info = msg.info;
        
        // we send back for all channels
        for(std::uint8_t i=0; i<6; i++)
        {
            info.channel = i;  // same info, different channel 
            config.agent->get(info, replyinfo);
                
            embot::hw::can::Frame frame0;
            if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
            {
                replies.push_back(frame0);
            }
            else
            {
                return false;
            }    
            
        }

        return true;        
    }
    else
    {
        config.agent->get(msg.info, replyinfo);
            
        embot::hw::can::Frame frame0;
        if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
        {
            replies.push_back(frame0);
            return true;
        } 
    }    

    return false;    
}


bool embot::app::application::theCANparserSTRAIN::Impl::process_get_strain2_regulationset(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_REGULATIONSET_GET msg;
    msg.load(frame);
    
    embot::app::canprotocol::analog::polling::Message_REGULATIONSET_GET::ReplyInfo replyinfo;
    config.agent->get(msg.info, replyinfo);
        
    embot::hw::can::Frame frame0;
    if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {
        replies.push_back(frame0);
        return true;
    } 

    return false;    
}


bool embot::app::application::theCANparserSTRAIN::Impl::process_set_strain2_regulationset(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{
    embot::app::canprotocol::analog::polling::Message_REGULATIONSET_SET msg;
    msg.load(frame);
    config.agent->set(msg.info);
                  
    return msg.reply();        
}




// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theCANparserSTRAIN& embot::app::application::theCANparserSTRAIN::getInstance()
{
    static theCANparserSTRAIN* p = new theCANparserSTRAIN();
    return *p;
}

embot::app::application::theCANparserSTRAIN::theCANparserSTRAIN()
//    : pImpl(new Impl)
{
    pImpl = std::make_unique<Impl>();

}  

    
embot::app::application::theCANparserSTRAIN::~theCANparserSTRAIN() { }
   
        
bool embot::app::application::theCANparserSTRAIN::initialise(const Config &config)
{
    if(!config.isvalid())
    {
        return false;
    }
    
    pImpl->config = config;
        
    return true;
}
  


bool embot::app::application::theCANparserSTRAIN::process(const embot::hw::can::Frame &frame, std::vector<embot::hw::can::Frame> &replies)
{    
    return pImpl->process(frame, replies);
}





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



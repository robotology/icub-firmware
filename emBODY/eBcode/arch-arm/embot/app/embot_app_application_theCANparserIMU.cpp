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

#include "embot_app_application_theCANparserIMU.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <new>
#include "embot_app_theCANboardInfo.h"



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::application::theCANparserIMU::Impl
{   

    class dummyCANagentIMU : public CANagentIMU 
    {
    public:
        dummyCANagentIMU() {}
        virtual ~dummyCANagentIMU() {}
            
        virtual bool set(const embot::prot::can::analog::polling::Message_ACC_GYRO_SETUP::Info &info) { return true; }
        virtual bool set(const embot::prot::can::analog::polling::Message_IMU_CONFIG_SET::Info &info) { return true; }  
        virtual bool set(const embot::prot::can::analog::polling::Message_IMU_TRANSMIT::Info &info) { return true; } 
    
        virtual bool get(const embot::prot::can::analog::polling::Message_IMU_CONFIG_GET::Info &info, embot::prot::can::analog::polling::Message_IMU_CONFIG_GET::ReplyInfo &replyinfo) { return true; }                
    };
    
    dummyCANagentIMU dummyagent;
    
    Config config;
        
    bool txframe;
    bool recognised;
    
    embot::prot::can::Clas cls;
    std::uint8_t cmd;    

        
    embot::prot::can::Frame reply;
    

    Impl() 
    { 
        config.agent = &dummyagent;        
        recognised = false;        
        txframe = false;
        cls = embot::prot::can::Clas::none;
        cmd = 0;              
    }
    
   
    bool process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    
    bool process_set_accgyrosetup(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    bool process_set_imu_config(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies); 
    bool process_get_imu_config(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);    
    bool process_imu_transmit(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);    
};


bool embot::app::application::theCANparserIMU::Impl::process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    txframe = false;
    recognised = false;
    
    if(false == embot::prot::can::frameis4board(frame, embot::app::theCANboardInfo::getInstance().cachedCANaddress()))
    {
        recognised = false;
        return recognised;
    }
        
    // now get cls and cmd
    cls = embot::prot::can::frame2clas(frame);
    cmd = embot::prot::can::frame2cmd(frame);
    
    
    // the basic can handle only some messages ...
    
    switch(cls)
    {
        
        case embot::prot::can::Clas::pollingAnalogSensor:
        {
            // only a few used for the inertials ...
            if(static_cast<std::uint8_t>(embot::prot::can::analog::polling::CMD::ACC_GYRO_SETUP) == cmd)
            { 
                txframe = process_set_accgyrosetup(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::prot::can::analog::polling::CMD::IMU_CONFIG_SET) == cmd)
            { 
                txframe = process_set_imu_config(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::prot::can::analog::polling::CMD::IMU_CONFIG_GET) == cmd)
            { 
                txframe = process_get_imu_config(frame, replies);
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::prot::can::analog::polling::CMD::IMU_TRANSMIT) == cmd)
            { 
                txframe = process_imu_transmit(frame, replies);
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



bool embot::app::application::theCANparserIMU::Impl::process_set_accgyrosetup(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::analog::polling::Message_ACC_GYRO_SETUP msg;
    msg.load(frame);
    config.agent->set(msg.info);
    
    return msg.reply();        
}

bool embot::app::application::theCANparserIMU::Impl::process_set_imu_config(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::analog::polling::Message_IMU_CONFIG_SET msg;
    msg.load(frame);
    config.agent->set(msg.info);
    
    return msg.reply();        
}

bool embot::app::application::theCANparserIMU::Impl::process_get_imu_config(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::analog::polling::Message_IMU_CONFIG_GET msg;
    msg.load(frame);    
    embot::prot::can::analog::polling::Message_IMU_CONFIG_GET::ReplyInfo replyinfo;      
    config.agent->get(msg.info, replyinfo);

    embot::prot::can::Frame frame0;
    if(true == msg.reply(frame0, embot::app::theCANboardInfo::getInstance().cachedCANaddress(), replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }  
    
    return false;         
}
  

bool embot::app::application::theCANparserIMU::Impl::process_imu_transmit(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::analog::polling::Message_IMU_TRANSMIT msg;
    msg.load(frame);    
    config.agent->set(msg.info);
      
    return msg.reply();        
}




// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theCANparserIMU& embot::app::application::theCANparserIMU::getInstance()
{
    static theCANparserIMU* p = new theCANparserIMU();
    return *p;
}

embot::app::application::theCANparserIMU::theCANparserIMU()
//    : pImpl(new Impl)
{
    pImpl = std::make_unique<Impl>();

}  

    
embot::app::application::theCANparserIMU::~theCANparserIMU() { }
   
        
bool embot::app::application::theCANparserIMU::initialise(const Config &config)
{
    if(!config.isvalid())
    {
        return false;
    }
    
    pImpl->config = config;
        
    return true;
}
  


bool embot::app::application::theCANparserIMU::process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{    
    return pImpl->process(frame, replies);
}





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



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

#include "embot_app_application_theCANparserSkin.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include <new>

#include "embot_app_theCANboardInfo.h"

#include "embot_app_application_theCANtracer.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::application::theCANparserSkin::Impl
{   
    class dummyCANagentSKIN : public CANagentSKIN 
    {
    public:
        dummyCANagentSKIN() {}
        virtual ~dummyCANagentSKIN() {}
            
        bool isactive() const override { return false; }
        const std::string& status() const override { static const std::string s {"dummy"}; return s; }
            
        bool set(const embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG::Info &info) override {  return true; }
        bool set(const embot::prot::can::analog::polling::Message_SKIN_SET_TRIANG_CFG::Info &info) override {  return true; }  
        bool set(const embot::prot::can::analog::polling::Message_SET_TXMODE::Info &info) override {  return true; } 
        bool set(const embot::prot::can::analog::polling::Message_SKIN_OBSOLETE_TACT_SETUP::Info &info) override {  return true; }            
    };
    
    dummyCANagentSKIN dummyagent;
    
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
    
    
    bool process_set_brdcfg(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);   
    bool process_set_trgcfg(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);      
    bool process_set_txmode(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);   
    bool process_set_obsolete_tactsetup(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);    
};


bool embot::app::application::theCANparserSkin::Impl::process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
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
            
            // only embot::prot::can::analog::polling::CMD::SKIN_SET_BRD_CFG, SKIN_SET_TRIANG_CFG, SET_TXMODE            
            if(static_cast<std::uint8_t>(embot::prot::can::analog::polling::CMD::SKIN_SET_BRD_CFG) == cmd)
            {
                txframe = process_set_brdcfg(frame, replies);
                recognised = true;
            }
            else if(static_cast<std::uint8_t>(embot::prot::can::analog::polling::CMD::SKIN_SET_TRIANG_CFG) == cmd)
            {
                txframe = process_set_trgcfg(frame, replies);
                recognised = true;
            }
            else if(static_cast<std::uint8_t>(embot::prot::can::analog::polling::CMD::SET_TXMODE) == cmd)
            {
                txframe = process_set_txmode(frame, replies);
                recognised = true;
            }
            else if(static_cast<std::uint8_t>(embot::prot::can::analog::polling::CMD::SKIN_OBSOLETE_TACT_SETUP) == cmd)
            {
                txframe = process_set_obsolete_tactsetup(frame, replies);
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




bool embot::app::application::theCANparserSkin::Impl::process_set_brdcfg(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    if(false == config.agent->isactive())
    {
        embot::app::theCANtracer &tr = embot::app::theCANtracer::getInstance();
        tr.print("ERR: " + config.agent->status(), replies);  
        return true;        
    }
    
    embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG msg;
    msg.load(frame);
      
    config.agent->set(msg.info);
            
    return msg.reply();        
}

bool embot::app::application::theCANparserSkin::Impl::process_set_obsolete_tactsetup(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    if(false == config.agent->isactive())
    {
        embot::app::theCANtracer &tr = embot::app::theCANtracer::getInstance();
        tr.print("ERR: " + config.agent->status(), replies);  
        return true;        
    }
    
    embot::prot::can::analog::polling::Message_SKIN_OBSOLETE_TACT_SETUP msg;
    msg.load(frame);
      
    config.agent->set(msg.info);
            
    return msg.reply();        
}



bool embot::app::application::theCANparserSkin::Impl::process_set_trgcfg(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{    
    if(false == config.agent->isactive())
    {
        embot::app::theCANtracer &tr = embot::app::theCANtracer::getInstance();
        tr.print("ERR: " + config.agent->status(), replies);  
        return true;        
    }
    
    embot::prot::can::analog::polling::Message_SKIN_SET_TRIANG_CFG msg;
    msg.load(frame);
      
    config.agent->set(msg.info);
    
    return msg.reply();        
}



bool embot::app::application::theCANparserSkin::Impl::process_set_txmode(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    if(false == config.agent->isactive())
    {
        embot::app::theCANtracer &tr = embot::app::theCANtracer::getInstance();
        tr.print("ERR: " + config.agent->status(), replies);  
        return true;        
    }
    
    embot::prot::can::analog::polling::Message_SET_TXMODE msg(embot::prot::can::Board::mtb4);
    msg.load(frame);
    
    config.agent->set(msg.info);
                  
    return msg.reply();        
}





// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theCANparserSkin& embot::app::application::theCANparserSkin::getInstance()
{
    static theCANparserSkin* p = new theCANparserSkin();
    return *p;
}

embot::app::application::theCANparserSkin::theCANparserSkin()
//    : pImpl(new Impl)
{
    pImpl = std::make_unique<Impl>();

}  

    
embot::app::application::theCANparserSkin::~theCANparserSkin() { }
   
        
bool embot::app::application::theCANparserSkin::initialise(const Config &config)
{
    if(!config.isvalid())
    {
        return false;
    }
    
    pImpl->config = config;
        
    return true;
}
  


bool embot::app::application::theCANparserSkin::process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{    
    return pImpl->process(frame, replies);
}





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



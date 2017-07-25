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

#include "embot_app_application_theSTRAIN.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot.h"

#include <new>
#include "embot_sys_Timer.h"
#include "embot_sys_Action.h"
#include "embot_hw.h"
#include "embot_app_canprotocol.h"

#include <cstdio>

#include "embot_app_theCANboardInfo.h"

#include "embot_app_application_theCANtracer.h"


#include "embot_common.h"



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


    
struct embot::app::application::theSTRAIN::Impl
{ 
    
    static const std::uint32_t validityKey = 0x7777dead;
    struct StrainStoredInfo
    {   
        std::uint32_t       key;
        char                serial[8];      // we use 7 bytes actually
        std::uint16_t       fullscales[6];
        std::int16_t        matrix[6][6];  
        std::uint8_t        gain;  
        std::uint16_t       calib_bias[6]; 
        std::uint16_t       curr_bias[6];
        std::uint16_t       ampgain1[6];
        std::uint16_t       ampgain2[6];        
//        std::uint8_t        tobefilled[7];  // to make the size of struct ... multiple of 8.
        void reset() { 
            key = 0; 
            std::memset(serial, 0, sizeof(serial)); 
            std::memset(fullscales, 0, sizeof(fullscales));
            std::memset(matrix, 0, sizeof(matrix)); 
            std::memset(calib_bias, 0, sizeof(calib_bias)); 
            std::memset(curr_bias, 0, sizeof(curr_bias));
            std::memset(ampgain1, 0, sizeof(ampgain1));
            std::memset(ampgain2, 0, sizeof(ampgain2));
            gain = 0;
        }
        void setdefaultserial() { 
            std::snprintf(serial, sizeof(serial), "SN0000");
        }
        void setserial(const char *str) {
            std::snprintf(serial, sizeof(serial), "%s", str);
        }
        void setdefaultfullscales() { 
            fullscales[0] = fullscales[1] = fullscales[2] = fullscales[3] = fullscales[4] = fullscales[5] = 0xffff;
        } 
        void setfullscale(std::uint8_t c, std::uint16_t value) { 
            if(c<6)
            {
                fullscales[c] = value;
            }
        }         
        void setdefaultmatrix() { 
            std::memset(matrix, 0, sizeof(matrix));
            matrix[0][0] = matrix[1][1] = matrix[2][2] = matrix[3][3] = matrix[4][4] = matrix[5][5] = 1;
        } 
        void setmatrix(std::uint8_t r, std::uint8_t c, std::int16_t value) { 
            if((r<6) && (c<6))
            {
                matrix[r][c] = value;
            }
        } 
        void setdefault() {
            setdefaultserial();
            setdefaultfullscales();
            setdefaultmatrix();
            gain = 1;
            std::memset(calib_bias, 0, sizeof(calib_bias)); 
            std::memset(curr_bias, 0, sizeof(curr_bias));
            std::memset(ampgain1, 1, sizeof(ampgain1));
            std::memset(ampgain2, 1, sizeof(ampgain2));
        }
        StrainStoredInfo() { reset(); }
    };    
    
    // pity that armcc does not supporst std::tuple<>
    struct TripleValue
    {
        std::int16_t    x;
        std::int16_t    y;
        std::int16_t    z;
        void reset() { x = y = z = 0; }
        void set(std::int16_t xx, std::int16_t yy, std::int16_t zz) { x = xx; y = yy; z = zz; }
        TripleValue() { reset(); } 
    }; 
    
      
    Config config;
           
    bool ticking;
    
    
    embot::sys::Timer *ticktimer;
    embot::sys::Action action;
        
    
    StrainStoredInfo strainstoredinfo;
    bool infosavedoneeprom;
    
    std::uint8_t canaddress;
    
    embot::common::Time txperiod;
       
    TripleValue force;
    TripleValue torque;
    
    // i put in here some values proper of the STRAIN. they may be many.... hence better to organise them in a struct later on.
    
    

    Impl() 
    {   
        ticking = false;  

        ticktimer = new embot::sys::Timer;   
        txperiod = 50*embot::common::time1millisec;    

        force.reset();
        torque.reset();

        canaddress = 0;  
        
        strainstoredinfo.reset();
        infosavedoneeprom = false;
        
    }
    
   
    bool start();
    bool stop();
    
    bool tick(std::vector<embot::hw::can::Frame> &replies);
    
    bool configure();
    
    bool acquisition();
    
    
    bool fill(embot::app::canprotocol::Message_asper_FORCE_VECTOR::Info &info);
    bool fill(embot::app::canprotocol::Message_asper_TORQUE_VECTOR::Info &info);
                      
};



bool embot::app::application::theSTRAIN::Impl::start()
{    
    ticktimer->start(txperiod, embot::sys::Timer::Type::forever, action);
    ticking = true;    
    return true;
}


bool embot::app::application::theSTRAIN::Impl::stop()
{    
    ticktimer->stop();
    ticking = false;    
    return true;
}


bool embot::app::application::theSTRAIN::Impl::fill(embot::app::canprotocol::Message_asper_FORCE_VECTOR::Info &info)
{
    bool ret = true;
    
    info.canaddress = canaddress;
    info.x = force.x;
    info.y = force.y;
    info.z = force.z;
         
    return ret;    
}


bool embot::app::application::theSTRAIN::Impl::fill(embot::app::canprotocol::Message_asper_TORQUE_VECTOR::Info &info)
{
    bool ret = true;

    info.canaddress = canaddress;
    info.x = torque.x;
    info.y = torque.y;
    info.z = torque.z;
    
    return ret;    
}


bool embot::app::application::theSTRAIN::Impl::acquisition()
{
    #warning TODO: perform hw acquisition of all FT values

    
    static std::int16_t vf = 0;
    static std::int16_t vt = 3;
    
    force.x = vf++;
    force.y = vf++;
    force.z = vf++;
    
    torque.x = vt++;
    torque.y = vt++;
    torque.z = vt++;
    
    return true;
}


bool embot::app::application::theSTRAIN::Impl::tick(std::vector<embot::hw::can::Frame> &replies)
{   
    if(false == ticking)
    {
        return false;
    }
        
    // perform acquisition
    acquisition();
    
    embot::hw::can::Frame frame;   
                                            
    
    embot::app::canprotocol::Message_asper_FORCE_VECTOR::Info forceinfo;
    if(true == fill(forceinfo))
    {
        embot::app::canprotocol::Message_asper_FORCE_VECTOR msg;
        msg.load(forceinfo);
        msg.get(frame);
        replies.push_back(frame);
    }            

    
    embot::app::canprotocol::Message_asper_TORQUE_VECTOR::Info torqueinfo;
    if(true == fill(torqueinfo))
    {
        embot::app::canprotocol::Message_asper_TORQUE_VECTOR msg;
        msg.load(torqueinfo);
        msg.get(frame);
        replies.push_back(frame);
    }           
   
       
    return true;    
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theSTRAIN::theSTRAIN()
: pImpl(new Impl)
{       

}

         
bool embot::app::application::theSTRAIN::initialise(Config &config)
{
    pImpl->config = config;
    
    pImpl->action.set(embot::sys::Action::EventToTask(pImpl->config.tickevent, pImpl->config.totask));

    // retrieve address    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    pImpl->canaddress = canbrdinfo.getCANaddress();
    
    // retrieve strainstoredinfo
    canbrdinfo.userdataread(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);
    // if it is not valid, then ... set default and impose validity key
    if(pImpl->validityKey != pImpl->strainstoredinfo.key)
    {
        pImpl->strainstoredinfo.reset();        
        pImpl->strainstoredinfo.setdefault();
        pImpl->strainstoredinfo.key = pImpl->validityKey;
        canbrdinfo.userdatawrite(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);
        
        canbrdinfo.userdataread(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);
    }
    
    pImpl->infosavedoneeprom = true;
     
    return true;
}


bool embot::app::application::theSTRAIN::configure(embot::common::Time txperiod)
{
    // if ticking: stop it
    if(true == pImpl->ticking)
    {
        stop();
    }
    
    pImpl->txperiod = txperiod;

//    // if the rate is not zero: start acquisition
//            
//    if((0 != pImpl->txperiod))
//    {
//        start();
//    }
//  
    return true;    
}


bool embot::app::application::theSTRAIN::configure(embot::app::canprotocol::Message_aspoll_SET_SERIAL_NO::Info &info)
{
//    // if ticking: stop it
//    if(true == pImpl->ticking)
//    {
//        stop();
//    }
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    
    // retrieve strainstoredinfo
    canbrdinfo.userdataread(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);    
    pImpl->strainstoredinfo.setserial(info.serial);
    canbrdinfo.userdatawrite(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);

    return true;    
}


bool embot::app::application::theSTRAIN::get_serial(embot::app::canprotocol::Message_aspoll_GET_SERIAL_NO::ReplyInfo &replyinfo)
{    
   
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    
    // retrieve strainstoredinfo
    canbrdinfo.userdataread(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);    
    std::snprintf(replyinfo.serial, sizeof(replyinfo.serial), pImpl->strainstoredinfo.serial);
   
    return true;    
}

bool embot::app::application::theSTRAIN::get_fullscale(std::uint8_t channel, std::uint16_t &value)
{    
    if(channel >= 6 )
    {
        return false;
    }
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    
    // retrieve strainstoredinfo
    canbrdinfo.userdataread(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);    
    value = pImpl->strainstoredinfo.fullscales[channel];
    return true;    
}


bool embot::app::application::theSTRAIN::get_adc(std::uint8_t channel, bool useraw, std::uint16_t &value)
{    
    if(channel >= 6 )
    {
        return false;
    }
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();

    value = 0x1234;
    
    return true;    
}


bool embot::app::application::theSTRAIN::get_offset(std::uint8_t channel, std::uint16_t &value)
{    
    if(channel >= 6 )
    {
        return false;
    }
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();

    value = 0x1234;
    
    return true;    
}

bool embot::app::application::theSTRAIN::configure(embot::app::canprotocol::Message_aspoll_SET_FULL_SCALES::Info &info)
{    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    
    // retrieve strainstoredinfo
    canbrdinfo.userdataread(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);    
    pImpl->strainstoredinfo.setfullscale(info.channel, info.fullscale);
    canbrdinfo.userdatawrite(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);

    return true;    
}

bool embot::app::application::theSTRAIN::get_eepromstatus(bool &saved)
{    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    saved = pImpl->infosavedoneeprom;
    return true;    
}


bool embot::app::application::theSTRAIN::save2eeprom()
{    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    
    #warning: you must copy the temporary strain info into eeprom, set true the pImpl->infosavedoneeprom variable etc.
    pImpl->infosavedoneeprom = true;

    //canbrdinfo.userdatawrite(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);

    return true;    
}

bool embot::app::application::theSTRAIN::configure(embot::app::canprotocol::Message_aspoll_SET_CH_DAC_offset::Info &info)
{  
    if(info.channel >= 6 )
    {
        return false;
    } 
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
   
    // manage:

    std::uint16_t o = info.offset;

//    // retrieve strainstoredinfo
//    canbrdinfo.userdataread(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);    
//    pImpl->strainstoredinfo.setfullscale(info.channel, info.fullscale);
//    canbrdinfo.userdatawrite(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);

    return true;    
}


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::Message_aspoll_GET_MATRIX_RC::ReplyInfo &replyinfo)
{    
    if((replyinfo.row >= 6) || (replyinfo.col >= 6))
    {
        return false;
    }
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();

    replyinfo.value =  pImpl->strainstoredinfo.matrix[replyinfo.row][replyinfo.col];
    
    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::Message_aspoll_SET_MATRIX_RC::Info &info)
{  
    if((info.row >= 6) || (info.col >= 6))
    {
        return false;
    }
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
   
    // manage:

    pImpl->strainstoredinfo.matrix[info.row][info.col] = info.value;

//    // retrieve strainstoredinfo
//    canbrdinfo.userdataread(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);    
//    pImpl->strainstoredinfo.setfullscale(info.channel, info.fullscale);
//    canbrdinfo.userdatawrite(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);

    return true;    
}


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::Message_aspoll_GET_MATRIX_G::ReplyInfo &replyinfo)
{        
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();

    replyinfo.gain =  pImpl->strainstoredinfo.gain;
    
    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::Message_aspoll_SET_MATRIX_G::Info &info)
{  

    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
   
    // manage:

    pImpl->strainstoredinfo.gain = info.gain;

//    // retrieve strainstoredinfo
//    canbrdinfo.userdataread(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);    
//    pImpl->strainstoredinfo.setfullscale(info.channel, info.fullscale);
//    canbrdinfo.userdatawrite(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);

    return true;    
}

bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::Message_aspoll_GET_CALIB_TARE_bias::ReplyInfo &replyinfo)
{  
    if(replyinfo.channel >= 6)
    {
        return false;
    }     
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();

    replyinfo.value =  pImpl->strainstoredinfo.calib_bias[replyinfo.channel];
    
    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::Message_aspoll_SET_CALIB_TARE_bias::Info &info)
{ 
    if(info.channel >= 6)
    {
        return false;
    }     

    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
   
    // manage:
    bool ret = true;

    switch(info.mode)
    {
        case embot::app::canprotocol::Message_aspoll_SET_CALIB_TARE_bias::Mode::useval:
        {
            pImpl->strainstoredinfo.calib_bias[info.channel] = info.value;
        } break;            

        case embot::app::canprotocol::Message_aspoll_SET_CALIB_TARE_bias::Mode::reset:
        {
            pImpl->strainstoredinfo.calib_bias[info.channel] = 0;
        } break;

        case embot::app::canprotocol::Message_aspoll_SET_CALIB_TARE_bias::Mode::def:
        {
            pImpl->strainstoredinfo.calib_bias[info.channel] = 1; // TBD: set proper value for this case
        } break;    

        default:
        case embot::app::canprotocol::Message_aspoll_SET_CALIB_TARE_bias::Mode::unknown:
        {
            ret = false;
        } break;        
    }
    

//    // retrieve strainstoredinfo
//    canbrdinfo.userdataread(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);    
//    pImpl->strainstoredinfo.setfullscale(info.channel, info.fullscale);
//    canbrdinfo.userdatawrite(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);

    return ret;    
}


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::Message_aspoll_GET_CURR_TARE_bias::ReplyInfo &replyinfo)
{  
    if(replyinfo.channel >= 6)
    {
        return false;
    }     
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();

    replyinfo.value =  pImpl->strainstoredinfo.curr_bias[replyinfo.channel];
    
    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::Message_aspoll_SET_CURR_TARE_bias::Info &info)
{ 
    if(info.channel >= 6)
    {
        return false;
    }     

    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
   
    // manage:
    bool ret = true;

    switch(info.mode)
    {
        case embot::app::canprotocol::Message_aspoll_SET_CURR_TARE_bias::Mode::useval:
        {
            pImpl->strainstoredinfo.curr_bias[info.channel] = info.value;
        } break;            

        case embot::app::canprotocol::Message_aspoll_SET_CURR_TARE_bias::Mode::reset:
        {
            pImpl->strainstoredinfo.curr_bias[info.channel] = 0;
        } break;

        case embot::app::canprotocol::Message_aspoll_SET_CURR_TARE_bias::Mode::def:
        {
            pImpl->strainstoredinfo.curr_bias[info.channel] = 1; // TBD: set proper value for this case
        } break;    

        default:
        case embot::app::canprotocol::Message_aspoll_SET_CURR_TARE_bias::Mode::unknown:
        {
            ret = false;
        } break;        
    }
    

//    // retrieve strainstoredinfo
//    canbrdinfo.userdataread(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);    
//    pImpl->strainstoredinfo.setfullscale(info.channel, info.fullscale);
//    canbrdinfo.userdatawrite(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);

    return ret;    
}


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::Message_aspoll_GET_AMP_GAIN::ReplyInfo &replyinfo)
{  
    if(replyinfo.channel >= 6)
    {
        return false;
    }     
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();

    replyinfo.gain1 =  pImpl->strainstoredinfo.ampgain1[replyinfo.channel];
    replyinfo.gain2 =  pImpl->strainstoredinfo.ampgain2[replyinfo.channel];
    
    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::Message_aspoll_SET_AMP_GAIN::Info &info)
{ 
    if(info.channel >= 6)
    {
        return false;
    }     

    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    
    pImpl->strainstoredinfo.ampgain1[info.channel] = info.gain1;
    pImpl->strainstoredinfo.ampgain2[info.channel] = info.gain2;
   

//    // retrieve strainstoredinfo
//    canbrdinfo.userdataread(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);    
//    pImpl->strainstoredinfo.setfullscale(info.channel, info.fullscale);
//    canbrdinfo.userdatawrite(0, sizeof(pImpl->strainstoredinfo), &pImpl->strainstoredinfo);

    return true;    
}


bool embot::app::application::theSTRAIN::start()
{    
    return pImpl->start();
}


bool embot::app::application::theSTRAIN::stop()
{    
    return pImpl->stop();
}


bool embot::app::application::theSTRAIN::tick(std::vector<embot::hw::can::Frame> &replies)
{   
    return pImpl->tick(replies);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



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

#include "embot_app_application_theSkin.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot.h"
#include "embot_common.h"
#include "embot_binary.h"

#include <new>
#include "embot_sys_Timer.h"
#include "embot_sys_Action.h"
#include "embot_hw.h"
#include "embot_app_canprotocol.h"
#include "embot_app_canprotocol_skin_periodic.h"
#include "embot_app_canprotocol_analog_polling.h"
#include "embot_app_canprotocol_analog_periodic.h"

#include <cstdio>

#include "embot_app_theCANboardInfo.h"

#include "embot_app_application_theCANtracer.h"

#include "AD7147.h"

#include "embot_common.h"



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

//#define TEST_CANPRINT

static const std::uint8_t dotNumberOf = 12;
static const std::uint8_t trgNumberOf = 16;

struct TriangleCfg
{
    static const std::uint16_t  defaultCDCoffset = 0x2200;
    static const std::uint8_t   defaultSHIFT = 2;
    std::uint8_t                shift;
    std::uint8_t                dummy;
    std::uint16_t               cdcoffset; 
    TriangleCfg() : shift(defaultSHIFT), dummy(0), cdcoffset(defaultCDCoffset) {}
};

struct TriangleErr
{
    std::uint16_t   mask;   // bits 0-11 are for outofrange of the dot. bits 12, 13, 14 are for future use. bit 15 is error
    void reset() { mask = 0; } 
    void set_error() { embot::binary::bit::set(mask, 15); }    // error is in bit 15
    void set_outofrange(std::uint8_t dot) { if(dot < dotNumberOf) { embot::binary::bit::set(mask, dot); } }
    bool is_error() { return embot::binary::bit::check(mask, 15); }
    std::uint16_t get_outofrangemask() { return mask & 0x0fff; }
    TriangleErr() : mask(0) {}
};

struct Triangles
{
    std::uint16_t       activemask;
    std::uint16_t       connectedmask;
    TriangleCfg         config[trgNumberOf];
    static const std::uint8_t GAIN[dotNumberOf];
    static const std::uint8_t GAIN_PALM[dotNumberOf];
    if2hw_data_ad7147_t capoffsets[trgNumberOf][dotNumberOf];
    if2hw_data_ad7147_t rawvalues[trgNumberOf][dotNumberOf];
    TriangleErr         errors[trgNumberOf];
    Triangles() : activemask(0xffff), connectedmask(0xffff) { std::memset(capoffsets, 0, sizeof(capoffsets)); std::memset(rawvalues, 0, sizeof(rawvalues)); 
        uint16_t v = 0;
        for(int t=0; t<trgNumberOf; t++)
        for(int d=0; d<dotNumberOf; d++)
            rawvalues[t][d] = v++;    
    }
};

const std::uint8_t Triangles::GAIN[dotNumberOf] = {70 ,96, 83, 38, 38, 70, 0, 45, 77, 164, 0, 77}; // these gains are moltiplied by 128 with respect to matlab
const std::uint8_t Triangles::GAIN_PALM[dotNumberOf] = {0 ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // these gains are moltiplied by 128 with respect to matlab



//    static const std::uint8_t GAIN[12] = {70 ,96, 83, 38, 38, 70, 0, 45, 77, 164, 0, 77}; // these gains are moltiplied by 128 with respect to matlab
//    static const std::uint8_t GAIN_PALM[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};    
    
struct embot::app::application::theSkin::Impl
{ 
      
    Config config;
           
    bool ticking;
    
    bool forcecalibration;
    
    embot::sys::Timer *ticktimer;
    embot::sys::Action action;
    
    embot::app::canprotocol::analog::polling::Message_SKIN_SET_BRD_CFG::Info boardconfig;
    
    embot::app::canprotocol::analog::polling::Message_SKIN_SET_TRIANG_CFG::Info triangleconfigcommand;
    
    Triangles triangles;


    Impl() 
    {   
        ticktimer = new embot::sys::Timer; 

        setdefault();                
    }
    
   
    void setdefault()
    {
        ticking = false;  
        forcecalibration = false;

        boardconfig.skintype = embot::app::canprotocol::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::withTemperatureCompensation;
        boardconfig.txperiod = 50*embot::common::time1millisec;   
        boardconfig.noload = ad7147_dot_value_noload;         

        triangles.activemask = 0xffff; 
        triangles.connectedmask = 0xffff;        
    }
    
    bool start();
    bool stop();
    
    bool tick(std::vector<embot::hw::can::Frame> &replies);
    
    bool configtriangles(embot::app::canprotocol::analog::polling::Message_SKIN_SET_TRIANG_CFG::Info &trgcfg);
    
    bool calibrate();
    
    bool fill(embot::app::canprotocol::skin::periodic::Message_TRG::Info &info, const std::uint8_t trg);
    
    int computedrift(std::uint8_t trg, std::uint8_t dot);
                  
};


bool embot::app::application::theSkin::Impl::configtriangles(embot::app::canprotocol::analog::polling::Message_SKIN_SET_TRIANG_CFG::Info &trgcfg)
{    
    triangleconfigcommand = trgcfg;
    
    if((triangleconfigcommand.trgStart > 15) || (triangleconfigcommand.trgEnd > 15) || (triangleconfigcommand.trgStart > triangleconfigcommand.trgEnd))
    {
        return false;
    }
    
    for(std::uint8_t i=triangleconfigcommand.trgStart; i<=triangleconfigcommand.trgEnd; i++)
    {
                
        if(true == triangleconfigcommand.enabled)
        {
            embot::binary::bit::set(triangles.activemask, i);
        }
        else
        {
            embot::binary::bit::clear(triangles.activemask, i);
        }
        
        // we process shift and cdcoffset even if we have enabled == false ... as the old mtb3 application does
        
        triangles.config[i].shift = triangleconfigcommand.shift;
        if(triangles.config[i].cdcoffset != triangleconfigcommand.cdcOffset)
        {
            triangles.config[i].cdcoffset = triangleconfigcommand.cdcOffset;
            ad7147_set_cdcoffset(i, triangles.config[i].cdcoffset); 
        }
    }
    
    // also... disable tx
    if(true == ticking)
    {
        stop();
    }        
    
    calibrate();
        
    return true;    
}

bool embot::app::application::theSkin::Impl::calibrate()
{    
    
    // 1. get values
    // 2. call triangles init .. ad7147_calibrate() does that
    
    ad7147_calibrate();

    return true;    
}


bool embot::app::application::theSkin::Impl::start()
{
#if defined(TEST_CANPRINT)  
    embot::app::application::theCANtracer &tr = embot::app::application::theCANtracer::getInstance();      
    tr.start();
#endif
    
    ticktimer->start(boardconfig.txperiod, embot::sys::Timer::Type::forever, action);
    ticking = true;    
    return true;
}


bool embot::app::application::theSkin::Impl::stop()
{    
    ticktimer->stop();
    ticking = false;    
    return true;
}

int embot::app::application::theSkin::Impl::computedrift(std::uint8_t trg, std::uint8_t dot)
{
    std::uint8_t gain = 0; 
    if2hw_data_ad7147_t Tpad_base = 0;
    if2hw_data_ad7147_t Tpad = 0;
    
    int drift = 0;
    
    switch(boardconfig.skintype)
    {        
        case embot::app::canprotocol::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::withTemperatureCompensation:
        {
            gain = triangles.GAIN[dot];  
            Tpad_base = triangles.capoffsets[trg][6]; // in original mtb3 code there is ADCRESULT_S6 which is defined as 6
            Tpad = triangles.rawvalues[trg][6];
                
            if(Tpad > Tpad_base)
            {
                drift = ( ( (Tpad - Tpad_base) >> 2) * gain ) >> 5;
            }
            else
            {
                drift = -( ( (Tpad_base - Tpad) >> 2) * gain ) >> 5;
            }     
        } break;
        
        case embot::app::canprotocol::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::palmFingerTip:
        {
            gain = triangles.GAIN_PALM[dot];  // in original mtb3 code GAIN_PALM[] is ... all zero, hence drift is ZERO! 
            Tpad_base = triangles.capoffsets[trg][11]; // in original mtb3 code there is ADCRESULT_S11 which is defined as 11
            Tpad = triangles.rawvalues[trg][11];
                
            if(Tpad > Tpad_base)
            {
                drift = ( ( (Tpad - Tpad_base) >> 2) * gain ) >> 5;
            }
            else
            {
                drift = -( ( (Tpad_base - Tpad) >> 2) * gain ) >> 5;
            }     
        } break;
        
        default:
        case embot::app::canprotocol::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::testmodeRAW:
        case embot::app::canprotocol::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::withoutTempCompensation:
        {
            drift = 0;
        } break;                
    }

    return drift;    
}


bool embot::app::application::theSkin::Impl::fill(embot::app::canprotocol::skin::periodic::Message_TRG::Info &info, const std::uint8_t trg)
{
    bool ret = true;
    if2hw_data_ad7147_t *the12rawvalues = ad7147_get12rawvaluesoftriangle(trg);
    if2hw_data_ad7147_t *the12capoffsets = ad7147_get12capoffsetsoftriangle(trg);
    if((nullptr == the12rawvalues) || (nullptr == the12capoffsets))
    {
        return false;
    }
    
    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
    info.trianglenum = trg;
    info.outofrangemaskofthe12s = 0;        // none
    info.notackmaskofthe12s = 0;            // none
    info.notconnectedmaskofthe12s = 0;      // none
    std::memset(info.the12s, 0, sizeof(info.the12s));
    
    
    if(embot::app::canprotocol::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::testmodeRAW == boardconfig.skintype)
    {   // just copy what is into the rawvalues. just the lsb...
        for(std::uint8_t i=0; i<dotNumberOf; i++)
        {
            info.the12s[i] = (the12rawvalues[i]) & 0xff;
        } 
                
        return ret;
    }

    // all other cases require a compensation

    int UP_LIMIT = static_cast<int>(ad7147_dot_value_max-boardconfig.noload) << triangles.config[trg].shift;
    int BOT_LIMIT = static_cast<int>(boardconfig.noload) << triangles.config[trg].shift;
       
    int value = 0; 
    int drift = 0;
 
    for(std::uint8_t i=0; i<dotNumberOf; i++)
    {
        drift = computedrift(trg, i);
        value = static_cast<int>(the12rawvalues[i]) - static_cast<int>(the12capoffsets[i]) - drift;
            
        if(value <= -UP_LIMIT)
        {
            info.the12s[i] = ad7147_dot_value_max;  // out of range, pressure too low
        }
        else if(value >= BOT_LIMIT)
        {
            info.the12s[i] = ad7147_dot_value_min;  // out of range, pressure too high
        }
        else // if((value > -UP_LIMIT) && (value < BOT_LIMIT))
        {
            info.the12s[i] = boardconfig.noload - (value >> triangles.config[trg].shift);
        }
        
        // check vs a strange value (original comment: if the sensor is far from the limits -> taxel could be broken)
        if((value <= -(UP_LIMIT<<1)) || (value >= (BOT_LIMIT << 1)))
        {
            embot::binary::bit::set(info.outofrangemaskofthe12s, i);  
        }                    
    
        // check if any errors on rawvalues[i]              
        if(0xffff == the12rawvalues[i])
        {
            embot::binary::bit::set(info.notconnectedmaskofthe12s, i);                    
        }
        else if((0 == the12rawvalues[i]) && (0 != the12capoffsets[i]))
        {
            embot::binary::bit::set(info.notackmaskofthe12s, i);                    
        }
    }
    
    
//    bool triangleISnotconnected = false;
//    if(dotNumberOf == embot::binary::bit::count(info.notconnectedmaskofthe12s))
//    {
//        triangleISnotconnected = true;
//    }  


    // now ... the original mtb3 code does that, and i do the same .... even if i doubt about that.
    // if there is any error in triangle trg such as: any dot does not ack and none is notconnected
    // we increment a variable errorcounter. when errorcounter is 5, we:
    // 1. set the cdcoffset on that triangle, ????
    // 2. force a new CALIB on every triangle.
    // BUT ... I SAY: why settig the cdc offset only for one?.
    // then .... much better to see dot by dot: if at least one ha not ached & is connected ....
    
           
    std::uint16_t notackedBUTconnected = info.notackmaskofthe12s & ~info.notconnectedmaskofthe12s;
    
    if((0 != notackedBUTconnected))
    {
        static std::uint8_t errorcounter = 0;
        
        errorcounter++;  
        if(errorcounter >= 5)
        {
            forcecalibration = true;
            errorcounter = 0;
        }        
    }
    
         
    return ret;    
}


bool embot::app::application::theSkin::Impl::tick(std::vector<embot::hw::can::Frame> &replies)
{   
    if(false == ticking)
    {
        return false;
    }
    
#if defined(TEST_CANPRINT)     
    embot::app::application::theCANtracer &tr = embot::app::application::theCANtracer::getInstance();
    tr.stop(std::string("u"), replies);    
    tr.start();
//    tr.print(std::string("ciao uomo, come stai? io bene!xxx devo dire. anche se il protocollo tronca oltre xxx"), replies);
#endif  
    
    embot::app::application::theCANtracer &tr = embot::app::application::theCANtracer::getInstance(); 
    tr.start();
    ad7147_acquire();  
    tr.stop(std::string("u"), replies);       

#if 1
    
    // if requested ... calibrate
    if(true == forcecalibration)
    {
        forcecalibration = false;
        calibrate();
    }
    
    // acquire and transmit ...
    
    embot::app::canprotocol::skin::periodic::Message_TRG msg;
    embot::app::canprotocol::skin::periodic::Message_TRG::Info info;
    for(std::uint8_t t=0; t<trgNumberOf; t++)
    {
        if(true == embot::binary::bit::check(triangles.activemask, t))
        {                
            if(true == fill(info, t))
            {
                msg.load(info);
                
                embot::hw::can::Frame frame0;
                embot::hw::can::Frame frame1;
                msg.get(frame0, frame1);
                replies.push_back(frame0);
                replies.push_back(frame1);
            }
        }
    }

#endif

    
    return true;    
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theSkin::theSkin()
: pImpl(new Impl)
{       

}

   
        
bool embot::app::application::theSkin::initialise(Config &config)
{
    pImpl->config = config;
    
    pImpl->action.set(embot::sys::Action::EventToTask(pImpl->config.tickevent, pImpl->config.totask));

    
    pImpl->triangles.activemask = 0xffff;
    
    ad7147_init(pImpl->triangles.rawvalues, pImpl->triangles.capoffsets);
    
    pImpl->triangles.connectedmask = ad7147_gettrianglesconnectedmask();
    
    
    return true;
}
  

bool embot::app::application::theSkin::configure(embot::app::canprotocol::analog::polling::Message_SKIN_SET_BRD_CFG::Info &brdcfg)
{
    pImpl->boardconfig = brdcfg;
    
    // as in application of mtb3:
    // 1. disable all triangles.
    // 2. get the noload.
    // 3. get the txperiod
    // 4. get type of board ...
    // 5. dont stop tx if any ... ? boh. maybe better stop and restart with new value
    
    // ? should i?
    pImpl->triangles.activemask = 0;
            
    if(true == pImpl->ticking)
    {
        // stop and start with new period, as mtb3 does.
        stop();
        start();
    }
  
    return true;    
}



bool embot::app::application::theSkin::configure(embot::app::canprotocol::analog::polling::Message_SKIN_SET_TRIANG_CFG::Info &trgcfg)
{ 
    return pImpl->configtriangles(trgcfg);
}

bool embot::app::application::theSkin::configure(embot::app::canprotocol::analog::polling::Message_SKIN_OBSOLETE_TACT_SETUP::Info &tactsetup)
{    
    if(true == pImpl->ticking)
    {
        // stop
        stop();
    }
    
    pImpl->setdefault();
    
    pImpl->boardconfig.txperiod = tactsetup.txperiod;
    
    
    for(std::uint8_t i=0; i<trgNumberOf; i++)
    {                       
        // we process cdcoffset even if we have enabled == false ... as the old mtb3 application does

        if(pImpl->triangles.config[i].cdcoffset != tactsetup.cdcOffset)
        {
            pImpl->triangles.config[i].cdcoffset = tactsetup.cdcOffset;
            ad7147_set_cdcoffset(i, pImpl->triangles.config[i].cdcoffset); 
        }
    }
         
    pImpl->calibrate();    
    
    // config board w/ default values.
    // config all triangles w/ default values.
    // set calib with cdcfoffset
    // start tx at 40 ms.
    
    start();
    
    return true;    
}


bool embot::app::application::theSkin::start()
{    
    return pImpl->start();
}


bool embot::app::application::theSkin::stop()
{    
    return pImpl->stop();
}

bool embot::app::application::theSkin::tick(std::vector<embot::hw::can::Frame> &replies)
{   
    return pImpl->tick(replies);
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



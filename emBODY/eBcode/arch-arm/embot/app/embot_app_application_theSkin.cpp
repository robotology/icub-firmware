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


#include "embot_core.h"
#include "embot_core_binary.h"

#include <new>
#include "embot_os_Timer.h"
#include "embot_os_Action.h"
#include "embot_hw.h"
#include "embot_prot_can.h"
#include "embot_prot_can_skin_periodic.h"
#include "embot_prot_can_analog_polling.h"
#include "embot_prot_can_analog_periodic.h"

#include <cstdio>

#include "embot_app_theCANboardInfo.h"

#include "embot_app_application_theCANtracer.h"

#include "AD7147.h"

#include "embot_core.h"



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

//#define TEST_CANPRINT
std::uint16_t dbg = 0;
constexpr std::uint8_t dotNumberOf = 12;
constexpr std::uint8_t trgNumberOf = 16; //4 for each sda. This is used in the CAN message forming so it's locked to 16
uint16_t trianglesOrder[16]  = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
uint16_t origTrianglesOrder[16]  = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
uint16_t altTrianglesOrder[16]  = {0,4,8,12,16,10,11,13,15,1,2,3,5,6,7,9}; //to reshuffle the triangles on-the-run


struct TriangleCfg
{
    static constexpr std::uint16_t  defaultCDCoffset = 0x2200;
    static constexpr std::uint8_t   defaultSHIFT = 2;
    std::uint8_t                shift;
    std::uint8_t                dummy;
    std::uint16_t               cdcoffset; 
    TriangleCfg() : shift(defaultSHIFT), dummy(0), cdcoffset(defaultCDCoffset) {}
};

struct TriangleErr
{
    std::uint16_t   mask;   // bits 0-11 are for outofrange of the dot. bits 12, 13, 14 are for future use. bit 15 is error
    void reset() { mask = 0; } 
    void set_error() { embot::core::binary::bit::set(mask, 15); }    // error is in bit 15
    void set_outofrange(std::uint8_t dot) { if(dot < dotNumberOf) { embot::core::binary::bit::set(mask, dot); } }
    bool is_error() { return embot::core::binary::bit::check(mask, 15); }
    std::uint16_t get_outofrangemask() { return mask & 0x0fff; }
    TriangleErr() : mask(0) {}
};

struct Triangles
{
    std::uint16_t       activemask;
    std::uint16_t       connectedmask;
    TriangleCfg         config[trgNumberOf];
    TriangleCfg         config5th[4];
    static const std::uint8_t GAIN[dotNumberOf];
    static const std::uint8_t GAIN_PALM[dotNumberOf];
    static const std::uint8_t GAIN_V2[dotNumberOf];
    if2hw_data_ad7147_t capoffsets[trgNumberOf][dotNumberOf];
    if2hw_data_ad7147_t rawvalues[trgNumberOf][dotNumberOf];
    if2hw_data_ad7147_t capoffsets5th[4][dotNumberOf];
    if2hw_data_ad7147_t rawvalues5th[4][dotNumberOf];
    TriangleErr         errors[trgNumberOf];
    Triangles() : activemask(0xffff), connectedmask(0xffff) { std::memset(capoffsets, 0, sizeof(capoffsets)); std::memset(rawvalues, 0, sizeof(rawvalues)); 
        uint16_t v = 0;
        for(int t=0; t<trgNumberOf; t++)
        for(int d=0; d<dotNumberOf; d++)
            rawvalues[t][d] = v++;    
    }
};

//uint16_t testraw[20][12] = {0};
//uint16_t testoff[20][12] = {0};

const std::uint8_t Triangles::GAIN[dotNumberOf] = {70 ,96, 83, 38, 38, 70, 0, 45, 77, 164, 0, 77}; // these gains are moltiplied by 128 with respect to matlab
const std::uint8_t Triangles::GAIN_PALM[dotNumberOf] = {0 ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // these gains are moltiplied by 128 with respect to matlab
const std::uint8_t Triangles::GAIN_V2[dotNumberOf] = {58 , 91, 39, 23, 35, 90, 0, 38, 68, 115, 0, 86}; // from simeone.dussoni on 30jul19


    
struct embot::app::application::theSkin::Impl
{ 
      
    Config config {};
           
    bool ticking { false};
    
    bool forcecalibration {false};
    
    embot::os::Timer *ticktimer {nullptr};
    embot::os::Action action {};
    
    embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG::Info boardconfig {};
    
    embot::prot::can::analog::polling::Message_SKIN_SET_TRIANG_CFG::Info triangleconfigcommand {};
    
    Triangles triangles {};
    
    bool hwisinitted {false};
    bool deactivated {false};


    Impl() 
    {   
        ticktimer = new embot::os::Timer; 

        setdefault();                
    }
    
   
    void setdefault()
    {
        ticking = false;  
        forcecalibration = false;

        boardconfig.skintype = embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::withTemperatureCompensation;
        boardconfig.txperiod = 50*embot::core::time1millisec;   
        boardconfig.noload = ad7147_dot_value_noload;         

        triangles.activemask = 0xffff; 
        triangles.connectedmask = 0xffff;        
    }
    
    bool initialise(Config &cfg, bool deferredHWinit);
    bool deactivate();
    bool hwinit();
    
    bool start();
    bool stop();
    
    bool tick(std::vector<embot::prot::can::Frame> &replies);
    
    bool configtriangles(const embot::prot::can::analog::polling::Message_SKIN_SET_TRIANG_CFG::Info &trgcfg);
    
    bool calibrate();
    
    bool fill(embot::prot::can::skin::periodic::Message_TRG::Info &info, const std::uint8_t trg);
    
    int computedrift(std::uint8_t trg, std::uint8_t dot);
                  
};

bool embot::app::application::theSkin::Impl::initialise(Config &cfg, bool deferredHWinit)
{
    config = cfg;
    
    action.load(embot::os::EventToThread(config.tickevent, config.totask));    
    
    if(false == deferredHWinit)
    {
        hwinit();
    }
    
    return true;
}

bool embot::app::application::theSkin::Impl::deactivate()
{
    if(false == hwisinitted)
    {
        deactivated = true;
    }
    else
    {
        // cannot deactivate because hw was already initted.
    }
    return true;
}


bool embot::app::application::theSkin::Impl::hwinit()
{
    if((true == deactivated) || (true == hwisinitted))
    {
        return true;
    }
    
    triangles.activemask = 0xffff;
    
    ad7147_init(triangles.rawvalues, triangles.capoffsets, triangles.rawvalues5th, triangles.capoffsets5th);
    
    triangles.connectedmask = ad7147_gettrianglesconnectedmask();
    
    hwisinitted = true;
    
    return true;
}


bool embot::app::application::theSkin::Impl::configtriangles(const embot::prot::can::analog::polling::Message_SKIN_SET_TRIANG_CFG::Info &trgcfg)
{ 
   
    triangleconfigcommand = trgcfg;
    
    
    // start and end must be in order (start <= end).
    if(triangleconfigcommand.trgStart > triangleconfigcommand.trgEnd)
    {
        return false;
    }
    
    // start is always positive as it is a uint8_t. end must be lower than triangle max number (end < 16). 
    if(triangleconfigcommand.trgEnd >= trgNumberOf)
    {
        return false;
    }
    
    for(std::uint8_t i=triangleconfigcommand.trgStart; i<=triangleconfigcommand.trgEnd; i++)
    {
                
        if(true == triangleconfigcommand.enabled)
        {
            embot::core::binary::bit::set(triangles.activemask, i);
        }
        else
        {
            embot::core::binary::bit::clear(triangles.activemask, i);
        }
        
        // we process shift and cdcoffset even if we have enabled == false ... as the old mtb3 application does
        if(trianglesOrder[i] >= triangles_max_num)
        {        
            triangles.config5th[trianglesOrder[i]-triangles_max_num].shift = triangleconfigcommand.shift;
            if(triangles.config5th[trianglesOrder[i]-triangles_max_num].cdcoffset != triangleconfigcommand.cdcOffset)
            {
                triangles.config5th[trianglesOrder[i]-triangles_max_num].cdcoffset = triangleconfigcommand.cdcOffset;
                ad7147_set_cdcoffset(trianglesOrder[i], triangles.config5th[trianglesOrder[i]-triangles_max_num].cdcoffset);         
            }    
        }
        else
        {
            triangles.config[trianglesOrder[i]].shift = triangleconfigcommand.shift;        
            if(triangles.config[trianglesOrder[i]].cdcoffset != triangleconfigcommand.cdcOffset)
            {                              
                triangles.config[trianglesOrder[i]].cdcoffset = triangleconfigcommand.cdcOffset;
                ad7147_set_cdcoffset(trianglesOrder[i], triangles.config[trianglesOrder[i]].cdcoffset);         
            }
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
    
    embot::os::Timer::Config cfg(boardconfig.txperiod, action, embot::os::Timer::Mode::forever);
    ticktimer->start(cfg);

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
    std::uint8_t posTemp = 6; 
    if2hw_data_ad7147_t Tpad_base = 0;
    if2hw_data_ad7147_t Tpad = 0;
    
    int drift = 0;
    
    switch(boardconfig.skintype)
    {        
        case embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::withTemperatureCompensation:
        case embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::palmFingerTip:
        case embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::withTemperatureCompensationV2:
        case embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::ergoHand:
        {
						
            if(embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::withTemperatureCompensation == boardconfig.skintype)
            {
                gain = triangles.GAIN[dot];  
                posTemp = 6;
            }
            else if(embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::palmFingerTip == boardconfig.skintype)
            {
                gain = triangles.GAIN_PALM[dot];
                posTemp = 11;
            }
            else if(embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::ergoHand == boardconfig.skintype)
            {
                if(trg>4)
                    { //are the indexes of the triangles 
                        gain = triangles.GAIN_V2[dot];
                        posTemp = 6;
                    }
     			else 
                    { // are the indexes of the 3DMID
                        gain = triangles.GAIN_PALM[dot];
                        posTemp = 11;
                    } 
            }
            else
            {
                gain = triangles.GAIN_V2[dot];
                posTemp = 6;
            }
                        
						// temperature compensation does not apply to fingertips i.e. triangles 0,4,8,12,16 thus this module-16 op is a bug but not having any effect: 
            Tpad_base = triangles.capoffsets[trianglesOrder[trg]%16][posTemp]; // in original mtb3 code there is ADCRESULT_S6 which is defined as 6
            Tpad = triangles.rawvalues[trianglesOrder[trg]%16][posTemp]; 
                
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
        case embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::testmodeRAW:
        case embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::withoutTempCompensation:
        {
            drift = 0;
        } break;                
    }

    return drift;    
}


bool embot::app::application::theSkin::Impl::fill(embot::prot::can::skin::periodic::Message_TRG::Info &info, const std::uint8_t trg)
{
    bool ret = true;
    if(embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::ergoHand == boardconfig.skintype)
		{
				for (uint8_t i = 0; i<16; i++) trianglesOrder[i] = altTrianglesOrder[i];
		}
		else 
		{
				for (uint8_t i = 0; i<16; i++) trianglesOrder[i] = origTrianglesOrder[i];
		}

    if2hw_data_ad7147_t *the12rawvalues = ad7147_get12rawvaluesoftriangle(trianglesOrder[trg]);
    if2hw_data_ad7147_t *the12capoffsets = ad7147_get12capoffsetsoftriangle(trianglesOrder[trg]);
    
    
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
    
    
    if(embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG::SkinType::testmodeRAW == boardconfig.skintype)
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
            embot::core::binary::bit::set(info.outofrangemaskofthe12s, i);  
        }                    
    
        // check if any errors on rawvalues[i]              
        if(0xffff == the12rawvalues[i])
        {
            embot::core::binary::bit::set(info.notconnectedmaskofthe12s, i);                    
        }
        else if((0 == the12rawvalues[i]) && (0 != the12capoffsets[i]))
        {
            embot::core::binary::bit::set(info.notackmaskofthe12s, i);                    
        }
    }
    
    
//    bool triangleISnotconnected = false;
//    if(dotNumberOf == embot::core::binary::bit::count(info.notconnectedmaskofthe12s))
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


bool embot::app::application::theSkin::Impl::tick(std::vector<embot::prot::can::Frame> &replies)
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
    
//    embot::app::application::theCANtracer &tr = embot::app::application::theCANtracer::getInstance(); 
//    tr.start();
    ad7147_acquire();  
//    tr.stop(std::string("u"), replies);       

#if 1
    
    // if requested ... calibrate
    if(true == forcecalibration)
    {
        forcecalibration = false;
        calibrate();
    }
    
    // acquire and transmit ...
    
    embot::prot::can::skin::periodic::Message_TRG msg;
    embot::prot::can::skin::periodic::Message_TRG::Info info;
    for(std::uint8_t t=0; t<trgNumberOf; t++)
    {
        if(true == embot::core::binary::bit::check(triangles.activemask, t))
        {                
            if(true == fill(info, t))
            {
                msg.load(info);
                
                embot::prot::can::Frame frame0;
                embot::prot::can::Frame frame1;
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


embot::app::application::theSkin& embot::app::application::theSkin::getInstance()
{
    static theSkin* p = new theSkin();
    return *p;
}

embot::app::application::theSkin::theSkin()
//    : pImpl(new Impl)
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::application::theSkin::~theSkin() { }
   
        
bool embot::app::application::theSkin::initialise(Config &config, bool deferredHWinit)
{    
    return pImpl->initialise(config, deferredHWinit);
}



bool embot::app::application::theSkin::deactivate()
{    
    return pImpl->deactivate();
}
  

bool embot::app::application::theSkin::start()
{    
    return pImpl->start();
}


bool embot::app::application::theSkin::stop()
{    
    return pImpl->stop();
}

bool embot::app::application::theSkin::tick(std::vector<embot::prot::can::Frame> &replies)
{   
    return pImpl->tick(replies);
}


// interface to CANagentSKIN

bool embot::app::application::theSkin::isactive() const
{
    return !pImpl->deactivated;
}

const std::string& embot::app::application::theSkin::status() const
{
    static const std::string sta[] = 
    {
        "SKIN active", "SKIN deactivated by POS"
    };
    
    return pImpl->deactivated ? sta[1] : sta[0];
}

bool embot::app::application::theSkin::set(const embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG::Info &info)
{
    if((false == pImpl->deactivated) && (false == pImpl->hwisinitted))
    {
        // it is not deactivated and hw not initted yet, so: i enable hw
        pImpl->hwinit();
    }
    else if(true == pImpl->deactivated)
    {
        // cannot do anything
        return true;
    }
    
    pImpl->boardconfig = info;
    
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


bool embot::app::application::theSkin::set(const embot::prot::can::analog::polling::Message_SKIN_SET_TRIANG_CFG::Info &info)
{
    if((false == pImpl->deactivated) && (false == pImpl->hwisinitted))
    {
        // it is not deactivated and hw not initted yet, so: i enable hw
        pImpl->hwinit();
    }
    else if(true == pImpl->deactivated)
    {
        // cannot do anything
        return true;
    }
    
    return pImpl->configtriangles(info);
}

  
bool embot::app::application::theSkin::set(const embot::prot::can::analog::polling::Message_SET_TXMODE::Info &info)
{
    if((false == pImpl->deactivated) && (false == pImpl->hwisinitted))
    {
        // it is not deactivated and hw not initted yet, so: i enable hw
        pImpl->hwinit();
    }
    else if(true == pImpl->deactivated)
    {
        // cannot do anything
        return true;
    }
    
    if(true == info.transmit)
    {
        start();        
    }
    else
    {
        stop();     
    }
                  
    return true;
}

 
bool embot::app::application::theSkin::set(const embot::prot::can::analog::polling::Message_SKIN_OBSOLETE_TACT_SETUP::Info &info)
{
    if((false == pImpl->deactivated) && (false == pImpl->hwisinitted))
    {
        // it is not deactivated and hw not initted yet, so: i enable hw
        pImpl->hwinit();
    }
    else if(true == pImpl->deactivated)
    {
        // cannot do anything
        return true;
    }
    
    if(true == pImpl->ticking)
    {
        // stop
        stop();
    }
    
    pImpl->setdefault();
    
    pImpl->boardconfig.txperiod = info.txperiod;
    
    
    for(std::uint8_t i=0; i<trgNumberOf; i++)
    {                       
        // we process cdcoffset even if we have enabled == false ... as the old mtb3 application does

        if(pImpl->triangles.config[i].cdcoffset != info.cdcOffset)
        {
            pImpl->triangles.config[i].cdcoffset = info.cdcOffset;
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



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



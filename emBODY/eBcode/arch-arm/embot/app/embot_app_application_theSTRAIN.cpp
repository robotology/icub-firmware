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
    
    // it is a holder of config data which is eventally saved to eeprom. 
    // this struct is without methods so that i am sure its sizeof() is the sum of the variables. 
    // this struct it is managed by the StrainConfig class
    struct strainConfigData_t
    {   
        std::uint32_t       key;
        char                serial[8];              // the serial number of the strain           
        std::int16_t        fullscale[6];           // the fullscales of the measuraments
        std::int16_t        tare[6];                // the stored tare
        std::uint16_t       offset[6];              // the offset generated with the dac
        std::uint8_t        matrixgain;             // the gain to be applied to the matrix as an unsigned integer value
        std::int16_t        matrix[6][6];           // transformation matrix from the 6 strain-gauge values to the 6 force-torque values  
        std::uint16_t       gainofamplifier[6][2];  // the gains used by .... 6sg maybe?                    
    };
        
    class StrainConfigData
    {   // only eepromread() and eepromwrite() operate on eeprom
        public:
            
        strainConfigData_t      data;           // we save it in eeprom when requested
        bool                    synched;        // true if data is equal to what is in eeprom

        // methods
        
        bool clear()
        {
            std::memset(&data, 0, sizeof(data));
            data.key = validityKey;
            synched = false;
            return synched;
        }
        
        StrainConfigData()
        {
            clear();
        }
        
        ~StrainConfigData() {}
            
        void matrix_set_identity()
        {   // value 0x7fff is close to 1 = 1-2^(-15)            
            std::memset(data.matrix, 0, sizeof(data.matrix));
            data.matrix[0][0] = data.matrix[1][1] = data.matrix[2][2] = data.matrix[3][3] = data.matrix[4][4] = data.matrix[5][5] = 0x7fff;
            synched = false;
        }
        
        void matrixgain_set(std::uint8_t g)
        {            
            data.matrixgain = g;
            synched = false;
        }
        
        void matrix_set(std::uint8_t r, std::uint8_t c, std::uint16_t value)
        {   
            if((r<6) && (c<6))
            {
                data.matrix[r][c] = value;
                synched = false;
            }
        }
        
        bool def_values()
        {
            clear();
            data.key = validityKey;
            std::snprintf(data.serial, sizeof(data.serial), "SN0000");
            data.fullscale[0] = data.fullscale[1] = data.fullscale[2] = data.fullscale[3] = data.fullscale[4] = data.fullscale[5] = 0x7fff;
            std::memset(data.tare, 0, sizeof(data.tare)); 
            matrix_set_identity();
            matrixgain_set(1);
            for(int i=0; i<6; i++) for(int j=0; j<2; j++) data.gainofamplifier[i][j] = 1;
            synched = false;
            return synched;
        }
        
        bool eeprom_read()
        {
            embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
            canbrdinfo.userdataread(0, sizeof(data), &data);
            synched = true;
            
            if(validityKey != data.key)
            {   // if it is not valid, then ... set default and impose validity key      
                def_values();
                data.key = validityKey;
                eeprom_write();
            }
                        
            return synched;
        }
        
        bool eeprom_write()
        {
            data.key = validityKey;
            embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance(); 
            canbrdinfo.userdatawrite(0, sizeof(data), &data);
            synched = true;
            return synched;
        }
        
        void serial_set(const char *str) 
        {
            std::snprintf(data.serial, sizeof(data.serial), "%s", str);
            synched = false;
        }
        
        const char * serial_get()
        {
            return data.serial;
        }
        
        std::int16_t fullscale_get(std::uint8_t channel)
        {
            if(channel < 6)
                return data.fullscale[channel];
            else
                return 0;
        }
        
        void fullscale_set(std::uint8_t channel, std::int16_t value)
        {
            if(channel < 6)
            {
                data.fullscale[channel] = value;
                synched = false;
            }
        }
        
        void tare_set(std::uint8_t channel, std::int16_t value)
        {
            if(channel < 6)
            {
                data.tare[channel] = value;
                synched = false;
            }
        }
        
        void tare_clear()
        {
            std::memset(data.tare, 0, sizeof(data.tare));
            synched = false;
        }
        
        void offset_set(std::uint8_t channel, std::uint16_t value)
        {
            if(channel < 6)
            {
                data.offset[channel] = value;
                synched = false;
            }
        }
        
        void gainofamplifier_set(std::uint8_t channel, std::uint16_t g0, std::uint16_t g1)
        {
            if(channel < 6)
            {
                data.gainofamplifier[channel][0] = g0;
                data.gainofamplifier[channel][1] = g1;
                synched = false;
            }            
        }
        
    };
    
    #warning -> complete content of StrainRuntimeData_t 
    
    
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
    
    // it is a holder of volatile data used in runtime. we use the same approach as for config data
    struct StrainRuntimeData_t
    {
        embot::common::Time txperiod;
        
        std::int16_t        tare[6];            // the tare (offset) as measured in runtime
        std::uint16_t       adcvalue[6];        // the values as acquired by the adc. it is uint16_t in original strain code
        std::int16_t        torqueforce[6];
        
        
        TripleValue force;
        TripleValue torque;
        
        bool TXcalibData;
        bool TXuncalibData;
        
        // methods
        void clear() { 
            std::memset(tare, 0, sizeof(tare)); 
            std::memset(adcvalue, 0, sizeof(adcvalue));
            std::memset(torqueforce, 0, sizeof(torqueforce));
            force.reset();
            torque.reset();
            TXcalibData = false; 
            TXuncalibData = false; 
        }       
        StrainRuntimeData_t() { clear(); }
    };
    
    class StrainRuntimeData
    {
        public:

        StrainRuntimeData_t     data;   
        
        // methods
        
        bool clear()
        {
            data.clear();
            return true;
        }
        
        StrainRuntimeData()
        {
            clear();
        }
        
        ~StrainRuntimeData() {}
            
        bool setdefault()
        {
            clear();
            return true;
        }
                
    };    
    
    
   
    
      
    Config config;
           
    bool ticking;
    
    
    embot::sys::Timer *ticktimer;
    embot::sys::Action action;
        
    
//    StrainStoredInfo strainstoredinfo;
//    bool infosavedoneeprom;
    
    std::uint8_t canaddress;
       
       
    
    // i put in here some values proper of the STRAIN. they may be many.... hence better to organise them in a struct later on.
    
    
    StrainRuntimeData runtimedata;
    StrainConfigData configdata;
    

    Impl() 
    {   
        ticking = false;  

        ticktimer = new embot::sys::Timer;   

        canaddress = 0;  
                
        configdata.clear();
        runtimedata.clear();
        
        runtimedata.data.txperiod = 50*embot::common::time1millisec;            
    }
    
   
    bool start(embot::app::canprotocol::analog::polling::Message_SET_TXMODE::StrainMode &mode);
    bool stop();
    
    bool tick(std::vector<embot::hw::can::Frame> &replies);
    
    bool configure();
    
    bool acquisition();
    
    bool fill(embot::app::canprotocol::analog::periodic::Message_UNCALIBFORCE_VECTOR_DEBUGMODE::Info &info);
    bool fill(embot::app::canprotocol::analog::periodic::Message_UNCALIBTORQUE_VECTOR_DEBUGMODE::Info &info);
    bool fill(embot::app::canprotocol::analog::periodic::Message_FORCE_VECTOR::Info &info);
    bool fill(embot::app::canprotocol::analog::periodic::Message_TORQUE_VECTOR::Info &info);
                      
};



bool embot::app::application::theSTRAIN::Impl::start(embot::app::canprotocol::analog::polling::Message_SET_TXMODE::StrainMode &mode)
{    
    if(true == ticking)
    {
        ticktimer->stop();
        ticking =  false;
    }
    
    runtimedata.data.TXcalibData = false;
    runtimedata.data.TXuncalibData = false;
    
    switch(mode)
    {
        case embot::app::canprotocol::analog::polling::Message_SET_TXMODE::StrainMode::txCalibrated:
        {
            runtimedata.data.TXcalibData = true; runtimedata.data.TXuncalibData = false;
        } break;
        
        case embot::app::canprotocol::analog::polling::Message_SET_TXMODE::StrainMode::txUncalibrated:
        {
            runtimedata.data.TXcalibData = false; runtimedata.data.TXuncalibData = true;
        } break;  
        
        case embot::app::canprotocol::analog::polling::Message_SET_TXMODE::StrainMode::txAll:
        {
            runtimedata.data.TXcalibData = true; runtimedata.data.TXuncalibData = true;
        } break; 

        default:
        {
            runtimedata.data.TXcalibData = false; runtimedata.data.TXuncalibData = false;
        } break;             
    }    
    
    if((false == runtimedata.data.TXcalibData) && (false == runtimedata.data.TXuncalibData))
    {
        ticking = false;
        return true;        
    }
    
    ticktimer->start(runtimedata.data.txperiod, embot::sys::Timer::Type::forever, action);
    ticking = true;    
    return true;
}


bool embot::app::application::theSTRAIN::Impl::stop()
{ 
    runtimedata.data.TXcalibData = false;
    runtimedata.data.TXuncalibData = false;
    
    ticktimer->stop();
    ticking = false;    
    return true;
}


bool embot::app::application::theSTRAIN::Impl::fill(embot::app::canprotocol::analog::periodic::Message_UNCALIBFORCE_VECTOR_DEBUGMODE::Info &info)
{
    bool ret = true;
    
    info.canaddress = canaddress;
    info.x = runtimedata.data.adcvalue[3];
    info.y = runtimedata.data.adcvalue[4];
    info.z = runtimedata.data.adcvalue[5];
         
    return ret;    
}


bool embot::app::application::theSTRAIN::Impl::fill(embot::app::canprotocol::analog::periodic::Message_UNCALIBTORQUE_VECTOR_DEBUGMODE::Info &info)
{
    bool ret = true;

    info.canaddress = canaddress;
    info.x = runtimedata.data.adcvalue[0];
    info.y = runtimedata.data.adcvalue[1];
    info.z = runtimedata.data.adcvalue[2];
    
    return ret;    
}

bool embot::app::application::theSTRAIN::Impl::fill(embot::app::canprotocol::analog::periodic::Message_FORCE_VECTOR::Info &info)
{
    bool ret = true;
    
    info.canaddress = canaddress;
    info.x = runtimedata.data.force.x;
    info.y = runtimedata.data.force.y;
    info.z = runtimedata.data.force.z;
         
    return ret;    
}


bool embot::app::application::theSTRAIN::Impl::fill(embot::app::canprotocol::analog::periodic::Message_TORQUE_VECTOR::Info &info)
{
    bool ret = true;

    info.canaddress = canaddress;
    info.x = runtimedata.data.torque.x;
    info.y = runtimedata.data.torque.y;
    info.z = runtimedata.data.torque.z;
    
    return ret;    
}


bool embot::app::application::theSTRAIN::Impl::acquisition()
{
    #warning TODO: perform hw acquisition of all adc values and computation of ft values. 
    

    
    // acquire from adc and put inside adcvalue
    runtimedata.data.adcvalue[0]++;
    runtimedata.data.adcvalue[1]++;
    runtimedata.data.adcvalue[2]++;
    runtimedata.data.adcvalue[3]++;
    runtimedata.data.adcvalue[4]++;
    runtimedata.data.adcvalue[5]++;
    
    
    // prepare uncalib data
    // so far just adcvalue[]

    
    // prepare calib data
    
    // formula is:
    // torqueforce = M * (adcvalue + calibtare ) + currtare
    
    runtimedata.data.torqueforce[0] = runtimedata.data.adcvalue[0] & 0xfff7;
    runtimedata.data.torqueforce[1] = runtimedata.data.adcvalue[1] & 0xfff7;
    runtimedata.data.torqueforce[2] = runtimedata.data.adcvalue[2] & 0xfff7;
    runtimedata.data.torqueforce[3] = runtimedata.data.adcvalue[3] & 0xfffc;
    runtimedata.data.torqueforce[4] = runtimedata.data.adcvalue[4] & 0xfffc;
    runtimedata.data.torqueforce[4] = runtimedata.data.adcvalue[5] & 0xfffc;
    
    runtimedata.data.torque.x = runtimedata.data.torqueforce[0];
    runtimedata.data.torque.y = runtimedata.data.torqueforce[1];
    runtimedata.data.torque.z = runtimedata.data.torqueforce[2];
    
    runtimedata.data.force.x = runtimedata.data.torqueforce[3];
    runtimedata.data.force.y = runtimedata.data.torqueforce[4];
    runtimedata.data.force.z = runtimedata.data.torqueforce[5];
    
    
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


    if(true == runtimedata.data.TXuncalibData)
    {
        embot::app::canprotocol::analog::periodic::Message_UNCALIBFORCE_VECTOR_DEBUGMODE::Info forceinfo;
        if(true == fill(forceinfo))
        {
            embot::app::canprotocol::analog::periodic::Message_UNCALIBFORCE_VECTOR_DEBUGMODE msg;
            msg.load(forceinfo);
            msg.get(frame);
            replies.push_back(frame);
        }            
    
        embot::app::canprotocol::analog::periodic::Message_UNCALIBTORQUE_VECTOR_DEBUGMODE::Info torqueinfo;
        if(true == fill(torqueinfo))
        {
            embot::app::canprotocol::analog::periodic::Message_UNCALIBTORQUE_VECTOR_DEBUGMODE msg;
            msg.load(torqueinfo);
            msg.get(frame);
            replies.push_back(frame);
        }  
    }    
                                            

    if(true == runtimedata.data.TXcalibData)
    {
        embot::app::canprotocol::analog::periodic::Message_FORCE_VECTOR::Info forceinfo;
        if(true == fill(forceinfo))
        {
            embot::app::canprotocol::analog::periodic::Message_FORCE_VECTOR msg;
            msg.load(forceinfo);
            msg.get(frame);
            replies.push_back(frame);
        }            
    
        embot::app::canprotocol::analog::periodic::Message_TORQUE_VECTOR::Info torqueinfo;
        if(true == fill(torqueinfo))
        {
            embot::app::canprotocol::analog::periodic::Message_TORQUE_VECTOR msg;
            msg.load(torqueinfo);
            msg.get(frame);
            replies.push_back(frame);
        }  
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
    
    // read from eeprom and make sure it is coherent data (first ever time we init eeprom)
    pImpl->configdata.eeprom_read();
    pImpl->runtimedata.clear();
     
    return true;
}


bool embot::app::application::theSTRAIN::configure(embot::common::Time txperiod)
{
    // if ticking: stop it
    if(true == pImpl->ticking)
    {
        stop();
    }
    
    pImpl->runtimedata.data.txperiod = txperiod;

    return true;    
}


bool embot::app::application::theSTRAIN::configure(embot::app::canprotocol::analog::polling::Message_SET_SERIAL_NO::Info &info)
{
    // original strain code saves the value in ram only. it is saved in eeprom only when the message save2eeprom arrives    
    pImpl->configdata.serial_set(info.serial);
    
    return true;    
}


bool embot::app::application::theSTRAIN::get_serial(embot::app::canprotocol::analog::polling::Message_GET_SERIAL_NO::ReplyInfo &replyinfo)
{  
    // original strain code gets the value from ram, even if the ram and the eeprom are not synched
    std::snprintf(replyinfo.serial, sizeof(replyinfo.serial), pImpl->configdata.serial_get());
   
    return true;    
}

bool embot::app::application::theSTRAIN::get_fullscale(std::uint8_t channel, std::uint16_t &value)
{    
    if(channel >= 6 )
    {
        value = 0;
        return false;
    }
       
    #warning: fix the problem uint16 vs int16
    value = pImpl->configdata.fullscale_get(channel);

    return true;    
}


bool embot::app::application::theSTRAIN::get_adc(embot::app::canprotocol::analog::polling::Message_GET_CH_ADC::ReplyInfo &replyinfo)
{    
    if(replyinfo.channel >= 6 )
    {
        replyinfo.adcvalue = 0;
        return false;
    }
    
    #warning -> VERY IMPORTANT: should we perform an acquisition or ... just get the buffered value? THINK of IT!
        // moreover: acquire but dont tx mode could be useful?
    
    // we perform an acquisition ...
    
    pImpl->acquisition();
        
    std::uint16_t v = 0;
    
    if(true == replyinfo.valueiscalibrated)
    {        
        switch(replyinfo.channel)
        {   
            // torque
            case 0:     v = pImpl->runtimedata.data.torque.x;   break;
            case 1:     v = pImpl->runtimedata.data.torque.y;   break;
            case 2:     v = pImpl->runtimedata.data.torque.z;   break;
            // force
            case 3:     v = pImpl->runtimedata.data.force.x;    break;
            case 4:     v = pImpl->runtimedata.data.force.y;    break;
            case 5:     v = pImpl->runtimedata.data.force.z;    break;
            // impossible ...
            default:    v = 0;                                  break;
        }

    }
    else
    {
       v = pImpl->runtimedata.data.adcvalue[replyinfo.channel]; 
    }
    
    
    replyinfo.adcvalue = v;
    
    return true;    
}


bool embot::app::application::theSTRAIN::get_offset(std::uint8_t channel, std::uint16_t &value)
{    
    if(channel >= 6 )
    {
        value = 0;
        return false;
    }
    
    value = pImpl->configdata.data.offset[channel];
    
    return true;    
}

bool embot::app::application::theSTRAIN::configure(embot::app::canprotocol::analog::polling::Message_SET_FULL_SCALES::Info &info)
{       
    // original strain code saves the value in ram only. it is saved in eeprom only when the message save2eeprom arrives     
    pImpl->configdata.fullscale_set(info.channel, info.fullscale);

    return true;    
}

bool embot::app::application::theSTRAIN::get_eepromstatus(bool &saved)
{    
    saved = pImpl->configdata.synched;
    return true;    
}


bool embot::app::application::theSTRAIN::save2eeprom()
{    
    pImpl->configdata.eeprom_write();

    return true;    
}

bool embot::app::application::theSTRAIN::configure(embot::app::canprotocol::analog::polling::Message_SET_CH_DAC::Info &info)
{  
    if(info.channel >= 6 )
    {
        return false;
    } 
   
    // original strain code saves the value in ram only. it is saved in eeprom only when the message save2eeprom arrives     
    pImpl->configdata.offset_set(info.channel, info.offset);

    return true;    
}


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_GET_MATRIX_RC::ReplyInfo &replyinfo)
{    
    if((replyinfo.row >= 6) || (replyinfo.col >= 6))
    {
        replyinfo.value = 0;
        return false;
    }
    
    replyinfo.value = pImpl->configdata.data.matrix[replyinfo.row][replyinfo.col];
    
    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::analog::polling::Message_SET_MATRIX_RC::Info &info)
{  
    if((info.row >= 6) || (info.col >= 6))
    {
        return false;
    }
    
    // original strain code saves the value in ram only. it is saved in eeprom only when the message save2eeprom arrives     
    pImpl->configdata.matrix_set(info.row, info.col, info.value);

    return true;    
}


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_GET_MATRIX_G::ReplyInfo &replyinfo)
{        
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();

    replyinfo.gain =  pImpl->configdata.data.matrixgain;
    
    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::analog::polling::Message_SET_MATRIX_G::Info &info)
{  
    // original strain code saves the value in ram only. it is saved in eeprom only when the message save2eeprom arrives     
    pImpl->configdata.matrixgain_set(info.gain);

    return true;    
}

bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_GET_CALIB_TARE::ReplyInfo &replyinfo)
{  
    if(replyinfo.channel >= 6)
    {
        replyinfo.value = 0;
        return false;
    } 

    replyinfo.value = pImpl->configdata.data.tare[replyinfo.channel];

    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE::Info &info)
{ 
    if(info.channel >= 6)
    {
        return false;
    }     

    // original strain code saves the value in ram only. it is saved in eeprom only when the message save2eeprom arrives
            
    // manage:
    bool ret = true;

    switch(info.mode)
    {
        case embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE::Mode::setchannelwithvalue:
        {
            pImpl->configdata.tare_set(info.channel, info.value);
        } break;            

        case embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE::Mode::everychannelreset:
        {
            pImpl->configdata.tare_clear();
        } break;

        case embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE::Mode::everychannelnegativeofadc:
        {
            pImpl->acquisition();
            
            for(int i=0; i<6; i++)
            {
                #warning -> TODO: adcvalue is u16 ... and tare is s16 ??
                pImpl->configdata.tare_set(i, -pImpl->runtimedata.data.adcvalue[i]);
            }
        } break;    

        default:
        case embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE::Mode::unknown:
        {
            ret = false;
        } break;        
    }
    

    return ret;    
}


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_GET_CURR_TARE::ReplyInfo &replyinfo)
{  
    if(replyinfo.channel >= 6)
    {
        replyinfo.value = 0;
        return false;
    }     
 
    replyinfo.value =  pImpl->runtimedata.data.tare[replyinfo.channel];
    
    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE::Info &info)
{ 
    if(info.channel >= 6)
    {
        return false;
    }     

 
    // manage:
    bool ret = true;

    switch(info.mode)
    {
        case embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE::Mode::setchannelwithvalue:
        {
            pImpl->runtimedata.data.tare[info.channel] = info.value;
        } break;            

        case embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE::Mode::everychannelreset:
        {
            std::memset(pImpl->runtimedata.data.tare, 0, sizeof(pImpl->runtimedata.data.tare));
        } break;

        case embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE::Mode::everychannelnegativeoftorqueforce:
        {
            pImpl->acquisition();
            
            #warning TODO: verify what we have to assign in SET_CURR_TARE
            
            for(int i=0; i<6; i++)
            {                
                pImpl->runtimedata.data.tare[i] = - (pImpl->runtimedata.data.torqueforce[i] - pImpl->runtimedata.data.tare[i]); 
            }
        } break;    

        default:
        case embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE::Mode::unknown:
        {
            ret = false;
        } break;        
    }
    
    return ret;    
}


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_GET_AMP_GAIN::ReplyInfo &replyinfo)
{  
    if(replyinfo.channel >= 6)
    {
        replyinfo.gain0 = replyinfo.gain1 = 0;
        return false;
    }     

    replyinfo.gain0 = pImpl->configdata.data.gainofamplifier[replyinfo.channel][0];
    replyinfo.gain1 = pImpl->configdata.data.gainofamplifier[replyinfo.channel][1];
    
    return true;    
}

bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_STRAIN2_AMPLIFIER_CFG1_GET::ReplyInfo &replyinfo)
{  
    if(replyinfo.channel >= 6)
    {
//        replyinfo.gain0 = replyinfo.gain1 = 0;
        return false;
    }     

    #warning TBD
    
    return true;    
}



bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::analog::polling::Message_SET_AMP_GAIN::Info &info)
{ 
    if(info.channel >= 6)
    {
        return false;
    }     

    pImpl->configdata.gainofamplifier_set(info.channel, info.gain0, info.gain1);
   
    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::analog::polling::Message_STRAIN2_AMPLIFIER_CFG1_SET::Info &info)
{ 
    if(info.channel >= 6)
    {
        return false;
    }     

    #warning TBD
    //pImpl->configdata.gainofamplifier_set(info.channel, info.gain0, info.gain1);
   
    return true;    
}


bool embot::app::application::theSTRAIN::start(embot::app::canprotocol::analog::polling::Message_SET_TXMODE::StrainMode &mode)
{    
    return pImpl->start(mode);
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



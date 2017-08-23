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
    // this struct is without methods so thta i am sure its sizeof() is the sum of the variables. 
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
        std::uint16_t       gainofamplifier[2][6];  // the gains used by .... 6sg maybe?                    
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
            
        void setmatrixidentity()
        {   // value 0x7fff is close to 1 = 1-2^(-15)            
            std::memset(data.matrix, 0, sizeof(data.matrix));
            data.matrix[0][0] = data.matrix[1][1] = data.matrix[2][2] = data.matrix[3][3] = data.matrix[4][4] = data.matrix[5][5] = 0x7fff;
        }
        
        void setmatrixgain(std::uint8_t g)
        {            
            data.matrixgain = g;
        }
        
        void setmatrix(std::uint8_t r, std::uint8_t c, std::uint16_t value)
        {   
            if((r<6) && (c<6))
            {
                data.matrix[r][c] = value;
            }
        }
        
        bool setdefault()
        {
            clear();
            data.key = validityKey;
            std::snprintf(data.serial, sizeof(data.serial), "SN0000");
            data.fullscale[0] = data.fullscale[1] = data.fullscale[2] = data.fullscale[3] = data.fullscale[4] = data.fullscale[5] = 0x7fff;
            setmatrixidentity();
            setmatrixgain(1);
            std::memset(data.gainofamplifier, 1, sizeof(data.gainofamplifier)); 
            synched = false;
            return synched;
        }
        
        bool eepromread()
        {
            embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
            canbrdinfo.userdataread(0, sizeof(data), &data);
            synched = true;
            
            if(validityKey != data.key)
            {   // if it is not valid, then ... set default and impose validity key      
                setdefault();
                data.key = validityKey;
                eepromwrite();
            }
                        
            return synched;
        }
        
        bool eepromwrite()
        {
            data.key = validityKey;
            embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance(); 
            canbrdinfo.userdatawrite(0, sizeof(data), &data);
            synched = true;
            return synched;
        }
        
        void setserial(const char *str) 
        {
            std::snprintf(data.serial, sizeof(data.serial), "%s", str);
            synched = false;
        }
        
        const char * getserial()
        {
            return data.serial;
        }
        
        std::int16_t getfullscale(std::uint8_t channel)
        {
            if(channel < 6)
                return data.fullscale[channel];
            else
                return 0;
        }
        
        void setfullscale(std::uint8_t channel, std::int16_t value)
        {
            if(channel < 6)
            {
                data.fullscale[channel] = value;
                synched = false;
            }
        }
        
        void setoffset(std::uint8_t channel, std::uint16_t value)
        {
            if(channel < 6)
            {
                data.offset[channel] = value;
                synched = false;
            }
        }
        
    };
    
    
    // it is a holder of volatile data used in runtime. we use tha same approach as for config data
    struct StrainRuntimeData_t
    {
        std::int16_t        tare[6];    // the tare (offset) as measured in runtime
        // add all useful data in here.
        
        // methods
        void clear() { std::memset(tare, 0, sizeof(tare)); }       
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
    
    
    StrainRuntimeData runtimedata;
    StrainConfigData configdata;
    

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
        
        configdata.clear();
        runtimedata.clear();
        
    }
    
   
    bool start();
    bool stop();
    
    bool tick(std::vector<embot::hw::can::Frame> &replies);
    
    bool configure();
    
    bool acquisition();
    
    
    bool fill(embot::app::canprotocol::analog::periodic::Message_FORCE_VECTOR::Info &info);
    bool fill(embot::app::canprotocol::analog::periodic::Message_TORQUE_VECTOR::Info &info);
                      
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


bool embot::app::application::theSTRAIN::Impl::fill(embot::app::canprotocol::analog::periodic::Message_FORCE_VECTOR::Info &info)
{
    bool ret = true;
    
    info.canaddress = canaddress;
    info.x = force.x;
    info.y = force.y;
    info.z = force.z;
         
    return ret;    
}


bool embot::app::application::theSTRAIN::Impl::fill(embot::app::canprotocol::analog::periodic::Message_TORQUE_VECTOR::Info &info)
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
    #warning TODO: perform hw acquisition of all adc values and computation of ft values. 
    
    // formula is:
    // torqueforce = M * (adcvalue + calibtare ) + currtare
    // each of teh 6 adc values is acquired one at a time after we have applied a proper dac-offset[] value

    
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
    pImpl->configdata.eepromread();
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
    #warning: think to move txperiod in runtimedata.
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


bool embot::app::application::theSTRAIN::configure(embot::app::canprotocol::analog::polling::Message_SET_SERIAL_NO::Info &info)
{
    // original strain code saves the value in ram only. it is saved in eeprom only when the message save2eeprom arrives    
    pImpl->configdata.setserial(info.serial);
    
    return true;    
}


bool embot::app::application::theSTRAIN::get_serial(embot::app::canprotocol::analog::polling::Message_GET_SERIAL_NO::ReplyInfo &replyinfo)
{      
    std::snprintf(replyinfo.serial, sizeof(replyinfo.serial), pImpl->configdata.getserial());
   
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
    value = pImpl->configdata.getfullscale(channel);
    
    return true;    
}


bool embot::app::application::theSTRAIN::get_adc(embot::app::canprotocol::analog::polling::Message_GET_CH_ADC::ReplyInfo &replyinfo)
{    
    if(replyinfo.channel >= 6 )
    {
        return false;
    }
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();

    replyinfo.adcvalue = 0x1234;
    
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

bool embot::app::application::theSTRAIN::configure(embot::app::canprotocol::analog::polling::Message_SET_FULL_SCALES::Info &info)
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

bool embot::app::application::theSTRAIN::configure(embot::app::canprotocol::analog::polling::Message_SET_CH_DAC::Info &info)
{  
    if(info.channel >= 6 )
    {
        return false;
    } 
   
  
    pImpl->configdata.setoffset(info.channel, info.offset);

    return true;    
}


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_GET_MATRIX_RC::ReplyInfo &replyinfo)
{    
    if((replyinfo.row >= 6) || (replyinfo.col >= 6))
    {
        return false;
    }
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();

    replyinfo.value =  pImpl->strainstoredinfo.matrix[replyinfo.row][replyinfo.col];
    
    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::analog::polling::Message_SET_MATRIX_RC::Info &info)
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


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_GET_MATRIX_G::ReplyInfo &replyinfo)
{        
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();

    replyinfo.gain =  pImpl->strainstoredinfo.gain;
    
    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::analog::polling::Message_SET_MATRIX_G::Info &info)
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

bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_GET_CALIB_TARE::ReplyInfo &replyinfo)
{  
    if(replyinfo.channel >= 6)
    {
        return false;
    }     
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();

    replyinfo.value =  pImpl->strainstoredinfo.calib_bias[replyinfo.channel];
    
    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE::Info &info)
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
        case embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE::Mode::setchannelwithvalue:
        {
            pImpl->strainstoredinfo.calib_bias[info.channel] = info.value;
        } break;            

        case embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE::Mode::everychannelreset:
        {
            std::memset(pImpl->strainstoredinfo.calib_bias, 0, sizeof(pImpl->strainstoredinfo.calib_bias));
        } break;

        case embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE::Mode::everychannelnegativeofadc:
        {
            #warning tbd
            for(int i=0; i<6; i++)
            {
                pImpl->strainstoredinfo.calib_bias[i] = -1;
            }
        } break;    

        default:
        case embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE::Mode::unknown:
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


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_GET_CURR_TARE::ReplyInfo &replyinfo)
{  
    if(replyinfo.channel >= 6)
    {
        return false;
    }     
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();

    replyinfo.value =  pImpl->strainstoredinfo.curr_bias[replyinfo.channel];
    
    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE::Info &info)
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
        case embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE::Mode::setchannelwithvalue:
        {
            pImpl->strainstoredinfo.curr_bias[info.channel] = info.value;
        } break;            

        case embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE::Mode::everychannelreset:
        {
            std::memset(pImpl->strainstoredinfo.curr_bias, 0, sizeof(pImpl->strainstoredinfo.curr_bias));
        } break;

        case embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE::Mode::everychannelnegativeoftorque:
        {
            #warning TBD
            for(int i=0; i<6; i++)
            {                
                pImpl->strainstoredinfo.curr_bias[info.channel] = -1; 
            }
        } break;    

        default:
        case embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE::Mode::unknown:
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


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_GET_AMP_GAIN::ReplyInfo &replyinfo)
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


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::analog::polling::Message_SET_AMP_GAIN::Info &info)
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



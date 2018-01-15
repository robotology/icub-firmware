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

#include <new>
#include <cstdio>

#include "stm32hal.h"

#include "embot.h"
#include "embot_common.h"
#include "embot_binary.h"
#include "embot_dsp.h"

#include "embot_hw.h"
#include "embot_hw_can.h"
#include "embot_hw_pga308.h"
#include "embot_hw_adc.h"


#include "embot_hw_sys.h"


#include "embot_sys_Timer.h"
#include "embot_sys_Action.h"

#include "embot_app_canprotocol.h"

#include "embot_app_theCANboardInfo.h"
#include "embot_app_application_theCANtracer.h"






// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


    
struct embot::app::application::theSTRAIN::Impl
{ 
    static const std::uint8_t numOfChannels = 6;
    static const std::uint8_t numOfSets = 3;
    
    // the new amplifier is the PGA308 by Texas Instruments. It has a completly different configuration
    // we use for it the compact form of the can message
    struct amplifierConfig_t
    { 
        embot::app::canprotocol::analog::polling::PGA308cfg1    pga308cfg1;   
        
        void factoryreset() {
            embot::hw::PGA308::TransferFunctionConfig tfc;
            tfc.setDefault();
            tfc.get(pga308cfg1);
        }
        
        amplifierConfig_t() { factoryreset(); }
    };
    
    
//    // the old amplifier just had an offset. the 6sg also had two gains.
//    // MAYBE: we should think to reuse these three values to set the whole pga308cfg1 a simpler way for the sake of canloader 
//    struct amplifier0Config_t
//    {   
//        std::uint16_t       offset;             // the offset generated with the dac  
//        std::uint16_t       gain[2];            // the gains used by .... 6sg maybe?                
//    }; 

    
    // the matrix transformation which is done after adc acquisition    
    // tf = M * (adc + tare)
    // then tf can also be corrected with an extra tare which is set in runtime: TF = tf + tare_set_in_runtime
    // the 6 values of TF are those transmitted to the host.
    // in here are also stored fullscale (and gain?) which are queried and used by the remote host to compensate the received TF values
    struct transformerConfig_t
    {
        std::uint8_t                dummy;          // just to remove a hole
        std::uint8_t                gain;           // the gain managed with GET_MATRIX_G / SET_MATRIX_G which is NOT USED INSIDE THE MPU        
        std::uint16_t               fullscale[6];   // the fullscale of the measurements: NOT USED INSIDE THE MPU
        embot::dsp::Q15             tare[6];        // in format 1Q15: the stored tare
        embot::dsp::Q15             matrix[6*6];    // in format 1Q15: transformation matrix from the 6 strain-gauge values to the 6 force-torque values        
    };
    
    
    // the complete configuration for the transformer and the six amplifiers
    struct setOfConfig_t
    {
        transformerConfig_t     transformer;            // the config of the transformer from dacs to torqueforce array
//        amplifier0Config_t      amplifiers0[6];         // the OLD config of the amplifiers from strain gauge to values given to dacs 
        amplifierConfig_t       amplifiers[6];          // the NEW config of the amplifiers from strain gauge to values given to dacs          
    };
    
    static const std::uint32_t validityKey = 0x7777dead;
    
    
    
    // it is a holder of config data which is eventally saved to eeprom. 
    // this struct is without methods so that i am sure its sizeof() is the sum of the variables. 
    // this struct it is managed by the StrainConfig class
    struct strainConfigData_t
    {   
        std::uint32_t           key;
        char                    serial[8];              // the serial number of the strain      
        setOfConfig_t           set[numOfSets];         // the three configurations sets. so far only one is supported
    };  
        
    
    // the class which manages the configuration data which is also stored in eeprom
    class StrainConfigData
    {   
        private:
        
        
        // data contains what is also stored in eeprom. only eepromread() and eepromwrite() operate on eeprom
        // and take care to set synched to true. there are methods used to modify data which also set synched 
        // to false. HENCE: DO NOT CHANGE data by hand. 
        strainConfigData_t                      data;           // we save it in eeprom when requested
        bool                                    synched;        // true if data is equal to what is in eeprom
        
        private:
        
        embot::dsp::q15::matrix transfQ15matrix[numOfSets];
        embot::dsp::q15::matrix transfQ15tare[numOfSets];

        public:
            
        
        // methods
        
        bool isEEPROMsynched()
        {
            return synched;
        }
        
        bool EEPROM_read()
        {
            embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
            canbrdinfo.userdataread(0, sizeof(data), &data);
            synched = true;
            
            if(validityKey != data.key)
            {   // if it is not valid, then ... set default and impose validity key      
                def_values();
                data.key = validityKey;
                EEPROM_write();
            }
                        
            return synched;
        }
        
        bool EEPROM_write()
        {
            data.key = validityKey;
            embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance(); 
            canbrdinfo.userdatawrite(0, sizeof(data), &data);
            synched = true;
            return synched;
        }
        
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
            
            for(int s=0; s<numOfSets; s++)
            {
                transfQ15matrix[s].load(6, 6, data.set[s].transformer.matrix);
                transfQ15tare[s].load(6, 1, data.set[s].transformer.tare);
            }
        }
        
        ~StrainConfigData() {}
            
            
                    
        void serial_set(const char *str) 
        {
            std::snprintf(data.serial, sizeof(data.serial), "%s", str);
            synched = false;
        }
        
        const char * serial_get()
        {
            return data.serial;
        }
                        
        void amplifiers_set(std::uint8_t set, std::uint8_t channel, const embot::app::canprotocol::analog::polling::PGA308cfg1 &cfg1)
        {
            if((channel < 6) && (set < numOfSets))
            {
                data.set[set].amplifiers[channel].pga308cfg1 = cfg1;
                synched = false;
            }
        } 
        
        bool amplifiers_get(std::uint8_t set, std::uint8_t channel, embot::app::canprotocol::analog::polling::PGA308cfg1 &cfg1)
        {
            bool ret = false;
            if((channel < 6) && (set < numOfSets))
            {
                cfg1 = data.set[set].amplifiers[channel].pga308cfg1;
                ret = true;
            }
            
            return ret;
        } 

        void amplifiers_reset(std::uint8_t set, std::uint8_t channel)
        {
            if((channel < 6) && (set < numOfSets))
            {
                data.set[set].amplifiers[channel].factoryreset();
                synched = false;
            }
        }         
          
        const embot::dsp::q15::matrix& transformer_matrix_handle(std::uint8_t set)
        {
            if(set >= numOfSets)
            {
                set = 0;
            }
            return transfQ15matrix[set];
        }
        
        const embot::dsp::q15::matrix& transformer_tare_handle(std::uint8_t set)
        {
            if(set >= numOfSets)
            {
                set = 0;
            }
            return transfQ15tare[set];
        }
        
        void transformer_matrix_fill_identity(std::uint8_t set)
        {
            if(set < numOfSets)
            {
                transfQ15matrix[set].diagonal(embot::dsp::q15::posOneNearly);
                synched = false;
            }
        }
        
        void transformer_matrix_fill_diagonal(std::uint8_t set, embot::dsp::Q15 value)
        {
            if(set < numOfSets)
            {
                transfQ15matrix[set].diagonal(value);
                synched = false;
            }
        }
        
        void transformer_gain_set(std::uint8_t set, std::uint8_t g)
        { 
            if(set < numOfSets)
            {            
                data.set[set].transformer.gain = g;
                synched = false;
            }
        }
        
        std::uint8_t transformer_gain_get(std::uint8_t set)
        {
            std::uint8_t g = 0;           
            if(set < numOfSets)
            {            
                g = data.set[set].transformer.gain;
            }
            return g;
        }
        
        void transformer_matrix_set(std::uint8_t set, std::uint8_t r, std::uint8_t c, embot::dsp::Q15 value)
        {   
            if((set < numOfSets) && (r < 6) && (c < 6))
            {
                transfQ15matrix[set].set(r, c, value);
                synched = false;
            }
        }
        
        embot::dsp::Q15 transformer_matrix_get(std::uint8_t set, std::uint8_t r, std::uint8_t c)
        { 
            embot::dsp::Q15 v = embot::dsp::q15::zero;
            
            if((set < numOfSets) && (r < 6) && (c < 6))
            {
                v = transfQ15matrix[set].get(r, c);
            }
            
            return v;
        }
        
        void transformer_tare_fill(std::uint8_t set, embot::dsp::Q15 value)
        {
            if(set < numOfSets)
            {                
                transfQ15tare[set].fill(value);
                synched = false;
            }
        }
        
        void transformer_tare_set(std::uint8_t set, std::uint8_t channel, embot::dsp::Q15 value)
        { 
            if((set < numOfSets) && (channel < 6))
            {
                transfQ15tare[set].set(channel, 1, value);
                synched = false;
            }
        }
        
        embot::dsp::Q15 transformer_tare_get(std::uint8_t set, std::uint8_t channel)
        {
            embot::dsp::Q15 v = embot::dsp::q15::zero;
            if((set < numOfSets) && (channel < 6))
            {
                v = transfQ15tare[set].get(channel, 1);
            }            
            return v;
        }        
        
        bool def_values()
        {
            clear();
            
            data.key = validityKey;
            std::snprintf(data.serial, sizeof(data.serial), "SN0000");            
            // and now the set   
            for(int s=0; s<numOfSets; s++)
            {
                // transformer
                transformer_gain_set(s, 1);
                transformer_fullscale_fill(s, 0x7fff);
                transformer_tare_fill(s, embot::dsp::q15::zero);
                transformer_matrix_fill_diagonal(s, embot::dsp::q15::posOneNearly);
                transformer_gain_set(s, 1);
                
                for(int chn=0; chn<6; chn++)
                {
//                    // amplifier0
//                    amplifier0_offset_set(s, chn, 0);
//                    amplifier0_gains_set(s, chn, 1, 1);
                    // amplifier
                    amplifiers_reset(s, chn);
                }
            }
            
                        
            synched = false;
            return synched;
        }
        
        
        std::uint16_t transformer_fullscale_get(std::uint8_t set, std::uint8_t channel)
        {
            if((set < numOfSets) && (channel < 6))
            {
                return data.set[set].transformer.fullscale[channel];
            }
            else
            {
                return 0;
            }
        }
        
        void transformer_fullscale_set(std::uint8_t set, std::uint8_t channel, std::uint16_t value)
        {
            if((set < numOfSets) && (channel < 6))
            {
                data.set[set].transformer.fullscale[channel] = value;
                synched = false;
            }
        } 

        void transformer_fullscale_fill(std::uint8_t set, std::uint16_t value)
        {
            for(int channel=0; channel<6; channel++)
            {
                transformer_fullscale_set(set, channel, value); 
            }
        }        
        
//        void amplifier0_offset_set(std::uint8_t set, std::uint8_t channel, std::uint16_t value)
//        {
//            if((set < numOfSets) && (channel < 6))
//            {
//                data.set[set].amplifiers0[channel].offset = value;
//                synched = false;
//            }
//        }
//        
//        std::uint16_t amplifier0_offset_get(std::uint8_t set, std::uint8_t channel)
//        {
//            std::uint16_t value = 0;
//            if((set < numOfSets) && (channel < 6))
//            {
//                value = data.set[set].amplifiers0[channel].offset;                
//            }
//            
//            return value;
//        }
//        
//        void amplifier0_gains_set(std::uint8_t set, std::uint8_t channel, std::uint16_t g0, std::uint16_t g1)
//        {
//            if((set < numOfSets) && (channel < 6))
//            {
//                data.set[set].amplifiers0[channel].gain[0] = g0;
//                data.set[set].amplifiers0[channel].gain[1] = g1;
//                synched = false;
//            }            
//        }
//        
//        bool amplifier0_gains_get(std::uint8_t set, std::uint8_t channel, std::uint16_t &g0, std::uint16_t &g1)
//        {
//            bool ret = false;
//            if((set < numOfSets) && (channel < 6))
//            {
//                g0 = data.set[set].amplifiers0[channel].gain[0];
//                g0 = data.set[set].amplifiers0[channel].gain[1];
//                ret =  true;
//            }  
//            return ret;            
//        }
        
    };
    
        
    // it is a holder of volatile data used in runtime. we use the same approach as for config data
    struct StrainRuntimeData_t
    {
        embot::common::Time         txperiod;
        
        std::uint16_t               dmabuffer[6]; 
        std::uint16_t               adcvalue[6];        // the values as acquired by the adc... but scaled to full 64k range. original strain had 16-bit adc   
        
        embot::dsp::Q15     q15value[6];        // same as adcvalue but in q15 format       
        embot::dsp::Q15     tare[6];            // the tare applied as a last correction. it is always ::zero unless changed with SET_CURR_TARE
        embot::dsp::Q15     torqueforce[6];
        
        // the calibrated values for torque and force. they are raw values, not in dsp::Q15 format
        embot::common::Triple<std::uint16_t>    torque;
        embot::common::Triple<std::uint16_t>    force;
        
        bool TXcalibData;
        bool TXuncalibData;
        
        bool adcsaturation;        
        embot::app::canprotocol::analog::periodic::ADCsaturation saturationonchannel[6];

        std::uint8_t adcfailures;
        
        
        // methods
        void clear() { 
            std::memset(q15value, 0, sizeof(q15value)); 
            std::memset(tare, 0, sizeof(tare)); 
            std::memset(dmabuffer, 0, sizeof(dmabuffer));
            std::memset(adcvalue, 0, sizeof(adcvalue));
            std::memset(torqueforce, 0, sizeof(torqueforce));
            force.reset();
            torque.reset();
            TXcalibData = false; 
            TXuncalibData = false; 
            adcsaturation =  false;
            adcfailures = 0;
            std::memset(saturationonchannel, static_cast<std::uint8_t>(embot::app::canprotocol::analog::periodic::ADCsaturation::NONE), sizeof(saturationonchannel));
        }       
        StrainRuntimeData_t() { clear(); }
    };

    
    class StrainRuntimeData
    {
        public:

        StrainRuntimeData_t data;   
        
        // used for the sake of matrix-based calculation
        embot::dsp::q15::matrix currtareQ15vector;
        embot::dsp::q15::matrix torqueforceQ15vector;
        embot::dsp::q15::matrix adcvalueQ15vector;
        
                        
        static const std::uint16_t ADCsaturationLow = 1000;
        static const std::uint16_t ADCsaturationHigh = 64000;
        
        
        // methods
        
        bool check_adcsaturation()
        {
            data.adcsaturation =  false;
            std::memset(data.saturationonchannel, static_cast<std::uint8_t>(embot::app::canprotocol::analog::periodic::ADCsaturation::NONE), sizeof(data.saturationonchannel));


            
            for(int channel=0; channel<6; channel++)
            {
                data.saturationonchannel[channel] = embot::app::canprotocol::analog::periodic::ADCsaturation::NONE;
                
                if(data.adcvalue[channel] < ADCsaturationLow)
                {
                    data.saturationonchannel[channel] = embot::app::canprotocol::analog::periodic::ADCsaturation::LOW;                    
                    data.adcsaturation = true;                
                }
                else if(data.adcvalue[channel] > ADCsaturationHigh)
                {
                    data.saturationonchannel[channel] = embot::app::canprotocol::analog::periodic::ADCsaturation::HIGH;
                    data.adcsaturation = true;                 
                }               
            }
            
            return data.adcsaturation;           
        }
        
        void tare_fill(embot::dsp::Q15 value)
        {    
            currtareQ15vector.fill(value);
        }
        
        void tare_set(std::uint8_t channel, embot::dsp::Q15 value)
        { 
            if(channel<6)
            {
                data.tare[channel] = value;
            }
        }
        
        embot::dsp::Q15 tare_get(std::uint8_t channel)
        {
            if(channel<6)
            {
                return data.tare[channel];
            }
            
            return embot::dsp::q15::zero;            
        }
        
        bool clear()
        {
            data.clear();
            return true;
        }
        
        StrainRuntimeData()
        {
            clear();
            
            torqueforceQ15vector.load(6, 1, data.torqueforce);
            currtareQ15vector.load(6, 1, data.tare);
            adcvalueQ15vector.load(6, 1, data.q15value);
        }
        
        ~StrainRuntimeData() {}
            
        bool setdefault()
        {
            clear();
            return true;
        }
                        
    };  

   
      
    embot::common::Time debugtime;
    Config config;
           
    bool ticking;
    embot::app::canprotocol::analog::polling::Message_SET_TXMODE::StrainMode txmode;
    volatile bool adcdataisready;
    
    
    embot::sys::Timer *ticktimer;
    embot::sys::Action action;
        
                     
    // i put in here some values proper of the STRAIN. they may be many.... hence better to organise them in a struct later on.
    
    
    StrainRuntimeData runtimedata;
    StrainConfigData configdata;
    

    Impl() 
    {

//        defaultAmplifConfig.factoryreset();     

        debugtime = 0;
        
        ticking = false;  
        txmode = embot::app::canprotocol::analog::polling::Message_SET_TXMODE::StrainMode::acquireOnly; // even better is to use none

        ticktimer = new embot::sys::Timer;   

                
        configdata.clear();
        runtimedata.clear();
        
        runtimedata.data.txperiod = 50*embot::common::time1millisec;       

        adcdataisready = false;        
    }
    
   
    bool start(embot::app::canprotocol::analog::polling::Message_SET_TXMODE::StrainMode &mode);
    bool stop();
    
    bool tick(std::vector<embot::hw::can::Frame> &replies);
    bool processdata(std::vector<embot::hw::can::Frame> &replies);
    
    bool configure();
    
    bool acquisition_start();
    bool acquisition_waituntilcompletion(embot::common::relTime timeout);
    bool acquisition_retrieve();
    
    bool acquisition_oneshot(embot::common::relTime timeout = 3*embot::common::time1millisec, bool restartifitwasticking = true);
    
    bool processing();
    
    bool fill(embot::app::canprotocol::analog::periodic::Message_UNCALIBFORCE_VECTOR_DEBUGMODE::Info &info);
    bool fill(embot::app::canprotocol::analog::periodic::Message_UNCALIBTORQUE_VECTOR_DEBUGMODE::Info &info);
    bool fill(embot::app::canprotocol::analog::periodic::Message_FORCE_VECTOR::Info &info);
    bool fill(embot::app::canprotocol::analog::periodic::Message_TORQUE_VECTOR::Info &info);
    
    
    static void alertdataisready(void *p)
    {
        embot::app::application::theSTRAIN::Impl *mypImpl = reinterpret_cast<embot::app::application::theSTRAIN::Impl*>(p);
        
        if(true == mypImpl->ticking)
        {
            mypImpl->config.totask->setEvent(mypImpl->config.datareadyevent);
        }
        
        mypImpl->adcdataisready = true;                            
    }
                      
};



bool embot::app::application::theSTRAIN::Impl::start(embot::app::canprotocol::analog::polling::Message_SET_TXMODE::StrainMode &mode)
{    
    if(true == ticking)
    {
        ticktimer->stop();
        ticking = false;
    }
    
    runtimedata.data.TXcalibData = false;
    runtimedata.data.TXuncalibData = false;
        
    txmode = mode;
    
    switch(txmode)
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
    
    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
    info.x = runtimedata.data.adcvalue[3];
    info.y = runtimedata.data.adcvalue[4];
    info.z = runtimedata.data.adcvalue[5];
    
    info.adcsaturationispresent = runtimedata.data.adcsaturation;
    if(true == runtimedata.data.adcsaturation)
    {
        info.adcsaturationinfo.thereissaturation =  true;
        info.adcsaturationinfo.channel[0] = runtimedata.data.saturationonchannel[3];
        info.adcsaturationinfo.channel[1] = runtimedata.data.saturationonchannel[4];
        info.adcsaturationinfo.channel[2] = runtimedata.data.saturationonchannel[5];
    }
    else
    {
        info.adcsaturationinfo.reset();
    }
         
    return ret;    
}


bool embot::app::application::theSTRAIN::Impl::fill(embot::app::canprotocol::analog::periodic::Message_UNCALIBTORQUE_VECTOR_DEBUGMODE::Info &info)
{
    bool ret = true;

    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
    info.x = runtimedata.data.adcvalue[0];
    info.y = runtimedata.data.adcvalue[1];
    info.z = runtimedata.data.adcvalue[2];

    info.adcsaturationispresent = runtimedata.data.adcsaturation;
    if(true == runtimedata.data.adcsaturation)
    {
        info.adcsaturationinfo.thereissaturation =  true;
        info.adcsaturationinfo.channel[0] = runtimedata.data.saturationonchannel[0];
        info.adcsaturationinfo.channel[1] = runtimedata.data.saturationonchannel[1];
        info.adcsaturationinfo.channel[2] = runtimedata.data.saturationonchannel[2];
    }
    else
    {
        info.adcsaturationinfo.reset();
    }
    
    return ret;    
}

bool embot::app::application::theSTRAIN::Impl::fill(embot::app::canprotocol::analog::periodic::Message_FORCE_VECTOR::Info &info)
{
    bool ret = true;
    
    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
    info.x = runtimedata.data.force.x;
    info.y = runtimedata.data.force.y;
    info.z = runtimedata.data.force.z;
    
    info.adcsaturationispresent = runtimedata.data.adcsaturation;
    if(true == runtimedata.data.adcsaturation)
    {
        info.adcsaturationinfo.thereissaturation =  true;
        info.adcsaturationinfo.channel[0] = runtimedata.data.saturationonchannel[3];
        info.adcsaturationinfo.channel[1] = runtimedata.data.saturationonchannel[4];
        info.adcsaturationinfo.channel[2] = runtimedata.data.saturationonchannel[5];
    }
    else
    {
        info.adcsaturationinfo.reset();
    }
    
    return ret;    
}


bool embot::app::application::theSTRAIN::Impl::fill(embot::app::canprotocol::analog::periodic::Message_TORQUE_VECTOR::Info &info)
{
    bool ret = true;

    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
    info.x = runtimedata.data.torque.x;
    info.y = runtimedata.data.torque.y;
    info.z = runtimedata.data.torque.z;
    
    info.adcsaturationispresent = runtimedata.data.adcsaturation;
    if(true == runtimedata.data.adcsaturation)
    {
        info.adcsaturationinfo.thereissaturation =  true;
        info.adcsaturationinfo.channel[0] = runtimedata.data.saturationonchannel[0];
        info.adcsaturationinfo.channel[1] = runtimedata.data.saturationonchannel[1];
        info.adcsaturationinfo.channel[2] = runtimedata.data.saturationonchannel[2];
    }
    else
    {
        info.adcsaturationinfo.reset();
    }

    return ret;    
}

bool embot::app::application::theSTRAIN::Impl::acquisition_start()
{
    runtimedata.data.adcfailures = 0;
    
    std::memset(runtimedata.data.dmabuffer, 0xff, sizeof(runtimedata.data.dmabuffer));    
    adcdataisready = false;
    embot::hw::adc::start(embot::hw::adc::Port::one);    
    return true;
}

bool embot::app::application::theSTRAIN::Impl::acquisition_waituntilcompletion(embot::common::relTime timeout)
{
    embot::common::Time now = embot::sys::timeNow();
    embot::common::Time endtime = now + timeout;
    
//    embot::common::Time start = now;
//    static embot::common::Time delta = 0;
     // delta is 429 usec

    
    bool ret = true;
    
    for(;;)
    {
        if(true == adcdataisready)
        {
            break;
        }
        now = embot::sys::timeNow();
        if(now > endtime)
        {
           ret = false;
           break;
        }                   
    }
    
//    delta = now - start;
    
    return ret;
}


bool embot::app::application::theSTRAIN::Impl::acquisition_oneshot(embot::common::relTime timeout, bool restartifitwasticking)
{    
    bool itwasticking = ticking;    
    
    if(true == itwasticking)
    {
        stop();
    }           
    // we perform a new acquisition in blocking mode
    acquisition_start();    
    bool ret = acquisition_waituntilcompletion(timeout);     
        acquisition_retrieve();    
    
    if((true == itwasticking) && (true == restartifitwasticking))
    {
        start(txmode);     
    }
    
    return ret;    
}




bool embot::app::application::theSTRAIN::Impl::acquisition_retrieve()
{

    // 1. acquire from adc and put inside adcvalue    
    std::memmove(runtimedata.data.adcvalue, runtimedata.data.dmabuffer, sizeof(runtimedata.data.adcvalue));
     

    // 2. convert adcvalue[] into q15value[]

    runtimedata.data.adcfailures = 0;
    for(int i=0; i<6; i++)
    {
        if(0xffff == runtimedata.data.adcvalue[i])
        {
            runtimedata.data.adcfailures ++;
        }
        runtimedata.data.adcvalue[i] <<= 3; // adc value is 13 bits. we need to scale it to 64k
        runtimedata.data.q15value[i] = embot::dsp::q15::U16toQ15(runtimedata.data.adcvalue[i]);
    }
    
    // 3. also check vs saturation
    runtimedata.check_adcsaturation();
    
    return true;
}


bool embot::app::application::theSTRAIN::Impl::processing()
{
    // we work on set 0
    const std::uint8_t set = 0;
    
    // in adcvalue[] we performed adc acquisition, in value[] we put the q15 values and we also have checkd vs adc saturation
    
    
    // now prepare calib data: it will be inside the triples: runtimedata.data.torque and runtimedata.data.force 
    
        
    // apply formula: torqueforce = M * (adcvalue + calibtare) + currtare
    // default values are: M = IdentityMatrix(0x7fff); calibtare = Vector(0); currtare = Vector(0)
    
    bool q15saturated =  false;
    
    const embot::dsp::q15::matrix& handleCalibMatrixQ15 = configdata.transformer_matrix_handle(set);     
    const embot::dsp::q15::matrix& handleCalibTareQ15 = configdata.transformer_tare_handle(set);   
        
    embot::dsp::Q15 q15tmpvector[6];
    embot::dsp::q15::matrix tmpQ15vector(6, 1, q15tmpvector);
    
    
    embot::dsp::q15::add(runtimedata.adcvalueQ15vector, handleCalibTareQ15, tmpQ15vector, q15saturated);
    embot::dsp::q15::multiply(handleCalibMatrixQ15, tmpQ15vector, runtimedata.torqueforceQ15vector, q15saturated);
    embot::dsp::q15::add(runtimedata.torqueforceQ15vector, runtimedata.currtareQ15vector, runtimedata.torqueforceQ15vector, q15saturated);
    
    // copy 
    
    // now in torqueforceQ15vector (runtimedata.data.torqueforce[]) we have the result ... 
    
    // if we did have saturation ... we send old safe data.
   
    // if we are ok, we use what in runtimedata.data.torqueforce[] to compute safe data and data to send
    // data2send = safedata = 0x8000 + torqueforce
   
    if(false == runtimedata.data.adcsaturation)
    {
        runtimedata.data.torque.set(embot::dsp::q15::Q15toU16(runtimedata.data.torqueforce[0]), embot::dsp::q15::Q15toU16(runtimedata.data.torqueforce[1]), embot::dsp::q15::Q15toU16(runtimedata.data.torqueforce[2]));
        runtimedata.data.force.set(embot::dsp::q15::Q15toU16(runtimedata.data.torqueforce[3]), embot::dsp::q15::Q15toU16(runtimedata.data.torqueforce[4]), embot::dsp::q15::Q15toU16(runtimedata.data.torqueforce[5]));
    }
    else
    {
        // we dont update so that in runtimedata.data.torque and runtimedata.data.force there are always valid value
    }
        
    
    return true;
}

// some timings ... 
// - from start of adc acquisition until calling of this function: ~638usec
// - from start of adc acquisition until after acquisition_retrieve() inside this function: ~664usec 
// - from start of adc acquisition until after processing() inside this function, case of calib mode: ~1041usec 
// - from start of adc acquisition until end of this function, case of calib mode: ~1164usec 
bool embot::app::application::theSTRAIN::Impl::processdata(std::vector<embot::hw::can::Frame> &replies)
{   
    if(false == ticking)
    {
        return false;
    }
    
//    embot::common::Time start = debugtime;
//    debugtime = embot::sys::timeNow() - start;
        
    // retreve acquired adc values
    acquisition_retrieve();
    
//    debugtime = embot::sys::timeNow() - start;
    
    // processing of acquired data
    processing();
    
//    debugtime = embot::sys::timeNow() - start;    

#if 0    
    if(0 != runtimedata.data.adcfailures)
    {
        char ss[8] = {0};
        snprintf(ss, sizeof(ss), "e%d", runtimedata.data.adcfailures);
        embot::app::application::theCANtracer &tracer = embot::app::application::theCANtracer::getInstance();
        tracer.print(ss, replies);
        runtimedata.data.adcfailures = 0;
    }
    else
    {
        static uint32_t count = 0;
        
        if(0 == (count % 1000))
        {
            embot::app::application::theCANtracer &tracer = embot::app::application::theCANtracer::getInstance();
            tracer.print("tick", replies);            
        }
        
        count++;        
    }
#endif
    
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

    
//    debugtime = embot::sys::timeNow() - start; 
       
    return true;        
    
}

bool embot::app::application::theSTRAIN::Impl::tick(std::vector<embot::hw::can::Frame> &replies)
{   
    if(false == ticking)
    {
        return false;
    }
    
    debugtime = embot::sys::timeNow();
        
    // start adc acquisition
    acquisition_start();
             
    return true;    
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theSTRAIN::theSTRAIN()
: pImpl(new Impl)
{       

}

//static void dmaiscomplete(void *p)
//{
//    // copy data by mildly getting a mutex
//    // alert the task ...
//    
//    static std::uint32_t ii = 0;
//    ii++;
//    
//    embot::app::application::theSTRAIN::Impl *pImpl = reinterpret_cast<embot::app::application::theSTRAIN::Impl*>(p);
//    
//    //embot::sys::Action::EventToTask(pImpl->config.tickevent, pImpl->config.totask
//    
//    pImpl->config.totask->setEvent(pImpl->config.datareadyevent);
//    
//    //embot::hw::adc::get(embot::hw::adc::Port::one, items);
//}
         
bool embot::app::application::theSTRAIN::initialise(Config &config)
{
    pImpl->config = config;
    
    pImpl->action.set(embot::sys::Action::EventToTask(pImpl->config.tickevent, pImpl->config.totask));

    // read from eeprom and make sure it is coherent data (first ever time we init eeprom)
    pImpl->configdata.EEPROM_read();
    pImpl->runtimedata.clear();
    
    // init the hw used to acquire from strain gauges: amplifier PGA308 + adc channels.

    // PGA308
    
    embot::hw::PGA308::Config pga308cfg;
        
    // common settings
    pga308cfg.powerongpio = embot::hw::gpio::GPIO(EN_2V8_GPIO_Port, EN_2V8_Pin);
    pga308cfg.poweronstate = embot::hw::gpio::State::SET;
    pga308cfg.onewireconfig.rate = embot::hw::onewire::Rate::tenKbps;
    pga308cfg.onewireconfig.usepreamble = true;
    pga308cfg.onewireconfig.preamble = 0x55;
    
    // from embot::hw::PGA308::one to embot::hw::PGA308::six
    
    // embot::hw::PGA308::zero
    pga308cfg.onewirechannel = embot::hw::onewire::Channel::one;
    pga308cfg.onewireconfig.gpio = embot::hw::gpio::GPIO(W_STRAIN1_GPIO_Port, W_STRAIN1_Pin);
    embot::hw::PGA308::init(embot::hw::PGA308::Amplifier::one, pga308cfg);
    
    // embot::hw::PGA308::two
    pga308cfg.onewirechannel = embot::hw::onewire::Channel::two;
    pga308cfg.onewireconfig.gpio = embot::hw::gpio::GPIO(W_STRAIN2_GPIO_Port, W_STRAIN2_Pin);
    embot::hw::PGA308::init(embot::hw::PGA308::Amplifier::two, pga308cfg);
    
    // embot::hw::PGA308::three
    pga308cfg.onewirechannel = embot::hw::onewire::Channel::three;
    pga308cfg.onewireconfig.gpio = embot::hw::gpio::GPIO(W_STRAIN3_GPIO_Port, W_STRAIN3_Pin);
    embot::hw::PGA308::init(embot::hw::PGA308::Amplifier::three, pga308cfg);
        
    // embot::hw::PGA308::four
    pga308cfg.onewirechannel = embot::hw::onewire::Channel::four;
    pga308cfg.onewireconfig.gpio = embot::hw::gpio::GPIO(W_STRAIN4_GPIO_Port, W_STRAIN4_Pin);
    embot::hw::PGA308::init(embot::hw::PGA308::Amplifier::four, pga308cfg);    
    
    // embot::hw::PGA308::five
    pga308cfg.onewirechannel = embot::hw::onewire::Channel::five;
    pga308cfg.onewireconfig.gpio = embot::hw::gpio::GPIO(W_STRAIN5_GPIO_Port, W_STRAIN5_Pin);
    embot::hw::PGA308::init(embot::hw::PGA308::Amplifier::five, pga308cfg);     

    // embot::hw::PGA308::six
    pga308cfg.onewirechannel = embot::hw::onewire::Channel::six;
    pga308cfg.onewireconfig.gpio = embot::hw::gpio::GPIO(W_STRAIN6_GPIO_Port, W_STRAIN6_Pin);
    embot::hw::PGA308::init(embot::hw::PGA308::Amplifier::six, pga308cfg);   
    
    
    // now i must apply the values on eeprom pImpl->configdata.EEPROM_read
    // #warning TODO: apply eeprom values to PGA308 ... all channels.
    embot::hw::PGA308::TransferFunctionConfig tfc;    
    for(int i=0; i<6; i++)
    {
        embot::app::canprotocol::analog::polling::PGA308cfg1 pga308cfg1;
        pImpl->configdata.amplifiers_get(0, i, pga308cfg1); 
        tfc.load(pga308cfg1);    
        embot::hw::PGA308::set(static_cast<embot::hw::PGA308::Amplifier>(i), tfc);                
    }
        
    
    // ADC
//    #warning TODO: decide how to acquire ....
    // some thougths ... the adc takes many micro-sconds (> 200)
    // 1.   periodic. tick() is called at tx period. it just triggers the start of adc.
    //      the adc callback sends a acquired event to this object.
    //      this object gets the data generated by the adc (hei, protect with a mutex: object waits, the isr does not wait and 
    //      if busy it does not write.) by calling a new funtion called processdata() whcih does what tick() does now.
    //      .... 
    embot::hw::adc::Config adcConf;
    adcConf.numberofitems = 6;
    adcConf.destination = pImpl->runtimedata.data.dmabuffer;
    adcConf.oncompletion.callback = pImpl->alertdataisready;
    adcConf.oncompletion.arg = pImpl;
    embot::hw::adc::init(embot::hw::adc::Port::one, adcConf);
    pImpl->adcdataisready = false;
    
    embot::app::application::theCANtracer &tracer = embot::app::application::theCANtracer::getInstance();
     
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
       
    const std::uint8_t set = 0;
    value = pImpl->configdata.transformer_fullscale_get(set, channel);

    return true;    
}


bool embot::app::application::theSTRAIN::get_adc(embot::app::canprotocol::analog::polling::Message_GET_CH_ADC::ReplyInfo &replyinfo)
{    
    if(replyinfo.channel >= 6 )
    {
        replyinfo.adcvalue = 0;
        return false;
    }
    
    // it acquire once. it also restarts if required.
    pImpl->acquisition_oneshot();
        
    std::uint16_t v = 0;
    
    if(true == replyinfo.valueiscalibrated)
    {
        // we call processing() because the calibration is not done inside acquisition_retrieve() 
        pImpl->processing();
        
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
    
    const std::uint8_t set = 0;   

    // it was: value = pImpl->configdata.amplifier0_offset_get(set, channel);
    
    // it returns the beta   
    embot::app::canprotocol::analog::polling::PGA308cfg1 cfg1;
    pImpl->configdata.amplifiers_get(set, channel, cfg1); 
    
    // now i retrieve beta.
    embot::hw::PGA308::TransferFunctionConfig tfc;
    tfc.load(cfg1);  
    float beta = tfc.beta() * 8;            // move from [0, 8k) into [0, 64k)
    value = static_cast<std::uint16_t>(std::floor(beta));
    
    return true;    
}

bool embot::app::application::theSTRAIN::configure(embot::app::canprotocol::analog::polling::Message_SET_FULL_SCALES::Info &info)
{       
    // original strain code saves the value in ram only. it is saved in eeprom only when the message save2eeprom arrives     
    const std::uint8_t set = 0;
    pImpl->configdata.transformer_fullscale_set(set, info.channel, info.fullscale);

    return true;    
}

bool embot::app::application::theSTRAIN::get_eepromstatus(bool &saved)
{    
    saved = pImpl->configdata.isEEPROMsynched();
    return true;    
}


bool embot::app::application::theSTRAIN::save2eeprom()
{    
    pImpl->configdata.EEPROM_write();

    return true;    
}

bool embot::app::application::theSTRAIN::configure(embot::app::canprotocol::analog::polling::Message_SET_CH_DAC::Info &info)
{  
    if(info.channel >= 6 )
    {
        return false;
    } 
   
    // original strain code saves the value in ram only. it is saved in eeprom only when the message save2eeprom arrives  
    const std::uint8_t set = 0;    
        
    // it was: pImpl->configdata.amplifier0_offset_set(set, info.channel, info.offset);
    
    // i assign a beta ....
    
    embot::app::canprotocol::analog::polling::PGA308cfg1 cfg1;
    pImpl->configdata.amplifiers_get(set, info.channel, cfg1);

    embot::hw::PGA308::TransferFunctionConfig tfc;
    tfc.load(cfg1);  

    if(true == tfc.setbeta(static_cast<float>(info.offset)/8.f))
    {
        embot::hw::PGA308::set(static_cast<embot::hw::PGA308::Amplifier>(info.channel), tfc);   

        tfc.get(cfg1);
        pImpl->configdata.amplifiers_set(set, info.channel, cfg1);                
    }
            
    return true;    
}


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_GET_MATRIX_RC::ReplyInfo &replyinfo)
{    
    if((replyinfo.row >= 6) || (replyinfo.col >= 6))
    {
        replyinfo.value = 0;
        return false;
    }
    const std::uint8_t set = 0;
    replyinfo.value = static_cast<std::uint16_t>(pImpl->configdata.transformer_matrix_get(set, replyinfo.row, replyinfo.col));
    
    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::analog::polling::Message_SET_MATRIX_RC::Info &info)
{  
    if((info.row >= 6) || (info.col >= 6))
    {
        return false;
    }
    
    // original strain code saves the value in ram only. it is saved in eeprom only when the message save2eeprom arrives  
    const std::uint8_t set = 0;    
    pImpl->configdata.transformer_matrix_set(set, info.row, info.col, static_cast<embot::dsp::Q15>(info.value));

    return true;    
}


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_GET_MATRIX_G::ReplyInfo &replyinfo)
{        
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();

    const std::uint8_t set = 0;
    replyinfo.gain =  pImpl->configdata.transformer_gain_get(set);
        
    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::analog::polling::Message_SET_MATRIX_G::Info &info)
{  
    // original strain code saves the value in ram only. it is saved in eeprom only when the message save2eeprom arrives     
    const std::uint8_t set = 0;
    pImpl->configdata.transformer_gain_set(set, info.gain);

    return true;    
}

bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_GET_CALIB_TARE::ReplyInfo &replyinfo)
{  
    if(replyinfo.channel >= 6)
    {
        replyinfo.value = 0;
        return false;
    } 

    const std::uint8_t set = 0;
    replyinfo.value = static_cast<std::uint16_t>(pImpl->configdata.transformer_tare_get(set, replyinfo.channel));

    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE::Info &info)
{ 
    if(info.channel >= 6)
    {
        return false;
    }     

    // original strain code saves the value in ram only. it is saved in eeprom only when the message save2eeprom arrives
    const std::uint8_t set = 0;        

    bool ret = true;

    
    switch(info.mode)
    {
        case embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE::Mode::setchannelwithvalue:
        {
            pImpl->configdata.transformer_tare_set(set, info.channel, static_cast<embot::dsp::Q15>(info.value));
        } break;            

        case embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE::Mode::everychannelreset:
        {
            pImpl->configdata.transformer_tare_fill(set, embot::dsp::q15::zero);
        } break;

        case embot::app::canprotocol::analog::polling::Message_SET_CALIB_TARE::Mode::everychannelnegativeofadc:
        {                        
            // it acquires once. it also restarts if required.
            pImpl->acquisition_oneshot();
            
            for(int i=0; i<6; i++)
            {
                embot::dsp::Q15 value = embot::dsp::q15::U16toQ15(pImpl->runtimedata.data.adcvalue[i]);
                pImpl->configdata.transformer_tare_set(set, i, embot::dsp::q15::opposite(value));
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
 
    replyinfo.value =  static_cast<std::uint16_t>(pImpl->runtimedata.tare_get(replyinfo.channel));
    
    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE::Info &info)
{ 
    if(info.channel >= 6)
    {
        return false;
    }     
 

    bool ret = true;

    switch(info.mode)
    {
        case embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE::Mode::setchannelwithvalue:
        {
            pImpl->runtimedata.tare_set(info.channel, static_cast<embot::dsp::Q15>(info.value));
        } break;            

        case embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE::Mode::everychannelreset:
        {
            pImpl->runtimedata.tare_fill(embot::dsp::q15::zero);
        } break;

        case embot::app::canprotocol::analog::polling::Message_SET_CURR_TARE::Mode::everychannelnegativeoftorqueforce:
        {
            // we need to assign the runtimedata.data.tare[] with:  
            // currtare = -tq, where tq = ( M * (adc + calibtare) )
            // after acquisition() + processing we compute runtimedata.data.torqueforce[] to be:
            // torqueforce = M * (adc + calibtare) + runtimedata.data.tare
            // hence, we do that:
            // 1. we clear runtimedata.data.tare[] to be zero.
            // 2. we call processing()
            // 3. we assign runtimedata.data.tare[] with the opposite of runtimedata.data.torqueforce[] 
            // THE RESULT OF THIS OPERATION IS that we after the assignement of the currtare have a zero runtimedata.data.torqueforce 
            
            pImpl->runtimedata.tare_fill(embot::dsp::q15::zero);
                        
            // it acquires once. it also restarts periodic acquisition if it was active.
            pImpl->acquisition_oneshot();

            pImpl->processing();
            
            for(int i=0; i<6; i++)
            {                
                pImpl->runtimedata.data.tare[i] = embot::dsp::q15::opposite(pImpl->runtimedata.data.torqueforce[i]); 
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


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_AMPLIFIER_PGA308_CFG1_GET::ReplyInfo &replyinfo)
{  
    if(replyinfo.channel >= 6)
    {
        return false;
    }  
    
    if(replyinfo.set >= 3)
    {   
        return false;
    }
    
    pImpl->configdata.amplifiers_get(replyinfo.set, replyinfo.channel, replyinfo.cfg1); 
    return true;    
}


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_AMPLIFIER_GAINOFFSET_GET::ReplyInfo &replyinfo)
{  
    if(replyinfo.channel >= 6)
    {
        return false;
    }  
    
    if(replyinfo.set >= 3)
    {   
        return false;
    }
    
    embot::app::canprotocol::analog::polling::PGA308cfg1 cfg1;
    pImpl->configdata.amplifiers_get(replyinfo.set, replyinfo.channel, cfg1); 
    
    // now i retrieve alpha and beta
    embot::hw::PGA308::TransferFunctionConfig tfc;
    tfc.load(cfg1);  
    float alpha = tfc.alpha() * 100.0f;     // represent in 0.01 ticks
    float beta = tfc.beta() * 8;            // move from [0, 8k) into [0, 64k)
    replyinfo.gain = static_cast<std::uint16_t>(std::floor(alpha));
    replyinfo.offset = static_cast<std::uint16_t>(std::floor(beta));
    
    return true;    
}


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_GAIN_GET::ReplyInfo &replyinfo)
{  
    if(replyinfo.channel >= 6)
    {
        return false;
    }  
    
    if(replyinfo.set >= 3)
    {   
        return false;
    }
    
    embot::app::canprotocol::analog::polling::PGA308cfg1 cfg1;
    pImpl->configdata.amplifiers_get(replyinfo.set, replyinfo.channel, cfg1); 
    
    // now i retrieve limits of alpha.
    embot::hw::PGA308::TransferFunctionConfig tfc;
    tfc.load(cfg1);  
    float low = tfc.alpha(0) * 100.0f;     
    float high = tfc.alpha(64*1024-1) * 100.f;  
    replyinfo.lowest = static_cast<std::uint16_t>(std::floor(low));
    replyinfo.highest = static_cast<std::uint16_t>(std::floor(high));
    
    return true;    
}


bool embot::app::application::theSTRAIN::get(embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RANGE_OF_OFFSET_GET::ReplyInfo &replyinfo)
{  
    if(replyinfo.channel >= 6)
    {
        return false;
    }  
    
    if(replyinfo.set >= 3)
    {   
        return false;
    }
    
//    embot::app::canprotocol::analog::polling::PGA308cfg1 cfg1;
//    pImpl->configdata.amplifiers_get(replyinfo.set, replyinfo.channel, cfg1); 
//     
//    // now i retrieve limits of beta.
//    embot::hw::PGA308::TransferFunctionConfig tfc;
//    tfc.load(cfg1);  
//    float low = tfc.alpha(0) * 100.0f;     
//    float high = tfc.alpha(64*1024-1) * 100.f;  
    replyinfo.lowest = 0;
    replyinfo.highest = 0xffff;
    
    return true;    
}


bool  embot::app::application::theSTRAIN::resetamplifier(embot::app::canprotocol::analog::polling::Message_AMPLIFIER_RESET::Info &info)
{ 
    bool allchannels = false;
    if(0x0f == info.channel)
    {
        // it is ok. we do on every channel
        allchannels = true;
    }    
    else if(info.channel >= 6)
    {
        return false;
    }   

    if(info.set >= 3)
    {   
        return false;
    }
    
    if(true == allchannels)
    {
        for(std::uint8_t i=0; i<6; i++)
        {
            // i reset amplifier settings to default
            pImpl->configdata.amplifiers_reset(info.set, i);
            
            // i retrieve the current settings (they are default now).
            embot::app::canprotocol::analog::polling::PGA308cfg1 cfg1;
            pImpl->configdata.amplifiers_get(info.set, i, cfg1); 
            
            // i apply them to the pga308.
            embot::hw::PGA308::TransferFunctionConfig tfc;
            tfc.load(cfg1);    
            embot::hw::PGA308::set(static_cast<embot::hw::PGA308::Amplifier>(i), tfc);
        }        
    }
    else
    {
    
        // i reset amplifier settings to default
        pImpl->configdata.amplifiers_reset(info.set, info.channel);
        
        // i retrieve the current settings (they are default now).
        embot::app::canprotocol::analog::polling::PGA308cfg1 cfg1;
        pImpl->configdata.amplifiers_get(info.set, info.channel, cfg1); 
        
        // i apply them to the pga308.
        embot::hw::PGA308::TransferFunctionConfig tfc;
        tfc.load(cfg1);    
        embot::hw::PGA308::set(static_cast<embot::hw::PGA308::Amplifier>(info.channel), tfc);
        
    }

    return true;    
}

bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::analog::polling::Message_AMPLIFIER_PGA308_CFG1_SET::Info &info)
{ 
    bool allchannels = false;
    if(0x0f == info.channel)
    {
        // it is ok. we do on every channel
        allchannels = true;
    }    
    else if(info.channel >= 6)
    {
        return false;
    }   

    if(info.set >= 3)
    {   
        return false;
    }
    
    
    if(true == allchannels)
    {
        for(std::uint8_t i=0; i<6; i++)
        {
            pImpl->configdata.amplifiers_set(info.set, i, info.cfg1);
            
            embot::hw::PGA308::TransferFunctionConfig tfc;
            tfc.load(info.cfg1);    
            embot::hw::PGA308::set(static_cast<embot::hw::PGA308::Amplifier>(i), tfc);
        }        
    }
    else
    {    
        pImpl->configdata.amplifiers_set(info.set, info.channel, info.cfg1);
        
        embot::hw::PGA308::TransferFunctionConfig tfc;
        tfc.load(info.cfg1);    
        embot::hw::PGA308::set(static_cast<embot::hw::PGA308::Amplifier>(info.channel), tfc);
    }
   
    return true;    
}


bool embot::app::application::theSTRAIN::autocalib(embot::app::canprotocol::analog::polling::Message_AMPLIFIER_OFFSET_AUTOCALIB::Info &info, std::uint8_t &noisychannelmask, std::uint8_t &algorithmOKmask, std::uint8_t &finalmeasureOKmask, std::uint16_t &mae)
{
    std::uint8_t channelmask = 0;
    if(0x0f == info.channel)
    {
        // it is ok. we do on every channel
        channelmask = 0x3f;
    }
    else if(info.channel >= 6)
    {
        return false;
    }
    else
    {
        embot::binary::bit::set(channelmask, info.channel); 
    }

    if(info.set >= 3)
    {   
        return false;
    }
    
    noisychannelmask = 0;

    // acquire for some times to get the measure
    std::uint8_t Nsamples = info.samples2average;
    if(0 == Nsamples)
    {
        Nsamples = 4;
    }
    std::uint32_t measure[6] = {0};
    std::uint16_t _max[6] = {0};
    std::uint16_t _min[6] = {0xffff};
    std::memset(_max, 0, sizeof(_max));
    std::memset(_min, 0xff, sizeof(_min));
    
    for(std::uint8_t a=0; a<Nsamples; a++)
    {
        // it acquire once. it also restarts if required.        
        pImpl->acquisition_oneshot();
        for(std::uint8_t c=0; c<6; c++)
        {
            std::uint16_t tmp = pImpl->runtimedata.data.adcvalue[c] >> 3;
            measure[c] += tmp;
            if(tmp<=_min[c])
            {
                _min[c] = tmp;
            }
            if(tmp>=_max[c])
            {
                _max[c] = tmp;
            }
        }
    }

    std::uint16_t delta = 0;
    for(std::uint8_t c=0; c<6; c++)
    {
        if(Nsamples > 0)
        {
            measure[c] /= Nsamples;
        }
        
        delta = _max[c] - _min[c];
        
        if(delta > (info.tolerance >> 3))
        {
            embot::binary::bit::set(noisychannelmask, 7);            
            embot::binary::bit::set(noisychannelmask, c);            
        }
    }
    

    
    algorithmOKmask = 0;
    // perform alignment to target
    for(std::uint8_t i=0; i<6; i++)
    {
        if(true == embot::binary::bit::check(channelmask, i))
        {
            embot::app::canprotocol::analog::polling::PGA308cfg1 cfg1;
            pImpl->configdata.amplifiers_get(0, i, cfg1);
        
            embot::hw::PGA308::TransferFunctionConfig tfc;
            tfc.load(cfg1);  
            std::uint8_t Y = 0;
            std::uint16_t Z = 0;
            if(true == tfc.alignVOUT(static_cast<std::uint16_t>(measure[i]), (info.target >> 3), Y, Z))
            {
                embot::hw::PGA308::set(static_cast<embot::hw::PGA308::Amplifier>(i), tfc);   
                tfc.get(cfg1);
                pImpl->configdata.amplifiers_set(0, i, cfg1); 
                
                embot::binary::bit::set(algorithmOKmask, i);
            }
            else
            {                                
            }
        }
    }
    
    std::uint32_t mae32 = 0;
    finalmeasureOKmask = 0;
    std::uint8_t num = 0;
    
    // perform measure again to compute the error
    std::memset(measure, 0, sizeof(measure));
    std::memset(_max, 0, sizeof(_max));
    std::memset(_min, 0xff, sizeof(_min));
    for(std::uint8_t a=0; a<Nsamples; a++)
    {
        // it acquire once. it also restarts if required.        
        pImpl->acquisition_oneshot();
        for(std::uint8_t c=0; c<6; c++)
        {
            std::uint16_t tmp = pImpl->runtimedata.data.adcvalue[c] >> 3;
            measure[c] += tmp;
            if(tmp<=_min[c])
            {
                _min[c] = tmp;
            }
            if(tmp>=_max[c])
            {
                _max[c] = tmp;
            }
        }
    }

    for(std::uint8_t c=0; c<6; c++)
    {
        if((_max[c] - _min[c]) > (info.tolerance >> 3))
        {
            embot::binary::bit::set(noisychannelmask, 6);            
            embot::binary::bit::set(noisychannelmask, c);            
        }
        if(Nsamples > 0)
        {
            measure[c] /= Nsamples;
        }
        if(true == embot::binary::bit::check(channelmask, c))
        {
            num++;
            std::int32_t mm = measure[c] - (info.target >> 3);
            if(mm < 0)
            {
                mm = -mm;
            }
            mae32 += mm;
            if(mm < (info.tolerance >> 3))
            {
                embot::binary::bit::set(finalmeasureOKmask, c);
            }                       
        }       
    }
    if(num > 0)
    {
        mae32 /= num;
    }
    
    if(mae32 >= 0xffff)
    {
        mae = 0xffff;
    }
    else
    {
        mae = mae32 & 0xffff;
    }
   
    return true;    
}


bool  embot::app::application::theSTRAIN::set(embot::app::canprotocol::analog::polling::Message_AMPLIFIER_GAINOFFSET_SET::Info &info, float &alpha, float &beta)
{ 
    bool allchannels = false;
    if(0x0f == info.channel)
    {
        // it is ok. we do on every channel
        allchannels = true;
    }    
    else if(info.channel >= 6)
    {
        return false;
    }   

    if(info.set >= 3)
    {   
        return false;
    }
    
    
    if(true == allchannels)
    {
        for(std::uint8_t i=0; i<6; i++)
        {
            embot::app::canprotocol::analog::polling::PGA308cfg1 cfg1;
            pImpl->configdata.amplifiers_get(info.set, i, cfg1);
        
            embot::hw::PGA308::TransferFunctionConfig tfc;
            tfc.load(cfg1);  
            if(false == tfc.setalpha(static_cast<float>(info.gain)/100.f))
            {
                return false;
            }
            if(false == tfc.setbeta(static_cast<float>(info.offset)/8.f))
            {
                return false;
            }
            
            alpha = tfc.alpha();
            beta = tfc.beta();
            
            embot::hw::PGA308::set(static_cast<embot::hw::PGA308::Amplifier>(i), tfc);   

            tfc.get(cfg1);
            pImpl->configdata.amplifiers_set(info.set, i, cfg1);                  
        }        
    }
    else
    {    
        
            embot::app::canprotocol::analog::polling::PGA308cfg1 cfg1;
            pImpl->configdata.amplifiers_get(info.set, info.channel, cfg1);
        
            embot::hw::PGA308::TransferFunctionConfig tfc;
            tfc.load(cfg1);  
            if(false == tfc.setalpha(static_cast<float>(info.gain)/100.f))
            {
                return false;
            }
            if(false == tfc.setbeta(static_cast<float>(info.offset)/8.f))
            {
                return false;
            }
            
            alpha = tfc.alpha();
            beta = tfc.beta();
            
            embot::hw::PGA308::set(static_cast<embot::hw::PGA308::Amplifier>(info.channel), tfc);   

            tfc.get(cfg1);
            pImpl->configdata.amplifiers_set(info.set, info.channel, cfg1);                                       
        
    }
  
   
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

bool embot::app::application::theSTRAIN::processdata(std::vector<embot::hw::can::Frame> &replies)
{   
    return pImpl->processdata(replies);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



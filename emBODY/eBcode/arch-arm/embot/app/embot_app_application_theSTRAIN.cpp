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

#include "embot_core.h"
#include "embot_core_utils.h"
#include "embot_core_binary.h"
#include "embot_dsp.h"

#include "embot_hw.h"
#include "embot_hw_pga308_bsp.h"
#include "embot_hw_onewire_bsp.h"
#include "embot_hw_can.h"
#include "embot_hw_pga308.h"
#include "embot_hw_adc.h"
#include "embot_hw_flash.h"

#include "embot_hw_sys.h"


#include "embot_os_Timer.h"
#include "embot_os_Action.h"

#include "embot_prot_can.h"

#include "embot_app_theCANboardInfo.h"
#include "embot_app_theStorage.h"
#include "embot_app_application_theCANtracer.h"

#include "embot_app_theLEDmanager.h"


#define _VERIFYsizeof(sname, ssize)    typedef uint8_t GUARD##sname[ ( ssize == sizeof(sname) ) ? (1) : (-1)];

#define DEBUG_adc_traceiszero

// debug macros ... enable them only for debug
#undef DEBUG_enabled                       //  #define OR #undef

#if defined(DEBUG_enabled)

    // enables / disables the trace to the host of adc acquisition failures
    #undef DEBUG_adc_tracefailure               //  #define OR #undef

    // enables / disables the computation of acquisition timings from adc start to tx of FT messages
    #define DEBUG_acquisition_computetiming                 //  #define OR #undef

    #if defined(DEBUG_acquisition_computetiming)
        // enables / disables the transmission to the host of the acquisition timing
        #define DEBUG_acquisition_computetiming_andtraceit  //  #define OR #undef
    #endif

#endif // #if defined(DEBUG_enabled)

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


    
struct embot::app::application::theSTRAIN::Impl
{ 
    static const std::uint8_t numOfChannels = 6;
    static const std::uint8_t numOfSets = 3;
    
    // the new amplifier is the pga308 by Texas Instruments. It has a completly different configuration
    // we use for it the compact form of the can message
    
    struct amplifierConfig_v00_t
    { 
        embot::prot::can::analog::polling::PGA308cfg1 pga308cfg1 {};   
        
        void factoryreset() {
            embot::hw::pga308::TransferFunctionConfig tfc {};
            tfc.setDefault();
            std::uint16_t       GD;
            std::uint8_t        GI; 
            std::uint8_t        S;
            std::uint8_t        GO; 
            std::uint8_t        Voffsetcoarse;
            std::uint16_t       Vzerodac;
            tfc.get(GD, GI, S, GO, Voffsetcoarse, Vzerodac);
            pga308cfg1 = {GD, GI, S, GO, Voffsetcoarse, Vzerodac};
        }
        
        amplifierConfig_v00_t() { factoryreset(); }
    }; _VERIFYsizeof(amplifierConfig_v00_t, 6)
    
    struct amplifierConfig_t
    {   // it is _v01_t 
        embot::prot::can::analog::polling::PGA308cfg1 pga308cfg1 {}; 
        std::uint8_t ffu[2] {0};        
        
        void factoryreset() {
            embot::hw::pga308::TransferFunctionConfig tfc {};
            tfc.setDefault();
            std::uint16_t       GD;
            std::uint8_t        GI; 
            std::uint8_t        S;
            std::uint8_t        GO; 
            std::uint8_t        Voffsetcoarse;
            std::uint16_t       Vzerodac;
            tfc.get(GD, GI, S, GO, Voffsetcoarse, Vzerodac);
            pga308cfg1 = {GD, GI, S, GO, Voffsetcoarse, Vzerodac};
            ffu[0] = ffu[1] = 0;
        }
        
        amplifierConfig_t() { factoryreset(); }
        
        void import(const amplifierConfig_v00_t& v00)
        {
            std::memmove(&pga308cfg1, &v00.pga308cfg1, sizeof(pga308cfg1));
            std::memset(ffu, 0, sizeof(ffu));
        }
        
    }; _VERIFYsizeof(amplifierConfig_t, 8)
    
    

    // the matrix transformation which is done after adc acquisition    
    // tf = M * (adc + tare)
    // then tf can also be corrected with an extra tare which is set in runtime: TF = tf + tare_set_in_runtime
    // the 6 values of TF are those transmitted to the host.
    // in here are also stored fullscale (and gain?) which are queried and used by the remote host to compensate the received TF values
    
    struct transformerConfig_v00_t
    {
        std::uint8_t                dummy;          // just to remove a hole
        std::uint8_t                gain;           // the gain managed with GET_MATRIX_G / SET_MATRIX_G which is NOT USED INSIDE THE MPU        
        std::uint16_t               fullscale[6];   // the fullscale of the measurements: NOT USED INSIDE THE MPU
        embot::dsp::Q15             tare[6];        // in format 1Q15: the stored tare
        embot::dsp::Q15             matrix[6*6];    // in format 1Q15: transformation matrix from the 6 strain-gauge values to the 6 force-torque values        
    };  _VERIFYsizeof(transformerConfig_v00_t, 98) 
    
    struct transformerConfig_t
    {
        std::uint8_t                gain;           // the gain managed with GET_MATRIX_G / SET_MATRIX_G which is NOT USED INSIDE THE MPU   
        std::uint8_t                ffu[7];
        std::uint16_t               fullscale[6];   // the fullscale of the measurements: NOT USED INSIDE THE MPU
        embot::dsp::Q15             tare[6];        // in format 1Q15: the stored tare
        embot::dsp::Q15             matrix[6*6];    // in format 1Q15: transformation matrix from the 6 strain-gauge values to the 6 force-torque values 
        
        void import(const transformerConfig_v00_t& v00)
        {
            gain = v00.gain;
            std::memset(ffu, 0, sizeof(ffu));
            std::memmove(fullscale, v00.fullscale, sizeof(fullscale));
            std::memmove(tare, v00.tare, sizeof(tare));
            std::memmove(matrix, v00.matrix, sizeof(matrix));
        }
        
    };  _VERIFYsizeof(transformerConfig_t, 104) 
    
    
    
    // the complete configuration for the transformer and the six amplifiers
    
    struct setOfConfig_v00_t
    {
        transformerConfig_v00_t transformer;            // the config of the transformer from dacs to forcetorque array
        amplifierConfig_v00_t   amplifiers[6];          // the NEW config of the amplifiers from strain gauge to values given to dacs          
    };  _VERIFYsizeof(setOfConfig_v00_t, 134)
    
    struct setOfConfig_t
    {
        transformerConfig_t     transformer;            // the config of the transformer from dacs to forcetorque array
        amplifierConfig_t       amplifiers[6];          // the NEW config of the amplifiers from strain gauge to values given to dacs      

        void import(const setOfConfig_v00_t &v00)
        {
            transformer.import(v00.transformer);
            for(std::uint8_t c=0; c<6; c++)
            {
                amplifiers[c].import(v00.amplifiers[c]);
            }            
        }
        
    };  _VERIFYsizeof(setOfConfig_t, 152)
    

    static const std::uint32_t validityKey = 0xdead0001;
    static const std::uint32_t validityKey_v00 = 0x7777dead;
    
    
    
    // it is a holder of config data which is eventally saved to eeprom. 
    // this struct is without methods so that i am sure its sizeof() is the sum of the variables. 
    // this struct it is managed by the StrainConfig class
    
    struct strainConfigData_v00_t
    {   
        std::uint32_t           key;
        char                    serial[8];              // the serial number of the strain. it start at offset +4     
        setOfConfig_v00_t       set[numOfSets];         // the three configurations sets. they start at offset +12
    };  _VERIFYsizeof(strainConfigData_v00_t, 416)
    
    
    // it is version v01 and with respect to version v00 has a remapping of teh same data into
    // multiples of 16. plus it has permanentregulationset. 
    struct strainConfigData_t
    {   
        std::uint32_t           key;
        char                    serial[8];              // the serial number of the strain  (at offset +4)   
        std::uint8_t            permanentregulationset; // the set to use at bootstrap (at offset +12)
        std::uint8_t            ffu[3];                 // a useful hole in the eeprom (at offset +13)
        setOfConfig_t           set[numOfSets];         // the three configurations sets (at offset +16)
        
        void import(const strainConfigData_v00_t &v00)
        {
            key = validityKey;
            std::memmove(serial, v00.serial, sizeof(serial));
            permanentregulationset = 0;
            std::memset(ffu, 0, sizeof(ffu));
            for(std::uint8_t s=0; s<numOfSets; s++)
            {
                set[s].import(v00.set[s]);
            }
        }
        
    };  _VERIFYsizeof(strainConfigData_t, 472)

        
    
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
        
        bool migrate(std::uint32_t fromkey)
        {
            if(validityKey == fromkey)
            {
                return true;
            }
            
            if(validityKey_v00 != fromkey)
            {
                return false;
            }
            
            // ok, ... we do business; strainConfigData_v00_t is smaller than strainConfigData_t
            strainConfigData_v00_t datav00;
            std::memmove(&datav00, &data,  sizeof(strainConfigData_v00_t));
            // now we get prepare data and then... we copy old stuff inside
            def_values();
            data.import(datav00);
           
            return true;
        }
        
        bool EEPROM_read()
        {
#if defined(STRAIN2_APP_AT_64K)            
            embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
            canbrdinfo.userdataread(0, sizeof(data), &data);
#else            
            embot::app::theStorage &storage = embot::app::theStorage::getInstance();
            storage.read(0, sizeof(data), &data);
#endif            
            synched = true;
            
            if(validityKey != data.key)
            {   // if it is not valid, then 
                // 1. attempt to migrate.
                // 2. if operation fails ... set default and impose validity key   
                if(false == migrate(data.key))
                {
                    def_values();                    
                    data.key = validityKey;
                }
                EEPROM_write();
            }
                        
            return synched;
        }
        
        bool EEPROM_write()
        {
            data.key = validityKey;
#if defined(STRAIN2_APP_AT_64K)            
            embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance(); 
            canbrdinfo.userdatawrite(0, sizeof(data), &data);
#else            
            embot::app::theStorage &storage = embot::app::theStorage::getInstance();
            storage.write(0, sizeof(data), &data);
#endif            
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
#if defined(STRAIN2_APP_AT_64K)
#else            
            embot::app::theStorage &storage = embot::app::theStorage::getInstance(); 
            storage.init(embot::hw::flash::getpartition(embot::hw::FLASH::applicationstorage).address, 1024);
#endif            
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
        
        void permanentregulationset_set(uint8_t set)
        {
            if(set < numOfSets)
            {
                data.permanentregulationset = set; 
                synched = false;                
            }
        }
        
        uint8_t permanentregulationset_get()
        {
            return data.permanentregulationset;
        }
                        
        void amplifiers_set(std::uint8_t set, std::uint8_t channel, const embot::prot::can::analog::polling::PGA308cfg1 &cfg1)
        {
            if((channel < numOfChannels) && (set < numOfSets))
            {
                data.set[set].amplifiers[channel].pga308cfg1 = cfg1;
                synched = false;
            }
        } 
        
        bool amplifiers_get(std::uint8_t set, std::uint8_t channel, embot::prot::can::analog::polling::PGA308cfg1 &cfg1)
        {
            bool ret = false;
            if((channel < numOfChannels) && (set < numOfSets))
            {
                cfg1 = data.set[set].amplifiers[channel].pga308cfg1;
                ret = true;
            }
            
            return ret;
        } 

        void amplifiers_reset(std::uint8_t set, std::uint8_t channel)
        {
            if((channel < numOfChannels) && (set < numOfSets))
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
            if((set < numOfSets) && (r < numOfChannels) && (c < numOfChannels))
            {
                transfQ15matrix[set].set(r, c, value);
                synched = false;
            }
        }
        
        embot::dsp::Q15 transformer_matrix_get(std::uint8_t set, std::uint8_t r, std::uint8_t c)
        { 
            embot::dsp::Q15 v = embot::dsp::q15::zero;
            
            if((set < numOfSets) && (r < numOfChannels) && (c < numOfChannels))
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
            if((set < numOfSets) && (channel < numOfChannels))
            {
                transfQ15tare[set].set(channel, 0, value);
                synched = false;
            }
        }
        
        embot::dsp::Q15 transformer_tare_get(std::uint8_t set, std::uint8_t channel)
        {
            embot::dsp::Q15 v = embot::dsp::q15::zero;
            if((set < numOfSets) && (channel < numOfChannels))
            {
                v = transfQ15tare[set].get(channel, 0);
            }            
            return v;
        }        
        
        bool def_values()
        {
            clear();
            
            data.key = validityKey;
            std::snprintf(data.serial, sizeof(data.serial), "SN0000");     
            data.permanentregulationset = 0;
            // and now the set   
            for(int s=0; s<numOfSets; s++)
            {
                // transformer
                transformer_gain_set(s, 1);
                transformer_fullscale_fill(s, 0x7fff);
                transformer_tare_fill(s, embot::dsp::q15::zero);
                transformer_matrix_fill_diagonal(s, embot::dsp::q15::posOneNearly);
                transformer_gain_set(s, 1);
                
                for(int chn=0; chn<numOfChannels; chn++)
                {
                    amplifiers_reset(s, chn);
                }
            }
            
                        
            synched = false;
            return synched;
        }
        
        
        std::uint16_t transformer_fullscale_get(std::uint8_t set, std::uint8_t channel)
        {
            if((set < numOfSets) && (channel < numOfChannels))
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
            if((set < numOfSets) && (channel < numOfChannels))
            {
                data.set[set].transformer.fullscale[channel] = value;
                synched = false;
            }
        } 

        void transformer_fullscale_fill(std::uint8_t set, std::uint16_t value)
        {
            for(int channel=0; channel<numOfChannels; channel++)
            {
                transformer_fullscale_set(set, channel, value); 
            }
        }                
        
    };
    
        
    // it is a holder of volatile data used in runtime. we use the same approach as for config data
    struct StrainRuntimeData_t
    {
        embot::core::Time         txperiod;
        
        std::uint16_t               dmabuffer[6]; 
        std::uint16_t               adcvalue[6];        // the values as acquired by the adc... but scaled to full 64k range. original strain had 16-bit adc   
        
        embot::dsp::Q15     q15value[6];        // same as adcvalue but in q15 format       
        embot::dsp::Q15     tare[6];            // the tare applied as a last correction. it is always ::zero unless changed with SET_CURR_TARE
        embot::dsp::Q15     forcetorque[6];
        
        // the calibrated values for torque and force. they are raw values, not in dsp::Q15 format
        embot::core::utils::Triple<std::uint16_t>    torque;
        embot::core::utils::Triple<std::uint16_t>    force;
        
        embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode strainmode;
        
        bool adcsaturation;        
        embot::prot::can::analog::periodic::ADCsaturation saturationonchannel[6];

        std::uint8_t adcfailures;
        std::uint8_t adciszero;
        
        std::uint8_t set2use;
        
        
        // methods
        void clear() { 
            std::memset(q15value, 0, sizeof(q15value)); 
            std::memset(tare, 0, sizeof(tare)); 
            std::memset(dmabuffer, 0, sizeof(dmabuffer));
            std::memset(adcvalue, 0, sizeof(adcvalue));
            std::memset(forcetorque, 0, sizeof(forcetorque));
            force.clear();
            torque.clear();
            strainmode = embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode::none;
            adcsaturation = false;
            adcfailures = 0;
            adciszero = 0;
            std::memset(saturationonchannel, static_cast<std::uint8_t>(embot::prot::can::analog::periodic::ADCsaturation::NONE), sizeof(saturationonchannel));
            set2use = 0;
        }       
        StrainRuntimeData_t() { clear(); }
    };

    
    class StrainRuntimeData
    {
        public:

        StrainRuntimeData_t data;   
        
        // used for the sake of matrix-based calculation
        embot::dsp::q15::matrix currtareQ15vector;
        embot::dsp::q15::matrix forcetorqueQ15vector;
        embot::dsp::q15::matrix adcvalueQ15vector;
        
                        
        static const std::uint16_t ADCsaturationLow = 1000;
        static const std::uint16_t ADCsaturationHigh = 64000;
        
        
        // methods
        
        bool check_adcsaturation()
        {
            data.adcsaturation = false;
            std::memset(data.saturationonchannel, static_cast<std::uint8_t>(embot::prot::can::analog::periodic::ADCsaturation::NONE), sizeof(data.saturationonchannel));


            
            for(int channel=0; channel<numOfChannels; channel++)
            {
                data.saturationonchannel[channel] = embot::prot::can::analog::periodic::ADCsaturation::NONE;
                
                if(data.adcvalue[channel] < ADCsaturationLow)
                {
                    data.saturationonchannel[channel] = embot::prot::can::analog::periodic::ADCsaturation::LOW;                    
                    data.adcsaturation = true;                
                }
                else if(data.adcvalue[channel] > ADCsaturationHigh)
                {
                    data.saturationonchannel[channel] = embot::prot::can::analog::periodic::ADCsaturation::HIGH;
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
            if(channel<numOfChannels)
            {
                data.tare[channel] = value;
            }
        }
        
        embot::dsp::Q15 tare_get(std::uint8_t channel)
        {
            if(channel<numOfChannels)
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
        
        void set2use_set(std::uint8_t set)
        {
            if(set < numOfSets)
            {
                data.set2use = set;
            }
        }
        
        std::uint8_t set2use_get()
        {
            return data.set2use;
        }
        
        StrainRuntimeData()
        {
            clear();
            
            forcetorqueQ15vector.load(6, 1, data.forcetorque);
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

   
      
#if defined(DEBUG_acquisition_computetiming) 
    embot::core::Time debugtime;
    embot::core::Time timestartADC;
    embot::core::relTime durationofADC;
    embot::core::relTime durationof00;
    embot::core::relTime durationof01;
    embot::core::relTime durationof02;
    embot::core::relTime durationof03;
    embot::core::relTime durationof04;
    embot::core::relTime durationof05;
    embot::core::relTime durationof06;
    embot::core::relTime durationof07;
    embot::core::relTime durationofDSP;
    embot::core::relTime durationofALL;
    std::uint32_t acquisitioncounter;
#endif // #if defined(DEBUG_acquisition_computetiming) 

    Config config;
           
    bool ticking;
    volatile bool adcdataisready;
    
    
    embot::os::Timer *ticktimer;
    embot::os::Action action;
        
                     
    // i put in here some values proper of the STRAIN. they may be many.... hence better to organise them in a struct later on.
    
    
    StrainRuntimeData runtimedata;
    StrainConfigData configdata;

		adc2ft_ns::adc2ft_class adc2ft; // Instance of model class		

    Impl() 
    {

//        defaultAmplifConfig.factoryreset();     
#if defined(DEBUG_acquisition_computetiming) 
        debugtime = 0;
#endif        
        ticking = false;  

        ticktimer = new embot::os::Timer;   

                
        configdata.clear();
        runtimedata.clear();
        
        runtimedata.data.txperiod = 2*embot::core::time1millisec;       

        adcdataisready = false;        
    }
    
   
    bool start(const embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode mode);
    bool stop();
    
    bool evalstop(embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode &mode);
    bool evalrestart(const embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode mode);
    
    bool tick(std::vector<embot::prot::can::Frame> &replies);
    bool processdata(std::vector<embot::prot::can::Frame> &replies);
    
    bool configure();
    
    bool acquisition_start();
    bool acquisition_waituntilcompletion(embot::core::relTime timeout);
    bool acquisition_retrieve();
    
    bool acquisition_oneshot(embot::core::relTime timeout = 3*embot::core::time1millisec, bool restartifitwasticking = true);
    
    bool processing();
    
    bool fill(embot::prot::can::analog::periodic::Message_UNCALIBFORCE_VECTOR_DEBUGMODE::Info &info);
    bool fill(embot::prot::can::analog::periodic::Message_UNCALIBTORQUE_VECTOR_DEBUGMODE::Info &info);
    bool fill(const bool calibrated, embot::prot::can::analog::periodic::Message_FORCE_VECTOR::Info &info);
    bool fill(const bool calibrated, embot::prot::can::analog::periodic::Message_TORQUE_VECTOR::Info &info);
    
    
    static void alertdataisready(void *p)
    {
        embot::app::application::theSTRAIN::Impl *mypImpl = reinterpret_cast<embot::app::application::theSTRAIN::Impl*>(p);
        
        if(true == mypImpl->ticking)
        {
            mypImpl->config.totask->setEvent(mypImpl->config.datareadyevent);
        }

#if defined(DEBUG_acquisition_computetiming)         
        mypImpl->debugtime = embot::core::now();
        mypImpl->durationof00 = mypImpl->debugtime - mypImpl->timestartADC;
#endif
    
        mypImpl->adcdataisready = true;                            
    }
    
    bool regulationset2index(const std::uint8_t rs, std::uint8_t &index)
    {
        if(static_cast<std::uint8_t>(embot::prot::can::analog::polling::StrainRegulationSet::theoneinuse) == rs)
        {
            index = runtimedata.set2use_get();
            return true;
        }
        else if(rs <= numOfSets)
        {
            index = rs - 1;
            return true;
        }
        
        return false;        
    }
    
    std::uint8_t indexofregsetinuse()
    {
        return runtimedata.set2use_get();
    }
    
    bool ischannelvalid(const std::uint8_t ch, bool &allchannels)
    {
        allchannels = false;
        if(0x0f == ch)
        {
            allchannels = true;
        }    
        else if(ch >= numOfChannels)
        {
            return false;
        }
        return true;        
    }
    
//    bool ischannelvalid(const std::uint8_t ch)
//    {
//        if(ch >= numOfChannels)
//        {
//            return false;
//        }
//        return true;        
//    }
    
          
    
    void amplifiers_reset(const std::uint8_t setindex, const embot::prot::can::analog::polling::StrainChannel ch)
    {    
        std::uint8_t first = 0;
        std::uint8_t nextlast = numOfChannels;
        
        if(ch != embot::prot::can::analog::polling::StrainChannel::all)
        {
            first = static_cast<std::uint8_t>(ch);
            nextlast = first + 1;
        }
        
        for(std::uint8_t i=first; i<nextlast; i++)
        {
            // i reset amplifier settings to default
            configdata.amplifiers_reset(setindex, i);               
        }
    }
    
    
    void amplifiers_set(const std::uint8_t setindex, const embot::prot::can::analog::polling::StrainChannel ch, const embot::prot::can::analog::polling::PGA308cfg1 &cfg1)
    {    
        std::uint8_t first = 0;
        std::uint8_t nextlast = numOfChannels;
        
        if(ch != embot::prot::can::analog::polling::StrainChannel::all)
        {
            first = static_cast<std::uint8_t>(ch);
            nextlast = first + 1;
        }
        
        for(std::uint8_t i=first; i<nextlast; i++)
        {
            // i set amplifier
            configdata.amplifiers_set(setindex, i, cfg1);               
        }
    }
    
    
    
    bool amplifiers_alfabeta_reset(const std::uint8_t setindex, const embot::prot::can::analog::polling::StrainChannel ch, float &a, float &b)
    {    
        std::uint8_t first = 0;
        std::uint8_t nextlast = numOfChannels;
        
        if(ch != embot::prot::can::analog::polling::StrainChannel::all)
        {
            first = static_cast<std::uint8_t>(ch);
            nextlast = first + 1;
        }
        
        for(std::uint8_t i=first; i<nextlast; i++)
        {
            embot::prot::can::analog::polling::PGA308cfg1 cfg1;
            embot::hw::pga308::TransferFunctionConfig tfc;
            // modify the tsf
            tfc.setDefault();                
            // retrieve the resulting alpha and beta for filling the reply
            a = tfc.alpha();
            b = tfc.beta();                
            // retrieve the new cfg1 and apply it to the specified regulation set
            std::uint16_t       GD;
            std::uint8_t        GI; 
            std::uint8_t        S;
            std::uint8_t        GO; 
            std::uint8_t        Voffsetcoarse;
            std::uint16_t       Vzerodac;
            tfc.get(GD, GI, S, GO, Voffsetcoarse, Vzerodac);
            cfg1 = {GD, GI, S, GO, Voffsetcoarse, Vzerodac};
            configdata.amplifiers_set(setindex, i, cfg1);         
        }
        
        return true;
    }
    
    bool amplifiers_alfabeta_set(const std::uint8_t setindex, const embot::prot::can::analog::polling::StrainChannel ch, const float alpha, const float beta, float &a, float &b)
    {    
        std::uint8_t first = 0;
        std::uint8_t nextlast = numOfChannels;
        
        if(ch != embot::prot::can::analog::polling::StrainChannel::all)
        {
            first = static_cast<std::uint8_t>(ch);
            nextlast = first + 1;
        }
        
        for(std::uint8_t i=first; i<nextlast; i++)
        {
            embot::prot::can::analog::polling::PGA308cfg1 cfg1;
            embot::hw::pga308::TransferFunctionConfig tfc;
            
            // retrieve current cfg1 and associated transform function
            configdata.amplifiers_get(setindex, i, cfg1);
            tfc.load(cfg1.GD, cfg1.GI, cfg1.S, cfg1.GO, cfg1.Voffsetcoarse, cfg1.Vzerodac);  
            
            // modify the tsf
            if(false == tfc.setalpha(alpha)) 
            {
                return false;
            }
            if(false == tfc.setbeta(beta))
            {
                return false;
            }
            
            // retrieve the resulting alpha and beta for filling the reply
            a = tfc.alpha();
            b = tfc.beta();

            // retrieve the new cfg1 and apply it to the specified regulation set
            std::uint16_t       GD;
            std::uint8_t        GI; 
            std::uint8_t        S;
            std::uint8_t        GO; 
            std::uint8_t        Voffsetcoarse;
            std::uint16_t       Vzerodac;
            tfc.get(GD, GI, S, GO, Voffsetcoarse, Vzerodac);
            cfg1 = {GD, GI, S, GO, Voffsetcoarse, Vzerodac};
            configdata.amplifiers_set(setindex, i, cfg1);               
        }
        
        return true;
    }
    
    void amplifiers_applyregulationsetochipPGA308(const std::uint8_t setindex, const embot::prot::can::analog::polling::StrainChannel ch)
    {    
        std::uint8_t first = 0;
        std::uint8_t nextlast = numOfChannels;
        
        if(ch != embot::prot::can::analog::polling::StrainChannel::all)
        {
            first = static_cast<std::uint8_t>(ch);
            nextlast = first + 1;
        }
        
        for(std::uint8_t i=first; i<nextlast; i++)
        {
            embot::prot::can::analog::polling::PGA308cfg1 cfg1;
            configdata.amplifiers_get(setindex, i, cfg1); 
            embot::hw::pga308::TransferFunctionConfig tfc;
            tfc.load(cfg1.GD, cfg1.GI, cfg1.S, cfg1.GO, cfg1.Voffsetcoarse, cfg1.Vzerodac);    
            embot::hw::pga308::set(static_cast<embot::hw::PGA308>(i), tfc);                
        }
    }  
                      
};



bool embot::app::application::theSTRAIN::Impl::start(const embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode mode)
{    
    if(true == ticking)
    {
        ticktimer->stop();
        ticking = false;
    }
    
    
    runtimedata.data.strainmode = mode;
            
    switch(runtimedata.data.strainmode)
    {
        case embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode::txCalibrated:
        case embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode::txUncalibrated:
        case embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode::txAll:
        {
            ticking = true; 
        } break;
        
        default:
        {
            ticking = false;
        } break;             
    }    
    
    if(false == ticking)
    {
        return true;        
    }
    
    embot::os::Timer::Config cfg(runtimedata.data.txperiod, action, embot::os::Timer::Mode::forever);
    ticktimer->start(cfg);
    
    ticking = true;    
#if defined(DEBUG_acquisition_computetiming) 
    acquisitioncounter = 0;
#endif
    return true;
}


bool embot::app::application::theSTRAIN::Impl::stop()
{ 
    runtimedata.data.strainmode = embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode::none;
    
    ticktimer->stop();
    ticking = false;    
    return true;
}

bool embot::app::application::theSTRAIN::Impl::evalstop(embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode &mode)
{
    mode = runtimedata.data.strainmode;
    if(true == ticking)
    {
        stop();
    }
    return true;    
}

bool embot::app::application::theSTRAIN::Impl::evalrestart(const embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode mode)
{
    return start(mode);
}


bool embot::app::application::theSTRAIN::Impl::fill(embot::prot::can::analog::periodic::Message_UNCALIBFORCE_VECTOR_DEBUGMODE::Info &info)
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


bool embot::app::application::theSTRAIN::Impl::fill(embot::prot::can::analog::periodic::Message_UNCALIBTORQUE_VECTOR_DEBUGMODE::Info &info)
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

bool embot::app::application::theSTRAIN::Impl::fill(const bool calibrated, embot::prot::can::analog::periodic::Message_FORCE_VECTOR::Info &info)
{
    bool ret = true;
    
    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
    
    if(true == calibrated)
    {
        info.x = runtimedata.data.force.x;
        info.y = runtimedata.data.force.y;
        info.z = runtimedata.data.force.z;
    }
    else
    {
        info.x = runtimedata.data.adcvalue[0];
        info.y = runtimedata.data.adcvalue[1];
        info.z = runtimedata.data.adcvalue[2];        
    }
    
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


bool embot::app::application::theSTRAIN::Impl::fill(const bool calibrated, embot::prot::can::analog::periodic::Message_TORQUE_VECTOR::Info &info)
{
    bool ret = true;

    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();

    if(true == calibrated)
    {    
        info.x = runtimedata.data.torque.x;
        info.y = runtimedata.data.torque.y;
        info.z = runtimedata.data.torque.z;
    }
    else
    {
        info.x = runtimedata.data.adcvalue[3];
        info.y = runtimedata.data.adcvalue[4];
        info.z = runtimedata.data.adcvalue[5];        
    }  
    
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

bool embot::app::application::theSTRAIN::Impl::acquisition_start()
{
    runtimedata.data.adcfailures = 0;
    runtimedata.data.adciszero = 0;

#if defined(DEBUG_acquisition_computetiming) 
    acquisitioncounter ++;
    timestartADC = embot::core::now();
#endif

    std::memset(runtimedata.data.dmabuffer, 0xff, sizeof(runtimedata.data.dmabuffer));    
    adcdataisready = false;
    embot::hw::adc::start(embot::hw::ADC::one);    
    return true;
}

bool embot::app::application::theSTRAIN::Impl::acquisition_waituntilcompletion(embot::core::relTime timeout)
{
    embot::core::Time now = embot::core::now();
    embot::core::Time endtime = now + timeout;
    
//    embot::core::Time start = now;
//    static embot::core::Time delta = 0;
     // delta is 429 usec

    
    bool ret = true;
    
    for(;;)
    {
        if(true == adcdataisready)
        {
            break;
        }
        now = embot::core::now();
        if(now > endtime)
        {
           ret = false;
           break;
        }                   
    }
    
//    delta = now - start;
    
    return ret;
}


bool embot::app::application::theSTRAIN::Impl::acquisition_oneshot(embot::core::relTime timeout, bool restartifitwasticking)
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
        start(runtimedata.data.strainmode);     
    }
    
    return ret;    
}




bool embot::app::application::theSTRAIN::Impl::acquisition_retrieve()
{

    // 1. acquire from adc and put inside adcvalue    
    std::memmove(runtimedata.data.adcvalue, runtimedata.data.dmabuffer, sizeof(runtimedata.data.adcvalue));
     

    // 2. convert adcvalue[] into q15value[]

    runtimedata.data.adcfailures = 0;
    runtimedata.data.adciszero = 0;
    for(int i=0; i<numOfChannels; i++)
    {
        if(0xffff == runtimedata.data.adcvalue[i])
        {
            runtimedata.data.adcfailures ++;
        }
        if(0 == runtimedata.data.adcvalue[i])
        {
            runtimedata.data.adciszero ++;
        }
        runtimedata.data.adcvalue[i] <<= 3; // adc value is 13 bits. we need to scale it to 64k
        runtimedata.data.q15value[i] = embot::dsp::q15::U16toQ15(runtimedata.data.adcvalue[i]);
    }
    
    // 3. also check vs saturation
    runtimedata.check_adcsaturation();
    
    
#if defined(DEBUG_adc_traceiszero) 
    if(0 != runtimedata.data.adciszero)
    {
        static bool adcwaszero = false;
        
        if(false == adcwaszero)
        {
            embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();    
            embot::app::LEDwaveT<64> ledwave(100*embot::core::time1millisec, 30, std::bitset<64>(0b010101));
            theleds.get(embot::hw::LED::one).wave(&ledwave);              
        }
        
        adcwaszero = true;
        runtimedata.data.adciszero = 0;
    }
    else
    {
//        static uint32_t count = 0;
//        
//        if(0 == (count % 1000))
//        {
//            embot::app::application::theCANtracer &tracer = embot::app::application::theCANtracer::getInstance();
//            tracer.print("tick", replies);            
//        }
//        
//        count++;        
    }
#endif // if defined(DEBUG_adc_tracefailure)
    
    return true;
}


bool embot::app::application::theSTRAIN::Impl::processing()
{
    // use variable set2use, originally initted w/ eeprom value but possibly changed by can message
    std::uint8_t set2use = runtimedata.set2use_get();
    
    // in adcvalue[] we performed adc acquisition, in value[] we put the q15 values and we also have checkd vs adc saturation
    
    
    // now prepare calib data: it will be inside the triples: runtimedata.data.torque and runtimedata.data.force 
    
        
    // apply formula: forcetorque = M * (adcvalue + calibtare) + currtare
    // default values are: M = IdentityMatrix(0x7fff); calibtare = Vector(0); currtare = Vector(0)
    
    bool q15saturated =  false;
    
    const embot::dsp::q15::matrix& handleCalibMatrixQ15 = configdata.transformer_matrix_handle(set2use);     
    const embot::dsp::q15::matrix& handleCalibTareQ15 = configdata.transformer_tare_handle(set2use);   
		//Move to the correct place Luca
		std::memcpy(adc2ft.rtP.calibration_matrix,handleCalibMatrixQ15.data,sizeof(adc2ft.rtP.calibration_matrix));     
		std::memcpy(adc2ft.rtP.calibration_offsets,handleCalibTareQ15.data,sizeof(adc2ft.rtP.calibration_offsets));     
	
		//Luca
		uint16_t a=adc2ft.rtP.calibration_offsets[0];
		uint16_t b=handleCalibTareQ15.data[0];
		if(a!=b)
		{
			int pippo=0;
			pippo++;
			printf("");
		}
		
		//Luca
	
		//Move to the correct place end Luca
        
	  // Remove Luca
    embot::dsp::Q15 q15tmpvector[6];
    embot::dsp::q15::matrix tmpQ15vector(6, 1, q15tmpvector);
    embot::dsp::q15::add(runtimedata.adcvalueQ15vector, handleCalibTareQ15, tmpQ15vector, q15saturated);
    embot::dsp::q15::multiply(handleCalibMatrixQ15, tmpQ15vector, runtimedata.forcetorqueQ15vector, q15saturated);
    embot::dsp::q15::add(runtimedata.forcetorqueQ15vector, runtimedata.currtareQ15vector, runtimedata.forcetorqueQ15vector, q15saturated);
    // Remove Luca
		
    // copy 
    
    // now in forcetorqueQ15vector (runtimedata.data.forcetorque[]) we have the result ... 
    
    // if we did have saturation ... we send old safe data.
   
    // if we are ok, we use what in runtimedata.data.forcetorque[] to compute safe data and data to send
    // data2send = safedata = 0x8000 + forcetorque
   
    if(false == runtimedata.data.adcsaturation)
    {
			for(size_t index=0;index<6;++index)	
			{
				adc2ft.rtU.adc[index] = runtimedata.data.adcvalue[index];
			}
			adc2ft.step();
			
			runtimedata.data.force.set(embot::dsp::q15::Q15toU16(adc2ft.rtY.ft_q15[0]),embot::dsp::q15::Q15toU16(adc2ft.rtY.ft_q15[1]),embot::dsp::q15::Q15toU16(adc2ft.rtY.ft_q15[2]));
			runtimedata.data.torque.set(embot::dsp::q15::Q15toU16(adc2ft.rtY.ft_q15[3]),embot::dsp::q15::Q15toU16(adc2ft.rtY.ft_q15[4]),embot::dsp::q15::Q15toU16(adc2ft.rtY.ft_q15[5]));
      //runtimedata.data.torque.set(embot::dsp::q15::Q15toU16(runtimedata.data.forcetorque[0]), embot::dsp::q15::Q15toU16(runtimedata.data.forcetorque[1]), embot::dsp::q15::Q15toU16(runtimedata.data.forcetorque[2]));
       
		}
    else
    {
        // we dont update so that in runtimedata.data.torque and runtimedata.data.force there are always valid values
    }
        
    
    return true;
}

// some timings ... 
// - from start of adc acquisition until calling of this function: ~638usec
// - from start of adc acquisition until after acquisition_retrieve() inside this function: ~664usec 
// - from start of adc acquisition until after processing() inside this function, case of calib mode: ~1041usec 
// - from start of adc acquisition until end of this function, case of calib mode: ~1164usec 
bool embot::app::application::theSTRAIN::Impl::processdata(std::vector<embot::prot::can::Frame> &replies)
{   
    if(false == ticking)
    {
        return false;
    }

#if defined(DEBUG_acquisition_computetiming)        
    debugtime = embot::core::now();
    durationof01 = debugtime - timestartADC;
#endif

    // retreve acquired adc values
    acquisition_retrieve();

#if defined(DEBUG_acquisition_computetiming) 
    debugtime = embot::core::now();
    durationof02 = debugtime - timestartADC;
#endif
 
    // processing of acquired data
    processing();

#if defined(DEBUG_acquisition_computetiming)    
    debugtime = embot::core::now();
    durationof03 = debugtime - timestartADC;
#endif

#if defined(DEBUG_adc_tracefailure) 
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
#endif // if defined(DEBUG_adc_tracefailure)


//#if defined(DEBUG_adc_traceiszero) 
//    if(0 != runtimedata.data.adciszero)
//    {
//        static bool adcwaszero = false;
//        
//        if(false == adcwaszero)
//        {
//            embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();    
//            embot::app::LEDwaveT<64> ledwave(100*embot::core::time1millisec, 30, std::bitset<64>(0b010101));
//            theleds.get(embot::hw::LED::one).wave(&ledwave);              
//        }
//        
//        adcwaszero = true;
//        runtimedata.data.adciszero = 0;
//    }
//    else
//    {
////        static uint32_t count = 0;
////        
////        if(0 == (count % 1000))
////        {
////            embot::app::application::theCANtracer &tracer = embot::app::application::theCANtracer::getInstance();
////            tracer.print("tick", replies);            
////        }
////        
////        count++;        
//    }
//#endif // if defined(DEBUG_adc_tracefailure)
    
 
    
    embot::prot::can::Frame frame;  
    
    switch(runtimedata.data.strainmode)
    {
        case embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode::txCalibrated:
        case embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode::txUncalibrated:
        {
            bool calibrated = (embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode::txCalibrated == runtimedata.data.strainmode) ? true : false;
            // we send up two messages A and B which may contain either calib or uncalib data
            embot::prot::can::analog::periodic::Message_FORCE_VECTOR::Info forceinfo0A;
            if(true == fill(calibrated, forceinfo0A))
            {
                embot::prot::can::analog::periodic::Message_FORCE_VECTOR msg;
                msg.load(forceinfo0A);
                msg.get(frame);
                replies.push_back(frame);
            }            
        
            embot::prot::can::analog::periodic::Message_TORQUE_VECTOR::Info torqueinfo0B;
            if(true == fill(calibrated, torqueinfo0B))
            {
                embot::prot::can::analog::periodic::Message_TORQUE_VECTOR msg;
                msg.load(torqueinfo0B);
                msg.get(frame);
                replies.push_back(frame);
            }  
        
        } break;
        
        case embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode::txAll:
        {
            // we send up two messages A and B w/ calib data
            embot::prot::can::analog::periodic::Message_FORCE_VECTOR::Info forceinfo0A;
            if(true == fill(true, forceinfo0A))
            {
                embot::prot::can::analog::periodic::Message_FORCE_VECTOR msg;
                msg.load(forceinfo0A);
                msg.get(frame);
                replies.push_back(frame);
            }            
        
            embot::prot::can::analog::periodic::Message_TORQUE_VECTOR::Info torqueinfo0B;
            if(true == fill(true, torqueinfo0B))
            {
                embot::prot::can::analog::periodic::Message_TORQUE_VECTOR msg;
                msg.load(torqueinfo0B);
                msg.get(frame);
                replies.push_back(frame);
            }  

            // and we send up two more messages 8 and 9 w/ uncalib data
            embot::prot::can::analog::periodic::Message_UNCALIBFORCE_VECTOR_DEBUGMODE::Info forceinfo08;
            if(true == fill(forceinfo08))
            {
                embot::prot::can::analog::periodic::Message_UNCALIBFORCE_VECTOR_DEBUGMODE msg;
                msg.load(forceinfo08);
                msg.get(frame);
                replies.push_back(frame);
            }            
        
            embot::prot::can::analog::periodic::Message_UNCALIBTORQUE_VECTOR_DEBUGMODE::Info torqueinfo09;
            if(true == fill(torqueinfo09))
            {
                embot::prot::can::analog::periodic::Message_UNCALIBTORQUE_VECTOR_DEBUGMODE msg;
                msg.load(torqueinfo09);
                msg.get(frame);
                replies.push_back(frame);
            }              
            
        } break;
        
        
        default:
        {
        } break;
        
    }
    

#if defined(DEBUG_acquisition_computetiming)

    debugtime = embot::core::now();
    durationof04 = debugtime - timestartADC;

    #if defined(DEBUG_acquisition_computetiming_andtraceit)    
 
    // and now i fill the frame with the times ...
    
    embot::prot::can::analog::periodic::Message_USERDEF::Info infoU;
    embot::prot::can::analog::periodic::Message_USERDEF msgU;
    infoU.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
    infoU.data[0] = (durationof00 >> 4) & 0xff; 
    infoU.data[1] = (durationof01 >> 4) & 0xff;  
    infoU.data[2] = (durationof02 >> 4) & 0xff; 
    infoU.data[3] = (durationof03 >> 4) & 0xff; 
    infoU.data[4] = (durationof04 >> 4) & 0xff; 
    infoU.data[5] = 0;
    infoU.data[6] = durationof04 & 0xff; 
    infoU.data[7] = (durationof04 & 0xff00) >> 8; 
    msgU.load(infoU);
    msgU.get(frame);
    replies.push_back(frame);
    
    #endif // #if defined(DEBUG_acquisition_computetiming_andtraceit) 

#endif  // #if defined(DEBUG_acquisition_computetiming) 

    return true;        
    
}

bool embot::app::application::theSTRAIN::Impl::tick(std::vector<embot::prot::can::Frame> &replies)
{   
    if(false == ticking)
    {
        return false;
    }
#if defined(DEBUG_acquisition_computetiming)     
    debugtime = embot::core::now();
#endif        
    // start adc acquisition
    acquisition_start();
             
    return true;    
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------


embot::app::application::theSTRAIN& embot::app::application::theSTRAIN::getInstance()
{
    static theSTRAIN* p = new theSTRAIN();
    return *p;
}

embot::app::application::theSTRAIN::theSTRAIN()
//    : pImpl(new Impl)
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::application::theSTRAIN::~theSTRAIN() { }


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
//    //embot::os::Action::EventToThread(pImpl->config.tickevent, pImpl->config.totask
//    
//    pImpl->config.totask->setEvent(pImpl->config.datareadyevent);
//    
//    //embot::hw::adc::get(embot::hw::adc::ADC::one, items);
//}
         
bool embot::app::application::theSTRAIN::initialise(Config &config)
{
    pImpl->config = config;
    
    pImpl->action.load(embot::os::EventToThread(pImpl->config.tickevent, pImpl->config.totask));

    // read from eeprom and make sure it is coherent data (first ever time we init eeprom)
    pImpl->configdata.EEPROM_read();
    pImpl->runtimedata.clear();
    
    // i init the set2use as the one stored in eeprom
    pImpl->runtimedata.set2use_set(pImpl->configdata.permanentregulationset_get());
    
    // init the hw used to acquire from strain gauges: amplifier pga308 + adc channels.

    // pga308
    
    embot::hw::pga308::Config pga308cfg;
    
    // common settings
    pga308cfg.powerongpio = embot::hw::pga308::getBSP().getPROP(embot::hw::PGA308::one)->poweron; // embot::hw::GPIO(EN_2V8_GPIO_Port, EN_2V8_Pin);
    pga308cfg.poweronstate = embot::hw::gpio::State::SET;
    pga308cfg.onewireconfig.rate = embot::hw::onewire::Rate::tenKbps;
    pga308cfg.onewireconfig.usepreamble = true;
    pga308cfg.onewireconfig.preamble = 0x55;
    
    // from embot::hw::pga308::one to embot::hw::pga308::six
    
    // embot::hw::PGA308::one
    pga308cfg.onewirechannel = embot::hw::ONEWIRE::one;
    pga308cfg.onewireconfig.gpio = embot::hw::onewire::getBSP().getPROP(embot::hw::ONEWIRE::one)->gpio; // embot::hw::GPIO(W_STRAIN1_GPIO_Port, W_STRAIN1_Pin);
    embot::hw::pga308::init(embot::hw::PGA308::one, pga308cfg);
    
    // embot::hw::PGA308::two
    pga308cfg.onewirechannel = embot::hw::ONEWIRE::two;
    pga308cfg.onewireconfig.gpio = embot::hw::onewire::getBSP().getPROP(embot::hw::ONEWIRE::two)->gpio; //embot::hw::GPIO(W_STRAIN2_GPIO_Port, W_STRAIN2_Pin);
    embot::hw::pga308::init(embot::hw::PGA308::two, pga308cfg);
    
    // embot::hw::PGA308::three
    pga308cfg.onewirechannel = embot::hw::ONEWIRE::three;
    pga308cfg.onewireconfig.gpio = embot::hw::onewire::getBSP().getPROP(embot::hw::ONEWIRE::three)->gpio;; // embot::hw::GPIO(W_STRAIN3_GPIO_Port, W_STRAIN3_Pin);
    embot::hw::pga308::init(embot::hw::PGA308::three, pga308cfg);
        
    // embot::hw::PGA308::four
    pga308cfg.onewirechannel = embot::hw::ONEWIRE::four;
    pga308cfg.onewireconfig.gpio = embot::hw::onewire::getBSP().getPROP(embot::hw::ONEWIRE::four)->gpio; // embot::hw::GPIO(W_STRAIN4_GPIO_Port, W_STRAIN4_Pin);
    embot::hw::pga308::init(embot::hw::PGA308::four, pga308cfg);    
    
    // embot::hw::PGA308::five
    pga308cfg.onewirechannel = embot::hw::ONEWIRE::five;
    pga308cfg.onewireconfig.gpio = embot::hw::onewire::getBSP().getPROP(embot::hw::ONEWIRE::five)->gpio; // embot::hw::GPIO(W_STRAIN5_GPIO_Port, W_STRAIN5_Pin);
    embot::hw::pga308::init(embot::hw::PGA308::five, pga308cfg);     

    // embot::hw::PGA308::six
    pga308cfg.onewirechannel = embot::hw::ONEWIRE::six;
    pga308cfg.onewireconfig.gpio = embot::hw::onewire::getBSP().getPROP(embot::hw::ONEWIRE::six)->gpio; // embot::hw::GPIO(W_STRAIN6_GPIO_Port, W_STRAIN6_Pin);
    embot::hw::pga308::init(embot::hw::PGA308::six, pga308cfg);   
    
    
    // now i must apply the values on eeprom pImpl->configdata.EEPROM_read
    
    std::uint8_t set2use = pImpl->runtimedata.set2use_get();
    pImpl->amplifiers_applyregulationsetochipPGA308(set2use, embot::prot::can::analog::polling::StrainChannel::all);
    
//    // #warning TODO: apply eeprom values to pga308 ... all channels. 
//    embot::hw::pga308::TransferFunctionConfig tfc;    
//    // set-todo: use variable set2use, originally initted w/ eeprom value but possibly changed by can message. 
//    #warning when changed by can message, remember to set the tfc ...
//    std::uint8_t set2use = pImpl->runtimedata.set2use_get();
//    for(int c=0; c<pImpl->numOfChannels; c++)
//    {
//        embot::prot::can::analog::polling::PGA308cfg1 pga308cfg1;
//        pImpl->configdata.amplifiers_get(set2use, c, pga308cfg1); 
//        tfc.load(cfg1.GD, cfg1.GI, cfg1.S, cfg1.GO, cfg1.Voffsetcoarse, cfg1.Vzerodac);    
//        embot::hw::pga308::set(static_cast<embot::hw::PGA308>(c), tfc);                
//    }
        
    
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
    adcConf.oncompletion = { pImpl->alertdataisready, pImpl.get() };
    embot::hw::adc::init(embot::hw::ADC::one, adcConf);
    pImpl->adcdataisready = false;
    
    embot::app::theCANtracer &tracer = embot::app::theCANtracer::getInstance();
     
    return true;
}




bool embot::app::application::theSTRAIN::autocalib(const embot::prot::can::analog::polling::Message_AMPLIFIER_OFFSET_AUTOCALIB::Info &info, std::uint8_t &noisychannelmask, std::uint8_t &algorithmOKmask, std::uint8_t &finalmeasureOKmask, std::uint16_t &mae)
{
    if(static_cast<std::uint8_t>(embot::prot::can::analog::polling::StrainRegulationSet::theoneinuse) != info.regulationset)
    {   // cannot launch on a regulation set which is not in use
        mae = 54321;
        return false;
    }
    
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(info.regulationset, setindex))
    {
        return false;
    } 
    
    bool allchannels = false;
    if(false == pImpl->ischannelvalid(info.channel, allchannels))
    {   // we may operate on all channels
        return false;
    }

    std::uint8_t channelmask = 0;
    if(true == allchannels)
    {
        channelmask = 0x3f;
    }
    else
    {
        embot::core::binary::bit::set(channelmask, info.channel);
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
        for(std::uint8_t c=0; c<pImpl->numOfChannels; c++)
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
    for(std::uint8_t c=0; c<pImpl->numOfChannels; c++)
    {
        if(Nsamples > 0)
        {
            measure[c] /= Nsamples;
        }
        
        delta = _max[c] - _min[c];
        
        if(delta > (info.tolerance >> 3))
        {
            embot::core::binary::bit::set(noisychannelmask, 7);            
            embot::core::binary::bit::set(noisychannelmask, c);            
        }
    }
    

    
    algorithmOKmask = 0;
    // perform alignment to target
    for(std::uint8_t i=0; i<pImpl->numOfChannels; i++)
    {
        if(true == embot::core::binary::bit::check(channelmask, i))
        {
            embot::prot::can::analog::polling::PGA308cfg1 cfg1;
            pImpl->configdata.amplifiers_get(setindex, i, cfg1);
        
            embot::hw::pga308::TransferFunctionConfig tfc;
            tfc.load(cfg1.GD, cfg1.GI, cfg1.S, cfg1.GO, cfg1.Voffsetcoarse, cfg1.Vzerodac);  
            std::uint8_t Y = 0;
            std::uint16_t Z = 0;
            if(true == tfc.alignVOUT(static_cast<std::uint16_t>(measure[i]), (info.target >> 3), Y, Z))
            {
                embot::hw::pga308::set(static_cast<embot::hw::PGA308>(i), tfc);   
                std::uint16_t       GD;
                std::uint8_t        GI; 
                std::uint8_t        S;
                std::uint8_t        GO; 
                std::uint8_t        Voffsetcoarse;
                std::uint16_t       Vzerodac;
                tfc.get(GD, GI, S, GO, Voffsetcoarse, Vzerodac);
                cfg1 = {GD, GI, S, GO, Voffsetcoarse, Vzerodac};
                pImpl->configdata.amplifiers_set(setindex, i, cfg1); 
                
                embot::core::binary::bit::set(algorithmOKmask, i);
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
        for(std::uint8_t c=0; c<pImpl->numOfChannels; c++)
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

    for(std::uint8_t c=0; c<pImpl->numOfChannels; c++)
    {
        if((_max[c] - _min[c]) > (info.tolerance >> 3))
        {
            embot::core::binary::bit::set(noisychannelmask, 6);            
            embot::core::binary::bit::set(noisychannelmask, c);            
        }
        if(Nsamples > 0)
        {
            measure[c] /= Nsamples;
        }
        if(true == embot::core::binary::bit::check(channelmask, c))
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
                embot::core::binary::bit::set(finalmeasureOKmask, c);
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


bool embot::app::application::theSTRAIN::setTXperiod(embot::core::relTime txperiod)
{
    // if ticking: stop it
    if(true == pImpl->ticking)
    {
        stop();
    }
    
    pImpl->runtimedata.data.txperiod = txperiod;

    return true;       
}

bool embot::app::application::theSTRAIN::start(const embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode mode)
{    
    return pImpl->start(mode);
}


bool embot::app::application::theSTRAIN::stop()
{    
    return pImpl->stop();
}


bool embot::app::application::theSTRAIN::tick(std::vector<embot::prot::can::Frame> &replies)
{   
    return pImpl->tick(replies);
}

bool embot::app::application::theSTRAIN::processdata(std::vector<embot::prot::can::Frame> &replies)
{   
    return pImpl->processdata(replies);
}


// interface to CANagentSTRAIN

bool embot::app::application::theSTRAIN::set(const embot::prot::can::analog::polling::Message_SET_TXMODE::Info &info)
{
    bool ret = false;
    if(true == info.transmit)
    {
        ret = start(info.strainmode);        
    }
    else
    {
        ret = stop();     
    }
    return ret;
}


bool embot::app::application::theSTRAIN::set(const embot::prot::can::analog::polling::Message_SET_SERIAL_NO::Info &info)
{
    // original strain code saves the value in ram only. it is saved in eeprom only when the message save2eeprom arrives    
    pImpl->configdata.serial_set(info.serial);
    
    return true;       
}
  
bool embot::app::application::theSTRAIN::set(const embot::prot::can::analog::polling::Message_SET_CANDATARATE::Info &info)
{
    // if ticking: stop it
    if(true == pImpl->ticking)
    {
        stop();
    }
    
    pImpl->runtimedata.data.txperiod = info.txperiod;

    return true;       
}
  
bool embot::app::application::theSTRAIN::set(const embot::prot::can::analog::polling::Message_SET_FULL_SCALES::Info &info)
{
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(info.regulationset, setindex))
    {
        return false;
    }
    
    bool allchannels = false;
    if(false == pImpl->ischannelvalid(info.channel, allchannels))
    {   
        return false;
    }  
    
    if(allchannels)
    {   // we cannot operate on all channels
        return false;
    }
    
    pImpl->configdata.transformer_fullscale_set(setindex, info.channel, info.fullscale);

    return true;        
}
   
bool embot::app::application::theSTRAIN::set(const embot::prot::can::analog::polling::Message_SAVE2EE::Info &info)
{
    pImpl->configdata.EEPROM_write();
    return true;      
}
   
bool embot::app::application::theSTRAIN::set(const embot::prot::can::analog::polling::Message_SET_CH_DAC::Info &info)
{
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(info.regulationset, setindex))
    {
        return false;
    }
    
    bool allchannels = false;
    if(false == pImpl->ischannelvalid(info.channel, allchannels))
    {   
        return false;
    }  
    
    if(allchannels)
    {   // we cannot operate on all channels
        return false;
    }

    // i assign a beta ....
    
    embot::prot::can::analog::polling::PGA308cfg1 cfg1;
    pImpl->configdata.amplifiers_get(setindex, info.channel, cfg1);

    embot::hw::pga308::TransferFunctionConfig tfc;
    tfc.load(cfg1.GD, cfg1.GI, cfg1.S, cfg1.GO, cfg1.Voffsetcoarse, cfg1.Vzerodac);  

    if(true == tfc.setbeta(static_cast<float>(info.offset)/8.f))
    {
        std::uint16_t       GD;
        std::uint8_t        GI; 
        std::uint8_t        S;
        std::uint8_t        GO; 
        std::uint8_t        Voffsetcoarse;
        std::uint16_t       Vzerodac;
        tfc.get(GD, GI, S, GO, Voffsetcoarse, Vzerodac);
        cfg1 = {GD, GI, S, GO, Voffsetcoarse, Vzerodac};
        pImpl->configdata.amplifiers_set(setindex, info.channel, cfg1);   
        
        // i apply settings to the pga308. but only if the setindex is the one in use...
        if(setindex == pImpl->indexofregsetinuse()) 
        {            
            embot::hw::pga308::set(static_cast<embot::hw::PGA308>(info.channel), tfc);   
        }
        
    }
            
    return true;      
}
   
bool embot::app::application::theSTRAIN::set(const embot::prot::can::analog::polling::Message_SET_MATRIX_RC::Info &info)
{
    if((info.row >=  pImpl->numOfChannels) || (info.col >=  pImpl->numOfChannels))
    {
        return false;
    }
    
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(info.regulationset, setindex))
    {
        return false;
    }
    
    // original strain code saves the value in ram only. it is saved in eeprom only when the message save2eeprom arrives   
    pImpl->configdata.transformer_matrix_set(setindex, info.row, info.col, static_cast<embot::dsp::Q15>(info.value));

    return true;       
}
   
bool embot::app::application::theSTRAIN::set(const embot::prot::can::analog::polling::Message_SET_MATRIX_G::Info &info)
{
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(info.regulationset, setindex))
    {
        return false;
    } 
    
    // original strain code saves the value in ram only. it is saved in eeprom only when the message save2eeprom arrives     
    pImpl->configdata.transformer_gain_set(setindex, info.gain);

    return true;       
}
   
bool embot::app::application::theSTRAIN::set(const embot::prot::can::analog::polling::Message_SET_CALIB_TARE::Info &info)
{
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(info.regulationset, setindex))
    {
        return false;
    } 

    bool ret = true;
    
    switch(info.mode)
    {
        case embot::prot::can::analog::polling::Message_SET_CALIB_TARE::Mode::setchannelwithvalue:
        {
            bool allchannels = false;
            if(false == pImpl->ischannelvalid(info.channel, allchannels))
            {   
                return false;
            }  
            
            if(allchannels)
            {   // we cannot operate on all channels
                return false;
            }
            
            pImpl->configdata.transformer_tare_set(setindex, info.channel, static_cast<embot::dsp::Q15>(info.value));
        } break;            

        case embot::prot::can::analog::polling::Message_SET_CALIB_TARE::Mode::everychannelreset:
        {
            // we dont care about channel: we always operate on all
            pImpl->configdata.transformer_tare_fill(setindex, embot::dsp::q15::zero);
        } break;

        case embot::prot::can::analog::polling::Message_SET_CALIB_TARE::Mode::everychannelnegativeofadc:
        {
            // there is no meaning to do that on a regulation set different from the one in use
            if(static_cast<std::uint8_t>(embot::prot::can::analog::polling::StrainRegulationSet::theoneinuse) != info.regulationset)
            {   
                return false;
            }
                        
            // it acquires once. it also restarts if required.
            pImpl->acquisition_oneshot();
            
            // we dont care about channel: we alwyas operate on all
            for(int i=0; i<pImpl->numOfChannels; i++)
            {
                embot::dsp::Q15 value = embot::dsp::q15::U16toQ15(pImpl->runtimedata.data.adcvalue[i]);
                pImpl->configdata.transformer_tare_set(setindex, i, embot::dsp::q15::opposite(value));
            }         
            
        } break;    

        default:
        case embot::prot::can::analog::polling::Message_SET_CALIB_TARE::Mode::unknown:
        {
            ret = false;
        } break;        
    }
    

    return ret;       
}
   
bool embot::app::application::theSTRAIN::set(const embot::prot::can::analog::polling::Message_SET_CURR_TARE::Info &info)
{
    bool ret = true;

    switch(info.mode)
    {
        case embot::prot::can::analog::polling::Message_SET_CURR_TARE::Mode::setchannelwithvalue:
        {
            bool allchannels = false;
            if(false == pImpl->ischannelvalid(info.channel, allchannels))
            {   
                return false;
            }  
            
            if(allchannels)
            {   // we cannot operate on all channels
                return false;
            }
            pImpl->runtimedata.tare_set(info.channel, static_cast<embot::dsp::Q15>(info.value));
        } break;            

        case embot::prot::can::analog::polling::Message_SET_CURR_TARE::Mode::everychannelreset:
        {
            // we dont care about channel: we operate on all
            pImpl->runtimedata.tare_fill(embot::dsp::q15::zero);
        } break;

        case embot::prot::can::analog::polling::Message_SET_CURR_TARE::Mode::everychannelnegativeofforcetorque:
        {
            // we dont care about channel: we operate on all
            
            // we need to assign the runtimedata.data.tare[] with:  
            // currtare = -tq, where tq = ( M * (adc + calibtare) )
            // after acquisition() + processing we compute runtimedata.data.forcetorque[] to be:
            // forcetorque = M * (adc + calibtare) + runtimedata.data.tare
            // hence, we do that:
            // 1. we clear runtimedata.data.tare[] to be zero.
            // 2. we call processing()
            // 3. we assign runtimedata.data.tare[] with the opposite of runtimedata.data.forcetorque[] 
            // THE RESULT OF THIS OPERATION IS that we after the assignement of the currtare have a zero runtimedata.data.forcetorque 
            
            pImpl->runtimedata.tare_fill(embot::dsp::q15::zero);
                        
            // it acquires once. it also restarts periodic acquisition if it was active.
            pImpl->acquisition_oneshot();

            pImpl->processing();
            
            for(int i=0; i<pImpl->numOfChannels; i++)
            {                
                pImpl->runtimedata.data.tare[i] = embot::dsp::q15::opposite(pImpl->runtimedata.data.forcetorque[i]); 
            }
        } break;    

        default:
        case embot::prot::can::analog::polling::Message_SET_CURR_TARE::Mode::unknown:
        {
            ret = false;
        } break;        
    }
    
    return ret;        
}
   
bool embot::app::application::theSTRAIN::set(const embot::prot::can::analog::polling::Message_AMPLIFIER_RESET::Info &info)
{
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(info.regulationset, setindex))
    {
        return false;
    } 
    
    bool allchannels = false;
    if(false == pImpl->ischannelvalid(info.channel, allchannels))
    {   
        return false;
    }  

    
    if(allchannels)
    {   // we may operate on all channels
        //return false;
    }
    
    // it may be zero, one, ..., five, all
    embot::prot::can::analog::polling::StrainChannel targetStrainChannel = static_cast<embot::prot::can::analog::polling::StrainChannel>(info.channel);
   
    // reset amplifier(s) to default
    pImpl->amplifiers_reset(setindex, targetStrainChannel);    
   
    // apply settings also to the target pga308. but only if the setindex is the one in use...
    if(setindex == pImpl->indexofregsetinuse())
    {
        pImpl->amplifiers_applyregulationsetochipPGA308(setindex, targetStrainChannel);    
    }  

    return true;        
}
   
bool embot::app::application::theSTRAIN::set(const embot::prot::can::analog::polling::Message_AMPLIFIER_PGA308_CFG1_SET::Info &info)
{
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(info.regulationset, setindex))
    {
        return false;
    } 
    
    bool allchannels = false;
    if(false == pImpl->ischannelvalid(info.channel, allchannels))
    {   
        return false;
    }  
    
    if(allchannels)
    {   // we may operate on all channels
        //return false;
    }

    // it may be zero, one, ..., five, all
    embot::prot::can::analog::polling::StrainChannel targetStrainChannel = static_cast<embot::prot::can::analog::polling::StrainChannel>(info.channel);

    // set amplifier(s)
    pImpl->amplifiers_set(setindex, targetStrainChannel, info.cfg1);    
    
    // apply settings also to the target pga308. but only if the setindex is the one in use...
    if(setindex == pImpl->indexofregsetinuse())
    {
        pImpl->amplifiers_applyregulationsetochipPGA308(setindex, targetStrainChannel);    
    }  
    
    return true;        
}
   
bool embot::app::application::theSTRAIN::set(const embot::prot::can::analog::polling::Message_AMPLIFIER_OFFSET_AUTOCALIB::Info &info, embot::prot::can::analog::polling::Message_AMPLIFIER_OFFSET_AUTOCALIB::ReplyInfo &replyinfo)
{
    replyinfo.regulationset = info.regulationset;
    replyinfo.channel = info.channel;
    return autocalib(info, replyinfo.noisychannelmask, replyinfo.algorithmOKmask, replyinfo.finalmeasureOKmask, replyinfo.mae);
}
   
bool embot::app::application::theSTRAIN::set(const embot::prot::can::analog::polling::Message_AMPLIFIER_GAINOFFSET_SET::Info &info)
{
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(info.regulationset, setindex))
    {
        return false;
    } 
    
    bool allchannels = false;
    if(false == pImpl->ischannelvalid(info.channel, allchannels))
    {   // we may operate on all channels
        return false;
    }

    // it may be zero, one, ..., five, all
    embot::prot::can::analog::polling::StrainChannel targetStrainChannel = static_cast<embot::prot::can::analog::polling::StrainChannel>(info.channel);
    
    bool set2default = (1 == info.mode) ? true : false;
    
    float a = 0;
    float b = 0;    
    
    bool ret = true;
    
    if(true == set2default)
    { 
        ret = pImpl->amplifiers_alfabeta_reset(setindex, targetStrainChannel, a, b);        
    }
    else
    {
        float alpha = static_cast<float>(info.gain)/100.f;  // info.gain has 0.01 ticks
        float beta = static_cast<float>(info.offset)/8.f;   // info.offset is in range [0, 64k) and here we need range [0, 8k)
        ret = pImpl->amplifiers_alfabeta_set(setindex, targetStrainChannel, alpha, beta, a, b);        
    }
    
    if(false == ret)
    {
        return false;
    }
      
    // apply settings also to the target pga308. but only if the setindex is the one in use...
    if(setindex == pImpl->indexofregsetinuse())
    {
        pImpl->amplifiers_applyregulationsetochipPGA308(setindex, targetStrainChannel);    
    }  
    
    return true;        
}
   
bool embot::app::application::theSTRAIN::set(const embot::prot::can::analog::polling::Message_REGULATIONSET_SET::Info &info)
{
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(info.regulationset, setindex))
    {
        return false;
    } 
    
    bool ret = false;
    
    // some explanations:
    // if we change the regulation set in use (::temporary), then we should:
    // 1. stop acquisition.
    // 2. change the set number
    // 3. apply the regulation set to the amplifiers
    // 4. no need to operate on regulation matrix and offset because at every tick we retrieve the correct one.
    //    ... aka: function embot::app::application::theSTRAIN::Impl::processing() uses set2use = runtimedata.set2use_get()
    //    to retrieve handles of handleCalibMatrixQ15 and handleCalibtareQ15
    // if we just set the ::permanent we dont need to stop acquisition or apply teh regulation set.
    
    if(info.mode == static_cast<std::uint8_t>(embot::prot::can::analog::polling::StrainRegulationSetMode::temporary))
    {
        embot::prot::can::analog::polling::Message_SET_TXMODE::StrainMode mode;
        pImpl->evalstop(mode);
        
        pImpl->runtimedata.set2use_set(setindex);
        
        // ok: now i must apply the new set to front-end amplifiers
        std::uint8_t set2use = pImpl->runtimedata.set2use_get();
        pImpl->amplifiers_applyregulationsetochipPGA308(set2use, embot::prot::can::analog::polling::StrainChannel::all);
        
        pImpl->evalrestart(mode);
        
        ret = true;
    }
    else if(info.mode == static_cast<std::uint8_t>(embot::prot::can::analog::polling::StrainRegulationSetMode::permanent))
    {
        pImpl->configdata.permanentregulationset_set(setindex);     
        ret = true;
    }
    
        
    return ret;        
}
   

bool embot::app::application::theSTRAIN::get(const embot::prot::can::analog::polling::Message_GET_SERIAL_NO::Info &info, embot::prot::can::analog::polling::Message_GET_SERIAL_NO::ReplyInfo &replyinfo)
{
    // original strain code gets the value from ram, even if the ram and the eeprom are not synched
    std::snprintf(replyinfo.serial, sizeof(replyinfo.serial), "%s", pImpl->configdata.serial_get());  
    return true;        
}
  
bool embot::app::application::theSTRAIN::get(const embot::prot::can::analog::polling::Message_GET_FULL_SCALES::Info &info, embot::prot::can::analog::polling::Message_GET_FULL_SCALES::ReplyInfo &replyinfo)
{
    replyinfo.channel = info.channel;
    replyinfo.regulationset = info.regulationset;
    replyinfo.fullscale = 0;
    
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(replyinfo.regulationset, setindex))
    {
        return false;
    }
        
    bool allchannels = false;
    if(false == pImpl->ischannelvalid(replyinfo.channel, allchannels))
    {   
        return false;
    }  

    
    if(allchannels)
    {   // we cannot operate on all channels
        return false;
    }
    
     
    replyinfo.fullscale = pImpl->configdata.transformer_fullscale_get(setindex, replyinfo.channel);

    return true;        
}
  
bool embot::app::application::theSTRAIN::get(const embot::prot::can::analog::polling::Message_GET_EEPROM_STATUS::Info &info, embot::prot::can::analog::polling::Message_GET_EEPROM_STATUS::ReplyInfo &replyinfo)
{    
    replyinfo.saved = pImpl->configdata.isEEPROMsynched();
    return true;        
}
  
bool embot::app::application::theSTRAIN::get(const embot::prot::can::analog::polling::Message_GET_CH_ADC::Info &info, embot::prot::can::analog::polling::Message_GET_CH_ADC::ReplyInfo &replyinfo)
{
    replyinfo.channel = info.channel;
    replyinfo.valueiscalibrated = info.getcalibrated;  
    
    
    bool allchannels = false;
    if(false == pImpl->ischannelvalid(replyinfo.channel, allchannels))
    {   
        return false;
    }  
    
    if(allchannels)
    {   // we can operate on all channels
        replyinfo.numberofvalues = 6;
        replyinfo.valueindex = 0; 
    }
    else
    {
        replyinfo.numberofvalues = 1;
        replyinfo.valueindex = 0; 
    }
    
    
    // it acquire once. it also restarts if required.
    pImpl->acquisition_oneshot();
        
    std::uint16_t v = 0;
    
    if(true == replyinfo.valueiscalibrated)
    {
        // we call processing() because the calibration is not done inside acquisition_retrieve() 
        pImpl->processing();
        
        if(1 == replyinfo.numberofvalues)
        {
            switch(replyinfo.channel)
            {   
                // force
                case 0:     v = pImpl->runtimedata.data.force.x;   break;
                case 1:     v = pImpl->runtimedata.data.force.y;   break;
                case 2:     v = pImpl->runtimedata.data.force.z;   break;
                // torque
                case 3:     v = pImpl->runtimedata.data.torque.x;    break;
                case 4:     v = pImpl->runtimedata.data.torque.y;    break;
                case 5:     v = pImpl->runtimedata.data.torque.z;    break;
                // impossible ...
                default:    v = 0;                                  break;
            }
            
            //replyinfo.adcvalue = v;
            replyinfo.adcvalues[0] = v;
        }
        else
        {
            replyinfo.adcvalues[0] = pImpl->runtimedata.data.force.x;
            replyinfo.adcvalues[1] = pImpl->runtimedata.data.force.y;
            replyinfo.adcvalues[2] = pImpl->runtimedata.data.force.z;
            replyinfo.adcvalues[3] = pImpl->runtimedata.data.torque.x;
            replyinfo.adcvalues[4] = pImpl->runtimedata.data.torque.y;
            replyinfo.adcvalues[5] = pImpl->runtimedata.data.torque.z;
        }


    }
    else
    {
        if(1 == replyinfo.numberofvalues)
        {
            v = pImpl->runtimedata.data.adcvalue[replyinfo.channel]; 
            //replyinfo.adcvalue = v;
            replyinfo.adcvalues[0] = v;
        }
        else
        {
            for(int i=0; i<6; i++)
            {
                replyinfo.adcvalues[i] = pImpl->runtimedata.data.adcvalue[i];
            }
        }
    }
    
    
    
    
    return true;        
}
  
bool embot::app::application::theSTRAIN::get(const embot::prot::can::analog::polling::Message_GET_CH_DAC::Info &info, embot::prot::can::analog::polling::Message_GET_CH_DAC::ReplyInfo &replyinfo)
{
    replyinfo.regulationset = info.regulationset;
    replyinfo.channel = info.channel;    
    replyinfo.offset = 0; 
        
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(replyinfo.regulationset, setindex))
    {
        return false;
    }
    
    bool allchannels = false;
    if(false == pImpl->ischannelvalid(replyinfo.channel, allchannels))
    {   
        return false;
    }  
    
    if(allchannels)
    {   // we cannot operate on all channels
        return false;
    }

    // it returns the beta   
    embot::prot::can::analog::polling::PGA308cfg1 cfg1;
    pImpl->configdata.amplifiers_get(setindex, replyinfo.channel, cfg1); 
    
    // now i retrieve beta.
    embot::hw::pga308::TransferFunctionConfig tfc;
    tfc.load(cfg1.GD, cfg1.GI, cfg1.S, cfg1.GO, cfg1.Voffsetcoarse, cfg1.Vzerodac);  
    float beta = tfc.beta() * 8;            // move from [0, 8k) into [0, 64k)
    replyinfo.offset = static_cast<std::uint16_t>(std::floor(beta));
    
    return true;    
    
}
  
bool embot::app::application::theSTRAIN::get(const embot::prot::can::analog::polling::Message_GET_MATRIX_RC::Info &info, embot::prot::can::analog::polling::Message_GET_MATRIX_RC::ReplyInfo &replyinfo)
{
    replyinfo.regulationset = info.regulationset;
    replyinfo.row = info.row;
    replyinfo.col = info.col;
    replyinfo.value = 0;
    
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(replyinfo.regulationset, setindex))
    {
        return false;
    }
    
    if((replyinfo.row >=  pImpl->numOfChannels) || (replyinfo.col >=  pImpl->numOfChannels))
    {
        return false;
    }
            
    replyinfo.value = static_cast<std::uint16_t>(pImpl->configdata.transformer_matrix_get(setindex, replyinfo.row, replyinfo.col));
    
    return true; 
}


bool embot::app::application::theSTRAIN::get(const embot::prot::can::analog::polling::Message_GET_MATRIX_G::Info &info, embot::prot::can::analog::polling::Message_GET_MATRIX_G::ReplyInfo &replyinfo)
{
    replyinfo.regulationset = info.regulationset;
    
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(replyinfo.regulationset, setindex))
    {
        return false;
    }   

    replyinfo.gain =  pImpl->configdata.transformer_gain_get(setindex);
        
    return true;   
}


bool embot::app::application::theSTRAIN::get(const embot::prot::can::analog::polling::Message_GET_CALIB_TARE::Info &info, embot::prot::can::analog::polling::Message_GET_CALIB_TARE::ReplyInfo &replyinfo)
{
    replyinfo.regulationset = info.regulationset;
    replyinfo.channel = info.channel;  
    
    replyinfo.value = 0; 
    
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(replyinfo.regulationset, setindex))
    {
        return false;
    } 
    
    bool allchannels = false;
    if(false == pImpl->ischannelvalid(replyinfo.channel, allchannels))
    {   
        return false;
    }  
    
    if(allchannels)
    {   // we cannot operate on all channels
        return false;
    }
    
    replyinfo.value = static_cast<std::uint16_t>(pImpl->configdata.transformer_tare_get(setindex, replyinfo.channel));

    return true;   
}


bool embot::app::application::theSTRAIN::get(const embot::prot::can::analog::polling::Message_GET_CURR_TARE::Info &info, embot::prot::can::analog::polling::Message_GET_CURR_TARE::ReplyInfo &replyinfo)
{
    replyinfo.channel = info.channel;  
    replyinfo.value = 0;
    
    bool allchannels = false;
    if(false == pImpl->ischannelvalid(replyinfo.channel, allchannels))
    {   
        return false;
    }  
    
    if(allchannels)
    {   // we cannot operate on all channels
        return false;
    }
 
    replyinfo.value =  static_cast<std::uint16_t>(pImpl->runtimedata.tare_get(replyinfo.channel));
    
    return true;     
}


bool embot::app::application::theSTRAIN::get(const embot::prot::can::analog::polling::Message_AMPLIFIER_PGA308_CFG1_GET::Info &info, embot::prot::can::analog::polling::Message_AMPLIFIER_PGA308_CFG1_GET::ReplyInfo &replyinfo)
{
    replyinfo.channel = info.channel;
    replyinfo.regulationset = info.regulationset;

    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(replyinfo.regulationset, setindex))
    {
        return false;
    }  

    bool allchannels = false;
    if(false == pImpl->ischannelvalid(replyinfo.channel, allchannels))
    {   
        return false;
    }  
    
    if(allchannels)
    {   // we cannot operate on all channels
        return false;
    }
    
    pImpl->configdata.amplifiers_get(setindex, replyinfo.channel, replyinfo.cfg1); 
    return true;    
}


bool embot::app::application::theSTRAIN::get(const embot::prot::can::analog::polling::Message_AMPLIFIER_GAINOFFSET_GET::Info &info, embot::prot::can::analog::polling::Message_AMPLIFIER_GAINOFFSET_GET::ReplyInfo &replyinfo)
{
    replyinfo.regulationset = info.regulationset;
    replyinfo.channel = info.channel;  
    
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(replyinfo.regulationset, setindex))
    {
        return false;
    }  

    bool allchannels = false;
    if(false == pImpl->ischannelvalid(replyinfo.channel, allchannels))
    {   
        return false;
    }  
    
    if(allchannels)
    {   // we cannot operate on all channels
        return false;
    }
    
    embot::prot::can::analog::polling::PGA308cfg1 cfg1;
    pImpl->configdata.amplifiers_get(setindex, replyinfo.channel, cfg1); 
    
    // now i retrieve alpha and beta
    embot::hw::pga308::TransferFunctionConfig tfc;
    tfc.load(cfg1.GD, cfg1.GI, cfg1.S, cfg1.GO, cfg1.Voffsetcoarse, cfg1.Vzerodac);  
    float alpha = tfc.alpha() * 100.0f;     // represent in 0.01 ticks
    float beta = tfc.beta() * 8;            // move from [0, 8k) into [0, 64k)
    replyinfo.gain = static_cast<std::uint16_t>(std::floor(alpha));
    replyinfo.offset = static_cast<std::uint16_t>(std::floor(beta));
    
    return true;        
    
}


bool embot::app::application::theSTRAIN::get(const embot::prot::can::analog::polling::Message_AMPLIFIER_RANGE_OF_GAIN_GET::Info &info, embot::prot::can::analog::polling::Message_AMPLIFIER_RANGE_OF_GAIN_GET::ReplyInfo &replyinfo)
{
    replyinfo.regulationset = info.regulationset;
    replyinfo.channel = info.channel;  
    
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(replyinfo.regulationset, setindex))
    {
        return false;
    }  
    
    bool allchannels = false;
    if(false == pImpl->ischannelvalid(replyinfo.channel, allchannels))
    {   
        return false;
    }  
    
    if(allchannels)
    {   // we cannot operate on all channels
        return false;
    }
    
    embot::prot::can::analog::polling::PGA308cfg1 cfg1;
    pImpl->configdata.amplifiers_get(setindex, replyinfo.channel, cfg1); 
    
    // now i retrieve limits of alpha.
    embot::hw::pga308::TransferFunctionConfig tfc;
    tfc.load(cfg1.GD, cfg1.GI, cfg1.S, cfg1.GO, cfg1.Voffsetcoarse, cfg1.Vzerodac);  
    float low = tfc.alpha(0) * 100.0f;     
    float high = tfc.alpha(64*1024-1) * 100.f;  
    replyinfo.lowest = static_cast<std::uint16_t>(std::floor(low));
    replyinfo.highest = static_cast<std::uint16_t>(std::floor(high));
    
    return true;        
    
}


bool embot::app::application::theSTRAIN::get(const embot::prot::can::analog::polling::Message_AMPLIFIER_RANGE_OF_OFFSET_GET::Info &info, embot::prot::can::analog::polling::Message_AMPLIFIER_RANGE_OF_OFFSET_GET::ReplyInfo &replyinfo)
{
    replyinfo.regulationset = info.regulationset;
    replyinfo.channel = info.channel; 
    
    uint8_t setindex = 0;
    if(false == pImpl->regulationset2index(replyinfo.regulationset, setindex))
    {
        return false;
    }  

    bool allchannels = false;
    if(false == pImpl->ischannelvalid(replyinfo.channel, allchannels))
    {   
        return false;
    }  
    
    if(allchannels)
    {   // we cannot operate on all channels
        return false;
    }
    
//    embot::prot::can::analog::polling::PGA308cfg1 cfg1;
//    pImpl->configdata.amplifiers_get(setindex, replyinfo.channel, cfg1); 
//     
//    // now i retrieve limits of beta.
//    embot::hw::pga308::TransferFunctionConfig tfc;
//    tfc.load(cfg1.GD, cfg1.GI, cfg1.S, cfg1.GO, cfg1.Voffsetcoarse, cfg1.Vzerodac);  
//    float low = tfc.alpha(0) * 100.0f;     
//    float high = tfc.alpha(64*1024-1) * 100.f;  
    replyinfo.lowest = 0;
    replyinfo.highest = 0xffff;
    
    return true;        
}


bool embot::app::application::theSTRAIN::get(const embot::prot::can::analog::polling::Message_REGULATIONSET_GET::Info &info, embot::prot::can::analog::polling::Message_REGULATIONSET_GET::ReplyInfo &replyinfo)
{    
    replyinfo.mode = info.mode;
    
    bool ret = false;
    
    if(replyinfo.mode == static_cast<std::uint8_t>(embot::prot::can::analog::polling::StrainRegulationSetMode::temporary))
    {   // +1 because allowed values for regulationset are 1, 2, 3 and runtimedata uses 0, 1, 2
        replyinfo.regulationset = pImpl->runtimedata.set2use_get() + 1; 
        ret = true;
    }
    else if(replyinfo.mode == static_cast<std::uint8_t>(embot::prot::can::analog::polling::StrainRegulationSetMode::permanent))
    {   // + 1 because allowed values for regulationset are 1, 2, 3 and configdata uses 0, 1, 2
        replyinfo.regulationset = pImpl->configdata.permanentregulationset_get() + 1;
        ret = true;
    }
        
    return ret;       
}

 
 


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



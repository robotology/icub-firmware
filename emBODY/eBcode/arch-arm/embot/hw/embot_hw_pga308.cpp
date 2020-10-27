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

#include "embot_hw_pga308.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_hw_bsp_config.h"
#include "embot_hw_pga308_bsp.h"

#include <cstring>
#include <vector>
#include "embot_core_binary.h"
#include "embot_hw_sys.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif

using namespace embot::hw;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

#if !defined(EMBOT_ENABLE_hw_pga308)

namespace embot { namespace hw { namespace pga308 {

    bool supported(PGA308 a)                                                                         { return false; }
    bool initialised(PGA308 a)                                                                       { return false; }
    result_t init(PGA308 a, const Config &config)                                                    { return resNOK; }    
    
    result_t set(PGA308 a, const TransferFunctionConfig &tfconfig)                                   { return resNOK; }
    result_t set(PGA308 a, const TransferFunctionConfig::Parameter par, const std::uint16_t value)   { return resNOK; }   
    result_t get(PGA308 a, TransferFunctionConfig &tfconfig)                                         { return resNOK; }
    
    result_t set(PGA308 a, const RegisterAddress address, const std::uint16_t value)                 { return resNOK; }
    result_t get(PGA308 a, const RegisterAddress address, std::uint16_t &value)                      { return resNOK; }

}}} // namespace embot { namespace hw { namespace pga308 {

#else

namespace embot { namespace hw { namespace pga308 {
             
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
           
    bool supported(PGA308 a)
    {
        return embot::hw::pga308::getBSP().supported(a);
    }
    
    bool initialised(PGA308 a)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(a));
    }    

    
    // ZDAC = 0x00, GDAC = 0x01, CFG0 = 0x02, CFG1 = 0x03, CFG2 = 0x04, CHKS = 0x05, SFTC = 0x07
    struct Registers
    {
        SFTCregister    SFTC;
        ZDACregister    ZDAC;
        GDACregister    GDAC;
        CFG0register    CFG0;
        CFG1register    CFG1;
        CFG2register    CFG2;        
    };

    struct PrivateData
    {    
        Config config[static_cast<unsigned int>(PGA308::maxnumberof)];        
        TransferFunctionConfig  transfunctconfig[static_cast<unsigned int>(PGA308::maxnumberof)];        
        Registers registers[static_cast<unsigned int>(PGA308::maxnumberof)];        
        PrivateData() { }
    };
    
    static PrivateData s_privatedata;
    

    result_t setdefault(PGA308 a)
    {        
        if(false == initialised(a))
        {
            return resNOK;
        } 

        std::uint8_t index = embot::core::tointegral(a);       
        
        // ora ... devo trasmettere vari registri
        
        // set the value of registers which will never change
        s_privatedata.registers[index].SFTC.setDefault();
        s_privatedata.registers[index].CFG1.setDefault();
        s_privatedata.registers[index].CFG2.setDefault();  
        // set the registers associated to the transfer function. 
        // at first set them to default value
        s_privatedata.transfunctconfig[index].setDefault();
        // and now get them
        s_privatedata.transfunctconfig[index].obtain(s_privatedata.registers[index].CFG0, s_privatedata.registers[index].ZDAC, s_privatedata.registers[index].GDAC);
        
        // now transmit them all
        set(a, RegisterAddress::SFTC, s_privatedata.registers[index].SFTC.value);        
        set(a, RegisterAddress::ZDAC, s_privatedata.registers[index].ZDAC.value);
        set(a, RegisterAddress::GDAC, s_privatedata.registers[index].GDAC.value);
        set(a, RegisterAddress::CFG0, s_privatedata.registers[index].CFG0.value);
        set(a, RegisterAddress::CFG1, s_privatedata.registers[index].CFG1.value);
        set(a, RegisterAddress::CFG2, s_privatedata.registers[index].CFG2.value);
            
            
        // i should read and verify but until reading is not supported i dont verify
        return resOK;
    }        
   
    result_t init(PGA308 a, const Config &config)
    {
        if(false == supported(a))
        {
            return resNOK;
        }
        
        if(true == initialised(a))
        {
            return resOK;
        }
        
        // init chip
        embot::hw::pga308::getBSP().init(a);     
        
        s_privatedata.config[embot::core::tointegral(a)] = config;
                        
        // init onewire. i dont check vs correct config apart from correct gpio
        if(false == config.onewireconfig.gpio.isvalid())
        {
            return resNOK;
        }
        
        if(false == config.powerongpio.isvalid())
        {
            return resNOK;
        }
        

        
        
        // init onewire ..
        if(resOK != embot::hw::onewire::init(config.onewirechannel, config.onewireconfig))
        {
            return resNOK; 
        }
        
        
        // power on: dont do it if another amplifier has alredy done it.
        if(config.poweronstate != embot::hw::gpio::get(config.powerongpio))
        {
            embot::hw::gpio::set(config.powerongpio, config.poweronstate); 
            embot::hw::sys::delay(25*embot::core::time1millisec);
        }
                
        
        // must set it to initialised in order to use setdefault
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(a));
        
        //embot::core::Time start = embot::os::timeNow();
        // load the default settings of pga308. i use the         
        setdefault(a);
        //static volatile embot::core::Time delta = 0;
        //delta = embot::os::timeNow() - start;
        //delta = delta;
        // some timing ... 
        // - setdefault() takes 24829 usec = ~25ms
        // - set(register value) takes 4106 usec = ~4ms
        return resOK;
    }
    
    
    result_t set(PGA308 a, const TransferFunctionConfig &tfconfig)
    {        
        if(false == initialised(a))
        {
            return resNOK;
        } 

        std::uint8_t index = embot::core::tointegral(a);  
        
        // set the registers related to the transfer function. 
        tfconfig.obtain(s_privatedata.registers[index].CFG0, s_privatedata.registers[index].ZDAC, s_privatedata.registers[index].GDAC);
        
        // now i transmit all registers
        set(a, RegisterAddress::ZDAC, s_privatedata.registers[index].ZDAC.value);
        set(a, RegisterAddress::GDAC, s_privatedata.registers[index].GDAC.value);
        set(a, RegisterAddress::CFG0, s_privatedata.registers[index].CFG0.value);

                
        // dovrei rileggere e verificare se e' ok. poi assegno: 
        s_privatedata.transfunctconfig[embot::core::tointegral(a)] = tfconfig;
        
        return resOK;        
    }
    
    result_t set(PGA308 a, const TransferFunctionConfig::Parameter par, const std::uint16_t value)
    {
        if(false == initialised(a))
        {
            return resNOK;
        } 

        std::uint8_t index = embot::core::tointegral(a);
        
        result_t r = resOK;
        
        RegisterAddress regaddr = RegisterAddress::CFG0;
        std::uint16_t regval = value;

         
        switch(par)
        {
            case TransferFunctionConfig::Parameter::GD:
            {
                regaddr = RegisterAddress::GDAC;
                // carico il valore completo del registro. in questo caso non devo calcolarlo
                s_privatedata.registers[index].GDAC.set(value);
                regval = s_privatedata.registers[index].GDAC.value;
                
            } break;
            
            case TransferFunctionConfig::Parameter::Vzerodac:
            {
                regaddr = RegisterAddress::ZDAC;
                // carico il valore completo del registro. in questo caso non devo calcolarlo
                s_privatedata.registers[index].ZDAC.set(value);
                regval = s_privatedata.registers[index].ZDAC.value;
                
            } break;
            
            case TransferFunctionConfig::Parameter::GI:
            {
                regaddr = RegisterAddress::CFG0;
                // carico il valore completo del registro. in questo caso non devo calcolarlo
                s_privatedata.registers[index].CFG0.setGI(value);
                regval = s_privatedata.registers[index].CFG0.value;
                
            } break;            

            case TransferFunctionConfig::Parameter::muxsign:
            {
                regaddr = RegisterAddress::CFG0;
                // carico il valore completo del registro. in questo caso non devo calcolarlo
                s_privatedata.registers[index].CFG0.setMUX(value);
                regval = s_privatedata.registers[index].CFG0.value;
                
            } break; 

            case TransferFunctionConfig::Parameter::GO:
            {
                regaddr = RegisterAddress::CFG0;
                // carico il valore completo del registro. in questo caso non devo calcolarlo
                s_privatedata.registers[index].CFG0.setGO(value);
                regval = s_privatedata.registers[index].CFG0.value;
                
            } break;             

            case TransferFunctionConfig::Parameter::Vcoarseoffset:
            {
                regaddr = RegisterAddress::CFG0;
                // carico il valore completo del registro. in questo caso non devo calcolarlo
                s_privatedata.registers[index].CFG0.setOS(value);
                regval = s_privatedata.registers[index].CFG0.value;
                
            } break;  
            
            default:
            {
                r = resNOK;
            } break;
            
        }
        
        if(resOK == r)
        {
            set(a, regaddr, regval);
        }

        return r;
    }

            
    result_t set(PGA308 a, const RegisterAddress address, const std::uint16_t value)
    {        
        if(false == initialised(a))
        {
            return resNOK;
        } 

        std::uint8_t index = embot::core::tointegral(a);        
                
        // ora ... devo trasmettere il valore        
        embot::hw::onewire::write(s_privatedata.config[index].onewirechannel, static_cast<std::uint8_t>(address), value);
        
        // now i save the value in registers etc.
        
        result_t r = resOK;
        
        switch(address)
        {
            case RegisterAddress::ZDAC:
            {
                s_privatedata.registers[index].ZDAC.value = value;
                s_privatedata.transfunctconfig[index].load(ZDACregister(value));
            } break;
            case RegisterAddress::GDAC:
            {
                s_privatedata.registers[index].GDAC.value = value;
                s_privatedata.transfunctconfig[index].load(GDACregister(value));
            } break;            
            case RegisterAddress::CFG0:
            {
                s_privatedata.registers[index].CFG0.value = value;
                s_privatedata.transfunctconfig[index].load(CFG0register(value));
            } break;
            case RegisterAddress::CFG1:
            {
                s_privatedata.registers[index].CFG1.value = value;
            } break;            
            case RegisterAddress::CFG2:
            {
                s_privatedata.registers[index].CFG2.value = value;
            } break;
            case RegisterAddress::SFTC:
            {
                s_privatedata.registers[index].SFTC.value = value;
            } break;
            default:
            {
                r = resNOK;
            } break;
            
        }
        
        
        return r;        
    }    
    
    
    result_t get(PGA308 a, const RegisterAddress address, std::uint16_t &value)
    {
        if(false == initialised(a))
        {
            return resNOK;
        } 

        std::uint8_t index = embot::core::tointegral(a);     
        result_t r = resOK;
         
        switch(address)
        {
            case RegisterAddress::ZDAC:
            {
                value = s_privatedata.registers[index].ZDAC.value;
            } break;
            case RegisterAddress::GDAC:
            {
                value = s_privatedata.registers[index].GDAC.value;
            } break;            
            case RegisterAddress::CFG0:
            {
                value = s_privatedata.registers[index].CFG0.value;
            } break;
            case RegisterAddress::CFG1:
            {
                value = s_privatedata.registers[index].CFG1.value;
            } break;            
            case RegisterAddress::CFG2:
            {
                value = s_privatedata.registers[index].CFG2.value;
            } break;
            case RegisterAddress::SFTC:
            {
                value = s_privatedata.registers[index].SFTC.value;
            } break;
            default:
            {
                value = 0;
                r = resNOK;
            } break;
            
        }
        
        return r;        
    }
    

}}} // namespace embot { namespace hw { namespace pga308 {


#endif //defined(EMBOT_PGA308_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


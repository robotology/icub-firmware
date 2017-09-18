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
#include "stm32hal.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>

using namespace std;


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

#define EMBOT_PGA308_ENABLED

#if     !defined(EMBOT_PGA308_ENABLED)



namespace embot { namespace hw { namespace pga308 {

    bool supported(Amplifier a)                                                                         { return false; }
    bool initialised(Amplifier a)                                                                       { return false; }
    result_t init(Amplifier a, const Config &config)                                                    { return resNOK; }
    
    
    result_t result_t set(Amplifier a, const TransferFunctionConfig &tfconfig)                          { return resNOK; }
    result_t set(Amplifier a, const TransferFunctionConfig::Parameter par, const std::uint16_t value)   { return resNOK; }   
    result_t get(Amplifier a, TransferFunctionConfig &tfconfig)                                         { return resNOK; }
    
    result_t set(Amplifier a, const RegisterAddress address, const std::uint16_t value)                 { return resNOK; }
    result_t get(Amplifier a, const RegisterAddress address, std::uint16_t &value)                      { return false; }

}}} // namespace embot { namespace hw { namespace pga308 {

#elif   defined(EMBOT_PGA308_ENABLED)


namespace embot { namespace hw { namespace PGA308 {
        
    struct bspmap_t
    {
        std::uint32_t       mask;
    };
    
    // const support maps
    #if     defined(STM32HAL_BOARD_STRAIN2)    
    
    static const bspmap_t bspmap = 
    {
        0x0000003F  // means... 6 amplifiers
    };
   
    #else
        static const bspmap_t bspmap = 
        {
            0x00000000
        };
    #endif
      
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    std::uint8_t amplifier2index(Amplifier a)
    {   // use it only after verification of supported() ...
        return static_cast<uint8_t>(a);
    }
        
    bool supported(Amplifier a)
    {
        if((Amplifier::none == a) || (Amplifier::maxnumberof == a))
        {
            return false;
        }
        return embot::common::bit::check(bspmap.mask, amplifier2index(a));
    }
    
    bool initialised(Amplifier a)
    {
        if(Amplifier::none == a)
        {
            return false;
        }
        return embot::common::bit::check(initialisedmask, amplifier2index(a));
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

        std::uint32_t none;
    
        Config config[static_cast<unsigned int>(Amplifier::maxnumberof)];
        
        TransferFunctionConfig  transfunctconfig[static_cast<unsigned int>(Amplifier::maxnumberof)];
        
        Registers registers[static_cast<unsigned int>(Amplifier::maxnumberof)];
        
        PrivateData() { none = 0;}
    };
    
    static PrivateData s_privatedata;;
    

    result_t setdefault(Amplifier a)
    {        
        if(false == initialised(a))
        {
            return resNOK;
        } 

        std::uint8_t index = amplifier2index(a);       
        
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
            
            
        // i shoudl read and verify but until reading is not supported i dont verify
        return resOK;
    }        
   
    result_t init(Amplifier a, const Config &config)
    {
        if(false == supported(a))
        {
            return resNOK;
        }
        
        if(true == initialised(a))
        {
            return resOK;
        }
        
        s_privatedata.config[amplifier2index(a)] = config;
                        
        // init onewire. i dont check vs correct config apart from correct gpio
        if(false == config.onewireconfig.gpio.isvalid())
        {
            return resNOK;
        }
        
        // init onewire ..
        embot::hw::onewire::init(config.onewirechannel, config.onewireconfig);
        
        
        // load the default settings of pga308. i use the         
        setdefault(a);
        
        
        embot::common::bit::set(initialisedmask, amplifier2index(a));

        return resOK;
    }
    
    
    result_t set(Amplifier a, const TransferFunctionConfig &tfconfig)
    {        
        if(false == initialised(a))
        {
            return resNOK;
        } 

        std::uint8_t index = amplifier2index(a);        
        
        // set the registers related to the transfer function. 
        tfconfig.obtain(s_privatedata.registers[index].CFG0, s_privatedata.registers[index].ZDAC, s_privatedata.registers[index].GDAC);
        
        // now i transmit all registers
        set(a, RegisterAddress::ZDAC, s_privatedata.registers[index].ZDAC.value);
        set(a, RegisterAddress::GDAC, s_privatedata.registers[index].GDAC.value);
        set(a, RegisterAddress::CFG0, s_privatedata.registers[index].CFG0.value);

                
        // dovrei rileggere e verificare se e' ok. poi assegno: 
        s_privatedata.transfunctconfig[amplifier2index(a)] = tfconfig;
        
        return resOK;        
    }
    
    result_t set(Amplifier a, const TransferFunctionConfig::Parameter par, const std::uint16_t value)
    {
        if(false == initialised(a))
        {
            return resNOK;
        } 

        std::uint8_t index = amplifier2index(a);
        
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

            
    result_t set(Amplifier a, const RegisterAddress address, const std::uint16_t value)
    {        
        if(false == initialised(a))
        {
            return resNOK;
        } 

        std::uint8_t index = amplifier2index(a);        
                
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
    
    
    result_t get(Amplifier a, const RegisterAddress address, std::uint16_t &value)
    {
        if(false == initialised(a))
        {
            return resNOK;
        } 

        std::uint8_t index = amplifier2index(a);     
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



/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_eeprom.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_hw_bsp_config.h"
#include "embot_hw_eeprom_bsp.h"

#include <cstring>
#include <vector>
#include "embot_core_binary.h"
#include "embot_hw_sys.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif



#if defined(EMBOT_ENABLE_hw_eeprom_emulatedMODE)

    #warning CAVEAT EMPTOR: EMBOT_ENABLE_hw_eeprom_emulatedMODE is defined ......
    
        
    constexpr uint8_t fake_sizeofdataee {100};
    constexpr uint8_t fake_dataeedefvalue {14};
    uint8_t fake_dataee[fake_sizeofdataee] = {0};
    
    embot::core::Data fake_eeprom {fake_dataee, fake_sizeofdataee};

#endif

using namespace std;

using namespace embot::hw;


#if !defined(EMBOT_ENABLE_hw_eeprom)

namespace embot { namespace hw { namespace eeprom {

    bool supported(EEPROM ee)                                                                           
    { return false; }
    bool initialised(EEPROM ee)                                                                         
    { return false; }
    result_t init(EEPROM ee, const Config &config)                                                      
    { return resNOK; }
    const Config & config(EEPROM ee)                                                                    
    { static Config cfg {}; return cfg; }        
    result_t read(EEPROM ee, ADR adr, embot::core::Data &destination, embot::core::relTime timeout)
    { return resNOK; }
    result_t write(EEPROM ee, ADR adr, const embot::core::Data &content, embot::core::relTime timeout)
    { return resNOK; }    

}}} // namespace embot { namespace hw { namespace eeprom {

#else

namespace embot { namespace hw { namespace eeprom {
    

/* Bit values in EEPROM status register */
#define EE_SR_SRWD  (0x80)
#define EE_SR_BP1   (0x08)
#define EE_SR_BP0   (0x04)
#define EE_SR_WEL   (0x02)
#define EE_SR_WIP   (0x01)

/* Values for BP1 and BP0 bits in status register */
#define EE_SR_BP_MASK       (EE_SR_BP1|EE_SR_BP0)
#define EE_SR_BP_NONE       (0|0)
#define EE_SR_BP_TOPQUARTER (0|EE_SR_BP0)
#define EE_SR_BP_TOPHALF    (EE_SR_BP1|0)
#define EE_SR_BP_ALL        (EE_SR_BP1|EE_SR_BP0)
    
    // definition of chip M95512DF
    // dont use pimpl. for now
    struct M95512DF
    {
        static constexpr size_t pagelength {128}; 
        static constexpr size_t storagesize {64*1024};
        static constexpr embot::core::relTime deftimeout {100*embot::core::time1millisec}; 
        static constexpr embot::core::relTime writetimeout {5*embot::core::time1millisec};
        enum class CMD : uint8_t {
            WREN = 0x06, WRDI = 0x04, RDSR = 0x05, WRSR = 0x01, READ = 0x03, WRITE = 0x02, RDID = 0x83, WRID = 0x82, RDLS = 0x83
        };
        using ADR = uint16_t;
        
        struct Config
        {            
            embot::hw::SPI spi {embot::hw::SPI::six};
            embot::hw::GPIO nWP {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::twelve};
            embot::hw::GPIO nHOLD {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::thirteen};  
            embot::hw::GPIO nSEL {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::eight}; 
            constexpr Config() = default;
            constexpr Config(embot::hw::SPI s, const embot::hw::GPIO& nwp, 
                             const embot::hw::GPIO& nhold, const embot::hw::GPIO& nsel) : spi(s), nWP(nwp), nHOLD(nhold), nSEL(nsel) {}            
        };        
               
        Config config {};
            
        M95512DF() = default;
        
        
        bool init(const Config &cfg)
        {
            bool r {false};
            
            config = cfg;
            
            holdcontrol(false);
            writecontrol(false);
            if(resOK == embot::hw::spi::init(config.spi, {}))
            {
                r = true;
            }
            
            return r; 
        }
        

        bool write(ADR adr, const embot::core::Data &data)
        {
            //bool r {false};                       
            
            if((false == data.isvalid()) || ((adr + data.capacity) > storagesize))
            {   // no need to check if(adr >= storagesize)
                return false;
            }
            constexpr size_t pagemask {pagelength-1};
            embot::core::Data remainingdata {data.pointer, data.capacity};
            size_t delta {pagelength - (adr & pagemask)};
            for(; remainingdata.capacity; )
            {
                delta = (delta > remainingdata.capacity) ? remainingdata.capacity : delta;
                
                // enable write and write to the status register
                if(true == cmd(CMD::WREN))
                {
                    embot::core::Data data2writeinpage {remainingdata.pointer, delta};
                    if(true == writeinpage(adr, data2writeinpage))
                    {
                        // now we read the status until it is ok, but no longer than 5 ms
                        embot::core::Time expirytime {embot::core::now() + writetimeout};
                        uint8_t status {0};
                        while((true == cmdrx(CMD::RDSR, status)) && (0 != (status & EE_SR_WIP)))
                        {
                            if(embot::core::now() > expirytime)
                            {
                                // timeout ....
                                return false;
                            }
                        }                        
                    }
                    // update values
                    remainingdata.capacity -= delta;
                    adr += delta;
                    remainingdata.pointer = reinterpret_cast<uint8_t*>(remainingdata.pointer) + delta;
                    delta = pagelength;                
                }                                               
            }            
            return true;
        }   


        bool read(ADR adr, embot::core::Data &data)
        {   
            if((false == data.isvalid()) || ((adr + data.capacity) > storagesize))
            {   // no need to check if(adr >= storagesize)
                return false;
            }
            
            bool r {false};
            adr = adjustaddress(adr);
            //embot::hw::gpio::set(nSEL, embot::hw::gpio::State::RESET);
            select(true);
            uint8_t cc {embot::core::tointegral(CMD::READ)};            
            if(true == send({&cc, 1}))
            {
                if(true == send({&adr, 2}))
                {
                    r = recv(data);                    
                }
            }
            //embot::hw::gpio::set(nSEL, embot::hw::gpio::State::SET);
            select(false);
            return r;
        } 

    private:
        
        void select(bool enable)
        {
            embot::hw::gpio::set(config.nSEL, enable ? embot::hw::gpio::State::RESET : embot::hw::gpio::State::SET);
        }
        
        void holdcontrol(bool enable)
        {
            embot::hw::gpio::set(config.nHOLD, enable ? embot::hw::gpio::State::RESET : embot::hw::gpio::State::SET);
        }

        void writecontrol(bool enable)
        {
            embot::hw::gpio::set(config.nWP, enable ? embot::hw::gpio::State::SET : embot::hw::gpio::State::RESET);
        }    
        
        bool send(const embot::core::Data &data)
        {
            return (resOK == embot::hw::spi::write(config.spi, data, deftimeout)) ? true : false;            
        }

        bool recv(embot::core::Data &dd)
        {
            return (resOK == embot::hw::spi::read(config.spi, dd, deftimeout)) ? true : false;            
        }
        
        bool cmd(CMD c)
        {
            bool r {false};
            //embot::hw::gpio::set(nSEL, embot::hw::gpio::State::RESET);
            select(true);
            uint8_t cc {embot::core::tointegral(c)};
            r = send({&cc, 1});
            //embot::hw::gpio::set(nSEL, embot::hw::gpio::State::SET);
            select(false);
            return r;
        } 

        bool cmdtx(CMD c, uint8_t v)
        {
            bool r {false};
            //embot::hw::gpio::set(nSEL, embot::hw::gpio::State::RESET);
            select(true);
            uint8_t cc {embot::core::tointegral(c)};
            if(true == send({&cc, 1}))            
            {
                uint8_t vv {v};
                r = send({&vv, 1});
            }            
            //embot::hw::gpio::set(nSEL, embot::hw::gpio::State::SET);
            select(false);
            return r;                        
        } 
        
        bool cmdrx(CMD c, uint8_t &v)
        {
            bool r {false};
            //embot::hw::gpio::set(nSEL, embot::hw::gpio::State::RESET);
            select(true);
            uint8_t cc {embot::core::tointegral(c)};
            if(true == send({&cc, 1}))            
            {
                embot::core::Data data {&v, 1};
                r = recv(data);
            }            
            //embot::hw::gpio::set(nSEL, embot::hw::gpio::State::SET);
            select(false);
            return r;                        
        }
        
        ADR adjustaddress(ADR adr)
        {
            return __REV16(adr);
        }

        bool writeinpage(ADR adr, const embot::core::Data &data)
        {
            bool r {false};
            adr = adjustaddress(adr);
            //embot::hw::gpio::set(nSEL, embot::hw::gpio::State::RESET);
            select(true);
            uint8_t cc {embot::core::tointegral(CMD::WRITE)};
            if(true == send({&cc, 1}))
            {
                if(true == send({&adr, 2}))
                {
                    r = send(data);                    
                }
            }
            //embot::hw::gpio::set(nSEL, embot::hw::gpio::State::SET);
            select(false);
            return r;
        } 
        
    };
    
    
       
          
    // initialised mask        
    static std::uint32_t initialisedmask = 0;
    
    struct PrivateData
    {        
        Config config[embot::core::tointegral(EEPROM::maxnumberof)] = {};  
        embot::hw::eeprom::Type type[embot::core::tointegral(EEPROM::maxnumberof)] = {embot::hw::eeprom::Type::none};
        PrivateData() = default;
    };
    
    static PrivateData s_privatedata {};    
    
    M95512DF *spieeprom {nullptr};
    embot::hw::chip::M95512DF *chipM95512DF {nullptr};
    
    bool supported(EEPROM ee)
    {
        return embot::hw::eeprom::getBSP().supported(ee);
    }
    
    bool initialised(EEPROM ee)
    {
        return embot::core::binary::bit::check(initialisedmask,embot::core::tointegral(ee));
    }
        
    result_t init(EEPROM ee, const Config &cfg)
    {
        if(!supported(ee))
        {
            return resNOK;
        }  
        
        if(initialised(ee))
        {   // dont need to re-init
            return resOK;
        }
                 
        if(!embot::hw::initialised())
        {   // requires embot::hw::bsp::init()
            return resNOK;
        }
        
        const embot::hw::eeprom::BSP &eeprombsp = embot::hw::eeprom::getBSP();
        
        embot::hw::eeprom::Type type = eeprombsp.getPROP(ee)->type;
        
        if((type != embot::hw::eeprom::Type::spiM95512DF) && (type != embot::hw::eeprom::Type::chipM95512DF))
        {
            return resNOK;
        }
        
        s_privatedata.config[embot::core::tointegral(ee)] = cfg;
        s_privatedata.type[embot::core::tointegral(ee)] = type;
                       
        // bsp specific initialization                    
        eeprombsp.init(ee);  

        // but ... initialization in here
        
        if(embot::hw::eeprom::Type::spiM95512DF == type)
        {                         
            spieeprom = new M95512DF;
            spieeprom->init({eeprombsp.getPROP(ee)->multi.spi.bus, eeprombsp.getPROP(ee)->multi.spi.wp, eeprombsp.getPROP(ee)->multi.spi.hold, eeprombsp.getPROP(ee)->multi.spi.sel});
        }
        else if(embot::hw::eeprom::Type::chipM95512DF == type)
        {
            chipM95512DF = new embot::hw::chip::M95512DF;
            chipM95512DF->init(eeprombsp.getPROP(ee)->multi.chipM95512DFcfg);
        }

                
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(ee));
                
        return resOK;        
    }


    const Config & config(EEPROM ee)
    {
        return s_privatedata.config[embot::core::tointegral(ee)];
    }
    

    result_t read(EEPROM ee, ADR adr, embot::core::Data &destination, embot::core::relTime timeout)
    {
        if(!initialised(ee))
        {
            return resNOK;
        } 
        
        // fake...
//#if defined(EMBOT_ENABLE_hw_eeprom_emulatedMODE)        
//        destination.copyfrom(fake_eeprom);
//#else
//    

//#endif        
        
        if(embot::hw::eeprom::Type::spiM95512DF == s_privatedata.type[embot::core::tointegral(ee)])
        {
            spieeprom->read(adr, destination);
        }
        else if(embot::hw::eeprom::Type::chipM95512DF == s_privatedata.type[embot::core::tointegral(ee)])
        {
            chipM95512DF->read(adr, destination);
        }

        return resOK;        
        
//        const embot::hw::GPIO gpio = embot::hw::eeprom::getBSP().getPROP(ee)->gpio;       
//        
//        embot::hw::gpio::State s = embot::hw::gpio::get(gpio);
//        return (embot::hw::eeprom::getBSP().getPROP(ee)->pressed == s) ? (true) : (false);            
        
    }
    

    result_t write(EEPROM ee, ADR adr, const embot::core::Data &content, embot::core::relTime timeout)
    {
        
        if(!initialised(ee))
        {
            return resNOK;
        } 
        
        // fake...
//#if defined(EMBOT_ENABLE_hw_eeprom_emulatedMODE)        
//        fake_eeprom.copyfrom(content);
//#else
//    

//#endif        
   
        if(embot::hw::eeprom::Type::spiM95512DF == s_privatedata.type[embot::core::tointegral(ee)])
        {
            spieeprom->write(adr, content);
        }        
        else if(embot::hw::eeprom::Type::chipM95512DF == s_privatedata.type[embot::core::tointegral(ee)])
        {
            chipM95512DF->write(adr, content);
        }
        
        return resOK;          
    }



    
}}} // namespace embot { namespace hw { namespace eeprom 

#endif // #if !defined(EMBOT_ENABLE_hw_eeprom)    


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


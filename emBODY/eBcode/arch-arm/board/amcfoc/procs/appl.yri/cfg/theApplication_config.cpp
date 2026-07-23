

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



#include "theApplication_config.h"


#include "embot_app_eth_theBackdoor.h"

#include "embot_app_eth_theErrorManager.h"
#include "embot_os_theScheduler.h"
#include "embot_hw_sys.h"
#include "embot_hw_analog_bsp.h"

namespace embot { namespace app { namespace eth {
    
    // user defined onidle
    void theApplication_Config_onIdle(embot::os::Thread *t, void* idleparam) 
    { 
        static volatile uint32_t cnt {0}; 
        cnt++; 
    }

    
    
    void emit_over_backdoor(theErrorManager::Severity sev, const theErrorManager::Caller &caller, const theErrorManager::Descriptor &des, const std::string &str);
    
    void addservice();
    
    // user defined worker called by INIT thread just before start of scheduling
    void theApplication_Config_inituserdefined(void *p)
    {
        // add what yout want
        volatile uint32_t ciao {0};
        ciao++;
        
        
        embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};            
        embot::app::eth::theErrorManager::getInstance().trace("calling embot::app::eth::addservice()", {"theApplication_Config_inituserdefined()", thr});         
        
        
        embot::app::eth::addservice();
        
    }
    
    void theApplication_Config_errorman_onemit(theErrorManager::Severity sev, const theErrorManager::Caller &caller, const theErrorManager::Descriptor &des, const std::string &str)
    {
        std::string timenow = embot::core::TimeFormatter(embot::core::now()).to_string();
        std::string eobjstr = (true == caller.isvalid()) ? caller.objectname : "OBJ";
        std::string threadname = (true == caller.isvalid()) ? caller.owner->getName() : "THR";
        std::string severity = theErrorManager::to_cstring(sev);
        
        embot::core::print(std::string("[") + severity + "] @" + timenow + " (" + eobjstr + ", " + threadname + "): " + str);
        
        if(theErrorManager::Severity::trace == sev) 
        {
            return;
        } 

        // you may in here send the diagnostics message
        if(true == des.isvalid())
        {
            
        }
      
        if(theErrorManager::Severity::fatal == sev)
        {
            for(;;);
        }        
        
    }

    static constexpr embot::app::icc::Signature signature =
    {
        embot::app::boards::Board::amcfoc,
        {embot::app::msg::BUS::icc1, 0},
        {theApplication_Config.property.version.major, theApplication_Config.property.version.minor, 0, 0}, 
        {2, 0},         // protocol version
        theApplication_Config.property.date
    };
    
    
    
    bool theApplication_Config_onICCROPrx(const embot::app::eth::icc::ItemROP &rxrop, embot::app::eth::icc::ItemROP &reply)
    {
        bool r {false};

        // it is mandatory that the memory of the variables stays alive after defOnROPrx() returns so in here we mark it as static 
        // to move it away from being a temporary variable allocated on the stack
        // a proper implementation however requires to have these variables somehow global.
        static embot::core::Time timeoflife {0};
        static uint64_t uid {0};
        static uint32_t dummyUINT32 {0};
        static embot::app::icc::Signature sign {signature};

        // we fill the default w/ nothing to transmit back 
        reply.fill(embot::app::eth::icc::ItemROP::CMD::none, rxrop.var);
        
        switch(rxrop.cmd)
        {               
            case embot::app::eth::icc::ItemROP::CMD::ping:
            {
                reply.fill(embot::app::eth::icc::ItemROP::CMD::ack, rxrop.var);
            } break;
            
            case embot::app::eth::icc::ItemROP::CMD::ask:
            {
                switch(rxrop.var.descriptor.id)
                {
                    case embot::app::eth::icc::ItemROP::IDtimeoflife:
                    {
                        timeoflife = embot::core::now();
                        reply.fill(embot::app::eth::icc::ItemROP::CMD::say, {{embot::app::eth::icc::ItemROP::IDtimeoflife, 8}, &timeoflife});
                    } break;
                    
                    case embot::app::eth::icc::ItemROP::IDdummyUINT32:
                    {
                        reply.fill(embot::app::eth::icc::ItemROP::CMD::say, {{embot::app::eth::icc::ItemROP::IDdummyUINT32, 4}, &dummyUINT32});                            
                    } break;

                    case embot::app::eth::icc::ItemROP::IDunique64:
                    {
                        uid = embot::hw::sys::uniqueid();
                        reply.fill(embot::app::eth::icc::ItemROP::CMD::say, {{embot::app::eth::icc::ItemROP::IDunique64, 8}, &uid});                            
                    } break; 
                    
                    // ... other managed cases

                    case embot::app::eth::icc::ItemROP::IDsignature:
                    {
                        reply.fill(embot::app::eth::icc::ItemROP::CMD::say, {{embot::app::eth::icc::ItemROP::IDsignature, 16}, &sign});                            
                    } break;                       

#if defined(STM32HAL_CORE_CM7)                     
                    case embot::app::eth::icc::ItemROP::IDadcfactorycalibration:
                    {
                        static embot::hw::adc::bsp::FactoryCalibration fc {};
                           
                        fc.vrefint_cal = *VREFINT_CAL_ADDR;
                        fc.vrefint_cal_vref = VREFINT_CAL_VREF;
                        fc.ts_cal1 = *TEMPSENSOR_CAL1_ADDR;
                        fc.ts_cal2 = *TEMPSENSOR_CAL2_ADDR;
                        fc.ts_cal1_temp = TEMPSENSOR_CAL1_TEMP;
                        fc.ts_cal2_temp = TEMPSENSOR_CAL2_TEMP;
                        fc.ts_cal_vref  = TEMPSENSOR_CAL_VREFANALOG; 
                          
                        reply.fill(embot::app::eth::icc::ItemROP::CMD::say, {{embot::app::eth::icc::ItemROP::IDadcfactorycalibration, sizeof(fc)}, &fc});                            
                    } break;                    
#endif    
                    
                    default: 
                    {   // if the ID is not managed we do nothing but we send a nak back   
                        reply.fill(embot::app::eth::icc::ItemROP::CMD::nak, rxrop.var);
                    } break;                     
                }                    
            } break;
        
            case embot::app::eth::icc::ItemROP::CMD::set:
            {
                switch(rxrop.var.descriptor.id)
                {                        
                    case embot::app::eth::icc::ItemROP::IDdummyUINT32:
                    {
                        dummyUINT32 = *reinterpret_cast<uint32_t*>(rxrop.var.memory);
                        reply.fill(embot::app::eth::icc::ItemROP::CMD::ack, {{embot::app::eth::icc::ItemROP::IDdummyUINT32, 4}, &dummyUINT32});                                                      
                    } break;
                    
                    // ... other managed cases
                    
                    default: 
                    {   // if the ID is not managed we do nothing but we send a nak back   
                        reply.fill(embot::app::eth::icc::ItemROP::CMD::nak, rxrop.var);
                    } break;                          
                }
                
            } break;                

            case embot::app::eth::icc::ItemROP::CMD::sig:
            {
                switch(rxrop.var.descriptor.id)
                {                        
                    case embot::app::eth::icc::ItemROP::IDdummyUINT32:
                    {
                        dummyUINT32 = *reinterpret_cast<uint32_t*>(rxrop.var.memory);
                        // i dont send any ack back                                                      
                    } break;
                    
                    // ... other managed cases
                    
                    default: 
                    {   // if the ID is not managed we do nothing and we send nothing back   
                        reply.fill(embot::app::eth::icc::ItemROP::CMD::none, rxrop.var);
                    } break;                        
                }
                
            } break;  
            
            default: { } break; // we dont manage any other received commands
            
        }

        r = (embot::app::eth::icc::ItemROP::CMD::none != rxrop.cmd);        
        
        return r;        
    }    
            
}}}


#include "embot_app_eth_theHandler.h"

namespace embot { namespace app { namespace eth {
    


void addservice()
{ 
    embot::app::eth::theHandler::getInstance().initialise({}); 
}


}}} // namespace embot { namespace app { namespace eth {

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEERRORMANAGER_H_
#define __EMBOT_APP_ETH_THEERRORMANAGER_H_

#include "embot_core.h"
#include "embot_core_binary.h"
#include "embot_app_eth.h"
#include "embot_os.h"

#include <vector>
#include <memory>


namespace embot { namespace app { namespace eth {
      
#if 0
    The singleton `theErrorManager` is responsible to ...
    
#endif
    
        
    class theErrorManager
    {
    public:
        static theErrorManager& getInstance();
                
    public:
        
    
        enum class Severity : uint8_t
        {
            trace = 0, 
            info = 1,     
            debug = 2,
            warning = 3,
            error = 4,
            fatal = 5
        };
        
        constexpr static const char* to_cstring(Severity s)
        {
            constexpr const char * names[] = {"trace", "info", "debug", "warning", "error", "fatal"};
            return names[embot::core::tointegral(s)];
        }

        struct Descriptor
        {
            uint32_t code {0};          /* the system uses eOerrmanCode_t, other modules may use values defined in a particular table ... see eoerror_code_get() */
            uint8_t sourcedevice {0};   /* use values in eOerrmanSourceDevice_t */
            uint8_t sourceaddress {0};  /* the system uses 0, other modules may use other values ...  e.g., CAN addresses */   
            uint16_t par16 {0};         /* it must be 0 unless the code needs one particular param */
            uint64_t par64 {0};         /* it must be 0 unless the code needs one particular param */
            constexpr Descriptor() = default;
            constexpr Descriptor(uint32_t c, uint16_t p16, uint64_t p64) : code(c), sourcedevice(0), sourceaddress(0), par16(p16), par64(p64) {}
            constexpr bool isvalid() const { return 0 != code; }
        };  

        struct Caller
        {
            const char* objectname {nullptr}; 
            embot::os::Thread *owner {nullptr}; 
            constexpr Caller() = default;
            constexpr Caller(const char* n, embot::os::Thread* t) : objectname(n), owner(t) {} 
            constexpr bool isvalid() const { return (nullptr != objectname) && (nullptr != owner); }
            std::string to_string() const
            {
                const char *obj = (objectname != nullptr) ? objectname : "o?";
                const char *thr = (nullptr != owner) ? owner->getName() : "t?";
                return std::string(obj) + ", " + std::string(thr);                
            }
        };        

        using fpOnEmit = void (*)(Severity sev, const Caller &caller, const Descriptor &des, const std::string &str);
        
        struct Config
        {
            fpOnEmit onemit {nullptr};
            constexpr Config() = default;
            constexpr Config(const fpOnEmit oe) : onemit(oe) {}
            constexpr bool isvalid() const { return nullptr != onemit; }
        }; 
                
        bool initialise(const Config &config);        
        bool trace(const std::string &str, const Caller &caller);
        bool emit(Severity sev, const Caller &caller, const Descriptor &des, const std::string &str);
        bool set(fpOnEmit onemit);
                     
    private:
        theErrorManager(); 
        ~theErrorManager(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    }; 
    
    constexpr theErrorManager::Severity sevTRACE { theErrorManager::Severity::trace };
    constexpr theErrorManager::Severity sevINFO { theErrorManager::Severity::info };
    constexpr theErrorManager::Severity sevDEBUG { theErrorManager::Severity::debug };
    constexpr theErrorManager::Severity sevWARNING { theErrorManager::Severity::warning };
    constexpr theErrorManager::Severity sevERROR { theErrorManager::Severity::error };
    constexpr theErrorManager::Severity sevFATAL { theErrorManager::Severity::fatal };

    bool emit(theErrorManager::Severity sev, const theErrorManager::Caller &caller, const theErrorManager::Descriptor &des, const std::string &str);
    
}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_ICC_PRINTER_H_
#define __EMBOT_HW_ICC_PRINTER_H_

#include "embot_core.h"
#include "embot_hw_icc_ltr.h"


namespace embot::hw::icc::printer {
    
    class theClient
    {
    public:       
        static theClient& getInstance();
     
        struct Config
        {            
            embot::hw::icc::LTR l {embot::hw::icc::LTR::one};
            constexpr Config() = default;
            constexpr Config(embot::hw::icc::LTR ll) : l(ll) {}
        };                             

        static constexpr size_t MAXcapacity = 256;   
                
        bool initialise(const Config &config);   
        bool post(const std::string &str, embot::core::relTime tout = 100*embot::core::time1millisec);        
        
   public:     
        theClient(const theClient&) = delete;               // non copyable
        theClient& operator=(const theClient&) = delete;    // non copyable
        theClient(theClient&&) = delete;                    // non moveable
        theClient& operator=(theClient&&) = delete;         // non moveable    
        
    private:
        theClient(); 
    
    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;    
    }; 

  
    class theServer
    {
    public:
        
        static theServer& getInstance();
     
        struct Config
        {            
            embot::hw::icc::LTR l {embot::hw::icc::LTR::one};
            constexpr Config() = default;
            constexpr Config(embot::hw::icc::LTR ll) : l(ll) {}
        };                             

        static constexpr size_t MAXcapacity = 256;   
                
        bool initialise(const Config &config);        
        
    private:
        theServer(); 
        ~theServer(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;    
    };
    
    
} // namespace embot::hw::icc::printer {


namespace embot::hw::icc::printer::test {

    void init(DIR dir);
    // only the client must call tick()
    void tick(const std::string &str);
}    




#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BLDC_THEAPPLICATION_H_
#define __EMBOT_APP_BLDC_THEAPPLICATION_H_


#include <memory>
#include <vector>
#include "embot_os_common.h"
#include "embot_os_thread.h"
#include "embot_hw.h"

#include "embot_app_application_CANagentCORE.h"

namespace embot { namespace app { namespace bldc {
           
    class theApplication
    {
    public:
        
        static theApplication& getInstance();
 
        using getCodePartition = embot::hw::FLASHpartitionID (*)();
        using getAgentCore = embot::app::application::CANagentCORE* (*)();
        using OnStartupCTRL = void (*)(embot::prot::can::Address adr);    
        using OnTickCTRL = void (*)(const std::vector<embot::prot::can::Frame> &input, std::vector<embot::prot::can::Frame> &output);

        struct CoreCfg
        {
            getCodePartition getcodepartition {nullptr};               
            getAgentCore getagentcore {nullptr};               

            constexpr CoreCfg() = default;
            
            constexpr CoreCfg(getCodePartition c, getAgentCore getag) : getcodepartition(c), getagentcore(getag) {} 
                    
            constexpr bool isvalid() const {
                bool notok {(nullptr == getagentcore) || (nullptr == getcodepartition)};
                return !notok;
            }            
        };
        
        struct SystCfg
        {   
            // in cfgInit it will be used only the stack size. the other fields will not be considered
            // if cfgIdle is not specified, then an internal one is used
            // if onOSerror is not specified then an internal one is used         
            embot::core::relTime ostick {1000*embot::core::time1microsec};
            embot::os::InitThread::Config cfgInit {6*1024, nullptr, nullptr};
            embot::os::IdleThread::Config cfgIdle {4*1024, nullptr, nullptr, nullptr};         
            embot::core::Callback onOSerror {};
                
            constexpr SystCfg() = default;
            
            constexpr SystCfg(embot::core::relTime t, const embot::os::InitThread::Config &cin, const embot::os::IdleThread::Config &cid)
                : ostick(t), cfgInit(cin), cfgIdle(cid) {} 
                    
            constexpr bool isvalid() const {
                bool notok = (ostick < 500) || (cfgInit.stacksize < 1024);
                return !notok;
            }                    
        };  

        
        struct CommCfg
        {
            static const uint16_t defstacksize {4*1024};
            uint16_t stacksize {defstacksize};
            uint8_t fifoRXcapacity {32}; 
            uint8_t fifoTXcapacity {32}; 
            
            constexpr CommCfg() = default;
            
            constexpr CommCfg(uint16_t s, uint8_t r, uint8_t t)
                : stacksize(s), fifoRXcapacity(r), fifoTXcapacity(t) {} 
                    
            constexpr bool isvalid() const {
                bool notok = (stacksize < 1024) || (fifoRXcapacity < 4) || (fifoTXcapacity < 4);
                return !notok;
            }                    
        };  

        struct CtrlCfg
        {
            static const uint16_t defstacksize {6*1024};
            OnStartupCTRL ctrlStartup {nullptr};
            OnTickCTRL ctrlTick {nullptr};  
            uint16_t stacksize {defstacksize};
                
            constexpr CtrlCfg() = default;
            
            constexpr CtrlCfg(OnStartupCTRL ctrls, OnTickCTRL ctrlt, uint16_t s = defstacksize)
                : ctrlStartup(ctrls), ctrlTick(ctrlt), stacksize(s) {} 
                    
             constexpr bool isvalid() const {
                bool notok = (stacksize < 1024);
                return !notok;
             }                    
        }; 
        
        
        struct Config
        {   
            CoreCfg core {};
            SystCfg syst {};
            CommCfg comm {};    
            CtrlCfg ctrl {};
            
            uint32_t tt {0};
            
            constexpr Config() = default;
            
            constexpr Config(const CoreCfg& cc, const SystCfg& sc, const CommCfg& co, const CtrlCfg& ct)
                : core(cc), syst(sc), comm(co), ctrl(ct)
                {}
            
            constexpr bool isvalid() const {
                bool ok = core.isvalid() && syst.isvalid() && comm.isvalid() && ctrl.isvalid();
                return ok;
            }
        
        };            
                  
                
        [[noreturn]] void start(const Config &config);        

        
    private:
        theApplication(); 
        ~theApplication(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;    
    };       
    
}}} // namespace embot { namespace app { namespace bldc


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

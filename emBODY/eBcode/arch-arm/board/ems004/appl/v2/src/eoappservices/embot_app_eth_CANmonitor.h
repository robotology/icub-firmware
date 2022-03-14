

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_CANMONITOR_H_
#define __EMBOT_APP_ETH_CANMONITOR_H_

#include "embot_core.h"
#include "embot_core_binary.h"

#include "EOtheServices.h"
#include "EoCommon.h"

#include <memory>

namespace embot { namespace app { namespace eth {

#if 0
    
### Description of object `CANmonitor`

The `CANmonitor` object can be used by any service to monitor the presence of the CAN boards it requires.
So far it is based on regular checks of the received CAN loactions during a particular time interval.
The CAN addresses of the frames received and processed by the service are used to touch the object,
which contains a list of CAN locations it must monitor. If any of these CAN locations did not touch
the object in the timeframe configured with `Config::rateofcheck`, then `CANmonitor` changes its internal
state and emits the relevant diagnostic messages. The messages are emitted with a policy defined at configuration
time by `Config::reportmode` and `Config::rateofregularreport`.

More details of its behaviour are in the following figures and table.    
    
```
              --- every 1 ms --------------------------------
             |                                               |
             v                                               | 
        -----------             -----------             -----------  
       | Thread RX | --------> | Thread DO | --------> | Thread TX |
        -----------             -----------             -----------
             |                       |                       .
  Service.AcceptCANframe()           |                       .
             |                       |                       .
             | touch(loc)            |                       .
             v                       |                       .
     --------------------      Service.Tick()                .
    |     CANmonitor     |           |                       .
    | ------------------ | tick()    |                       .
    |  eval State and    | <---------                        .
    |  touched CAN       |                                   .
    |  locations, send   |                                   .
    |  diagnostics if    |. . . [diagnostics message] . . . .
    |  needed            |
     --------------------   
```  

**Figure 1**. How an object `CANmonitor` works. If the service receives
a relevant CAN frame it calls `CANmonitor::touch()` with its CAN location.
The service regularly calls `CANmonitor::tick()` and every `Config::rateofcheck`
it evaluates its `State` and the touched CAN locations. That makes the `State` evolve 
as in following figure 2 and may produce diagnostics messages as described by next table 1. 



```
   --  
  |  |  
  |  v  
[[ OK ]] --> [ justFOUND ] <-------
   |            ^   |              |
   |            |   v          	   |
    -------> [ justLOST ] --> [[ stillLOST ]]
                                   |  ^
                                   |  |
                                    -- 
```
**Figure 2**. Possible transitions for `CANmonitor::State`. If all boards transmit we stay inside `OK`. Else,
if some is missing we go through `justLOST` and then we end up in `stillLOST` until they all reappear.
When all boards reappear we go through `justFOUND` and we end up in `OK` again. Transitions between `justFOUND` 
and `justLOST` states may happen if some boards keep on disappearing and reappearing.   


| State       | emits                                 | when                                                         |
| ----------- | ------------------------------------- | ------------------------------------------------------------ |
| `OK`        | `canservices_boards_regularcontact`   | Every `Config::rateofregularreport` if configured with `Report::ALL` |
| `justLOST`  | `canservices_boards_lostcontact`      | At check time if previous state was `OK` and if configured with `Report::ALL` or `Report::justLOSTjustFOUND` or `Report::justLOSTjustFOUNDstillLOST` |
| `stillLOST` | `canservices_boards_stillnocontact`   | Every `Config::rateofregularreport` if configured with `Report::ALL` or `Report::justLOSTjustFOUNDstillLOST` |
| `justFOUND` | `canservices_boards_retrievedcontact` | At check time if previous state was `justLOST`  or `stiilLOST` and if configured with `Report::ALL` or `Report::justLOSTjustFOUND` or `Report::justLOSTjustFOUNDstillLOST` |

**Table 1**. It contains the rules for emissione of diagnostics messages.

#endif    
    
    class CANmonitor
    {
    public:
        
        enum class Mode : uint8_t { 
            Listening       // the only mode is by listening reception of frames from a given eObrd_canlocation_t
        }; 
        
        enum class State : uint8_t { 
            OK,             // state at previous check was OK and all boards in Config::target touched since previous check   
            
            justLOST,       // state of previous check was OK (or justFOUND) and one or more boards did not touch since previous check. 
                            // we stay in this state for one check only, then we go either in stillLOST or justFOUND
            
            stillLOST,      // state of previous check was either justLOST or stillLOST.
                            // we stay in this state until all the boards touch. in this case we go to justFOUND
            
            justFOUND,      // state of previous check was either justLOST or stillLOST and since last check all boards touched.
                            // we stay in this state for one check only, then we go either in OK or justLOST      
        };  
        
        enum class Report : uint8_t { 
            NEVER = 0,                      // no report (diagnostics message) is ever sent.
            justLOSTjustFOUND = 1,          // only asynch report is sent at entering State::justLOST and State::justLOSTjustFOUND
            justLOSTjustFOUNDstillLOST = 2, // as in Report::justLOSTjustFOUND plus report every Config::rateofregularreport when we are in State::stillLOST
            ALL = 3                         // as in Report::justLOSTjustFOUNDstillLOST plus report every Config::rateofregularreport when we are in State::OK 
        };     

        // it tells when the report is enabled given the State and the Report mode
        static constexpr bool reportenabled(State s, Report r)
        {
            bool e {false};
            switch(s)
            {
                case State::justLOST: 
                case State::justFOUND: { e = (Report::NEVER != r); } break;
                case State::stillLOST: { e = (Report::ALL == r) || ((Report::justLOSTjustFOUNDstillLOST == r)); } break;
                case State::OK: { e = (Report::ALL == r); } break;
            }
            return e;
        }
        
        struct MAP
        {   // we dont want any cl.addr 0 or 15 inside mask[] -> see MAP() and set()
            static constexpr uint16_t validmask {0x7ffe};
            uint16_t mask[eOcanports_number] = {0x0000, 0x0000};
            constexpr MAP(uint16_t mc1, uint16_t mc2) { mask[eOcanport1] = mc1 & validmask; mask[eOcanport2] = mc2 & validmask; }
            constexpr MAP() = default;
            void set(eObrd_canlocation_t cl) 
            { 
                embot::core::binary::bit::set(mask[cl.port], cl.addr); 
                mask[cl.port] &= validmask;
            }
            void clear(eObrd_canlocation_t cl) { embot::core::binary::bit::clear(mask[cl.port], cl.addr); }
            constexpr bool check(eObrd_canlocation_t cl) const { return embot::core::binary::bit::check(mask[cl.port], cl.addr); }
            void clear() { mask[eOcanport1] = mask[eOcanport2] = 0; }
            constexpr bool empty() const { return ((0 == mask[eOcanport1]) && (0 == mask[eOcanport2])); }
            constexpr uint32_t getcompact() const { return (static_cast<uint32_t>(mask[eOcanport1]) << 16) | static_cast<uint32_t>(mask[eOcanport2]); }
        };
                
        struct Config
        {
            MAP target {}; // the boards to monitor
            embot::core::relTime rateofcheck {100*embot::core::time1millisec};      // how often we verify
            Report reportmode {Report::NEVER};   // tells which states can be reported
            embot::core::relTime rateofregularreport {10*embot::core::time1second}; // how often we send reports for State::OK or State::stillLOST
            
            const char *ownername {"dummy"}; // for printing the name of the owner 
            eOmn_serv_category_t servicecategory {eomn_serv_category_unknown}; // for reporting the service owning the CANmonitor
            
            constexpr Config(const MAP &map, embot::core::relTime rc, Report rm, embot::core::relTime rr, const char *o, eOmn_serv_category_t s)
            {
                target = map; rateofcheck = rc; reportmode = rm; ownername = o; servicecategory = s; 
            }  
            
            Config() = default;
        };
      
        CANmonitor();
        ~CANmonitor();       
        
        bool configure(const Config &cfg);  
        bool add(eObrd_canlocation_t cl);
        bool rem(eObrd_canlocation_t cl);
        bool setcheckrate(embot::core::relTime r);        
        
        bool start();
        bool stop();
        bool touch(eObrd_canlocation_t loc);  // called at every reception of a CAN frame from that CAN location 
        bool tick();                          // to be called often, even not regularly, typically inside the Tick() of the service                   
        
    private:        
        struct Impl;
        Impl *pImpl;    
    };    
    
        

}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

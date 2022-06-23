

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_SERVICE_H_
#define __EMBOT_APP_ETH_SERVICE_H_

#include "embot_core.h"
#include "embot_core_binary.h"

#include "EoCommon.h"
#include "EOnv.h"
#include "EOrop.h"
#include "EoManagement.h"


#include <vector>
#include <memory>


namespace embot { namespace app { namespace eth {
    
    typedef eOresult_t (*eOservice_onendofoperation_fun_t) (void* p, eObool_t operationisok);
    
    struct DescriptorCANframe
    {
        eOcanport_t port {eOcanport1};
        eOcanframe_t *frame {nullptr};
        uint32_t extra {0};       
        constexpr DescriptorCANframe() = default;            
        constexpr DescriptorCANframe(eOcanport_t p, eOcanframe_t *f, uint32_t e) : port(p), frame(f), extra(e) {};
    };
      
    struct DescriptorROP
    {
        eOropdescriptor_t *rd {nullptr};
        EOnv *nv {nullptr};
        uint32_t extra {0};       
        constexpr DescriptorROP() = default;            
        constexpr DescriptorROP(eOropdescriptor_t *r, EOnv *n, uint32_t e) : rd(r), nv(n), extra(e) {};
    };


    class Service
    {
        // pure interface
        public:
        
        using fpOnEndOfOperation = bool (*)(Service *s, const eOmn_serv_configuration_t *sc, bool operationisok);
        
        enum class Category : uint8_t { // the same values as eOmn_serv_category_t
            mc = 0, strain = 1, mais = 2, inertials = 3, skin = 4, inertials3 = 5,
            temperatures = 6, psc = 7, pos = 8, ft = 9, 
            all = 128, test = 200, unknown = 254, none = 255 
        };
        static constexpr uint8_t numberOfCategories {10}; 
        
        enum class Type : uint8_t { // the same values as eOmn_serv_type_t
            MC_foc = 1, MC_mc4plus = 3, 
            AS_ft = 17, 
            unknown = 254, none = 255 
        };
        static constexpr uint8_t numberOfTypes {18};

        
        enum class State : uint8_t { // the same values of eOmn_serv_state_t 
            notsupported = 0, idle = 1, verifying = 2, verified = 3, activated = 4, 
            failureofverify = 5, started = 6, fatalerror = 7
        };
        
        
        virtual Category category() const = 0;
        
        virtual State state() const = 0;
        virtual void set(State s) = 0;
        //virtual bool Verify(const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, bool activateafterverify) = 0;
        virtual bool verify(const eOmn_serv_configuration_t * servcfg, bool activateafterverify, fpOnEndOfOperation onendoperation) = 0;
        virtual bool activate(const eOmn_serv_configuration_t * servcfg) = 0;
        virtual bool deactivate() = 0; //const eOmn_serv_configuration_t * servcfg) = 0;
        virtual bool start() = 0;
        virtual bool stop() = 0;
        virtual bool setregulars(eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem) = 0;
        virtual bool tick(bool resetstatus = false) = 0;
//        virtual bool AcceptCANframe(const canFrameDescriptor &canframedescriptor) = 0;
        virtual bool report() = 0;   
        
        virtual bool process(const DescriptorCANframe &canframedescriptor) = 0;
        virtual bool process(const DescriptorROP &ropdescriptor) = 0;
                
    protected:
        virtual ~Service() {};    // cannot delete from interface but only from derived object
 
    }; 
    
    // maybe put it inside theServices
    
    Service * get(Service::Category category);


}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

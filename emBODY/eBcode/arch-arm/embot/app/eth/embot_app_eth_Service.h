

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

#include "embot_app_eth_mc_messaging.h"
#include "embot_prot_can.h"

#include <vector>
#include <memory>


namespace embot::app::eth {
    
  
    class Service
    {
        // pure interface
        public:
            
        // just for a CAN frame
        struct DescriptorCANframe
        {
            eOcanport_t port {eOcanport1};
            eOcanframe_t *frame {nullptr};
            uint32_t extra {0};       
            constexpr DescriptorCANframe() = default;            
            constexpr DescriptorCANframe(eOcanport_t p, eOcanframe_t *f, uint32_t e) : port(p), frame(f), extra(e) {};
        };
        
        // for a CAN or ICC frame
        struct DescriptorFrame
        {
            embot::msg::BUS bus {embot::msg::BUS::none};
            embot::prot::can::Frame frame {};
            constexpr DescriptorFrame() = default;
        };
        
        // for a ROP transported by UDP       
        struct DescriptorROP
        {
            const eOropdescriptor_t *rd {nullptr};
            const EOnv *nv {nullptr};
            uint32_t extra {0};       
            constexpr DescriptorROP() = default;            
            constexpr DescriptorROP(const eOropdescriptor_t *r, const EOnv *n, uint32_t e) : rd(r), nv(n), extra(e) {};
        };        
        
        
        enum class Category : uint8_t { // they are the same values as eOmn_serv_category_t
            mc = 0, strain = 1, mais = 2, skin = 3, inertials3 = 4,
            temperatures = 5, psc = 6, pos = 7, ft = 8, bat = 9,
            all = 128, test = 200, unknown = 254, none = 255 
        };
        static constexpr uint8_t numberOfCategories {10}; 
        
        enum class Type : uint8_t { // they are the same values as eOmn_serv_type_t
            MC_foc = 1, MC_mc4plus = 3, MC_advfoc = 15,
            AS_ft = 16, 
            unknown = 254, none = 255 
        };
        static constexpr uint8_t numberOfTypes {18};

        
        enum class State : uint8_t { // they are the same values as eOmn_serv_state_t 
            notsupported = 0, idle = 1, verifying = 2, verified = 3, activated = 4, 
            failureofverify = 5, started = 6, fatalerror = 7
        };
        
        struct Config
        {   // for now we support only a Typ::eOmn_serv that implements a wrapper to eOmn_serv_configuration_t
            enum class Type : uint8_t { eOmn_serv = 0 };            
            virtual Type type() const = 0;
            
            virtual const void * memory() const = 0;
            
        protected:
            virtual ~Config() {};
        };
        
        // we may use a embot::core::Confirmer where we later cast the void* to Service* but it is cleaner to have a dedicated type
        using OnEndOfOperation = void (*)(Service *s, bool result);
        
        virtual Category category() const = 0;
        virtual Type type() const = 0;
        
        virtual State state() const = 0;
        virtual void set(State s) = 0;

        virtual bool verifyactivate(const Config * servcfg, OnEndOfOperation onendofverifyactivate) = 0;
        virtual bool deactivate() = 0;
        virtual bool start() = 0;
        virtual bool stop() = 0;
        // if arrayofid32 is nullptr than we remove them all. if not we substitute. we dont add
        virtual bool setregulars(const eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem) = 0;
        // method signal() may be better. if vector is empty we remove signalling of all internal id32, else we add them
        // however, it takes some changes until theServices::setREGULARS() that i prefer postpone
        // virtual bool signal(const std::vector<eOprotID32_t> &id32s) = 0;
        virtual bool tick(bool resetstatus = false) = 0;
        virtual bool report() = 0;   
        
        virtual bool process(const DescriptorCANframe &canframedescriptor) = 0;
        // DescriptorFrame can be either CAN or ICC  
        virtual bool process(const DescriptorFrame &framedescriptor) = 0;
        virtual bool process(const DescriptorROP &ropdescriptor) = 0;
                
    protected:
        virtual ~Service() {};    // cannot delete from interface but only from derived object
 
    }; 
    

} // namespace embot::app::eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

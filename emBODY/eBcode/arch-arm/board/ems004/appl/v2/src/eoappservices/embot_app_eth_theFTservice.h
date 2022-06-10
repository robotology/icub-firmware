

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEFTSERVICE_H_
#define __EMBOT_APP_ETH_THEFTSERVICE_H_

#include "embot_core.h"
#include "embot_core_binary.h"

#include "EoCommon.h"
#include "EoProtocol.h"

#if defined(USE_EMBOT_theServices)  
#include "embot_app_eth_Service.h"
#else
#include "EOtheServices.h"
#endif

#include "embot_app_eth_theServiceTester.h"

#include <vector>
#include <memory>


namespace embot { namespace app { namespace eth {
      
      
    // fills a given eOropdescriptor_t w/ whatever is required to be given to theFTservice::process()   
    const eOropdescriptor_t * fill(eOropdescriptor_t &rd, eOnvID32_t id32, void *data, uint16_t size, eOropcode_t rpc = eo_ropcode_set);
    
    class theFTservice
    {
    public:
        static theFTservice& getInstance();
                
    public:
        
        static constexpr size_t maxSensors {eOas_ft_sensors_maxnumber}; 
        static constexpr size_t maxRegulars {maxSensors};
                
        struct Config
        {
            uint32_t tbd {0};
            constexpr Config() = default;
            constexpr Config(uint32_t t) : tbd(t) {}
            constexpr bool isvalid() const { return (666 == tbd) ? false : true; }
        }; 
        
        struct canFrameDescriptor
        {
            enum class Type : uint8_t { unspecified, fullscale, force, torque, temperature };
            eOcanport_t port {eOcanport1};
            eOcanframe_t *frame {nullptr};
            Type type {Type::unspecified};            
            canFrameDescriptor() = default;            
            canFrameDescriptor(eOcanport_t p, eOcanframe_t *f, Type t) : port(p), frame(f), type(t) {};
        };       
        
        eOresult_t initialise(const Config &config);  

        eOmn_serv_state_t GetServiceState() const;        
        eOresult_t Verify(const eOmn_serv_configuration_t * servcfg, 
                          eOservice_onendofoperation_fun_t onverify, 
                          bool activateafterverify);         
        eOresult_t Activate(const eOmn_serv_configuration_t * servcfg);           
        eOresult_t Deactivate();        
        eOresult_t Start();        
        eOresult_t Stop();        
        eOresult_t SetRegulars(eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem);        
        eOresult_t Tick();          
        // processes a CAN frame coming from the sensor        
        eOresult_t AcceptCANframe(const canFrameDescriptor &canframedescriptor);        
        // it can be called by the ETH callbacks eoprot_fun_UPDT_as_ft_*(EOnv* nv, eOropdescriptor_t* rd) 
        // in such a case use its nv and rd argument
        // but it can be called by any other module to emulate reception of a ROP.
        // in such a case, use nv = nullptr and the embot::app::eth::fill(eOropdescriptor_t ...) function
        eOresult_t process(const eOropdescriptor_t* rd, const EOnv* nv = nullptr);           
        eOresult_t SendReport();   
        
        // default status of the FT sensors
        static constexpr eOas_ft_status_t defaultFTstatus 
        {
            .timedvalue = 
            {
                .age = 0,
                .info = 0,
                .ffu = 0,
                .temperature = -2731, // ~ zero kelvin
                .values = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}                 
            },
            .fullscale = {1, 1, 1, 1, 1, 1},
            .mode = eoas_ft_mode_calibrated,
            .filler = {0, 0, 0}
        };
        // default configuration of the FT sensors
        static constexpr eOas_ft_config_t defaultFTconfig 
        {
            .mode = eoas_ft_mode_calibrated,
            .ftperiod = 10,
            .calibrationset = 0,
            .temperatureperiod = 1
        };  

        const theServiceTester::Config & servicetesterconfig() const;        
       
    private:        
        // this one is called inside process() when the tag is eoprot_tag_as_ft_config (or by theServiceTester)
        bool set(eOprotIndex_t index, const eOas_ft_config_t *ftc);
        // this one is called inside process() when the tag is eoprot_tag_as_ft_cmmnds_enable (or by theServiceTester)
        bool enable(eOprotIndex_t index, const uint8_t *cmdenable);
                     
    private:
        theFTservice(); 
        ~theFTservice(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;     
    }; 


//    class theFTserviceTester : public ServiceTester
//    {
//    public:
//        static theFTserviceTester& getInstance();
//                
//    public:
//                  
//        // bool initialise(const Config &config);  
//        virtual eOmn_serv_category_t category() const { return eomn_serv_category_ft; }
//        virtual eOmn_serv_state_t state() const;
//        virtual const eOmn_serv_configuration_t & configuration() const;        
//        virtual bool initialise();
//        virtual bool tick(Caller caller);

//        
//    private:
//        theFTserviceTester(); 
//        ~theFTserviceTester(); 

//    private:    
//        struct Impl;
//        std::unique_ptr<Impl> pImpl;     
//    };       
//    


}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

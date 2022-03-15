
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theServiceTester.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSconfigurator.h"

#define USE_EMBOBJ_MOSTLY
#undef USE_EMBOT_ONLT

#if defined(USE_EMBOBJ_MOSTLY)
#include "EOtimer.h"
#include "EOMmutex.h"
#include "EOVtheCallbackManager.h"
#else
    #error embot-only implementation is yest to be done
#endif


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

#define USE_EMBOBJ_MOSTLY
#undef USE_EMBOT_ONLY


struct embot::app::eth::theServiceTester::Impl
{
    
    struct mymtx_t
    {
#if defined(USE_EMBOBJ_MOSTLY)        
        EOMmutex *m {nullptr};
        
        mymtx_t() { m = eom_mutex_New(); }
        ~mymtx_t() { eom_mutex_Delete(m); }
        void take() { eom_mutex_Take(m, eok_reltimeINFINITE); }
        void release () { eom_mutex_Release(m); }  
#else
#endif        
    };
    
    struct Functor
    {
        mymtx_t *_mtx {nullptr};
        theServiceTester::Caller _caller { theServiceTester::Caller::none};
        embot::core::Callback _cbk{};  
        embot::core::Callback _ontick{};

        Functor() = default;  
            
        void initialise(const embot::core::Callback &ontick) 
        {
            if(nullptr == _mtx)
            {
                _mtx = new mymtx_t();
            }
            _ontick = ontick;
            clear();
        }
        
        void clear()
        {
            _caller = theServiceTester::Caller::none;
            _cbk.clear();
        }
        
        void load(theServiceTester::Caller ca, const embot::core::Callback &c)
        {
            _mtx->take();
            _caller = ca;
            _cbk = c;
            _mtx->release();
        }
        
        void tick(theServiceTester::Caller ca)
        {
            // we execute only if .... 
            _mtx->take();
            if(ca == _caller)
            {
                _cbk.execute();            
                clear();
                _ontick.execute();        
            }
            _mtx->release();           
        }
    };    
    
    Config config {};            
    eOmn_service_cmmnds_command_t * s_command = {0};   

#if defined(USE_EMBOBJ_MOSTLY)
    EOtimer *s_timer {nullptr};
    EOaction_strg s_astrg {0};
    EOaction *s_act { reinterpret_cast<EOaction*>(&s_astrg) };
#endif
    
    Functor functor {};
    uint8_t step2use {0};
    
    Impl(); 
    
    bool initialise(const Config &cfg);    
    bool tick(Caller caller); 
    
    void sendevent2cfgdeferredby(embot::core::relTime delta);
    void force_exec_inrunby(embot::core::relTime delta, const embot::core::Callback &cbk);
        
    static void ticker(void *p);
        
    static void serv_STOP(void *p);
    static void serv_VERIFYACTIVATE(void *p);
    static void serv_CONFIG(void *p);
    static void serv_TXstart(void *p);
    static void serv_REGULARS(void *p);
    static void serv_START(void *p);
    
    static void fillfunctorTX(void *p);
    static void fillfunctorSTOP(void *p);
};

embot::app::eth::theServiceTester::Impl::Impl()
{
}

bool embot::app::eth::theServiceTester::Impl::initialise(const Config &cfg)
{
    config = cfg;    
    s_timer = eo_timer_New();
    s_act = reinterpret_cast<EOaction*>(&s_astrg);
    functor.initialise({ticker, this});
    s_command =  new eOmn_service_cmmnds_command_t;
    s_command->category = config.category; 
    
    ticker(this);
    
    return true;
} 

bool embot::app::eth::theServiceTester::Impl::tick(Caller caller)
{
    functor.tick(caller);
    return true;
}

void embot::app::eth::theServiceTester::Impl::sendevent2cfgdeferredby(embot::core::relTime delta)
{
#if defined(USE_EMBOBJ_MOSTLY)
    EOVtaskDerived * t = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());
    eo_action_SetEvent(s_act, emsconfigurator_evt_userdef03, t);    
    eo_timer_Start(s_timer, eok_abstimeNOW, delta, eo_tmrmode_ONESHOT, s_act);  
#endif    
}

void embot::app::eth::theServiceTester::Impl::fillfunctorTX(void *p)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    impl->functor.load(Caller::threadRUN, {serv_TXstart, impl});    
}

void embot::app::eth::theServiceTester::Impl::fillfunctorSTOP(void *p)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    impl->functor.load(Caller::threadRUN, {serv_STOP, impl});    
}

void embot::app::eth::theServiceTester::Impl::force_exec_inrunby(embot::core::relTime delta, const embot::core::Callback &cbk)
{
#if defined(USE_EMBOBJ_MOSTLY)    
    EOVtaskDerived * t = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());
    eo_action_SetCallback(s_act, cbk.call, cbk.arg, eov_callbackman_GetTask(eov_callbackman_GetHandle()));    
    eo_timer_Start(s_timer, eok_abstimeNOW, delta, eo_tmrmode_ONESHOT, s_act);
#endif    
}

void embot::app::eth::theServiceTester::Impl::serv_STOP(void *p)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    impl->s_command->operation = eomn_serv_operation_stop;
    memset(&impl->s_command->parameter.configuration, 0, sizeof(eOmn_serv_configuration_t));
    eo_services_ProcessCommand(eo_services_GetHandle(), impl->s_command);           
}

void embot::app::eth::theServiceTester::Impl::serv_VERIFYACTIVATE(void *p)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    impl->s_command->operation = eomn_serv_operation_verifyactivate;
    std::memmove(&impl->s_command->parameter.configuration, impl->config.servconfig, sizeof(eOmn_serv_configuration_t));         
    eo_services_ProcessCommand(eo_services_GetHandle(), impl->s_command);   
}

void embot::app::eth::theServiceTester::Impl::serv_CONFIG(void *p)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    if(nullptr != impl->config.setconfig)
    {
        impl->config.setconfig(); 
    }
}

void embot::app::eth::theServiceTester::Impl::serv_TXstart(void *p)
{
    Impl *impl = reinterpret_cast<Impl*>(p);

    if(nullptr != impl->config.txstart)
    {
        impl->config.txstart();
    } 
}

void embot::app::eth::theServiceTester::Impl::serv_REGULARS(void *p)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    impl->s_command->operation = eomn_serv_operation_regsig_load;
    memset(&impl->s_command->parameter.configuration, 0, sizeof(eOmn_serv_configuration_t));
    
    eOmn_serv_arrayof_id32_t id32s = {};
    EOarray* ar = eo_array_New(eOmn_serv_capacity_arrayof_id32, sizeof(eOprotID32_t), &impl->s_command->parameter.arrayofid32);    

    size_t nn = std::min(impl->config.regulars.size(), impl->config.sizeofregulars);
     
    for(uint8_t i=0; i<nn; i++)
    {        
        eOprotID32_t id32 = impl->config.regulars[i];
        eo_array_PushBack(ar, &id32); 
    } 
       
    eo_services_ProcessCommand(eo_services_GetHandle(), impl->s_command);        
}


void embot::app::eth::theServiceTester::Impl::serv_START(void *p)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    impl->s_command->operation = eomn_serv_operation_start;
    memset(&impl->s_command->parameter.configuration, 0, sizeof(eOmn_serv_configuration_t));        
    eo_services_ProcessCommand(eo_services_GetHandle(), impl->s_command);      
}


void embot::app::eth::theServiceTester::Impl::ticker(void *p)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    // in here we implement what we want.
    eOreltime_t delta = 1*eok_reltime1sec;
    eOreltime_t runTXtime = 20*eok_reltime1sec;
    
    //impl->step2use = 1;
    
    if(0 == impl->step2use)
    {
        // INIT asks to CFG -> STOP()
        impl->functor.load(Caller::threadCFG, {impl->serv_STOP, impl});        
        impl->sendevent2cfgdeferredby(delta);         
    }
    else if(1 == impl->step2use)
    {
        // CFG ask to CFG -> VERIFY_ACTIVATE(okconfig)
        impl->functor.load(Caller::threadCFG, {serv_VERIFYACTIVATE, impl});        
        impl->sendevent2cfgdeferredby(delta);        
    }  
    else if(2 == impl->step2use)
    {
        // CFG asks to CFG -> CFG()
        impl->functor.load(Caller::threadCFG, {serv_CONFIG, impl});        
        impl->sendevent2cfgdeferredby(delta);        
    }     
    else if(3 == impl->step2use)
    {
        // CFG asks to CFG -> REGULARS()
        impl->functor.load(Caller::threadCFG, {serv_REGULARS, impl});        
        impl->sendevent2cfgdeferredby(delta);        
    }     
    else if(4 == impl->step2use)
    {
        // CFG asks to CFG -> START()
        impl->functor.load(Caller::threadCFG, {serv_START, impl});        
        impl->sendevent2cfgdeferredby(delta);        
    } 
    else if(5 == impl->step2use)
    {
        // we are still inside CFG which has just executed serv_START()
        // but we stay in CFG for short time because the START sends the application in RUN mode
        // so, ... next request will be done to RUN
        // CFG asks to RUN -> TX()
        // we exec a callback by delta time which:
        // - fills the functor for the sake of the run thread
        
        impl->functor.clear();            
        impl->force_exec_inrunby(delta, {fillfunctorTX, impl});
               
        // next time we execute inside RUN ....         
    } 
    else if(6 == impl->step2use)
    {
        // we have just started TX on CAN
        
        impl->functor.clear();            
        impl->force_exec_inrunby(runTXtime, {fillfunctorSTOP, impl});        
    }
    
    impl->step2use++;    
    
}


// --------------------------------------------------------------------------------------------------------------------


embot::app::eth::theServiceTester& embot::app::eth::theServiceTester::getInstance()
{
    static theServiceTester* p = new theServiceTester();
    return *p;
}

embot::app::eth::theServiceTester::theServiceTester()
{
    pImpl = std::make_unique<Impl>();
}  
    
embot::app::eth::theServiceTester::~theServiceTester() { }
        
bool embot::app::eth::theServiceTester::initialise(const Config &cfg)
{
    return pImpl->initialise(cfg);
}

bool embot::app::eth::theServiceTester::tick(Caller caller)
{
    return pImpl->tick(caller);
}
  

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



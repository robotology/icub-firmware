
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "servicetester.h"


#if defined(enableTHESERVICETESTER)

#include "EOMtheEMSconfigurator.h"
#include "embot_app_eth_theServiceTester.h"

//#define TESTtheFTservice
#define TESTtheMCserviceEO // embobj service 

#endif // enableTHESERVICETESTER


#if defined(TESTtheFTservice)
#include "embot_app_eth_theFTservice.h"
#elif defined(TESTtheMCserviceEO)
#include "embot_app_eth_theMCserviceEOtesterconfig.h"
#endif

extern void servicetester_init(void)
{
#if !defined(enableTHESERVICETESTER)
    // do nothing
#else
    
    // 1. get the config that depends on the kind of service you want to test
#if defined(TESTtheFTservice)
    const embot::app::eth::theServiceTester::Config &cfg = embot::app::eth::theFTservice::getInstance().servicetesterconfig(); 
#elif defined(TESTtheMCserviceEO) 
    const embot::app::eth::theServiceTester::Config &cfg = embot::app::eth::theMCserviceEOtesterconfig::getInstance().servicetesterconfig();
#else
    #error you must choose which service to test
#endif
    
    // 2. and initialise the service tester
    embot::app::eth::theServiceTester::getInstance().initialise(cfg);   
#endif    
}

// this must be called inside eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception()
extern void servicetester_runtick(void)
{   
#if !defined(enableTHESERVICETESTER)
// do nothing
#else
    embot::app::eth::theServiceTester::getInstance().tick(embot::app::eth::theServiceTester::Caller::threadRUN);
#endif    
}

// this must be called inside eom_emsconfigurator_hid_userdef_ProcessUserdef03Event()
extern void servicetester_cfgtick(void)
{    
#if !defined(enableTHESERVICETESTER)
// do nothing
#else
    embot::app::eth::theServiceTester::getInstance().tick(embot::app::eth::theServiceTester::Caller::threadCFG);
#endif    
}


#if !defined(enableTHESERVICETESTER)
// nothing
#else 
// this is the function that ticks the service tester when the board is in CFG state
extern "C"
{
    extern void eom_emsconfigurator_hid_userdef_ProcessUserdef03Event(EOMtheEMSconfigurator* p)
    {
        servicetester_cfgtick();
    }
}
#endif


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


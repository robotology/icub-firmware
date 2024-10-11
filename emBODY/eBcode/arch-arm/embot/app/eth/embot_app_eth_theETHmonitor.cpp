
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theETHmonitor.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os.h"
#include "embot_os_rtos.h"
#include "embot_hw_eth.h"
#include "embot_app_eth_theErrorManager.h"
#include "embot_os_theScheduler.h"
#include "EOtheErrorManager.h"
#include "EoError.h"
#include "EOMtheEMSrunner.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::app::eth::theETHmonitor::Impl
{
		//constexpr static char objectname[] = "theETHmonitor"; 
		
    Config _config {};
    bool _initted {false};
    
    embot::os::PeriodicThread *_thr {nullptr};

    embot::os::rtos::mutex_t *_mtx {nullptr};
        
    Impl() = default;      
    
    bool initialise(const Config &config);   
    
    bool set(const embot::core::Callback &tkr);
    bool tick();
    
		
		struct linkinfo
		{   
			bool previsup {false};
			embot::core::Time changedTime {0};
		}; 
		
		linkinfo link1 {};
		linkinfo link2 {};

		embot::core::Time lastreportTime {0};

    static void startup(embot::os::Thread *t, void *p);  
    static void onperiod(embot::os::Thread *t, void *p);  

private:
    bool ticker();        
};


static const char s_eobj_ownname[] = "EOtheETHmonitor";

void tETHmon(void *p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }

bool embot::app::eth::theETHmonitor::Impl::initialise(const Config &config)
{
    if(true == _initted)
    {
        return true;
    }
    
    if(false == config.isvalid())
    {
        return false;
    }
    
    _config = config;
    
    embot::core::print("embot::app::eth::theETHmonitor::Impl::initialise()");    
    
 
    // create the mutex
    _mtx = embot::os::rtos::mutex_new();
    
    constexpr embot::core::relTime timeout {1000*embot::core::time1millisec};

    embot::os::PeriodicThread::Config tCfg { 
        _config.thread.stacksize, 
        _config.thread.priority, 
        startup, this,
        _config.period,
        onperiod,
        "tETHmonitor"
    };
	
		lastreportTime = embot::core::now();
	
		link1.previsup = false;
		link1.changedTime = embot::core::now();
		
		link2.previsup = false;
		link2.changedTime = embot::core::now();
       
        
    // create the periodic thread 
    _thr = new embot::os::PeriodicThread;          
    // and start it
    _thr->start(tCfg, tETHmon);
    
    _initted = true;
    return true;
}

bool embot::app::eth::theETHmonitor::Impl::set(const embot::core::Callback &ticker)
{
    embot::os::rtos::mutex_take(_mtx, embot::core::reltimeWaitForever);
    _config.ticker = ticker;
    embot::os::rtos::mutex_release(_mtx);  
    return true;
}

bool embot::app::eth::theETHmonitor::Impl::ticker()
{
    embot::os::rtos::mutex_take(_mtx, embot::core::reltimeWaitForever);
    _config.ticker.execute();
    embot::os::rtos::mutex_release(_mtx);  
    return true;
}


bool embot::app::eth::theETHmonitor::Impl::tick()
{
    // in here we check if .... we have any data to put into the error manager
    return true;
}



// the static ones 

void embot::app::eth::theETHmonitor::Impl::startup(embot::os::Thread *t, void *p)
{
    embot::app::eth::theETHmonitor::Impl *impl = reinterpret_cast<embot::app::eth::theETHmonitor::Impl*>(p);
    embot::core::print("TODO: tETHmon must yet init embot::eth:: etc"); 
}

void embot::app::eth::theETHmonitor::Impl::onperiod(embot::os::Thread *t, void *p)
{
	embot::app::eth::theETHmonitor::Impl *impl = reinterpret_cast<embot::app::eth::theETHmonitor::Impl*>(p);
	
	embot::core::Time nowTime = embot::core::now();
	
	bool link1isup = false;    
	bool link2isup = false;

	link1isup = embot::hw::eth::islinkup(embot::hw::PHY::one); 
	link2isup = embot::hw::eth::islinkup(embot::hw::PHY::two); 
	
//	embot::app::eth::theErrorManager::Descriptor errdescr {};
	
	eOerrmanDescriptor_t errdes = {0};
	
	uint64_t applstate = (eobool_true == eom_emsrunner_IsRunning(eom_emsrunner_GetHandle())) ? (0x3000000000000000) : (0x1000000000000000);   
		
	
	if((impl->link1.previsup != link1isup) || ((impl->link2.previsup != link2isup)))
	{
		std::string msg = std::string("ETH link 1 (J6) is ") + (link1isup ? "UP" : "DOWN") + ", ETH link 2 (J7) is " + (link2isup ? "UP" : "DOWN");
		
		if(true == link1isup)
		{
			if (false == impl->link1.previsup)
			{
				
				//			this is the implementation in C++, but emit miss some functionality 
//			errdescr.code = eoerror_code_get(eoerror_category_ETHmonitor, eoerror_value_ETHMON_justverified);
//			errdescr.sourcedevice = eo_errman_sourcedevice_localboard;	
//			errdescr.sourceaddress = 0;
//			errdescr.par16 = 0;
//			errdescr.par64 = applstate; /// attention if error rxcrc, this shuld be modified
//							
//			embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::info, {objectname, t}, errdescr, "z");    
								
				errdes.code             = eoerror_code_get(eoerror_category_ETHmonitor, eoerror_value_ETHMON_link_goes_up);
				errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
				errdes.sourceaddress    = 0;
				errdes.par16            = 0;
				errdes.par64            = applstate;
				eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes); 
			}
			impl->link1.changedTime = nowTime;
			//here code for crc errors
		}
		else
		{
			if (true == impl->link1.previsup)
			{
				errdes.code             = eoerror_code_get(eoerror_category_ETHmonitor, eoerror_value_ETHMON_link_goes_down);
				errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
				errdes.sourceaddress    = 0;
				errdes.par16            = 0;
				errdes.par64            = applstate;
				eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);   
			}
			
			impl->link1.changedTime = nowTime;
		}
	
		
		if(true == link2isup)
		{
			if (false == impl->link2.previsup)
			{
				errdes.code             = eoerror_code_get(eoerror_category_ETHmonitor, eoerror_value_ETHMON_link_goes_up);
				errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
				errdes.sourceaddress    = 0;
				errdes.par16            = 1;
				errdes.par64            = applstate;
				eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes); 
			}
			impl->link2.changedTime = nowTime;
			//here code for crc errors
		}
		else
		{
			if (true == impl->link2.previsup)
			{
				errdes.code             = eoerror_code_get(eoerror_category_ETHmonitor, eoerror_value_ETHMON_link_goes_down);
				errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
				errdes.sourceaddress    = 0;
				errdes.par16            = 1;
				errdes.par64            = applstate;
				eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);   
			}
			
			impl->link2.changedTime = nowTime;
		}

		
		embot::app::eth::theErrorManager::getInstance().trace(msg, {"onperiod(): change link status: ",     
						embot::os::theScheduler::getInstance().scheduled()});
	}
	
	if(nowTime - impl->lastreportTime >= impl->_config.reportOKperiod)
	{
		embot::core::Time delta1 = embot::core::TimeFormatter(nowTime - impl->link1.changedTime).to_seconds();
		embot::core::Time delta2 = embot::core::TimeFormatter(nowTime - impl->link2.changedTime).to_seconds();
		
		std::string msgOK = std::string("ETH link 1 (J6) is ") + (link1isup ? "UP" : "DOWN")+ " by " +std::to_string(delta1) +" seconds, " +  
											std::string("ETH link 2 (J7) is ") + (link2isup ? "UP" : "DOWN")+ " by " +std::to_string(delta2) +" seconds";
		
		embot::app::eth::theErrorManager::getInstance().trace(msgOK, {"onperiod(): report OK period: ",     
						embot::os::theScheduler::getInstance().scheduled()});
					
			errdes.code             = eoerror_code_get(eoerror_category_ETHmonitor, eoerror_value_ETHMON_justverified);
			errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
			errdes.sourceaddress    = 0;
			errdes.par16            = link1isup | (link2isup << 1) | (true << 2);
			errdes.par64            = applstate;
			eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &errdes);  
		
//		errdescr.code = eoerror_code_get(eoerror_category_ETHmonitor, eoerror_value_ETHMON_justverified);
//		errdescr.sourcedevice = eo_errman_sourcedevice_localboard;	
//		errdescr.sourceaddress = 0;
//		errdescr.par16 = link1isup | (link2isup << 1) | 0x01<<2 ; // 0x01<<2 there is a third port embedded
//		errdescr.par64 = applstate; /// attention if error rxcrc, this shuld be modified
						
//		embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::info, {objectname, t}, errdescr, msg);    

		impl->lastreportTime = nowTime;
	}

	impl->link1.previsup = link1isup;
	impl->link2.previsup = link2isup;
}




// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theETHmonitor& embot::app::eth::theETHmonitor::getInstance()
{
    static theETHmonitor* p = new theETHmonitor();
    return *p;
}

embot::app::eth::theETHmonitor::theETHmonitor()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::eth::theETHmonitor::~theETHmonitor() { }
        

bool embot::app::eth::theETHmonitor::initialise(const Config &config)
{
    return pImpl->initialise(config);
}


bool embot::app::eth::theETHmonitor::set(const embot::core::Callback &ticker)
{
    return pImpl->set(ticker);
}


bool embot::app::eth::theETHmonitor::tick()
{
    return pImpl->tick();
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



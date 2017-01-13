
// start of file


#include "some-classes.h"


#include "EoCommon.h"
#include "EOtimer.h"
#include "EOaction.h"

namespace embot {   
namespace sys {
    
struct Timer::Impl
{
    EOtimer *tmr;
    Impl() 
    {
        tmr = eo_timer_New();
    }
    ~Impl()
    {
        eo_timer_Delete(tmr);
    }
};

Timer::Timer()
: pImpl(new Impl)
{   

}

Timer::~Timer()
{   
    delete pImpl;
}


bool Timer::start(relTime countdown, Mode mode, EOaction *action)
{
    eOresult_t r = eo_timer_Start(pImpl->tmr, eok_abstimeNOW, countdown, static_cast<eOtimerMode_t>(mode), action);    
    return (eores_OK == r) ? true : false;
}

bool Timer::stop()
{
    eOresult_t r = eo_timer_Stop(pImpl->tmr);    
    return (eores_OK == r) ? true : false;
}

Timer::Status Timer::getStatus()
{
    eOtimerStatus_t st = eo_timer_GetStatus(pImpl->tmr);    
    return static_cast<Timer::Status>(st);   
}

}
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------







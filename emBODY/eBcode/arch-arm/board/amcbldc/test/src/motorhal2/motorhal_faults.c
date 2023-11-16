
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Alessandro Scalzo
 * email:   alessandro.scalzo@iit.it
*/


#if 0
    The files motorhal_config.[h, c] contain macros and code which cuts aways un-necessary dependencies
    for the files provided by Giorgio Zini for the motor control of the amcbldc. These files are:
    encoder.[c, h], pwm.[c, h], analog.[c, h]. The dependencies cut away are those which offer
    generic services such as logging, leds, flash storage.    
#endif

// CODE SHAPER
#if defined(USE_STM32HAL)
#include "motorhal_faults.h"
#endif

static faultmask_t faultmask;

unsigned long motorhal_get_faultmask()
{
    return faultmask.faults;
}

unsigned long motorhal_clear_faultmask()
{
    unsigned long ret = faultmask.faults;
    faultmask.faults = 0;
    return ret;
}    

    
bool motorhal_check_fault(int n)
{
    return (faultmask.faults & (1<<n)) != 0;
}
bool motorhal_set_fault(int n)
{
    bool ret = (faultmask.faults & (1<<n)) != 0;
    faultmask.faults |= (1<<n);
    return ret;
}

bool motorhal_clear_fault(int n)
{
    bool ret = (faultmask.faults & (1<<n)) != 0;
    faultmask.faults &= ~(1<<n);
    return ret;
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

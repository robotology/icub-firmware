
/* @file       hal_cfg_basic.c
	@brief      This file keeps a very basic config for hal.
	@author     marco.accame@iit.it
    @date       08/21/2014
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "hal.h"
#include "stdlib.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

static void s_hal_cfg_on_fatalerror(hal_fatalerror_t errorcode, const char * errormsg);

static const hal_cfg_t s_cfg = 
{   
    .cpu_family             = (hal_cpufamily_t) HAL_CPUFAM,
    .cpu_type               = (hal_cputype_t) HAL_CPUTYPE,
    .cpu_freq               = HAL_CPUFREQ,
    .sys_stacksize          = 0,
    .sys_heapsize           = 0,
    .display_enable         = (hal_boolval_t) 0,
    .eth_enable             = (hal_boolval_t) 0,
    .eth_dmatxbuffer_num    = 0,
    .eth_dmarxbuffer_num    = 0,
    .can1_enable            = (hal_boolval_t) 0,
    .can1_rxqnorm_num       = 0,
    .can1_txqnorm_num       = 0,
    .can1_txqhigh_num       = 0,
    .can2_enable            = (hal_boolval_t) 0,
    .can2_rxqnorm_num       = 0,
    .can2_txqnorm_num       = 0,
    .can2_txqhigh_num       = 0,
	.spi1_enable            = (hal_boolval_t) 0,
	.spi2_enable            = (hal_boolval_t) 0,
	.spi3_enable            = (hal_boolval_t) 0,
    .timers_num             = 0,
    .arch                   = 
    {
        .pll_freq                           = 10000000,
        .dspengine_moltipl_unsigned         = (uint8_t) 1,
        .dspengine_enable_fractional_mode   = (uint8_t) 1
    },
    .extfn                  =
    {
        .usr_on_fatal_error                 = s_hal_cfg_on_fatalerror,
        .osal_system_scheduling_suspend     = NULL,
        .osal_system_scheduling_restart     = NULL
    }
    
};


const hal_cfg_t *hal_cfgBASIC = &s_cfg;



static void s_hal_cfg_on_fatalerror(hal_fatalerror_t errorcode, const char * errormsg)
{
    errorcode = errorcode;
    if(NULL != errormsg)
    {

    }

    for(;;);

}







// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



// - include guard ----------------------------------------------------------------------------------------------------
#ifndef __EOTHEFATALERROR_H_
#define __EOTHEFATALERROR_H_

#ifdef __cplusplus
extern "C" {
#endif



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheFatalError_hid EOtheFatalError;

typedef enum 
{
    fatalerror_handler_none             = 0,
    fatalerrr_handler_unknown           = 1,
    fatalerror_handler_hal              = 2,
    fatalerror_handler_osal             = 3,
    fatalerror_handler_ipal             = 4,
    fatalerror_handler_hw_NMI           = 5,
    fatalerror_handler_hw_HardFault     = 6,
    fatalerror_handler_hw_MemManage     = 7,
    fatalerror_handler_hw_BusFault      = 8,
    fatalerror_handler_hw_UsageFault    = 9,
    fatalerror_handler_hw_DebugMon      = 10,
    fatalerror_handler_hw_Default       = 11
} fatal_error_handler_t;

enum { fatal_error_handler_numbersof = 12 };

typedef struct
{
    uint8_t handlertype;        // use values from fatal_error_handler_t
    uint8_t handlererrorcode;   
    uint8_t forfutureuse0;
    uint8_t forfutureuse1;  
    void *param; 
    void *mpucontext;     
} fatal_error_descriptor_t;


   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------


extern EOtheFatalError* eo_fatalerror_Initialise(void);

extern EOtheFatalError* eo_fatalerror_GetHandle(void);

extern void eo_fatalerror_AtStartup(EOtheFatalError *p);

extern fatal_error_descriptor_t * eo_fatalerror_GetDescriptor(EOtheFatalError *p);

extern void eo_fatalerror_Restart(EOtheFatalError *p, fatal_error_descriptor_t *des);

extern void eo_fatalerror_Test(EOtheFatalError *p, fatal_error_descriptor_t *des);

extern void eo_fatalerror_RegisterThread(EOtheFatalError *p, void *t, const char *n);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



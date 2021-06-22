

/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EOTHEFATALERROR_HID_H_
#define __EOTHEFATALERROR_HID_H_


#ifdef __cplusplus
extern "C" {
#endif

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

#include "eEsharedServices.h"
    
#include "EOtheErrorManager.h"
    
#include "osal.h"

// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheFatalError.h"
    
 

// - macros which enable/disable the code for the services ------------------------------------------------------------    

// - definition of the hidden struct implementing the object ----------------------------------------------------------

enum { fatal_error_signature = 0x66 };

typedef struct
{
    uint32_t millisecondsfromstart;
    uint8_t handlertype;
    uint8_t handlererrorcode;
    uint8_t ipsr;
    uint8_t idofthelastscheduledthread;
    char forfutureuse[6];
    uint8_t signature;         // it must be fatal_error_signature
    uint8_t notusable;
} fatal_error_message_t; EO_VERIFYsizeof(fatal_error_message_t, 16)


typedef struct
{
    uint64_t                par64;
    uint16_t                par16;
} fatal_error_params_t; EO_VERIFYsizeof(fatal_error_params_t, 16)

typedef union 
{
    fatal_error_message_t   message;
    fatal_error_params_t    params;    
} fatal_error_t; EO_VERIFYsizeof(fatal_error_t, 16)


enum { threadnameofsize = 15 };
typedef struct 
{
//    osal_task_t *t;
    osal_task_id_t tid;
    char nameof[threadnameofsize];
} threadinfo_t;

enum { maxthreads = 16 };


struct EOtheFatalError_hid
{
    eObool_t                    initted;
    uint8_t                     threadcount;
    fatal_error_descriptor_t    descriptor;
    fatal_error_t               tobewritten;
    fatal_error_t               detectedfatalerror;
    eOerrmanDescriptor_t        errdes;
    threadinfo_t                threads[maxthreads];
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




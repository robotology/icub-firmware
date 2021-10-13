
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "eEsharedServices.h"
#include "osal.h"
#include "hal.h"
#include "EoError.h"
#include "EOMtask.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheFatalError.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheFatalError_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

//#define EOTHEFATALERROR_ACTIVATE_TEST


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static const char * s_fatalerror2string(fatal_error_handler_t feh);
static const char * s_get_threadstring(const char *s, osal_task_id_t tid);
static const char * s_get_irqhandlerstring(uint8_t ipsr);
static uint32_t s_getmillitime(void);
static const char * s_get_threadinfo(const fatal_error_descriptor_t *des, osal_task_id_t *tid);
static const char * s_get_fatalerrorstring(uint8_t h, uint8_t ec);

static const char * s_tid_to_threadname(osal_task_id_t tid);

static void s_test(fatal_error_descriptor_t *des);

static void s_save_standard(fatal_error_descriptor_t *des);
static void s_info_standard(EOtheFatalError *p);

static void s_save_hardfault(fatal_error_descriptor_t *des);
static void s_info_hardfault(EOtheFatalError *p);
static void s_save_mpustate(fatal_error_descriptor_t *des);
static void s_info_mpustate(EOtheFatalError *p);
static void s_set_divide_by_zero_trap(void);
static void s_set_separate_handlers(void);
static void s_disable_writebuffering(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
    
static EOtheFatalError s_eo_thefatalerror = 
{
    .initted            = eobool_false,
    .threadcount        = 0,
    .descriptor         = {0, 0, 0, 0, NULL, NULL},
    .tobewritten        = {0},
    .detectedfatalerror = {0},
    .errdes             = {0},
    .threads            = 
    {
        {0, ""}, {0, ""}, {0, ""}, {0, ""}, {0, ""}, {0, ""}, {0, ""}, {0, ""},
        {0, ""}, {0, ""}, {0, ""}, {0, ""}, {0, ""}, {0, ""}, {0, ""}, {0, ""}
    },
    .mpustate = {{0, {0, 0, 0}, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

static const char s_eobj_ownname[] = "EOtheFatalError";

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheFatalError* eo_fatalerror_Initialise(void)
{
    EOtheFatalError *p = &s_eo_thefatalerror;
    
    if(eobool_true == p->initted)
    {
        return(p);
    }
    
    s_set_divide_by_zero_trap();
    s_set_separate_handlers();
    s_disable_writebuffering();
    
//    eo_errman_Trace(eo_errman_GetHandle(), "eo_fatalerror_Initialise() starts", s_eobj_ownname); 
       
    p->initted = eobool_true;
                  
    // for the rest we rely on the static initialisation of s_eo_thefatalerror    
    
//    eo_errman_Trace(eo_errman_GetHandle(), "eo_fatalerror_Initialise() is over", s_eobj_ownname);
    
    return(p);
}
 
    
extern EOtheFatalError* eo_fatalerror_GetHandle(void)
{
    EOtheFatalError *p = &s_eo_thefatalerror;
    
    if(eobool_true == p->initted)
    {
        return(p);
    }
    
    return(NULL);
}



extern void eo_fatalerror_AtStartup(EOtheFatalError *p)
{
    if(NULL == p)
    {
        p = eo_fatalerror_Initialise();
    }
    
    uint8_t detectedsize = 0;
    // read ipc memory
    if(ee_res_OK == ee_sharserv_ipc_userdefdata_get((uint8_t*)&p->detectedfatalerror, &detectedsize, sizeof(fatal_error_t)))
    {
        // there is something. 
        // i clear ipc ram
        ee_sharserv_ipc_userdefdata_clr();        
        if((detectedsize <= sharserv_base_ipc_userdefdata_maxsize) && (fatal_error_signature == p->detectedfatalerror.message.signature))
        {
            if(fatalerror_handler_hw_HardFault == p->detectedfatalerror.message.handlertype)
            {
               s_info_hardfault(p); 
               s_info_mpustate(p);
            }
            else
            {
                s_info_standard(p);
                s_info_mpustate(p);
            }
        }                        
    } 

// test code for dive by zero    
//    volatile uint32_t value = 0;
//    volatile uint32_t tmp = 10;
//    tmp = tmp / value;  
//   
//    value = value;
  
// test code for access to non existing memory    
//    volatile unsigned int* pp;
//    volatile unsigned int n;
//    pp = (unsigned int*)0xCCCCCCCC;
//    n = *pp;
//    
//    n = n;

    
}


extern fatal_error_descriptor_t * eo_fatalerror_GetDescriptor(EOtheFatalError *p)
{
    return &s_eo_thefatalerror.descriptor; 
}

extern void eo_fatalerror_Restart(EOtheFatalError *p, fatal_error_descriptor_t *des)
{
    // must be lean and mean. we dont check vs p 
    if(NULL == des)
    {
        des = &s_eo_thefatalerror.descriptor;
    }
    
    if(fatalerror_handler_hw_HardFault == des->handlertype)
    {
        s_save_hardfault(des);
        s_save_mpustate(des);
    }
    else
    {
        s_save_standard(des);
        s_save_mpustate(des);
    }

    
    // write in ipc memory and ... restart
    ee_sharserv_ipc_userdefdata_set((uint8_t*)&s_eo_thefatalerror.tobewritten, sizeof(s_eo_thefatalerror.tobewritten));
    ee_sharserv_sys_restart();    
    
}

extern void eo_fatalerror_Test(EOtheFatalError *p, fatal_error_descriptor_t *des)
{  
    s_test(des);
}



extern void eo_fatalerror_RegisterThread(EOtheFatalError *p, void *t, const char *n)
{
    if(s_eo_thefatalerror.threadcount < maxthreads)
    {
//        s_eo_thefatalerror.threads[s_eo_thefatalerror.threadcount].t = t;
        osal_task_id_get(t, &s_eo_thefatalerror.threads[s_eo_thefatalerror.threadcount].tid);
        snprintf(s_eo_thefatalerror.threads[s_eo_thefatalerror.threadcount].nameof, threadnameofsize, "%s", n);
        s_eo_thefatalerror.threadcount ++;
    }    
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static const char * s_fatalerror2string(fatal_error_handler_t feh)
{    
    static const char * s_fatalerror_strings[] =
    {
        "none",
        "unknown",
        "HAL",
        "OSAL",
        "IPAL",   
        "hw_NMI",    
        "hw_HardFault",
        "hw_MemManage",
        "hw_BusFault",
        "hw_UsageFault",
        "hw_DebugMon",
        "hw_Default"
    };  EO_VERIFYsizeof(s_fatalerror_strings, fatal_error_handler_numbersof*sizeof(const char *))  
    
    const char * ret = s_fatalerror_strings[fatalerrr_handler_unknown];
    
    if(feh < fatal_error_handler_numbersof)
    {
        ret = s_fatalerror_strings[feh];
    }
        
    return ret;    
}

#if 0

        osal_task_t * osalt = NULL;
    if((fatalerror_handler_osal == des->handlertype) && (NULL != des->param))
    {
        osalt = des->param;
        osal_task_id_get(osalt, &tid);
    }
    else
    {
        osalt = osal_task_get(osal_callerAUTOdetect);
        osal_task_id_get(osalt, &tid);
    }
    
    //uint8_t irqh = (hal_true == hal_sys_irq_is_running()) ? 1 : 0;
       
    s_eo_thefatalerror.tobewritten.message.millisecondsfromstart = s_getmillitime();
    s_eo_thefatalerror.tobewritten.message.handlertype = des->handlertype;
    s_eo_thefatalerror.tobewritten.message.handlererrorcode = des->handlererrorcode;
    s_eo_thefatalerror.tobewritten.message.ipsr = hal_sys_get_IPSR();
    s_eo_thefatalerror.tobewritten.message.idofthelastscheduledthread = tid;

    // get name of the scheduled thread
    EOMtask * tsk = (EOMtask*)osal_task_extdata_get(osalt);
    thrname = eom_task_GetName(tsk);
    
    if(NULL == tsk)
    {
        for(int i=0; i<16; i++)
        {
            if(s_eo_thefatalerror.threads[i].t == osalt)
            {
                thrname = s_eo_thefatalerror.threads[i].nameof;                
            }
        }
    }

#endif

static const char * s_get_threaname_mode1(fatal_error_descriptor_t *des)
{
   // get name of the scheduled thread. the code is safe also if we have NULL
    EOMtask * tsk = (EOMtask*)osal_task_extdata_get(osal_task_get(osal_callerAUTOdetect));
    return(eom_task_GetName(tsk));    
}

extern void * oosiit_tsk_self(void);
extern uint8_t oosiit_tsk_get_id(void * tp);

static const char * s_get_threaname_mode2(const fatal_error_descriptor_t *des, osal_task_id_t *ptid)
{   
//    osal_task_id_t tid1 = 0;
//    osal_task_id_get(osal_task_get(osal_callerAUTOdetect), &tid1); 
//    volatile  osal_task_id_t tid2 = tid1;
//    tid2 = tid2;    
    
    osal_task_id_t tid = oosiit_tsk_get_id(oosiit_tsk_self());    
    *ptid = tid;
    
    return s_tid_to_threadname(tid);

}

static const char * s_get_threadinfo(const fatal_error_descriptor_t *des, osal_task_id_t *tid)
{
    // how to get the name of the thread?
    // there are several ways.
    // the quickest and cleanest way is to get it directly from the EOMtask through the osal_task_t.
    // see function s_get_threaname_mode1()
    // however, i have experienced that this mode does not work when we have a stack overflow
    // because the stack memory is close by the RAM used for description of osal_task_t and EOMtask.
    // so, i have implemented a second way, where we look for a private look up table
    
    return(s_get_threaname_mode2(des, tid));
}

static const char * s_tid_to_threadname(osal_task_id_t tid)
{
    const char * thrname = NULL; 
        
    for(uint8_t i=0; i<maxthreads; i++)
    {
        if(s_eo_thefatalerror.threads[i].tid == tid)
        {
            thrname = s_eo_thefatalerror.threads[i].nameof;                
        }
    }
    
    if((NULL == thrname) && (1 == tid))
    {
        thrname = "tIDLE";
    }
    else if((NULL == thrname) && (2 == tid))
    {
        thrname = "tINIT";
    }
    else if(NULL == thrname)
    {
        thrname = "tUNKN";
    }

   return thrname;        
}

static const char * s_get_irqhandlerstring(uint8_t ipsr)
{
    enum { ss = 16 };
    static const char *s_irqh[ss] = 
    {
        "none",
        "Reserved",
        "NMI",
        "HardFault",
        "MemManage",
        "BusFault",
        "UsageFault",
        "Reserved", "Reserved", "Reserved", "Reserved", // 7-10 = Reserved
        "SVCall",
        "Reserved", "Reserved",
        "PendSV",
        "SysTick" // 15 = SysTick
    };
    
    if(ipsr < ss)
    {
        return s_irqh[ipsr];
    }
    
    return "IRQx";
}

static const char * s_get_fatalerrorstring(uint8_t h, uint8_t ec)
{
    enum { dd = 6 };
    static const char *s_ose[dd] = 
    {
        "osal_generic",
        "osal_wrongcfg",
        "osal_heapisover",
        "osal_wrongpar",
        "osal_unsupp",
        "osal_stackovf"
    };
    
    if(fatalerror_handler_osal != h)
    {
        return "see TBL";
    }
    
    if((ec >= 0xE0) && (ec <= 0xE5))
    {
        return s_ose[ec-0xE0];
    }
    
    return "see TBL";
}


static const char * s_get_threadstring(const char *s, osal_task_id_t tid)
{
    const char *name = s;
    // or even better: we may match the start of the string with our own copy inside s_eo_thefatalerror.threads
    
    return s;
}

static uint32_t s_getmillitime(void)
{
    // using oosiit_time and osal_info_get_tick() we DONT call the SVC handler, 
    // so we have better possibility of success even in case of fatal disaster...
    extern volatile uint64_t oosiit_time;
    uint64_t ms = oosiit_time * osal_info_get_tick() / 1000;
    
    return ms;
}

static void s_save_standard(fatal_error_descriptor_t *des)
{
    osal_task_id_t tid = 0;    
    const char *thrname = s_get_threadinfo(des, &tid);
    
    s_eo_thefatalerror.tobewritten.message.millisecondsfromstart = s_getmillitime();
    s_eo_thefatalerror.tobewritten.message.handlertype = des->handlertype;
    s_eo_thefatalerror.tobewritten.message.handlererrorcode = des->handlererrorcode;
    s_eo_thefatalerror.tobewritten.message.ipsr = hal_sys_get_IPSR();
    s_eo_thefatalerror.tobewritten.message.idofthelastscheduledthread = tid;    
    snprintf(s_eo_thefatalerror.tobewritten.message.forfutureuse, sizeof(s_eo_thefatalerror.tobewritten.message.forfutureuse), "%s", thrname);
    s_eo_thefatalerror.tobewritten.message.signature = fatal_error_signature;    
}

static void s_info_standard(EOtheFatalError *p)
{
    // it is of the correct size. i use it to send a diagnostic message
    uint16_t par16 = 0;
    uint64_t par64 = p->detectedfatalerror.params.par64;             

    p->errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
    p->errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    p->errdes.sourceaddress    = 0;
    p->errdes.par16            = par16;
    p->errdes.par64            = par64;
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, "RESTARTED after FATAL error", s_eobj_ownname, &p->errdes);
    
    // further information is in here
    char str[64] = {0};
    
    snprintf(str, sizeof(str), "@ %d ms", 
            p->detectedfatalerror.message.millisecondsfromstart
    );
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);
    
    snprintf(str, sizeof(str), "handler %s, code 0x%x", 
            s_fatalerror2string(p->detectedfatalerror.message.handlertype),
            p->detectedfatalerror.message.handlererrorcode
    );
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);            


    snprintf(str, sizeof(str), "type %s", 
            s_get_fatalerrorstring(p->detectedfatalerror.message.handlertype, p->detectedfatalerror.message.handlererrorcode)
    );
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);                 

          
    
    osal_task_id_t tid = p->detectedfatalerror.message.idofthelastscheduledthread;  
    uint8_t ipsr = p->detectedfatalerror.message.ipsr;            
    const char * nameofthread = s_get_threadstring(p->detectedfatalerror.message.forfutureuse, tid);
    const char * nameofirqhan = s_get_irqhandlerstring(ipsr);
     
    snprintf(str, sizeof(str), "IRQHan %s Thread %s", 
            nameofirqhan,
            nameofthread
    );            
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);
    
    snprintf(str, sizeof(str), "ipsr %d, tid %d", 
            ipsr,
            tid
    );
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);    
}

static void s_set_divide_by_zero_trap(void)
{
    volatile uint32_t *pCCR = (volatile uint32_t *) 0xE000ED14;
    (*pCCR) |= 0x00000010;    
}

static void s_set_separate_handlers(void)
{    
    volatile uint32_t *pSHCSR = (volatile uint32_t *) 0xE000ED24;
    #define SCB_SHCSR_USGFAULTENA_Msk (0x01 << 18)
    #define SCB_SHCSR_BUSFAULTENA_Msk (0x01 << 17)
    #define SCB_SHCSR_MEMFAULTENA_Msk (0x01 << 16)
    (*pSHCSR) |= (SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk);
}

static void s_disable_writebuffering(void)
{
    volatile uint32_t *ACTLR = (volatile uint32_t *)0xE000E008;
    (*ACTLR) |= 2;
}

static void s_save_hardfault(fatal_error_descriptor_t *des)
{
//    s_save_standard(des);  
    osal_task_id_t tid = 0;    
    const char *thrname = s_get_threadinfo(des, &tid);
    
    s_eo_thefatalerror.tobewritten.message.millisecondsfromstart = s_getmillitime();
    s_eo_thefatalerror.tobewritten.message.handlertype = des->handlertype;
    s_eo_thefatalerror.tobewritten.message.handlererrorcode = 0; // 1 B
    s_eo_thefatalerror.tobewritten.message.ipsr = hal_sys_get_IPSR();
    s_eo_thefatalerror.tobewritten.message.idofthelastscheduledthread = tid;    
    
    
    volatile uint32_t CFSR = *(volatile uint32_t *) 0xE000ED28;

    s_eo_thefatalerror.tobewritten.message.forfutureuse[0] = CFSR & 0xff; 
    s_eo_thefatalerror.tobewritten.message.forfutureuse[1] = (CFSR >> 8) & 0xff;
    s_eo_thefatalerror.tobewritten.message.forfutureuse[2] = (CFSR >> 16) & 0xff;
    s_eo_thefatalerror.tobewritten.message.forfutureuse[3] = (CFSR >> 24) & 0xff;
    
    //volatile uint32_t HFSR = *(volatile uint32_t *) 0xE000ED2C;
       
    s_eo_thefatalerror.tobewritten.message.signature = fatal_error_signature;     
}

static void s_info_hardfault(EOtheFatalError *p)
{
    // it is of the correct size. i use it to send a diagnostic message
    uint16_t par16 = 0;
    uint64_t par64 = p->detectedfatalerror.params.par64;             

    p->errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
    p->errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    p->errdes.sourceaddress    = 0;
    p->errdes.par16            = par16;
    p->errdes.par64            = par64;
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, "RESTARTED after FATAL error", s_eobj_ownname, &p->errdes);
    
    // further information is in here
    char str[64] = {0};
    
    snprintf(str, sizeof(str), "@ %d ms", 
            p->detectedfatalerror.message.millisecondsfromstart
    );
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);
    
    snprintf(str, sizeof(str), "handler %s, code 0x%x", 
            s_fatalerror2string(p->detectedfatalerror.message.handlertype),
            p->detectedfatalerror.message.handlererrorcode
    );
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);            


    snprintf(str, sizeof(str), "type %s", 
            s_get_fatalerrorstring(p->detectedfatalerror.message.handlertype, p->detectedfatalerror.message.handlererrorcode)
    );
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);                 
              
    osal_task_id_t tid = p->detectedfatalerror.message.idofthelastscheduledthread;  
    uint8_t ipsr = p->detectedfatalerror.message.ipsr;            
    const char * nameofthread = s_tid_to_threadname(tid);
    const char * nameofirqhan = s_get_irqhandlerstring(ipsr);
     
    snprintf(str, sizeof(str), "IRQHan %s Thread %s", 
            nameofirqhan,
            nameofthread
    );            
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);
    
         
    snprintf(str, sizeof(str), "ipsr %d, tid %d", 
            ipsr,
            tid
    );
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes); 

    uint32_t CFSR = ((uint32_t)(p->detectedfatalerror.message.forfutureuse[0])       )    | 
                    ((uint32_t)(p->detectedfatalerror.message.forfutureuse[1]) << 8  )    |
                    ((uint32_t)(p->detectedfatalerror.message.forfutureuse[2]) << 16 )    |
                    ((uint32_t)(p->detectedfatalerror.message.forfutureuse[3]) << 24 )    ;
    snprintf(str, sizeof(str), "CFSR 0x%x", 
            CFSR
    );            
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);    
}

static void s_save_mpustate(fatal_error_descriptor_t *des)
{
    volatile fatal_error_mpustate_cm4_t *pHFmore = (volatile fatal_error_mpustate_cm4_t*) 0x10000000;
    volatile uint32_t * hardfault_args = (volatile uint32_t *) des->mpucontext;

    pHFmore->header.signature = fatal_error_signature;
    pHFmore->header.dummy[0] = 0;
    pHFmore->header.dummy[1] = 1;
    pHFmore->header.dummy[2] = 2;
    pHFmore->header.tbd0 = 0;
    
    if(NULL == hardfault_args)
    {
        pHFmore->context.r0 = pHFmore->context.r1 = pHFmore->context.r2 = pHFmore->context.r3 = 
        pHFmore->context.r12 = pHFmore->context.lr = pHFmore->context.pc = pHFmore->context.psr = 0;
    }
    else 
    {        
        pHFmore->context.r0 = hardfault_args[0];
        pHFmore->context.r1 = hardfault_args[1];
        pHFmore->context.r2 = hardfault_args[2];
        pHFmore->context.r3 = hardfault_args[3];        
        pHFmore->context.r12 = hardfault_args[4];
        pHFmore->context.lr = hardfault_args[5];
        pHFmore->context.pc = hardfault_args[6];
        pHFmore->context.psr = hardfault_args[7];
    }
    
    pHFmore->sysregs.ICSR = (*((volatile uint32_t *)(0xE000ED04)));
    pHFmore->sysregs.SHCSR = (*((volatile uint32_t *)(0xE000ED24)));
    pHFmore->sysregs.CFSR = (*((volatile uint32_t *)(0xE000ED28)));
    pHFmore->sysregs.HFSR = (*((volatile uint32_t *)(0xE000ED2C)));
    pHFmore->sysregs.DFSR = (*((volatile uint32_t *)(0xE000ED30)));   
    pHFmore->sysregs.MMFAR = (*((volatile uint32_t *)(0xE000ED34)));
    pHFmore->sysregs.BFAR = (*((volatile uint32_t *)(0xE000ED38)));
    pHFmore->sysregs.AFSR = (*((volatile uint32_t *)(0xE000ED3C)));
}

static void s_info_mpustate(EOtheFatalError *p)
{
    // it is of the correct size. i use it to send a diagnostic message
    uint16_t par16 = 0;
    uint64_t par64 = p->detectedfatalerror.params.par64;     

    volatile fatal_error_mpustate_cm4_t *pHFmore = (volatile fatal_error_mpustate_cm4_t*) 0x10000000;    

    p->errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
    p->errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    p->errdes.sourceaddress    = 0;
    p->errdes.par16            = par16;
    p->errdes.par64            = par64;
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, "MORE INFO", s_eobj_ownname, &p->errdes);

    // further information is in here
    char str[64] = {0};
        
    if(fatal_error_signature != pHFmore->header.signature)
    {
        snprintf(str, sizeof(str), "mpu state invalid sign = 0x%08x",
            pHFmore->header.signature
        );
        
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);        
    }
    else
    {
        // caveat the string can be at most 48 characters 
        
        // System control registers. 
        
        snprintf(str, sizeof(str), "ICSR = 0x%08x SHCSR = 0x%08x", 
            pHFmore->sysregs.ICSR, pHFmore->sysregs.SHCSR
        );        
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);   
    
        snprintf(str, sizeof(str), "CFSR = 0x%08x HFSR = 0x%08x", 
            pHFmore->sysregs.CFSR, pHFmore->sysregs.HFSR
        );        
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);   

        snprintf(str, sizeof(str), "DFSR = 0x%08x MMFAR = 0x%08x", 
            pHFmore->sysregs.DFSR, pHFmore->sysregs.MMFAR
        );        
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);   
 
        snprintf(str, sizeof(str), "BFAR = 0x%08x AFSR = 0x%08x", 
            pHFmore->sysregs.BFAR, pHFmore->sysregs.AFSR
        );        
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);   
           
        // execution registers

        snprintf(str, sizeof(str), "r0 = 0x%08x r1 = 0x%08x", 
            pHFmore->context.r0, pHFmore->context.r1
        );        
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);   
    
        snprintf(str, sizeof(str), "r2 = 0x%08x r3 = 0x%08x", 
            pHFmore->context.r2, pHFmore->context.r3
        );        
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);   

        snprintf(str, sizeof(str), "r12 = 0x%08x lr = 0x%08x", 
            pHFmore->context.r12, pHFmore->context.lr
        );        
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);   
 
        snprintf(str, sizeof(str), "pc = 0x%08x psr = 0x%08x", 
            pHFmore->context.pc, pHFmore->context.psr
        );        
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &p->errdes);   
        
    }
    
    // and now i reset memory bank area
    memset((fatal_error_mpustate_cm4_t *)pHFmore, 0, sizeof(fatal_error_mpustate_cm4_t));
    
}



// error tests

#if !defined(EOTHEFATALERROR_ACTIVATE_TEST)

static void s_test(fatal_error_descriptor_t *des)
{
}

#else

//#define USE_CBKMANAGER
#define USE_DELAY

//#define USE_IRQ_HANDLER_RTC_ALARM
//#define USE_HALERROR
//#define USE_OSALERROR
//#define USE_BIGMALLOC
//#define USE_BIGSTACK
//#define USE_DIVIDEBYZERO
#define USE_NULLPTR

#include "EOtimer.h"
#include "EOVtheCallbackManager.h"
#include "EOtheMemoryPool.h"

extern void RTC_Alarm_IRQHandler();
extern const hal_core_cfg_t hal_cfg;
extern const osal_cfg_t osal_cfg;

uint32_t useme(uint32_t n)
{
    volatile uint64_t buffer[2000] = {0};
    
    for(int i = 0; i<2000; i++)
    {
        buffer[i] = 666;
    }
    
    return n;
}

void callback(void* par)
{
#if defined(USE_IRQ_HANDLER_RTC_ALARM)
    RTC_Alarm_IRQHandler();
#elif defined(USE_HALERROR)
    hal_cfg.basecfg.extfn.usr_on_fatal_error(hal_fatalerror_runtimefault, "hal");
#elif defined(USE_OSALERROR)    
    osal_cfg.extfn.usr_on_fatal_error(NULL, osal_error_stackoverflow, "osal");
#elif defined(USE_BIGMALLOC)
    uint8_t *p = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 10000, 1);
    par = p;
#elif defined(USE_BIGSTACK)
    static volatile uint32_t me = 1000;
    me++;
    useme(me);   
    osal_task_id_t tid = 0;
    osal_task_t * osalt = osal_task_get(osal_callerAUTOdetect);
    osal_task_id_get(osalt, &tid);
    
#elif defined(USE_DIVIDEBYZERO)

   static volatile uint32_t res = 32;
   static volatile uint32_t div = 0;
   volatile uint32_t re = 0;
   re = res / div;
   re = res;

#elif defined(USE_NULLPTR)

    EOtheFatalError *pp = NULL;
    for(int i=0; i<2000; i++)
    {

        {
            memcpy(pp->threads[i].nameof, "CIIIIIIIIIIIIIIIIIIIIIIII", 50);                
        }
    }

#endif    
}

void *cbkpar = NULL;

EOtimer *s_timer = NULL;

static EOaction_strg s_astrg = {0};
static EOaction *s_act = (EOaction*)&s_astrg;

static const eOreltime_t s_delay = 10*EOK_reltime1sec;

static volatile eOabstime_t s_after = 20*EOK_reltime1sec;

static void s_test(fatal_error_descriptor_t *des)
{
#if defined(USE_CBKMANAGER)    
    if(NULL == s_timer)
    {
        s_timer = eo_timer_New();
    }
    eo_action_SetCallback(s_act, callback, cbkpar, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
    eo_timer_Start(s_timer, eok_abstimeNOW, s_delay, eo_tmrmode_ONESHOT, s_act);  
#elif defined(USE_DELAY)
    
    volatile eOabstime_t now = osal_system_abstime_get();
    if(now > s_after)
    {
        callback(NULL);
    }

#endif        
}

#endif

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


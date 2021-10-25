
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _CMSISOS2_H_
#define _CMSISOS2_H_

#ifdef  __cplusplus
extern "C"
{
#endif

    // macro used to change the behaviour of the original cmsis-rtos2 using rtx
    #if !defined(CMSISOS2_ICUB_CHANGES)
    #define CMSISOS2_ICUB_CHANGES
    #endif

    // original api from cmsis-rtos2 using rtx
    #include "../src/cmsis5/cmsis/RTOS2/Include/cmsis_os2.h"
    #include "../src/cmsis5/cmsis/RTOS2/RTX/Include/rtx_os.h"

    // extras
    typedef uint64_t cmsisos2_abstime_t;    // expressed in us
    typedef uint32_t cmsisos2_reltime_t;    // expressed in us
    
    
    
    extern const cmsisos2_reltime_t cmsisos_reltimezero;
    extern const cmsisos2_reltime_t cmsisos_reltime1microsec;
    extern const cmsisos2_reltime_t cmsisos_reltime1millisec;
    extern const cmsisos2_reltime_t cmsisos_reltime1second;
    extern const cmsisos2_reltime_t cmsisos_reltimeforever;

    cmsisos2_abstime_t cmsisos2_sys_abstime_milliresolution(void);
    cmsisos2_abstime_t cmsisos2_sys_abstime(void);
    uint32_t cmsisos2_sys_reltime2tick(cmsisos2_reltime_t t);
    
    void * cmsisos2_memory_new(size_t size);
    void * cmsisos2_memory_realloc(void *p, size_t size);
    void cmsisos2_memory_delete(void *p);



#ifdef  __cplusplus
}
#endif

#endif  // include-guard


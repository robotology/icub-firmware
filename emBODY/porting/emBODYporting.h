/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EMBODYPORTING_H_
#define _EMBODYPORTING_H_

#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       emBODYporting.h
    @brief      This header file cevcervcwer.
    @author     marco.accame@iit.it
    @date       11/03/2011
**/

/** @defgroup cecedcedc complier specifics for emBODY environment
    The embENV allows ...... 
 
    @todo acemor-facenda: do documentation.
    

    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"


// - public #define  --------------------------------------------------------------------------------------------------

#if defined(_MSC_VER)
    // msc does not support c99, thus inline must be redefined as __inline
    #define EO_extern_inline       extern __inline
    #define EO_static_inline       static __inline
    //#define inline          __inline
    // msc does not support c99, thus designated initializers in structs (i.e., .item = val) must be moved to the old way
    #define EO_INIT(f) 
    //#pragma pack(4) 
    #pragma pack(8) 
    #define snprintf        sprintf_s
	#define float32_t       float
	#define __weak	
    #define EO_TAILOR_CODE_FOR_WINDOWS
    #define EO_WARNING(a)   __pragma(message("EOWARNING-> "##a))
//	#define EOCFG_NVSEP_MC_OVERRIDE
//	#define EOCFG_NVSEP_SK_OVERRIDE
//	#define EOCFG_NVSEP_AS_OVERRIDE
//#pragma message(a)
#elif defined(__linux__) & defined(__GNUC__)
    // gcc-linux
    //#define inline          inline
    #define EO_extern_inline       static inline
    #define EO_static_inline       static inline
    #define EO_INIT(f)
    #pragma pack(8)
    #define snprintf        snprintf
    #define float32_t       float
    #define __weak          __attribute__((weak))
    #define EO_TAILOR_CODE_FOR_LINUX
    #define EO_WARNING(a)   _Pragma(message("EOWARNING-> "##a))
    #define OVERRIDE_eo_receiver_callback_incaseoferror_in_sequencenumberReceived
    #define _PEDANT_WARNING_ON_COMPILATION_CALLBACK_
    #define _STATS_DEBUG_FOR_CYCLE_TIME_

#elif defined(__arm__)
    // __arm__ or __ARMCC_VERSION are always defined in armcc compiler
    #define EO_extern_inline        extern inline
    #define EO_static_inline        static inline
    // other compilers which support c99 can keep the designated initializers in structs
    #define EO_INIT(f)      f =
    #pragma pack(8)
    // or pack(4) ???
    #define snprintf        snprintf  
    #define float32_t       float
    #define EO_TAILOR_CODE_FOR_ARM
#elif defined(_DSPIC_) & defined(__GNUC__)
    // for dspic ...
    #define EO_extern_inline        extern inline
    #define EO_static_inline        static inline
    // other compilers which support c99 can keep the designated initializers in structs
    #define EO_INIT(f)      f =
    //#pragma pack(4)
    // or pack(8) ???
    //#define snprintf        snprintf   
    //#define stdint    dspic_stdint
    #define EO_TAILOR_CODE_FOR_DSPIC
    #define __weak      __attribute__((__weak__))
#elif defined(__APPLE__)
    #define EO_extern_inline       static inline
    #define EO_static_inline       static inline
    #define EO_INIT(f)
    #pragma pack(8)
    #define snprintf        snprintf
    #define float32_t       float
    #define __weak __attribute__((weak))
#else
    #error architecture not defined 
#endif


#define __emBODYportingVERIFYsizeof(sname, ssize)    typedef uint8_t GUARD##sname[ ( ssize == sizeof(sname) ) ? (1) : (-1)];


// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef int8_t emBODYporting_enum08_t;

// - declaration of extern public functions ---------------------------------------------------------------------------

 

/** @}            
    end of group cecedcedc 
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




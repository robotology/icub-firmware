/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Davide Pollarolo
 * email:   davide.pollarolo@iit.it
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
#ifndef _EOMCCONFIGURATIONS_H_
#define _EOMCCONFIGURATIONS_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoMCConfigurations.h
    @brief      This header file contains the definitions of the allowed motion control endpoint configuration.
				This structure are used to initialize (1) the Motion Control endpoint (2) and the Motion Control service with the
				desired configuration received in a ROP from RobotInterface
    @author     marco.accame@iit.it
    @date       Nov 10 2014
**/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - public #define  --------------------------------------------------------------------------------------------------

// these macros describe the partition of the 32 bits used by the errorcode into a type and a number.
// so far we use 8 bits to define a type (e.g., board system) and 24 bits to define a number (e.g., missing memory).
 
//these macros describes the partition of the 16 bits used by the configuration code into a motion control type and a number.

#define EOMCCONFIG_TYPE_MASK    0xFF00
#define EOMCCONFIG_VALUE_MASK   0x00FF
#define EOMCCONFIG_TYPE_SHIFT   8


#define EOMCCONFIG_TYPE_DUMMY  	EOK_uint08dummy
#define EOMCCONFIG_VALUE_DUMMY  EOK_uint08dummy
#define EOMCCONFIG_CODE_DUMMY   EOK_uint16dummy
#define EOMCCONFIG_BOARD_DUMMY  EOK_uint08dummy

// this macro-function should be used for constant initialisation, for all other cases use: eomcconfig_code_get() function
#define EOMCCONFIGCODE(type, val)   ((val) & EOMCCONFIG_VALUE_MASK) | (((type) << EOMCCONFIG_TYPE_SHIFT) & EOMCCONFIG_TYPE_MASK)


// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef uint32_t eOmcconfig_code_t
    @brief      generic type used to specify an error code. it is formed of 16 bits, of which the mbb is the type, 
                and the others are a configuration inside that type. We represent the bits of the type with type eOmcconfig_type_t,
                whereas we represent the bits of the configuration with the enum type specific of the type.
                As an example, if the type is eOmcconfig_type_mc4plus, then the enum type for the values is eOmcconfig_config_MC4PLUS_t
                
                We also use a generic type eOmcconfig_value_t for generic function parameter or return value.
 **/
typedef uint16_t eOmcconfig_code_t;


/** @typedef    typedef enum eOmcconfig_type_t
    @brief      it contains the type in the msb in eOmcconfig_code_t. if you want to add a new type: use CamelCaseConvention for the 
                enumerative label, add it just before eOmcconfig_type_dummy.        
 **/
      
typedef enum
{
    eOmcconfig_type_undefined	= 0,
    eOmcconfig_type_2foc		= 1,
	eOmcconfig_type_mc4can		= 2,
    eOmcconfig_type_mc4plus		= 3,
    // add a new type in here. remember to increment eOmcconfig_type_numberof 
    eOmcconfig_type_dummy       = EOMCCONFIG_TYPE_DUMMY   /**< used to express a dummy type. dont count it for eOmcconfig_type_numberof */
} eOmcconfig_type_t;

enum { eOmcconfig_type_numberof = 3 };


/** @typedef    typedef uint32_t eOmcconfig_value_t
    @brief      generic value for use as a function parameter. For usage, you must prefer the enumerative type relevant to the configuration.         
 **/
typedef uint16_t eOmcconfig_value_t;


/** @typedef    typedef enum eOmcconfig_value_2FOC_t
    @brief      it contains the values of the eOmcconfig_type_2FOC type. In case you need a new value: add it
                in consecutive mode without holes and increment eOmcconfig_value_2FOC_numberof. In case you need to remove a value, don't
                remove the number but simply rename it: that will help detecting old boards sending pairs {type, value} not supported yet.  
 **/
typedef enum
{
    eOmcconfig_value_2FOC_unspecified	= 0
	
} eOmcconfig_value_2FOC_t;

enum { eOmcconfig_value_2FOC_numberof = 1 };


/** @typedef    typedef enum eOmcconfig_value_MC4CAN_t
    @brief      it contains the values of the eOmcconfig_type_MC4CAN type. In case you need a new value: add it
                in consecutive mode without holes and increment eOmcconfig_value_CAN_numberof. In case you need to remove a value, don't
                remove the number but simply rename it: that will help detecting old boards sending pairs {type, value} not supported yet.  
 **/
typedef enum
{
    eOmcconfig_value_MC4CAN_unspecified	= 0
	
} eOmcconfig_value_MC4CAN_t;

enum { eOmcconfig_value_MC4CAN_numberof = 1 };



/** @typedef    typedef enum eOmcconfig_value_MC4PLUS_t
    @brief      it contains the values of the eOmcconfig_type_MC4PLUS type. In case you need a new value: add it
                in consecutive mode without holes and increment eOmcconfig_value_MC4PLUS_numberof. In case you need to remove a value, dont
                remove the number but simply rename it: that will help detecting old boards sending pairs {type, value} not supported yet.  
 **/
typedef enum
{
    eOmcconfig_value_MC4PLUS_unspecified	       = 0,
    eOmcconfig_value_MC4PLUS_experimental          = 1,
    eOmcconfig_value_MC4PLUS_experimental_2joints  = 2,
    //only v3 head boards inserted at the moment (change the name later on...)
    eOmcconfig_value_MC4PLUS_b0	                   = 3,
    eOmcconfig_value_MC4PLUS_b1	                   = 4, 
    eOmcconfig_value_MC4PLUS_b7	                   = 5,
    eOmcconfig_value_MC4PLUS_b9	                   = 6,
    eOmcconfig_value_MC4PLUS_1b4                   = 7,
    eOmcconfig_value_MC4PLUS_CER_wrist             = 8
	
} eOmcconfig_value_MC4PLUS_t;

enum { eOmcconfig_value_MC4PLUS_numberof = 9 };
/*
typedef enum
{
    eomcserv_type_undefined     = 0,
    eomcserv_type_2foc          = 1,
    eomcserv_type_mc4can        = 2,
    eomcserv_type_mc4plus       = 3        
} eOmcserv_type_t;
*/

/* actuators mapping */
typedef struct
{   // mapped as eOcanmap_entitylocation_t
    uint8_t     port        : 1;        // 0 is can1, 1 is can2. not used if onboard is 1.
    uint8_t     addr        : 4;        // if onboard is 1, then it is the index used by hal, otherwise the can address
    uint8_t     insideindex : 2;    // in case of can it is the inside index (0 or 1) of where the motor/joint is. otherwise it is 2
    uint8_t     onboard     : 1;        // if 1 is onboard pwm otherwise is on can
} eOmcconfig_pwm_mapping_t;

typedef struct
{  
    uint8_t     dummy0   : 1;       
    uint8_t     index    : 4;       // it is the index used by hal,
    uint8_t     dummy1   : 2;       
    uint8_t     type     : 1;       // if 1 is local otherwise is on can
} eOmcconfig_act_local_mapping_t;


typedef struct
{   // mapped as eOcanmap_entitylocation_t
    uint8_t     port        : 1;        // 0 is can1, 1 is can2. not used if onboard is 1.
    uint8_t     addr        : 4;        // if onboard is 1, then it is the index used by hal, otherwise the can address
    uint8_t     insideindex : 2;    // in case of can it is the inside index (0 or 1) of where the motor/joint is. otherwise it is 2
    uint8_t     type        : 1;        // if 1 is onboard  otherwise is on can
} eOmcconfig_act_oncan_mapping_t;

typedef struct
{   // mapped as eOcanmap_entitylocation_t
    uint8_t     dummy   : 7;       
    uint8_t     type    : 1;         // if 1 is onboard  otherwise is on can
} eOmcconfig_act_any_mapping_t;


typedef union
{   // mapped as eOcanmap_entitylocation_t
    eOmcconfig_act_any_mapping_t      any;
    eOmcconfig_act_local_mapping_t    local;
    eOmcconfig_act_oncan_mapping_t    oncan;    
} eOmcconfig_act_mapping_t;           EO_VERIFYsizeof(eOmcconfig_act_mapping_t, 1);

/* ------------------------ */

/* encoders mapping */
typedef struct
{
    uint8_t     etype       : 4;      // 0 aea, 1 amo, 2 incr, 3 adc, 4 mais, etc. 
    uint8_t     index       : 4;      // 0, 1, 2, 3, 4, etc where the number is referred to the hal enum specified by the type.
    uint8_t     enc_joint   : 3;      // 0...6 joint index referred to the joints list inside the eoappencodersreader
    uint8_t     pos_type    : 2;      // 0...3 detected position type : 0 joint, 1 rotor, 2 unspecified...
    uint8_t     isthere     : 1;      // 0 not, 1 is there
    uint8_t     dummy       : 2;
    // example: aea encoder on port p6 or the ems is etype=0, index=0
} eOmcconfig_enc_mapping_t;
//we'll need also an external mapping (encoder on CAN, e.g MAIS) but we'll resolve it later on

/* ------------------------ */

/* joint/motor complete config */
typedef struct
{
    eOmcconfig_enc_mapping_t    encoder;
    eOmcconfig_enc_mapping_t    extra_encoder;  
    eOmcconfig_act_mapping_t    actuator;
    uint8_t                     dummy[3];
} eOmcconfig_jomo_cfg_t;        EO_VERIFYsizeof(eOmcconfig_jomo_cfg_t, 8);

enum { eomcconfig_jomo_maxnumberof = 12 };

/* ------------------------ */

/* motion control service complete config */
typedef struct
{
    eOmcconfig_type_t       type;
    uint8_t                 jomosnumber; // so far we have number of motors = number of joints 
    uint16_t                ffu;
    eOmcconfig_jomo_cfg_t   jomos[eomcconfig_jomo_maxnumberof];
} eOmcconfig_cfg_t;         // EO_VERIFYsizeof(eOmcconfig_cfg_t, 56); why is it complaining?


/* ------------------------ */

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------


extern const eOmcconfig_code_t eOmcconfig_code_dummy;     // = eOmcconfig_CODE_DUMMY
extern const eOmcconfig_value_t eOmcconfig_value_dummy;   // = eOmcconfig_VALUE_DUMMY


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern eOmcconfig_code_t eOmcconfig_configuration_get(eOmcconfig_type_t type, eOmcconfig_value_t config_val)
    @brief      it retrieves the complete code given a type and a value.
    @param      type            the type
    @param      config_value    the value. Use relevant values for the enum of the specific type:e.g. eOmcconfig_value_MC4PLUS_t, 
                                et cetera.
    @return     the valid config or eOmcconfig_code_dummy in case of wrong parameters.
 **/
extern eOmcconfig_code_t eOmcconfig_code_get(eOmcconfig_type_t type, eOmcconfig_value_t val); 


/** @fn         extern eOmcconfig_type_t eOmcconfig_code2type(eOmcconfig_code_t code)
    @brief      it retrieves the type given a code.
    @param      code            the code obtained from a message or from function eOmcconfig_code_get(). 

    @return     the valid type or eOmcconfig_type_dummy in case of unsupported code.
 **/
extern eOmcconfig_type_t eOmcconfig_code2type(eOmcconfig_code_t code); 


/** @fn         extern eOmcconfig_value_t eOmcconfig_code2value(eOmcconfig_code_t code)
    @brief      it retrieves the configuration value given a code.
    @param      code            the configuration code obtained from a message or from function eOmcconfig_code_get(). 

    @return     the valid configuration value or eOmcconfig_value_dummy in case of unsupported code.
 **/
extern eOmcconfig_value_t eOmcconfig_code2value(eOmcconfig_code_t code); 


/** @fn         extern eOmcconfig_jomo_cfg_t* eOmcconfig_code2config(eOmcconfig_code_t code)
    @brief      it retrieves the configuration reference given an error code.
    @param      code            the configuration code obtained from a message or from function eOmcconfig_code_get(). 

    @return     the valid pointer or NULL in case of unsupported code.
 **/
extern const eOmcconfig_jomo_cfg_t* eOmcconfig_code2config(eOmcconfig_code_t code);


/** @fn         extern eOmcconfig_value_t eOmcconfig_string2value(const char * str, eOmcconfig_type_t type)
    @brief      it retrieves the configuration value given a string and a type.
    @param      str            the string representing the configuration (being parsed in the XML files from RobotInterface)
    @param      type           the configuration type

    @return     the valid value or eOmcconfig_value_dummy in case of unsupported code.
 **/
extern eOmcconfig_value_t eOmcconfig_string2value(const char * str, eOmcconfig_type_t type);

extern void eOmcconfig_Set_Active_Code(eOmcconfig_code_t code);

extern eOmcconfig_code_t eOmcconfig_Get_Active_Code(void);

extern const eOmcconfig_jomo_cfg_t* eOmcconfig_board2config(uint8_t board_num, eOmcconfig_type_t type);

extern uint8_t eOmcconfig_board2jomosnumber(uint8_t board_num);


/** @}            
    end of group eo_eOmcconfig  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------





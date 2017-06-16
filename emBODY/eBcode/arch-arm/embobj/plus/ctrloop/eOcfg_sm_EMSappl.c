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

/** @file       eOcfg_sm_EMSappl.c
    @brief      This file implements internal implementation to the configuration of the example state machine EMSappl
    @author     marco.accame@iit.it
    @date       05/21/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "EoCommon.h"
#include "EOsm_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_sm_EMSappl.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_sm_EMSappl_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

/* ------------------------------------------------------------------------------------
   <SMCFG_SPECIALISE>

        01: define the max number of events managed by the state machine.
            No more than 32.
        -------------------------------------------------------------------------------
 
   </SMCFG_SPECIALISE>
   ------------------------------------------------------------------------------------
 */

enum
{
    s_events_number           = eo_sm_emsappl_EVnumberof  
};

// use this trick to verify that the number of events is no more than 32
typedef int dummy[(s_events_number > 32) ? -1 : 1];


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section






// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


/* ------------------------------------------------------------------------------------
   <SMCFG_SPECIALISE>

        02: declare name of functions used as actions in the state machine
        -------------------------------------------------------------------------------
            these are the functions executed: 
            - in transitions (on-transitions), 
            - in entering the state (on-entry),
            - in exiting the state (on-exit).
            
            it is important to define them in here before the definition of other data 
            structures which embed their names in their inside.
             
            use following naming conventions:

                prefix:
                s_smcfg_<state-machine-name>_on_trans_                for on transition 
 
                suffix:
                <name-of-state>             for on entry and on exit
                <name-of-state>_<event>     for on transition and for guard

            if there are common functionalities to several functions it is
            much better to embed a single function in different ones, than to
            lose the naming conventions.

   </SMCFG_SPECIALISE>
   ------------------------------------------------------------------------------------
 */




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

/* ------------------------------------------------------------------------------------
   <SMCFG_SPECIALISE>

        03: define the table of the states and its indices
            No more than 256
        -------------------------------------------------------------------------------
 
   </SMCFG_SPECIALISE>
   ------------------------------------------------------------------------------------
 */

typedef enum 
{
     index00State_CFG                  = eo_sm_emsappl_STcfg, /*0*/     /**< state CFG */
     index01State_RUN                  = eo_sm_emsappl_STrun, /*1*/    /**< state RUN */
     index02State_ERR                  = eo_sm_emsappl_STerr  /*2*/    /**< state ERR */
} eOsmcfgStatesIndex_t;

static const eOsmState_t s_smcfg_EMSappl_states_table_global[] = 
{ 
    {   // index00State_CFG
        EO_INIT(.name)          "stCFG",                                         
        EO_INIT(.on_entry_fn)   eo_cfg_sm_EMSappl_hid_on_entry_CFG,                   
        EO_INIT(.on_exit_fn)    eo_cfg_sm_EMSappl_hid_on_exit_CFG                  
    },
    {   // index01State_RUN
        EO_INIT(.name)          "stRUN",                               
        EO_INIT(.on_entry_fn)   eo_cfg_sm_EMSappl_hid_on_entry_RUN,                
        EO_INIT(.on_exit_fn)    eo_cfg_sm_EMSappl_hid_on_exit_RUN                
    },
    {   // index02State_ERR
        EO_INIT(.name)          "stERR",                              
        EO_INIT(.on_entry_fn)   eo_cfg_sm_EMSappl_hid_on_entry_ERR,               
        EO_INIT(.on_exit_fn)    eo_cfg_sm_EMSappl_hid_on_exit_ERR                
    }
};

/* ------------------------------------------------------------------------------------

   <SMCFG_SPECIALISE>

        05: define first state with s_initial_state
        -------------------------------------------------------------------------------
            it is the initial state of the state machine.
            however, if this state will have an initial_substate field not SMCfg_NCFG,
            then teh state machine will go to initial_substate and recursively to 
            the deepest sub-state.

            initialise it with a state STATE_ value from eOsmcfgStatesIndex_t
    
   </SMCFG_SPECIALISE>
   ------------------------------------------------------------------------------------
 */


enum    
{   // trick for compilers which cannot initialise a struct with a static const variable
    s_initial_state = index00State_CFG
};


/* ------------------------------------------------------------------------------------
   <SMCFG_SPECIALISE>

        06: define the table of transitions (one only for the entire state machine)
        -------------------------------------------------------------------------------
            the table of transitions contains all the transitions inside the state 
            machine. the transition is modeled as a struct which contains:
            - curr:             index of the originating state
            - next:             index to the destination state
            - evt:              the triggering event
            - on_transition_fn:    executed during the transition

            these fields are to be filled with already defined values or with NULL if
            no function is to be executed.

            use following naming conventions:
            
                prefix:        s_smcfg_<name>_trans_table_global

              keep unspecified the size of the array.

          IMPORTANT: by using the const qualifier these tables will stay in rom.
           

   </SMCFG_SPECIALISE>
   ------------------------------------------------------------------------------------
 */


static const eOsmTransition_t s_smcfg_EMSappl_trans_table_global[] = 
{ 
    {   // transition marked as (1) in the picture EOtheSMCfgEMSappl.jpg
        EO_INIT(.curr)              index00State_CFG,                                 
        EO_INIT(.next)              index01State_RUN,                                  
        EO_INIT(.evt)               eo_sm_emsappl_EVgo2run,                             
        EO_INIT(.on_transition_fn)  eo_cfg_sm_EMSappl_hid_on_trans_CFG_EVgo2run               
    },

    {   // transition marked as (2) in the picture EOtheSMCfgEMSappl.jpg
        EO_INIT(.curr)              index00State_CFG,                                 
        EO_INIT(.next)              index02State_ERR,                                  
        EO_INIT(.evt)               eo_sm_emsappl_EVgo2err,                             
        EO_INIT(.on_transition_fn)  eo_cfg_sm_EMSappl_hid_on_trans_CFG_EVgo2err               
    },
    
    {   // transition marked as (3) in the picture EOtheSMCfgEMSappl.jpg
        EO_INIT(.curr)              index01State_RUN,                                 
        EO_INIT(.next)              index00State_CFG,                                  
        EO_INIT(.evt)               eo_sm_emsappl_EVgo2cfg,                             
        EO_INIT(.on_transition_fn)  eo_cfg_sm_EMSappl_hid_on_trans_RUN_EVgo2cfg               
    },
 
    {   // transition marked as (4) in the picture EOtheSMCfgEMSappl.jpg
        EO_INIT(.curr)              index01State_RUN,                                 
        EO_INIT(.next)              index02State_ERR,                                  
        EO_INIT(.evt)               eo_sm_emsappl_EVgo2err,                             
        EO_INIT(.on_transition_fn)  eo_cfg_sm_EMSappl_hid_on_trans_RUN_EVgo2err               
    }     
};



/* ------------------------------------------------------------------------------------

   <SMCFG_SPECIALISE>

        06: define the global number of transitions
        -------------------------------------------------------------------------------
             naming convention:

                prefix:    s_trans_number_
                suffix: <name>

   </SMCFG_SPECIALISE>
   ------------------------------------------------------------------------------------
 */

/** @var        s_trans_number_CFG
    @brief      It is the number of transitions from the state EXAMPLE1.
    @details    use the following naming convention:
                s_smcfg_<name>_trans_number 
 
                initialise it with ... 
                s_smcfg_EMSappl_trans_number = (sizeof(s_smcfg_EMSappl_trans_table_global)/sizeof(eOsmTransition_t));
 **/



enum    
{   // trick for compilers which cannot initialise a struct with a static const variable
    s_trans_number = (sizeof(s_smcfg_EMSappl_trans_table_global)/sizeof(eOsmTransition_t))
};


/* ------------------------------------------------------------------------------------

   <SMCFG_SPECIALISE>

        08: define number of states with s_states_number
        -------------------------------------------------------------------------------
             this is the number of states. so far it is not used.

               if you have followed the naming conventions u dont need to change anything.


   </SMCFG_SPECIALISE>
   ------------------------------------------------------------------------------------
*/

/** @var        s_states_number
    @brief      It is the number of states.
    @details    Use the name s_states_number.
 **/
  
enum    
{   // trick for compilers which cannot initialise a struct with a static const variable
    s_states_number = (sizeof(s_smcfg_EMSappl_states_table_global)/sizeof(eOsmState_t))
};




/** @var        s_reset
    @brief      It is the pointer to the reset function for the state machine. This functions
                should erase all dynamic data structure and perform proper operations which are
                dependent of the state machine implementation defined in this file.
    @details    Use the name s_reset.
 **/

//#define s_reset     s_smcfg_EMSappl_reset    // or any other function or NULL
// on armcc a struct can be correctly initialised by the following static const variable
//static const eOvoid_fp_cvoidp_t s_reset = s_smcfg_EMSappl_reset;  


/** @var        s_dynamicdata_size
    @brief      It is the size of the dynamic data that the state machine object will allocate.
    @details    Use the name s_dynamicdata_size.
 **/

enum    
{   // trick for compilers which cannot initialise a struct with a static const variable
    s_dynamicdata_size = sizeof(eOsmDynamicDataEMSappl_t)
};



//#define s_init     s_smcfg_EMSappl_init    // or any other function or NULL

//static const eOvoid_fp_cvoidp_t s_init = s_smcfg_EMSappl_init;  

/* ------------------------------------------------------------------------------------

   <SMCFG_SPECIALISE>

        66: define the variable with configuration data for the state machine
        -------------------------------------------------------------------------------
               this is the ultimate data structure that is required to specialise a
            state machine. 

   </SMCFG_SPECIALISE>
   ------------------------------------------------------------------------------------
 */ 

/** @var        s_theconfiguration
    @brief      It is the required configuration, which is the colelction of previously defined 
                const variables.
    @details    Use the name s_theconfiguration.
 **/
static const eOsm_cfg_t s_theconfiguration =    
{
    EO_INIT(.nstates)               s_states_number,  
    EO_INIT(.ntrans)                s_trans_number, 
    EO_INIT(.maxevts)               s_events_number,                                         
    EO_INIT(.initstate)             s_initial_state,
    EO_INIT(.sizeofdynamicdata)     s_dynamicdata_size,     
    EO_INIT(.states)                s_smcfg_EMSappl_states_table_global,          
    EO_INIT(.transitions)           s_smcfg_EMSappl_trans_table_global,                          
    EO_INIT(.init_fn)               eo_cfg_sm_EMSappl_hid_init,
    EO_INIT(.resetdynamicdata_fn)   eo_cfg_sm_EMSappl_hid_reset
}; 


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


/* ------------------------------------------------------------------------------------
   <SMCFG_SPECIALISE>

        12: define the function which will return the configuration of the state machine
        -------------------------------------------------------------------------------
        enjoy it.

   </SMCFG_SPECIALISE>
   ------------------------------------------------------------------------------------
 */ 

extern const eOsm_cfg_t * eo_cfg_sm_EMSappl_Get(void)
{
    return(&s_theconfiguration);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// -- init
EO_weak extern void eo_cfg_sm_EMSappl_hid_init(EOsm *s)
{
    //eOsmDynamicDataEMSappl_t *ram = eo_sm_GetDynamicData(s);
}  

// -- reset
EO_weak extern void eo_cfg_sm_EMSappl_hid_reset(EOsm *s)
{
    //eOsmDynamicDataEMSappl_t *ram = eo_sm_GetDynamicData(s);
}  


// -- on entry
EO_weak extern void eo_cfg_sm_EMSappl_hid_on_entry_CFG(EOsm *s)
{
    //eOsmDynamicDataEMSappl_t *ram = eo_sm_GetDynamicData(s);
}  
EO_weak extern void eo_cfg_sm_EMSappl_hid_on_entry_RUN(EOsm *s)
{
    //eOsmDynamicDataEMSappl_t *ram = eo_sm_GetDynamicData(s);
}  
EO_weak extern void eo_cfg_sm_EMSappl_hid_on_entry_ERR(EOsm *s)
{
    //eOsmDynamicDataEMSappl_t *ram = eo_sm_GetDynamicData(s);
}  



// -- on exit 
EO_weak extern void eo_cfg_sm_EMSappl_hid_on_exit_CFG(EOsm *s)
{
    //eOsmDynamicDataEMSappl_t *ram = eo_sm_GetDynamicData(s);
}  
EO_weak extern void eo_cfg_sm_EMSappl_hid_on_exit_RUN(EOsm *s)
{

}
EO_weak extern void eo_cfg_sm_EMSappl_hid_on_exit_ERR(EOsm *s)
{
    //eOsmDynamicDataEMSappl_t *ram = eo_sm_GetDynamicData(s);
}  



// -- on trans

EO_weak extern void eo_cfg_sm_EMSappl_hid_on_trans_CFG_EVgo2run(EOsm *s)
{
    //eOsmDynamicDataEMSappl_t *ram = eo_sm_GetDynamicData(s);
}  
EO_weak extern void eo_cfg_sm_EMSappl_hid_on_trans_CFG_EVgo2err(EOsm *s)
{
    //eOsmDynamicDataEMSappl_t *ram = eo_sm_GetDynamicData(s);
}  

EO_weak extern void eo_cfg_sm_EMSappl_hid_on_trans_RUN_EVgo2cfg(EOsm *s)
{
    //eOsmDynamicDataEMSappl_t *ram = eo_sm_GetDynamicData(s);
}  
EO_weak extern void eo_cfg_sm_EMSappl_hid_on_trans_RUN_EVgo2err(EOsm *s)
{
    //eOsmDynamicDataEMSappl_t *ram = eo_sm_GetDynamicData(s);
}  




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

// WARNING: dont use static ram in the static function. they must be fully re-entrant.


/* ------------------------------------------------------------------------------------
   <SMCFG_SPECIALISE>

        13: define the static functions already declared 
        -------------------------------------------------------------------------------
            in here it is required to place the definition of on-entr, on-exit, 
            on-transition, guard.

   </SMCFG_SPECIALISE>
   ------------------------------------------------------------------------------------
 */ 
 

 // eOsmDynamicDataEMSappl_t *ram = eo_sm_GetDynamicData(s);




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




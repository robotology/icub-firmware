
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _BOOTLOADER_SM_H_
#define _BOOTLOADER_SM_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       bootloader_sm.h
    @brief      This header file implements public interface to the sm used in the bootloader of dspic33
    @author     valentina.gagegro@iit.it / marco.accame@iit.it
    @date       10/03/2011
**/

/** @defgroup bl_sm state machine for bootloader dspic33
    The ... allows ... 
 
    @todo acemor-facenda: do documentation.
    
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "eEcommon.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section




// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum blsm_state_t
    @brief      blsm_state_t contains the possible states of the bootloader. 
 **/  
typedef enum 
{
    blsm_state_idle       = 0,                    /**< the starting state */
    blsm_state_error      = 1,                    /**< the error state */
    blsm_state_exec       = 2,                    /**< the state in which the application is exec-ed */
    blsm_state_update     = 3                     /**< the state in which the application is updated from a host*/
} blsm_state_t;

enum { blsm_states_num = 4 };


/** @typedef    typedef enum blsm_state_sub_t
    @brief      blsm_state_sub_t contains the possible sub-states of the bootloader. 
 **/  
typedef enum
{
    blsm_state_any_sub_none               = 0,    /**< to be used when the state has no substate */
    blsm_state_update_sub_connected       = 1,    /**< in the update state when there is connection with the host */
    blsm_state_update_sub_readytorecbin   = 2,    /**< in the update state we are ready to receive data from the host */
    blsm_state_update_sub_writingbin      = 3,    /**< in the update state we are writing inside the flash the data received from the host */
    blsm_state_update_sub_finishtowrite   = 4,    /**< in the update state we finish to write last bytes inside the flash */
    blsm_state_update_sub_end             = 5     /**< it is a temporary sub-state from which we immediately go to exec state */
} blsm_state_sub_t;

enum { blsm_states_sub_num = 6 };


/** @typedef    typedef enum blsm_event_t
    @brief      blsm_event_t contains the possible events processed by the state machine. 
 **/ 
typedef enum
{
    blsm_evt_none                         = 0,      /**< no msg received, no timer expired, no goto, no .... just go in the forever loop. */
    blsm_evt_tickexpired                  = 1,      /**< event coming from a timer which keeps a tick of teh system */
    blsm_evt_validcanmsgreceived          = 2,      /**< event coming by polling the can bus for a received message */
    blsm_evt_goto_exec                    = 3,      /**< event which sends to execution state, either because timeout of the timer ticks or for the updater has finished burning a new application */  
    blsm_evt_goto_error                   = 4,      /**< event whcih sends to an error state, possibly for some erroneous can messages or for hw fault */
//    blsm_evt_goto_update                  = 5,      /**< event which sends to the update state because of a decoded can message */
    blsm_evt_goto_update_connected        = 6,      /**< event which sends to the update-connected state because of a decoded can message */
    blsm_evt_goto_update_readytorecbin    = 7,      /**< event which sends to the update-readytorecbin state because of a decoded can message */
    blsm_evt_goto_update_writingbin       = 8,      /**< event which sends to the update-writingbin state because of a decoded can message */
    blsm_evt_goto_update_finishtowrite    = 9,      /**< event which sends to the update-finishtowrite state because of a decoded can message */
    blsm_evt_goto_update_end              = 10      /**< event which sends to the update-end state because of a decoded can message */
} blsm_event_t;

enum { blsm_events_num = 11 };


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern void blsm_init(void)
    @brief      inits the state machine.
 **/
extern void blsm_init(void);


/** @fn         extern void blsm_set_evt(blsm_event_t evt)
    @brief      sets an event inside the internal queue of teh state machine. internal events have priorities upon
                external events.
    @param      evt             The event
    @warning    the size of the queue so far is only one !
 **/
extern void blsm_set_evt(blsm_event_t evt);

/** @fn         extern blsm_event_t blsm_get_evt(void)
    @brief      retrieves the next event by first picking any internal event or by verifying the internal ticking timer or 
                if a can message has been received.
    @param      evt             The event
    @warning    the size of the queue so far is only one !
 **/
extern blsm_event_t blsm_get_evt(void);  

/** @fn         extern void blsm_process_evt(blsm_event_t evt)
    @brief      process the event retrieved with blsm_get_evt().
    @param      evt             The event
 **/          
extern void blsm_process_evt(blsm_event_t evt);


#if 0
/** @fn         extern void blsm_get_state_substate(blsm_state_t* state, blsm_state_sub_t* sub)
    @brief      retrieve the current state and substate.
    @param      state               pointer to the state
    @param      substate            pointer to the substate
 **/ 
extern void blsm_get_state_substate(blsm_state_t* state, blsm_state_sub_t* sub); 
#endif


// - doxy end --------------------------------------------------------------------------------------------------------- 

/** @}            
    end of group bl_sm 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




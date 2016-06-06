/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "EoCommon.h"
#include "EOsm_hid.h"



// other things required for this particular state machine
#include "eupdater_parser.h"
#include "eupdater_cangtw_hid.h"
#include "hal_can.h"
#include "hal_led.h"
#include "hal_trace.h"
#include "EOtimer.h"
#include "EOaction.h"

#include "osal_task.h"
#include "osal_system.h"

#include "eupdater-info.h"

extern eObool_t eom_eupdater_main_connectsocket2host(eOipv4addr_t remaddr, EOsocketDatagram *skt, uint32_t usec);

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_sm_CanGtw.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#if		defined(_DEBUG_MODE_FULL_)
	#define _DEBUG_MODE_PRINTCAN_
	#define _DEBUG_MODE_PRINTETH_
#endif

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
    s_events_number           = 7  
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



static void s_smcfg_CanGtw_init(EOsm *s);
static void s_smcfg_CanGtw_reset(EOsm *s);


static void s_smcfg_CanGtw_on_entry_IDLE(EOsm *s);
static void s_smcfg_CanGtw_on_entry_STARTUP(EOsm *s);
static void s_smcfg_CanGtw_on_entry_RUN(EOsm *s);


//static void s_smcfg_CanGtw_on_trans_IDLE_evstart(EOsm *s);
static void s_smcfg_CanGtw_on_trans_STARTUP_evcanstable(EOsm *s);
//static void s_smcfg_CanGtw_on_trans_STARTUP_evgo2run(EOsm *s);
static void s_smcfg_CanGtw_on_trans_RUN_evgo2run(EOsm *s);
static void s_smcfg_CanGtw_on_trans_RUN_evrxeth(EOsm *s);
static void s_smcfg_CanGtw_on_trans_RUN_evrxcan1(EOsm *s);
static void s_smcfg_CanGtw_on_trans_RUN_evrxcan2(EOsm *s);


// service static functions

static void s_refresh_eth_state(void);
static void s_refresh_can1_state(void);
static void s_refresh_can2_state(void);
static void s_parse_upd_packet(EOpacket* pkt);
static void s_can_get(hal_can_port_t port);
static void s_send_blmsg(EOsm *s);
static void s_smcfg_can_clean(hal_can_port_t port);

extern void cangtw_send_ack(void);

#if     defined(_DEBUG_MODE_PRINTCAN_)
static void s_print_canframe(uint32_t progr, uint8_t tx, hal_can_port_t port, hal_can_frame_t* frame);
#endif

static eOresult_t s_valid_canid_class(uint32_t id);


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
     index00State_IDLE                  = 0,    /**< state IDLE */
     index01State_STARTUP               = 1,    /**< state STARTUP */
     index02State_RUN                   = 2     /**< state RUN */
} eOsmcfgStatesIndex_t;

static const eOsmState_t s_smcfg_CanGtw_states_table_global[] = 
{ 
    {   // index00State_IDLE
        .name           = "stIDLE",                                     // name
        .on_entry_fn    = s_smcfg_CanGtw_on_entry_IDLE,                 // onentry
        .on_exit_fn     = NULL                                          // onexit
    },
    {   // index01State_STARTUP
        .name           = "stSTART",                                    // name
        .on_entry_fn    = s_smcfg_CanGtw_on_entry_STARTUP,              // onentry
        .on_exit_fn     = NULL                                          // onexit
    },
    {   // index02State_RUN
        .name           = "stRUN",                                      // name
        .on_entry_fn    = s_smcfg_CanGtw_on_entry_RUN,                  // onentry
        .on_exit_fn     = NULL                                          // onexit
    }
};

/* ------------------------------------------------------------------------------------

   <SMCFG_SPECIALISE>

        05: define first state with s_initial_state
        -------------------------------------------------------------------------------
            it is the initial state of the state machine.
            however, if this state will have an initial_substate field not SMCfg_NIDLE,
            then teh state machine will go to initial_substate and recursively to 
            the deepest sub-state.

            initialise it with a state STATE_ value from eOsmcfgStatesIndex_t
    
   </SMCFG_SPECIALISE>
   ------------------------------------------------------------------------------------
 */


enum    
{   // trick for compilers which cannot initialise a struct with a static const variable
    s_initial_state = index00State_IDLE
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


static const eOsmTransition_t s_smcfg_CanGtw_trans_table_global[] = 
{ 
    {   // 
        .curr               = index00State_IDLE,                                 
        .next               = index01State_STARTUP,                                  
        .evt                = eo_sm_cangtw_evstart,                             
        .on_transition_fn   = NULL // s_smcfg_CanGtw_on_trans_IDLE_evstart               
    },
 
    {   // 
        .curr               = index01State_STARTUP,                                 
        .next               = index01State_STARTUP,                                  
        .evt                = eo_sm_cangtw_evcanstable,                             
        .on_transition_fn   = s_smcfg_CanGtw_on_trans_STARTUP_evcanstable               
    },
    
    {   // 
        .curr               = index01State_STARTUP,                                  
        .next               = index02State_RUN,                                
        .evt                = eo_sm_cangtw_evgo2run,                             
        .on_transition_fn   = NULL //s_smcfg_CanGtw_on_trans_STARTUP_evgo2run               
    },
 
    {   // 
        .curr               = index02State_RUN,                                
        .next               = index02State_RUN,                                 
        .evt                = eo_sm_cangtw_evgo2run,                             
        .on_transition_fn   = s_smcfg_CanGtw_on_trans_RUN_evgo2run             
    },
    
    {   // 
        .curr               = index02State_RUN,                                 
        .next               = index02State_RUN,                                  
        .evt                = eo_sm_cangtw_evrxeth,                             
        .on_transition_fn   = s_smcfg_CanGtw_on_trans_RUN_evrxeth              
    },

    {   // 
        .curr               = index02State_RUN,                                 
        .next               = index02State_RUN,                                  
        .evt                = eo_sm_cangtw_evrxcan1,                             
        .on_transition_fn   = s_smcfg_CanGtw_on_trans_RUN_evrxcan1             
    }, 
    
    {   // 
        .curr               = index02State_RUN,                                 
        .next               = index02State_RUN,                                  
        .evt                = eo_sm_cangtw_evrxcan2,                             
        .on_transition_fn   = s_smcfg_CanGtw_on_trans_RUN_evrxcan2             
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

/** @var        s_trans_number_IDLE
    @brief      It is the number of transitions from the state EXAMPLE1.
    @details    use the following naming convention:
                s_smcfg_<name>_trans_number 
 
                initialise it with ... 
                s_smcfg_CanGtw_trans_number = (sizeof(s_smcfg_CanGtw_trans_table_global)/sizeof(eOsmTransition_t));
 **/



enum    
{   // trick for compilers which cannot initialise a struct with a static const variable
    s_trans_number = (sizeof(s_smcfg_CanGtw_trans_table_global)/sizeof(eOsmTransition_t))
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
    s_states_number = (sizeof(s_smcfg_CanGtw_states_table_global)/sizeof(eOsmState_t))
};




/** @var        s_reset
    @brief      It is the pointer to the reset function for the state machine. This functions
                should erase all dynamic data structure and perform proper operations which are
                dependent of the state machine implementation defined in this file.
    @details    Use the name s_reset.
 **/

//#define s_reset     s_smcfg_CanGtw_reset    // or any other function or NULL
// on armcc a struct can be correctly initialised by the following static const variable
//static const eOvoid_fp_cvoidp_t s_reset = s_smcfg_CanGtw_reset;  


/** @var        s_dynamicdata_size
    @brief      It is the size of the dynamic data that the state machine object will allocate.
    @details    Use the name s_dynamicdata_size.
 **/

enum    
{   // trick for compilers which cannot initialise a struct with a static const variable
    s_dynamicdata_size = sizeof(eOsmDynamicDataCanGtw_t)
};



//#define s_init     s_smcfg_CanGtw_init    // or any other function or NULL

//static const eOvoid_fp_cvoidp_t s_init = s_smcfg_CanGtw_init;  

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
    .nstates                = s_states_number,  
    .ntrans                 = s_trans_number, 
    .maxevts                = s_events_number,                                         
    .initstate              = s_initial_state,
    .sizeofdynamicdata      = s_dynamicdata_size,     
    .states                 = s_smcfg_CanGtw_states_table_global,          
    .transitions            = s_smcfg_CanGtw_trans_table_global,                          
    .init_fn                = s_smcfg_CanGtw_init,
    .resetdynamicdata_fn    = s_smcfg_CanGtw_reset
}; 


// - service static variables

static EOtimer* s_smcfg_CanGtw_service_timer    = NULL;
static EOpacket* s_rxpkt_gtwcan                 = NULL;
static EOpacket* s_txpkt_gtwcan                 = NULL;



// first ms three bits are 0x7 (bootloader), then 4 bits are source and 4 bits are dest (broadcast)
#define BOOTLOADER_BROADCAST_ADDRESS    0x70f 


#define PAYLOAD_CMD_BROADCAST_LEN       0x01    // Data length (in byte) for BROADCAST command
#define CMD_BROADCAST                   0xff
static const uint8_t s_blcanBROADCASTmsg[PAYLOAD_CMD_BROADCAST_LEN] = 
{
    CMD_BROADCAST
};

#define CANMSG2SEND s_blcanBROADCASTmsg
//#define CANMSG2SEND NULL

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

extern const eOsm_cfg_t * eo_cfg_sm_cangtw_Get(void)
{
    return(&s_theconfiguration);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


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
 

static void s_smcfg_CanGtw_init(EOsm *s) 
{
    eOsmDynamicDataCanGtw_t *ram = eo_sm_GetDynamicData(s);
    
}

static void s_smcfg_CanGtw_reset(EOsm *s) 
{
    
    eOsmDynamicDataCanGtw_t *ram = eo_sm_GetDynamicData(s);
    
    // you must :
    // 1. clear dynamic data
    ram->number_of_sent_canblmsg    = 0;
        
    // 2. do proper actions which are state-machine dependent.
    // for instance ...
 
} 

static void s_smcfg_CanGtw_on_entry_IDLE(EOsm *s) 
{
//    eupdater_info_trace(NULL, "inside s_smcfg_CanGtw_on_entry_IDLE()");
    
    // allocate the service timer.
    s_smcfg_CanGtw_service_timer = eo_timer_New();   
    
    // allocate the packets
    s_rxpkt_gtwcan = eo_packet_New(eupdater_cangtw_udp_packet_maxsize);  
    s_txpkt_gtwcan = eo_packet_New(eupdater_cangtw_udp_packet_maxsize);    
}


static void s_smcfg_CanGtw_on_entry_STARTUP(EOsm *s) 
{   
    // eupdater_info_trace(NULL, "inside s_smcfg_CanGtw_on_entry_STARTUP() w/ CAN power on");
    
    // prepare can bus. then, emit two events which send all the can boards in bootloader mode    
    cangateway_hid_hal_init();
    cangateway_hid_hal_enable();
     
      
    // start a timer which shall trigger the event which sends first can frame. 
    // we give the time for can frame to get stable: xx ms is well enough.     
    EOaction_strg action_strg;
    EOaction* act = (EOaction*)&action_strg;    
    eo_action_SetEvent  (   act,
                            event_cangtw_canstable,
                            eupdater_task_cangateway
                        );
    const cangtw_parameters_t * par = eupdater_cangtw_get_parameters();    
    eo_timer_Start(s_smcfg_CanGtw_service_timer, eok_abstimeNOW, 1000 * par->t_can_stabilisation, eo_tmrmode_ONESHOT, act);    
}


static void s_smcfg_CanGtw_on_entry_RUN(EOsm *s) 
{   
    // eupdater_info_trace("GTW", "inside on_entry_RUN()");
    
    eupdater_parser_cangtw_activate();
    
    const cangtw_parameters_t * par = eupdater_cangtw_get_parameters();    
    
    if(eobool_true == par->clear_can_onentry_gtw)
    {
        s_smcfg_can_clean(hal_can_port1);
        s_smcfg_can_clean(hal_can_port2);
    }      
 
    if(eobool_true == par->send_ack)
    {
       cangtw_send_ack(); 
    }
    
    // we trigger events of kind eth, can1, can2 depending on the state of the queues.
    // we do that to avoid losing past events which were not processed.    
    s_refresh_eth_state();
    s_refresh_can1_state();
    s_refresh_can2_state();   
}


//static void s_smcfg_CanGtw_on_trans_IDLE_evstart(EOsm *s)
//{
//}


static void s_smcfg_CanGtw_on_trans_STARTUP_evcanstable(EOsm *s)
{  
    const cangtw_parameters_t * par = eupdater_cangtw_get_parameters();
    
    if(eobool_true == par->send_ff)
    {
        // send a message ff to can1 + can2
        s_send_blmsg(s);
    }
    

    // start a timer for going into run. if we sent the ff message we must wait at least its reply
    EOaction_strg action_strg;
    EOaction* act = (EOaction*)&action_strg;    
//    eo_action_SetEvent  (   act,
//                            event_cangtw_blmsg2,
//                            eupdater_task_cangateway
//                        );
    eo_action_SetEvent  (   act,
                        event_cangtw_go2run,
                        eupdater_task_cangateway
                    );
       
    
    uint32_t tt = (0 == par->t_wait_ff_reply) ? (1000) : (1000*par->t_wait_ff_reply);   
    eo_timer_Start(s_smcfg_CanGtw_service_timer, eok_abstimeNOW, tt, eo_tmrmode_ONESHOT, act);
}


//static void s_smcfg_CanGtw_on_trans_STARTUP_evgo2run(EOsm *s)
//{ 
//   
//}

static void s_smcfg_CanGtw_on_trans_RUN_evgo2run(EOsm *s)
{    
    // eupdater_info_trace("GTW", "called on_trans_RUN_evgo2run()");

    const cangtw_parameters_t * par = eupdater_cangtw_get_parameters();    
    
    if(eobool_true == par->clear_can_onentry_gtw)
    {
        s_smcfg_can_clean(hal_can_port1);
        s_smcfg_can_clean(hal_can_port2);
    }      
 
    if(eobool_true == par->send_ack)
    {
       cangtw_send_ack(); 
    }    
}


static void s_smcfg_CanGtw_on_trans_RUN_evrxeth(EOsm *s)
{
    // retrieve the packet       
    if(eores_OK == eo_socketdtg_Get(eupdater_sock_cangateway, s_rxpkt_gtwcan, eok_reltimeINFINITE))    
    {   
        // parse udp and send can frame(s)
        s_parse_upd_packet(s_rxpkt_gtwcan);        
    }
    
   
    // finally verify if socket has any other udp packet in rx fifo
    s_refresh_eth_state();
}


static void s_smcfg_CanGtw_on_trans_RUN_evrxcan1(EOsm *s)
{  
    // get can frame from the specified port and forward a single can 
    s_can_get(hal_can_port1);
    
    // verify if can1 has any other frames in rx fifo
    s_refresh_can1_state();   
}


static void s_smcfg_CanGtw_on_trans_RUN_evrxcan2(EOsm *s)
{
    // get can frame from the specified port and forward a single can 
    s_can_get(hal_can_port2);
    
    // verify if can2 has any other frames in rx fifo
    s_refresh_can2_state();   
}



static void s_refresh_eth_state(void)
{
    uint16_t numberof = 0;   
    eo_socketdtg_Received_NumberOf(eupdater_sock_cangateway, &numberof);
    if(0 != numberof)
    {
        eom_task_SetEvent(eupdater_task_cangateway, event_cangtw_sock_rec);
    }
}

static void s_refresh_can1_state(void)
{
    // verify if can1 has any other frames in rx fifo
    uint8_t framesnumberof = 0;
    hal_can_received(hal_can_port1, &framesnumberof);
    if(0 != framesnumberof)
    {
        eom_task_SetEvent(eupdater_task_cangateway, event_cangtw_can1_rec);
    } 
}

static void s_refresh_can2_state(void)
{
    // verify if can1 has any other frames in rx fifo
    uint8_t framesnumberof = 0;
    hal_can_received(hal_can_port2, &framesnumberof);
    if(0 != framesnumberof)
    {
        eom_task_SetEvent(eupdater_task_cangateway, event_cangtw_can2_rec);
    } 
}

static void s_parse_upd_packet(EOpacket* pkt)
{
	static uint32_t prevprog = 0;
    // extract the can frame   
    // send it to relevant can bus    
    eOipv4addr_t remaddr;
    eOipv4port_t remport;    
    eupdater_cangtw_udp_simpleframe_t* simpleudpframe =  NULL;
    uint16_t size = 0;
    
    eo_packet_Payload_Get(pkt, (uint8_t**) &simpleudpframe, &size);
    eo_packet_Addressing_Get(pkt, &remaddr, &remport);

    
    eupdater_cangtw_set_remote_addr(remaddr);
    
#if	defined(_DEBUG_MODE_PRINTETH_)    
    eupdater_info_trace("GTW", "ETH2: prog #%d", simpleudpframe->header.progressive);
#endif
  
    
#if !defined(_DEBUG_MODE_FULL_) // the true one    
    
    // do some basic checks ...
    if  (   (sizeof(eupdater_cangtw_udp_simpleframe_t) != size) || 
            (UDPCANSIGN != simpleudpframe->header.signature)    ||
            (simpleudpframe->onecanframe.len > 8)
        )
    {
        return;
    }
    
    // prepare the hal can frame
    hal_can_port_t port = (1 == simpleudpframe->onecanframe.canbus) ? (hal_can_port1) : (hal_can_port2);
    hal_can_frame_t frame;
    
    frame.id            = simpleudpframe->onecanframe.canid;
    frame.id_type       = hal_can_frameID_std;
    frame.frame_type    = hal_can_frame_data;
    frame.size          = simpleudpframe->onecanframe.len;
    memcpy(frame.data, simpleudpframe->onecanframe.data, frame.size);
    
#else   // debug
    
    #warning --> we are in _DEBUG_MODE_FULL_ and: the gateway retrieves the udp but alwyas sends a broadcast can message on can2.
    
    // prepare the hal can frame
    hal_can_port_t port = hal_can_port2;
    hal_can_frame_t frame;
    
    frame.id            = BOOTLOADER_BROADCAST_ADDRESS;
    frame.id_type       = hal_can_frameID_std;
    frame.frame_type    = hal_can_frame_data;
    frame.size          = sizeof(s_blcanBROADCASTmsg);
    memcpy(frame.data, s_blcanBROADCASTmsg, frame.size);

#endif   
 
    uint32_t prog = 0;    
#if     defined(_DEBUG_MODE_PRINTCAN_)
#ifdef  USE_PROG_ID
    prog = simpleudpframe->header.progressive;
#else
    prog = 0;
#endif    
    s_print_canframe(prog, 1, port, &frame); 
#endif 

	prog = simpleudpframe->header.progressive;
    if(0 != prevprog)
	{
		if(prog != (prevprog+1))
		{
            eupdater_info_trace("GTW", "LOST ETH #%d from host", prevprog+1);
		}
	} 
	prevprog = prog;  

    // and send it 
    hal_led_toggle((hal_can_port1 == port) ? hal_led4 : hal_led5);
    hal_can_put(port, &frame, hal_can_send_normprio_now);
}

static void s_can_get(hal_can_port_t port)
{
    hal_can_frame_t frame;
    uint8_t remaining = 0;
    hal_result_t res;
    eupdater_cangtw_udp_simpleframe_t* simpleudpframe = NULL;
    uint16_t size = 0;

    eo_packet_Payload_Get(s_txpkt_gtwcan, (uint8_t**)&simpleudpframe, &size);
    
  
    res = hal_can_get(port, &frame, &remaining);
    
    if(eobool_false == eupdater_parser_cangtw_isactivated())
    {
    //    eupdater_info_trace("GTW", "called s_can_get() but the gateway is not active yet");
#if     defined(_DEBUG_MODE_PRINTCAN_)          
        s_print_canframe(0, 0, port, &frame);   
#endif        
        return;
    }

    
    if(hal_res_OK == res)
    {    
        if(eores_OK == s_valid_canid_class(frame.id))
        {
            simpleudpframe->header.signature        = UDPCANSIGN;
            simpleudpframe->header.canframenumof    = 1;
            memset(&simpleudpframe->header.dummy, 0, sizeof(simpleudpframe->header.dummy));
            simpleudpframe->onecanframe.canbus      = (hal_can_port1 == port) ? (1) : (2);
            simpleudpframe->onecanframe.canid       = frame.id;
            simpleudpframe->onecanframe.len         = frame.size;
            memset(&simpleudpframe->onecanframe.dummy, 0, sizeof(simpleudpframe->onecanframe.dummy));
            memset(&simpleudpframe->onecanframe.data, 0, sizeof(simpleudpframe->onecanframe.data));
            memcpy(&simpleudpframe->onecanframe.data, frame.data, simpleudpframe->onecanframe.len);
            
            size = sizeof(eupdater_cangtw_udp_simpleframe_t);
            eo_packet_Size_Set(s_txpkt_gtwcan, size);
            
            eOipv4addr_t remhostaddr = eupdater_cangtw_get_remote_addr();
            if(eok_ipv4addr_localhost != remhostaddr)
            {
                if(eobool_true == eom_eupdater_main_connectsocket2host(remhostaddr, eupdater_sock_cangateway, 1000*eok_reltime1ms))
                {
                    eo_packet_Addressing_Set(s_txpkt_gtwcan, remhostaddr, eupdater_cangtw_get_remote_port());
                    eo_socketdtg_Put(eupdater_sock_cangateway, s_txpkt_gtwcan); 
                }                    
            }
        }
        
#if     defined(_DEBUG_MODE_PRINTCAN_)                
        s_print_canframe(0, 0, port, &frame);        
#endif
        
    }
}


extern void cangtw_send_ack(void)
{
    uint8_t * ack = NULL;
    uint16_t size = 0;

    eo_packet_Payload_Get(s_txpkt_gtwcan, (uint8_t**)&ack, &size);
    
    ack[0] = 0x20;
    ack[1] = (eobool_true == eupdater_parser_cangtw_isactivated()) ? 1 : 0;
    
    eo_packet_Size_Set(s_txpkt_gtwcan, 2);
            
    eOipv4addr_t remhostaddr = eupdater_cangtw_get_remote_addr();
    if(eok_ipv4addr_localhost != remhostaddr)
    {
        if(eobool_true == eom_eupdater_main_connectsocket2host(remhostaddr, eupdater_sock_cangateway, 1000*eok_reltime1ms))
        {
            //eo_packet_Addressing_Set(s_txpkt_gtwcan, remhostaddr, eupdater_cangtw_get_remote_port());
            eo_packet_Addressing_Set(s_txpkt_gtwcan, remhostaddr, 3334);
            eo_socketdtg_Put(eupdater_sock_cangateway, s_txpkt_gtwcan); 
        }                    
    }

}
    
static void s_send_blmsg(EOsm *s)
{
    eOsmDynamicDataCanGtw_t *ram = eo_sm_GetDynamicData(s);
    ram->number_of_sent_canblmsg ++;
    

    if(NULL != CANMSG2SEND)
    {            
    //   eupdater_info_trace("GTW", "sent 0xff can command on both can buses");
        
        hal_can_frame_t frame;
              
        frame.id            = BOOTLOADER_BROADCAST_ADDRESS;
        frame.id_type       = hal_can_frameID_std;
        frame.frame_type    = hal_can_frame_data;
        frame.size          = sizeof(CANMSG2SEND);
        memcpy(frame.data, CANMSG2SEND, frame.size);
        
        hal_can_put(hal_can_port1, &frame, hal_can_send_normprio_now);
        hal_can_put(hal_can_port2, &frame, hal_can_send_normprio_now);      
        
        hal_led_toggle(hal_led4);
        hal_led_toggle(hal_led5); 
    }  
}

static void s_smcfg_can_clean(hal_can_port_t port)
{   
    hal_can_frame_t frame;
    uint8_t remaining = 0;
    hal_result_t res = hal_res_OK;
    uint8_t num = 0;
    
    //eupdater_info_trace("GTW", "called s_smcfg_can_clean() for CAN%d", port+1);
    
    
    for(;;)
    {
        res = hal_can_get(port, &frame, &remaining);
        
        if(hal_res_OK == res)
        {
            num++;

//            eupdater_info_trace("GTW", "removed frame from can%d: size=%d, id=%d, d0=%d, d1=%d", 
//                                       (hal_can_port1==port)?1:2, 
//                                        frame.size, 
//                                        frame.id, 
//                                        frame.data[0], frame.data[1]);          
        }
        else
        {            
            // quit
            break;        
        }
    }
//    eupdater_info_trace("GTW", "in total removed %d frames from can%d.", 
//                                num,
//                               (hal_can_port1==port)?1:2
//                                
//            );
    
}

#if     defined(_DEBUG_MODE_PRINTCAN_)
static void s_print_canframe(uint32_t progr, uint8_t tx, hal_can_port_t port, hal_can_frame_t* frame)
{
    uint8_t dd[8] = {0xee};

    static osal_abstime_t tt = 0;
    osal_abstime_t t = osal_system_ticks_abstime_get();
    uint32_t delta = (t - tt)/1000;


    memcpy(dd, frame->data, frame->size);
    
    eupdater_info_trace("GTW", "%s frame #%d \t\t %d \t after %d ms: \t\tcan%d, id = %3.3x, s = %d, d[] = %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x ", 
                                (1 == tx) ? "tx" : "rx",
                                progr,
                                delta,
                               (hal_can_port1==port)?1:2,
                                frame->id,
                                frame->size,
                                dd[0], dd[1], dd[2], dd[3], dd[4], dd[5], dd[6], dd[7]
             );
    
    
}
#endif

#define PASS_ALL
static eOresult_t s_valid_canid_class(uint32_t id)
{
#ifdef PASS_ALL
    return(eores_OK);
#else
    #define BL_MASK 0x00000700
    if((id & BL_MASK) == BL_MASK)
    {
        return(eores_OK);
    }    
    return(eores_NOK_generic);    
#endif
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------







// this file contains redefinition of functions in eOcfg_sm_EMSappl_hid.h, 
// in legacy ETH application they were redefined in EOMtheEMSappl.c 
// but not anymore. FOR NOW

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "EoProtocol.h"
#include "EoProtocolMN.h"
#include "EOaction.h"
#include "EOMtheEMSconfigurator.h"
#include "EOMtheEMSrunner.h"
#include "EOMtheEMSerror.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_sm_EMSappl_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// -- init

extern void eo_cfg_sm_EMSappl_hid_init(EOsm *s)
{
    // does nothing
}  

// -- reset

extern void eo_cfg_sm_EMSappl_hid_reset(EOsm *s)
{
    // does nothing
}  


// -- on entry

extern void eo_cfg_sm_EMSappl_hid_on_entry_CFG(EOsm *s)
{
    // set the correct state 
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_appl, 0, eoprot_tag_mn_appl_status);
    eOmn_appl_status_t *status = reinterpret_cast<eOmn_appl_status_t*>(eoprot_variable_ramof_get(eoprot_board_localboard, id32));
    status->currstate = applstate_config;      
    
    // tell the ems socket that it must alert the EOMtheEMSconfigurator upon RX of packets
    EOaction_strg astg = {0};
    EOaction *onrx = reinterpret_cast<EOaction*>(&astg);
    eo_action_SetEvent(onrx, emssocket_evt_packet_received, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));
    eom_emssocket_Open(eom_emssocket_GetHandle(), onrx, NULL, NULL);
    
    // if any rx packets is already in the socket then alert the cfg task
    if(0 != eom_emssocket_NumberOfReceivedPackets(eom_emssocket_GetHandle()))
    {
        eom_task_SetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emssocket_evt_packet_received);        
    }
     
    // exec any user-defined activity
    #warning: this one is inside ... overridden_appl
    //eom_emsappl_hid_userdef_on_entry_CFG();
}  

extern void eo_cfg_sm_EMSappl_hid_on_entry_RUN(EOsm *s)
{
    // set the correct state 
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_appl, 0, eoprot_tag_mn_appl_status);
    eOmn_appl_status_t *status = reinterpret_cast<eOmn_appl_status_t*>(eoprot_variable_ramof_get(eoprot_board_localboard, id32));
    status->currstate = applstate_running;       
    
    EOaction_strg astg = {0};
    EOaction *ontxdone = reinterpret_cast<EOaction*>(&astg);
    // tell the ems socket that it must alert the EOMtheEMSconfigurator upon RX of packets
    eo_action_SetCallback(ontxdone, (eOcallback_t)eom_emsrunner_OnUDPpacketTransmitted, eom_emsrunner_GetHandle(), NULL);
    // the socket does not alert anybody when it receives a pkt, but will alert the sending task, so that it knows that it can stop wait.
    // the alert is done by a callback, eom_emsrunner_OnUDPpacketTransmitted(), which executed by the sender of the packet directly.
    // this funtion is executed with eo_action_Execute(s->socket->ontransmission) inside the EOMtheIPnet object
    // for this reason we call eo_action_SetCallback(....., exectask = NULL_); IT MUST NOT BE the callback-manager!!!!
    eom_emssocket_Open(eom_emssocket_GetHandle(), NULL, ontxdone, NULL);
    
    // we activate the runner
    eom_emsrunner_Start(eom_emsrunner_GetHandle());

#warning add something in here    
    // exec any user-defined activity
//    eom_emsappl_hid_userdef_on_entry_RUN(&s_emsappl_singleton);   
}  

extern void eo_cfg_sm_EMSappl_hid_on_entry_ERR(EOsm *s)
{
    // set the correct state 
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_appl, 0, eoprot_tag_mn_appl_status);
    eOmn_appl_status_t *status = reinterpret_cast<eOmn_appl_status_t*>(eoprot_variable_ramof_get(eoprot_board_localboard, id32));
    status->currstate = applstate_error;    
   
    // redirect the socket
    EOaction_strg astg = {0};
    EOaction *onrx = reinterpret_cast<EOaction*>(&astg);
    eo_action_SetEvent(onrx, emssocket_evt_packet_received, eom_emserror_GetTask(eom_emserror_GetHandle()));
    // the socket alerts the error task for any newly received packets
    eom_emssocket_Open(eom_emssocket_GetHandle(), onrx, NULL, NULL);
    
    // if any rx packets already in socket then alert the err task
    if(0 != eom_emssocket_NumberOfReceivedPackets(eom_emssocket_GetHandle()))
    {
        eom_task_SetEvent(eom_emserror_GetTask(eom_emserror_GetHandle()), emssocket_evt_packet_received);        
    }

#warning add something in here       
    // exec any user-defined activity
//    eom_emsappl_hid_userdef_on_entry_ERR(&s_emsappl_singleton);      
}  



// -- on exit 

extern void eo_cfg_sm_EMSappl_hid_on_exit_CFG(EOsm *s)
{
    #warning add something in here  
    // eom_emsappl_hid_userdef_on_exit_CFG(&s_emsappl_singleton);
}  

extern void eo_cfg_sm_EMSappl_hid_on_exit_RUN(EOsm *s)
{
    //#warning --> it is good thing to attempt to stop the hal timers in here as well. see comment below.
    // marco.accame: if we exit from the runner in an un-expected way with a fatal error, then we dont 
    // stop teh timers smoothly. thus we do it in here as well.
    
    // in normal case instead, the stop of the emsrunner is not executed in one function, but in steps 
    // inside its rx-do-tx tasks.
    
    
    eom_emsrunner_Stop(eom_emsrunner_GetHandle());
    #warning add something in here 
    // eom_emsappl_hid_userdef_on_exit_RUN(&s_emsappl_singleton);
}

extern void eo_cfg_sm_EMSappl_hid_on_exit_ERR(EOsm *s)
{
    #warning add something in here 
    // eom_emsappl_hid_userdef_on_exit_ERR(&s_emsappl_singleton);
}  



// -- on trans

extern void eo_cfg_sm_EMSappl_hid_on_trans_CFG_EVgo2run(EOsm *s)
{
    // nothing to do
}  

extern void eo_cfg_sm_EMSappl_hid_on_trans_CFG_EVgo2err(EOsm *s)
{
    // nothing to do
}  

extern void eo_cfg_sm_EMSappl_hid_on_trans_RUN_EVgo2cfg(EOsm *s)
{
    // nothing to do
}  

extern void eo_cfg_sm_EMSappl_hid_on_trans_RUN_EVgo2err(EOsm *s)
{
    // nothing to do
}  


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





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

/* @file       eom_emsappl_main.c
	@brief      This file keeps the main of an application on ems using the embobj
	@author     marco.accame@iit.it
    @date       05/21/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------




#include "stdint.h"
#include "stdlib.h"
#include "string.h"



#include "EOMtheSystem.h"

#include "EOMtheEMSapplCfg.h"
#include "EOMtheEMSappl.h"

#include "emBODYrobot.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eom_emsappl_main_init(void);


static void s_eom_emsappl_main_sometests(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


int main(void)
{
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_Initialise();

    eom_sys_Initialise( &emscfg->wsyscfg.msyscfg, 
                        &emscfg->wsyscfg.mempoolcfg,                         
                        &emscfg->wsyscfg.errmancfg,                 
                        &emscfg->wsyscfg.tmrmancfg, 
                        &emscfg->wsyscfg.cbkmancfg 
                      );  
    
    eom_sys_Start(eom_sys_GetHandle(), s_eom_emsappl_main_init);

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


/** @fn         static void s_eom_emsappl_main_init(void)
    @brief      It initialises the emsappl 
    @details    bla bla bla.
 **/

static void s_eom_emsappl_main_init(void)
{
   
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();

//    s_eom_emsappl_main_sometests();
    
    eom_emsappl_Initialise(&emscfg->applcfg);
}


// - some tests


#include "emBODYrobot.h"
#include "EOprotocol.h"
#include "hal_trace.h"
 
// #include "eOprot_robot.h"
// #include "eOprot_b01.h"
// #include "eOprot_b02.h"

// #include "EOnvsetDEVbuilder.h"
// #include "EOconstvector_hid.h"

// #include "EOprotocolConfigurator.h"


// extern eOnvset_DEVcfg_t* devcfg0 = NULL;
// extern eOnvset_DEVcfg_t* devcfgC = NULL;

// #include "EOhostTransceiver.h"
// extern EOhostTransceiver* ht = NULL;

// #include "EoProtocolMN.h"
// #include "EoProtocolMC.h"
// #include "EoProtocolSK.h"

static void s_eom_emsappl_main_sometests(void)
{
	
//	#if	defined(emBODYrobot_ROBOT_NAME)
	
	#if	(emBODYrobot_ROBOT_NAME2SERIAL(iCubDarmstadt01) == emBODYrobot_ROBOT_NAME2SERIAL(EO_ROBOT_NAME))
//	sasads
	#endif
    
	#if	(iCubDarmstadt01 == emBODYrobot_ROBOT_NAME)
//	xeqwfdwe
	#endif    
	
        const char str[] = "iCubDarmstadt01";
        const char sss[] = EO_STRINGIFY(emBODYrobot_ROBOT_NAME);
        char ssss[64] = "";    
    
        hal_trace_puts(emBODYrobot_ROBOT_NAME2STRING(emBODYrobot_ROBOT_NAME));
	
		if(0 == strcmp("iCubDarmstadt01", emBODYrobot_ROBOT_NAME2STRING(emBODYrobot_ROBOT_NAME)))
		{	
            snprintf(ssss, 64, "emBODYrobot_ROBOT_NAME = %d", emBODYrobot_ROBOT_NAME);
            hal_trace_puts(ssss);
			hal_trace_puts("iCubDarmstadt01");
//            hal_trace_puts(EO_STRINGIFY(emBODYrobot_ROBOT_NAME));
            
		}

        snprintf(ssss, 64, "emBODYrobot_ROBOT_NAME = %d, %s", emBODYrobot_ROBOT_NAME, emBODYrobot_ROBOT_NAME2STRING(emBODYrobot_ROBOT_NAME));
        hal_trace_puts(ssss);   
        
        snprintf(ssss, 64, "emBODYrobot_BOARD_NAME = %d, %s", emBODYrobot_BOARD_NAME, emBODYrobot_BOARD_NAME2STRING(emBODYrobot_BOARD_NAME));
        hal_trace_puts(ssss);   

        snprintf(ssss, 64, "emBODYrobot_BOARD_NUMBER = %d", emBODYrobot_BOARD_NUMBER);
        hal_trace_puts(ssss);         
	
//	#endif

#if 0    
#if 1   
    const eOnvset_DEVcfg_t *devcfg  = NULL;
    const eOnvset_DEVcfg_t *theone = NULL;
    theone = &eoprot_b02_nvsetDEVcfg;
    theone = theone;
    
    devcfg = eoprot_robot_DEVcfg_get(0);
    devcfg = devcfg;
    
    uint32_t id = eoprot_robot_id_get();
#endif
    
#if 1
    
    eOresult_t res = eores_OK;        
    
    // params
    eOnvBRD_t       brd         = 0;

    uint8_t         njoints     = 4;
    uint8_t         nmotors     = 4;

    uint8_t         nstrains    = 1;
    uint8_t         nmaises     = 0;
    uint8_t         nexttorques = 4;   

    //uint8_t         nskins = 1;
    
    // new
    EOnvsetDEVbuilder* d = eo_nvsetdevbuilder_New(brd);
   
     
    // mn
    res = eo_nvsetdevbuilder_ENTITYpushback(d, eoprot_endpoint_management, eoprot_entity_mn_comm, 1);
    res = eo_nvsetdevbuilder_ENTITYpushback(d, eoprot_endpoint_management, eoprot_entity_mn_appl, 1);    
    // mc
    res = eo_nvsetdevbuilder_ENTITYpushback(d, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, njoints);
    res = eo_nvsetdevbuilder_ENTITYpushback(d, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, nmotors);
    res = eo_nvsetdevbuilder_ENTITYpushback(d, eoprot_endpoint_motioncontrol, eoprot_entity_mc_controller, 1);
    // as
    res = eo_nvsetdevbuilder_ENTITYpushback(d, eoprot_endpoint_analogsensors, eoprot_entity_as_strain, nstrains);
    //res = eo_nvsetdevbuilder_ENTITYpushback(d, eoprot_endpoint_analogsensors, eoprot_entity_as_mais, nmaises);
    res = eo_nvsetdevbuilder_ENTITYpushback(d, eoprot_endpoint_analogsensors, eoprot_entity_as_extorque, nexttorques);

    // sk
    //res = eo_nvsetdevbuilder_ENTITYpushback(d, eoprot_endpoint_skin, eoprot_entity_sk_skin, nskins);
    
    res = eo_nvsetdevbuilder_Prepare(d);
    
    devcfg0 = eo_nvsetdevbuilder_DEVcfg_Get(d);
    
    devcfg0 = devcfg0;
    
    eOnvset_EPcfg_t epcfgram[4];
    eOnvset_EPcfg_t epcfgrom[4];
    memcpy(epcfgram, devcfg0->vectorof_epcfg->item_array_data, sizeof(epcfgram)); 
    memcpy(epcfgrom, theone->vectorof_epcfg->item_array_data, sizeof(epcfgrom));
    
    res = res;
    
    
    // --
    eOprotconfig_cfg_t prcfg;
    memcpy(&prcfg, &eo_protconfig_cfg_default, sizeof(eOprotconfig_cfg_t));
    prcfg.board                             = brd+1;
//     prcfg.en_mc_entity_joint_numberof       = njoints;
//     prcfg.en_mc_entity_motor_numberof       = nmotors;
//     prcfg.en_as_entity_strain_numberof      = nstrains;
//     prcfg.en_as_entity_mais_numberof        = nmaises;
//     prcfg.en_as_entity_extorque_numberof    = nexttorques;
//     prcfg.ep_skin_is_present                = eobool_false;
//     prcfg.en_sk_entity_skin_numberof        = 0;
    
    EOprotocolConfigurator* pc = eo_protconfig_New(&prcfg);
    devcfgC = eo_protconfig_DEVcfg_Get(pc);
    eOnvset_EPcfg_t epcfgramC[4];
    memcpy(epcfgramC, devcfgC->vectorof_epcfg->item_array_data, sizeof(epcfgramC)); 
    
    res = res;
    res = res;
    res = res;
    res = res;
    res = res;
    
#endif

#endif


//     eOhosttransceiver_cfg_t hconfig;
//     memcpy(&hconfig, &eo_hosttransceiver_cfg_default, sizeof(eOhosttransceiver_cfg_t));
//     //hconfig.sizes.capacityoftxpacket = 8;
//     hconfig.sizes.capacityofropframeregulars = 0;
//     hconfig.sizes.capacityofropframereplies = 0;
//     hconfig.sizes.capacityofropframeoccasionals = 0; //hconfig.sizes.capacityoftxpacket;
//     
//     hconfig.nvsetdevcfg = &eoprot_b02_nvsetDEVcfg;   
//     
//     ht = eo_hosttransceiver_New(&hconfig);
//     EOtransceiver * tr = eo_hosttransceiver_GetTransceiver(ht);
//     
//     eOropdescriptor_t ropdes = {0};
//     memcpy(&ropdes, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
//     ropdes.ropcode = eo_ropcode_say;
//     ropdes.id32 = eoprot_ID_get(0, 0, 0, 0);

//     eo_transceiver_RegularROP_Load(tr, &ropdes);
//     
//     eo_transceiver_ReplyROP_Load(tr, &ropdes);
//     
//     uint16_t num = 0;
//     eo_transceiver_outpacket_Prepare(tr, &num);
    
//     
//     //eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, 0, eoprot_tag_mc_motor_config_maxcurrentofmotor);
//     eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_skin, eoprot_entity_sk_skin, 0, eoprot_tag_sk_skin_cmd_boardscfg);
//     
//     
//     const char *entityname = eoprot_ID2stringOfEntity(id32);
//     const char *endpointname = eoprot_ID2stringOfEndpoint(id32);
//     const char *tagname = eoprot_ID2stringOfTag(id32);
//     
//     hal_trace_puts(endpointname);
//     hal_trace_puts(entityname);
//     hal_trace_puts(tagname);

    
//     const eOprot_callbacks_variable_descriptor_t xxx = 
//     {
//         .endpoint   = eoprot_endpoint_management,
//         .entity     = eoprot_entity_mn_comm,
//         .tag        = eoprot_rwm_mn_comm_status,
//         .init       = eoprot_fun_INIT_mn_comm_status,
//         .update     = NULL        
//     };
//     
//     eOresult_t res = eoprot_config_callbacks_variable_set(&xxx);
//     
//     res = res;
    
}

//#include "EOtheBOARDtransceiver.h"
//#include "EOrop.h"
//#include "EOprotocolMN.h"
//#include "EOnv_hid.h"

//extern eOresult_t send_diagnostics_to_server(const char *str, uint32_t signature)
//{
//    static uint8_t initted = 0;
//    static eOropdescriptor_t rd;
//    static EOnv* nv = NULL;
//    static EOtransceiver *t = NULL;  
//    static EOnvSet * nvset = NULL;  

//    
//    if((NULL == str) || (strlen(str) > 5))
//    {
//        return(eores_NOK_generic);
//    }
//    
//    if(0 == initted)
//    {
//        t = eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle());
//        nvset = eo_boardtransceiver_GetNVset(eo_boardtransceiver_GetHandle());
//        memcpy(&rd, &eok_ropdesc_basic, sizeof(eOropdescriptor_t));
//        rd.ropcode  = eo_ropcode_sig;
//        rd.id32     = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_appl, 0, eoprot_tag_mn_appl_status);
//        rd.data     = NULL; 
//        
//        nv = eo_nv_New();
//        eo_nvset_NV_Get(nvset, eo_nv_ownership_local, rd.id32, nv);    
//        
//     
//        initted = 1;
//    }
//    
//    // fill signature
//    rd.control.plussign     = (eo_nv_ID32dummy == signature) ? (0) : 1;
//    rd.signature            = signature;
//    
//    // must put str into the 8 bytes of eOmn_appl_status_t
//    
//    eOmn_appl_status_t applstatus;
//    uint16_t size = 0;
//    eo_nv_Get(nv, eo_nv_strg_volatile, &applstatus, &size);
//   
//    memcpy(&applstatus.filler06[0], str, 6);
//    
//    eo_nv_Set(nv, &applstatus, eobool_true, eo_nv_upd_dontdo);
//       
//    
//    eo_transceiver_OccasionalROP_Load(t, &rd);
//  
//    return(eores_OK);
//}


//#include "EoCommon.h"
//#include "EoProtocolMN.h"

//#include "string.h"
//#include "stdint.h"
//#include "stdlib.h"


//    
//void eoprot_fun_UPDT_mn_appl_status(const EOnv* nv, const eOropdescriptor_t* rd)
//{
//    static const char* states[] =     
//    {
//        "applstate_config",
//        "applstate_running",
//        "applstate_error",
//        "not initted"
//    };
//    
//    char str[128] = {0};
//    
//    eOmn_appl_status_t* appstatus = (eOmn_appl_status_t*) rd->data;
//    
//    const char* state = (appstatus->currstate > 2) ? (states[3]) : (states[appstatus->currstate]);
//    
//    snprintf(str, sizeof(str), "DIAGNOSTICS: board EB%d -> state = %s, sign = %x, msg = %s ", eo_nv_GetBRD(nv)+1, state, rd->signature, appstatus->filler06); 
//    
//    printf("%s\n", str);
//    //printf("DIAGNOSTICS: board EB%d -> state = %s, sign = %x, msg = %s\n", eo_nv_GetBRD(nv)+1, state, rd->signature, appstatus->filler06); 

//}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




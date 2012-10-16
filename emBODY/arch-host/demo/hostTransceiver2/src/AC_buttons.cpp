/*
 * AC_callback.cpp
 *
 *  Created on: Sep 20, 2012
 *      Author: icub
 */

#include "main-transceiver.hpp"



// --------------------------------------------------------------------------------------------------------------------
// - definition of callback functions
// --------------------------------------------------------------------------------------------------------------------

bool 		enableSender = true;

void commands(void)
{

}

void s_callback_button_0(void)
{
	char str[128];
	eOcfg_nvsEP_mn_applNumber_t dummy = 0;
	eOnvID_t nvid_go2state 		= eo_cfg_nvsEP_mn_appl_NVID_Get(endpoint_mn_appl, dummy, applNVindex_cmmnds__go2state);
	if(EOK_uint16dummy == nvid_go2state)
	{
		printf("nvid_go2state error ");
		return;
	}


	EOnv 	*nv_p 				= transceiver->getNVhandler(endpoint_mn_appl, nvid_go2state);
	if(NULL == nv_p)
	{
		printf("nv pointer error ");
		return;
	}


	eOmn_appl_state_t  desired 	= applstate_running;

	printf("nvid = %d (0X%04X", nvid_go2state, nvid_go2state);
	if( eores_OK != eo_nv_Set(nv_p, &desired, eobool_true, eo_nv_upd_dontdo))
		printf("error!!");
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, endpoint_mn_appl, nvid_go2state);
}

void s_callback_button_1(void)
{
	char str[128];
	EOnv 						*cnv;
	eOmn_ropsigcfg_command_t 	*ropsigcfgassign;
	EOarray						*array;
	eOropSIGcfg_t 				sigcfg;
	eOcfg_nvsEP_mn_commNumber_t dummy = 0;
	eOnvID_t 					nvid = -1;
	EOnv 						*nvRoot = NULL;

	printf("Setting regulars!\n");

	//
	//  1: skin sconfig = 1
	//
	eOnvEP_t 			ep = endpoint_sk_emsboard_rightlowerarm;
	switch (boardN )
	{
	case 2:	// left
		ep = endpoint_sk_emsboard_leftlowerarm;
		printf("left\n");
		break;

	case 4:	// right
		ep = endpoint_sk_emsboard_rightlowerarm;
		printf("right\n");
		break;
	}

	nvid = eo_cfg_nvsEP_sk_NVID_Get((eOcfg_nvsEP_sk_endpoint_t)ep, dummy, skinNVindex_sconfig__sigmode);
	nvRoot = transceiver->getNVhandler(ep, nvid);
	if(NULL == nvRoot)
	{
		printf("\n>>> ERROR \ntransceiver->getNVhandler returned NULL!!\n");
		return;
	}
	uint8_t dat = 1;
	if( eores_OK != eo_nv_Set(nvRoot, &dat, eobool_true, eo_nv_upd_dontdo))
	{
		printf("\n>>> ERROR \neo_nv_Set !!\n");
		return;
	}
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);


	//
	// 2: Segnalazione spontanea
	//
	eOnvID_t nvid_ropsigcfgassign = eo_cfg_nvsEP_mn_comm_NVID_Get(endpoint_mn_comm, dummy, commNVindex__ropsigcfgcommand);
	cnv = transceiver->getNVhandler(endpoint_mn_comm, nvid_ropsigcfgassign);
	ropsigcfgassign = (eOmn_ropsigcfg_command_t*) cnv->loc;
	array = (EOarray*) &ropsigcfgassign->array;
	eo_array_Reset(array);
	array->head.capacity = NUMOFROPSIGCFG;
	array->head.itemsize = sizeof(eOropSIGcfg_t);
	ropsigcfgassign->cmmnd = ropsigcfg_cmd_assign;

	switch (boardN )
	{
	case 2:  // left
		ep = endpoint_mc_leftlowerarm;
		break;

	case 4:	// right
		ep = endpoint_mc_rightlowerarm;
		break;
	}

/*
	nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, 0, jointNVindex_jstatus__basic);
	sigcfg.ep = ep;
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

	nvid = eo_cfg_nvsEP_mc_motor_NVID_Get(ep, 0, motorNVindex_mstatus__basic);
	sigcfg.ep = ep;
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

	nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, 1, jointNVindex_jstatus__basic);
	sigcfg.ep = ep;
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

	nvid = eo_cfg_nvsEP_mc_motor_NVID_Get(ep, 1, motorNVindex_mstatus__basic);
	sigcfg.ep = endpoint_mc_rightlowerarm;
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

	nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, 0, jointNVindex_jconfig);
	sigcfg.ep = ep;
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

	nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, 0, jointNVindex_jconfig__maxpositionofjoint);
	sigcfg.ep = ep;
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

	nvid = eo_cfg_nvsEP_mc_motor_NVID_Get(ep, 0, motorNVindex_mconfig);
	sigcfg.ep = ep;
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);


	sigcfg.ep = ep;
	sigcfg.id = eo_cfg_nvsEP_mc_motor_NVID_Get((eOcfg_nvsEP_mc_endpoint_t)sigcfg.ep, 0, motorNVindex_mconfig__maxcurrentofmotor);
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);
 */

	switch (boardN )
	{
	case 2:  // left
		ep = endpoint_sk_emsboard_leftlowerarm;
		break;

	case 4:	// right
		ep = endpoint_sk_emsboard_rightlowerarm;
		break;
	}
	sigcfg.ep = ep;
	sigcfg.id = eo_cfg_nvsEP_sk_NVID_Get((eOcfg_nvsEP_sk_endpoint_t)sigcfg.ep, 0, skinNVindex_sstatus__arrayof10canframe);
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

#ifdef _MAIS_
	switch (boardN )
	{
	case 2:	// left
		ep = endpoint_as_leftlowerarm;
		break;

	case 4:	// right
		ep = endpoint_as_rightlowerarm;
		break;
	}

	sigcfg.ep = ep;
	sigcfg.id = eo_cfg_nvsEP_as_mais_NVID_Get((eOcfg_nvsEP_as_endpoint_t)sigcfg.ep, dummy, maisNVindex_mstatus__the15values);
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);
#endif
	transceiver->load_occasional_rop(eo_ropcode_set, endpoint_mn_comm, nvid_ropsigcfgassign);
	//	fine della configurazione delle variabili da segnalare spontaneamente : commNVindex__ropsigcfgcommand


#ifdef _MAIS_
	//
	//  3: mais datarate = 1millisec
	//
	switch (boardN )
	{
	case 2:	// left
		ep = endpoint_as_leftlowerarm;
		break;

	case 4:	// right
		ep = endpoint_as_rightlowerarm;
		break;
	}
	nvid = eo_cfg_nvsEP_as_mais_NVID_Get((eOcfg_nvsEP_as_endpoint_t)ep, dummy, maisNVindex_mconfig__datarate);
	nvRoot = transceiver->getNVhandler(ep, nvid);
	dat = 1;
	if( eores_OK != eo_nv_Set(nvRoot, &dat, eobool_true, eo_nv_upd_dontdo))
		printf("error!!");
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);


	//
	//  4: mais datarate = contnuosly
	//
	switch (boardN )
	{
	case 2:	// left
		ep = endpoint_as_leftlowerarm;
		break;

	case 4:	// right
		ep = endpoint_as_rightlowerarm;
		break;
	}
	nvid = eo_cfg_nvsEP_as_mais_NVID_Get((eOcfg_nvsEP_as_endpoint_t)ep, dummy, maisNVindex_mconfig__mode);
	nvRoot = transceiver->getNVhandler(ep, nvid);
	dat = 0;
	if( eores_OK != eo_nv_Set(nvRoot, &dat, eobool_true, eo_nv_upd_dontdo))
		printf("error!!");
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);




#endif
}

void s_callback_button_2(void )
{
	char str[128];
	int j = 0;

	EOnv 						*cnv;
	eOmn_ropsigcfg_command_t 	*ropsigcfgassign;
	EOarray						*array;
	eOropSIGcfg_t 				sigcfg;
	eOcfg_nvsEP_mn_commNumber_t dummy = 0;


	// get nvid from parameters
	//#warning "aggiornare chiamate a funzione"
	eOnvID_t signal = eo_cfg_nvsEP_sk_NVID_Get(endpoint_sk_emsboard_leftlowerarm, dummy, skinNVindex_sconfig__sigmode);
	EOnv 		*nvRoot;
	nvRoot = transceiver->getNVhandler(endpoint_sk_emsboard_leftlowerarm, signal);

	if(NULL == nvRoot)
	{
		printf("\n>>> ERROR \ntransceiver->getNVhandler returned NULL!!\n");
		return;
	}
	uint8_t dat = 1;
	if( eores_OK != eo_nv_Set(nvRoot, &dat, eobool_true, eo_nv_upd_dontdo))
	{
		printf("\n>>> ERROR \neo_nv_Set!!\n");
		return;
	}
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, endpoint_sk_emsboard_leftlowerarm, signal);

	eOnvID_t nvid_ropsigcfgassign = eo_cfg_nvsEP_mn_comm_NVID_Get(endpoint_mn_comm, dummy, commNVindex__ropsigcfgcommand);
	cnv = transceiver->getNVhandler(endpoint_mn_comm, nvid_ropsigcfgassign);
	ropsigcfgassign = (eOmn_ropsigcfg_command_t*) cnv->loc;
	array = (EOarray*) &ropsigcfgassign->array;
	eo_array_Reset(array);
	array->head.capacity = NUMOFROPSIGCFG;
	array->head.itemsize = sizeof(eOropSIGcfg_t);
	ropsigcfgassign->cmmnd = ropsigcfg_cmd_assign;


	/*
	sigcfg.ep = endpoint_mc_rightlowerarm;
	nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(sigcfg.ep, 0, jointNVindex_jstatus__basic);
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);
	 */


	eOnvID_t nvid;
	sigcfg.ep = endpoint_sk_emsboard_leftlowerarm;
	nvid = eo_cfg_nvsEP_sk_NVID_Get(endpoint_sk_emsboard_leftlowerarm, 0, skinNVindex_sstatus__arrayof10canframe);
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

	transceiver->load_occasional_rop(eo_ropcode_set, endpoint_mn_comm, nvid_ropsigcfgassign);

	printf("added a set<__upto10rop2signal, list>");
}

void s_callback_button_3(void )
{
	char str[128];
	EOnv 		*nvRoot;
	eOresult_t res;

	//--
	eOcfg_nvsEP_mc_endpoint_t ep = endpoint_mc_rightlowerarm;
	eOcfg_nvsEP_mc_jointNumber_t j = 0;
	eOcfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jconfig__maxpositionofjoint;

	// get nvid from parameters
	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);

	eOmc_joint_config_t jData;
	memset(&jData, 0x00, sizeof(eOmc_joint_config_t));
	jData.pidposition.kp 				= 0x1AAA;
	jData.pidposition.ki 				= 0x1BBB;
	jData.pidposition.kd 				= 0x1CCC;
	jData.pidposition.limitonintegral 	= 0x1DDD;
	jData.pidposition.limitonoutput 	= 0X1EEE;
	jData.pidposition.offset 			= 0X1FFF;
	jData.pidposition.scale 			= 0X11;
	jData.minpositionofjoint			= 0x22333322;
	jData.maxpositionofjoint			= 0x44555544;
	if( eores_OK != eo_nv_Set(nvRoot, &jData, eobool_true, eo_nv_upd_dontdo))
		printf("error!!");
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);

	//--
	// get nvid from parameters
	eOcfg_nvsEP_mc_motorNVindex_t mNVindex = motorNVindex_mconfig;
	nvid = eo_cfg_nvsEP_mc_motor_NVID_Get(ep, j, mNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);

	eOmc_motor_config_t mData;
	memset(&mData, 0x00, sizeof(eOmc_motor_config_t));
	mData.pidcurrent.kp					= 0x6AAA;
	mData.pidcurrent.ki					= 0x6BBB;
	mData.pidcurrent.kd					= 0x6CCC;
	mData.pidcurrent.limitonintegral	= 0x6DDD;
	mData.pidcurrent.limitonoutput		= 0x6EEE;
	mData.pidcurrent.offset				= 0x6FFF;
	mData.pidcurrent.scale				= 0x66;
	mData.maxvelocityofmotor			= 0x77888877;
	mData.maxcurrentofmotor				= 0x9999;
	if( eores_OK != eo_nv_Set(nvRoot, &mData, eobool_true, eo_nv_upd_dontdo))
		printf("error!!");
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);

	snprintf(str, sizeof(str)-1, "called set motor and joint config, index %d\n", j);
}

void s_callback_button_4(void )
{
	char str[128];
	EOnv 		*nvRoot;
	eOmeas_position_t jDataPos;
	memset(&jDataPos, 0x00, sizeof(eOmeas_position_t));

	// joint Max Position
	eOcfg_nvsEP_mc_endpoint_t ep = endpoint_mc_rightlowerarm;
	eOcfg_nvsEP_mc_jointNumber_t j = 0;
	eOcfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jconfig__maxpositionofjoint;
	// get nvid from parameters
	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);
	// Set-up new value
	jDataPos					= 0x44556677;
	if( eores_OK != eo_nv_Set(nvRoot, &jDataPos, eobool_true, eo_nv_upd_dontdo))
		printf("error!!");
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);

	// Motor qualcosa
	//eo_cfg_nvsEP_mc_endpoint_t ep = endpoint_mc_rightlowerarm;
	eOcfg_nvsEP_mc_motorNumber_t j2 = 0;
	eOcfg_nvsEP_mc_motorNVindex_t jNVindex2 = motorNVindex_mconfig__maxcurrentofmotor;
	memset(&jDataPos, 0x00, sizeof(eOmeas_position_t));
	// get nvid from parameters
	nvid = eo_cfg_nvsEP_mc_motor_NVID_Get(ep, j2, jNVindex2);
	nvRoot = transceiver->getNVhandler(ep, nvid);
	// Set-up new value
	jDataPos					= 0xC1A0DAAC;
	if( eores_OK != eo_nv_Set(nvRoot, &jDataPos, eobool_true, eo_nv_upd_dontdo))
		printf("error!!");
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);

	snprintf(str, sizeof(str)-1, "called set jointNVindex_jconfig__maxpositionofjoint\n");
}

void s_callback_button_5(void)
{
	char str[128];
	EOnv 		*nvRoot;

	eOcfg_nvsEP_mc_endpoint_t ep = endpoint_mc_rightlowerarm;
	eOcfg_nvsEP_mc_jointNumber_t j = 0;
	eOcfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jcmmnds__setpoint;

	// get nvid from parameters
	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);

	eOmc_setpoint_t setPoint;
	setPoint.type = eomc_setpoint_position;
	setPoint.to.position.value 			= 0x0610;
	setPoint.to.position.withvelocity 	= 0x6996;

	eo_nv_Set(nvRoot, &setPoint, eo_nv_upd_ifneeded, eo_nv_upd_dontdo);
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);

	snprintf(str, sizeof(str)-1, "called setPoint position\n");
}

void s_callback_button_6(void )
{
	char str[128];
	int j = 0;

	// weak up EMS
#ifdef _LINUX_UDP_SOCKET_
	// not used in this debug
#else
	str[0] = 0x01;

	ACE_socket->send(str, 1, remote02.addr, flags);
	//sleep(5);

	str[0] = 0x02;
	ACE_socket->send(str, 1, remote02.addr, flags);
#endif
	snprintf(str, sizeof(str)-1, "called weak up EMS\n", j);
}


void s_callback_button_7(void )
{

}

void s_callback_button_8(void )
{

}

void s_callback_button_9(void )
{

}

/*
 * vale_buttons.cpp
 *
 *  Created on: Oct 1, 2012
 *      Author: icub
 */

#include "main-transceiver.hpp"



bool enableSender = false;

// --------------------------------------------------------------------------------------------------------------------
// - definition of callback functions
// --------------------------------------------------------------------------------------------------------------------
static eOcfg_nvsEP_mc_endpoint_t s_get_mc_ep_byBoardNum(uint8_t boardnum);

void commands(void)
{
	printf("\nq: quit\n");
	printf("0: send wake up packet -> set appl state to running\n");
	printf("1: configure regulars  R/L ARM \n");
	printf("2: configure skin  R/L ARM \n");
	printf("3: configure mais\n");
	printf("4 <jId>: configure joint and motor with id=jId\n");
	printf("5: send position setPoint\n");
	printf("\n");
}

// say to ems to go in running state
void s_callback_button_0(void)
{
	char str[128];
	eOcfg_nvsEP_mn_applNumber_t dummy = 0;
	eOnvID_t nvid_go2state 		= eo_cfg_nvsEP_mn_appl_NVID_Get(endpoint_mn_appl, dummy, applNVindex_cmmnds__go2state);
	EOnv 	*nv_p 				= transceiver->getNVhandler(endpoint_mn_appl, nvid_go2state);
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
	eOcfg_nvsEP_mc_endpoint_t   ep;

	printf("Setting regulars!\n");


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

//skin
	sigcfg.ep = ep;
	sigcfg.id = eo_cfg_nvsEP_sk_NVID_Get((eOcfg_nvsEP_sk_endpoint_t)sigcfg.ep, 0, skinNVindex_sstatus__arrayof10canframe);
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

//mais
	sigcfg.ep = ep;
	sigcfg.id = eo_cfg_nvsEP_as_mais_NVID_Get((eOcfg_nvsEP_as_endpoint_t)sigcfg.ep, dummy, maisNVindex_mstatus__the15values);
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

	transceiver->load_occasional_rop(eo_ropcode_set, endpoint_mn_comm, nvid_ropsigcfgassign);
	//	fine della configurazione delle variabili da segnalare spontaneamente : commNVindex__ropsigcfgcommand
}

//skin config
void s_callback_button_2(void)
{
	char str[128];
	EOnv 						*cnv;
	eOmn_ropsigcfg_command_t 	*ropsigcfgassign;
	EOarray						*array;
	eOropSIGcfg_t 				sigcfg;
	eOcfg_nvsEP_mn_commNumber_t dummy = 0;
	eOnvID_t 					nvid = -1;
	EOnv 						*nvRoot = NULL;

	printf("send skin config!\n");

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

}

//configure mais
void s_callback_button_3(void)
{
	eOnvID_t 					nvid = -1;
	EOnv 						*nvRoot = NULL;
	eOcfg_nvsEP_as_endpoint_t   ep;
	uint8_t						maisnum = 0;
	uint8_t                     datarate = 1; //1 milli
	eOsnsr_maismode_t			maismode = snsr_maismode_txdatacontinuously;

	printf("config mais!\n");

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

	//set mais datarate = 1millisec
	nvid = eo_cfg_nvsEP_as_mais_NVID_Get((eOcfg_nvsEP_as_endpoint_t)ep, maisnum, maisNVindex_mconfig__datarate);
	nvRoot = transceiver->getNVhandler(ep, nvid);
	if( eores_OK != eo_nv_Set(nvRoot, &datarate, eobool_true, eo_nv_upd_dontdo))
		printf("error!!");
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);

	//set tx mode continuosly
	nvid = eo_cfg_nvsEP_as_mais_NVID_Get((eOcfg_nvsEP_as_endpoint_t)ep, maisnum, maisNVindex_mconfig__mode);
	nvRoot = transceiver->getNVhandler(ep, nvid);
	if( eores_OK != eo_nv_Set(nvRoot, &maismode, eobool_true, eo_nv_upd_dontdo))
		printf("error!!");
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);
}

//configure joint and motor with id=jId
void s_callback_button_4(void )
{
	char 							str[128];
	EOnv 							*nvRoot;
	eOresult_t 						res;
	eOcfg_nvsEP_mc_endpoint_t 		ep;
	eOcfg_nvsEP_mc_jointNumber_t 	j = 0;
	eOcfg_nvsEP_mc_jointNVindex_t 	jNVindex = jointNVindex_jconfig;
	uint16_t 						maxNumOfJointInEp;
	eOmc_joint_config_t 			jData;
	eOmc_motor_config_t 			mData;

	// 1) get correct ep
	ep = s_get_mc_ep_byBoardNum(boardN);

	// 2) get jid
	maxNumOfJointInEp = eo_cfg_nvsEP_mc_joint_numbermax_Get(ep);

	char pressedkey = getchar();
	if (pressedkey != '\n')
	{
		getchar();
		j = atoi(&pressedkey);
	}

	printf("joint to cfg is %d, max is %d\n", j, maxNumOfJointInEp);
	if(j >= maxNumOfJointInEp)
	{
		j = 0;
	}



	// 3) get nvid from parameters
	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);


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

void s_callback_button_5(void )
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

void s_callback_button_6(void)
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

void s_callback_button_7(void )
{
	char str[128];
	int j = 0;

	// weak up EMS
//	str[0] = 0x01;
//
//	ACE_socket->send(str, 1, remote02.addr, flags);
//	//sleep(5);
//
//	str[0] = 0x02;
//	ACE_socket->send(str, 1, remote02.addr, flags);
//
//	snprintf(str, sizeof(str)-1, "called weak up EMS\n", j);
}


void s_callback_button_8(void )
{
	char str[128];
	EOnv 		*nvRoot;
	eOresult_t res;
	eOcfg_nvsEP_mc_endpoint_t ep;
	eOcfg_nvsEP_mc_jointNumber_t j = 0;
	eOcfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jconfig;
	uint16_t maxNumOfJointInEp;
	eOmc_joint_config_t jData;

	EOnv 						*cnv;
	eOmn_ropsigcfg_command_t 	*ropsigcfgassign;
	EOarray						*array;
	eOropSIGcfg_t 				sigcfg;
	eOcfg_nvsEP_mn_commNumber_t dummy = 0;
	eOnvID_t 					nvid = -1;


	//1) get correct ep
	switch (boardN )
	{
	case 2:	// left
		ep = endpoint_mc_leftlowerarm;
		printf("left\n");
		break;

	case 4:	// right
		ep = endpoint_mc_rightlowerarm;
		printf("right\n");
		break;
	default:
		ep = endpoint_mc_rightlowerarm;
	    printf("defualt - right\n");
	    break;
	}


	//2) get max num of joint for this ep
	maxNumOfJointInEp = eo_cfg_nvsEP_mc_joint_numbermax_Get(ep);

	//3) set values to joint
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


	//4) for each joint in this ep set test data and load acccasional rop
	for(j = 0; j<5/*maxNumOfJointInEp*/; j++)
	{
		// get nvid from parameters
		eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
		nvRoot = transceiver->getNVhandler(ep, nvid);

		//set value of pid position
		jData.pidposition.kp =  ((j & 0xF) << 12) | (jData.pidposition.kp &0x0FFF);
		jData.pidposition.ki =  ((j & 0xF) << 12) | (jData.pidposition.ki &0x0FFF);
		jData.pidposition.kd =  ((j & 0xF) << 12) | (jData.pidposition.kd &0x0FFF);
		if( eores_OK != eo_nv_Set(nvRoot, &jData, eobool_true, eo_nv_upd_dontdo))
			printf("error!!");

		// tell agent to prepare a rop to send
		transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);
	}


	//
	// 7: Segnalazione spontanea
	//
	eOnvID_t nvid_ropsigcfgassign = eo_cfg_nvsEP_mn_comm_NVID_Get(endpoint_mn_comm, dummy, commNVindex__ropsigcfgcommand);
	cnv = transceiver->getNVhandler(endpoint_mn_comm, nvid_ropsigcfgassign);
	ropsigcfgassign = (eOmn_ropsigcfg_command_t*) cnv->loc;
	array = (EOarray*) &ropsigcfgassign->array;
	eo_array_Reset(array);
	array->head.capacity = NUMOFROPSIGCFG;
	array->head.itemsize = sizeof(eOropSIGcfg_t);
	ropsigcfgassign->cmmnd = ropsigcfg_cmd_assign;

	for(j = 0; j<maxNumOfJointInEp; j++)
	{
		nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jointNVindex_jconfig__pidposition);
		sigcfg.ep = ep;
		sigcfg.id = nvid;
		sigcfg.plustime = 0;
		eo_array_PushBack(array, &sigcfg);
	}
	transceiver->load_occasional_rop(eo_ropcode_set, endpoint_mn_comm, nvid_ropsigcfgassign);




//	//--
//	// get nvid from parameters
//	eOcfg_nvsEP_mc_motorNVindex_t mNVindex = motorNVindex_mconfig;
//	nvid = eo_cfg_nvsEP_mc_motor_NVID_Get(ep, j, mNVindex);
//	nvRoot = transceiver->getNVhandler(ep, nvid);
//
//	eOmc_motor_config_t mData;
//	memset(&mData, 0x00, sizeof(eOmc_motor_config_t));
//	mData.pidcurrent.kp					= 0x6AAA;
//	mData.pidcurrent.ki					= 0x6BBB;
//	mData.pidcurrent.kd					= 0x6CCC;
//	mData.pidcurrent.limitonintegral	= 0x6DDD;
//	mData.pidcurrent.limitonoutput		= 0x6EEE;
//	mData.pidcurrent.offset				= 0x6FFF;
//	mData.pidcurrent.scale				= 0x66;
//	mData.maxvelocityofmotor			= 0x77888877;
//	mData.maxcurrentofmotor				= 0x9999;
//	if( eores_OK != eo_nv_Set(nvRoot, &mData, eobool_true, eo_nv_upd_dontdo))
//		printf("error!!");
//	// tell agent to prepare a rop to send
//	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);
//
//	snprintf(str, sizeof(str)-1, "called set motor and joint config, index %d\n", j);
}

void s_callback_button_9(void )
{
	printf("not implemented\n");
}


static eOcfg_nvsEP_mc_endpoint_t s_get_mc_ep_byBoardNum(uint8_t boardnum)
{
	static eOcfg_nvsEP_mc_endpoint_t ep_map[9] =
	{
			endpoint_mc_leftupperarm,   /**< used by EB1 board */
			endpoint_mc_leftlowerarm,   /**< used by EB2 board */
			endpoint_mc_rightupperarm,  /**< used by EB3 board */
			endpoint_mc_rightlowerarm,  /**< used by EB4 board */
			endpoint_mc_torso,   		/**< used by EB5 board */
			endpoint_mc_leftupperleg,   /**< used by EB6 board */
			endpoint_mc_leftlowerleg,   /**< used by EB7 board */
			endpoint_mc_rightupperleg,  /**< used by EB8 board */
			endpoint_mc_rightlowerleg   /**< used by EB9 board */
	};
	//boardnum is in [1,9] ==> -1
	return(ep_map[(boardnum-1)]);

}

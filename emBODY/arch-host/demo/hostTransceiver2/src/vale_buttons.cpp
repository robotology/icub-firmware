/*
 * vale_buttons.cpp
 *
 *  Created on: Oct 1, 2012
 *      Author: icub
 */

#include "main-transceiver.hpp"



bool 		enableSender = false;
double		encoderconvfactor = 1; //configured during joint configuration
double		encoderconvoffset = 0; //configured during joint configuration


// --------------------------------------------------------------------------------------------------------------------
// - definition of callback functions
// --------------------------------------------------------------------------------------------------------------------
static eOcfg_nvsEP_mc_endpoint_t s_get_mc_ep_byBoardNum(uint8_t boardnum);
static eOcfg_nvsEP_as_endpoint_t s_get_as_ep_byBoardNum(uint8_t boardnum);
static eOcfg_nvsEP_sk_endpoint_t s_get_sk_ep_byBoardNum(uint8_t boardnum);
static bool s_getJointNum_frominput(eOcfg_nvsEP_mc_endpoint_t ep, eOcfg_nvsEP_mc_jointNumber_t 	*j);
static bool s_getMotorNum_frominput(eOcfg_nvsEP_mc_endpoint_t ep, eOcfg_nvsEP_mc_motorNumber_t 	*m);
static bool s_get_pidvalues_frominput(eOmc_PID_t *pid_ptr);
static int32_t s_posA2E(eOmeas_position_t posA, double covfactor, double convoffset);

void commands(void)
{
	printf("\nq: quit\n");
	printf("0: send wake up packet -> set appl state to running\n");
	printf("1: configure regulars \n");
	printf("2: configure skin  R/L ARM \n");
	printf("3: configure mais\n");
	printf("4: send a static config to joint and motor with id=jId\n");
	printf("5: config joint with user param\n");
	printf("6: send pid to a motor or a joint\n");
	printf("7: config motor\n");

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

//configure regulars
void s_callback_button_1(void)
{
	char str[128];
	EOnv 							*cnv;
	eOmn_ropsigcfg_command_t 		*ropsigcfgassign;
	EOarray							*array;
	eOropSIGcfg_t 					sigcfg;
	eOcfg_nvsEP_mn_commNumber_t 	dummy = 0;
	eOnvID_t 						nvid = -1;
	EOnv 							*nvRoot = NULL;

	printf("Setting regulars!\n");

	// get array of sif cfg
	eOnvID_t nvid_ropsigcfgassign = eo_cfg_nvsEP_mn_comm_NVID_Get(endpoint_mn_comm, dummy, commNVindex__ropsigcfgcommand);
	cnv = transceiver->getNVhandler(endpoint_mn_comm, nvid_ropsigcfgassign);
	ropsigcfgassign = (eOmn_ropsigcfg_command_t*) cnv->loc;
	array = (EOarray*) &ropsigcfgassign->array;
	eo_array_Reset(array);
	array->head.capacity = NUMOFROPSIGCFG;
	array->head.itemsize = sizeof(eOropSIGcfg_t);
	ropsigcfgassign->cmmnd = ropsigcfg_cmd_assign;



	printf("add mc regulars? [y/n]\n");
	gets(str);
	if((strcmp("y", str) == 0 ))
	{
		eOcfg_nvsEP_mc_jointNumber_t 	j = 0;
		eOcfg_nvsEP_mc_motorNumber_t 	m = 0;

		// 1) get correct ep
		sigcfg.ep = s_get_mc_ep_byBoardNum(boardN);

		// 2) get jid
		if(!s_getJointNum_frominput((eOcfg_nvsEP_mc_endpoint_t)sigcfg.ep, &j))
		{
			return;
		}

		// 3) get mid
		if(!s_getMotorNum_frominput((eOcfg_nvsEP_mc_endpoint_t)sigcfg.ep, &m))
		{
			return;
		}

		nvid = eo_cfg_nvsEP_mc_joint_NVID_Get((eOcfg_nvsEP_mc_endpoint_t)sigcfg.ep, j, jointNVindex_jstatus__basic);
		sigcfg.id = nvid;
		sigcfg.plustime = 0;
		eo_array_PushBack(array, &sigcfg);

		nvid = eo_cfg_nvsEP_mc_joint_NVID_Get((eOcfg_nvsEP_mc_endpoint_t)sigcfg.ep, j, jointNVindex_jstatus__ofpid);
		sigcfg.id = nvid;
		sigcfg.plustime = 0;
		eo_array_PushBack(array, &sigcfg);

		nvid = eo_cfg_nvsEP_mc_motor_NVID_Get((eOcfg_nvsEP_mc_endpoint_t)sigcfg.ep, m, motorNVindex_mstatus__basic);
		sigcfg.id = nvid;
		sigcfg.plustime = 0;
		eo_array_PushBack(array, &sigcfg);
	}


	printf("add skin regulars? [y/n]\n");
	gets(str);
	if((strcmp("y", str) == 0 ))
	{
		eOcfg_nvsEP_sk_skinNumber_t 	sk = 0;

		sigcfg.ep = s_get_sk_ep_byBoardNum(boardN);
		sigcfg.id = eo_cfg_nvsEP_sk_NVID_Get((eOcfg_nvsEP_sk_endpoint_t)sigcfg.ep, sk, skinNVindex_sstatus__arrayof10canframe);
		sigcfg.plustime = 0;
		eo_array_PushBack(array, &sigcfg);

	}


	printf("add mais regulars? [y/n]\n");
	gets(str);
	if((strcmp("y", str) == 0 ))
	{
		eOcfg_nvsEP_as_maisNumber_t 	s = 0;

		sigcfg.ep = s_get_as_ep_byBoardNum(boardN);
		sigcfg.id = eo_cfg_nvsEP_as_mais_NVID_Get((eOcfg_nvsEP_as_endpoint_t)sigcfg.ep, s, maisNVindex_mstatus__the15values);
		sigcfg.plustime = 0;
		eo_array_PushBack(array, &sigcfg);

	}

	transceiver->load_occasional_rop(eo_ropcode_set, endpoint_mn_comm, nvid_ropsigcfgassign);
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
	uint8_t                     datarate = 10; //10 milli (like in icub_right_arm_safe.ini)
	eOsnsr_maismode_t			maismode = snsr_maismode_txdatacontinuously;

	printf("config mais!\n");

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
	char							numjointstr[5];
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
	if(!s_getJointNum_frominput(ep, &j))
	{
		return;
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
	char							str[30];
	char							numjointstr[5];
	EOnv 							*nvRoot;
	eOresult_t 						res;
	eOcfg_nvsEP_mc_endpoint_t 		ep;
	eOcfg_nvsEP_mc_jointNumber_t 	j = 0;
	eOcfg_nvsEP_mc_jointNVindex_t 	jNVindex = jointNVindex_jconfig;
	eOmc_joint_config_t 			jData;

	memset(&jData, 0x00, sizeof(eOmc_joint_config_t));

	// 1) get correct ep
	ep = s_get_mc_ep_byBoardNum(boardN);

	// 2) get jid
	if(!s_getJointNum_frominput(ep, &j))
	{
		return;
	}

	// 3) get pos pid
	if(! (s_get_pidvalues_frominput(&jData.pidposition)))
	{
		return;
	}

	// 4) get min pos of joint
	printf("min position of joint: ");
	gets(str);
	jData.minpositionofjoint = atoi(str);

	// 5) get min pos of joint
	printf("max position of joint: ");
	gets(str);
	jData.maxpositionofjoint = atoi(str);

	// 6) get setpoint timeout
	printf("setpoint timeout: ");
	gets(str);
	jData.velocitysetpointtimeout = atoi(str);

	// 6) get encoder conversion factor
	printf("encoder conversion factor: ");
	gets(str);
//	jData.encoderconversionfactor = atoi(str);
	encoderconvfactor = atoi(str);
	jData.encoderconversionfactor = 1;

	// 7) get encoder conversion factor
	printf("encoder conversion offset: ");
	gets(str);
//	jData.encoderconversionoffset = atoi(str);
	encoderconvoffset = atoi(str);
	jData.encoderconversionoffset = 0;

	jData.motionmonitormode = eomc_motionmonitormode_dontmonitor;

	// 8) conversion of position values from degree to enc ticks
	if((boardN == 2) || (boardN ==4))
	{
		jData.minpositionofjoint = s_posA2E(jData.minpositionofjoint, encoderconvfactor, encoderconvoffset);
		jData.maxpositionofjoint = s_posA2E(jData.maxpositionofjoint, encoderconvfactor, encoderconvoffset);
	}
	// 9) prepare data to send
	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);
	if( eores_OK != eo_nv_Set(nvRoot, &jData, eobool_true, eo_nv_upd_dontdo))
		printf("error in nv set\n ");
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);

}

// send pid to a motor or a joint
void s_callback_button_6(void)
{
	char							str[30];
	char							numjointstr[5];
	EOnv 							*nvRoot;
	eOresult_t 						res;
	eOnvID_t 						nvid;
	eOmc_PID_t 						pid;
	eOcfg_nvsEP_mc_endpoint_t 		ep;

	memset(&pid, 0x00, sizeof(eOmc_PID_t));

	printf("send pid to joint? [y/n]\n");
	gets(str);
	if((strcmp("y", str) == 0 ))
	{
		eOcfg_nvsEP_mc_jointNumber_t 	j = 0;
		eOcfg_nvsEP_mc_jointNVindex_t 	jNVindex = jointNVindex_jconfig__pidposition;

		// 1) get correct ep
		ep = s_get_mc_ep_byBoardNum(boardN);

		// 2) get jid
		if(!s_getJointNum_frominput(ep, &j))
		{
			return;
		}

		nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
	}
	else //motor
	{

		eOcfg_nvsEP_mc_motorNumber_t 	m = 0;
		eOcfg_nvsEP_mc_motorNVindex_t 	mNVindex = motorNVindex_mconfig__pidcurrent;

		// 1) get correct ep
		ep = s_get_mc_ep_byBoardNum(boardN);

		// 2) get mid
		if(!s_getMotorNum_frominput(ep, &m))
		{
			return;
		}
		nvid = eo_cfg_nvsEP_mc_motor_NVID_Get(ep, m, mNVindex);

	}


	// 3) get pos pid
	if(! (s_get_pidvalues_frominput(&pid)))
	{
		return;
	}

	// 9) prepare data to send
	nvRoot = transceiver->getNVhandler(ep, nvid);
	if( eores_OK != eo_nv_Set(nvRoot, &pid, eobool_true, eo_nv_upd_dontdo))
		printf("error in nv set\n ");
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);

}

void s_callback_button_7(void )
{
	char							str[30];
	char							numjointstr[5];
	EOnv 							*nvRoot;
	eOresult_t 						res;
	eOcfg_nvsEP_mc_endpoint_t 		ep;
	eOcfg_nvsEP_mc_motorNumber_t 	m = 0;
	eOcfg_nvsEP_mc_motorNVindex_t 	mNVindex = motorNVindex_mconfig;
	eOmc_motor_config_t 			mData;

	memset(&mData, 0x00, sizeof(eOmc_motor_config_t));

	// 1) get correct ep
	ep = s_get_mc_ep_byBoardNum(boardN);

	// 2) get jid
	if(!s_getMotorNum_frominput(ep, &m))
	{
		return;
	}

	// 3) get current pid
	if(! (s_get_pidvalues_frominput(&mData.pidcurrent)))
	{
		return;
	}

	// 4) get max velocity of motor
	printf("get max velocity of motor: ");
	gets(str);
	mData.maxvelocityofmotor = atoi(str);

	// 5) get min pos of joint
	printf("get max current of motor: ");
	gets(str);
	mData.maxcurrentofmotor = atoi(str);


	// 6) prepare data to send
	eOnvID_t nvid = eo_cfg_nvsEP_mc_motor_NVID_Get(ep, m, mNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);
	if( eores_OK != eo_nv_Set(nvRoot, &mData, eobool_true, eo_nv_upd_dontdo))
		printf("error in nv set\n ");
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);
}


void s_callback_button_8(void )
{
	printf("not implemented\n");
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


static eOcfg_nvsEP_sk_endpoint_t s_get_sk_ep_byBoardNum(uint8_t boardnum)
{
	eOcfg_nvsEP_sk_endpoint_t ep;

	switch (boardnum )
	{
	case 2:  // left
		ep = endpoint_sk_emsboard_leftlowerarm;
		break;

	case 4:	// right
		ep = endpoint_sk_emsboard_rightlowerarm;
		break;
	}

	return(ep);
}

static eOcfg_nvsEP_as_endpoint_t s_get_as_ep_byBoardNum(uint8_t boardnum)
{
	static eOcfg_nvsEP_as_endpoint_t ep_map[9] =
	{
			endpoint_as_leftupperarm,   /**< used by EB1 board */
			endpoint_as_leftlowerarm,   /**< used by EB2 board */
			endpoint_as_rightupperarm,  /**< used by EB3 board */
			endpoint_as_rightlowerarm,  /**< used by EB4 board */
			(eOcfg_nvsEP_as_endpoint_t)0,   						/**< used by EB5 board */
			endpoint_as_leftupperleg,   /**< used by EB6 board */
			(eOcfg_nvsEP_as_endpoint_t)0,						    /**< used by EB7 board */
			endpoint_as_rightupperleg,  /**< used by EB8 board */
			(eOcfg_nvsEP_as_endpoint_t)0   						/**< used by EB9 board */
	};
	//boardnum is in [1,9] ==> -1
	return(ep_map[(boardnum-1)]);

}



static bool s_get_pidvalues_frominput(eOmc_PID_t *pid_ptr)
{
	char							str[30];

	// 3) get pos pid
	printf("Get pid pos:\n");
	printf("	- kp: ");
	gets(str);
	pid_ptr->kp = atoi(str);

	printf("	- ki: ");
	gets(str);
	pid_ptr->ki = atoi(str);

	printf("	- kd: ");
	gets(str);
	pid_ptr->kd = atoi(str);

	printf("	- limit on integral: ");
	gets(str);
	pid_ptr->limitonintegral = atoi(str);

	printf("	- limit on output: ");
	gets(str);
	pid_ptr->limitonoutput = atoi(str);

	printf("	- offset: ");
	gets(str);
	pid_ptr->offset = atoi(str);

	printf("	- scale: ");
	gets(str);
	pid_ptr->scale = atoi(str);

	//check values
	printf("the pid read is: \n");
	printf("	- kp: %d \t", pid_ptr->kp);
	printf("	- ki: %d \t", pid_ptr->ki);
	printf("	- kd: %d \n", pid_ptr->kd);
	printf("	- limit on integral: %d \n", pid_ptr->limitonintegral);
	printf("	- limit on output: %d \n", pid_ptr->limitonoutput);
	printf("	- offset: %d \n", pid_ptr->offset);
	printf("	- scale: %d \n", pid_ptr->scale);

	printf("is it correct? [y/n]\n");
	gets(str);
	if(!(strcmp("y", str) == 0 ))
	{
		printf("Error: pid param not correct! retry.....\n");
		return(false);
	}

	return(true);

}

static bool s_getJointNum_frominput(eOcfg_nvsEP_mc_endpoint_t ep, eOcfg_nvsEP_mc_jointNumber_t 	*j)
{
	char	 str[30];
	uint16_t maxNumOfJointInEp = eo_cfg_nvsEP_mc_joint_numbermax_Get(ep);

	printf("joint num: ");
	gets(str);
	*j = atoi(str);

	printf("joint is %d, max is %d\n", *j, maxNumOfJointInEp);
	if(*j >= maxNumOfJointInEp)
	{
		printf("error: num of joint not valid\n");
		return(false);
	}
	return(true);
}


static bool s_getMotorNum_frominput(eOcfg_nvsEP_mc_endpoint_t ep, eOcfg_nvsEP_mc_motorNumber_t 	*m)
{
	char	 str[30];
	uint16_t maxNumOfMotorInEp = eo_cfg_nvsEP_mc_motor_numbermax_Get(ep);

	printf("motor num: ");
	gets(str);
	*m = atoi(str);

	printf("motor id is %d, max is %d\n", *m, maxNumOfMotorInEp);
	if(*m >= maxNumOfMotorInEp)
	{
		printf("error: num of motor not valid \n");
		return(false);
	}
	return(true);
}

static int32_t s_posA2E(eOmeas_position_t posA, double covfactor, double convoffset)
{
	return((posA + convoffset)*covfactor);
}

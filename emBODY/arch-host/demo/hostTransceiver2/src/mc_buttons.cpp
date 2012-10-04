/*
 * mc_callback.cpp
 *
 *  Created on: Sep 20, 2012
 *      Author: icub
 */

#include "main-transceiver.hpp"

bool enableSender = false;

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

void commands(void)
{
	printf("\nq: quit\n");
	printf("0: send wake up packet -> set appl state to running\n");
	printf("1: position setpoint\n");
	printf("2: velocity setpoint\n");
	printf("3: stop\n");
	printf("4: calibrate\n");
	printf("5: controlmode\n");
    printf("6: set limits\n");
    printf("7: set vel timeout\n");
    printf("8: set pos pid\n");
	printf("\n");
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of callback functions
// --------------------------------------------------------------------------------------------------------------------

// say to ems to go in running state
void s_callback_button_0()
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

// send position setpoint
void s_callback_button_1()
{
    int joint,pos,vel;

    printf("\nsetpos <joint> <pos> <vel>: ");
    scanf("%d %d %d",&joint,&pos,&vel);

	EOnv *nvRoot;

	eOcfg_nvsEP_mc_endpoint_t ep           = s_get_mc_ep_byBoardNum(boardN);
	eOcfg_nvsEP_mc_jointNumber_t j         = joint;
	eOcfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jcmmnds__setpoint;

	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);

	eOmc_setpoint_t setPoint;
	setPoint.type                       = eomc_setpoint_position;
	setPoint.to.position.value 			= pos;
	setPoint.to.position.withvelocity 	= vel;

	if (eores_OK != eo_nv_Set(nvRoot, &setPoint, eobool_true, eo_nv_upd_always)) printf("ERROR\n");

	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);
}

// send velocity setpoint
void s_callback_button_2()
{
    int joint,vel,acc;

    printf("\nsetvel <joint> <vel> <acc>: ");
    scanf("%d %d %d",&joint,&vel,&acc);

	EOnv *nvRoot;

	eOcfg_nvsEP_mc_endpoint_t ep           = s_get_mc_ep_byBoardNum(boardN);
	eOcfg_nvsEP_mc_jointNumber_t j         = joint;
	eOcfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jcmmnds__setpoint;

	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);

	eOmc_setpoint_t setPoint;
	setPoint.type                           = eomc_setpoint_velocity;
	setPoint.to.velocity.value 			    = vel;
	setPoint.to.velocity.withacceleration 	= acc;

	if (eores_OK != eo_nv_Set(nvRoot, &setPoint, eobool_true, eo_nv_upd_always)) printf("ERROR\n");

	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);
}

// send stop
void s_callback_button_3()
{
    int joint;

    printf("\nstop <joint>: ");
    scanf("%d",&joint);

	EOnv *nvRoot;

	eOcfg_nvsEP_mc_endpoint_t ep           = s_get_mc_ep_byBoardNum(boardN);
	eOcfg_nvsEP_mc_jointNumber_t j         = joint;
	eOcfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jcmmnds__stoptrajectory;

	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);

    eObool_t stopCmd = eobool_true;

	if (eores_OK != eo_nv_Set(nvRoot, &stopCmd, eobool_true, eo_nv_upd_always)) printf("ERROR\n");

	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);
}

// send calibrate
void s_callback_button_4()
{
    int joint,pos,vel,offset;

    printf("\ncalibrate <joint> <pos> <vel> <offset>: ");
    scanf("%d %d %d",&joint,&pos,&vel,&offset);    

	EOnv *nvRoot;

	eOcfg_nvsEP_mc_endpoint_t ep           = s_get_mc_ep_byBoardNum(boardN);
	eOcfg_nvsEP_mc_jointNumber_t j         = joint;
	eOcfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jcmmnds__calibration;

	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);

    eOmc_calibrator_t calibData;
	
    calibData.type = eomc_calibration_type3_abs_sens_digital;
    calibData.params.type3.position = pos;
    calibData.params.type3.velocity = vel;
    calibData.params.type3.offset = offset;
	
	if (eores_OK != eo_nv_Set(nvRoot, &calibData, eobool_true, eo_nv_upd_always)) printf("ERROR\n");

	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);
}

// set controlmode
void s_callback_button_5()
{
    int joint,cm;

    printf("\ncontrolmode <joint> <cm>: ");
    scanf("%d %d %d",&joint,&cm);         
    
	EOnv *nvRoot;

	eOcfg_nvsEP_mc_endpoint_t ep           = s_get_mc_ep_byBoardNum(boardN);
	eOcfg_nvsEP_mc_jointNumber_t j         = joint;
	eOcfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jcmmnds__controlmode;

	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);

    eOmc_controlmode_t controlMode = (eOmc_controlmode_t)cm;

	if (eores_OK != eo_nv_Set(nvRoot, &controlMode, eobool_true, eo_nv_upd_always)) printf("ERROR\n");
	
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);
}

// set limits
void s_callback_button_6()
{
    int joint,min,max;

    printf("\nsetlimits <joint> <min> <max>: ");
    scanf("%d %d %d",&joint,&min,&max); 


    // min pos
    {
       	EOnv *nvRoot;

	    eOcfg_nvsEP_mc_endpoint_t ep           = s_get_mc_ep_byBoardNum(boardN);
	    eOcfg_nvsEP_mc_jointNumber_t j         = joint;
	    eOcfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jconfig__minpositionofjoint;

    	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
    	nvRoot = transceiver->getNVhandler(ep, nvid);

        eOmeas_position_t min_position = (eOmeas_position_t)min;

    	if (eores_OK != eo_nv_Set(nvRoot, &min_position, eobool_true, eo_nv_upd_always)) printf("ERROR\n");
	
	    transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);
    }

    // max pos
    {
       	EOnv *nvRoot;

	    eOcfg_nvsEP_mc_endpoint_t ep           = s_get_mc_ep_byBoardNum(boardN);
	    eOcfg_nvsEP_mc_jointNumber_t j         = joint;
	    eOcfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jconfig__maxpositionofjoint;

    	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
    	nvRoot = transceiver->getNVhandler(ep, nvid);

        eOmeas_position_t max_position = (eOmeas_position_t)max;

    	if (eores_OK != eo_nv_Set(nvRoot, &max_position, eobool_true, eo_nv_upd_always)) printf("ERROR\n");
	
	    transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);
    }
}

// set velocity timeout
void s_callback_button_7()
{
    int joint,to;

    printf("\nsettimeout <joint> <tout>: ");
    scanf("%d %d %d",&joint,&to); 

	EOnv *nvRoot;

	eOcfg_nvsEP_mc_endpoint_t ep           = s_get_mc_ep_byBoardNum(boardN);
	eOcfg_nvsEP_mc_jointNumber_t j         = joint;
	eOcfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jconfig__velocitysetpointtimeout;

	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);

    eOmeas_time_t timeout = (eOmeas_time_t)to;

	if (eores_OK != eo_nv_Set(nvRoot, &timeout, eobool_true, eo_nv_upd_always)) printf("ERROR\n");
	
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);
}

// set position PID
void s_callback_button_8()
{
    int joint;
    double Kp,Kd,Ki;
    int Imax,Ymax;

    printf("\nsetpospid <joint> <Kp> <Kd> <Ki> <Imax> <Ymax>: ");
    scanf("%d %f %f %f %d %d",&joint,&Kp,&Kd,&Ki,&Imax,&Ymax); 

	EOnv *nvRoot;

	eOcfg_nvsEP_mc_endpoint_t ep           = s_get_mc_ep_byBoardNum(boardN);
	eOcfg_nvsEP_mc_jointNumber_t j         = joint;
	eOcfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jconfig__pidposition;

	// get nvid from parameters
	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);

    /*
    int16_t                 kp;                 proportional gain
    int16_t                 ki;                 integral gain 
    int16_t                 kd;                 derivative gain 
    int16_t                 limitonintegral;    limit of integral term  
    int16_t                 limitonoutput;      limit of the output of the pid
    int16_t                 offset;             the k0 in the pid formula
    int8_t                  scale;              scale factor for the gains (used for a rigth shift 
    */

    eOmc_PID16_t pidConfig;

    pidConfig.kp = int(65536.0*Kp);
    pidConfig.ki = int(65536.0*Ki);
    pidConfig.kd = int(65536.0*Kd);

    pidConfig.limitonintegral = Imax;
    pidConfig.limitonoutput   = Ymax;
    pidConfig.offset = 0;
    pidConfig.scale  = 16;
	
	if (eores_OK != eo_nv_Set(nvRoot, &pidConfig, eobool_true, eo_nv_upd_always)) printf("ERROR\n");
	
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);
}

void s_callback_button_9()
{
}





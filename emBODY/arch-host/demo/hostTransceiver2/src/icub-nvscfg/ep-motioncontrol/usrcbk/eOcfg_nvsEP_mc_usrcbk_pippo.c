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

/* @file       eOcfg_nvsEP_mc_usrcbk_pippo.c
    @brief     This file keeps examples for ems / pc104 of the user-defined functions used for all endpoints in mc
    @author     marco.accame@iit.it
    @date       05/02/2012
**/

//#warning --> read this very important comment on the different use of group or general callbacks
// very important comment:
// the EOtheAgent calls a default callback which is specific of a given _usr group (upperarm, lowerarm, torso, upperleg, lowerleg).
// inside this callback it is possible to understand which endpoint is (left or right) by evaluating argument ep.
// the default group callback then calls the default motioncontrol callback. the same mechanism is used to understand which is the relevant 
// endpoint.
// the two default callbacks do nothing else.
// the user can override one of the two by simply defining a new function with the same name and declaration.
// if the more specific group callback is redefined, then the more general is not called anymore for those groups which found this overriden.
// RULE TO USE: override only the general or the only the group ones. If the genral ones are overridden it is possible to write more general
//              code inside the ems.

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"
#include "stdint.h"

#include "EoCommon.h"
#include "eOcfg_nvsEP_mc_hid.h"
#include "EOnv_hid.h"
#include "eOcfg_nvsEP_mc_overridden_pc104.h"

typedef struct
{
    uint32_t dummy1;
    uint32_t dummy2;
} MYmotionController;

static void motioncontroller_setpoint(MYmotionController *m, eOmc_setpoint_t *setp)
{
    switch(setp->type)
    {
        case eomc_setpoint_position:    
        {
            m->dummy1 = setp->to.position.value;
            m->dummy2 = setp->to.position.withvelocity;
        } break;
        
        default:
        {
            
        } break;
    }
}

static MYmotionController themotioncontrollers[3];


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
#define isOverCurrent(status)						((status) & (0x08))
#define isFaultUndervoltage(status)					((status) & (0x01))
#define isFaultExternal(status)						((status) & (0x04))
#define isFaultOverload(status)						((status) & 0x02)
#define isHallSensorError(status)					((status) & 0x10)
#define isAbsEncoderError(status)					((status) & 0x20)
//can fault
#define isCanTxOverflow(status)						((status) & 0x01)
#define isCanBusOff(status)							((status) & 0x02)
#define isCanTxError(status)						((status) & 0x04)
#define isCanRxError(status)						((status) & 0x08)
#define isCanTxOverrun(status)						((status) & 0x10)
#define isCanRxWarning(status)						((status) & 0x20)
#define isCanRxOverrun(status)						((status) & 0x4)

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
	eOmc_joint_status_t *jstatus = nv->rem;
	printf("joint %d position %d\n", jxx, jstatus->basic.position);

	printf("joint %d fault: ", jxx);
	if(isOverCurrent(jstatus->chamaleon04[0]))
		printf("overcurrent, ");
	if(isFaultUndervoltage(jstatus->chamaleon04[0]))
		printf("Undervoltage, ");
	if(isFaultExternal(jstatus->chamaleon04[0]))
		printf("FaultExternal, ");
	if(isFaultOverload(jstatus->chamaleon04[0]))
		printf("FaultOverload, ");
	if(isHallSensorError(jstatus->chamaleon04[0]))
		printf("HallSensorError, ");
	if(isAbsEncoderError(jstatus->chamaleon04[0]))
		printf("AbsEncoderError, ");
//can fault
	if(isCanTxOverflow(jstatus->chamaleon04[1]))
		printf("CanTxOverflow, ");
	if(isCanBusOff(jstatus->chamaleon04[1]))
		printf("CanBusOff, ");
	if(isCanTxError(jstatus->chamaleon04[1]))
		printf("isCanTxError, ");
	if(isCanTxOverrun(jstatus->chamaleon04[1]))
		printf("CanTxOverrun, ");
	if(isCanRxWarning(jstatus->chamaleon04[1]))
		printf("isCanRxWarning, ");
	if(isCanRxOverrun(jstatus->chamaleon04[1]))
		printf("isCanRxOverrun, ");

	printf("\n");

}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__basic(eOcfg_nvsEP_mc_jointNumber_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
	eOmc_joint_status_basic_t *jstatus_b = nv->rem;
	printf("\njstatus__basic for Joint num = %d\n", xx);
	printf("ep = 0x%X\n", nv->ep);
	printf("jstatus_b->acceleration = 0x%X\n", jstatus_b->acceleration);
	printf("jstatus_b->controlmodestatus = 0x%X\n", jstatus_b->controlmodestatus);
	printf("jstatus_b->motionmonitorstatus = 0x%X\n", jstatus_b->motionmonitorstatus);
	printf("jstatus_b->position = 0x%X\n", jstatus_b->position);
	printf("jstatus_b->torque = 0x%X\n", jstatus_b->torque);
	printf("jstatus_b->velocity = 0x%X\n", jstatus_b->velocity);
}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus__basic(eOcfg_nvsEP_mc_motorNumber_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
	eOmc_motor_status_basic_t *mstatus_b = nv->rem;
	printf("\nmstatus__basic for motor num = %d\n", xx);
	printf("ep = 0x%X\n", nv->ep);
	printf("mstatus_b->current  = 0x%X\n", mstatus_b->current);
	printf("mstatus_b->filler02 = 0x%X\n", *mstatus_b->filler02);
	printf("mstatus_b->position = 0x%X\n", mstatus_b->position);
	printf("mstatus_b->velocity = 0x%X\n", mstatus_b->velocity);
}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig(eOcfg_nvsEP_mc_jointNumber_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
	eOmc_joint_config_t *jConfig = nv->rem;
	printf("\nmaxpositionofjoint for Joint num = %d\n", xx);
	printf("jConfig->pidposition.kp 	= 0x%X\n",	jConfig->pidposition.kp		);
    printf("jConfig->pidposition.ki 	= 0x%X\n",	jConfig->pidposition.ki		);
    printf("jConfig->pidposition.kd 	= 0x%X\n",		jConfig->pidposition.kd);
    printf("jConfig->pidposition.limitonintegral = 0x%X\n",	jConfig->pidposition.limitonintegral);
    printf("jConfig->pidposition.limitonoutput = 0x%X\n",	jConfig->pidposition.limitonoutput);
    printf("jConfig->pidposition.offset 	= 0x%X\n",		jConfig->pidposition.offset);
    printf("jConfig->pidposition.scale 		= 0x%X\n",	jConfig->pidposition.scale);
    printf("jConfig->minpositionofjoint		= 0x%X\n",	jConfig->minpositionofjoint);
	printf("jConfig->maxpositionofjoint		= 0x%X\n",	jConfig->maxpositionofjoint);
	printf("ep = 0x%X\n", nv->ep);

}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig(eOcfg_nvsEP_mc_motorNumber_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
	eOmc_motor_config_t *jConfig = nv->rem;
	printf("\nmaxpositionofjoint for Joint num = %d\n", xx);
	printf("mConfig->pidcurrent.kp 	= 0x%X\n",	jConfig->pidcurrent.kp		);
    printf("mConfig->pidcurrent.ki 	= 0x%X\n",	jConfig->pidcurrent.ki		);
    printf("mConfig->pidcurrent.kd 	= 0x%X\n",		jConfig->pidcurrent.kd);
    printf("mConfig->pidcurrent.limitonintegral = 0x%X\n",	jConfig->pidcurrent.limitonintegral);
    printf("mConfig->pidcurrent.limitonoutput = 0x%X\n",	jConfig->pidcurrent.limitonoutput);
    printf("mConfig->pidcurrent.offset 	= 0x%X\n",		jConfig->pidcurrent.offset);
    printf("mConfig->pidcurrent.scale 		= 0x%X\n",	jConfig->pidcurrent.scale);
    printf("mConfig->maxvelocityofmotor		= 0x%X\n",	jConfig->maxvelocityofmotor);
	printf("mConfig->maxcurrentofmotor		= 0x%X\n",	jConfig->maxcurrentofmotor);
	printf("ep = 0x%X\n", nv->ep);

}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__maxpositionofjoint(eOcfg_nvsEP_mc_jointNumber_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
	eOmeas_position_t *jMaxPosition_b = nv->rem;
	printf("\nmaxpositionofjoint for Joint num = %d\n", xx);
	printf("ep = 0x%X\n", nv->ep);
	printf("maxpositionofjoint = 0x%X\n", *jMaxPosition_b);
}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__maxcurrentofmotor(eOcfg_nvsEP_mc_motorNumber_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
	eOmeas_position_t *jMaxCurrent_b = nv->rem;
	printf("\nmaxcurrentofmotor for Joint num = %d\n", xx);
	printf("ep = 0x%X\n", nv->ep);
	printf("maxcurrentofmotor = 0x%X\n", *jMaxCurrent_b);
}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidposition(eOcfg_nvsEP_mc_motorNumber_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
	printf("jconfig__pidposition Callback\n");
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


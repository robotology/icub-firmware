/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alessandro Scalzo
 * email:   alessandro.scalzo@iit.it
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
 * MERCHANTABILITY or  FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License foFITNESSr more details
*/

#if !defined(__cplusplus)
    #error this Calibrator.c file must be compiled in C++
#endif

// - API
#include "Calibrators.h"
    

// - HIDDEN DATA
// note: it is not an object that has data but we nevertheless keep the _hid.h because ... 
//       in the future we may add in it a pointter to the relevant JointSet
#include "Calibrators_hid.h"

    
// - dependencies    
#include "JointSet.h"
#include "JointSet_hid.h"       // we access internals of the object
#include "Joint.h"
#include "Joint_hid.h"          // we access internals of the object
#include "Motor.h"
#include "Motor_hid.h"          // we access internals of the object
#include "AbsEncoder.h"
#include "AbsEncoder_hid.h"     // we access internals of the object

#include "EOtheEntities.h"
    
#include "CalibrationHelperData.h"

#if defined(USE_EMBOT_theServices) 
//#warning USE_EMBOT_theServices is defined
// marco.accame: use objects embot::app::eth::theEncoderReader and ... future ones
#else
#include "embot_app_eth_theEncoderReader.h"
#include "EOtheMAIS.h"
#include "EOthePOS.h"
#endif

#include "hal_adc.h"

#define CALIB_TYPE_6_POS_TRHESHOLD 730 //= 4 deg //1820 //2730 //546=3 degree //91.02f // = 0.5 degree
#define CALIB_TYPE_6_7_POS_ERROR_TRHESHOLD 14563 //= 80 deg express in icubDeg
#define ROTOR_LIMIT_DELTA (159.0f/360.0f)*65536.0f  // small amount of iCubdegree on the rotor to be slightly distant from hard-stop


// static function declaration

static BOOL JointSet_do_wait_calibration_3(JointSet* o);
static BOOL JointSet_do_wait_calibration_5(JointSet* o);
static BOOL JointSet_do_wait_calibration_8(JointSet* o);
static BOOL JointSet_do_wait_calibration_9(JointSet* o);
static BOOL JointSet_do_wait_calibration_10(JointSet* o);
static BOOL JointSet_do_wait_calibration_11(JointSet* o);
static BOOL JointSet_do_wait_calibration_12(JointSet* o);
static BOOL JointSet_do_wait_calibration_13(JointSet* o);
static BOOL JointSet_do_wait_calibration_14(JointSet* o);
static BOOL JointSet_do_wait_calibration_mixed(JointSet* o); //calib type 6 and 7
static eoas_pos_ROT_t JointSet_calib14_ROT2pos_ROT(eOmc_calib14_ROT_t rot);
    


// extern function definition

extern void Calibrator_calibrate(JointSet* o, uint8_t e, eOmc_calibrator_t *calibrator)
{
//    for (int js=0; js<*(o->pN); ++js)
//    {
//        o->joint[o->joints_of_set[js]].control_mode = eomc_controlmode_calib;
//    }
    
    eOmc_controlmode_t joint_controlMode_old = o->joint[e].control_mode;
    eOmc_controlmode_t jointSet_controlMode_old = o->control_mode;
    
    o->joint[e].control_mode = eomc_controlmode_calib;
    
    o->control_mode = eomc_controlmode_calib;
    
    o->is_calibrated = FALSE;
    
    switch (calibrator->type)
    {
        case eomc_calibration_type3_abs_sens_digital:
        {
            AbsEncoder_calibrate_absolute(o->absEncoder+e, calibrator->params.type3.offset, calibrator->params.type3.calibrationZero);
            
            Motor_calibrate_withOffset(o->motor+e, 0);
            Motor_set_run(o->motor+e, o->postrj_ctrl_out_type);
            o->calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
            
        }
        break;
        
        case eomc_calibration_type5_hard_stops:
        {
            o->calibration_timeout = 0;
            BOOL ret = Motor_calibrate_moving2Hardstop(o->motor+e, calibrator->params.type5.pwmlimit, (calibrator->params.type5.final_pos - calibrator->params.type5.calibrationZero));
            
            if(!ret)
            {
                o->joint[e].control_mode = joint_controlMode_old;
                o->control_mode = jointSet_controlMode_old;
                return;
            }
            
            Joint_set_hardware_limit(o->joint+e);
            
            AbsEncoder_calibrate_fake(o->absEncoder+e);
            o->calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
            
        }
        break;
        
#if defined(USE_EMBOT_theServices) 
//#warning removed some code
#else
        
        case eomc_calibration_type6_mais:
        {
            // 1) check params are ok

            CTRL_UNITS target_pos;
            
            if (calibrator->params.type6.current == 1 )
            {
                target_pos = calibrator->params.type6.vmax;
            }
            else if (calibrator->params.type6.current == -1)
            {
                target_pos = calibrator->params.type6.vmin;
            }
            else
            {
                ////debug code
                char info[70];
                snprintf(info, sizeof(info), "error type6.current=%d",calibrator->params.type6.current);
                JointSet_send_debug_message(info, e, 0, 0);
                ////debug code ended
                return;
            } 
            //if I'm here I can perform calib type 6.
            
            // 2) set state
            o->calibration_timeout = 0;
            o->calibration_in_progress = eomc_calibration_typeMixed;
            o->joint[e].running_calibration.type = (eOmc_calibration_type_t)calibrator->type;
            o->joint[e].running_calibration.data.type6.is_active = TRUE;
            o->joint[e].running_calibration.data.type6.state = calibtype6_st_inited;
            
            
            
            // 3) calculate new joint encoder factor and param_zero
            eOmc_joint_config_t *jconfig = &o->joint[e].eo_joint_ptr->config;
            if (eo_mais_isAlive(eo_mais_GetHandle()))
            {
                float computedJntEncoderResolution = (float)(calibrator->params.type6.vmax - calibrator->params.type6.vmin) / (float) (jconfig->userlimits.max  - jconfig->userlimits.min);

#if 1
                embot::app::eth::theEncoderReader::getInstance().Scale({e, embot::app::eth::encoder::v1::Position::every}, {computedJntEncoderResolution, 0});
#else
                eOresult_t res = eo_appEncReader_UpdatedMaisConversionFactors(eo_appEncReader_GetHandle(), e, computedJntEncoderResolution);
                if(eores_OK != res)
                {    
                    ////debug code
                    char info[70];
                    snprintf(info, sizeof(info), "calib6: error updating Mais conversion factor j%d", e);
                    JointSet_send_debug_message(info, e, 0, 0);
                    ////debug code ended
                    return;
                }
#endif 
                AbsEncoder_config_resolution(o->absEncoder+e, computedJntEncoderResolution);
            
                //Now I need to re-init absEncoder because I chenged maisConversionFactor, therefore the values returned by EOappEncoreReder are changed.
                o->absEncoder[e].state.bits.not_initialized = TRUE;

                float computedJntEncoderZero =  - (float)(jconfig->userlimits.min) + ((float)(calibrator->params.type6.vmin) / computedJntEncoderResolution);
                o->joint[e].running_calibration.data.type6.computedZero = computedJntEncoderZero;
                o->joint[e].running_calibration.data.type6.targetpos = target_pos / computedJntEncoderResolution - computedJntEncoderZero; //convert target pos from mais unit to icub deegre

            }
            else if (eo_pos_isAlive(eo_pos_GetHandle()))
            {
                o->joint[e].running_calibration.data.type6.targetpos = target_pos;
                o->joint[e].running_calibration.data.type6.computedZero = 0;
            }
            else
            {
                ////debug code
                char info[70];
                snprintf(info, 70, "calib6: Error! Found neither MAIS not POS service active");
                JointSet_send_debug_message(info, e, 0, 0);
                ////debug code ended
                return;
            }
            
            o->joint[e].running_calibration.data.type6.velocity = calibrator->params.type6.velocity;
            o->joint[e].running_calibration.data.type6.state = calibtype6_st_jntEncResComputed;
        }
        break;

        case eomc_calibration_type7_hall_sensor:
        {
            //1) check params: nothinh to do
            
            // 2) set state
            o->calibration_timeout = 0;
            o->calibration_in_progress = eomc_calibration_typeMixed;
            o->joint[e].running_calibration.type = (eOmc_calibration_type_t)calibrator->type;
            o->joint[e].running_calibration.data.type7.state = calibtype7_st_inited;
            o->joint[e].running_calibration.data.type7.is_active = TRUE;
            
//            o->joint[e].calib_type7_data.is_active = TRUE;
//            o->joint[e].calib_type7_data.state = calibtype7_st_inited;
//            o->joint[e].calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
            
            
            // 2) calculate new joint encoder factor and param_zero
            eOmc_joint_config_t *jconfig = &o->joint[e].eo_joint_ptr->config;
            float computedJntEncoderResolution = (float)(calibrator->params.type7.vmax - calibrator->params.type7.vmin) / (float) (jconfig->userlimits.max  - jconfig->userlimits.min);
            
            int32_t offset = (((float)calibrator->params.type7.vmin)/computedJntEncoderResolution) - jconfig->userlimits.min;
            

#if 1
            embot::app::eth::theEncoderReader::getInstance().Scale({e, embot::app::eth::encoder::v1::Position::every}, {computedJntEncoderResolution, offset});
#else            
            eOresult_t res = eo_appEncReader_UpdatedHallAdcOffset(eo_appEncReader_GetHandle(), e, offset);
            if(eores_OK != res)
            {    
                ////debug code 
                char info[70];
                snprintf(info, 70, "calib7: error updating HallADC offset j%d", e);
                JointSet_send_debug_message(info, e, 0, 0);
                ////debug code ended
                return;
            }
            
            
            res = eo_appEncReader_UpdatedHallAdcConversionFactors(eo_appEncReader_GetHandle(), e, computedJntEncoderResolution);
            if(eores_OK != res)
            {    
                ////debug code 
                char info[70];
                snprintf(info, 70, "calib7: error updating HallADC conversion factor j%d", e);
                JointSet_send_debug_message(info, e, 0, 0);
                ////debug code ended
                return;
            }
#endif       
            AbsEncoder_config_resolution(o->absEncoder+e, computedJntEncoderResolution);
            
            //Now I need to re init absEncoder because I chenged hallADCConversionFactor, therefore the values returned by EOappEncoreReder are changed.
            o->absEncoder[e].state.bits.not_initialized = TRUE;
            
            float computedJntEncoderZero =  (((float)calibrator->params.type7.vmin) / computedJntEncoderResolution) - ((float)(jconfig->userlimits.min)) - offset;

            
            o->joint[e].running_calibration.data.type7.computedZero = computedJntEncoderZero;
            o->joint[e].running_calibration.data.type7.state = calibtype7_st_jntEncResComputed;
            
        }
        break;

#endif

        case eomc_calibration_type8_tripod_internal_hard_stop:
        case eomc_calibration_type9_tripod_external_hard_stop:
        {
            if (o->calibration_in_progress == calibrator->type) return;
            
            o->calibration_wait = 0;
            o->calibration_timeout = 0;
            
            o->calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
            
            o->joint[o->joints_of_set[0]].control_mode = eomc_controlmode_calib;
            o->joint[o->joints_of_set[1]].control_mode = eomc_controlmode_calib;
            o->joint[o->joints_of_set[2]].control_mode = eomc_controlmode_calib;
            
            AbsEncoder_calibrate_fake(o->absEncoder+o->encoders_of_set[0]);
            AbsEncoder_calibrate_fake(o->absEncoder+o->encoders_of_set[1]);
            AbsEncoder_calibrate_fake(o->absEncoder+o->encoders_of_set[2]);
            
            o->tripod_calib.pwm       = calibrator->params.type9.pwmlimit;
            o->tripod_calib.max_delta = calibrator->params.type9.max_delta;
            o->tripod_calib.zero      = calibrator->params.type9.calibrationZero;
            
            Motor_uncalibrate(o->motor+o->motors_of_set[0]);
            Motor_uncalibrate(o->motor+o->motors_of_set[1]);
            Motor_uncalibrate(o->motor+o->motors_of_set[2]);
            
            Motor_set_run(o->motor+o->motors_of_set[0], eomc_ctrl_out_type_pwm);
            Motor_set_run(o->motor+o->motors_of_set[1], eomc_ctrl_out_type_pwm);
            Motor_set_run(o->motor+o->motors_of_set[2], eomc_ctrl_out_type_pwm);
            
            o->tripod_calib.start_pos[0] = o->motor[o->motors_of_set[0]].pos_fbk;
            o->tripod_calib.start_pos[1] = o->motor[o->motors_of_set[1]].pos_fbk;
            o->tripod_calib.start_pos[2] = o->motor[o->motors_of_set[2]].pos_fbk;
            
            
        }
        break;
        
        case eomc_calibration_type10_abs_hard_stop:
        {
            o->calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
            
            o->calibration_timeout = 0;
            
            Joint_set_hardware_limit(o->joint+e);

            Motor_calibrate_withOffset(o->motor+e, 0);
            Motor_set_run(o->motor+e, eomc_ctrl_out_type_pwm);
            o->motor[e].calib_pwm = calibrator->params.type10.pwmlimit;
            
            if (AbsEncoder_is_fake(o->absEncoder+e))
            {
                AbsEncoder_calibrate_fake(o->absEncoder+e);
                o->hard_stop_calib.zero = calibrator->params.type10.calibrationZero;
                o->hard_stop_calib.pwm = calibrator->params.type10.pwmlimit;
                o->hard_stop_calib.space_thr = 12000; // we can make them configurable (probably not needed)
                o->hard_stop_calib.time_thr = 1000;   // we can make them configurable (probably not needed)
            }
            else
            {
                o->hard_stop_calib.zero = calibrator->params.type10.calibrationZero;
                o->hard_stop_calib.pwm = calibrator->params.type10.pwmlimit;
                o->hard_stop_calib.space_thr = 1000; // we can make them configurable (probably not needed)
                o->hard_stop_calib.time_thr = 200;   // we can make them configurable (probably not needed)
                AbsEncoder_still_check_reset(o->absEncoder+e);
                AbsEncoder_start_hard_stop_calibrate(o->absEncoder+e, calibrator->params.type10.calibrationZero);
            }
        }
        break;
        
        case eomc_calibration_type11_cer_hands:
        {   
            AbsEncoder* enc = o->absEncoder + 3*e;
            AbsEncoder_calibrate_absolute(enc  , calibrator->params.type11.offset0, enc[0].mul*32767);
            AbsEncoder_calibrate_absolute(enc+1, calibrator->params.type11.offset1, enc[1].mul*32767);
            AbsEncoder_calibrate_absolute(enc+2, calibrator->params.type11.offset2, enc[2].mul*32767);
            
            JointSet_do_odometry(o);
            
            Motor_calibrate_withOffset(o->motor+e, 0);
            Motor_set_run(o->motor+e, eomc_ctrl_out_type_pwm);
            Motor_uncalibrate(o->motor+e);
            
            o->joint[e].cable_constr.max_tension = SPRING_MAX_TENSION;
            
            o->joint[e].cable_calib.pwm         = calibrator->params.type11.pwm;
            o->joint[e].cable_calib.cable_range = calibrator->params.type11.cable_range;
            
            o->joint[e].cable_calib.delta       = 900;//calibrator->params.type11.delta;
            o->joint[e].cable_calib.target      = o->joint[e].pos_fbk + o->joint[e].cable_calib.delta;
            
            if (o->joint[e].cable_calib.target > o->joint[e].pos_max) o->joint[e].cable_calib.target = o->joint[e].pos_max;  
            if (o->joint[e].cable_calib.target < o->joint[e].pos_min) o->joint[e].cable_calib.target = o->joint[e].pos_min;
            
            o->calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
            
            o->calibration_wait = TRUE;
            
            o->calibration_timeout = 0;
            
            
        }
        break;
        
        case eomc_calibration_type12_absolute_sensor:
        {
            int32_t offset;
            int32_t zero;
            eOmc_joint_config_t *jointcfg = eo_entities_GetJointConfig(eo_entities_GetHandle(), e);
            //1) Take absolute value of calibation parametr
            int32_t abs_raw = (calibrator->params.type12.rawValueAtZeroPos > 0) ? calibrator->params.type12.rawValueAtZeroPos : -calibrator->params.type12.rawValueAtZeroPos;
            // 1.1) update abs_raw with gearbox_E2J
            abs_raw = abs_raw * jointcfg->gearbox_E2J;
            // 2) calculate offset
            if(abs_raw >= TICKS_PER_HALF_REVOLUTION)
                offset = abs_raw - TICKS_PER_HALF_REVOLUTION;
            else
                offset = abs_raw + TICKS_PER_HALF_REVOLUTION;
            
            // 3) find out sign of zero
            
            if(jointcfg->jntEncoderResolution > 0)
                zero = TICKS_PER_HALF_REVOLUTION / jointcfg->gearbox_E2J;
            else
                zero = -TICKS_PER_HALF_REVOLUTION / jointcfg->gearbox_E2J;
            
            zero+=calibrator->params.type12.calibrationDelta;  //this parameter should contain only the delta
            // 4) call calibration function
            
            ////debug code
            char info[80];
            snprintf(info, sizeof(info), "CALIB 12 j %d: offset=%d zero=%d ", e, offset, zero);
            JointSet_send_debug_message(info, e, 0, 0);
            ////debug code ended
            AbsEncoder_calibrate_absolute(o->absEncoder+e, offset, zero);
            
            Motor_calibrate_withOffset(o->motor+e, 0);
            Motor_set_run(o->motor+e, o->postrj_ctrl_out_type);
            o->calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;   
        }
        break;
        
        case eomc_calibration_type13_cer_hands_2:
        {
            AbsEncoder* enc = o->absEncoder + 4*e;
            
            //int32_t rawValueAtZeroPos[4];
            
            //rawValueAtZeroPos[0] = calibrator->params.type13.rawValueAtZeroPos0;
            //rawValueAtZeroPos[1] = calibrator->params.type13.rawValueAtZeroPos1;
            //rawValueAtZeroPos[2] = calibrator->params.type13.rawValueAtZeroPos2;
            //rawValueAtZeroPos[3] = calibrator->params.type13.rawValueAtZeroPos3;
            
            AbsEncoder_calibrate_absolute(enc  , calibrator->params.type13.rawValueAtZeroPos0, enc[0].mul*32767);
            AbsEncoder_calibrate_absolute(enc+1, calibrator->params.type13.rawValueAtZeroPos1, enc[1].mul*32767);
            AbsEncoder_calibrate_absolute(enc+2, calibrator->params.type13.rawValueAtZeroPos2, enc[2].mul*32767);
            AbsEncoder_calibrate_absolute(enc+3, calibrator->params.type13.rawValueAtZeroPos3, enc[3].mul*32767);
            
            /*
            for (int k=0; k<4; ++k)
            {
                int32_t offset;
                int32_t zero;
                
                //eOmc_joint_config_t *jointcfg = eo_entities_GetJointConfig(eo_entities_GetHandle(), e);
                //1) Take absolute value of calibation parametr
                int32_t abs_raw = (rawValueAtZeroPos[k] > 0) ? rawValueAtZeroPos[k] : -rawValueAtZeroPos[k];
                // 1.1) update abs_raw with gearbox_E2J
                //abs_raw = abs_raw * jointcfg->gearbox_E2J;
                // 2) calculate offset
                if(abs_raw >= TICKS_PER_HALF_REVOLUTION)
                    offset = abs_raw - TICKS_PER_HALF_REVOLUTION;
                else
                    offset = abs_raw + TICKS_PER_HALF_REVOLUTION;
            
                // 3) find out sign of zero
            
                //if(jointcfg->jntEncoderResolution > 0)
                    zero = TICKS_PER_HALF_REVOLUTION;// / jointcfg->gearbox_E2J;
                //else
                //    zero = -TICKS_PER_HALF_REVOLUTION;// / jointcfg->gearbox_E2J;
            
                //zero+=calibrator->params.type12.calibrationDelta;  //this parameter should contain only the delta
                // 4) call calibration function
            
                ////debug code
                char info[80];
                snprintf(info, sizeof(info), "CALIB 13 j(%d)[%d]: offset=%d zero=%d ", e, k, offset, zero);
                JointSet_send_debug_message(info, e);
                
                ////debug code ended
                AbsEncoder_calibrate_absolute(o->absEncoder+4*e+k, offset, zero);
            }
            */
            
            JointSet_do_odometry(o);
            
            Motor_calibrate_withOffset(o->motor+e, 0);
            o->calibration_in_progress = (eOmc_calibration_type_t)calibrator->type;
        }
        break;
       

#if defined(USE_EMBOT_theServices) 
//#warning removed some code
#else        
        case eomc_calibration_type14_qenc_hard_stop_and_fap:
        {
            // instantiate here info buffer used in debug message. clean it for each message instead of reinstantiate it
            char info[70] = {};
            
            // (1) check current params is ok and set target and hardstop positions
            o->joint[e].running_calibration.data.type14.hardstopPos = calibrator->params.type14.final_pos;
            o->joint[e].running_calibration.data.type14.computedZero = calibrator->params.type14.calibrationZero;
            
            // If here calibration type 14 can start it process            
            // (2) set states for joint and motor
            o->calibration_timeout = 0;
            o->calibration_in_progress = eomc_calibration_type14_qenc_hard_stop_and_fap;
            o->joint[e].running_calibration.type = (eOmc_calibration_type_t)calibrator->type;
            o->joint[e].running_calibration.data.type14.is_active = TRUE;
            o->joint[e].running_calibration.data.type14.state = calibtype14_st_inited;
            
            //debug code
            memset(&info[0], 0, sizeof(info));
                snprintf(info, sizeof(info), "Going to hardstopPos:%.2f w/ PWM:%d", o->joint[e].running_calibration.data.type14.hardstopPos, calibrator->params.type14.pwmlimit);
            JointSet_send_debug_message(info, e, 0, 0);
            //debug code ended
            
            // (3) set motor params for moving to hard stop
            // as above, this check of the pos service is not mandatory
            // considering that at this point the POS service is already up (it's a lower runlevel)
            // anyway, let's keep it as a preventive check
            if (eo_pos_isAlive(eo_pos_GetHandle()))
            {   
                //Debug code
                memset(&info[0], 0, sizeof(info));
                snprintf(info, sizeof(info), "Move hrd_stp motor=%d with pwm=%d and cz=%d", e, calibrator->params.type14.pwmlimit, calibrator->params.type14.calibrationZero);
                JointSet_send_debug_message(info, e, 0, 0);
                //debug code ended
                
                //1) Fix the offset with the rotation raw
                int32_t offset_raw = (calibrator->params.type14.offset + calibrator->params.type14.rotation ) % ((int32_t)DEG2ICUB*360);
                
                //3) Convert offset to decideg for POS service and add "-" sign to offset_raw to correct raw position (always minus sign independently from invertDirection value)
                int32_t offset = ((ICUB2DEG)*(CTRL_UNITS)((-1)*offset_raw))*10.0f;
                
                //4) Add the rotation
                eOmc_calib14_ROT_t rotation = eomc_int2calib14_ROT(calibrator->params.type14.rotation);
                
                if(rotation == eOmc_calib14_ROT_unknown || rotation == eOmc_calib14_ROT_none)
                {
                    //Debug code
                    memset(&info[0], 0, sizeof(info));
                    snprintf(info, sizeof(info), "Error in converting rotation to enum ret: %u", rotation);
                    JointSet_send_debug_message(info, e, 0, 0);
                    //debug code ended
                    return;
                }
                eoas_pos_ROT_t posrotation = JointSet_calib14_ROT2pos_ROT(rotation);
                
                if (eo_pos_Calibrate(eo_pos_GetHandle(), e, posrotation, calibrator->params.type14.invertdirection, offset) != eores_OK)
                {
                    //Debug code
                    memset(&info[0], 0, sizeof(info));
                    snprintf(info, sizeof(info), "Error in recalibrate POS encoder");
                    JointSet_send_debug_message(info, e, 0, 0);
                    //debug code ended
                    
                    o->joint[e].control_mode = joint_controlMode_old;
                    o->control_mode = jointSet_controlMode_old;
                    return;
                }
                
                // this is just a setter
                BOOL ret = Motor_calibrate_moving2Hardstop(o->motor+e, calibrator->params.type14.pwmlimit, o->joint[e].running_calibration.data.type14.hardstopPos);
                
                if(!ret)
                {
                    //Debug code
                    memset(&info[0], 0, sizeof(info));
                    snprintf(info, sizeof(info), "Error in set hs: ret=%d", ret);
                    JointSet_send_debug_message(info, e, 0, 0);
                    //debug code ended
                    o->joint[e].control_mode = joint_controlMode_old;
                    o->control_mode = jointSet_controlMode_old;
                    return;
                }
                
                // (4) set hoint hardware limits
                Joint_set_hardware_limit(o->joint+e);
                o->joint[e].running_calibration.data.type14.state = calibtype14_st_absEncoderCalib;
            }
            else
            {
                //debug code
                char info[70];
                snprintf(info, sizeof(info), "Error! POS service NOT active");
                JointSet_send_debug_message(info, e, 0, 0);
                //debug code ended
                return;
            }
        }
        break;
        
#endif
        
        default:
            break;
    }
}

extern void Calibrator_do_wait_calibration(JointSet* o)
{
    int N = *(o->pN);
    int E = *(o->pE);
    
    if (o->calibration_in_progress != eomc_calibration_typeUndefined)
    {
        if (o->calibration_timeout < CALIBRATION_TIMEOUT)
        {
            ++o->calibration_timeout;
        }
        else
        {
            //first of all I need to restore rotor limits if i was doing mais calib
            if(eomc_calibration_typeMixed == o->calibration_in_progress)
            {
                for (int k=0;  k<*(o->pN); ++k)
                {
                    int m = o->motors_of_set[k];
                    int j = o->joints_of_set[k];
                    Joint* j_ptr = o->joint+j;
                    if(eomc_calibration_type6_mais == j_ptr->running_calibration.type)
                    {                
                        //restore rotor limits
                        o->motor[m].pos_min = j_ptr->running_calibration.data.type6.rotorposmin;
                        o->motor[m].pos_max = j_ptr->running_calibration.data.type6.rotorposmax;
                    }
                }
            }    
            o->calibration_in_progress = eomc_calibration_typeUndefined;
        
            o->control_mode = eomc_controlmode_notConfigured;
            
            o->is_calibrated = FALSE;
            
            for (int k=0; k<N; ++k)
            {
                o->joint[o->joints_of_set[k]].control_mode = eomc_controlmode_notConfigured;
            
                Motor_set_pwm_ref(o->motor+o->motors_of_set[k], 0);
                Motor_set_idle(o->motor+o->motors_of_set[k]);
                Motor_uncalibrate(o->motor+o->motors_of_set[k]);
            }    
            
            return;
        }
    }

    o->is_calibrated = TRUE;
    
    switch (o->calibration_in_progress)
    {
        case eomc_calibration_type3_abs_sens_digital:
            o->is_calibrated = JointSet_do_wait_calibration_3(o);
            break;
        
        case eomc_calibration_type5_hard_stops:
            o->is_calibrated = JointSet_do_wait_calibration_5(o);
            break;
            
//        case eomc_calibration_type6_mais: ==> managed in mixed
//            o->is_calibrated = JointSet_do_wait_calibration_6(o);
//            break;
//        
//        case eomc_calibration_type7_hall_sensor: ==> managed in mixed
//            o->is_calibrated = JointSet_do_wait_calibration_7(o);
//            break;

        case eomc_calibration_type8_tripod_internal_hard_stop:
            o->is_calibrated = JointSet_do_wait_calibration_8(o);
            break;
        
        case eomc_calibration_type9_tripod_external_hard_stop:
            o->is_calibrated = JointSet_do_wait_calibration_9(o);
            break;

        case eomc_calibration_type10_abs_hard_stop:
            o->is_calibrated = JointSet_do_wait_calibration_10(o);
            break;
            
        case eomc_calibration_type11_cer_hands:
            o->is_calibrated = JointSet_do_wait_calibration_11(o);
            break;
        
        case eomc_calibration_type12_absolute_sensor:
            o->is_calibrated = JointSet_do_wait_calibration_12(o);
            break;
            
        case eomc_calibration_type13_cer_hands_2:
            o->is_calibrated = JointSet_do_wait_calibration_13(o);
            break;
            
        case eomc_calibration_type14_qenc_hard_stop_and_fap:
            o->is_calibrated = JointSet_do_wait_calibration_14(o);
            break;
        case eomc_calibration_typeMixed:
            o->is_calibrated = JointSet_do_wait_calibration_mixed(o);
            break;
        
        default:
            o->is_calibrated = FALSE;
            break;
    }
    
    if (!o->is_calibrated) 
    {
        return;
    }
    
    for (int es=0; es<E; ++es)
    {
        if (!AbsEncoder_is_calibrated(o->absEncoder+o->encoders_of_set[es])) 
        {
            o->is_calibrated = FALSE;
            return;
        }
    }
    
    o->calibration_in_progress = eomc_calibration_typeUndefined;
    
    JointSet_do_odometry(o);
    
    o->control_mode = eomc_controlmode_idle;

    for (int js=0; js<N; ++js)
    {
        o->joint[o->joints_of_set[js]].control_mode = eomc_controlmode_idle;
    }
    
    JointSet_set_control_mode(o, eomc_controlmode_cmd_position);
    
}

static BOOL JointSet_do_wait_calibration_3(JointSet* o)
{
    BOOL calibrated = TRUE;
    
    for (int ms=0; ms<*(o->pN); ++ms)
    {
        if (!Motor_is_calibrated(o->motor+o->motors_of_set[ms])) 
        {
            calibrated = FALSE;
        }
    }
    
    return calibrated;
}

static BOOL JointSet_do_wait_calibration_5(JointSet* o)
{
    BOOL calibrated = TRUE;
    
    for (int ms=0; ms<*(o->pN); ++ms)
    {
        if(Calibration_check(o->motor+o->motors_of_set[ms])) 
        {
            calibrated = FALSE;
            
            Motor_do_calibration_hard_stop(o->motor+o->motors_of_set[ms]);
        }
    }
    
    return calibrated;
}

static BOOL JointSet_calibType6_check_reached_pos(Joint *j)
{
//    char info[70];

      CTRL_UNITS delta = j->running_calibration.data.type6.targetpos - j->pos_fbk;
    
//    ///// debug code
//    int32_t t_ref_pos = Trajectory_get_pos_ref(&j->trajectory);
//    int32_t t_ref_vel = Trajectory_get_vel_ref(&j->trajectory);
//    snprintf(info, 70, "e=%.1f rp=%d rv=%d d=%.1f", j->pos_err, t_ref_pos, t_ref_vel, delta);
//    send_debug_message(info, j->ID);
//    ///// debug code end
    
    if( (delta>CALIB_TYPE_6_POS_TRHESHOLD ) || (delta<-CALIB_TYPE_6_POS_TRHESHOLD ) )
        return FALSE;
    
    return TRUE;
}

static eOresult_t JointSet_do_wait_calibration_6_singleJoint(JointSet *o, int indexSet, BOOL* calibrationCompleted)
{
    
    /* When i'm here i sure that:
       - state = calibtype6_st_jntEncResComputed
     */
    char info[80];
    *calibrationCompleted = FALSE;   
    //get poiter to the joint to calibrate
    Joint* j_ptr = o->joint + o->joints_of_set[indexSet];
    Motor* m_ptr = o->motor + o->motors_of_set[indexSet];
    
    //get the encoder of joint to calibrate
    AbsEncoder* e_ptr = o->absEncoder+ o->encoders_of_set[indexSet];

    
    jointCalibType6Data *jCalib6Data_ptr = &(j_ptr->running_calibration.data.type6);

    
    switch(jCalib6Data_ptr->state)
    {
        case calibtype6_st_jntEncResComputed:
        {
            
            AbsEncoder_calibrate_absolute(e_ptr, 0, jCalib6Data_ptr->computedZero);
            
            jCalib6Data_ptr->state = calibtype6_st_absEncoderCalibrated;
            
        }
        break;
        
        case calibtype6_st_absEncoderCalibrated:
        {
            //if the current position (computed with calib param of abs encoder) is out of limits range, I'll put joint in fault
            //the limit range is very big. this check should save us from wrong calib params.
            int32_t curr_pos = AbsEncoder_position(e_ptr);

            if((curr_pos > j_ptr->pos_max+CALIB_TYPE_6_7_POS_ERROR_TRHESHOLD) || (curr_pos < j_ptr->pos_min-CALIB_TYPE_6_7_POS_ERROR_TRHESHOLD))
            {
                
                jCalib6Data_ptr->is_active = FALSE;
                memset(&info[0], 0, sizeof(info));
                snprintf(info, sizeof(info), "calib 6:outLim: cp:%d mx:%.1f mn:%.1f", curr_pos, j_ptr->pos_max, j_ptr->pos_min);
                JointSet_send_debug_message(info, j_ptr->ID, 0, 0);

                return(eores_NOK_generic);
            }
            for (int k=0; k<*(o->pN); ++k)
            {
                int m = o->motors_of_set[k];
                int j = o->joints_of_set[k];
                Motor_motion_reset(o->motor+ m);  // reset PID
                Joint_motion_reset(o->joint+ j);  // reset PID and errors
                //Motor_set_run(o->motor+ m, PWM_INPUT_MOTOR); 
                Motor_set_run(o->motor+ m, eomc_ctrl_out_type_pwm); //clear ext fault bit if it is not pressed
            }
        
            if(j_ptr->control_mode == eomc_controlmode_hwFault)
            {
                return(eores_NOK_generic);
            }
            //Vale: shall I to save limits for all moors in the set? maybe not...
            j_ptr->running_calibration.data.type6.rotorposmin = m_ptr->pos_min;
            j_ptr->running_calibration.data.type6.rotorposmax = m_ptr->pos_max;
            m_ptr->pos_min = 0;
            m_ptr->pos_max = 0;
            
            BOOL ret = Joint_set_pos_ref_in_calibType6(j_ptr, jCalib6Data_ptr->targetpos, jCalib6Data_ptr->velocity);
            if(!ret)
            {
                memset(&info[0], 0, sizeof(info));
                snprintf(info, sizeof(info),"error in Joint_set_pos_ref_in_calibType6");
                JointSet_send_debug_message(info, j_ptr->ID, 0, 0);
                //restore rotor limits
                m_ptr->pos_min = j_ptr->running_calibration.data.type6.rotorposmin;
                m_ptr->pos_max = j_ptr->running_calibration.data.type6.rotorposmax;
    
                return(eores_NOK_generic);
            }

            jCalib6Data_ptr->state = calibtype6_st_trajectoryStarted;
        }    
        break;
        
        case calibtype6_st_trajectoryStarted:
        {
            JointSet_do_pwm_control(o);
            
            
            if(JointSet_calibType6_check_reached_pos(j_ptr))
            {
                jCalib6Data_ptr->state = calibtype6_st_finished;
                
                Motor* m_ptr = o->motor + o->motors_of_set[indexSet];
                m_ptr->pos_calib_offset = m_ptr->pos_fbk;
                //reset value of position
                m_ptr->pos_fbk = m_ptr->pos_fbk - m_ptr->pos_calib_offset;
                m_ptr->pos_fbk_old = 0;
                m_ptr->not_init = TRUE;
                
                //Debug code
                memset(&info[0], 0, sizeof(info));
                snprintf(info, sizeof(info), "c6M:pos reached: cp=%d co=%d", m_ptr->pos_fbk, m_ptr->pos_calib_offset);
                JointSet_send_debug_message(info, m_ptr->ID, 0, 0);
                //ended
                
                m_ptr->not_calibrated = FALSE;
                
                for (int k=0; k<*(o->pN); ++k)
                {
                    int m = o->motors_of_set[k];
                    int j = o->joints_of_set[k];
                    Motor_motion_reset(o->motor+ m);
                    Joint_motion_reset(o->joint+ j);
                    Motor_set_idle(o->motor+ m);
                }
                //restore rotor limits
                m_ptr->pos_min = j_ptr->running_calibration.data.type6.rotorposmin;
                m_ptr->pos_max = j_ptr->running_calibration.data.type6.rotorposmax;
                
                //Debug code 
                memset(&info[0], 0, sizeof(info));
                snprintf(info, sizeof(info), "Mtr limits: m_min=%d m_max=%d", m_ptr->pos_min, m_ptr->pos_max );
                JointSet_send_debug_message(info, j_ptr->ID, 0, 0);
                //ended
            }
                
        }
        break;
        case calibtype6_st_finished:
        {
            //set status to complete calibration
            jCalib6Data_ptr->is_active = FALSE;
            *calibrationCompleted = TRUE;
            
            //Debug code 
            char info[80];
            snprintf(info, sizeof(info), "Calib Completed" );
            JointSet_send_debug_message(info, j_ptr->ID, 0, 0);
            //ended
            
        }break;
        
        default:
            return(eores_NOK_generic);

    }
    return(eores_OK);
}


static eOresult_t JointSet_do_wait_calibration_7_singleJoint(Joint *j, Motor* m, AbsEncoder* e, BOOL* calibrationCompleted)
{
    
    eOresult_t ret = eores_OK;
    
    /* When i'm here i sure that:
       - state = calibtype7_st_jntEncResComputed
    */
    
    *calibrationCompleted = FALSE;
    
    jointCalibType7Data* jCalib7data_ptr = &(j->running_calibration.data.type7);

    switch(jCalib7data_ptr->state)
    {
        case calibtype7_st_jntEncResComputed:
        {
            AbsEncoder_calibrate_absolute(e, 0, j->running_calibration.data.type7.computedZero);
            jCalib7data_ptr->state = calibtype7_st_jntCheckLimits;
        }    
        break;
        
        case calibtype7_st_jntCheckLimits:
        {
            int32_t curr_pos = AbsEncoder_position(e);
            if((curr_pos > j->pos_max+CALIB_TYPE_6_7_POS_ERROR_TRHESHOLD) || (curr_pos < j->pos_min-CALIB_TYPE_6_7_POS_ERROR_TRHESHOLD))
            {
                //// debug code
                char info[80];
                snprintf(info, sizeof(info), "calib7:outLim: cp%d mx%.1f mn%.1f",curr_pos, j->pos_max, j->pos_min);
                JointSet_send_debug_message(info, j->ID, 0, 0);
                ////debug code ended

                return(eores_NOK_generic);
            
            }
            else
            {
                *calibrationCompleted = TRUE;
                jCalib7data_ptr->state = calibtype7_st_finished;
            }
         }    
        break;
        case calibtype7_st_finished:
        {
            *calibrationCompleted = TRUE;
        }break;
        
        default:
        {
            ret = eores_NOK_generic;
        } break;
    }
    
    return(ret);
}

static BOOL JointSet_do_wait_calibration_mixed(JointSet* o)
{
    JointSet_set_interaction_mode(o, eOmc_interactionmode_stiff);
    
    BOOL calibrationCompleted = TRUE;
    eOresult_t res = eores_OK;
    volatile uint8_t num_of_j = *(o->pN);
    
    for (int k=0; ( (k<*(o->pN)) && (eores_OK == res) ); ++k)
    {
        int m = o->motors_of_set[k];
        int e = o->encoders_of_set[k];
        int j = o->joints_of_set[k];
        Joint* j_ptr = o->joint+j;
        BOOL calibrated_single_joint = FALSE;
        
       switch (j_ptr->running_calibration.type)
       {
        case eomc_calibration_type6_mais:
            res = JointSet_do_wait_calibration_6_singleJoint(o, k, &calibrated_single_joint);
            break;
        
        case eomc_calibration_type7_hall_sensor:
            res = JointSet_do_wait_calibration_7_singleJoint(j_ptr, o->motor+m, o->absEncoder+e, &calibrated_single_joint);
            break;

        case eomc_calibration_typeUndefined:
            res = eores_OK; //if I'm here means joint j didn't receive calib command or is already calibrated. Both cases are ok! 
            
            break;
        default://If i'm here means joint has a calib type that not support calibration for single joint.
            res = eores_NOK_generic;
            break;
        }
       
       calibrationCompleted = calibrationCompleted & calibrated_single_joint;

    }//end for
    
    
    if(res != eores_OK)
    {
        o->calibration_in_progress = eomc_calibration_typeUndefined;
        o->control_mode = eomc_controlmode_hwFault;
        
        for (int k=0; k<*(o->pN); ++k)
        {
            Joint* j_ptr = o->joint + o->joints_of_set[k];
            j_ptr->control_mode = eomc_controlmode_hwFault;
            //j_ptr->calibration_in_progress = eomc_calibration_typeUndefined;
            Joint_reset_calibration_data(j_ptr);
            Motor_set_idle(o->motor+o->motors_of_set[k]);
        }
        
    }
    if(calibrationCompleted)
    {
        for (int k=0; k<*(o->pN); ++k)
        {
            Joint* j_ptr = o->joint + o->joints_of_set[k];
            Joint_reset_calibration_data(j_ptr);
        }
        //Debug code
        char message[180];
        snprintf(message, sizeof(message), "JointSet_do_wait_calibration_mixed: calib completed.");
        JointSet_send_debug_message(message, 0, 0, 0);
    
    }
    return(calibrationCompleted);

}


static BOOL JointSet_do_wait_calibration_8(JointSet* o)
{    
    BOOL calibrated = TRUE;
    
    int32_t pos[3];
    
    for (int ms=0; ms<*(o->pN); ++ms)
    {
        int m = o->motors_of_set[ms];

        if (hal_adc_get_hall_sensor_analog_input_mV(o->motor[m].motorlocation.adr) < 1500)
        {
            o->motor[m].not_calibrated = FALSE;
        }
        
        if (Motor_is_calibrated(o->motor+m))
        {
            Motor_set_pwm_ref(o->motor+m, 0);
            
            pos[ms] = 0x7FFFFFFF;
        }
        else
        {
            pos[ms] = o->motor[m].pos_fbk - o->tripod_calib.start_pos[ms];
            
            //Motor_set_pwm_ref(o->motor+m, o->tripod_calib.pwm);
            
            calibrated = FALSE;
        }
    }
    
    if (calibrated)
    {        
        o->motor[o->motors_of_set[0]].pos_calib_offset += o->motor[o->motors_of_set[0]].pos_fbk - o->tripod_calib.zero;
        o->motor[o->motors_of_set[1]].pos_calib_offset += o->motor[o->motors_of_set[1]].pos_fbk - o->tripod_calib.zero;
        o->motor[o->motors_of_set[2]].pos_calib_offset += o->motor[o->motors_of_set[2]].pos_fbk - o->tripod_calib.zero;
        
        o->motor[o->motors_of_set[0]].pos_fbk = o->tripod_calib.zero;
        o->motor[o->motors_of_set[1]].pos_fbk = o->tripod_calib.zero;
        o->motor[o->motors_of_set[2]].pos_fbk = o->tripod_calib.zero;

        o->motor[o->motors_of_set[0]].not_init = FALSE;
        o->motor[o->motors_of_set[1]].not_init = FALSE;
        o->motor[o->motors_of_set[2]].not_init = FALSE;
        
        o->motor[o->motors_of_set[0]].pos_fbk_old = o->motor[o->motors_of_set[0]].pos_fbk;
        o->motor[o->motors_of_set[1]].pos_fbk_old = o->motor[o->motors_of_set[1]].pos_fbk;
        o->motor[o->motors_of_set[2]].pos_fbk_old = o->motor[o->motors_of_set[2]].pos_fbk;


        
        return TRUE;
    }
    
    float pwm[3];
    
    if (pos[0]<0) pos[0] = -pos[0];
    if (pos[1]<0) pos[1] = -pos[1];
    if (pos[2]<0) pos[2] = -pos[2];
    
    int32_t posL = pos[0];
    if (pos[1] < posL) posL = pos[1];
    if (pos[2] < posL) posL = pos[2];
    
    float pwm_calib = (float)o->tripod_calib.pwm;
    
    if (pwm_calib < 0.0f) pwm_calib = -pwm_calib;
    
    pwm[0] = pwm_calib * (1.0f - (float)(pos[0] - posL)/(float)o->tripod_calib.max_delta);
    pwm[1] = pwm_calib * (1.0f - (float)(pos[1] - posL)/(float)o->tripod_calib.max_delta);
    pwm[2] = pwm_calib * (1.0f - (float)(pos[2] - posL)/(float)o->tripod_calib.max_delta);
    
    if (pwm[0] < 0.0f) pwm[0] = 0.0f;
    if (pwm[1] < 0.0f) pwm[1] = 0.0f;
    if (pwm[2] < 0.0f) pwm[2] = 0.0f;
    
    float pwm_min = pwm[0];
    if (pwm[1] < pwm_min) pwm_min = pwm[1];
    if (pwm[2] < pwm_min) pwm_min = pwm[2];

    if (o->tripod_calib.pwm < 0)
    {
        pwm[0] = -pwm[0];
        pwm[1] = -pwm[1];
        pwm[2] = -pwm[2];
    }
    
    Motor_set_pwm_ref(o->motor+o->motors_of_set[0], (int32_t)pwm[0]);
    Motor_set_pwm_ref(o->motor+o->motors_of_set[1], (int32_t)pwm[1]);
    Motor_set_pwm_ref(o->motor+o->motors_of_set[2], (int32_t)pwm[2]);

    return FALSE;
}

static BOOL JointSet_do_wait_calibration_9(JointSet* o)
{
    if (o->calibration_wait<250)
    {
        Motor_set_pwm_ref(o->motor+o->motors_of_set[0], o->tripod_calib.pwm);
        Motor_set_pwm_ref(o->motor+o->motors_of_set[1], o->tripod_calib.pwm);
        Motor_set_pwm_ref(o->motor+o->motors_of_set[2], o->tripod_calib.pwm);
        
        ++o->calibration_wait;
        
        return FALSE;
    }
    
    BOOL calibrated = TRUE;
    
    int32_t pos[3];
    
    BOOL mcalib[3] = {FALSE, FALSE, FALSE };
    
    for (int ms=0; ms<*(o->pN); ++ms)
    {
        int m = o->motors_of_set[ms];
        
        mcalib[ms] = Motor_is_calibrated(o->motor+m);
        
        if (mcalib[ms])
        {
            pos[ms] = 0x7FFFFFFF;
        }
        else
        {
            pos[ms] = o->motor[m].pos_fbk - o->tripod_calib.start_pos[ms];
            
            calibrated = FALSE;
        }
    }
    
    if (calibrated)
    {        
        o->motor[o->motors_of_set[0]].pos_calib_offset += o->motor[o->motors_of_set[0]].pos_fbk - o->tripod_calib.zero;
        o->motor[o->motors_of_set[1]].pos_calib_offset += o->motor[o->motors_of_set[1]].pos_fbk - o->tripod_calib.zero;
        o->motor[o->motors_of_set[2]].pos_calib_offset += o->motor[o->motors_of_set[2]].pos_fbk - o->tripod_calib.zero;
        
        o->motor[o->motors_of_set[0]].pos_fbk = o->tripod_calib.zero;
        o->motor[o->motors_of_set[1]].pos_fbk = o->tripod_calib.zero;
        o->motor[o->motors_of_set[2]].pos_fbk = o->tripod_calib.zero;
        
        o->motor[o->motors_of_set[0]].not_init = FALSE;
        o->motor[o->motors_of_set[1]].not_init = FALSE;
        o->motor[o->motors_of_set[2]].not_init = FALSE;

        o->motor[o->motors_of_set[0]].pos_fbk_old = o->motor[o->motors_of_set[0]].pos_fbk;
        o->motor[o->motors_of_set[1]].pos_fbk_old = o->motor[o->motors_of_set[1]].pos_fbk;
        o->motor[o->motors_of_set[2]].pos_fbk_old = o->motor[o->motors_of_set[2]].pos_fbk;
        
        Motor_set_pwm_ref(o->motor+o->motors_of_set[0], 0);
        Motor_set_pwm_ref(o->motor+o->motors_of_set[1], 0);
        Motor_set_pwm_ref(o->motor+o->motors_of_set[2], 0);
        
        return TRUE;
    }
    
    float pwm[3];
    
    if (pos[0]<0) pos[0] = -pos[0];
    if (pos[1]<0) pos[1] = -pos[1];
    if (pos[2]<0) pos[2] = -pos[2];
    
    int32_t posL = pos[0];
    if (pos[1] < posL) posL = pos[1];
    if (pos[2] < posL) posL = pos[2];
    
    float pwm_calib = (float)o->tripod_calib.pwm;
    
    if (pwm_calib < 0.0f) pwm_calib = -pwm_calib;
    
    pwm[0] = pwm_calib * (1.0f - (float)(pos[0] - posL)/(float)o->tripod_calib.max_delta);
    pwm[1] = pwm_calib * (1.0f - (float)(pos[1] - posL)/(float)o->tripod_calib.max_delta);
    pwm[2] = pwm_calib * (1.0f - (float)(pos[2] - posL)/(float)o->tripod_calib.max_delta);
    
    if (pwm[0] < 0.0f) pwm[0] = 0.0f;
    if (pwm[1] < 0.0f) pwm[1] = 0.0f;
    if (pwm[2] < 0.0f) pwm[2] = 0.0f;
    
    float pwm_min = pwm[0];
    if (pwm[1] < pwm_min) pwm_min = pwm[1];
    if (pwm[2] < pwm_min) pwm_min = pwm[2];

    if (pwm_min < 3500.0f)
    {
        pwm[0] += 3500.0f - pwm_min;
        pwm[1] += 3500.0f - pwm_min;
        pwm[2] += 3500.0f - pwm_min;
    }

    if (o->tripod_calib.pwm < 0)
    {
        pwm[0] = -pwm[0];
        pwm[1] = -pwm[1];
        pwm[2] = -pwm[2];
    }
    
    Motor_set_pwm_ref(o->motor+o->motors_of_set[0], mcalib[0] ? 0 : (int32_t)pwm[0]);
    Motor_set_pwm_ref(o->motor+o->motors_of_set[1], mcalib[1] ? 0 : (int32_t)pwm[1]);
    Motor_set_pwm_ref(o->motor+o->motors_of_set[2], mcalib[2] ? 0 : (int32_t)pwm[2]);

    return FALSE;
}

static BOOL JointSet_do_wait_calibration_10(JointSet* o)
{
    BOOL calibrated = TRUE;
    
    for (int ms=0; ms<*(o->pN); ++ms)
    {
        if (!Motor_is_calibrated(o->motor+o->motors_of_set[ms])) 
        {
            calibrated = FALSE;
        }
    }
    
    if (!calibrated) return FALSE;
    
    for (int k=0; k<*(o->pN); ++k)
    {
        int m = o->motors_of_set[k];
        
        AbsEncoder *encoder = o->absEncoder+o->encoders_of_set[k];
        
        if (AbsEncoder_is_fake(encoder))
        {
            Motor_set_pwm_ref(o->motor+m, o->motor[m].calib_pwm);
            
            if (Motor_is_still(o->motor+m, o->hard_stop_calib.space_thr, o->hard_stop_calib.time_thr))
            {
                o->motor[m].pos_calib_offset += o->motor[m].pos_fbk - o->hard_stop_calib.zero;        
                o->motor[m].pos_fbk = o->hard_stop_calib.zero;
                o->motor[m].not_init = FALSE;
                o->motor[m].pos_fbk_old = o->motor[m].pos_fbk;

                char message[128];
                snprintf(message, sizeof(message), "CALIBRATED!!!");
                JointSet_send_debug_message(message, 0, 0, 0);
                
                Motor_set_pwm_ref(o->motor+m, 0);
            }
            else
            {
                //char message[128];
                //snprintf(message, sizeof(message), "fake ENCODER=%d", o->motor[m].partial_space);
                //JointSet_send_debug_message(message, 0, 0, 0);
                
                calibrated = FALSE;
            }
        }
        else
        {
            if (AbsEncoder_is_hard_stop_calibrating(encoder) && (!encoder->state.bits.not_initialized))
            {
                Motor_set_pwm_ref(o->motor+m, o->motor[m].calib_pwm);
        
                if (AbsEncoder_is_still(encoder, o->hard_stop_calib.space_thr, o->hard_stop_calib.time_thr))
                {
                    AbsEncoder_calibrate_in_hard_stop(encoder);
                }
                else
                {                
                    calibrated = FALSE;
                }
            }
            else
            {
                Motor_set_pwm_ref(o->motor+m, 0);
            }
        }
    }
    
    return calibrated;
}

static BOOL JointSet_do_wait_calibration_11(JointSet* o)
{
    BOOL calibrated = TRUE;
    
    for (int k=0; k<*(o->pN); ++k)
    {
        Joint* pj = o->joint+o->joints_of_set[k];
        Motor* pm = o->motor+o->motors_of_set[k];
        
        if (pm->not_calibrated)
        {
            if (o->calibration_wait)
            {
                if ((pj->trq_fbk > pj->cable_constr.max_tension/16) || (pj->pos_fbk_from_motors > -2*pj->cable_calib.delta))
                {
                    Motor_set_pwm_ref(pm, -pj->cable_calib.pwm);
                }
                else
                {
                    Motor_set_pwm_ref(pm, 0);
                    pj->cable_calib.target = pj->pos_fbk + pj->cable_calib.delta;
                    o->calibration_wait = FALSE;
                }
                
                calibrated = FALSE;
            }
            else if ((pj->trq_fbk > pj->cable_constr.max_tension/4) || (pj->pos_fbk >= pj->cable_calib.target))
            {
                Motor_set_pwm_ref(pm, 0);
            
                pm->not_calibrated = FALSE;
                
                float L =  (float)(pj->pos_fbk - pj->pos_min) / (float)(pj->pos_max - pj->pos_min);
                
                pj->cable_constr.motor_pos_min = pj->pos_fbk_from_motors -       L *pj->cable_calib.cable_range;
                pj->cable_constr.motor_pos_max = pj->pos_fbk_from_motors + (1.0f-L)*pj->cable_calib.cable_range;
            }
            else
            {
                Motor_set_pwm_ref(pm, pj->cable_calib.pwm);
            
                calibrated = FALSE;
            }
        }
        else
        {
            Motor_set_pwm_ref(pm, 0);
        }
    }
    
    return calibrated;
}

static BOOL JointSet_do_wait_calibration_12(JointSet* o)
{
    BOOL calibrated = TRUE;
    
    for (int ms=0; ms<*(o->pN); ++ms)
    {
        if (!Motor_is_calibrated(o->motor+o->motors_of_set[ms])) 
        {
            calibrated = FALSE;
        }
    }
    
    return calibrated;
}

static BOOL JointSet_do_wait_calibration_13(JointSet* o)
{
    BOOL calibrated = TRUE;
    
    for (int ms=0; ms<*(o->pN); ++ms)
    {
        if (!Motor_is_calibrated(o->motor+o->motors_of_set[ms])) 
        {
            calibrated = FALSE;
        }
    }
    
    return calibrated;
}

static BOOL JointSet_do_wait_calibration_14(JointSet* o)
{
    BOOL calibrated = FALSE;
    JointSet_set_interaction_mode(o, eOmc_interactionmode_stiff);
    char info[90] = {};
    
    for (int k=0; k<*(o->pN); ++k)
    {
        //set pointers to data
        Joint* j_ptr = o->joint+o->joints_of_set[k];
        Motor* m_ptr = o->motor+o->motors_of_set[k];
        AbsEncoder* e_ptr = o->absEncoder+o->encoders_of_set[k];
        jointCalibType14Data* jcalibdata_ptr = &(j_ptr->running_calibration.data.type14);
        
        switch(jcalibdata_ptr->state)
        {
            case calibtype14_st_absEncoderCalib:
            {
                if(!Motor_is_calibrated(m_ptr))
                {
                    Motor_do_calibration_hard_stop(m_ptr);
                }
                else
                {
                    // debug code
                    memset(&info[0], 0, sizeof(info));
                    snprintf(info, sizeof(info), "Hw lim reached: mcp:%d cz:%d co:%d eps:%d", m_ptr->pos_fbk, m_ptr->hardstop_calibdata.zero, m_ptr->pos_calib_offset, e_ptr->position_sure);
                    JointSet_send_debug_message(info, j_ptr->ID, 0, 0);
                    // debug code ended
                    
                    AbsEncoder_calibrate_absolute(e_ptr, 0, jcalibdata_ptr->computedZero);
                    
                    jcalibdata_ptr->state = calibtype14_st_hardLimitSet;
                }
            }
            break;
            
            case calibtype14_st_hardLimitSet:
            {
                //if the current position (computed with calib param of abs encoder) is out of limits range, I'll put joint in fault
                //the limit range is very big. this check should save us from wrong calib params.
                int32_t curr_pos = AbsEncoder_position(e_ptr);
                        
                // debug code
                memset(&info[0], 0, sizeof(info));
                snprintf(info, sizeof(info), "Encoder pos:%d and zero:%d", curr_pos, e_ptr->zero);
                JointSet_send_debug_message(info, j_ptr->ID, 0, 0);
                // debug code ended
                
                if((curr_pos > j_ptr->pos_max+CALIB_TYPE_6_7_POS_ERROR_TRHESHOLD) || (curr_pos < j_ptr->pos_min-CALIB_TYPE_6_7_POS_ERROR_TRHESHOLD))
                {
                    jcalibdata_ptr->is_active = FALSE;
                    memset(&info[0], 0, sizeof(info));
                    snprintf(info, sizeof(info), "calib 14:outLim: cp:%d mx:%.1f mn:%.1f", curr_pos, j_ptr->pos_max, j_ptr->pos_min);
                    JointSet_send_debug_message(info, j_ptr->ID, 0, 0);
                    return FALSE;
                }
                
                // debug code
                memset(&info[0], 0, sizeof(info));
                snprintf(info, sizeof(info), "New Mtr limits: m_min=%d m_max=%d", m_ptr->pos_min, m_ptr->pos_max);
                JointSet_send_debug_message(info, j_ptr->ID, 0, 0);
                // debug code ended

                jcalibdata_ptr->state = calibtype14_st_finished;
            }
            break;
            
            case calibtype14_st_finished:
            {
                //set status to complete calibration
                jcalibdata_ptr->is_active = FALSE;
                calibrated = TRUE;
                
                //Debug code 
                char info[80];
                snprintf(info, sizeof(info), "Calib Completed.");
                JointSet_send_debug_message(info, j_ptr->ID, 0, 0);
                //ended
                
            }
            break;
            
            default:
                return FALSE;
        }// end switch
        
    }
    return calibrated;
}


// static funtions

static eoas_pos_ROT_t JointSet_calib14_ROT2pos_ROT(eOmc_calib14_ROT_t rot)
{
    eoas_pos_ROT_t retValue = eoas_pos_ROT_unknown;
    switch(rot)
    {
        case eOmc_calib14_ROT_zero:
            return eoas_pos_ROT_zero;
            break;
        
        case eOmc_calib14_ROT_plus180:
            return eoas_pos_ROT_plus180;
            break;
        
        case eOmc_calib14_ROT_plus090:
            return eoas_pos_ROT_plus090;
            break;
        
        case eOmc_calib14_ROT_minus090:
            return eoas_pos_ROT_minus090;
            break;
        
        case eOmc_calib14_ROT_none:
            return eoas_pos_ROT_none;
            break;
        
        case eOmc_calib14_ROT_unknown:
            return eoas_pos_ROT_unknown;
            break;
        
        default:
            break;
    }
    
    return retValue;
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/




// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_app_eth_theServiceMC.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolMC.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


// -- entity joint



extern void eoprot_fun_UPDT_mc_joint_config(const EOnv* nv, const eOropdescriptor_t* rd)
{  
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});
    
//    eOmc_joint_config_t *cfg = (eOmc_joint_config_t*)rd->data;
//    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
////    eOmc_joint_status_t *jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), jxx);
//    
//    // now we see if it is a mc4can or a 2foc or a mc4plus    
//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();

//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        MController_config_joint(jxx, cfg);
//    }

}



extern void eoprot_fun_UPDT_mc_joint_config_pidposition(const EOnv* nv, const eOropdescriptor_t* rd)
{   
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});
    
//    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
//    eOmc_PID_t *pid = (eOmc_PID_t*)rd->data;

//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        MController_config_minjerk_pid(jxx, pid);
//    }
}


extern void eoprot_fun_UPDT_mc_joint_config_pidvelocity(const EOnv* nv, const eOropdescriptor_t* rd)
{  
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});
    
//    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
//    eOmc_PID_t *pid = (eOmc_PID_t*)rd->data;

//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        //MController_config_direct_pid(jxx, pid);
//    }
}


extern void eoprot_fun_UPDT_mc_joint_config_pidtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{   
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});
    
//    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
//    eOmc_PID_t *pid = (eOmc_PID_t*)rd->data;
//    //float rescaler = 1.0f/(float)(1<<pid->scale);

//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        MController_motor_config_torque_PID(jxx, pid);
//    }

}


extern void eoprot_fun_UPDT_mc_joint_config_motor_params(const EOnv* nv, const eOropdescriptor_t* rd) 
{       
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});
    
//    eOmc_motor_params_t *mparams = (eOmc_motor_params_t*)rd->data;
//    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);        

//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        MController_config_motor_friction(jxx, mparams);
//    }
    
}



extern void eoprot_fun_UPDT_mc_joint_config_impedance(const EOnv* nv, const eOropdescriptor_t* rd)
{   
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});
    
//    eOmc_impedance_t *impedance = (eOmc_impedance_t*)rd->data;
//    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
//    
//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        MController_config_joint_impedance(jxx, impedance);
//    } // mark
}



extern void eoprot_fun_UPDT_mc_joint_config_userlimits(const EOnv* nv, const eOropdescriptor_t* rd)
{   
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});
    
//    eOmeas_position_limits_t *limitsofjoint = (eOmeas_position_limits_t*)rd->data;
//    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
//    
//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        MController_config_joint_pos_limits(jxx, limitsofjoint->min, limitsofjoint->max);
//    }

}

#warning the funtion / tag eoprot_fun_UPDT_mc_joint_config_velocitysetpointtimeout() does not exist anymore
// f-marker-begin
//extern void eoprot_fun_UPDT_mc_joint_config_velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd)
//{
//    eOmeas_time_t *time = (eOmeas_time_t*)rd->data;
//    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);

//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        MController_config_joint_vel_ref_timeout(jxx, *time);
//    } // marker

//}


extern void eoprot_fun_UPDT_mc_joint_status_core_modes_ismotiondone(const EOnv* nv, const eOropdescriptor_t* rd)
{
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});
    
//    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//   
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        // do nothing
//    } // marker
 
}



extern void eoprot_fun_UPDT_mc_joint_cmmnds_setpoint(const EOnv* nv, const eOropdescriptor_t* rd)
{   
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});    

//#if 0
//    
//    eOmc_setpoint_t *setpoint = (eOmc_setpoint_t*)rd->data;
//    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
//    eOmc_joint_t *joint = eo_entities_GetJoint(eo_entities_GetHandle(), jxx);
//    
//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();

//    if(NULL == joint)
//    {
//        return; // error... maybe robotInterface sends a command to a joint with index higher than supported
//    } 
//       

//    joint->status.core.modes.ismotiondone = eobool_false;

//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {        
//        switch(setpoint->type)
//        { 
//            case eomc_setpoint_position:
//            {
//                MController_set_joint_pos_ref(jxx, setpoint->to.position.value, setpoint->to.position.withvelocity);
//            } break;
//            
//            case eomc_setpoint_positionraw:
//            {
//                MController_set_joint_pos_raw(jxx, setpoint->to.positionraw.value);
//            } break;
//            
//            case eomc_setpoint_velocity:
//            {
//                MController_set_joint_vel_ref(jxx, setpoint->to.velocity.value, setpoint->to.velocity.withacceleration);
//            } break;

//            case eomc_setpoint_torque:
//            {
//                MController_set_joint_trq_ref(jxx, setpoint->to.torque.value);
//            } break;

//            case eomc_setpoint_openloop:
//            {
//                MController_set_joint_pwm_ref(jxx, setpoint->to.openloop.value);
//            } break;

//            case eomc_setpoint_current:
//            {
//                MController_set_joint_cur_ref(jxx, setpoint->to.current.value);
//            } break;
//            
//            default:
//            {
//                
//            } break;
//        }
//        
//        MController_update_joint_targets(jxx);
//    }
//    
//#endif
    
    
} // f-marker-end


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_cmmnds_stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd)
{   
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});    
    
//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
//        MController_stop_joint(jxx);
//    }

} // fmarker-end


//// f-marker-begin
//static void send_diagnostic_debugmessage(eOerrmanErrorType_t type, eOerror_value_DEB_t value, uint8_t jointnum, uint16_t par16, uint64_t par64, const char* info)
//{
//    eOerrmanDescriptor_t errdes = {0};

//    errdes.code             = eoerror_code_get(eoerror_category_Debug, value);
//    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//    errdes.sourceaddress    = jointnum;
//    errdes.par16            = par16;
//    errdes.par64            = par64;
//    eo_errman_Error(eo_errman_GetHandle(), type, info, NULL, &errdes);
//}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_cmmnds_calibration(const EOnv* nv, const eOropdescriptor_t* rd)
{      
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});

//#if 0
//    
//    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
//    eOmc_calibrator_t *calibrator = (eOmc_calibrator_t*)rd->data;    
//    
//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        MController_calibrate(jxx, calibrator);        
//    } 
//#endif
    
}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_cmmnds_controlmode(const EOnv* nv, const eOropdescriptor_t* rd)
{    
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});   
    
//    eOmc_controlmode_command_t *controlmode = (eOmc_controlmode_command_t*)rd->data;
//    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
//    
//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        MController_set_control_mode(jxx, *controlmode);        
//    }    

}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_cmmnds_interactionmode(const EOnv* nv, const eOropdescriptor_t* rd)
{      
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});  
    
//    eOmc_interactionmode_t* interaction = (eOmc_interactionmode_t*)rd->data;
//    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
//    
//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        MController_set_interaction_mode(jxx, *interaction);
//    } // marker

}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_inputs_externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{  
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});    

//#if 0    
//    eOmeas_torque_t *torque = (eOmeas_torque_t*)rd->data;
//    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
//    
//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        MController_update_joint_torque_fbk(jxx, *torque);
//    }
//#endif
    
}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_motor_config_gearbox_M2J(const EOnv* nv, const eOropdescriptor_t* rd)
{      
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0}); 
    
//    // not for mc4can
//    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
//    float32_t *gbxratio = (float32_t*)rd->data;

//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        MController_config_motor_gearbox_M2J(jxx, *gbxratio);
//    }
    
}



extern void eoprot_fun_UPDT_mc_motor_config_rotorencoder(const EOnv* nv, const eOropdescriptor_t* rd)
{          
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});     

//    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
//    int32_t *rotenc = (int32_t*)rd->data;    
//    
//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        MController_config_motor_encoder(jxx, *rotenc);
//    }
    
}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_motor_config(const EOnv* nv, const eOropdescriptor_t* rd)
{ 
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});    
    
//    eOmc_motor_config_t *mconfig = (eOmc_motor_config_t*)rd->data;
//    eOprotIndex_t mxx = eoprot_ID2index(rd->id32);

//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//            
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        eo_motioncontrol_ConfigMotor(eo_motioncontrol_GetHandle(), mxx, mconfig);         
//    }

}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_motor_config_pidcurrent(const EOnv* nv, const eOropdescriptor_t* rd)
{    
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});    
    
//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        eOmc_PID_t *pid = (eOmc_PID_t*)rd->data;
//        eOprotIndex_t mxx = eoprot_ID2index(rd->id32);

//        MController_motor_config_current_PID(mxx, pid);

//        return;
//    }
}

// f-marker-begin
extern void eoprot_fun_UPDT_mc_motor_config_pidspeed(const EOnv* nv, const eOropdescriptor_t* rd)
{   
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});   
    
//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        eOmc_PID_t *pid = (eOmc_PID_t*)rd->data;
//        eOprotIndex_t mxx = eoprot_ID2index(rd->id32);

//        MController_motor_config_speed_PID(mxx, pid);

//        return;
//    } 
}

// f-marker-begin
extern void eoprot_fun_UPDT_mc_motor_config_currentlimits(const EOnv* nv, const eOropdescriptor_t* rd)
{    
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0}); 
    
//    eOprotIndex_t mxx = eoprot_ID2index(rd->id32);

//    eOmc_current_limits_params_t *currentLimits = (eOmc_current_limits_params_t*)rd->data;

//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    // marco.accame on 22 apr 2021: detected ERROR
//    // it is wrong the way the CAN frame of type ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT is
//    // managed, both in here and inside MController_motor_config_max_currents().
//    // because:
//    // 1. ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT requires command.value to be a pointer 
//    //    to eOmc_current_limits_params_t and in here we have a pointer to eOmc_current_limits_params_t::overloadCurrent
//    // 2. also MController_motor_config_max_currents() in its internals sends a CAN frame
//    //    of type ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT but in a wrong way because
//    //    it uses eOmc_current_limits_params_t::peakCurrent instead
//    // luckily, the MController from inside Motor_config_2FOC() sends correctly the 
//    // current limits using the correct argument eOmc_current_limits_params_t.    
//    // so far I prefer just to log this error. 
//    
//   
//    // foc-base and mc4based should send overloadCurrent to can-boards


//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        MController_motor_config_max_currents(mxx, currentLimits);
//    }
}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_motor_config_pwmlimit(const EOnv* nv, const eOropdescriptor_t* rd)
{     
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});    
    
//    eOprotIndex_t mxx = eoprot_ID2index(rd->id32);
//    eOmeas_pwm_t *pwm_limit = (eOmeas_pwm_t *)rd->data;

//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
//    
//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        *pwm_limit = MController_config_motor_pwm_limit(mxx, *pwm_limit);
//    }

}

extern void eoprot_fun_UPDT_mc_motor_config_temperaturelimit(const EOnv* nv, const eOropdescriptor_t* rd)
{       
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0});   
    
//    
//    eOprotIndex_t mxx = eoprot_ID2index(rd->id32);

//    eOmeas_temperature_t *temperatureLimit = (eOmeas_temperature_t*)rd->data;

//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();

//    if(eo_motcon_mode_foc == mcmode)
//    {
//        MController_motor_config_max_temperature(mxx, temperatureLimit);
//    }
//    else 
//    {
//       // just send the can message to relevant board
//       //  send the can message to relevant board
//       // NOT MANAGED
//    }
    
}

extern void eoprot_fun_UPDT_mc_controller_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    embot::app::eth::theServiceMC::getInstance().process({rd, nv, 0}); 
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

// -- common functions

//static eOmotioncontroller_mode_t s_motorcontrol_getmode(void)
//{
//    return(eo_motioncontrol_GetMode(eo_motioncontrol_GetHandle()));
//}


// -- 2foc-based functions


// -- mc4plus-based funtions



// -- mc4can-based functions



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




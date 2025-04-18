
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#ifndef MC_CONTROLLER_HID___
#define MC_CONTROLLER_HID___


// dependencies
#include "EOemsControllerCfg.h"
#include "EoManagement.h"   
#include "JointSet.h"
#include "Joint.h"
#include "Motor.h"
#include "AbsEncoder.h"


struct MController_hid
{
    uint8_t nEncods;
    uint8_t nJoints;
    uint8_t nSets;
    
    uint8_t* set_dim;
    uint8_t* enc_set_dim;
    
    uint8_t multi_encs;
    
    uint8_t** jos;
    uint8_t** mos;
    uint8_t** eos;
    
    uint8_t *j2s;
    uint8_t *m2s;
    uint8_t *e2s;

    JointSet *jointSet;         // all jointsets, max MAX_JOINTS_PER_BOARD = 4    
    Motor *motor;               // all motors, max MAX_MOTORS_PER_BOARD = 4
    Joint *joint;               // all joints, max MAX_JOINTS_PER_BOARD = 4
    AbsEncoder *absEncoder;     // all encoders, max MAX_ENCODS_PER_BOARD = 8
   
    float **Jjm;                // matrix 4x4 (j m)
    float **Jmj;                // matrix 
    
    float **Sjm;
    float **Smj;
    
    float **Sje;
    
    MC_ACTUATION_t actuation_type;
    
    
    // it gets those values of eOmn_serv_type_t that belong to category eomn_serv_category_mc.
    // they are: eomn_serv_MC* 
    // i could use also values of eOmotioncontroller_mode_t but i prefere remove dependancy from EOtheMotionController.h in here
    eOmn_serv_type_t mcmode; 
    
    BOOL isMaintenanceMode;
};
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

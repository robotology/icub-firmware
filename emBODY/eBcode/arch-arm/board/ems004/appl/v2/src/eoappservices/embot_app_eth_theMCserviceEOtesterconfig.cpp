
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theMCserviceEOtesterconfig.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <array>

#include "EoProtocolMC.h"
#include "EOtheMotionController.h"



#if defined(USE_EMBOT_theServices)
#include "embot_app_eth_theServices.h"
#include "embot_app_eth_Service_legacy.h"
#else 
#include "EOtheServices_hid.h"
#endif

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


namespace embot::app::eth {
    

struct embot::app::eth::theMCserviceEOtesterconfig::Impl
{
    
    const theServiceTester::Config & servicetesterconfig(Type type) const;
    
    
    static void setconfig();
    static void txstart();
        
    // the object EOtheMotionControl does not have this method, so we put it in here 
    static eOresult_t process(const eOropdescriptor_t* rd, const EOnv* nv = nullptr);  

    static const eOropdescriptor_t * fill0(eOropdescriptor_t &rd, eOnvID32_t id32, void *data, uint16_t size, eOropcode_t rpc);
      
    
    static constexpr size_t numberofJOMOs2test {3};
    
    static constexpr eOcanport_t canport {eOcanport1};
    static constexpr eOcanport_t othercanport {eOcanport2};

    static constexpr eOmn_serv_configuration_t s_serv_config_MC_foc_eb1_fake_aea =
    {
        // this configuration is a MC_foc service.
        // it has three joints served by three amcbldc
        // it does no force discovery of foc board unless we have .... 
        .type               = eomn_serv_MC_foc,
        .diagnosticsmode    = eomn_serv_diagn_mode_NONE,
        .diagnosticsparam   = 0,
        .data { .mc { .foc_based  = 
        {
            .version   =
            {
                .firmware   = { .major = 0, .minor = 0, .build = 0 },
                .protocol   = { .major = 0, .minor = 0 }
            },
            .type       = eobrd_amcbldc,
            .filler     = {0, 0}, 
            .arrayofjomodescriptors =
            {
                .head   = 
                {
                    .capacity       = 4,
                    .itemsize       = sizeof(eOmc_jomo_descriptor_t),
                    .size           = numberofJOMOs2test,
                    .internalmem    = 0                    
                },
                .data   =
                {
                    { // joint 0
#if defined(YRI_uses_MC_foc_actuator_descriptor_generic)
//                        .actuator { .gen { .location { 
//                            .can = {
//                                .place          = eobrd_place_can,
//                                .port           = canport,
//                                .addr           = 3,
//                                .ffu            = 0
//                            }                            
//                        }}},
                        .actuator { .gen { .location { 
                            .eth = {
                                .place          = eobrd_place_eth,
                                .id             = 1
                            }                            
                        }}},                        
#else                        
                        .actuator { .foc { .canloc = 
                        {
                            .port           = canport,
                            .addr           = 3,
                            .insideindex    = eobrd_caninsideindex_first                             
                        }}},
#endif                        
                        .encoder1         =
                        {
                            .type   = eomc_enc_aea,
                            .port   = eobrd_port_emsP10,    
                            .pos    = eomc_pos_atjoint
                        },
                        .encoder2    =
                        {
                            .type   = eomc_enc_none,
                            .port   = eobrd_port_none,
                            .pos    = eomc_pos_none
                        }
                    },
                    { // joint 1
                        
#if defined(YRI_uses_MC_foc_actuator_descriptor_generic)
                        .actuator { .gen { .location { 
                            .can = {
                                .place          = eobrd_place_can,
                                .port           = canport,
                                .addr           = 2,
                                .ffu            = 0
                            }                            
                        }}},
#else                           
                         .actuator { .foc { .canloc = 
                        {
                            .port           = canport,
                            .addr           = 2,
                            .insideindex    = eobrd_caninsideindex_first                             
                        }}},
#endif                        
                        .encoder1         =
                        {
                            .type   = eomc_enc_aea,
                            .port   = eobrd_port_emsP7, // hal_encoder4
                            .pos    = eomc_pos_atjoint                            
                        },
                        .encoder2    =
                        {
                            .type   = eomc_enc_none,
                            .port   = eobrd_port_none,
                            .pos    = eomc_pos_none
                        }
                    },                    
                    { // joint 2
#if defined(YRI_uses_MC_foc_actuator_descriptor_generic)
                        .actuator { .gen { .location { 
                            .can = {
                                .place          = eobrd_place_can,
                                .port           = canport,
                                .addr           = 1,
                                .ffu            = 0
                            }                            
                        }}},
#else                           
                         .actuator { .foc { .canloc = 
                        {
                            .port           = canport,
                            .addr           = 1,
                            .insideindex    = eobrd_caninsideindex_first                             
                        }}},
#endif                        
                        .encoder1         =
                        {
                            .type   = eomc_enc_aea,
                            .port   = eobrd_port_emsP8, // hal_encoder2  
                            .pos    = eomc_pos_atjoint
                        },
                        .encoder2    =
                        {
                            .type   = eomc_enc_none,
                            .port   = eobrd_port_none,
                            .pos    = eomc_pos_none
                        }
                    },               
                    { // joint 3
#if defined(YRI_uses_MC_foc_actuator_descriptor_generic)
                        .actuator { .gen { .location { 
                            .can = {
                                .place          = eobrd_place_can,
                                .port           = canport,
                                .addr           = 4,
                                .ffu            = 0
                            }                            
                        }}},
#else                           
                        .actuator { .foc { .canloc = 
                        {
                            .port           = canport,
                            .addr           = 4,
                            .insideindex    = eobrd_caninsideindex_first                             
                        }}},
#endif                        
                        .encoder1 =
                        {
                            .type   = eomc_enc_aea,
                            .port   = eobrd_port_emsP9,    // hal_encoder5 
                            .pos    = eomc_pos_atjoint
                        },
                        .encoder2 =
                        {
                            .type   = eomc_enc_none,
                            .port   = eobrd_port_none,
                            .pos    = eomc_pos_none
                        }
                    }                    
                }   
            },  
            .jomocoupling       =
            {
                .joint2set      = 
                {   // each joint is on a different set 
                    0, 1, 2, 255 
                },
                .joint2motor    = 
                {   // zero matrix: use matrix embedded in controller and seecetd by boardtype4mccontroller
                    { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                    { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                    { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                    { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) }        
                },
                .encoder2joint  = 
                {   // identical matrix
                    { EO_COMMON_FLOAT_TO_Q17_14(1.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                    { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                    { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f) },
                    { EO_COMMON_FLOAT_TO_Q17_14(0.0f),      EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(0.0f),    EO_COMMON_FLOAT_TO_Q17_14(1.0f) } 
                }  
            }        
        }}}
    };
 
    static constexpr eOmc_joint_config_t joiconfig[4] =
    {
//        {
//            .mode = eoas_ft_mode_calibrated,
//            .ftperiod = 50,
//            .calibrationset = 0,
//            .temperatureperiod = 1
//        },
//        {
//            .mode = eoas_ft_mode_raw,
//            .ftperiod = 50,
//            .calibrationset = 0,
//            .temperatureperiod = 0
//        }, 
//        {
//            .mode = eoas_ft_mode_raw,
//            .ftperiod = 50,
//            .calibrationset = 0,
//            .temperatureperiod = 0
//        },
//        {
//            .mode = eoas_ft_mode_raw,
//            .ftperiod = 50,
//            .calibrationset = 0,
//            .temperatureperiod = 0
//        }            
    }; 
    
    static constexpr eOmc_motor_config_t motconfig[4] =
    {
//        {
//            .mode = eoas_ft_mode_calibrated,
//            .ftperiod = 50,
//            .calibrationset = 0,
//            .temperatureperiod = 1
//        },
//        {
//            .mode = eoas_ft_mode_raw,
//            .ftperiod = 50,
//            .calibrationset = 0,
//            .temperatureperiod = 0
//        }, 
//        {
//            .mode = eoas_ft_mode_raw,
//            .ftperiod = 50,
//            .calibrationset = 0,
//            .temperatureperiod = 0
//        },
//        {
//            .mode = eoas_ft_mode_raw,
//            .ftperiod = 50,
//            .calibrationset = 0,
//            .temperatureperiod = 0
//        }            
    }; 
    
    
    static constexpr embot::app::eth::theServiceTester::Config FTservtestconfig =
    {
        eomn_serv_category_mc,
        &s_serv_config_MC_foc_eb1_fake_aea,
        setconfig,
        txstart,
        0,  // number of regulars 
        {{ 
            EOPROT_ID_GET(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, 0, eoprot_tag_mc_joint_status),
            EOPROT_ID_GET(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, 0, eoprot_tag_mc_motor_status),            
            //eo_prot_ID32dummy, eo_prot_ID32dummy, 
            eo_prot_ID32dummy, eo_prot_ID32dummy,
            eo_prot_ID32dummy, eo_prot_ID32dummy, eo_prot_ID32dummy, eo_prot_ID32dummy
        }}   
    }; 

};



#if defined(enableTHESERVICETESTER)



const eOropdescriptor_t * embot::app::eth::theMCserviceEOtesterconfig::Impl::fill0(eOropdescriptor_t &rd, eOnvID32_t id32, void *data, uint16_t size, eOropcode_t rpc)
{
    std::memset(&rd, 0, sizeof(rd));
    rd.id32 = id32;
    rd.data = reinterpret_cast<uint8_t*>(data);
    rd.size = size;
    rd.ropcode = rpc;
    return &rd;
}

eOmc_motor_config_t * getmotorconfig()
{
    static eOmc_motor_config_t mc {};
    mc.pidcurrent = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 2, {0, 0, 0}};
    mc.pidspeed = {10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0, 2, {0, 0, 0}};
    mc.gearbox_M2J = 1.0;
    mc.rotorEncoderResolution = 3;
    mc.maxvelocityofmotor = 100;
    mc.currentLimits = {10, 11, 12};
    mc.rotorIndexOffset = 0x66;
    mc.motorPoles = 7;
    mc.hasHallSensor = 1;
    mc.hasTempSensor = 0;
    mc.hasRotorEncoder = 0;
    mc.hasRotorEncoderIndex = 0;
    mc.hasSpeedEncoder = 0;
    mc.useSpeedFbkFromMotor = 0;
    mc.verbose = 1;
    mc.rotorEncoderType = 0x55;
    mc.rotEncTolerance = 6.6;
    mc.pwmLimit = 11;
    mc.temperatureLimit = 15;
    mc.limitsofrotor = {-10, +7};
        
    return &mc;
}

void embot::app::eth::theMCserviceEOtesterconfig::Impl::setconfig()
{
    // marco.accame: in here for now i do nothing.
    // we should actually fill the joint and motor configurations with values that yri gets from xml files
    
    
    // maybe for now a eOmc_motor_config_t ...
    
    eOmc_motor_config_t *m = getmotorconfig();
    eOropdescriptor_t ropdes = {};
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, 0, eoprot_tag_mc_motor_config);
    void *da = m;
    uint16_t si = sizeof(eOmc_motor_config_t);
    fill0(ropdes, id32, da, si, eo_ropcode_set);    
    process(&ropdes); 
    
//    for(uint8_t i=0; i<numberofJOMOs2test; i++)
//    {
//        eOropdescriptor_t ropdes = {};
//        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, eoprot_tag_mc_joint_config);
//        void *da = reinterpret_cast<void*>(const_cast<eOmc_joint_config_t*>(&joiconfig[i]));
//        uint16_t si = sizeof(joiconfig[i]);
//        fill0(ropdes, id32, da, si, eo_ropcode_set);    
//        process(&ropdes);   
//    }  

//    for(uint8_t i=0; i<numberofJOMOs2test; i++)
//    {
//        eOropdescriptor_t ropdes = {};
//        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, i, eoprot_tag_mc_motor_config);
//        void *da = reinterpret_cast<void*>(const_cast<eOmc_motor_config_t*>(&motconfig[i]));
//        uint16_t si = sizeof(motconfig[i]);
//        fill0(ropdes, id32, da, si, eo_ropcode_set);    
//        process(&ropdes);   
//    }    
}

void embot::app::eth::theMCserviceEOtesterconfig::Impl::txstart()
{
    // marco.accame: in here for now i do nothing.
    // we could put in here code that emulates the reception of rops when the board is in run mode
    // some sensor services receive a tx start, an the mc service a lot of commands such as set control mode,
    // calibration, setpoints, etc    
    
    // maybe for nwo i send a controlmode
    //icubCanProto_controlmode_speed_voltage + icubCanProto_controlmode_forceIdle
    
    eOmc_controlmode_command_t cmd {eomc_controlmode_cmd_force_idle};
        
    eOropdescriptor_t ropdes = {};
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, 1, eoprot_tag_mc_joint_cmmnds_controlmode);
    void *da = &cmd;
    uint16_t si = sizeof(eOmc_controlmode_command_t);
    fill0(ropdes, id32, da, si, eo_ropcode_set);    
    process(&ropdes); 
              
    
//    for(uint8_t i=0; i<numberofJOMOs2test; i++)
//    {
//        uint8_t enable = 1;  
//        eOropdescriptor_t ropdes = {};
//        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, i, eoprot_tag_as_ft_cmmnds_enable);
//        fill0(ropdes, id32, &enable, sizeof(enable), eo_ropcode_set);    
//        process(&ropdes);  
//    }       
}

eOresult_t embot::app::eth::theMCserviceEOtesterconfig::Impl::process(const eOropdescriptor_t* rd, const EOnv* nv)
{
    // marco.accame: this function calls what it must at the reception of a rop.
    if((nullptr == rd) || (nullptr == rd->data))
    {
        return eores_NOK_nullpointer;
    }
    
    eOprotEntity_t entity = eoprot_ID2entity(rd->id32); 
    eOprotIndex_t index = eoprot_ID2index(rd->id32);
    eOprotTag_t tag = static_cast<eOprotTag_t>(eoprot_ID2tag(rd->id32));
    
    if(eoprot_entity_mc_joint == entity)
    {
        switch(tag)
        {
            case eoprot_tag_mc_joint_cmmnds_controlmode: 
            {
                eOmc_controlmode_command_t *ctrlmode = reinterpret_cast<eOmc_controlmode_command_t*>(rd->data);
                MController_set_control_mode(index, *ctrlmode);            
            } break;
            
            case eoprot_tag_mc_joint_config: 
            {
                eOmc_joint_config_t *cfg = reinterpret_cast<eOmc_joint_config_t*>(rd->data);
                MController_config_joint(index, cfg);            
            } break;
            
            default:
            {            
            } break;
            
        }
    }
    else if(eoprot_entity_mc_motor == entity)
    {
        switch(tag)
        {
            case eoprot_tag_mc_motor_config: 
            {
                eOmc_motor_config_t *cfg = reinterpret_cast<eOmc_motor_config_t*>(rd->data);
                eo_motioncontrol_ConfigMotor(eo_motioncontrol_GetHandle(), index, cfg);            
            } break;
            
            
            default:
            {            
            } break;
            
        }
    }
    

    
    return eores_OK;
}


const theServiceTester::Config & embot::app::eth::theMCserviceEOtesterconfig::Impl::servicetesterconfig(Type type) const
{
   return FTservtestconfig;
}

#else

const theServiceTester::Config & embot::app::eth::theMCserviceEOtesterconfig::Impl::servicetesterconfig(Type type) const
{
   static constexpr embot::app::eth::theServiceTester::Config FTservtestconfig {};
   return FTservtestconfig;
}

#endif

} // namespace embot::app::eth {

// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theMCserviceEOtesterconfig& embot::app::eth::theMCserviceEOtesterconfig::getInstance()
{
    static theMCserviceEOtesterconfig* p = new theMCserviceEOtesterconfig();
    return *p;
}

embot::app::eth::theMCserviceEOtesterconfig::theMCserviceEOtesterconfig()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::eth::theMCserviceEOtesterconfig::~theMCserviceEOtesterconfig() { }
        


const embot::app::eth::theServiceTester::Config & embot::app::eth::theMCserviceEOtesterconfig::servicetesterconfig(Type type) const
{
    return pImpl->servicetesterconfig(type);
}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



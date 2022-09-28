
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Simone Girardi
 * email:   simone.girardi@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theEncoderReader.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os.h"
#include "embot_hw_encoder.h"
#include "EOtheErrorManager.h"
#include "EoProtocol.h"
#include "EoMotionControl.h"
#include "EoError.h"
#include "EOVtheCallbackManager.h"

#include "embot_app_eth_theServices.h"
#include "embot_app_eth_Service_legacy.h"

using namespace std;
using namespace embot::hw;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------
 
constexpr eOservice_core_t dummy_service_core 
{
    .initted = eobool_false,
    .active = eobool_false,
    .activateafterverify = eobool_false,
    .started = eobool_false,
    .onverify = nullptr,
    .onverifyarg = nullptr,
    .state = eomn_serv_state_notsupported,
    .tmpcfg = nullptr,
    .servconfig = { .type = eomn_serv_NONE }
};    


constexpr eOservice_diagnostics_t dummy_service_diagnostics
{
    .reportTimer = nullptr,
    .reportPeriod = 0, // 10*EOK_reltime1sec, // with 0 we dont periodically report
    .errorDescriptor = {0},
    .errorType = eo_errortype_info,
    .errorCallbackCount =  0, 
    .repetitionOKcase =  0 // 10 // with 0 we transmit report only once at succesful activation    
}; 

struct embot::app::eth::theEncoderReader::Impl
{
    Config _config {};
    bool _initted {false};
    bool _actived {false};
    
    eOservice_core_t service {dummy_service_core};
    eOservice_diagnostics_t diagnostics {dummy_service_diagnostics};
    
    typedef struct
    {
        eOmc_encoder_descriptor_t *descriptor;
        eOencoderreader_valueInfo_t *valueinfo;
    } eOencoderProperties_t;
    
    constexpr static const char s_eobj_ownname[] = "theEncoderReader";
    
    Impl() = default;
    
    eOresult_t initialise();
    eOresult_t Verify(      EOconstarray *arrayofjomodes, 
                            eOservice_onendofoperation_fun_t onverify,
                            void *arg,        
                            bool activateafterverify);
                            
    eOresult_t activate(EOconstarray *arrayofjomodes, eOmn_serv_diagn_cfg_t dc);
    eOresult_t deactivate();
    eOresult_t startReading();
    eOresult_t read(uint8_t position, eOencoderreader_valueInfo_t *valueInfo1, eOencoderreader_valueInfo_t *valueInfo2);
    bool report();
                            
    bool isreadingavailable();
    
    static eObool_t s_eo_isconnected(eOmc_encoder_descriptor_t *des);
    static uint32_t rescale2icubdegrees(uint32_t val_raw, uint8_t jomo, eOmc_position_t pos);
    static eObool_t isValidValue_AEA(const uint32_t &valueraw, eOencoderreader_errortype_t *error);
    static void alert(void* p);
    static void startup(embot::os::Thread *t, void *p);
};


eOresult_t embot::app::eth::theEncoderReader::Impl::initialise()
{
    if(true == _initted)
    {
        return eores_OK;
    }
    
    
    embot::core::print("embot::app::eth::theEncoderReader::Impl::initialise()");
     
//    s_eo_theappencreader.totalnumberofencoders = 0;
//    
//    // step 1: retrieve the spi mapping from hal
//    
//    s_eo_theappencreader.stream_map = hal_spiencoder_stream_map_get();
//    
//    if(NULL == s_eo_theappencreader.stream_map)
//    {
//        // marco.accame: put a diagnostics message about the failure
//        return(NULL);
//    }   

//    memset(&s_eo_theappencreader.config, 0, sizeof(s_eo_theappencreader.config));
//    
//    s_eo_clear_SPI_streams(&s_eo_theappencreader);   
//    
//    
//    // amodiag is an extra respect to the legacy diagnostics.
//    // in here i just init it. later i will set its enable state
//    s_eo_appEncReader_amodiag_Init();

    service.servconfig.type = eomn_serv_NONE;
    service.initted = eobool_true;
    service.active = eobool_false;
    service.started = eobool_false;    
    service.state = eomn_serv_state_idle;  
    
    diagnostics.reportTimer = eo_timer_New();
    diagnostics.errorType = eo_errortype_error;
    diagnostics.errorDescriptor.sourceaddress = eo_errman_sourcedevice_localboard;
    diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_encoders_failed_verify);   
    
    _initted = true;
    return eores_OK;
}

eOresult_t embot::app::eth::theEncoderReader::Impl::Verify(EOconstarray *arrayofjomodes, 
                            eOservice_onendofoperation_fun_t onverify,
                            void *arg,        
                            bool activateafterverify)
{ 
   
    
    service.state = eomn_serv_state_verifying;
    //synchservice(service.state);

    // make sure the timer is not running
    eo_timer_Stop(diagnostics.reportTimer);  
    
    service.onverify = onverify;
    service.onverifyarg = arg;
    service.activateafterverify = activateafterverify;
    
    // we dont check and we just assume that everything is all right.
    // 1. we activate
    // 2. we send a nice disgnostic message 
    // 3. ...
    
    if(true == activateafterverify)
    {
        activate(arrayofjomodes, {});        
    }
    
    diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
    diagnostics.errorDescriptor.sourceaddress    = 0;
    diagnostics.errorDescriptor.par16            = 0;
    diagnostics.errorDescriptor.par64            = 0;    
//    EOaction_strg astrg = {0};
//    EOaction *act = (EOaction*)&astrg;
//    eo_action_SetCallback(act, s_send_periodic_error_report, this, eov_callbackman_GetTask(eov_callbackman_GetHandle()));
              
    diagnostics.errorType = eo_errortype_debug;
    diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_encoders_ok);
    eo_errman_Error(eo_errman_GetHandle(), diagnostics.errorType, NULL, s_eobj_ownname, &diagnostics.errorDescriptor);
    
//    if((0 != diagnostics.repetitionOKcase) && (0 != diagnostics.reportPeriod))
//    {
//        diagnostics.errorCallbackCount = diagnostics.repetitionOKcase;        
//        eo_timer_Start(diagnostics.reportTimer, eok_abstimeNOW, diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
//    }
         
    if(NULL != service.onverify)
    {
        service.onverify(service.onverifyarg, eobool_true); 
    }    
    
    return eores_OK;  
}


eOresult_t embot::app::eth::theEncoderReader::Impl::activate(EOconstarray *arrayofjomodes, eOmn_serv_diagn_cfg_t dc)
{
    eo_errman_Trace(eo_errman_GetHandle(), "::Activate()", s_eobj_ownname);
    
    if((nullptr == arrayofjomodes))
    {
        return eores_NOK_nullpointer;
    } 

    if(eo_constarray_Size(arrayofjomodes) > max_number_of_jomos)
    {
        return eores_NOK_generic;
    }

    EOconstarray* carray = eo_constarray_Load((const EOarray*)arrayofjomodes);
    
    if(eobool_true == _actived)
    {
        deactivate();
    }

    
    // 1. prepare the config
    _config.numofjomos = eo_constarray_Size(carray);
    
    for(uint8_t i=0; i < _config.numofjomos; i++)
    {
        const eOmc_jomo_descriptor_t *jomodes = (eOmc_jomo_descriptor_t*) eo_constarray_At(carray, i);
        if(NULL != jomodes)
        {
            _config.jomo_cfg[i].encoder1des = jomodes->encoder1;
            _config.jomo_cfg[i].encoder2des = jomodes->encoder2;
            
            embot::hw::encoder::Config cfg {};
            
            switch(_config.jomo_cfg[i].encoder1des.type)
            {
                case eomc_enc_aea:
                    cfg.type = embot::hw::encoder::Type::chipAS5045;
                    break;
                case eomc_enc_aea3:
                    cfg.type = embot::hw::encoder::Type::chipMA730;
                    break;
                case eomc_enc_aksim2:
                    cfg.type = embot::hw::encoder::Type::chipMB049;
                    break;
                default:
                    // unsupported encoder
                    return eores_NOK_unsupported;
            }
            
            // 2. configure and initialize SPI encoders
            switch(_config.jomo_cfg[i].encoder1des.port)
            {
                case eobrd_port_amc_J5_X1:
                    embot::hw::encoder::init(embot::hw::ENCODER::one, cfg);
                    break;
                case eobrd_port_amc_J5_X2:
                    embot::hw::encoder::init(embot::hw::ENCODER::two, cfg);
                    break;
                case eobrd_port_amc_J5_X3:
                    embot::hw::encoder::init(embot::hw::ENCODER::three, cfg);
                    break;
                default:
                    // error invalid SPI
                    return eores_NOK_generic;
            }
        }
    }
    
//    // 3. configure other encoders
//    s_eo_appEncReader_configure_NONSPI_encoders(p);
    
    
    _actived = eobool_true;
    
    // to enable the diagnostics ... use on equal to eobool_true
//    s_eo_appEncReader_amodiag_Config(dc);     
//    eo_appEncReader_Diagnostics_Enable(p, (eomn_serv_diagn_mode_MC_ENC == dc.mode) ? eobool_true: eobool_false);
    
    return eores_OK;
}

eOresult_t embot::app::eth::theEncoderReader::Impl::deactivate()
{
    eo_errman_Trace(eo_errman_GetHandle(), "::Deactivate()", s_eobj_ownname);
    
    if(eobool_false == _actived)
    {
        return(eores_OK);
    }
    
    if((resOK == embot::hw::encoder::deinit(embot::hw::ENCODER::one)) &&
       (resOK == embot::hw::encoder::deinit(embot::hw::ENCODER::two)) &&
       (resOK == embot::hw::encoder::deinit(embot::hw::ENCODER::three)))
    {
        _config.numofjomos = 0;
        _actived = false;
        
        return eores_OK;
    }
    else
    {
        return eores_NOK_generic;
    }
}

eOresult_t embot::app::eth::theEncoderReader::Impl::startReading()
{
    //eo_errman_Trace(eo_errman_GetHandle(), "::Start()", s_eobj_ownname);
    
    // start the encoder reading
    embot::hw::encoder::startRead(embot::hw::ENCODER::one);
    embot::hw::encoder::startRead(embot::hw::ENCODER::two);
    embot::hw::encoder::startRead(embot::hw::ENCODER::three);
    
    return eores_OK;
}

eOresult_t embot::app::eth::theEncoderReader::Impl::read(uint8_t jomo, eOencoderreader_valueInfo_t *valueInfo1, eOencoderreader_valueInfo_t *valueInfo2)
{
    if((nullptr == valueInfo1) || (nullptr == valueInfo2))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == _actived)
    {   // nothing to do because we dont have it
        return eores_OK;
    }
    
    embot::hw::ENCODER e = static_cast<embot::hw::ENCODER>(jomo);
    if(e >= embot::hw::ENCODER::maxnumberof)
    {
        return eores_NOK_generic;
    }
    
    eOencoderProperties_t encProp[2] = {NULL};
    
    encProp[0].descriptor = &_config.jomo_cfg[jomo].encoder1des;
    encProp[0].valueinfo = valueInfo1;
    
    encProp[1].descriptor = &_config.jomo_cfg[jomo].encoder2des;
    encProp[1].valueinfo = valueInfo2;
    
    for(uint8_t i=0; i<2; i++)
    {   // for each of the two encoders ....
        eOencoderProperties_t prop = encProp[i]; 
        uint16_t errorparam = 0;
        
        // assign composedof and position
        prop.valueinfo->composedof = eomc_encoder_get_numberofcomponents((eOmc_encoder_t)prop.descriptor->type); 
        prop.valueinfo->position = (eOmc_position_t)prop.descriptor->pos;
        
        // so far we assume no errors and we assign 0 to all values
        prop.valueinfo->errortype = encreader_err_NONE;
        memset(prop.valueinfo->value, 0, sizeof(prop.valueinfo->value)); // was not 0 but: hal_NA32;
        
        // now we compute the value(s) and adjust if we detect any errors
        
        if(eobool_false == s_eo_isconnected(prop.descriptor))
        {   // the encoder is not connected: adjust its error type and continue with other encoders in the loop
            prop.valueinfo->errortype = encreader_err_NOTCONNECTED;
            continue;   // KEEP this continue! IT'S IMPORTANT!
        } 
        
        // ok, we have a connected encoder. we see what type it is and we perform the proper acquisition
        // we also retrieve its diagnostic
        //hal_spiencoder_diagnostic_t diagn = {0};
        //diagn.type = hal_spiencoder_diagnostic_type_none;
        
        switch(prop.descriptor->type)
        {
            // they are: eomc_enc_aea3, eomc_enc_aea, eomc_enc_amo, eomc_enc_spichainof2,
            //           eomc_enc_spichainof3, eomc_enc_qenc, eomc_enc_absanalog, eomc_enc_mais
            
            case eomc_enc_aea:
            {
                embot::hw::encoder::POS spiRawValue = 0; 
                // hal_spiencoder_errors_flags flags = {0};
                
                
                // if(hal_res_OK == hal_spiencoder_get_value((hal_spiencoder_t)prop.descriptor->port, &spiRawValue, &flags))
                if(resOK == embot::hw::encoder::getValue(e, spiRawValue/*, &diagn*/))                
                {   // ok, the hal reads correctly
                    if(eobool_true == isValidValue_AEA(spiRawValue, &prop.valueinfo->errortype))
                    {   // the spi raw reading from hal is valid. i just need to rescale it.

                        // marco.accame: hal_spiencoder_get_value2() gives back a value in uint32_t with only 18 bits of information (internally masked with 0x03FFFF).
                        // only the 12 most significant bits contain a position reading. to obtain the ticks we should do:
                        // ticks = (spiRawValue >> 6) & 0x0FFF;
                        // the resolution is now 4096 ticks per revolution.
                        
                        // GOOD VALUE:
                        uint32_t ticks = spiRawValue; //(spiRawValue >> 6) & 0x0FFF;
                        prop.valueinfo->value[0] = rescale2icubdegrees(ticks, jomo, (eOmc_position_t)prop.descriptor->pos);                           
                    }
                    else
                    {   // we have a valid raw value from hal but ... it is not valid after a check                        
                        prop.valueinfo->errortype = prop.valueinfo->errortype;
                        errorparam = spiRawValue; //(spiRawValue >> 6) & 0x0FFF;                                           
                    }                    
                }
                else
                {   // we dont even have a valid reading from hal
                    prop.valueinfo->errortype = encreader_err_AEA_READING;
                    errorparam = 0xffff;                                         
                }   
               
            } break;
            
            default:
            {   // we have not recognised any valid encoder type
                prop.valueinfo->errortype = encreader_err_GENERIC;   
                errorparam = 0;                
            } break;
        }
    }
    


    //eOresult_t res = eo_appEncReader_GetValue(s_eo_theencoderreader.reader, position, primary, secondary); 
    
    return eores_OK;
}

bool embot::app::eth::theEncoderReader::Impl::report()
{
    return true;
}

bool embot::app::eth::theEncoderReader::Impl::isreadingavailable()
{
    return true;
}

// private memmbers

eObool_t embot::app::eth::theEncoderReader::Impl::s_eo_isconnected(eOmc_encoder_descriptor_t *des)
{
    return ((eomc_enc_none != (des->type)) && (eobrd_port_nolocal != (des->port))) ? eobool_true : eobool_false;
}

uint32_t embot::app::eth::theEncoderReader::Impl::rescale2icubdegrees(uint32_t val_raw, uint8_t jomo, eOmc_position_t pos)
{
    // this is the correct code: we divide by the encoderconversionfactor ...
    // formulas are:
    // in xml file there is GENERAL:Encoders = tidegconv = 182.044 = (64*1024/360) is the conversion from degrees to icubdeg and is expressed as [icubdeg/deg]
    // In joint->config.jntEncoderResolution and motor->config.rotorEncoderResolution there are the resolutions of joint and motor encoders,
    // that is number of ticks per round angle.
    // 
    // Thus, to obtain the icub-degress in here we must divide the reading of the encoder expressed in [ticks] by
    // divider and multiply for 65535. (divider is joint->config.jntEncoderResolution or motor->config.rotorEncoderResolution)

    // moreover .... if the encoderconversionfactor is negative, then i assume it is positive. because its sign is managed internally in the ems-controller


    uint32_t retval = val_raw;
    int32_t divider = 1;

    
    if(eomc_pos_atjoint == pos)
    {
        eOmc_joint_t *joint = (eOmc_joint_t*) eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, jomo);
                
        if(nullptr == joint)
        {
            return(2000);
        }
        
        divider = joint->config.jntEncoderResolution;
    }
    else if(eomc_pos_atmotor == pos)
    {
        eOmc_motor_t *motor = (eOmc_motor_t*) eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, jomo);
                
        if(nullptr == motor)
        {
            return(2000);
        }
        
        divider = motor->config.rotorEncoderResolution;
    }
    else
    {
        return(0);
    }
    
    //check divider validity
    if(0.0f == divider)
    {
        return(3000);       
    }
    
    if(divider < 0)
    {
        divider = -divider;
    }

    uint64_t aux = (uint64_t)val_raw* 65535;
    
    retval = aux /divider;
    return(retval);
}


eObool_t embot::app::eth::theEncoderReader::Impl::isValidValue_AEA(const uint32_t &valueraw, eOencoderreader_errortype_t *error)
{
// TODO: probably redundant function
//    uint8_t parity_error = 0;
//    uint8_t b = 0;
//    
//    *error = encreader_err_NONE;
//    
//    for (b=0; b<18; ++b)
//    {
//        parity_error ^= (valueraw)>>b;
//    }
//    
//    if (parity_error & 1) 
//    { 
//        *error = encreader_err_AEA_PARITY;
//        return(eobool_false);
//    }
//    
//    if ((0x38 & valueraw) != 0x20)
//    {
//        *error = encreader_err_AEA_CHIP;
//        return(eobool_false);

//    }
    
    return(eobool_true);
}



// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theEncoderReader& embot::app::eth::theEncoderReader::getInstance()
{
    static theEncoderReader* p = new theEncoderReader();
    return *p;
}

embot::app::eth::theEncoderReader::theEncoderReader()
{
    pImpl = std::make_unique<Impl>();
}

embot::app::eth::theEncoderReader::~theEncoderReader() { }

eOresult_t embot::app::eth::theEncoderReader::initialise()
{
    return pImpl->initialise();
}

eOresult_t embot::app::eth::theEncoderReader::Verify(
                            EOconstarray *arrayofjomodes, 
                            eOservice_onendofoperation_fun_t onverify,
                            void *arg,        
                            bool activateafterverify)
{
    return pImpl->Verify(arrayofjomodes, onverify, arg, activateafterverify); 
}
//        eOresult_t Activate(const eOmn_serv_configuration_t * servcfg);  

eOresult_t embot::app::eth::theEncoderReader::activate(EOconstarray *arrayofjomodes, eOmn_serv_diagn_cfg_t dc)
{
    return pImpl->activate(arrayofjomodes, dc);
}

eOresult_t embot::app::eth::theEncoderReader::deactivate()
{
    return pImpl->deactivate();
}

eOresult_t embot::app::eth::theEncoderReader::startReading()
{
    return pImpl->startReading();
}

eOresult_t embot::app::eth::theEncoderReader::read(uint8_t position, eOencoderreader_valueInfo_t *valueInfo1, eOencoderreader_valueInfo_t *valueInfo2)
{
    return pImpl->read(position, valueInfo1, valueInfo2);
}

bool embot::app::eth::theEncoderReader::report()
{
    return pImpl->report();
}

bool embot::app::eth::theEncoderReader::isreadingavailable()
{
    return pImpl->isreadingavailable();
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



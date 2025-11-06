/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Simone Girardi
 * email:   simone.girardi@iit.it
 * Author:  Kevin Sangalli
 * email:   kevin.sangalli@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theEncoderReader.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_encoder.h"
#include "EOtheErrorManager.h"
#include "EoProtocol.h"
#include "EoMotionControl.h"
#include "EoError.h"
#include "EOVtheCallbackManager.h"

#include "embot_app_eth_theServices.h"
#include "embot_app_eth_Service_legacy.h"

#include <algorithm>

#if defined(STM32HAL_BOARD_AMC) && defined(DEBUG_AEA3_stream_over_theBackdoor)   
#include "embot_app_eth_theBackdoor.h"
#include "EOpacket.h"
#endif
    
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
    static constexpr size_t max_number_of_jomos { 4 };                          
    static constexpr size_t max_number_of_encoders { 2 * max_number_of_jomos }; 

    
    struct jomoconfig_t
    {
        eOmc_encoder_descriptor_t   encoder1des;
        eOmc_encoder_descriptor_t   encoder2des;
    };

    struct ImplConfig
    {
        uint8_t numofjomos {0};
        std::array<jomoconfig_t, max_number_of_jomos> jomo_cfg {};
    };     
    
    embot::app::eth::encoder::v1::IFreader::Config config {};
    ImplConfig _implconfig {};
    bool _initted  {false};
    bool _actived  {false};
    bool _verified {false};
    
    bool activateafterverify {true};
    embot::core::Confirmer onverifycompleted {};
    eOservice_core_t service {dummy_service_core};
    eOservice_diagnostics_t diagnostics {dummy_service_diagnostics};
    
    std::array<std::array<embot::app::eth::encoder::experimental::RawValueEncoder, 2>, max_number_of_jomos> rawvalues {};
    
    struct eOencoderProperties_t
    {
        eOmc_encoder_descriptor_t *descriptor;
        embot::app::eth::encoder::v1::valueInfo *valueinfo;
        eOencoderProperties_t() = default;
        eOencoderProperties_t(eOmc_encoder_descriptor_t *d, embot::app::eth::encoder::v1::valueInfo *v) : descriptor(d), valueinfo(v) {}
    };
    
    constexpr static const char s_eobj_ownname[] = "theEncoderReader";
    
    Impl() = default;
    
    bool initialise();
    
    bool Verify(const Config &config, bool activateafterverify, const embot::core::Confirmer &oncompletion);                            
    bool Activate(const Config &config);    
    bool Deactivate();   
    bool StartReading();    
    bool Read(uint8_t jomo, embot::app::eth::encoder::v1::valueInfo &primary, embot::app::eth::encoder::v1::valueInfo &secondary);
    bool SendReport();                            
    bool IsReadingAvailable();
    bool Diagnostics_Tick() { return true; }
    embot::app::eth::encoder::v1::Type GetType(const embot::app::eth::encoder::v1::Target &target);
    bool Scale(const embot::app::eth::encoder::v1::Target &target, const embot::app::eth::encoder::v1::Scaler &scaler);

    void log();
    
    // advanced
    bool GetRaw(uint8_t jomo, embot::app::eth::encoder::experimental::RawValuesOfJomo &rawValuesArray);
    bool GetRawSingle(uint8_t jomo, embot::app::eth::encoder::experimental::Position pos, embot::app::eth::encoder::experimental::RawValueEncoder &rawValue);

private:    
    static bool s_eo_isconnected(eOmc_encoder_descriptor_t *des);
    static uint32_t rescale2icubdegrees(uint32_t val_raw, uint8_t jomo, eOmc_position_t pos);
    static bool isValidValue_AEA(const uint32_t &valueraw, embot::app::eth::encoder::v1::Error &error);
    static bool isValidValue_AEA3(const uint32_t &valueraw, embot::app::eth::encoder::v1::Error &error);
    bool IsEncoderSupported(const Config &config);
    bool TestRead(const Config &config);

#if defined(STM32HAL_BOARD_AMC) && defined(DEBUG_AEA3_stream_over_theBackdoor)    
    EOpacket *packet {nullptr};    
    eOipv4addr_t hostip {EO_COMMON_IPV4ADDR(10, 0, 1, 104)};
    eOipv4port_t hostport {6666};
    char sss[512] = {0};
#endif


    // port is a uint8_t taken by eOmc_encoder_descriptor_t::port
    embot::hw::ENCODER port2encoder(uint8_t port)
    {
        // marco.accame on 29 oct 2024: for now it is simple .... 
        // on the amc we have port equal only to eobrd_port_amc_J5_X1 / X2 / X3 = 0 / 1 / 2
        // and we want them to be mapped onto embot::hw::ENCODER::one / two / three = 0 / 1 / 2 
        // so .... if we have an amc and port is 0, 1, or 2 we just do a static_cast<>
        // for other boards to come (amcfoc, etc) the port will have the same values 0, 1, 2 so that 
        // it is easy to convert w/ a static_cast<>
        return static_cast<embot::hw::ENCODER>(port);        
    }
    
};


bool embot::app::eth::theEncoderReader::Impl::initialise()
{
    if(true == _initted)
    {
        return true;
    }
    
    
    embot::core::print("embot::app::eth::theEncoderReader::Impl::initialise()");
     

    service.servconfig.type = eomn_serv_NONE;
    service.initted = eobool_true;
    service.active = eobool_false;
    service.started = eobool_false;    
    service.state = eomn_serv_state_idle;  
    
    diagnostics.errorType = eo_errortype_error;
    diagnostics.errorDescriptor.sourceaddress = eo_errman_sourcedevice_localboard;
    diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_encoders_failed_verify);   

#if defined(STM32HAL_BOARD_AMC) && defined(DEBUG_AEA3_stream_over_theBackdoor)    
    packet = eo_packet_New(512);    
    eo_packet_Addressing_Set(packet, hostip, hostport);  
#endif
    
    _initted = true;
    return true;
}


//some info onn how to fil in the par16 and par64 for theEncoderReader
// in par16 = [N3|N2|N1|N0] I put: number of joint in N3, 0 in N2, failure mask of secondary in N1, failure mask of primary in N0
// in par64 = [N15|N14|N13|N12|N11|N10|N09|N08|N07|N06|N05|N04|N03|N02|N01|N00] i put:
// in nibbles N00-N03 i put error codes of primary
// in nibbles N04-N07 i put error codes of secondary



bool embot::app::eth::theEncoderReader::Impl::Verify(const Config &config, bool ActivateafterverifY, const embot::core::Confirmer &oncompletion)
{    
    service.state = eomn_serv_state_verifying;

        
    activateafterverify = ActivateafterverifY;
    onverifycompleted = oncompletion; 

    
    constexpr bool verificationFailed {false};
    
    diagnostics.errorDescriptor.sourcedevice    = eo_errman_sourcedevice_localboard;
    diagnostics.errorDescriptor.par16           = diagnostics.errorDescriptor.par64 = 0;
    diagnostics.errorDescriptor.sourceaddress   = 0;
    
    if( false == IsEncoderSupported(config) || false ==  TestRead(config))
    {
        
        diagnostics.errorType = eo_errortype_error;
        diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_encoders_failed_verify);  
        eo_errman_Error(eo_errman_GetHandle(), diagnostics.errorType, nullptr, s_eobj_ownname, &diagnostics.errorDescriptor);
        
        Deactivate();     
         _verified = false;
        onverifycompleted.execute(verificationFailed);  

        return false;  
    }
   
    
    bool _verified = true;
    constexpr bool verificationisOK {true};
 
    if(true == activateafterverify)
    {
        Activate(config);      
        _actived = true;        
    } 
    
    diagnostics.errorType = eo_errortype_debug;
    diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_encoders_ok);
    eo_errman_Error(eo_errman_GetHandle(), diagnostics.errorType, nullptr, s_eobj_ownname, &diagnostics.errorDescriptor);
    
    onverifycompleted.execute(verificationisOK);  
    
    return true;  
}

//check if the encoder(s) selected is supported by the board
bool embot::app::eth::theEncoderReader::Impl::IsEncoderSupported(const Config &config)
{
    
    bool ret {true};
    
    if((nullptr == config.carrayofjomodes))
    {
        ret = false;
        return ret;
    } 

    if(eo_constarray_Size(config.carrayofjomodes) > max_number_of_jomos)
    {
        ret = false;
        return ret;
    }
    
    //in doubt I deinit the embot::hw::encoders
    Deactivate();
    
    // 1. prepare the config
    EOconstarray* carray = eo_constarray_Load(reinterpret_cast<EOconstarray*>(config.carrayofjomodes));
    _implconfig.numofjomos = eo_constarray_Size(carray);
    for(uint8_t i=0; i<_implconfig.numofjomos; i++)
    {
       const eOmc_jomo_descriptor_t *jomodes = (eOmc_jomo_descriptor_t*) eo_constarray_At(carray, i);
        
       if(nullptr == jomodes) { continue; }

        _implconfig.jomo_cfg[i].encoder1des = jomodes->encoder1;
        _implconfig.jomo_cfg[i].encoder2des = jomodes->encoder2;
        
        embot::hw::encoder::Config cfg {};
        
        switch(_implconfig.jomo_cfg[i].encoder1des.type)
        {
            case eomc_enc_aea:
            {
                cfg.type = embot::hw::encoder::Type::chipAS5045;
            } break;
            
            case eomc_enc_aea3:
            {
                cfg.type = embot::hw::encoder::Type::chipMA730;
            } break;
            
            case eomc_enc_aksim2:
            {
                cfg.type = embot::hw::encoder::Type::chipMB049;
            } break;
            case eomc_enc_none:
            {
                continue;
            }break;
            case eomc_enc_unknown:
            default:
            {
                //unknown/no/unsupported encoder, pass to embot::hw::encoder::init cfg.type = embot::hw::encoder::Type::none, it will rise an error
//                embot::core::print("theEncoderReader: encoder unknown/none/unsupported");
                ret = false;
            } break;
        }
        
        // 2. configure and initialize SPI encoder
        embot::hw::ENCODER enc = port2encoder(_implconfig.jomo_cfg[i].encoder1des.port);
        embot::hw::result_t r = embot::hw::encoder::init(enc, cfg);
        //I write how main motor joints there are
        diagnostics.errorDescriptor.par16 = diagnostics.errorDescriptor.par16 |((_implconfig.numofjomos)<<12);
        
        if(embot::hw::resOK != r)
        {                            
//            embot::core::print("theEncoderReader: encoder type not supported");
            //in par16 we put the encoder with problems, in par64 we tell the specific error, in this case is a generic error
            diagnostics.errorDescriptor.par16 = diagnostics.errorDescriptor.par16 | (1<<i);
            diagnostics.errorDescriptor.par64 = diagnostics.errorDescriptor.par64 | (embot::core::tointegral(embot::app::eth::encoder::v1::Error::GENERIC)<<(i*4));
            
            ret = false;
        }
    }

    return ret;
}



//I'm not giving error messages here bc I assume no errors will be encountered as the checks are done before
bool embot::app::eth::theEncoderReader::Impl::Activate(const Config &config)
{

    bool ret {true};
    
    if((nullptr == config.carrayofjomodes))
    {
        ret = false;
        return ret;
    } 

    if(eo_constarray_Size(config.carrayofjomodes) > max_number_of_jomos)
    {
        ret = false;
        return ret;
    }
    
    if(true == _actived)
    {
        Deactivate();
    }

    EOconstarray* carray = eo_constarray_Load(reinterpret_cast<EOconstarray*>(config.carrayofjomodes));
    // 1. prepare the config
    _implconfig.numofjomos = eo_constarray_Size(carray);
    for(uint8_t i=0; i<_implconfig.numofjomos; i++)
    {
        const eOmc_jomo_descriptor_t *jomodes = (eOmc_jomo_descriptor_t*) eo_constarray_At(carray, i);
        
        if(nullptr == jomodes) { continue; }

        _implconfig.jomo_cfg[i].encoder1des = jomodes->encoder1;
        _implconfig.jomo_cfg[i].encoder2des = jomodes->encoder2;
        
        embot::hw::encoder::Config cfg {};
        
        switch(_implconfig.jomo_cfg[i].encoder1des.type)
        {
            case eomc_enc_aea:
            {
                cfg.type = embot::hw::encoder::Type::chipAS5045;
            } break;
            
            case eomc_enc_aea3:
            {
                cfg.type = embot::hw::encoder::Type::chipMA730;
            } break;
            
            case eomc_enc_aksim2:
            {
                cfg.type = embot::hw::encoder::Type::chipMB049;
            } break;
            case eomc_enc_unknown:
            case eomc_enc_none:
            default:
            {
                //unknown/no/unsupported encoder
                ret = false;
            } break;
        }
        
        // 2. configure and initialize SPI encoder
        embot::hw::ENCODER enc = port2encoder(_implconfig.jomo_cfg[i].encoder1des.port);
        embot::hw::result_t r = embot::hw::encoder::init(enc, cfg);
        if(embot::hw::resOK != r)
        {          
            ret = false;
        }

    }
    
    if(false == ret)
    {
        Deactivate();      
    }
        
    _actived = ret;
      
    return _actived;
}

bool embot::app::eth::theEncoderReader::Impl::Deactivate()
{
    _implconfig.numofjomos = 0;
    embot::hw::encoder::deinit(embot::hw::ENCODER::one);
    embot::hw::encoder::deinit(embot::hw::ENCODER::two);
    embot::hw::encoder::deinit(embot::hw::ENCODER::three);
    _actived = false;
    
    return _actived;
}

bool embot::app::eth::theEncoderReader::Impl::StartReading()
{
    //eo_errman_Trace(eo_errman_GetHandle(), "::Start()", s_eobj_ownname);
    
    // start the encoder reading
    // marco.accame on 29 oct 2024: maybe we can read just the configured ones. 
    embot::hw::encoder::startRead(embot::hw::ENCODER::one);
    embot::hw::encoder::startRead(embot::hw::ENCODER::two);
    embot::hw::encoder::startRead(embot::hw::ENCODER::three);
    
    return true;
}

bool embot::app::eth::theEncoderReader::Impl::Read(uint8_t jomo, embot::app::eth::encoder::v1::valueInfo &primary, embot::app::eth::encoder::v1::valueInfo &secondary)
{
    
    if(false == _actived)
    {   // nothing to do because we dont have it
        return true;
    }

    if(jomo >= _implconfig.numofjomos)
    {
        return false;
    }
    
    constexpr size_t numOfEncodersPerJoint {2};
    
    eOencoderProperties_t encProp[numOfEncodersPerJoint] = { 
        {&_implconfig.jomo_cfg[jomo].encoder1des, &primary}, 
        {&_implconfig.jomo_cfg[jomo].encoder2des, &secondary }
    };
        
    for(uint8_t i=0; i<numOfEncodersPerJoint; i++)
    {   // for each of the two encoders ....
        
        eOencoderProperties_t prop = encProp[i]; 
        uint16_t errorparam = 0;
        
        // assign composedof and position
        prop.valueinfo->composedof = eomc_encoder_get_numberofcomponents((eOmc_encoder_t)prop.descriptor->type); 
        prop.valueinfo->placement = static_cast<embot::app::eth::encoder::v1::Placement>(prop.descriptor->pos);
        
        // so far we assume no errors and we assign 0 to all values
        prop.valueinfo->errortype = embot::app::eth::encoder::v1::Error::NONE;
        memset(prop.valueinfo->value, 0, sizeof(prop.valueinfo->value)); // was not 0 but: hal_NA32;
        
        // now we compute the value(s) and adjust if we detect any errors
        
        if(false == s_eo_isconnected(prop.descriptor))
        {   // the encoder is not connected: adjust its error type and continue with other encoders in the loop
            prop.valueinfo->errortype = embot::app::eth::encoder::v1::Error::NOTCONNECTED;
            continue;   // KEEP this continue! IT'S IMPORTANT!
        } 
        
        // ok, we have a connected encoder. we see what type it is and we perform the proper acquisition
        // we also retrieve its diagnostic
        //hal_spiencoder_diagnostic_t diagn = {0};
        //diagn.type = hal_spiencoder_diagnostic_type_none;
        embot::hw::result_t rr {resNOK}; 
        embot::hw::encoder::POS spiRawValue = 0; 
        
        switch(prop.descriptor->type)
        {
            // they are: eomc_enc_aea3, eomc_enc_aea, eomc_enc_amo, eomc_enc_spichainof2,
            //           eomc_enc_spichainof3, eomc_enc_qenc, eomc_enc_absanalog, eomc_enc_mais            
            // however, in here we support (so far) only: aea and aea3
                       
            case eomc_enc_aea:
            {                               
                embot::hw::ENCODER e = port2encoder(prop.descriptor->port);
                
                if(resOK == (rr = embot::hw::encoder::getValue(e, spiRawValue/*, &diagn*/)))                
                {   // ok, the hal reads correctly
                    if(true == isValidValue_AEA(spiRawValue, prop.valueinfo->errortype))
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
                    prop.valueinfo->errortype = embot::app::eth::encoder::v1::Error::AEA_READING;
                    errorparam = 0xffff;                                         
                }   
                
                rawvalues[i][jomo].val = (int32_t)spiRawValue;
                //rawvalues[i][jomo].diagnInfo = (resOK == rr) ? 0 : 1;
               
            } break;
            
            
            case eomc_enc_aea3:
            {                         
                embot::hw::ENCODER e = port2encoder(prop.descriptor->port);  
                
                if(resOK == (rr = embot::hw::encoder::getValue(e, spiRawValue/*, &diagn*/)))                
                {   // ok, the hal reads correctly
                    if(true == isValidValue_AEA3(spiRawValue, prop.valueinfo->errortype))
                    {   // the spi raw reading from hal is valid. i just need to rescale it.
                        // the resolution is 16384 ticks per revolution.
                        
                        prop.valueinfo->value[0] = rescale2icubdegrees(spiRawValue, jomo, (eOmc_position_t)prop.descriptor->pos);                           
                    }
                    else
                    {   // we have a valid raw value from hal but ... it is not valid after a check                        
                        prop.valueinfo->errortype = prop.valueinfo->errortype;
                        errorparam = spiRawValue; //(spiRawValue >> 6) & 0x0FFF;                                           
                    }                    
                }
                else
                {   // we dont even have a valid reading from hal
                    prop.valueinfo->errortype = embot::app::eth::encoder::v1::Error::AEA_READING;
                    errorparam = 0xffff;                                         
                } 

                rawvalues[i][jomo].val = spiRawValue;                
               
            } break;
            
            default:
            {   // we have not recognised any valid encoder type
                prop.valueinfo->errortype = embot::app::eth::encoder::v1::Error::GENERIC;   
                errorparam = 0; 
                
                rawvalues[i][jomo].val = 0;
                //rawvalues[i][jomo].error = embot::app::eth::encoder::experimental::Error::SOME;
                
            } break;
        }
    }
    
    //eOresult_t res = eo_appEncReader_GetValue(s_eo_theencoderreader.reader, position, primary, secondary); 
    
    return true;
}



// I'll do one reading for each one of the encoder
bool embot::app::eth::theEncoderReader::Impl::TestRead(const Config &config)
{
    
    bool ret {true};
    
    EOconstarray* carray = eo_constarray_Load(reinterpret_cast<EOconstarray*>(config.carrayofjomodes));
    _implconfig.numofjomos = eo_constarray_Size(carray);
    
    //I will try to read the primary encoder, if the reading is succesfful (aka embot::hw::encoder::read reads plausible data) ok, if not I will send an error to yarp
    for(uint8_t i=0; i<_implconfig.numofjomos; i++)
    {
       const eOmc_jomo_descriptor_t *jomodes = (eOmc_jomo_descriptor_t*) eo_constarray_At(carray, i);
        
       if(nullptr == jomodes) { continue; }

        _implconfig.jomo_cfg[i].encoder1des = jomodes->encoder1;        
        embot::hw::ENCODER enc = port2encoder(_implconfig.jomo_cfg[i].encoder1des.port);
        embot::hw::encoder::POS pos;
       
        if(resNOK == embot::hw::encoder::read(enc, pos, 5*embot::core::time1millisec))     
        {                        
            diagnostics.errorDescriptor.par16 = diagnostics.errorDescriptor.par16 | (1<<i);
            switch(_implconfig.jomo_cfg[i].encoder1des.type)
            {
                case eomc_enc_aea:
                case eomc_enc_aea3:
                {
                    diagnostics.errorDescriptor.par64 = diagnostics.errorDescriptor.par64 | (embot::core::tointegral(embot::app::eth::encoder::v1::Error::AEA_READING)<<(i*4));
                } break;
                case eomc_enc_none:
                {  
                    continue;
                }break;
                case eomc_enc_aksim2:
                case eomc_enc_unknown:
                default:
                {
                    diagnostics.errorDescriptor.par64 = diagnostics.errorDescriptor.par64 | (embot::core::tointegral(embot::app::eth::encoder::v1::Error::GENERIC)<<(i*4));
                } break;
            }
//            embot::core::print("theEncoderReader: encoder test reading fails");   
            ret = false;            
        }
    }

    return ret;
}


bool embot::app::eth::theEncoderReader::Impl::SendReport()
{
    return true;
}

bool embot::app::eth::theEncoderReader::Impl::IsReadingAvailable()
{
    return true;
}

embot::app::eth::encoder::v1::Type  embot::app::eth::theEncoderReader::Impl::GetType(const embot::app::eth::encoder::v1::Target &target)
{  
    return embot::app::eth::encoder::v1::Type::none;
}

bool embot::app::eth::theEncoderReader::Impl::Scale(const embot::app::eth::encoder::v1::Target &target, const embot::app::eth::encoder::v1::Scaler &scaler)
{
    return false;
}


void embot::app::eth::theEncoderReader::Impl::log()
{
#if defined(STM32HAL_BOARD_AMC) && defined(DEBUG_AEA3_stream_over_theBackdoor) 
    
    embot::core::Time n {embot::core::now()};
    std::string s {embot::core::TimeFormatter(n).to_string() + ":"};
    for(size_t jomo=0; jomo<3; jomo++)
    {
        s += " ";
        s += std::to_string(rawvalues[0][jomo].raw);           
    } 
    
    std::snprintf(sss, sizeof(sss), "%s\n", s.c_str());             
        
    eo_packet_Payload_Set(packet, reinterpret_cast<uint8_t*>(sss), std::strlen(sss));    
    embot::app::eth::theBackdoor::getInstance().transmit(packet);   
    
#endif    
}
bool embot::app::eth::theEncoderReader::Impl::GetRaw(uint8_t jomo, embot::app::eth::encoder::experimental::RawValuesOfJomo &rawValuesArray)
{
    
    if(jomo >= _implconfig.numofjomos)
    {
        return false;
    }

    // copy the raw encoder elements for the requested jomo (copy up to destination capacity)
    // rawvalues[jomo] is std::array<RawValueEncoder, Nsrc>, where Nsrc = 2 (line 98); rawValuesArray.rawvalues is a C-array with some capacity Ndst
    // compute the number of elements to copy and use std::copy_n
    size_t dst_size = sizeof(rawValuesArray.rawvalues) / sizeof(rawValuesArray.rawvalues[0]);
    size_t src_size = rawvalues[jomo].size();
   
    size_t n = (dst_size < src_size) ? dst_size : src_size;
    std::copy_n(rawvalues[jomo].begin(), n, rawValuesArray.rawvalues);

    return true;
    
}

bool embot::app::eth::theEncoderReader::Impl::GetRawSingle(uint8_t jomo, embot::app::eth::encoder::experimental::Position pos, embot::app::eth::encoder::experimental::RawValueEncoder &rawValue)
{
    if(jomo >= _implconfig.numofjomos)
    {
        return false;
    }
    switch (pos)
    {
        case embot::app::eth::encoder::experimental::Position::joint:
        {
            rawValue = rawvalues[0][jomo];
        } break;
        case embot::app::eth::encoder::experimental::Position::motor:
        {
            rawValue = rawvalues[1][jomo];
        } break;
        case embot::app::eth::encoder::experimental::Position::aux:
        {
            // aux encoder not supported
            return false;
        } break;
        default:
            break;
    
    }
    return true;
}

// private members

bool embot::app::eth::theEncoderReader::Impl::s_eo_isconnected(eOmc_encoder_descriptor_t *des)
{
    return ((eomc_enc_none != (des->type)) && (eobrd_port_nolocal != (des->port))) ? true : false;
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


bool embot::app::eth::theEncoderReader::Impl::isValidValue_AEA(const uint32_t &valueraw, embot::app::eth::encoder::v1::Error &error)
{
    error = embot::app::eth::encoder::v1::Error::NONE;
    
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
    
    return true;
}

bool embot::app::eth::theEncoderReader::Impl::isValidValue_AEA3(const uint32_t &valueraw, embot::app::eth::encoder::v1::Error &error)
{
    error = embot::app::eth::encoder::v1::Error::NONE;
    // TODO: there is no way to check the validity when using the AEA3 in SSI mode
    return true;
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

bool embot::app::eth::theEncoderReader::initialise()
{
    return pImpl->initialise();
}

bool embot::app::eth::theEncoderReader::Verify(const Config &config, bool activateafterverify, const embot::core::Confirmer &oncompletion)
{
    return pImpl->Verify(config, activateafterverify, oncompletion); 
}


bool embot::app::eth::theEncoderReader::Activate(const Config &config)
{
    return pImpl->Activate(config);
}

bool embot::app::eth::theEncoderReader::Deactivate()
{
    return pImpl->Deactivate();
}

bool embot::app::eth::theEncoderReader::StartReading()
{
    return pImpl->StartReading();
}

bool embot::app::eth::theEncoderReader::Read(uint8_t jomo, embot::app::eth::encoder::v1::valueInfo &primary, embot::app::eth::encoder::v1::valueInfo &secondary)
{
    return pImpl->Read(jomo, primary, secondary);
}

bool embot::app::eth::theEncoderReader::SendReport()
{
    return pImpl->SendReport();
}

bool embot::app::eth::theEncoderReader::IsReadingAvailable()
{
    return pImpl->IsReadingAvailable();
}

bool embot::app::eth::theEncoderReader::Diagnostics_Tick()
{
    return pImpl->Diagnostics_Tick();
}

embot::app::eth::encoder::v1::Type  embot::app::eth::theEncoderReader::GetType(const embot::app::eth::encoder::v1::Target &target)
{
    return pImpl->GetType(target);
}

bool embot::app::eth::theEncoderReader::Scale(const embot::app::eth::encoder::v1::Target &target, const embot::app::eth::encoder::v1::Scaler &scaler)
{
    return pImpl->Scale(target, scaler);
}

void embot::app::eth::theEncoderReader::log()
{
    pImpl->log();
}

bool embot::app::eth::theEncoderReader::GetRaw(uint8_t jomo, embot::app::eth::encoder::experimental::RawValuesOfJomo &rawValuesArray)
{
    return pImpl->GetRaw(jomo, rawValuesArray);
}

bool embot::app::eth::theEncoderReader::GetRawSingle(uint8_t jomo, embot::app::eth::encoder::experimental::Position pos, embot::app::eth::encoder::experimental::RawValueEncoder &rawValue)
{
    return pImpl->GetRawSingle(jomo, pos, rawValue);
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



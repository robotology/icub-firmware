
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theEncoderReader.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheEncoderReader.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



struct embot::app::eth::theEncoderReader::Impl
{
    
    Impl() = default;
    
    bool initialise();
    
    bool Verify(const Config &config, bool activateafterverify, const embot::core::Confirmer &oncompletion);                            
    bool Activate(const Config &config);    
    bool Deactivate();   
    bool StartReading();    
    bool Read(uint8_t position, embot::app::eth::encoder::v1::valueInfo &primary, embot::app::eth::encoder::v1::valueInfo &secondary);
    bool SendReport();                            
    bool IsReadingAvailable();
    bool Diagnostics_Tick();
    embot::app::eth::encoder::v1::Type GetType(const embot::app::eth::encoder::v1::Target &target);
    bool Scale(const embot::app::eth::encoder::v1::Target &target, const embot::app::eth::encoder::v1::Scaler &scaler);
    
    // advanced
    bool GetRaw(uint8_t jomo, embot::app::eth::encoder::experimental::RawValuesOfJomo &rawValuesArray);
    bool GetRawSingle(uint8_t jomo, embot::app::eth::encoder::experimental::Position pos, embot::app::eth::encoder::experimental::RawValueEncoder &rawValue);
    
};


bool embot::app::eth::theEncoderReader::Impl::initialise()
{
    eo_encoderreader_Initialise();
    
    return true;
}

bool embot::app::eth::theEncoderReader::Impl::Verify(const Config &config, bool activateafterverify, const embot::core::Confirmer &oncompletion)
{ 
    // safe reinterpret_cast<eOservice_onendofoperation_fun_t> because ... 
    return eores_OK == eo_encoderreader_Verify(eo_encoderreader_GetHandle(), config.carrayofjomodes, reinterpret_cast<eOservice_onendofoperation_fun_t>(oncompletion.call), static_cast<eObool_t>(activateafterverify), config.dc);
}


bool embot::app::eth::theEncoderReader::Impl::Activate(const Config &config)
{
    return eores_OK == eo_encoderreader_Activate(eo_encoderreader_GetHandle(), config.carrayofjomodes, config.dc);
}

bool embot::app::eth::theEncoderReader::Impl::Deactivate()
{
    return eores_OK == eo_encoderreader_Deactivate(eo_encoderreader_GetHandle());
}

bool embot::app::eth::theEncoderReader::Impl::StartReading()
{
    return eores_OK == eo_encoderreader_StartReading(eo_encoderreader_GetHandle());
}

bool embot::app::eth::theEncoderReader::Impl::Read(uint8_t position, embot::app::eth::encoder::v1::valueInfo &primary, embot::app::eth::encoder::v1::valueInfo &secondary)
{
    eOencoderreader_valueInfo_t *e1 = reinterpret_cast<eOencoderreader_valueInfo_t*>(&primary);
    eOencoderreader_valueInfo_t *e2 = reinterpret_cast<eOencoderreader_valueInfo_t*>(&secondary);
    return eores_OK == eo_encoderreader_Read(eo_encoderreader_GetHandle(), position, e1, e2);
}

bool embot::app::eth::theEncoderReader::Impl::SendReport()
{
    return eores_OK == eo_encoderreader_SendReport(eo_encoderreader_GetHandle());
}

bool embot::app::eth::theEncoderReader::Impl::Diagnostics_Tick()
{    
    return eores_OK == eo_encoderreader_Diagnostics_Tick(eo_encoderreader_GetHandle());
}

bool embot::app::eth::theEncoderReader::Impl::IsReadingAvailable()
{    
    return eobool_true == eo_encoderreader_IsReadingAvailable(eo_encoderreader_GetHandle());
}

embot::app::eth::encoder::v1::Type  embot::app::eth::theEncoderReader::Impl::GetType(const embot::app::eth::encoder::v1::Target &target)
{
    eOmc_encoder_t t = eo_encoderreader_GetType(eo_encoderreader_GetHandle(), target.jomo, static_cast<eOencoderreader_Position_t>(embot::core::tointegral(target.pos)));    
    return static_cast<embot::app::eth::encoder::v1::Type>(t);
}

bool embot::app::eth::theEncoderReader::Impl::Scale(const embot::app::eth::encoder::v1::Target &target, const embot::app::eth::encoder::v1::Scaler &scaler)
{
    // for now only mais and absanalog accept to be scaled ... but in here i dont verify
    eOencoderreader_Scaler sca {scaler.scale, scaler.offset};
    return eores_OK == eo_encoderreader_Scale(eo_encoderreader_GetHandle(), target.jomo, static_cast<eOencoderreader_Position_t>(embot::core::tointegral(target.pos)), &sca);
}

bool embot::app::eth::theEncoderReader::Impl::GetRaw(uint8_t jomo, embot::app::eth::encoder::experimental::RawValuesOfJomo &rawValuesArray)
{
    eOencoderreader_RawValuesOfJomo_t *rvja = reinterpret_cast<eOencoderreader_RawValuesOfJomo_t*>(&rawValuesArray);
    
    return eores_OK == eo_encoderreader_GetRaw(eo_encoderreader_GetHandle(), jomo, rvja);
}

bool embot::app::eth::theEncoderReader::Impl::GetRawSingle(uint8_t jomo, embot::app::eth::encoder::experimental::Position pos, embot::app::eth::encoder::experimental::RawValueEncoder &rawValue)
{
    return eores_OK;
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

bool embot::app::eth::theEncoderReader::Read(uint8_t position, embot::app::eth::encoder::v1::valueInfo &primary, embot::app::eth::encoder::v1::valueInfo &secondary)
{
    return pImpl->Read(position, primary, secondary);
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

bool embot::app::eth::theEncoderReader::GetRaw(uint8_t jomo, embot::app::eth::encoder::experimental::RawValuesOfJomo &rawValuesArray)
{
    return pImpl->GetRaw(jomo, rawValuesArray);
}

bool embot::app::eth::theEncoderReader::GetRawSingle(uint8_t jomo, embot::app::eth::encoder::experimental::Position pos, embot::app::eth::encoder::experimental::RawValueEncoder &rawValue)
{
    return pImpl->GetRawSingle(jomo, pos, rawValue);
}
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



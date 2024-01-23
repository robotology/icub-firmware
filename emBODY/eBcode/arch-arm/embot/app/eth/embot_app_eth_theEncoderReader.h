

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Simone Girardi
 * email:   simone.girardi@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEENCODERREADER_H_
#define __EMBOT_APP_ETH_THEENCODERREADER_H_

#include "embot_core.h"
#include "embot_app_eth.h"
#include "embot_app_eth_Encoder.h"


// embobj dependencies
#include "EoCommon.h"
#include "EOconstarray.h"
#include "EoManagement.h"



namespace embot { namespace app { namespace eth {
      
#if 0
    The singleton `theEncoderReader` is responsible to ... 
    
#endif
    
        
    /**
     * @brief theEncoderReader class used to read the encoder data from the SPI.
     * 
     */
    class theEncoderReader : public embot::app::eth::encoder::v1::IFreader, embot::app::eth::encoder::experimental::IFreader
    {
    public:
        static theEncoderReader& getInstance();
                

        bool initialise();
       
        // v1::IFreader
        bool Verify(const Config &config, bool activateafterverify, const embot::core::Confirmer &oncompletion) override;         
        bool Activate(const Config &config) override;
        bool Deactivate() override;
        bool SendReport() override;         
        bool StartReading() override;                          
        bool IsReadingAvailable() override;       
        bool Read(uint8_t jomo, embot::app::eth::encoder::v1::valueInfo &primary, embot::app::eth::encoder::v1::valueInfo &secondary) override;
        bool Diagnostics_Tick() override;
        embot::app::eth::encoder::v1::Type GetType(const embot::app::eth::encoder::v1::Target &target) override;
        bool Scale(const embot::app::eth::encoder::v1::Target &target, const embot::app::eth::encoder::v1::Scaler &scaler) override;
    
        // experimental::IFreader
        bool read(const embot::app::eth::encoder::experimental::Target &target, embot::app::eth::encoder::experimental::Value &value) override;
    
        bool raw(uint8_t jomo, embot::app::eth::encoder::v1::Position pos, embot::app::eth::encoder::experimental::Value &value);
    
        void log();
        
    private:
        theEncoderReader(); 
        ~theEncoderReader(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    }; 


}}} // namespace embot { namespace app { namespace eth


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

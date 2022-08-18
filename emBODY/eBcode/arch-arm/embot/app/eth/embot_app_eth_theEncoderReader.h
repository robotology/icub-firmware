

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Simone Girardi
 * email:   simone.girardi@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEENCODERREADER_H_
#define __EMBOT_APP_ETH_THEENCODERREADER_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_core_binary.h"
#include "embot_app_eth.h"
#include "embot_os.h"
#include "embot_app_eth_Service.h"

#include <array>

// embobj dependencies
#include "EoCommon.h"
#include "EOconstarray.h"
#include "EoManagement.h"

typedef enum
{
    encreader_err_NONE                  = 0,
    encreader_err_AEA_READING           = 1,
    encreader_err_AEA_PARITY            = 2,
    encreader_err_AEA_CHIP              = 3,
    encreader_err_QENC_GENERIC          = 4,
    encreader_err_ABSANALOG_GENERIC     = 5,
    encreader_err_MAIS_GENERIC          = 6,
    encreader_err_SPICHAINOF2_GENERIC   = 7,
    encreader_err_SPICHAINOF3_GENERIC   = 8,
    encreader_err_AMO_GENERIC           = 9,
    encreader_err_PSC_GENERIC           = 10,  
    encreader_err_POS_GENERIC           = 11,    
    encreader_err_GENERIC               = 14,    
    encreader_err_NOTCONNECTED          = 15 /* this error happens when the encoder type is none or encoder is not local, for example it is connected to 2foc board */
} eOencoderreader_errortype_t;

typedef struct
{
    uint32_t                                value[eomc_encoders_maxnumberofcomponents];
    eOencoderreader_errortype_t             errortype;
    uint8_t                                 composedof;
    eOmc_position_t                         position;
} eOencoderreader_valueInfo_t;

namespace embot { namespace app { namespace eth {
      
#if 0
    The singleton `theEncoderReader` is responsible to start the reading from one or more SPI encoder(s).
    Once the data is available ...
    Finally the position data is converted into icub degrees. 
    
#endif
    
        
    /**
     * @brief theEncoderReader class used to read the encoder data from the SPI.
     * 
     */
    class theEncoderReader
    {
    public:
        /**
         * @brief Get the Instance object
         * 
         * @return theEncoderReader& 
         */
        static theEncoderReader& getInstance();
                
    public:
        
        static constexpr size_t max_number_of_jomos { 7 };                          // TODO: verify. Probably unnecessary and wrong.
        static constexpr size_t max_number_of_encoders { 2 * max_number_of_jomos }; // TODO: verify. Probably unnecessary and wrong.
        
        typedef struct
        {
            eOmc_encoder_descriptor_t   encoder1des;
            eOmc_encoder_descriptor_t   encoder2des;
        } jomoconfig_t;
        
        /**
         * @brief Config struct used to configure the encoder reader.
         * 
         */
        struct Config
        {
            uint8_t numofjomos {0};
            std::array<jomoconfig_t, max_number_of_jomos> jomo_cfg {};
            constexpr Config() = default;
//            constexpr Config(const embot::os::Thread::Props &t,
//                             const embot::app::eth::SocketDescriptor &s,
//                             const embot::app::eth::SocketAddress &h)
//                             : thread(t), socket(s), hostaddress(h) {}
        }; 
        
        /**
         * @brief Initialize the encoder reader.
         * 
         * @param config 
         * @return true if the encoder is initialized correctly, false otherwise.
         */
        eOresult_t initialise();
        
        eOresult_t Verify(EOconstarray *arrayofjomodes, 
                          eOservice_onendofoperation_fun_t onverify,
                          void *arg,        
                          bool activateafterverify);         
//        eOresult_t Activate(const eOmn_serv_configuration_t * servcfg);  
                          
        eOresult_t activate(EOconstarray *arrayofjomodes, eOmn_serv_diagn_cfg_t dc);
        
        eOresult_t deactivate();
         
        eOresult_t startReading();
                          
        bool isreadingavailable();
        
        eOresult_t read(uint8_t position, eOencoderreader_valueInfo_t *valueInfo1, eOencoderreader_valueInfo_t *valueInfo2);
        
        bool report();
        
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

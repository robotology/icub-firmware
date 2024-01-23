

/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_ENCODER_H_
#define __EMBOT_APP_ETH_ENCODER_H_


#if 0
 
### The encoder in embot environment

The management of the encoders in the embOBJ environment ....


#endif

#include "embot_core.h"
#include "embot_app_eth.h"

#include "EoCommon.h"
#include "EoManagement.h"
#include "EOconstarray.h"

// in here we place types required to manage the encoders in the embot environment.

#if 0
    In here we place types required to manage the encoders in the embot environment.
    We use namespaces to manage the evolution of the required functionalities.
    
    The `embot::app::eth::encoder::v1` namespace holds a mixture of:
    - embOBJ types, 
    - embot types derived by alias of embOBJ ones, and
    - brand new embot types.
    All its data structures are meant to mimic the behavior of ETH boards using sensors up to July 2023.
        
    The `embot::app::eth::encoder::experimental` namespace is just a tentative example of how to enhance the reading of encoders w/ higher
    resulution the likes of the AKSIM.

    Future `embot::app::eth::encoder::v2` etc namespaces will introduce the new features
        
    From top to down:
    - the reading service is offered by the PIMPL singleton `embot::app::eth::theEncoderReader` which uses 
      multiple inheritance from the pure interface classes `embot::app::eth::encoder::v1::IFreader` etc 
      to expose all the required features (higher encoder resolution, diagnostics, etc).
    - the internals of the `theEncoderReader` PIMPL singleton are left open to offer the most suitable implementation.
      So far (July 2023), we have two different implementations:
      - the one used by `ems`, `mc4plus`, `mc2plus` which relies upon `EOtheEncoderReader`, `EOappEncoderReaader` and `hal` of stm32f407;
      - the simplified one used by `amc` which directly uses `embot::hw::encoder` to manage just `AEA` encoders

    
#endif 

namespace embot::app::eth::encoder::v1 {
       
    // it tells to which port the encoder is physically attached on the PCB
    // for now it keep U8 values from eObrd_port_t, eObrd_portmais_t, eObrd_portpsc_t, eObrd_portpos_t, etc (used by icub-main)    
    using Port = uint8_t;                                               
        
    // it tells the type of the encoder. 
    // for now we use the same values of eOmc_encoder_t (used by icub-main) conveniently placed in here.
    enum class Type : uint8_t 
    {   // same as eOmc_encoder_t
        aea         = eomc_enc_aea,         //  1
        roie        = eomc_enc_roie,        //  2
        absanalog   = eomc_enc_absanalog,   //  3, // or HALL_ADC
        mais        = eomc_enc_mais,        //  4, // MAIS 
        qenc        = eomc_enc_qenc,        //  5, // or OPTICAL_QUAD
        hallmotor   = eomc_enc_hallmotor,   //  6, // HALL_MOTOR_SENS    
        spichainof2 = eomc_enc_spichainof2, //  7,  
        spichainof3 = eomc_enc_spichainof3, //  8,    
        amo         = eomc_enc_amo,         //  9, 
        psc         = eomc_enc_psc,         // 10,
        pos         = eomc_enc_pos,         // 11,
        aea3        = eomc_enc_aea3,        // 12,
        aksim2      = eomc_enc_aksim2,      // 13,
        mrie        = eomc_enc_mrie,        // 14,
    
        none        = eomc_enc_none,        //  0,
        any         = 254,
        unknown     = eomc_enc_unknown      //255                
    };
    constexpr size_t maxTYPEs {14};    
    static_assert(maxTYPEs == eomc_encoders_numberof, "v1::maxTYPEs) is not equal to eomc_encoders_numberof");
    
    
    // it tells the logical placement of the encoder, so if the encoder offers measures of the joint or of the motor
    // we keep values from eOmc_position_t (used by icub-main)
    enum class Placement : uint8_t { none = 0, atjoint = 1, atmotor = 2 };  
    constexpr size_t maxPLACEMENTs {3};    
    
    // it tells if the encoder is the first or the second of the joint-motor 
    // Position::one / ::two is associated to ENCODER1 / ENCODER2 of the xml file.
    // CAVEAT: Position  may be seen as a duplicate of Placement. So far, however we have kept the possibility
    // to have two encoder for the pair joint-motor, with any placement: even the two of them at the joint.
    enum class Position : uint8_t { one = 0, two = 1, every = 14, none = 15, primary = one, secondary = two};
    constexpr size_t maxPOSITIONs {2};  
  
    
    // it tells which are the possible errors in reading encoders. 
    // so far we just use what was inside the embOBJ encoder reader in enum eOencoderreader_errortype_t, which is now removed. 
    // CAVEAT: any error beyond 15 cannot be managed by diagnostics messages because it will not fit inside a nibble
    enum class Error : uint8_t
    {   // from the old eOencoderreader_errortype_t, now removed
        NONE                  = 0,
        AEA_READING           = 1,
        AEA_PARITY            = 2,
        AEA_CHIP              = 3,
        QENC_GENERIC          = 4,
        ABSANALOG_GENERIC     = 5,
        MAIS_GENERIC          = 6,
        SPICHAINOF2_GENERIC   = 7,
        SPICHAINOF3_GENERIC   = 8,
        AMO_GENERIC           = 9,
        PSC_GENERIC           = 10,
        POS_GENERIC           = 11,
        GENERIC               = 14,
        NOTCONNECTED          = 15, /* this error happens when the encoder type is none or encoder is not local, for example it is connected to 2foc board */
        
        AKSIM2_INVALID_DATA   = 16,
        AKSIM2_CLOSE_TO_LIMITS= 17,
        AKSIM2_CRC_ERROR      = 18,
        AKSIM2_GENERIC        = 19
    };   
    //constexpr size_t maxERRORs {20}; // caveat there is a hole and ...    
    
    // it contains the readings of one encoder in multiple components plus its placement and errors.
    // so far it uses the same complex structure and memory layout of eOencoderreader_valueInfo_t (now removed)
    struct valueInfo
    {   // its size is: 4*4+1+1+1 = 19. but it will use 20 bytes
        static constexpr size_t MAXcomponents {eomc_encoders_maxnumberofcomponents}; // eomc_encoders_maxnumberofcomponents is 4
        
        uint32_t value[MAXcomponents] {0};          // typically we use only values[0] unless we have composedof > 1
        Error errortype {Error::NONE};              // use values of v1::Error
        uint8_t composedof {0};                     // typically 1, unless we have eomc_enc_spichainof2 / eomc_enc_spichainof3 as in the hand of R1 
        Placement placement {Placement::none};      // use values from Placement. it tells if at joint or at motor
    };
    
    static_assert(sizeof(valueInfo) == 20, "sizeof(v1::valueInfo) is not 20");
    
    // it is used to offer gain and offset to some encoders such as absanalog and mais. 
    struct Scaler
    {   // out = input * 1/fabs(scale) - offset
        float scale {1.0f};
        int32_t offset {0};    
    };
    
    // it tells that we can manage up to a max number of joint-motor entities
    static constexpr size_t maxJOMOs {4};  
    
    // it defines the coordinates of the encoder with:
    // - jomo index in range [0, .. , maxJOMOs) 
    // - position 
    struct Target
    {   
        uint8_t jomo {0};
        Position pos {Position::one};
        
        constexpr Target() = default;
        constexpr Target(uint8_t j, Position p) : jomo(j), pos(p) {}
            
        constexpr bool isvalid() {  return (jomo < maxJOMOs) && (embot::core::tointegral(pos) < maxPOSITIONs); }        
    };
    
    
    // for use by the ems, mc4plus and mc2plus but also by the amc
    // it allows to read all the standard resolution encoders used up to july 2023 
    struct IFreader
    {
        struct Config
        {
            EOconstarray *carrayofjomodes {nullptr};
            eOmn_serv_diagn_cfg_t dc {};
            Config() = default;
            Config(EOconstarray *cjo, const eOmn_serv_diagn_cfg_t &d) : carrayofjomodes(cjo), dc(d) {}   
            constexpr bool isvalid() { return carrayofjomodes != nullptr; }
        };
        
        virtual bool Verify(const Config &config, bool activateafterverify, const embot::core::Confirmer &oncompletion) = 0; 
        virtual bool Activate(const Config &config) = 0;
        virtual bool Deactivate() = 0;
        virtual bool SendReport() = 0;
        virtual bool StartReading() = 0;
        virtual bool IsReadingAvailable();
        virtual bool Read(uint8_t jomo, v1::valueInfo &primary, v1::valueInfo &secondary) = 0;   
        virtual bool Diagnostics_Tick() = 0;
        virtual Type GetType(const v1::Target &target) = 0;
        virtual bool Scale(const v1::Target &target, const Scaler &scaler) = 0;
    protected:
        virtual ~IFreader() {};    // cannot delete from interface but only from derived object        
    };          
   
} // namespace embot::app::eth::encoder::v1 {
    


namespace embot::app::eth::encoder::experimental {
        
    // this is just an example of an experimental interface which can match the needs of the new sensors
    
    enum class Error : uint8_t
    {
        NONE        = 0,
        SOME        = 1,
        OTHER       = 2        
    };
    
    // maybe in the future we want more than two encoders per joint-motor entity
    enum class Position : uint8_t { one = 0, two = 1, three = 2, four = 3, every = 14, none = 15};
    constexpr size_t maxPOSITIONs {4};
    
    struct Value
    {   // the value w/ a possible error     
        int64_t raw {0};                                                    // to accomodate high resolution values
        embot::app::eth::encoder::experimental::Error error {Error::NONE};      // 
    };
    
    struct Target
    {   // it tells which joint-motor pair + plus the posotion of the encoder
        static constexpr size_t maxJOMOs {4};
        uint8_t jomo {0};
        Position pos {Position::one};
        
        constexpr Target() = default;
        constexpr Target(uint8_t j, Position p) : jomo(j), pos(p) {}
            
        constexpr bool isvalid() {  return (jomo < maxJOMOs) && (embot::core::tointegral(pos) < maxPOSITIONs); }        
    };

    struct IFreader
    {
        virtual bool read(const embot::app::eth::encoder::experimental::Target &target, embot::app::eth::encoder::experimental::Value &value) = 0;   
        
    protected:
        virtual ~IFreader() {};    // cannot delete from interface but only from derived object        
    };     
    
} // namespace embot::app::eth::encoder::experimental:: {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_ad7147.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_hw_bsp_config.h"
#include "embot_hw_ad7147_bsp.h"

#include <cstring>
#include <vector>
#include "embot_core_binary.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif

using namespace embot::hw;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if !defined(EMBOT_ENABLE_hw_ad7147)

namespace embot { namespace hw { namespace ad7147 {

    bool supported(AD7147 s)                                                                         { return false; }
    bool initialised(AD7147 s)                                                                       { return false; }
    result_t init(AD7147 s, const Config &config)                                                    { return resNOK; }    
    bool isalive(embot::hw::AD7147 s, embot::core::relTime timeout)                                  { return false; }    
    bool isacquiring(embot::hw::AD7147 s)                                                            { return false; }    
    bool canacquire(embot::hw::AD7147 s)                                                             { return false; }        
    result_t acquisition(embot::hw::AD7147 s, const embot::core::Callback &oncompletion)             { return resNOK; }
    bool operationdone(embot::hw::AD7147 s)                                                          { return false; }
    result_t read(embot::hw::AD7147 s, Values &values)                                                  { return resNOK; }   

}}} // namespace embot { namespace hw { namespace AD7147 {


#else

namespace embot { namespace hw { namespace ad7147 {
              
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    bool supported(AD7147 a)
    {
        return embot::hw::ad7147::getBSP().supported(a);
    }
    
    bool initialised(AD7147 a)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(a));
    }    
      

    struct Acquisition
    {
        static constexpr uint8_t sizeofdata = 24; // 12 samples of 2 bytes each
        volatile bool done {false};
        volatile bool ongoing {false};
        Values values {};
        std::uint8_t rxdata[sizeofdata] {0};
        embot::core::Callback userdefCBK {};  
        void clear() { done = false; ongoing = false; memset(rxdata, 0, sizeof(rxdata)); userdefCBK.clear(); }        
        Acquisition() = default;        
    };
    
    struct PrivateData
    {
        embot::hw::i2c::Descriptor i2cdes[embot::core::tointegral(AD7147::maxnumberof)];   
        Config config[embot::core::tointegral(AD7147::maxnumberof)];        
        Acquisition acquisition[embot::core::tointegral(AD7147::maxnumberof)];
        PrivateData() = default;
    };
    
       
    static PrivateData s_privatedata;
    
    static void sharedCBK(void *p)
    {
        Acquisition *acq = reinterpret_cast<Acquisition*>(p);        
                      
        for(int i=0; i<acq->values.size(); i++)
        {
            acq->values[i] = (static_cast<uint16_t>(acq->rxdata[2*i]) << 8) + static_cast<uint16_t>(acq->rxdata[2*i+1]);
        }
             
        acq->ongoing = false;
        acq->done = true;
               
        acq->userdefCBK.execute();
    }
    
    // ------------------------------------------------------------------------------------------------------------------
    // management of registers: start
    // this section manages the sending / reading of the registers of the chip.

    
    // the manipulation of the register value works in the following way:
    // a value of 0xMMLL maps to the register so that 0xMM goes bits [15, 8] and 0xLL into bits [7, 0]
    // as an example we write in here how it works for register STAGE0_AFE_OFFSET which has address 0x82
    // when we use afeoffset = 0x2001.
    // at first we use constructor regContent{0x082, afeoffset} which inits teh address to be 0x82 and then
    // maps afeoffset into regContent::valREG in swapped byte order. After that, a I2C transmission will map
    // afeoffset to the field of teh register in teh following way
    
    // afeoffset =  0b0010000000000001
    // (afeoffset & 0b1000000000000000) >> 15 -> bits[15]    -> POS_AFE_OFFSET_SWAP = 0b0
    // (afeoffset & 0b0100000000000000) >> 14 -> bits[14]    -> Unused
    // (afeoffset & 0b0011111100000000) >> 8  -> bits[13:8]  -> POS_AFE_OFFSET = 0b100000
    // (afeoffset & 0b0000000010000000) >> 7  -> bits[7]     -> NEG_AFE_OFFSET_SWAP = 0b0
    // (afeoffset & 0b0000000001000000) >> 6  -> bits[6]     -> Unused
    // (afeoffset & 0b0000000000111111) >> 0 ->  bits[5:0]   -> NEG_AFE_OFFSET = 0b000001
    
    struct regContent
    {    
        embot::hw::i2c::Reg reg {0, embot::hw::i2c::Reg::Size::sixteenbits};
        uint16_t valREG {0};
        
        constexpr regContent(uint16_t a, uint16_t v) : reg({a, embot::hw::i2c::Reg::Size::sixteenbits}), valREG((v>>8) | ((v&0x00ff)<<8)) {} 
        constexpr regContent() = default; 
        uint16_t getvalue() const
        {
            return (valREG>>8) | ((valREG&0x00ff)<<8);
        }  
        void setvalue(uint16_t v)
        {
            valREG = (v>>8) | ((v&0x00ff)<<8);
        }
        void* getcontent() const
        {
            return const_cast<uint16_t*>(&valREG);
        }     
    };

    // it sends a request via i2c to the chip about a single register
    // the request can be of type Request
    enum class Request 
    { 
        writeonly,          // it asks to write the value inside the register
        writepluscheck,     // it asks to write the value inside the register and then it reads it back to verify the writing
        checkonly           // it asks to read the value so that it can verify it is equal to the value
    };
    bool send(embot::hw::I2C bus, std::uint8_t i2caddress, const regContent &rc, Request rqst, embot::core::relTime timeout = 20*embot::core::time1millisec)
    {
        bool ret = true;
        embot::hw::i2c::Reg reg2use {rc.reg};
        static volatile int errors {0};
        
        constexpr size_t NumberOfAttempts = 3;
        // in case of failure to read the value we wrote or we expect we attempt at max 3 times. 
        for(size_t i=0; i<NumberOfAttempts; i++)
        {                    
            if((Request::writeonly == rqst) || (Request::writepluscheck == rqst))
            {
                embot::core::Data content {rc.getcontent(), 2};
                embot::hw::i2c::write(bus, i2caddress, reg2use, content, timeout); 
            } 

            if((Request::checkonly == rqst) || (Request::writepluscheck == rqst))
            {
                uint8_t val[2] {0};
                uint16_t readvalue {0};
                volatile uint16_t targetvalue = rc.getvalue();
                embot::core::Data content_read_back {val, 2};
                embot::hw::i2c::read(bus,i2caddress, reg2use, content_read_back, timeout);
                readvalue = val[1] + (static_cast<uint16_t>(val[0]) << 8);
                if(readvalue != targetvalue)
                {
                    ret = false;
                    errors ++;
                }
            }
            
            if(true == ret)
            {
                break;
            }
        }
        
        if(true != ret)
        {
            errors = errors;
        }

        return ret;    
    }
    
        
    // we have 12 stages. each stage is configured with 8 registers 
    constexpr size_t numberOFstages {numberofTaxels};    
    constexpr size_t numberOFregistersINstage {8};
    
    // it sends the values of the 8 registers required to configure a stage. they are contained inside an array of 8 regContent objects
    void sendstage(embot::hw::I2C bus, std::uint8_t i2caddress, const std::array<regContent, numberOFregistersINstage> &ar, Request rqst,  embot::core::relTime timeout = 20*embot::core::time1millisec)
    {
        for(int i=0; i<ar.size(); i++)
        {
            send(bus, i2caddress, ar[i], rqst, timeout);        
        }   
    }
    
    // in here are the 12 arrays, one for each stages. each array contains 8 registers. all of them are rom-mapped
    
    // that is so far the only configurable value. It is the same for every register STAGEx_AFE_OFFSET, with x = [0, 12]
    // the addresses are 0x082, 0x08a, .., 0x0Da
    // the position of these registers inside the arrays is alwways 2
    constexpr uint16_t afeoffset = 0x2000; // in mtb4 the default is 0x2200. in other places is used 0x2000
    constexpr uint8_t afeoffsetPosition = 2;
    
    // the values in [st0, st11] are taken as they are from the driver of the mtb3 / mtb4
    
    constexpr std::array<regContent, numberOFregistersINstage> st0 
    {
        regContent{0x080, 0xFFFE},
        regContent{0x081, 0x1FFF},
        regContent{0x082, afeoffset},
        regContent{0x083, 0x2626},
        regContent{0x084, 50},
        regContent{0x085, 50},
        regContent{0x086, 100},
        regContent{0x087, 100}      
    };

    constexpr std::array<regContent, numberOFregistersINstage> st1 
    {
        regContent{0x088, 0xFFFB},
        regContent{0x089, 0x1FFF},
        regContent{0x08a, afeoffset},
        regContent{0x08b, 0x2626},
        regContent{0x08c, 50},
        regContent{0x08d, 50},
        regContent{0x08e, 100},
        regContent{0x08f, 100}      
    };

    constexpr std::array<regContent, numberOFregistersINstage> st2 
    {
        regContent{0x090, 0xFFEF},
        regContent{0x091, 0x1FFF},
        regContent{0x092, afeoffset},
        regContent{0x093, 0x2626},
        regContent{0x094, 50},
        regContent{0x095, 50},
        regContent{0x096, 100},
        regContent{0x097, 100}       
    };  

    constexpr std::array<regContent, numberOFregistersINstage> st3 
    {
        regContent{0x098, 0xFFBF},
        regContent{0x099, 0x1FFF},
        regContent{0x09a, afeoffset},
        regContent{0x09b, 0x2626},
        regContent{0x09c, 50},
        regContent{0x09d, 50},
        regContent{0x09e, 100},
        regContent{0x09f, 100}      
    };
    
    constexpr std::array<regContent, numberOFregistersINstage> st4 
    {
        regContent{0x0A0, 0xFEFF},
        regContent{0x0A1, 0x1FFF},
        regContent{0x0A2, afeoffset},
        regContent{0x0A3, 0x2626},
        regContent{0x0A4, 50},
        regContent{0x0A5, 50},
        regContent{0x0A6, 100},
        regContent{0x0A7, 100}      
    };
    
    constexpr std::array<regContent, numberOFregistersINstage> st5 
    {
        regContent{0x0A8, 0xFBFF},
        regContent{0x0A9, 0x1FFF},
        regContent{0x0Aa, afeoffset},
        regContent{0x0Ab, 0x2626},
        regContent{0x0Ac, 50},
        regContent{0x0Ad, 50},
        regContent{0x0Ae, 100},
        regContent{0x0Af, 100}   
    };
    
    constexpr std::array<regContent, numberOFregistersINstage> st6 
    {
        regContent{0x0B0, 0xEFFF},
        regContent{0x0B1, 0x1FFF},
        regContent{0x0B2, afeoffset},
        regContent{0x0B3, 0x2626},
        regContent{0x0B4, 50},
        regContent{0x0B5, 50},
        regContent{0x0B6, 100},
        regContent{0x0B7, 100}      
    };
    
    constexpr std::array<regContent, numberOFregistersINstage> st7 
    {
        regContent{0x0B8, 0xFFFF},
        regContent{0x0B9, 0x1FFE},
        regContent{0x0Ba, afeoffset},
        regContent{0x0Bb, 0x2626},
        regContent{0x0Bc, 50},
        regContent{0x0Bd, 50},
        regContent{0x0Be, 100},
        regContent{0x0Bf, 100}   
    };    

    constexpr std::array<regContent, numberOFregistersINstage> st8 
    {   
        regContent{0x0C0, 0xFFFF},
        regContent{0x0C1, 0x1FFB},
        regContent{0x0C2, afeoffset},
        regContent{0x0C3, 0x2626},
        regContent{0x0C4, 50},
        regContent{0x0C5, 50},
        regContent{0x0C6, 100},
        regContent{0x0C7, 100}      
    };
    
    constexpr std::array<regContent, numberOFregistersINstage> st9 
    {
        regContent{0x0C8, 0xFFFF},
        regContent{0x0C9, 0x1FEF},
        regContent{0x0Ca, afeoffset},
        regContent{0x0Cb, 0x2626},
        regContent{0x0Cc, 50},
        regContent{0x0Cd, 50},
        regContent{0x0Ce, 100},
        regContent{0x0Cf, 100}   
    };  
    
    constexpr std::array<regContent, numberOFregistersINstage> st10 
    {   
        regContent{0x0D0, 0xFFFF},
        regContent{0x0D1, 0x1FBF},
        regContent{0x0D2, afeoffset},
        regContent{0x0D3, 0x2626},
        regContent{0x0D4, 50},
        regContent{0x0D5, 50},
        regContent{0x0D6, 100},
        regContent{0x0D7, 100}      
    };
    
    constexpr std::array<regContent, numberOFregistersINstage> st11 
    {
        regContent{0x0D8, 0xFFFF},
        regContent{0x0D9, 0x1EFF},
        regContent{0x0Da, afeoffset},
        regContent{0x0Db, 0x2626},
        regContent{0x0Dc, 50},
        regContent{0x0Dd, 50},
        regContent{0x0De, 100},
        regContent{0x0Df, 100}   
    };  

    // this rom-mapped array contains the default values of every set of 8 registers for each of the 12 stages
    constexpr std::array<const std::array<regContent, numberOFregistersINstage>*, numberOFstages> theregistersofthe12stages 
    {
        &st0, &st1, &st2, &st3, &st4, &st5, &st6, &st7, &st8, &st9, &st10, &st11
    };  


    // management of registers: end
    // ------------------------------------------------------------------------------------------------------------------    
              
    result_t init(AD7147 s, const Config &config)
    {
        if(false == supported(s))
        {
            return resNOK;
        }
        
        if(true == initialised(s))
        {
            return resOK;
        }
        
        // init peripheral
        embot::hw::ad7147::getBSP().init(s);
        
        std::uint8_t index = embot::core::tointegral(s);
               
        // init i2c ..
        embot::hw::i2c::init(embot::hw::ad7147::getBSP().getPROP(s)->i2cdes.bus, {});
        if(false == embot::hw::i2c::ping(embot::hw::ad7147::getBSP().getPROP(s)->i2cdes.bus,embot::hw::ad7147::getBSP().getPROP(s)->i2cdes.adr, 3*embot::core::time1millisec))
        {
            return resNOK;
        }
                            
        s_privatedata.i2cdes[index] = embot::hw::ad7147::getBSP().getPROP(s)->i2cdes;
        s_privatedata.config[index] = config;
        s_privatedata.acquisition[index].clear();
        
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(s));
        
        // and now ... configure        
        return configure(s, config);        
    }
    
    
    result_t configure(AD7147 s, const Config &config)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 
        
        std::uint8_t index = embot::core::tointegral(s);
                   
        // 1. configure the 12 stages
        
        std::array<regContent, numberOFregistersINstage> st {}; // temporary object
        
        for(size_t i=0; i<theregistersofthe12stages.size(); i++)
        {
            // prepare teh temporary object w/ the constant values 
            st = *theregistersofthe12stages[i];
            // modified with the content of the register for the cdc offset which is in config
            st[afeoffsetPosition].setvalue(config.STAGEx_AFE_OFFSET); 
            // and set the registers of stage i-th
            sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st, Request::writepluscheck);
        }

        // 2. configure the PWR_CONTROL
        
        constexpr uint16_t pw_control_val = 0x00B0; // = 0000000010110000b
        // 00 0 0 0 0 00 1011 00 00
        // 00 -> CDC_BIAS = normal operation
        //    0 -> unused
        //      0 -> EXT_SOURCE = enable excitation source to CINx pins
        //        0 -> INT_POL = active low
        //          0 -> SW_RESET -> DOES NOT reset all registers to default values
        //            00 -> DECIMATION -> decimate by 256
        //               1011 -> SEQUENCE_STAGE_NUM = Number of stages in sequence (N + 1) -> 12 conversion stages in sequence 
        //                    00 -> LP_CONV_DELAY = 200 ms (Low power mode conversion delay)
        //                       00 -> POWER_MODE = 00 = full power mode (normal operation, CDC conversions approximately every 36 ms)
        send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x000, pw_control_val}, Request::writepluscheck);
        // configure the AMB_COMP_CTRL0
        send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x002, 0x0000}, Request::writepluscheck);
        // configure the AMB_COMP_CTRL1
        send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x003, 0x0000}, Request::writepluscheck);
        // configure the AMB_COMP_CTRL2
        send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x004, 0x0000}, Request::writepluscheck);
        // configure STAGE_LOW_INT_ENABLE
        send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x005, 0x0000}, Request::writepluscheck);
        // configure STAGE_HIGH_INT_ENABLE 
        send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x006, 0x0000}, Request::writepluscheck);
        // configure STAGE_COMPLETE_INT_ENABLE 
        send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x007, 0x0400}, Request::writepluscheck);
        // the following three *_STATUS register are read only and should not be written !!!!!!!!!!!!!
        //    // configure STAGE_LOW_INT_STATUS 
        //    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x008, 0x0000}, Request::writepluscheck);
        //    // configure STAGE_HIGH_INT_STATUS 
        //    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x009, 0x0000}, Request::writepluscheck);
        //    // configure STAGE_COMPLETE_INT_STATUS 
        //    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x00a, 0x0fff}, Request::writepluscheck);
        
        
        // 3. finally: STAGE_CAL_EN
        
        // 0x0fff = 0000111111111111b
        //          00 -> AVG_LP_SKIP Low power mode skip control 00 = use all samples
        //            00 -> AVG_FP_SKIP Full power mode skip control 00 = skip 3 samples 
        //              1 -> STAGE11_CAL_EN STAGE11 calibration enable
        //               1 -> STAGE10_CAL_EN 
        //                 etc... calibration enabled on all stages.    
        send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x001, 0x0fff}, Request::writepluscheck);
    
        return resOK;
    }

    
    bool isacquiring(AD7147 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = embot::core::tointegral(s);        
        return s_privatedata.acquisition[index].ongoing;     
    }
    
    
    bool canacquire(AD7147 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = embot::core::tointegral(s);  
        
        if(true == s_privatedata.acquisition[index].ongoing)
        {
            return false;
        }      
        return !embot::hw::i2c::isbusy(s_privatedata.i2cdes[index].bus);         
    }    
    
    result_t acquisition(AD7147 s, const embot::core::Callback &oncompletion)
    {
        if(false == canacquire(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = embot::core::tointegral(s);  
        

        volatile result_t res = result_t::NOK;
 
               
        s_privatedata.acquisition[index].clear();
        s_privatedata.acquisition[index].ongoing = true;
        s_privatedata.acquisition[index].done = false;
        s_privatedata.acquisition[index].userdefCBK = oncompletion;
           
        // ok, now i trigger the reading of the value.
        embot::core::Callback cbk(sharedCBK, &s_privatedata.acquisition[index]);
        embot::core::Data data = embot::core::Data(&s_privatedata.acquisition[index].rxdata[0], sizeof(s_privatedata.acquisition[index].rxdata));
        
        constexpr embot::hw::i2c::Reg reg_CDC_RESULT_S0 {0x00B, embot::hw::i2c::Reg::Size::sixteenbits};
        return embot::hw::i2c::read(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, reg_CDC_RESULT_S0, data, cbk);                     
    }
    
    bool isalive(AD7147 s, embot::core::relTime timeout)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = embot::core::tointegral(s);
        return embot::hw::i2c::ping(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, timeout);  
    }

    
    bool operationdone(AD7147 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        return s_privatedata.acquisition[embot::core::tointegral(s)].done;        
    } 
    
    
    result_t read(AD7147 s, Values &values)
    {
        if(false == initialised(s))
        {
            return resNOK;
        } 

        if(false == operationdone(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = embot::core::tointegral(s);
        values = s_privatedata.acquisition[index].values;
        
        return resOK;        
    }
    
 
}}} // namespace embot { namespace hw { namespace ad7147 {


#undef ENABLE_OLD_DEBUG_CODE
#if defined(ENABLE_OLD_DEBUG_CODE)
// old code used inside init() for testing the chip
// i attach the old init function in here as it is.

namespace embot { namespace hw { namespace ad7147 {

//        embot::hw::i2c::Reg reg_DeviceID {0x017, embot::hw::i2c::Reg::Size::sixteenbits};
//        embot::hw::i2c::Reg reg_AMB_COMP_CTRL0 {0x002, embot::hw::i2c::Reg::Size::sixteenbits};
//        embot::hw::i2c::Reg reg_AMB_COMP_CTRL1 {0x003, embot::hw::i2c::Reg::Size::sixteenbits};
//        embot::hw::i2c::Reg reg_AMB_COMP_CTRL2 {0x004, embot::hw::i2c::Reg::Size::sixteenbits};
//        
//        embot::hw::i2c::Reg reg_STAGE0_CONNECTION {0x080, embot::hw::i2c::Reg::Size::sixteenbits};
//        embot::hw::i2c::Reg reg_STAGE1_CONNECTION {0x088, embot::hw::i2c::Reg::Size::sixteenbits};
//        embot::hw::i2c::Reg reg_STAGE2_CONNECTION {0x090, embot::hw::i2c::Reg::Size::sixteenbits};
//        embot::hw::i2c::Reg reg_STAGE3_CONNECTION {0x098, embot::hw::i2c::Reg::Size::sixteenbits};
//        embot::hw::i2c::Reg reg_STAGE4_CONNECTION {0x0A0, embot::hw::i2c::Reg::Size::sixteenbits};
//        embot::hw::i2c::Reg reg_STAGE5_CONNECTION {0x0A8, embot::hw::i2c::Reg::Size::sixteenbits};
//        embot::hw::i2c::Reg reg_STAGE6_CONNECTION {0x0B0, embot::hw::i2c::Reg::Size::sixteenbits};
//        embot::hw::i2c::Reg reg_STAGE7_CONNECTION {0x0B8, embot::hw::i2c::Reg::Size::sixteenbits};
//        embot::hw::i2c::Reg reg_STAGE8_CONNECTION {0x0C0, embot::hw::i2c::Reg::Size::sixteenbits};
//        embot::hw::i2c::Reg reg_STAGE9_CONNECTION {0x0C8, embot::hw::i2c::Reg::Size::sixteenbits};
//        embot::hw::i2c::Reg reg_STAGE10_CONNECTION {0x0D0, embot::hw::i2c::Reg::Size::sixteenbits};
//        embot::hw::i2c::Reg reg_STAGE11_CONNECTION {0x0D8, embot::hw::i2c::Reg::Size::sixteenbits};
//        embot::hw::i2c::Reg reg_PWR_CONTROL {0x000, embot::hw::i2c::Reg::Size::sixteenbits};
//        embot::hw::i2c::Reg reg_STAGE_CAL_EN {0x001, embot::hw::i2c::Reg::Size::sixteenbits};    


//    // its sends the values of many registers contained inside a vector
//    void send(embot::hw::I2C bus, std::uint8_t i2caddress, std::vector<regContent> &tx, Request mode,  embot::core::relTime timeout = 20*embot::core::time1millisec)
//    {
//        for(int i=0; i<tx.size(); i++)
//        {
//            send(bus, i2caddress, tx[i], mode, timeout);        
//        }       
//    }

    result_t init_old(AD7147 s, const Config &config)
    {
        if(false == supported(s))
        {
            return resNOK;
        }
        
        if(true == initialised(s))
        {
            return resOK;
        }
        
        // init peripheral
        embot::hw::ad7147::getBSP().init(s);
        
        std::uint8_t index = embot::core::tointegral(s);
               
        // init i2c ..
        embot::hw::i2c::init(embot::hw::ad7147::getBSP().getPROP(s)->i2cdes.bus, {});
        if(false == embot::hw::i2c::ping(embot::hw::ad7147::getBSP().getPROP(s)->i2cdes.bus,embot::hw::ad7147::getBSP().getPROP(s)->i2cdes.adr, 3*embot::core::time1millisec))
        {
            return resNOK;
        }
                            
        s_privatedata.i2cdes[index] = embot::hw::ad7147::getBSP().getPROP(s)->i2cdes;
        s_privatedata.config[index] = config;
        s_privatedata.acquisition[index].clear();
        
       
        

           

//        // check the device id
//        if(!send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x017, 0x1471}, Request::checkonly))
//        {
//            return resNOK;
//        }
    
//    // just for test  
//    // AMB_COMP_CTRL0
//    if(!send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x002, 0x0ff0}, Request::checkonly))
//    {
//        for(;;);
//    }
//    
//    // AMB_COMP_CTRL1
//    if(!send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x003, 0x0164}, Request::checkonly))
//    {
//        for(;;);
//    }
//    
//    // AMB_COMP_CTRL2
//    if(!send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x004, 0xffff}, Request::checkonly))
//    {
//        for(;;);
//    }
    
    // configure the 12 stages
    std::array<regContent, 8> st {}; // temporary object
    
    for(size_t i=0; i<theregistersofthe12stages.size(); i++)
    {
        st = *theregistersofthe12stages[i];
        st[2].setvalue(config.STAGEx_AFE_OFFSET);
        sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st, Request::writepluscheck);
    }
    
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st0, Request::writepluscheck);           
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st1, Request::writepluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st2, Request::writepluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st3, Request::writepluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st4, Request::writepluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st5, Request::writepluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st6, Request::writepluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st7, Request::writepluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st8, Request::writepluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st9, Request::writepluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st10, Request::writepluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st11, Request::writepluscheck);
    

    // configure the PWR_CONTROL
    constexpr uint16_t pw_control_val = 0x00B0; // = 0000000010110000b
    // 00 0 0 0 0 00 1011 00 00
    // 00 -> CDC_BIAS = normal operation
    //    0 -> unused
    //      0 -> EXT_SOURCE = enable excitation source to CINx pins
    //        0 -> INT_POL = active low
    //          0 -> SW_RESET -> DOES NOT reset all registers to default values
    //            00 -> DECIMATION -> decimate by 256
    //               1011 -> SEQUENCE_STAGE_NUM = Number of stages in sequence (N + 1) -> 12 conversion stages in sequence 
    //                    00 -> LP_CONV_DELAY = 200 ms (Low power mode conversion delay)
    //                       00 -> POWER_MODE = 00 = full power mode (normal operation, CDC conversions approximately every 36 ms)
    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x000, pw_control_val}, Request::writepluscheck);
    // configure the AMB_COMP_CTRL0
    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x002, 0x0000}, Request::writepluscheck);
    // configure the AMB_COMP_CTRL1
    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x003, 0x0000}, Request::writepluscheck);
    // configure the AMB_COMP_CTRL2
    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x004, 0x0000}, Request::writepluscheck);
    // configure STAGE_LOW_INT_ENABLE
    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x005, 0x0000}, Request::writepluscheck);
    // configure STAGE_HIGH_INT_ENABLE 
    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x006, 0x0000}, Request::writepluscheck);
    // configure STAGE_COMPLETE_INT_ENABLE 
    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x007, 0x0400}, Request::writepluscheck);
// the following three *_STATUS register are read only and should not be written !!!!!!!!!!!!!
//    // configure STAGE_LOW_INT_STATUS 
//    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x008, 0x0000}, Request::writepluscheck);
//    // configure STAGE_HIGH_INT_STATUS 
//    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x009, 0x0000}, Request::writepluscheck);
//    // configure STAGE_COMPLETE_INT_STATUS 
//    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x00a, 0x0fff}, Request::writepluscheck);
    
    // and now, finally: STAGE_CAL_EN
    // 0x0fff = 0000111111111111b
    //          00 -> AVG_LP_SKIP Low power mode skip control 00 = use all samples
    //            00 -> AVG_FP_SKIP Full power mode skip control 00 = skip 3 samples 
    //              1 -> STAGE11_CAL_EN STAGE11 calibration enable
    //               1 -> STAGE10_CAL_EN 
    //                 etc... calibration enabled on all stages.    
    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x001, 0x0fff}, Request::writepluscheck);
    
       
#if 0
    // just a reding text. to make it work we need some extra code
    // now i read values
    
        volatile uint32_t count = 0;
    
        
    
        volatile bool readit = false;

        embot::hw::i2c::Reg reg_CDC_RESULT_S0 {0x00B, embot::hw::i2c::Reg::Size::sixteenbits};
        
        
        embot::core::Time s0 {0};
        volatile embot::core::Time delta {0};
        
        embot::core::Data dest = {readings, 24};
        
        std::array<uint16_t, 12> values = {0};
        
        for(;;)
        {
            done = false;
            count = 0;
            s0 = embot::core::now();
            std::memset(readings, 0xff, sizeof(readings));
            embot::hw::i2c::read(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, reg_CDC_RESULT_S0, dest, embot::core::Callback{finished, nullptr});
            while(!done)
            {
                count++;
            }
            delta = embot::core::now() - s0;
            
            for(int i=0; i<values.size(); i++)
            {
                values[i] = (static_cast<uint16_t>(readings[2*i]) << 8) + static_cast<uint16_t>(readings[2*i+1]);
            }
            count++;
        }

#endif


#if defined(TEST_RW)        
        // ora leggo il Device ID Register in address 0x017
        
        // devo usare registri a due bytes.
        embot::hw::i2c::Reg reg_DeviceID {0x017, embot::hw::i2c::Reg::Size::sixteenbits};
        embot::hw::i2c::Reg reg_AMB_COMP_CTRL0 {0x002, embot::hw::i2c::Reg::Size::sixteenbits};
        embot::hw::i2c::Reg reg_AMB_COMP_CTRL1 {0x003, embot::hw::i2c::Reg::Size::sixteenbits};
        embot::hw::i2c::Reg reg_AMB_COMP_CTRL2 {0x004, embot::hw::i2c::Reg::Size::sixteenbits};
        
        uint8_t data[2] {0};
        embot::core::Data dest = {data, 2};
        
        // read Device ID Register 0x017
        embot::hw::i2c::read(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, reg_DeviceID, dest, 10*1000);
        //embot::hw::i2c::read(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, reg_DeviceID, dest, embot::core::Callback{});
        //embot::core::delay(10*1000);
        
        // ok but in big endian. data[0] contains the msb and data[1] contains the lsb.
        // data[0] = 0x14 dat[1] = 0x71 and the correct value is [15:4] [3:0] -> 0x 147 1
        datav = data[1] + (static_cast<uint16_t>(data[0]) << 8);
        datav = datav;
        
        // read default value of AMB_COMP_CTRL0 Register 0x002
        embot::hw::i2c::read(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, reg_AMB_COMP_CTRL0, dest, 10*1000);
        datav = data[1] + (static_cast<uint16_t>(data[0]) << 8);
        datav = datav;
        // i read data[0] = 0x0f data[1] = 0xf0
        // default values must be from 15->00: 0x 0 f f 0
        
        // write AMB_COMP_CTRL0 with 
        uint8_t txdata[2] = {0};
        txdata[0] = 0x3e;   // nibble 3 is: bits 13:12 PWR_DOWN_TIMEOUT, nibble 0 is the bit 15 (CONV_RESET) and 16 (FORCED_CAL)
                            // nibble e is: bits 11:8 LP_PROXIMITY_CNT
        txdata[1] = 0xa1;   // nibble a is: bits 7:4 FP_PROXIMITY_CNT
                            // nibble 1 is: bits 3:0 FF_SKIP_CNT 
        embot::core::Data content = {txdata, 2};
        embot::hw::i2c::write(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, reg_AMB_COMP_CTRL0, content, 10*1000);
        //embot::hw::i2c::write16(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, 0x002, content, 10*1000);
        
        
        // read back the value of AMB_COMP_CTRL0 Register 0x002
        embot::hw::i2c::read(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, reg_AMB_COMP_CTRL0, dest, 10*1000);
        datav = data[1] + (static_cast<uint16_t>(data[0]) << 8);
        datav = datav;        

        // read default value of AMB_COMP_CTRL1 Register 0x003
        embot::hw::i2c::read(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, reg_AMB_COMP_CTRL1, dest, 10*1000);
        datav = data[1] + (static_cast<uint16_t>(data[0]) << 8);
        datav = datav;
        // i read data[0] = 0x01 data[1] = 0x64
        // default values must be from 15->00: 0x 0 1 6 4
     
        // read default value of AMB_COMP_CTRL2 Register 0x004
        embot::hw::i2c::read(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, reg_AMB_COMP_CTRL2, dest, 10*1000);
        datav = data[1] + (static_cast<uint16_t>(data[0]) << 8);
        datav = datav;
        // i read data[0] = 0x01 data[1] = 0x64
        // default values must be from 15->00: 0x f f f f        

   
        // we need to perform chip initialization
        
        _ads_chip.setaddress(config.i2cdes.bus, embot::hw::ad7147::getBSP().getPROP(s)->i2caddress);
        
        volatile result_t res = result_t::NOK;     
        // 1. reset
        _ads_chip.sendcommand(chipAD7147::CMD::reset);
        
        // 2. configure registers                 
        res = _ads_chip.configure(chipAD7147::SPS::threehundredfifty, chipAD7147::GAIN::onehundredtwentyeigth, 3*embot::core::time1millisec);
       
//        // 3. sanity check: reading back the config registers.
//        memset(_ads_chip.memory, 0, sizeof(_ads_chip.memory));
//        res = _ads_chip.readeveryregister();
//        volatile uint8_t v1 = _ads_chip.r1.get(REG1::FIELD::dr);
//        v1 = v1;   

     
#endif // #if defined(TEST_RW) 

        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(s));
                
        return resOK;
    }

}}} // namespace embot { namespace hw { namespace ad7147 {

#endif



#endif //defined(EMBOT_ENABLE_hw_ad7147)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


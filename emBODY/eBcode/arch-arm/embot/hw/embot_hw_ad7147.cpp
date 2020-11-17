
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


    struct regContent
    {    
        embot::hw::i2c::Reg reg {0, embot::hw::i2c::Reg::Size::sixteenbits};
        uint16_t val {0};
        
        constexpr regContent(uint16_t a, uint16_t v) : reg({a, embot::hw::i2c::Reg::Size::sixteenbits}), val((v>>8) | ((v&0x00ff)<<8)) {} 
        constexpr regContent() = default; 
        uint16_t getvalue() const
        {
            return (val>>8) | ((val&0x00ff)<<8);
        }  
        void setvalue(uint16_t v)
        {
            val = (v>>8) | ((v&0x00ff)<<8);
        }
        void* getcontent() const
        {
            return const_cast<uint16_t*>(&val);
        }     
    };

    enum class sendmode { sendonly, pluscheck, onlycheck };
    bool send(embot::hw::I2C bus, std::uint8_t i2caddress, const regContent &rc, sendmode mode, embot::core::relTime timeout = 20*embot::core::time1millisec)
    {
        bool ret = true;
        embot::hw::i2c::Reg reg2use {rc.reg};
        static volatile int errors {0};
        
        for(int i=0; i<3; i++)
        {
                    
            if((sendmode::sendonly == mode) || (sendmode::pluscheck == mode))
            {
                embot::core::Data content {rc.getcontent(), 2};
                embot::hw::i2c::write(bus, i2caddress, reg2use, content, timeout); 
            } 

            if((sendmode::onlycheck == mode) || (sendmode::pluscheck == mode))
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

    void send(embot::hw::I2C bus, std::uint8_t i2caddress, std::vector<regContent> &tx, sendmode mode,  embot::core::relTime timeout = 20*embot::core::time1millisec)
    {
        for(int i=0; i<tx.size(); i++)
        {
            send(bus, i2caddress, tx[i], mode, timeout);        
        }
        
    }

    void sendstage(embot::hw::I2C bus, std::uint8_t i2caddress, const std::array<regContent, 8> &ar, sendmode mode,  embot::core::relTime timeout = 20*embot::core::time1millisec)
    {
        for(int i=0; i<ar.size(); i++)
        {
            send(bus, i2caddress, ar[i], mode, timeout);        
        }   
    }
    
    constexpr uint16_t conval = 0x2000; // in mtb4 the default is 0x2200. in other places is used 0x2000
    
    constexpr std::array<regContent, 8> st0 
    {
        regContent{0x080, 0xFFFE},
        regContent{0x081, 0x1FFF},
        regContent{0x082, conval},
        regContent{0x083, 0x2626},
        regContent{0x084, 50},
        regContent{0x085, 50},
        regContent{0x086, 100},
        regContent{0x087, 100}      
    };

    constexpr std::array<regContent, 8> st1 
    {
        regContent{0x088, 0xFFFB},
        regContent{0x089, 0x1FFF},
        regContent{0x08a, conval},
        regContent{0x08b, 0x2626},
        regContent{0x08c, 50},
        regContent{0x08d, 50},
        regContent{0x08e, 100},
        regContent{0x08f, 100}      
    };

    constexpr std::array<regContent, 8> st2 
    {
        regContent{0x090, 0xFFEF},
        regContent{0x091, 0x1FFF},
        regContent{0x092, conval},
        regContent{0x093, 0x2626},
        regContent{0x094, 50},
        regContent{0x095, 50},
        regContent{0x096, 100},
        regContent{0x097, 100}       
    };  

    constexpr std::array<regContent, 8> st3 
    {
        regContent{0x098, 0xFFBF},
        regContent{0x099, 0x1FFF},
        regContent{0x09a, conval},
        regContent{0x09b, 0x2626},
        regContent{0x09c, 50},
        regContent{0x09d, 50},
        regContent{0x09e, 100},
        regContent{0x09f, 100}      
    };
    
    constexpr std::array<regContent, 8> st4 
    {
        regContent{0x0A0, 0xFEFF},
        regContent{0x0A1, 0x1FFF},
        regContent{0x0A2, conval},
        regContent{0x0A3, 0x2626},
        regContent{0x0A4, 50},
        regContent{0x0A5, 50},
        regContent{0x0A6, 100},
        regContent{0x0A7, 100}      
    };
    
    constexpr std::array<regContent, 8> st5 
    {
        regContent{0x0A8, 0xFBFF},
        regContent{0x0A9, 0x1FFF},
        regContent{0x0Aa, conval},
        regContent{0x0Ab, 0x2626},
        regContent{0x0Ac, 50},
        regContent{0x0Ad, 50},
        regContent{0x0Ae, 100},
        regContent{0x0Af, 100}   
    };
    
    constexpr std::array<regContent, 8> st6 
    {
        regContent{0x0B0, 0xEFFF},
        regContent{0x0B1, 0x1FFF},
        regContent{0x0B2, conval},
        regContent{0x0B3, 0x2626},
        regContent{0x0B4, 50},
        regContent{0x0B5, 50},
        regContent{0x0B6, 100},
        regContent{0x0B7, 100}      
    };
    
    constexpr std::array<regContent, 8> st7 
    {
        regContent{0x0B8, 0xFFFF},
        regContent{0x0B9, 0x1FFE},
        regContent{0x0Ba, conval},
        regContent{0x0Bb, 0x2626},
        regContent{0x0Bc, 50},
        regContent{0x0Bd, 50},
        regContent{0x0Be, 100},
        regContent{0x0Bf, 100}   
    };    

    constexpr std::array<regContent, 8> st8 
    {   
        regContent{0x0C0, 0xFFFF},
        regContent{0x0C1, 0x1FFB},
        regContent{0x0C2, conval},
        regContent{0x0C3, 0x2626},
        regContent{0x0C4, 50},
        regContent{0x0C5, 50},
        regContent{0x0C6, 100},
        regContent{0x0C7, 100}      
    };
    
    constexpr std::array<regContent, 8> st9 
    {
        regContent{0x0C8, 0xFFFF},
        regContent{0x0C9, 0x1FEF},
        regContent{0x0Ca, conval},
        regContent{0x0Cb, 0x2626},
        regContent{0x0Cc, 50},
        regContent{0x0Cd, 50},
        regContent{0x0Ce, 100},
        regContent{0x0Cf, 100}   
    };  
    
    constexpr std::array<regContent, 8> st10 
    {   
        regContent{0x0D0, 0xFFFF},
        regContent{0x0D1, 0x1FBF},
        regContent{0x0D2, conval},
        regContent{0x0D3, 0x2626},
        regContent{0x0D4, 50},
        regContent{0x0D5, 50},
        regContent{0x0D6, 100},
        regContent{0x0D7, 100}      
    };
    
    constexpr std::array<regContent, 8> st11 
    {
        regContent{0x0D8, 0xFFFF},
        regContent{0x0D9, 0x1EFF},
        regContent{0x0Da, conval},
        regContent{0x0Db, 0x2626},
        regContent{0x0Dc, 50},
        regContent{0x0Dd, 50},
        regContent{0x0De, 100},
        regContent{0x0Df, 100}   
    };  

    constexpr std::array<const std::array<regContent, 8>*, 12>  theregistersofthe12stages {&st0, &st1, &st2, &st3, &st4, &st5, &st6, &st7, &st8, &st9, &st10, &st11};   
              
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
        
       
        

           

//        // check the device id
//        if(!send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x017, 0x1471}, sendmode::onlycheck))
//        {
//            return resNOK;
//        }
    
//    // just for test  
//    // AMB_COMP_CTRL0
//    if(!send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x002, 0x0ff0}, sendmode::onlycheck))
//    {
//        for(;;);
//    }
//    
//    // AMB_COMP_CTRL1
//    if(!send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x003, 0x0164}, sendmode::onlycheck))
//    {
//        for(;;);
//    }
//    
//    // AMB_COMP_CTRL2
//    if(!send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x004, 0xffff}, sendmode::onlycheck))
//    {
//        for(;;);
//    }
    
    // configure the 12 stages
    std::array<regContent, 8> st {}; // temporary object
    
    for(size_t i=0; i<theregistersofthe12stages.size(); i++)
    {
        st = *theregistersofthe12stages[i];
        st[2].setvalue(config.STAGEx_AFE_OFFSET);
        sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st, sendmode::pluscheck);
    }
    
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st0, sendmode::pluscheck);           
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st1, sendmode::pluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st2, sendmode::pluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st3, sendmode::pluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st4, sendmode::pluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st5, sendmode::pluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st6, sendmode::pluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st7, sendmode::pluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st8, sendmode::pluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st9, sendmode::pluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st10, sendmode::pluscheck);
//    sendstage(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, st11, sendmode::pluscheck);
    

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
    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x000, pw_control_val}, sendmode::pluscheck);
    // configure the AMB_COMP_CTRL0
    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x002, 0x0000}, sendmode::pluscheck);
    // configure the AMB_COMP_CTRL1
    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x003, 0x0000}, sendmode::pluscheck);
    // configure the AMB_COMP_CTRL2
    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x004, 0x0000}, sendmode::pluscheck);
    // configure STAGE_LOW_INT_ENABLE
    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x005, 0x0000}, sendmode::pluscheck);
    // configure STAGE_HIGH_INT_ENABLE 
    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x006, 0x0000}, sendmode::pluscheck);
    // configure STAGE_COMPLETE_INT_ENABLE 
    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x007, 0x0400}, sendmode::pluscheck);
// the following three *_STATUS register are read only and should not be written !!!!!!!!!!!!!
//    // configure STAGE_LOW_INT_STATUS 
//    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x008, 0x0000}, sendmode::pluscheck);
//    // configure STAGE_HIGH_INT_STATUS 
//    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x009, 0x0000}, sendmode::pluscheck);
//    // configure STAGE_COMPLETE_INT_STATUS 
//    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x00a, 0x0fff}, sendmode::pluscheck);
    
    // and now, finally: STAGE_CAL_EN
    // 0x0fff = 0000111111111111b
    //          00 -> AVG_LP_SKIP Low power mode skip control 00 = use all samples
    //            00 -> AVG_FP_SKIP Full power mode skip control 00 = skip 3 samples 
    //              1 -> STAGE11_CAL_EN STAGE11 calibration enable
    //               1 -> STAGE10_CAL_EN 
    //                 etc... calibration enabled on all stages.    
    send(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, regContent{0x001, 0x0fff}, sendmode::pluscheck);
    
       
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



#endif //defined(EMBOT_ENABLE_hw_ad7147)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


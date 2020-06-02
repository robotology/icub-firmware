
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Andrea Mura
 * email:   marco.accame@iit.it, andrea.mura@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_ads122c04.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>
#include "stm32hal.h"
#include "embot_hw_bsp.h"
#include "embot_hw_bsp_config.h"

using namespace std;

#include "embot_core_binary.h"
#include "embot_hw_sys.h"


using namespace embot::hw;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

//#define ads122c04_FAKEmode

#if defined(ads122c04_FAKEmode)
#warning WIP: ads122c04_FAKEmode is still defined 
#endif

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if !defined(EMBOT_ENABLE_hw_ads122c04)

namespace embot { namespace hw { namespace ads122c04 {

    bool supported(ADS122C04 s)                                                                         { return false; }
    bool initialised(ADS122C04 s)                                                                       { return false; }
    result_t init(ADS122C04 s, const Config &config)                                                    { return resNOK; }    
    bool isalive(embot::hw::ADS122C04 s, embot::core::relTime timeout)                                  { return false; }    
    bool isacquiring(embot::hw::ADS122C04 s)                                                            { return false; }    
    bool canacquire(embot::hw::ADS122C04 s)                                                             { return false; }        
    result_t acquisition(embot::hw::ADS122C04 s, const embot::core::Callback &oncompletion)             { return resNOK; }
    bool operationdone(embot::hw::ADS122C04 s)                                                          { return false; }
    result_t read(embot::hw::ADS122C04 s, Value &temp)                                                  { return resNOK; }   

}}} // namespace embot { namespace hw { namespace ADS122C04 {


#else

namespace embot { namespace hw { namespace ads122c04 {
              
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    bool supported(ADS122C04 a)
    {
        return embot::hw::bsp::ads122c04::getBSP().supported(a);
    }
    
    bool initialised(ADS122C04 a)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(a));
    }    
      

    struct Acquisition
    {
        static constexpr uint8_t sizeofdata = 6; // 24 bits, two samples
        volatile bool done {false};
        volatile bool ongoing {false};
        Values values {};
        std::uint8_t rxdata[sizeofdata] {0};
        embot::core::Callback userdefCBK {};  
        void clear() { done = false; ongoing = false; values.clear(); memset(rxdata, 0, sizeof(rxdata)); userdefCBK.clear(); }        
        Acquisition() = default;        
    };
    
    struct PrivateData
    {
        std::uint8_t i2caddress[embot::core::tointegral(ADS122C04::maxnumberof)] = {0};   
        Config config[embot::core::tointegral(ADS122C04::maxnumberof)];        
        Acquisition acquisition[embot::core::tointegral(ADS122C04::maxnumberof)];
        PrivateData() = default;
    };
    
    
    //static const std::uint8_t i2caddress = 0x80;
    static const std::uint8_t registerTemperatureRead = 0xE3;
    
    static PrivateData s_privatedata;
    
    static void sharedCBK(void *p)
    {
        Acquisition *acq = reinterpret_cast<Acquisition*>(p);        
        

#if defined(ads122c04_FAKEmode)        
        static volatile uint32_t tmp = 0;
        tmp++;
        acq->values.v1 = tmp;
        acq->values.v2 = tmp+1;
#else
        #warning ADS122C04: to be implemented 
                //std::uint16_t value = (acq->rxdata[0]<<8) + acq->rxdata[1];
               
        //std::int32_t res = ( (17572 * static_cast<std::int32_t>(value) ) >> 16) - 4685;
        //res /= 10;
        
        //acq->values.v1 = static_cast<std::int16_t>(res);
#endif        
        acq->ongoing = false;
        acq->done = true;
        
        acq->userdefCBK.execute();
    }
    

#define ADS122C04_RESET         0x06
#define ADS122C04_START         0x08
#define ADS122C04_POWERDOWN     0x02
#define ADS122C04_RDATA         0x10
#define ADS122C04_RREG          0x20
#define ADS122C04_WREG          0x40
#define ADS122C04_REGISTER0     0
#define ADS122C04_REGISTER1     1
#define ADS122C04_REGISTER2     2
#define ADS122C04_REGISTER3     3

// REGISTER0
#define ADS122C04_CHANNEL1          0x00        // MUX[3:0] = 0000 : AINP = AIN0, AINN = AIN1
#define ADS122C04_CHANNEL2          0x06        // MUX[3:0] = 0110 : AINP = AIN2, AINN = AIN3
#define ADS122C04_GAINx1            0x00        // GAIN[2:0]
#define ADS122C04_PGA_BYPASS_EN     0x00        // PGA enabled (default)
#define ADS122C04_PGA_BYPASS_DIS    0x01        // PGA disabled and bypassed

// REGISTER1
#define ADS122C04_DR_1kSPS          0x06        // DR[2:0]
#define ADS122C04_NORMAL            0x00        // 0 : Normal mode (256-kHz modulator clock, default)
#define ADS122C04_TURBO             0x01        // 1 : Turbo mode (512-kHz modulator clock)
#define ADS122C04_SINGLESHOT        0x00        // Single-shot conversion mode
#define ADS122C04_CM                0x01        // Continuous conversion mode



    struct reg_t
    { 
        uint8_t memory {0};
        reg_t() 
        {
            synch2values();
        };
        virtual void synch2memory() = 0;
        virtual void synch2values() = 0;
        void set(uint8_t val, uint8_t pos, uint8_t msk)
        {
            memory |= embot::core::binary::mask::place(val, pos, msk);
        } 
        uint8_t get(uint8_t pos, uint8_t msk) const
        {
            return embot::core::binary::mask::extract(memory, pos, msk);
        }
    };
    
    struct reg00_t : public reg_t
    {   // [mux | gain | bypass]
        uint8_t mux     :4;    
        uint8_t gain    :3;        
        uint8_t bypass  :1;       

        virtual void synch2memory() 
        {  
            set(mux, 4, 0b1111);
            set(gain, 1, 0b111);
            set(bypass, 0, 0b1);
        }
        virtual void synch2values()
        {
            mux = get(4, 0b1111);
            gain = get(1, 0b111);
            bypass = get(0, 0b1);
        } 
        reg00_t() = default;        
    };
    
    struct reg01_t : public reg_t
    {   // [dr | mode | cm | vref | ts]
        uint8_t dr     : 3;      
        uint8_t mode   : 1;        
        uint8_t cm     : 1;        
        uint8_t vref   : 2;        
        uint8_t ts     : 1;     
        
        virtual void synch2memory() 
        {  
            set(dr, 5, 0b111);
            set(mode, 4, 0b1);
            set(cm, 3, 0b1);
            set(vref, 1, 0b11);
            set(ts, 0, 0b1);
        }
        virtual void synch2values()
        {
            dr = get(5, 0b111);
            mode = get(4, 0b1);
            cm = get(3, 0b1);
            vref = get(1, 0b11);
            ts = get(0, 0b1);
        }        
        reg01_t() = default;     
    };
    
//        enum class REG : uint8_t { zero = 0, one = 1, two = 2, three = 3 };
//        
//        // REG::zero ->         
//        uint32_t mux    : 4;        // 4 bits
//        uint32_t gain   : 3;        // 3 bits
//        uint32_t bypass : 1;        // 1 bit
//        
//        // REG::one ->        
//        uint32_t dr     : 3;        // 3 bits
//        uint32_t mode   : 1;        // 1 bit
//        uint32_t cm     : 1;        // 1 bit
//        uint32_t vref   : 2;        // 2 bits
//        uint32_t ts     : 1;        // 1 bit
//        
//        // REG::two ->        
//        uint32_t drdy   : 1;        // 1 bit 
//        uint32_t dcnt   : 1;        // 1 bit
//        uint32_t crc    : 2;        // 2 bits
//        uint32_t bcs    : 1;        // 1 bit
//        uint32_t idac   : 3;        // 3 bits
//        
//        // REG::three ->
//        uint32_t i1mux  : 3;        // 3 bits
//        uint32_t i2mux  : 3;        // 3 bits  
//        uint32_t unused : 2;        
//        
//        uint8_t regs[4] {0};
//        
//        ADS122C04config() = default;
//        
//        void update() 
//        {   // better to limit values of the fields
//            regs[0] = (mux<<4) +  (gain<<1) + bypass;
//            regs[1] = (dr<<5) + (mode<<4) + (cm<<3) + (vref<<1) + (ts);
//            regs[2] = (drdy<<7) + (dcnt<<6) + (crc<<4) + (bcs<<3) + idac;
//            regs[3] = (i1mux<<5) + (i2mux<<2);                        
//        }
//        
//        uint8_t get(REG r)
//        {
//            update();
//            return regs[embot::core::tointegral(r)];
//        } 
//        
//        result_t write(embot::hw::I2C b, embot::hw::i2c::ADR adr, REG r, embot::core::relTime timeout = 3*embot::core::time1millisec)
//        {
//            uint8_t regWrite[2] = {0};
//            regWrite[0] = 0x40+(embot::core::tointegral(r)<<2);
//            regWrite[1] = regs[embot::core::tointegral(r)];
//            return embot::hw::i2c::transmit(b, adr, {&regWrite, 2}, timeout);
//        } 
//        
//        result_t read(embot::hw::I2C b, embot::hw::i2c::ADR adr, REG r, embot::core::relTime timeout = 3*embot::core::time1millisec)
//        {
//            embot::hw::i2c::REG reg = 0x20 + (embot::core::tointegral(r)<<2);
//            embot::core::Data dest = {&regs[embot::core::tointegral(r)], 1};
//            return embot::hw::i2c::read(b, adr, reg, dest, timeout);
//        }         
// 
//    }; 
    
    struct ADS122C04config
    {
        enum class REG : uint8_t { zero = 0, one = 1, two = 2, three = 3 };
        
        // REG::zero ->         
        uint32_t mux    : 4;        // 4 bits
        uint32_t gain   : 3;        // 3 bits
        uint32_t bypass : 1;        // 1 bit
        
        // REG::one ->        
        uint32_t dr     : 3;        // 3 bits
        uint32_t mode   : 1;        // 1 bit
        uint32_t cm     : 1;        // 1 bit
        uint32_t vref   : 2;        // 2 bits
        uint32_t ts     : 1;        // 1 bit
        
        // REG::two ->        
        uint32_t drdy   : 1;        // 1 bit 
        uint32_t dcnt   : 1;        // 1 bit
        uint32_t crc    : 2;        // 2 bits
        uint32_t bcs    : 1;        // 1 bit
        uint32_t idac   : 3;        // 3 bits
        
        // REG::three ->
        uint32_t i1mux  : 3;        // 3 bits
        uint32_t i2mux  : 3;        // 3 bits  
        uint32_t unused : 2;        
        
        uint8_t regs[4] {0};
        
        ADS122C04config() = default;
        
        void update() 
        {   // better to limit values of the fields
            regs[0] = (mux<<4) +  (gain<<1) + bypass;
            regs[1] = (dr<<5) + (mode<<4) + (cm<<3) + (vref<<1) + (ts);
            regs[2] = (drdy<<7) + (dcnt<<6) + (crc<<4) + (bcs<<3) + idac;
            regs[3] = (i1mux<<5) + (i2mux<<2);                        
        }
        
        uint8_t get(REG r)
        {
            update();
            return regs[embot::core::tointegral(r)];
        } 
        
        result_t write(embot::hw::I2C b, embot::hw::i2c::ADR adr, REG r, embot::core::relTime timeout = 3*embot::core::time1millisec)
        {
            uint8_t regWrite[2] = {0};
            regWrite[0] = 0x40+(embot::core::tointegral(r)<<2);
            regWrite[1] = regs[embot::core::tointegral(r)];
            return embot::hw::i2c::transmit(b, adr, {&regWrite, 2}, timeout);
        } 
        
        result_t read(embot::hw::I2C b, embot::hw::i2c::ADR adr, REG r, embot::core::relTime timeout = 3*embot::core::time1millisec)
        {
            embot::hw::i2c::REG reg = 0x20 + (embot::core::tointegral(r)<<2);
            embot::core::Data dest = {&regs[embot::core::tointegral(r)], 1};
            return embot::hw::i2c::read(b, adr, reg, dest, timeout);
        }         
 
    }; 
    
    ADS122C04config _chip_config;
    
    
              
    result_t init(ADS122C04 s, const Config &config)
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
        embot::hw::bsp::ads122c04::getBSP().init(s);
        
        std::uint8_t index = embot::core::tointegral(s);
        
#if defined(ads122c04_FAKEmode)
#else        
                
        // init i2c ..
        embot::hw::i2c::init(config.i2cdes.bus, config.i2cdes.config);
        if(false == embot::hw::i2c::ping(config.i2cdes.bus, embot::hw::bsp::ads122c04::getBSP().getPROP(s)->i2caddress))
        {
            return resNOK;
        }
        
        reg00_t reg00;
        
        reg00.mux = 0xff;
        reg00.gain = 1;
        reg00.bypass = 3;
        
        reg00.synch2memory();
        
        reg00.memory = 0x3;
        reg00.synch2values();
        
        reg00.mux = reg00.mux;
        
        // we need to perform chip initialization
        
        // 1. reset
        uint8_t cmdRESET {ADS122C04_RESET}; 
        embot::hw::i2c::transmit(config.i2cdes.bus, embot::hw::bsp::ads122c04::getBSP().getPROP(s)->i2caddress, {&cmdRESET, 1});
        embot::hw::sys::delay(embot::core::time1millisec);
        
        // 2. basic configuration 
        constexpr uint8_t regREGISTER1 = 1;
        _chip_config.dr = ADS122C04_DR_1kSPS;
        _chip_config.cm = ADS122C04_SINGLESHOT;
        uint8_t regval = _chip_config.get(ADS122C04config::REG::one);
        uint8_t regWrite[2] = {ADS122C04_WREG+(regREGISTER1<<2), regval};
        embot::hw::i2c::transmit(config.i2cdes.bus, embot::hw::bsp::ads122c04::getBSP().getPROP(s)->i2caddress, {&regWrite, 2});
        
        // 3. sanity check: reading back the config registers.
        uint8_t regs[4] = {0};
        for(int i=0; i<4; i++)
        {
            embot::hw::i2c::REG reg = ADS122C04_RREG + (i<<2);
            embot::core::Data dest = {&regs[i], 1};
            embot::hw::i2c::read(config.i2cdes.bus, embot::hw::bsp::ads122c04::getBSP().getPROP(s)->i2caddress, reg, dest, 5*embot::core::time1millisec);
        }
        
        regs[0] = regs[0];
        volatile int a = regs[0];
        a = a+1;
        
        //
//        void ADS122C04_ReadRegister(uint8_t REGISTER_ADDRESS){
//    ADS122C04_I2C1_TxBuffer[0]=ADS122C04_RREG + (REGISTER_ADDRESS<<2);
//    HAL_I2C_Master_Transmit_DMA(&hi2c1, (uint16_t)ADS122C04_I2C_ADDRESS, (uint8_t*)ADS122C04_I2C1_TxBuffer, 1);
//    reply=1;
//}
        
        #if 0
        posso usare  read(embot::hw::I2C b, ADR adr, REG reg, embot::core::Data &destination, embot::core::relTime timeout)
        usando
        REG reg = ADS122C04_RREG + (REGISTER_ADDRESS<<2);
        destination = {&res, 1};        
        
        #endif
        
#endif        
        s_privatedata.i2caddress[index] = embot::hw::bsp::ads122c04::getBSP().getPROP(s)->i2caddress;
        s_privatedata.config[index] = config;
        s_privatedata.acquisition[index].clear();
        
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(s));
                
        return resOK;
    }

    
    bool isacquiring(ADS122C04 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        std::uint8_t index = embot::core::tointegral(s);        
        return s_privatedata.acquisition[index].ongoing;     
    }
    
    
    bool canacquire(ADS122C04 s)
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
#if defined(ads122c04_FAKEmode)
        return true;
#else         
        return !embot::hw::i2c::isbusy(s_privatedata.config[index].i2cdes.bus);  
#endif        
    }    
    
    result_t acquisition(ADS122C04 s, const embot::core::Callback &oncompletion)
    {
        if(false == canacquire(s))
        {
            return resNOK;
        }
        
        std::uint8_t index = embot::core::tointegral(s);
                
        s_privatedata.acquisition[index].clear();
        s_privatedata.acquisition[index].ongoing = true;
        s_privatedata.acquisition[index].done = false;
        s_privatedata.acquisition[index].userdefCBK = oncompletion;

#if defined(ads122c04_FAKEmode)
        embot::core::Callback cbk(sharedCBK, &s_privatedata.acquisition[index]);
        cbk.execute();
#else         
        // ok, now i trigger i2c.
        embot::core::Callback cbk(sharedCBK, &s_privatedata.acquisition[index]);
        embot::core::Data data = embot::core::Data(&s_privatedata.acquisition[index].rxdata[0], sizeof(s_privatedata.acquisition[index].rxdata));
        embot::hw::i2c::read(s_privatedata.config[index].i2cdes.bus, s_privatedata.i2caddress[index], registerTemperatureRead, data, cbk);
#endif                
        return resOK;
    }
    
    bool isalive(ADS122C04 s, embot::core::relTime timeout)
    {
        if(false == initialised(s))
        {
            return false;
        } 
#if defined(ads122c04_FAKEmode)
        return true;
#else 
        std::uint8_t index = embot::core::tointegral(s);
        return embot::hw::i2c::ping(s_privatedata.config[index].i2cdes.bus, s_privatedata.i2caddress[index], timeout);  
#endif
    }

    
    bool operationdone(ADS122C04 s)
    {
        if(false == initialised(s))
        {
            return false;
        } 

        return s_privatedata.acquisition[embot::core::tointegral(s)].done;        
    } 
    
    
    result_t read(ADS122C04 s, Values &vals)
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
        vals.v1 = s_privatedata.acquisition[index].values.v1;
        vals.v2 = s_privatedata.acquisition[index].values.v2;
  
        return resOK;        
    }
    
    
    result_t s_write_register(ADS122C04 s, Values &vals)
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

        constexpr uint8_t regREGISTER1 = 1;
        _chip_config.dr = ADS122C04_DR_1kSPS;
        _chip_config.cm = ADS122C04_SINGLESHOT;
        uint8_t regval = _chip_config.get(ADS122C04config::REG::one);
        uint8_t regWrite[2] = {ADS122C04_WREG+(regREGISTER1<<2), regval};
        embot::hw::i2c::transmit(s_privatedata.config[index].i2cdes.bus, s_privatedata.i2caddress[index], {&regWrite, 2});
  
        return resOK;        
    }    
    
 
}}} // namespace embot { namespace hw { namespace ads122c04 {



#endif //defined(EMBOT_ADS122C04_ENABLED)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


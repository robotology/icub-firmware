
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// the API of the module
#include "testHW.h"


// the dependencies for this implementation

#include "embot_core.h"
#include "embot_core_binary.h"

#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_led.h"
#include "embot_hw_sys.h"




//#define TEST_CAN_ENABLED

//#define TEST_TLV_EMULATED

#define TEST_I2C_GPIO

#if defined(TEST_CAN_ENABLED)
#include "embot_hw_can.h"
void can_init();
void can_print(const std::string &s);
void can_send(uint32_t v);
#endif

#if defined(TEST_TLV_EMULATED)
void tlv_init();
void tlv_tick();
#endif

#if defined(TEST_I2C_GPIO)
void i2c_gpio_init();
void i2c_gpio_tick();
#endif  

void testHWinit(embot::os::Thread *owner)
{
#if defined(TEST_CAN_ENABLED)    
    can_init();
#endif

#if defined(TEST_TLV_EMULATED)
    tlv_init();
#endif    

#if defined(TEST_I2C_GPIO)  
		i2c_gpio_init();  
#endif  	
}

void testHWinit2()
{

#if defined(TEST_I2C_GPIO)  
		i2c_gpio_init();  
#endif  	
}

void testHWtick()
{
#if defined(TEST_TLV_EMULATED)    
    tlv_tick();  
#endif

	#if defined(TEST_I2C_GPIO)
		i2c_gpio_tick();  
#endif  
}


// now some code


// used to wait for end of an hw wait
struct WaitCBK
{
    volatile bool done {true};
    embot::core::Callback onTermination {stop, this};
    
    void start() 
    { 
        done = false; 
    }
    
    void wait() 
    { 
        for(;;) 
        { 
            if(true == done) 
            {
                return; 
            } 
        }
    }
    
    static void stop(void *p)
    {
        WaitCBK * me = reinterpret_cast<WaitCBK*>(p);
        me->done =  true;
    }
    
};

#if defined(TEST_CAN_ENABLED)    

constexpr uint8_t canID {1};
void can_init()
{
    static bool initted = false;
    if(true == initted)
    {
        return;
    }
    initted = true;
    // init can but do not enabled yet
    embot::hw::can::Config canconfig;   // default is tx/rxcapacity=8
    canconfig.rxcapacity = 8;
    canconfig.txcapacity = 8;
    canconfig.onrxframe = {}; 
    embot::hw::can::init(embot::hw::CAN::one, canconfig);
    embot::hw::can::setfilters(embot::hw::CAN::one, canID); 
        
    embot::hw::can::enable(embot::hw::CAN::one);
}

void can_print(const std::string &s)
{
    embot::hw::can::Frame frame {};
    frame.id = canID;
    frame.size = 8;
            
    const char *str = s.c_str();
    size_t size = strlen(str);
    size_t count {0};
    constexpr size_t maxbytes {8};
    for(size_t count=0; count<size; )
    {
        uint8_t n = std::min(maxbytes, size-count);
        memset(frame.data, 0, 8);
        memmove(frame.data, &str[count], n);
        count += n;
        embot::hw::can::put(embot::hw::CAN::one, frame);
    }  
    embot::hw::can::transmit(embot::hw::CAN::one);    
    
}

void can_send(uint32_t v)
{
    embot::hw::can::Frame frame {};
    frame.id = canID;
    frame.size = 8;
    frame.data[0] = v & 0x000000ff;
    frame.data[1] = (v >> 8) & 0x000000ff;
    frame.data[2] = (v >> 16) & 0x000000ff;
    frame.data[3] = (v >> 24) & 0x000000ff;
    embot::hw::can::put(embot::hw::CAN::one, frame);
    embot::hw::can::transmit(embot::hw::CAN::one);
}
    
#endif


// TLV


#if defined(TEST_TLV_EMULATED)

#include "embot_hw_tlv493d.h"

constexpr embot::hw::TLV493D tlv2test {embot::hw::TLV493D::one};
    
embot::hw::tlv493d::Position position{0};
   
volatile bool error {false};

void tlv_init()
{    
    if(embot::hw::resOK != embot::hw::tlv493d::init(tlv2test, {embot::hw::tlv493d::Config::startupMODE::dontresetCHIP}))
    {
        error = true;
    }   
}

WaitCBK wait4it {};
    
void tlv_tick()
{
    wait4it.start();
    embot::hw::tlv493d::acquisition(tlv2test, wait4it.onTermination);
    
    wait4it.wait();
    
    // and now read it
    embot::hw::tlv493d::read(tlv2test, position);
    
    position = position;  
    
#if defined(TEST_CAN_ENABLED)    
    can_send(position);
    std::string msg {"pos = " + std::to_string(position)};
    //can_print("ciao uomo, come stai?");
    can_print(msg);
#endif  
    
}

#endif 


#if 0

the new emulated driver for i2c needs:
- embot::hw::i2c::Descriptor
- embot::hw::i2c::isbusy(s_privatedata.i2cdes[index].bus)
- embot::hw::i2c::init(embot::hw::tlv493d::getBSP().getPROP(h)->i2cdes.bus, {});
- embot::hw::i2c::receive(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, data, cbk);
- embot::hw::i2c::ping(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, timeout);
- embot::hw::i2c::receive(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, data, embot::core::time1second);
- embot::hw::i2c::transmit(s_privatedata.i2cdes[index].bus, s_privatedata.i2cdes[index].adr, data, embot::core::time1second);
- embot::hw::i2c::tx(s_privatedata.i2cdes[index].bus, 0x00, dummy, 3*embot::core::time1millisec); 




#endif

#if defined(TEST_I2C_GPIO)

constexpr embot::hw::GPIO scl = {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::four};     // scl
constexpr embot::hw::GPIO sda = {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::eight};    // sda

static constexpr embot::hw::gpio::Config out 
{ 
	  embot::hw::gpio::Mode::OUTPUTpushpull, 
  	embot::hw::gpio::Pull::pullup, 
  	embot::hw::gpio::Speed::veryhigh 
};    



void i2c_gpio_init() 
{
	embot::hw::gpio::init(scl, out);
	embot::hw::gpio::init(sda, out);
}

void i2c_gpio_tick() 
{
	for(uint8_t i=0; i<8; i++){
			embot::hw::gpio::set(scl, embot::hw::gpio::State::SET);
			embot::hw::gpio::set(sda, embot::hw::gpio::State::RESET);
		  embot::core::wait(embot::core::time1second);
			
			embot::hw::gpio::set(scl, embot::hw::gpio::State::RESET);
			embot::hw::gpio::set(sda, embot::hw::gpio::State::SET);
		  embot::core::wait(embot::core::time1second);
	}
}

#endif


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




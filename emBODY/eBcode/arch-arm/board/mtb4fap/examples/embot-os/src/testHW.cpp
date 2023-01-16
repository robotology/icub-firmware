
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
#include "embot_hw_timer.h"

#include "embot_hw_i2ce.h"



//#define TEST_CAN_ENABLED

#define TEST_TLV_EMULATED

//#define TEST_I2C_EMULATED

//#define TEST_I2C_EMULATED_embot

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

#if defined(TEST_I2C_EMULATED)

#if defined(TEST_I2C_EMULATED_embot)

// just
#include "embot_hw_i2ce.h"

#else

constexpr embot::hw::GPIO scl = {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::four};     // scl
constexpr embot::hw::GPIO sda1 = {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::eight};   // sda1

void i2c_gpio_init(embot::hw::GPIO sda);
void i2c_gpio_tick();

void i2c_scl_low();
void i2c_scl_high();
void i2c_sda_low(embot::hw::GPIO sda);
void i2c_sda_high(embot::hw::GPIO sda);

void i2c_start(embot::hw::GPIO sda);
void i2c_stop(embot::hw::GPIO sda);

void i2c_sda_input(embot::hw::GPIO sda);
void i2c_sda_output(embot::hw::GPIO sda);
void i2c_sda_input_off(embot::hw::GPIO sda);
void i2c_sda_output_off(embot::hw::GPIO sda);

void i2c_send_ACK(embot::hw::GPIO sda);
void i2c_send_NACK(embot::hw::GPIO sda);

volatile bool go {false};
void lock()
{
    go = false;
}
void release(void *p)
{
    go = true;
}
void hwtimerwait()
{
    go = false;
    embot::hw::timer::start(embot::hw::TIMER::six);
    for(;;)
    {
        if(go) break;
    }
}

    

void i2c_scl_delay(void)
{
//    constexpr embot::core::Time semiperiod = 5*embot::core::time1microsec;
//    constexpr embot::core::Time semiperiod = 1*embot::core::time1microsec;
    constexpr embot::core::Time semiperiod = 5;
    embot::hw::sys::delay(semiperiod);
//    embot::hw::sys::onemicro();
    
//    hwtimerwait();
    
}

embot::hw::gpio::State i2c_sda_status(embot::hw::GPIO sda);
embot::hw::gpio::State i2c_read_ACK(embot::hw::GPIO sda);

bool i2c_write_byte(unsigned char data);
unsigned char i2c_read_byte(void);

bool I2C_init();
bool I2C_ping(uint8_t adr);
bool I2C_read(uint8_t adr, size_t n, uint8_t *data);
bool I2C_write(uint8_t adr, size_t n, uint8_t *data);

#endif
	
#endif  


void testHWinit(embot::os::Thread *owner)
{
#if defined(TEST_CAN_ENABLED)    
    can_init();
#endif

#if defined(TEST_TLV_EMULATED)
    tlv_init();
#endif    

#if defined(TEST_I2C_EMULATED)  
 

#if defined(TEST_I2C_EMULATED_embot)

    embot::hw::i2ce::init(embot::hw::I2CE::one, {});


#else    
    
    embot::hw::timer::Config tc 
    {
        2*embot::core::time1microsec,
        embot::hw::timer::Mode::oneshot,
        {release, nullptr}
    };
    embot::hw::timer::init(embot::hw::TIMER::six, tc);
    embot::hw::sys::smalldelayinit();
    I2C_init();
	
#endif


#endif  	
}

#if defined(TEST_I2C_EMULATED)  

uint8_t data2reset[1] = {0};
void testI2Creset()
{
#if defined(TEST_I2C_EMULATED_embot)

    // to be done
    
#else    
    static volatile uint32_t n {0};
    volatile bool ok {false};
    ok = I2C_write(0xbc, sizeof(data2reset), data2reset);
    
    if(ok)
    {
        n++;
    }
    
#endif    
       
    embot::hw::sys::delay(200*1000);
    
}

void testI2Cping()
{
    static volatile embot::core::Time t {0};
    static volatile embot::core::Time duration {0};
    
    volatile bool found1 = false;
    volatile bool found2 = false;
    static volatile uint32_t hereitis = 0;
    static volatile uint32_t unluckyyou = 0;
    t = embot::core::now();
#if defined(TEST_I2C_EMULATED_embot)
    found1 = embot::hw::i2ce::ping(embot::hw::I2CE::one, 0xbc, 3*embot::core::time1millisec);
#else    
    found1 = I2C_ping(0xbc);
#endif    
    duration = embot::core::now() - t;
#if defined(TEST_I2C_EMULATED_embot)
    found2 = embot::hw::i2ce::ping(embot::hw::I2CE::one, 0x3e, 3*embot::core::time1millisec);
#else     
    found2 = I2C_ping(0x3e);
#endif
    
    if(found1 || found2)
    {
        hereitis++;
    }
    else
    {
        unluckyyou++;
    }
}

void testI2Cdiscover()
{
#if defined(TEST_I2C_EMULATED_embot)
    static volatile uint32_t therearesome = 0;
    static volatile uint32_t unluckyyou = 0;
    std::vector<embot::hw::i2ce::ADR> adrs {};
    volatile bool foundany = embot::hw::i2ce::discover(embot::hw::I2CE::one, adrs);
    volatile size_t ss = adrs.size();
    if(foundany)
    {
        therearesome = ss;
        volatile embot::hw::i2ce::ADR a {0};
        for(size_t i=0; i<ss; i++)
        {
            a = adrs[i];
            a = a;
        }
    }
    else
    {
        unluckyyou++;
    }
#endif   
}

uint8_t data2read[10] = {0};
void testI2Cread()
{
#if defined(TEST_I2C_EMULATED_embot)
    static volatile embot::core::Time t {0};
    static volatile embot::core::Time duration {0};
    
    volatile embot::hw::result_t r {embot::hw::resNOK};
    volatile uint8_t nn {0};
    memset(data2read, 0, sizeof(data2read));
    embot::core::Data da {data2read, sizeof(data2read)};
    t = embot::core::now();
    r = embot::hw::i2ce::receive(embot::hw::I2CE::one, 0xbc, da, 5*embot::core::time1millisec);
    duration = embot::core::now() - t;
    if(embot::hw::resNOK == r)
    {
        nn++;
    }
#else    
    static volatile uint32_t n {0};
    volatile bool ok {false};
    memset(data2read, 0, sizeof(data2read));
    ok = I2C_read(0xbc, sizeof(data2read), data2read);
    
    if(ok)
    {
        n++;
    }
#endif    
}

uint8_t data2write[4] = {0};
void testI2Cwrite()
{
#if defined(TEST_I2C_EMULATED_embot)

    // to be done
#else    
    static volatile uint32_t n {0};
    volatile bool ok {false};
    memmove(data2write, data2read, sizeof(data2write));
    ok = I2C_write(0xbc, sizeof(data2write), data2write);
    
    if(ok)
    {
        n++;
    }
#endif    
}

#endif // #if defined(TEST_I2C_EMULATED)  


void testHWtick()
{
#if defined(TEST_TLV_EMULATED)    
    tlv_tick();  
#endif

#if defined(TEST_I2C_EMULATED)
    
    
#if defined(TEST_I2C_EMULATED_embot)

    testI2Cping();
    testI2Cdiscover();
    testI2Cread();
    
    
#else    

 //   testI2Creset();
    testI2Cping();
    testI2Cread();
    testI2Cwrite();
#endif        
    
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
    
    bool wait(embot::core::relTime timeout = embot::core::reltimeWaitForever) 
    {
        if(true == done)
        {
            return done;
        }
        
        embot::core::Time timeofexit = embot::core::now() + timeout;
        
        for(;;) 
        { 
            if(true == done) 
            {
                return done; 
            } 
            
            if(embot::core::now() > timeofexit)
            {
                return done;
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

volatile bool stophere {true};
   
volatile bool error {false};

void tlv_init()
{    
    if(embot::hw::resOK != embot::hw::tlv493d::init(tlv2test, {embot::hw::tlv493d::Config::startupMODE::dontresetCHIP}))
    {
        error = true;
    }   
}

WaitCBK wait4it {};
    
volatile embot::core::Time acqstart {0};
volatile embot::core::Time acquisitiontime {0};
volatile float angle {0};
volatile embot::hw::result_t res {embot::hw::resNOK};
    
void tlv_tick()
{
    position = 0;
    wait4it.start();
    acqstart = embot::core::now();
    res = embot::hw::tlv493d::acquisition(tlv2test, wait4it.onTermination);
    wait4it.wait(5*embot::core::time1millisec);
    acquisitiontime = embot::core::now() - acqstart;
    
    // and now read it
    embot::hw::tlv493d::read(tlv2test, position);
    
    position = position;
    angle = position / 100.0f;
    
    stophere = stophere;
    
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

#if defined(TEST_I2C_EMULATED)


#if defined(TEST_I2C_EMULATED_embot)

    // we use the code moved to embot::hw::i2c2

#else

//#define DONTUSE_EMBOT_HW_GPIO
#define DONTDEINIT

/* emulated i2c basic functions
   ref. https://www.nxp.com/docs/en/application-note/AN12841.pdf */

static constexpr embot::hw::gpio::Config out 
{ 
    embot::hw::gpio::Mode::OUTPUTpushpull, 
  	embot::hw::gpio::Pull::pullup, 
  	embot::hw::gpio::Speed::veryhigh 
};

static constexpr embot::hw::gpio::Config in 
{ 
    embot::hw::gpio::Mode::INPUT, 
    embot::hw::gpio::Pull::pullup, 
  	embot::hw::gpio::Speed::veryhigh 
};    

GPIO_InitTypeDef GPIO_sda_inp = {0};
GPIO_InitTypeDef GPIO_sda_out = {0};
void i2c_gpio_init(embot::hw::GPIO sda) 
{
    GPIO_sda_inp.Pin = GPIO_PIN_8;
    GPIO_sda_inp.Mode = GPIO_MODE_INPUT;
    GPIO_sda_inp.Pull = GPIO_PULLUP;
    GPIO_sda_inp.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    
    GPIO_sda_out.Pin = GPIO_PIN_8;
    GPIO_sda_out.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_sda_out.Pull = GPIO_PULLUP;
    GPIO_sda_out.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  
    
	embot::hw::gpio::init(scl, out);
    
	i2c_sda_output(sda);
}



void i2c_sda_input(embot::hw::GPIO sda) 
{
    #if defined(DONTUSE_EMBOT_HW_GPIO)
    HAL_GPIO_Init(GPIOA, &GPIO_sda_inp);
    #else 
	embot::hw::gpio::init(sda, in);
    #endif
}

void i2c_sda_output(embot::hw::GPIO sda) 
{
    #if defined(DONTUSE_EMBOT_HW_GPIO)
    HAL_GPIO_Init(GPIOA, &GPIO_sda_out);
//    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_8, LL_GPIO_MODE_OUTPUT);		
//    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_8, LL_GPIO_OUTPUT_PUSHPULL);            

    #else    
	embot::hw::gpio::init(sda, out);
    #endif
}

void i2c_sda_input_off(embot::hw::GPIO sda) 
{
    #if defined(DONTUSE_EMBOT_HW_GPIO)
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8);
    #else 
    #if defined(DONTDEINIT)
    #else    
	embot::hw::gpio::deinit(sda);
    #endif
    #endif
}

void i2c_sda_output_off(embot::hw::GPIO sda) 
{
    #if defined(DONTUSE_EMBOT_HW_GPIO)
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8);
    #else 
    #if defined(DONTDEINIT)
    #else      
	embot::hw::gpio::deinit(sda);
    #endif
    #endif
}

embot::hw::gpio::State i2c_sda_status(embot::hw::GPIO sda) 
{
    #if defined(DONTUSE_EMBOT_HW_GPIO)
    GPIO_PinState s = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);
    return static_cast<embot::hw::gpio::State>(s);
    #else	
	return embot::hw::gpio::get(sda);
	#endif

}

void i2c_scl_low(void)
{
    #if defined(DONTUSE_EMBOT_HW_GPIO)
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    #else
	embot::hw::gpio::set(scl, embot::hw::gpio::State::RESET);
    #endif
}

void i2c_scl_high(void)
{
    #if defined(DONTUSE_EMBOT_HW_GPIO)
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
    #else
	embot::hw::gpio::set(scl, embot::hw::gpio::State::SET);
    #endif
}

void i2c_sda_low(embot::hw::GPIO sda)
{
    #if defined(DONTUSE_EMBOT_HW_GPIO)
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    #else    
	embot::hw::gpio::set(sda, embot::hw::gpio::State::RESET);
    #endif
}

void i2c_sda_high(embot::hw::GPIO sda)
{
    #if defined(DONTUSE_EMBOT_HW_GPIO)
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
    #else
	embot::hw::gpio::set(sda, embot::hw::gpio::State::SET);
    #endif
}



/* START condition */
void i2c_start(embot::hw::GPIO sda)
{
 i2c_scl_low();
 i2c_sda_high(sda);
 i2c_scl_high();
 i2c_scl_delay();
 /* SDA change from high to low when SCL is high, i2c start */
 i2c_sda_low(sda);
}

/* STOP condition */
void i2c_stop(embot::hw::GPIO sda)
{
 i2c_scl_low();
 i2c_sda_low(sda);
 i2c_scl_high();
 i2c_scl_delay();
 /* SDA change from low to high when SCL is high, i2c stop */
 i2c_sda_high(sda);
}

/* master reads ACK signal */
embot::hw::gpio::State i2c_read_ACK(embot::hw::GPIO sda)
{
    embot::hw::gpio::State  ack;
    /* the 9th clock start */
    i2c_scl_low();

    /* disable PTB6(SDA) output, enable PTB6(SDA) input */
    i2c_sda_output_off(sda);
    i2c_sda_input(sda);

    i2c_scl_delay();
    i2c_scl_high();
    /* read ACK signal when SCL is high */
    ack = i2c_sda_status(sda);
    i2c_scl_delay();
    /* the 9th clock end */

    /* disable SDA input, enable SDA output */
    i2c_sda_input_off(sda);
    i2c_sda_output(sda);

    /* SCL hold low to wait */
    i2c_scl_low();
    i2c_sda_high(sda);
    return ack;
}

/* master sends NACK to slave */
void i2c_send_NACK(void)
{
 /* the 9th clock start*/
 i2c_scl_low();
 /*send NACK signal */
 i2c_sda_high(sda1);
 i2c_scl_delay();
 i2c_scl_high();
 i2c_scl_delay();
 /* the 9th clock start */
 /* SCL hold low to wait */
 i2c_scl_low();
 i2c_sda_high(sda1);
}

/* master writes a byte to salve and check ACK signal */
bool i2c_write_byte(unsigned char data)
{
    bool ret = true;
    unsigned char i;
    /* write 8 bits data */
    for(i=0;i<8;i++)
    {
        i2c_scl_low();
        i2c_scl_delay();

        /* Data can be changed only while SCL is low */
        if(data & 0x80)
        {
            i2c_sda_high(sda1);
        }
        else
        {
            i2c_sda_low(sda1);
        }

        /* Data must be held stable while SCL is high */
        i2c_scl_high();
        i2c_scl_delay();
        data = data<<1;
    }
    /* Must set SCL to low. If SCL is high, SDA change to High/Low will cause Stop/Start signal.*/
    i2c_scl_low();
    /* check ACK signal, ACK signal is 0, NACK signal is 1 */
    if(i2c_read_ACK(sda1) == embot::hw::gpio::State::SET)
    {
        /* receive NACK signal,stop data transfer */
        i2c_stop(sda1);
        ret = false;
//        while(1);
    }
    
    return ret;
}

/* master reads a byte from slave */
unsigned char i2c_read_byte(void)
{
 unsigned char i;
 unsigned char data = 0;

 /* disable PTB6(SDA) output, enable PTB6(SDA) input */
 i2c_sda_output_off(sda1);
 i2c_sda_input(sda1);
	 /* write 8 bits data */
 for(i=0;i<8;i++)
 {
 i2c_scl_low();
 i2c_scl_delay();
 /* Data must be held stable while SCL is high */
 i2c_scl_high();
 data = data<<1;
 /* read SDA data */
	 if(i2c_sda_status(sda1) == embot::hw::gpio::State::SET)
 {
 data |= 0x01;
 }
 else
 {
 data &= ~0x01;
 }
 i2c_scl_delay();
 }
 /* disable PTB6(SDA) input, enable PTB6(SDA) output */
 i2c_sda_input_off(sda1);
 i2c_sda_output(sda1);

 i2c_scl_low(); // set SCL to low
 return data;
}

void i2c_gpio_tick() 
{

}


bool I2C_readbyte(uint8_t *val)
{
    bool r = true;
    
    uint8_t data = 0;

    /* disable PTB6(SDA) output, enable PTB6(SDA) input */
    i2c_sda_output_off(sda1);
    i2c_sda_input(sda1);
    i2c_scl_delay(); // messo io
     /* write 8 bits data */
    for(uint8_t i=0;i<8;i++)
    {
        i2c_scl_low();
        i2c_scl_delay();
        /* Data must be held stable while SCL is high */
        i2c_scl_high();
        i2c_scl_delay(); // messo io
        i2c_scl_delay(); // messo io
        data = data<<1;
        /* read SDA data */
        if(i2c_sda_status(sda1) == embot::hw::gpio::State::SET)
        {
            data |= 0x01;
        }
        else
        {
            data &= ~0x01;
        }
        i2c_scl_delay();
    }
    /* disable PTB6(SDA) input, enable PTB6(SDA) output */
    i2c_sda_input_off(sda1);
    i2c_sda_output(sda1);

    i2c_scl_low(); // set SCL to low

    *val = data;
 
    return r;
}

bool I2C_init()
{
    bool r = true;
    
    embot::hw::gpio::init(scl, out);
    embot::hw::gpio::set(scl, embot::hw::gpio::State::SET);
    
    embot::hw::gpio::init(sda1, out);
    embot::hw::gpio::set(sda1, embot::hw::gpio::State::SET);

    return r;
}

bool I2C_ping(uint8_t adr)
{
    bool r = false;
    
    uint8_t wadr = adr | 0;
    
    i2c_start(sda1);
    
    r = i2c_write_byte(wadr);
    
    i2c_stop(sda1);
    
    return r;
}

uint8_t I2Cerror = 0;

uint8_t I2C_geterror()
{
    return I2Cerror;
}

bool I2C_read(uint8_t adr, size_t n, uint8_t *data)
{
    bool r = false;
    
    I2Cerror = 0;
    
    uint8_t wadr = adr | 1;
    i2c_start(sda1);
    
    r = i2c_write_byte(wadr);
    
    if(false == r)
    {
        I2Cerror = 1;
        return r;
    }
    
    for(uint8_t k=0; k<n; k++)
    {
        //data[k] = i2c_read_byte();
        
        I2C_readbyte(&data[k]);
        
        if(k == (n-1))
        {
            i2c_send_NACK(sda1);
            i2c_stop(sda1);
            break;            
        }
        else
        {
            i2c_send_ACK(sda1);
        }
        
        i2c_scl_delay();        
    }
    
    return r;
}


bool I2C_write(uint8_t adr, size_t n, uint8_t *data)
{
    bool r = false;
    
    I2Cerror = 0;
    
    uint8_t wadr = adr | 0;
    i2c_start(sda1);
    
    r = i2c_write_byte(wadr);
    
    if(false == r)
    {
        I2Cerror = 1;
        return r;
    }
    
    for(uint8_t k=0; k<n; k++)
    {       
        r = i2c_write_byte(data[k]);     
    }
  
    i2c_stop(sda1);   
    
    return r;
}

void i2c_send_ACK(embot::hw::GPIO sda)
{
    /* the 9th clock start */
    i2c_scl_low();
    /*send ACK signal */
    i2c_sda_low(sda1);
    i2c_scl_delay();
    i2c_scl_high();
    i2c_scl_delay();
    /* the 9th clock start */
    /* SCL hold low to wait */
    i2c_scl_low();
    i2c_sda_high(sda1);    
}

void i2c_send_NACK(embot::hw::GPIO sda)
{
    /* the 9th clock start*/
    i2c_scl_low();
    /*send NACK signal */
    i2c_sda_high(sda1);
    i2c_scl_delay();
    i2c_scl_high();
    i2c_scl_delay();
    /* the 9th clock start */
    /* SCL hold low to wait */
    i2c_scl_low();
    i2c_sda_high(sda1);    
}

#endif  // #if defined(TEST_I2C_EMULATED_embot)

#endif  // #if defined(TEST_I2C_EMULATED)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------






/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_app_skeleton_os_evthreadcan.h"

// --------------------------------------------------------------------------------------------------------------------
// config start

constexpr embot::app::theCANboardInfo::applicationInfo applInfo 
{ 
    embot::prot::can::versionOfAPPLICATION {1, 3 , 1},    
    embot::prot::can::versionOfCANPROTOCOL {2, 0}    
};

constexpr std::uint16_t threadIDLEstacksize = 512;
constexpr std::uint16_t threadINITstacksize = 2048;
constexpr std::uint16_t threadEVNTstacksize = 5*1024;
constexpr std::uint8_t maxINPcanframes = 16;
constexpr std::uint8_t maxOUTcanframes = 64;
constexpr embot::core::relTime threadEVNTtimeout = 50*embot::core::time1millisec;

static void *paramINIT = nullptr;
static void *paramIDLE = nullptr;
static void *paramERR = nullptr;
static void *paramEVNT = nullptr;

constexpr embot::os::theTimerManager::Config tmcfg {};
constexpr embot::os::theCallbackManager::Config cmcfg {};
    
    
static const embot::app::skeleton::os::basic::sysConfig syscfg { threadINITstacksize, paramINIT, threadIDLEstacksize, paramIDLE, paramERR, tmcfg, cmcfg};

static const embot::app::skeleton::os::evthreadcan::evtConfig evtcfg { threadEVNTstacksize, paramEVNT, threadEVNTtimeout};

static const embot::app::skeleton::os::evthreadcan::canConfig cancfg { maxINPcanframes, maxOUTcanframes };

// config end
// --------------------------------------------------------------------------------------------------------------------


class mySYS final : public embot::app::skeleton::os::evthreadcan::SYSTEMevtcan
{
public:
    mySYS(const embot::app::skeleton::os::basic::sysConfig &cfg) 
        : SYSTEMevtcan(cfg) {}
        
    void userdefOnIdle(embot::os::Thread *t, void* idleparam) const override;
    void userdefonOSerror(void *errparam) const override;
    void userdefInit_Extra(embot::os::EventThread* evtsk, void *initparam) const override;
};


class myEVT final : public embot::app::skeleton::os::evthreadcan::evThreadCAN
{
public:
    myEVT(const embot::app::skeleton::os::evthreadcan::evtConfig& ecfg, const embot::app::skeleton::os::evthreadcan::canConfig& ccfg, const embot::app::theCANboardInfo::applicationInfo& a) 
        : evThreadCAN(ecfg, ccfg, a) {}
        
    void userdefStartup(embot::os::Thread *t, void *param) const override;
    void userdefOnTimeout(embot::os::Thread *t, embot::os::EventMask eventmask, void *param) const override;
    void userdefOnEventRXcanframe(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &outframes) const override;
    void userdefOnEventANYother(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, std::vector<embot::prot::can::Frame> &outframes) const override;                   
};


static const mySYS mysys { syscfg };
static const myEVT myevt { evtcfg, cancfg, applInfo };
constexpr embot::app::skeleton::os::evthreadcan::CFG cfg{ &mysys, &myevt };

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    embot::app::skeleton::os::evthreadcan::run(cfg);
    for(;;);   
}


// - here is the tailoring of the board.

#include "embot_prot_can_analog_periodic.h"


#include "embot_hw_bsp_mtb4.h"

#include "embot_os_theScheduler.h"
#include "embot_app_theLEDmanager.h"
#include "embot_app_application_theCANparserBasic.h"

#include "embot_hw_can.h"


//WASEDA SKIN COARSE CHANGES
#include "if2hw_common.h"
#include "I2C_Multi_SDA.h"
unsigned char RM = { 0x4E };
unsigned char SB = { 0x1F };
unsigned char EX = { 0x80 };
//Hardcoded Board ID, need to get it from EEPROM //Ask MARCO 
volatile unsigned char _board_ID = 2;
//Pin assignment for the I2C interface
#define CH0     0


//Signatures
void InitWasedaSensor(unsigned char sensor_address);
void ReadAndTransferWasedaSensor(unsigned char sensor_address, std::vector<embot::prot::can::Frame> &replies,unsigned char sensor_address_index);
void WasedaSensors_Initialization(embot::os::Thread *tsk, embot::os::EventMask e);
void WasedaSensors_ReadAndTransfer(std::vector<embot::prot::can::Frame> &replies);

//Read Buffers
unsigned int Receive_Buffer_SDA0[8] = { 0 };
unsigned int Receive_Buffer_SDA1[8] = { 0 };
unsigned int Receive_Buffer_SDA2[8] = { 0 };
unsigned int Receive_Buffer_SDA3[8] = { 0 };
unsigned int Receive_Buffer_SDA4[8] = { 0 };

//Ask MARCO - Global variables ok?
//All possible Sensor Addresses
constexpr int SENSOR_ADDRESSES_NUM = 16;
constexpr unsigned char sensors_Addresses[SENSOR_ADDRESSES_NUM] = { 0x0C,0x0D,0x0E,0x0F,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x10 };

//Mapping for SDAs that tell us which sensor indexes are connected to which SDA
static std::uint16_t PresentSensorsMask = {0};

//Create 5 possible SDAs
constexpr int VIRTUAL_SDA_NUM = 5;


void InitWasedaSensor(unsigned char sensor_address)
{
   	int write_buffer[4];
	
    // Configure address 0
    unsigned int Z_SERIES = 0x00 << 7;      // 0: Default and recommended. 1: Enable all plates for Z-measurement
    unsigned int GAIN_SEL = 0x04 << 4;      // 0x07 is the highest response for the new skin // 0x00 is the lowest response for old skin:
    unsigned int HALLCONF = 0x00; 			// to achieve 500Hz
    
    // Configure address 1
    unsigned int TRIG_INT = 0x0 << 15;      // Puts TRIG_INT pin in TRIG mode when cleared
    unsigned int COMM_MODE = 0x3 << 13;     // 10b : only SPI. 11b : only I2C
    unsigned int WOC_DIFF = 0x0 << 12;      // Setup wake up on change, 0: off
    unsigned int EXT_TRIG = 0x0 << 11;      // External trigger, 0: not allowed
    unsigned int TCMP_EN = 0x0 << 10;     	// we use external drift compensation  // Drift compensation, 0: off 1: on
    //unsigned int BURST_SEL = 0xF << 6;      // Select xyzt
    unsigned int BURST_SEL = 0xE << 6; 		// only xyz
    unsigned int BURST_DATA_RATE = 0x00;    // Defines T_INTERVAL = BURST_DATA_RATE * 20ms

    // Configure address 2
    unsigned int OSR2 = 0x00 << 11;
    unsigned int RES_XYZ = 0x3F << 5;
    unsigned int DIG_FILT = 0x00 << 2; 		//to achieve 500Hz
    unsigned int OSR = 0x02; 				//Note: The chip will not work properly with the following combinations: OSR=0 and DIG_FILT=0, OSR=0 and DIG_FILT=1 and OSR=1 and DIG_FILT=0

    unsigned int data[7];

    data[0] = Z_SERIES | GAIN_SEL | HALLCONF;
    data[1] = TRIG_INT | COMM_MODE | WOC_DIFF | EXT_TRIG | TCMP_EN | BURST_SEL | BURST_DATA_RATE;
    data[2] = OSR2 | RES_XYZ | DIG_FILT | OSR;
        
    for (int j = 0; j < 3; j++) 
    {
        write_buffer[0] = 0x60;
        write_buffer[1] = (data[j] & 0xFF00) >> 8;
        write_buffer[2] = data[j] & 0x00FF;
        write_buffer[3] = j << 2;

        SendFourCommands_I2C_SkinWaseda(sensor_address, write_buffer[0], write_buffer[1], write_buffer[2], write_buffer[3]);
    }
    
    // Reset offset by configuring register address 4 - 6
    for (int i = 4; i < 7; i++) 
    {
        write_buffer[0] = 0x60;
        write_buffer[1] = 0x00;
        write_buffer[2] = 0x00;
        write_buffer[3] = i << 2;
        SendFourCommands_I2C_SkinWaseda(sensor_address, write_buffer[0], write_buffer[1], write_buffer[2], write_buffer[3]);
    }

//    // Old calibration part  - No need for further 'calibration'
//    for (int i = 0; i < 3; i++) 
//    {
//        write_buffer[0] = 0x60;
//        write_buffer[1] = (0x0000 & 0xFF00) >> 8;
//        write_buffer[2] = 0x0000 & 0x00FF;
//        write_buffer[3] = (i+4) << 2;

//        SendFourCommands_I2C_SkinWaseda(sensor_address, write_buffer[0], write_buffer[1], write_buffer[2], write_buffer[3]);
//    }
    
    //Start Burst - Return error state 		
    SendSingleCommand_I2C_SkinWaseda(sensor_address, SB, Receive_Buffer_SDA0, Receive_Buffer_SDA1, Receive_Buffer_SDA2, Receive_Buffer_SDA3, Receive_Buffer_SDA4, 1);
}

bool PingWasedaSensor(unsigned char sensor_address)
{
    //Reading 4 SDAs at the same time
    SendSingleCommand_I2C_SkinWaseda(sensor_address, RM, 
                                        Receive_Buffer_SDA0, 
                                        Receive_Buffer_SDA1, 
                                        Receive_Buffer_SDA2, 
                                        Receive_Buffer_SDA3,
                                        Receive_Buffer_SDA4,
                                        7);
    
    //Assign which buffer we will be reading, basing on the working SDA
    unsigned int* BufferToRead = nullptr;
    
    for (int i = 0; i < VIRTUAL_SDA_NUM; i++)  //Loop thru all virtual SDAs
    { 
        switch(i)
        {
            case 0: BufferToRead = Receive_Buffer_SDA0; break;
            case 1: BufferToRead = Receive_Buffer_SDA1; break;
            case 2: BufferToRead = Receive_Buffer_SDA2; break;
            case 3: BufferToRead = Receive_Buffer_SDA3; break;
            case 4: BufferToRead = Receive_Buffer_SDA4; break;
        }
        
        //Check BufferToRead's STATUS byte. If any SDA has this sensor, return true immediately. 
        if(BufferToRead[0] != 0xFF) 
           return true;
    }
    
    return false;
}

void ReadAndTransferWasedaSensor(unsigned char sensor_address, std::vector<embot::prot::can::Frame> &replies,unsigned char sensor_address_index)
{
    //Reading 4 SDAs at the same time
    SendSingleCommand_I2C_SkinWaseda(sensor_address, RM, 
                                        Receive_Buffer_SDA0, 
                                        Receive_Buffer_SDA1, 
                                        Receive_Buffer_SDA2, 
                                        Receive_Buffer_SDA3,
                                        Receive_Buffer_SDA4,
                                        7);
    
    //Assign which buffer we will be reading, basing on the working SDA
    unsigned int* BufferToRead = nullptr;

    //Read successful!
    //Prepare to send out 4 SDAs data
    for (int i = 0; i < VIRTUAL_SDA_NUM; i++)  //Loop thru all virtual SDAs
    { 
        switch(i)
        {
            case 0: BufferToRead = Receive_Buffer_SDA0; break;
            case 1: BufferToRead = Receive_Buffer_SDA1; break;
            case 2: BufferToRead = Receive_Buffer_SDA2; break;
            case 3: BufferToRead = Receive_Buffer_SDA3; break;
            case 4: BufferToRead = Receive_Buffer_SDA4; break;
        }
        
        //Prepare CAN bus
        embot::prot::can::analog::periodic::Message_SKINWASEDA_MEASURE::Info info;  
        
        //Porbably don't need this line
        info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
       
        //Assign info to send into CAN
        for (int i = 0; i < 7; i++) 
        {
            info.data[i] = BufferToRead[i];  //store: status byte[1], x[2], y[2], z[2] (= Total 7)
        }
        //Use last available byte to write the sensor I2C address. Redundant information as we can infer the address from the sensor_address_index. May be used for temp reading.
        info.data[7] = sensor_address;  

        //Only send messages with valid Status Byte (!= 0xFF)
        if(info.data[0] != 0xFF)
        {
            //Prepare CAN frame
            embot::prot::can::Frame frame;
            embot::prot::can::analog::periodic::Message_SKINWASEDA_MEASURE msg;  
            msg.load(info);
            msg.get(frame);
            
            _board_ID = embot::app::theCANboardInfo::getInstance().cachedCANaddress();
            //Set Frame Id as MTB id + SDA # + sensor index. iCub code should comment this out to use the standard communication protocols.
            //  | 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
            //  |   MTB Id   |     SDA  #    | Sensor Index  |                  
            frame.id = (_board_ID << 8) |  (i << 4) | sensor_address_index; 
            
#if 0
            //Transmit immediately 1 CAN message per sensor. 
            //TRANSMIT by CAN bus  immediately     
            //static volatile embot::hw::result_t r = embot::hw::resNOK;                
            embot::hw::can::put(embot::hw::CAN::one, frame);                                       
            embot::hw::can::transmit(embot::hw::CAN::one);  
#else    
            //Try to accummulate more CAN messages into the out buffer.
            //Accumulates 1 Sensor ID from all 4 channels.
            replies.push_back(frame);
#endif
        }
    }
}

void WasedaSensors_Initialization(embot::os::Thread *tsk, embot::os::EventMask e)
{
    //Initialize all possible sensors for all 4 virtual SDAs. Mark which sensor ids are connected to any sda line
    for (int j = 0; j < SENSOR_ADDRESSES_NUM; j++) //Loop thru all possible sensors connected
    { 
        InitWasedaSensor(sensors_Addresses[j]);

        //Try to Read from Sensor. If Status flag comes back, then the sensor is present in one of the SDA channels.
        if(PingWasedaSensor(sensors_Addresses[j]))
        {
            //set the PresentSensorsMask to flag sensor index that is connected to one of the SDA channels. We read all channels in parallel, so we just mark the sensor's connected. 
            embot::core::binary::bit::set(PresentSensorsMask,j);
        }
    }

    embot::os::Timer *ticktimer = new embot::os::Timer;
    embot::os::Action action;
    action.load(embot::os::EventToThread(e, tsk));

    //Waseda Sensors will be read on this frequency.
    embot::core::relTime txperiod = 1*embot::core::time1millisec; //Old value 1000 = 1 sec. 
    embot::os::Timer::Config cfg(txperiod, action, embot::os::Timer::Mode::forever);
    ticktimer->start(cfg);

    return;       
}

void WasedaSensors_ReadAndTransfer(std::vector<embot::prot::can::Frame> &replies)
{
    for (int j = 0; j < SENSOR_ADDRESSES_NUM; j++) //Loop thru all possible sensors connected
    { 
        //If current sensor id is connected to any sda, read it and send out CAN message
        if(embot::core::binary::bit::check(PresentSensorsMask, j))
        {
            ReadAndTransferWasedaSensor(sensors_Addresses[j], replies, j);
        }
    }    
}

constexpr embot::os::Event evTick = 0x00000001 << 1;

//constexpr embot::os::Event evSKINprocess = 0x00000001 << 1;
//constexpr embot::os::Event evIMUtick = 0x00000001 << 3;
//constexpr embot::os::Event evIMUdataready = 0x00000001 << 4;
//constexpr embot::os::Event evTHERMOtick = 0x00000001 << 5;
//constexpr embot::os::Event evTHERMOdataready = 0x00000001 << 6;




void mySYS::userdefOnIdle(embot::os::Thread *t, void* idleparam) const
{
    static int a = 0;
    a++;        
}

void mySYS::userdefonOSerror(void *errparam) const
{
    static int code = 0;
    embot::os::theScheduler::getInstance().getOSerror(code);
    for(;;);    
}


void mySYS::userdefInit_Extra(embot::os::EventThread* evtsk, void *initparam) const
{
    // inside the init task: put the init of many things ...  
    
    // led manager
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(embot::hw::LED::one).pulse(embot::core::time1second); 

    // init of can basic paser
    embot::app::application::theCANparserBasic::getInstance().initialise({});
        
//    // init agent of skin 
//    embot::app::application::theSkin &theskin = embot::app::application::theSkin::getInstance();
//    embot::app::application::theSkin::Config configskin;
//    configskin.tickevent = evSKINprocess;
//    configskin.totask = evtsk;
//    theskin.initialise(configskin);   
//    
//    // init canparser skin and link it to its agent
//    embot::app::application::theCANparserSkin &canparserskin = embot::app::application::theCANparserSkin::getInstance();
//    embot::app::application::theCANparserSkin::Config configparserskin { &theskin };
//    canparserskin.initialise(configparserskin);  
//                   
//    // init agent of imu
//    embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
//    embot::app::application::theIMU::Config configimu(embot::hw::bsp::mtb4::imuBOSCH, embot::hw::bsp::mtb4::imuBOSCHconfig, evIMUtick, evIMUdataready, evtsk);
//    theimu.initialise(configimu);

//    // init canparser imu and link it to its agent
//    embot::app::application::theCANparserIMU &canparserimu = embot::app::application::theCANparserIMU::getInstance();
//    embot::app::application::theCANparserIMU::Config configparserimu { &theimu };
//    canparserimu.initialise(configparserimu);   
//    
//    // init agent of thermo
//    embot::app::application::theTHERMO &thethermo = embot::app::application::theTHERMO::getInstance();
//    embot::app::application::theTHERMO::Config configthermo(embot::hw::bsp::mtb4::thermometer, embot::hw::bsp::mtb4::thermometerconfig, evTHERMOtick, evTHERMOdataready, evtsk);
//    thethermo.initialise(configthermo);  

//    // init canparser thermo and link it to its agent
//    embot::app::application::theCANparserTHERMO &canparserthermo = embot::app::application::theCANparserTHERMO::getInstance();
//    embot::app::application::theCANparserTHERMO::Config configparserthermo { &thethermo };
//    canparserthermo.initialise(configparserthermo);    

    WasedaSensors_Initialization(evtsk, evTick);
        
}

void myEVT::userdefStartup(embot::os::Thread *t, void *param) const
{
    // inside startup of evnt task: put the init of many things ...     
}


void myEVT::userdefOnTimeout(embot::os::Thread *t, embot::os::EventMask eventmask, void *param) const
{
    static uint32_t cnt = 0;
    cnt++;    
}


void myEVT::userdefOnEventRXcanframe(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &outframes) const
{        
    // process w/ the basic parser. if not recognised call the parsers specific of the board
    if(true == embot::app::application::theCANparserBasic::getInstance().process(frame, outframes))
    {                   
    }
//    if(true == embot::app::application::theCANparserSkin::getInstance().process(frame, outframes))
//    {               
//    }
//    else if(true == embot::app::application::theCANparserIMU::getInstance().process(frame, outframes))
//    {               
//    }
//    else if(true == embot::app::application::theCANparserTHERMO::getInstance().process(frame, outframes))
//    {               
//    }   
}

void myEVT::userdefOnEventANYother(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, std::vector<embot::prot::can::Frame> &outframes) const
{
    
    if(true == embot::core::binary::mask::check(eventmask, evTick))
    {
        WasedaSensors_ReadAndTransfer(outframes);        
    }
    
//    if(true == embot::core::binary::mask::check(eventmask, evSKINprocess))
//    {
//        embot::app::application::theSkin &theskin = embot::app::application::theSkin::getInstance();
//        theskin.tick(outframes);
//        
//        // we operate on the skin triangles by calling a skin.tick(outframes);
//        // the evSKINprocess is emitted  by:
//        // 1. a periodic timer started at the reception of a specific message.


//        // the .tick(outframes) will do whatever it needs to do and it may emit some 
//        // can frames for transmission. the can frames can be up to 16x2 = 32.
//        // hence, how many packets? max of replies = 8 + max of broadcast = 32 --> 40.
//        
//    }
//        
//    if(true == embot::core::binary::mask::check(eventmask, evIMUtick))
//    {        
//        embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
//        theimu.tick(outframes);        
//    }   
//    
//    if(true == embot::core::binary::mask::check(eventmask, evIMUdataready))
//    {        
//        embot::app::application::theIMU &theimu = embot::app::application::theIMU::getInstance();
//        theimu.processdata(outframes);        
//    }    

//    if(true == embot::core::binary::mask::check(eventmask, evTHERMOtick))
//    {        
//        embot::app::application::theTHERMO &thethermo = embot::app::application::theTHERMO::getInstance();
//        thethermo.tick(outframes);        
//    }   
//    
//    if(true == embot::core::binary::mask::check(eventmask, evTHERMOdataready))
//    {        
//        embot::app::application::theTHERMO &thethermo = embot::app::application::theTHERMO::getInstance();
//        thethermo.processdata(outframes);        
//    }
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------





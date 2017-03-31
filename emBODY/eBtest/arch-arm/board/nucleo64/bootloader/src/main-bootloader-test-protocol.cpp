

#include "EOtheLEDpulser.h"

#include "embot_app_theCANboardInfo.h"

#include "embot.h"

#include "stm32hal.h" // to see bsp_led_init etc

#include "embot_i2h.h"
#include "embot_hw.h"

#include "embot_hw_FlashStorage.h"
#include "embot_sys_theStorage.h"

#include "embot_app_theBootloader.h"

#include "embot_hw_FlashBurner.h"

#include "embot_app_bootloader_theCANparser.h"


struct ActivityParam
{
    uint32_t blinkingperiod;
};

static void bl_activity(void* param);

void periodic_activity(embot::sys::Task *tsk, void *param);

static ActivityParam activity_param = {0};

//static embot::hw::FlashBurner *fb = nullptr;

#undef TEST_BL_CANFRAME

#define FINAL_BOOTLOADER
#define TEST_FINAL_BOOTLOADER

int main(void)
{ 
    embot::app::theBootloader & bootloader = embot::app::theBootloader::getInstance();

    std::uint32_t param = 0;    
    embot::sys::theJumper::Command cmd = bootloader.getcommand(param);
           
    embot::app::theBootloader::Config config;
    config.userdeflauncher.callback = bl_activity;
    config.userdeflauncher.arg = &activity_param;
    activity_param.blinkingperiod = 1*EOK_reltime100ms;
    config.countdown = 5*embot::common::time1second;
    
    switch(cmd)
    {
        case embot::sys::theJumper::Command::none:
        {   // we dont change what we want to do: we exec for at 5 seconds
        } break;
        
        case embot::sys::theJumper::Command::stay:
        {   // we have received a command from the previous running process to stay in here forever
            config.countdown = 0;
            activity_param.blinkingperiod = 5*EOK_reltime100ms;
        } break; 
        
        case embot::sys::theJumper::Command::jump:
        {   // we have received command to jump to an address
            bootloader.jump(param); 
            // if we cannot jump to the specified address ... we stay in here forever
            config.countdown = 0;
            activity_param.blinkingperiod = 5*EOK_reltime10ms;
        } break;          
    }


#if defined(TEST_BL_CANFRAME)
    config.countdown = 0;
    activity_param.blinkingperiod = 5*EOK_reltime100ms;
#endif
    
    bootloader.execute(config);
    
    for(;;);
    
}


#if defined(FINAL_BOOTLOADER)

#if defined(TEST_FINAL_BOOTLOADER)

bool get_canframe(embot::hw::can::Frame &frame);
#endif


void onevent(embot::sys::Task *t, embot::common::Event evt, void *p)
{    
//    if(0x00000001 == evt)
    {
        
        // 1. get the frame ... ehi, use the button. or the timeout
        
        embot::hw::can::Frame frame;
        embot::hw::can::Frame frameout;
        embot::app::bootloader::theCANparser &canparser = embot::app::bootloader::theCANparser::getInstance();
        
        bool rx = get_canframe(frame);
        if(true == rx)
        {
            if(true == canparser.process(frame, frameout))
            {
                // send the frameout
            }
        }
   
        
    }
    
}

static void bl_activity(void* param)
{
    
    ActivityParam* pp = (ActivityParam*) param;
    
    uint32_t period = 0;
    
    if(nullptr != pp)
    {
        period = pp->blinkingperiod;
    }
    
    eOledpulser_cfg_t ledconfig = {0};
    
    ledconfig.led_enable_mask   = (1 << eo_ledpulser_led_zero);
    ledconfig.led_init          = reinterpret_cast<eOint8_fp_uint8_cvoidp_t>(embot::hw::led::init_legacy);
    ledconfig.led_on            = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::led::on); 
    ledconfig.led_off           = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::led::off);
    ledconfig.led_toggle        = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::led::toggle);
    
    eo_ledpulser_Initialise(&ledconfig);    

    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero, period, 0);  

    
    embot::hw::button::init(embot::hw::button::BTN::zero);

    // also start a periodic task which checks .... if button is pressed .... so far for test only
    embot::sys::PeriodicTask *taskper = new embot::sys::PeriodicTask;        
    taskper->init(nullptr, periodic_activity, 1024, 30, 100*1000, nullptr);
    
    // init the storage object ....
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    
    canbrdinfo.synch(embot::app::theCANboardInfo::Type::mtb3, 1);
    
    uint8_t adr = canbrdinfo.getCANaddress();
    adr = adr;
    if(1 == adr)
    {
        canbrdinfo.setCANaddress(2);
    }
    
    
    // start can activity with a decent rx buffer (8 is fine) and a decent tx buffer (8 is fine).
    // set a given event at rx of can frame, say 0x00000001.
    #warning TODO: start can services.
    
    // start the principal event-based task with a good stack size. it must be enough to host te processing of can frames.
    // the action is triggered if an event of rx can frame is received, say 0x00000001.
    
    // in its inside we retrieve a can frame.
    // we call the bool tx = obj.process(rxframe, txframe)
    // if tx is true we transmit.
    
    embot::app::bootloader::theCANparser &canparser = embot::app::bootloader::theCANparser::getInstance();
    embot::app::bootloader::theCANparser::Config config;
    canparser.initialise(config);
    
    
    embot::sys::EventTask* task = new embot::sys::EventTask;
    
    task->init(nullptr, onevent, 4*1024, 200, 100*1000, nullptr, nullptr);
    
}

#else

static void bl_activity(void* param)
{
    
    ActivityParam* pp = (ActivityParam*) param;
    
    uint32_t period = 0;
    
    if(nullptr != pp)
    {
        period = pp->blinkingperiod;
    }
    
    eOledpulser_cfg_t ledconfig = {0};
    
    ledconfig.led_enable_mask   = (1 << eo_ledpulser_led_zero);
    ledconfig.led_init          = reinterpret_cast<eOint8_fp_uint8_cvoidp_t>(embot::hw::led::init_legacy);
    ledconfig.led_on            = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::led::on); 
    ledconfig.led_off           = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::led::off);
    ledconfig.led_toggle        = reinterpret_cast<eOint8_fp_uint8_t>(embot::hw::led::toggle);
    
    eo_ledpulser_Initialise(&ledconfig);    

    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero, period, 0);  

    
    embot::hw::button::init(embot::hw::button::BTN::zero);

    // also start a periodic task which checks ....
    embot::sys::PeriodicTask *taskper = new embot::sys::PeriodicTask;        
    taskper->init(nullptr, periodic_activity, 1024, 30, 100*1000, nullptr);
    
    // init the storage object ....
    
    embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();
    
    canbrdinfo.synch(embot::app::theCANboardInfo::Type::mtb3, 1);
    
    uint8_t adr = canbrdinfo.getCANaddress();
    adr = adr;
    if(1 == adr)
    {
        canbrdinfo.setCANaddress(2);
    }
    
    
}
#endif


#if defined(TEST_BL_CANFRAME) || defined(TEST_FINAL_BOOTLOADER)

#include "embot_app_canprotocol.h"


static embot::hw::can::Frame s_canframe;

bool get_canframe(embot::hw::can::Frame &frame)
{
    static const uint32_t start = 20;
    
    static uint32_t cnt = 0;
    
    static uint32_t cc = 0;
    
    bool ret = true;
    
    static bool stop = false;
    
    static bool itisover = false;
    
    if(itisover)
    {
        return false;
    }
    
    std::uint8_t data[8] = {0};
    
    
    if(cnt < start)
    {
        // test the additionalinfo        
        if(cnt < 16)
        {
            std::uint8_t counter = cnt % 8;
            std::uint8_t adder = cnt / 8;
            adder *= 128;
            const static char info32[32] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
            // SET_ADDITIONAL_INFO   
                
            if(cnt >= 8)
            {
                adder = adder;
            }
            std::uint8_t dd[5] = {0};
            dd[0] = counter; dd[1] = adder+info32[4*counter+0]; dd[2] = adder+info32[4*counter+1]; dd[3] = adder+info32[4*counter+2]; dd[4] = adder+info32[4*counter+3];   
            embot::app::canprotocol::frame_set_sender(frame, 0); // the sender is eth board
            embot::app::canprotocol::frame_set_clascmddestinationdata(frame, 
                                        embot::app::canprotocol::Clas::pollingMotorControl, static_cast<std::uint8_t>(embot::app::canprotocol::mcpollCMD::SET_ADDITIONAL_INFO),
                                        6, // to the board
                                        dd, 5);
            embot::app::canprotocol::frame_set_size(frame, 6);     
        }
        else if(16 == cnt)
        {

            embot::app::canprotocol::frame_set_sender(frame, 0); // the sender is eth board
            embot::app::canprotocol::frame_set_clascmddestinationdata(frame, 
                                        embot::app::canprotocol::Clas::pollingMotorControl, static_cast<std::uint8_t>(embot::app::canprotocol::mcpollCMD::GET_ADDITIONAL_INFO),
                                        6, // to the board
                                        nullptr, 0);
            embot::app::canprotocol::frame_set_size(frame, 1);              
            
        }
        else 
        {
            ret = false;  
        }    
    
              
    }    
    else if((start+0) == cnt)
    {
        // BROADCAST        
        embot::app::canprotocol::frame_set_sender(frame, 0); // the sender is eth board
        embot::app::canprotocol::frame_set_clascmddestinationdata(frame, 
                                    embot::app::canprotocol::Clas::bootloader, static_cast<std::uint8_t>(embot::app::canprotocol::bldrCMD::BROADCAST),
                                    0xf, // destination broadcast
                                    nullptr, 0);
        embot::app::canprotocol::frame_set_size(frame, 1); 
    }
    else if((start+1) == cnt)
    {
        // BOARD
        data[0] = 0;
        embot::app::canprotocol::frame_set_sender(frame, 0); // the sender is eth board
        embot::app::canprotocol::frame_set_clascmddestinationdata(frame, 
                                    embot::app::canprotocol::Clas::bootloader, static_cast<std::uint8_t>(embot::app::canprotocol::bldrCMD::BOARD),
                                    0x4, // destination the specific board
                                    data, 1);  
        embot::app::canprotocol::frame_set_size(frame, 2);                                     
    }
    else if((cnt > (start+1)) && (!stop))
    {
        if(0 == (cc%4))
        {
            // ADDRESS
            data[0] = 16;
            data[1] = cc*4; data[2] = 0x00; data[3] = 0; data[4] = 0x02; data[5] = 0x08;
            embot::app::canprotocol::frame_set_sender(frame, 0); // the sender is eth board
            embot::app::canprotocol::frame_set_clascmddestinationdata(frame, 
                                        embot::app::canprotocol::Clas::bootloader, static_cast<std::uint8_t>(embot::app::canprotocol::bldrCMD::ADDRESS),
                                        0xf, // destination broadcast
                                        data, 6);
            embot::app::canprotocol::frame_set_size(frame, 7); 
        } 
        else
        {
            uint8_t len = (3 == (cc%4)) ? 4 : 6;
            // DATA
            data[0] = 0x80+cc; data[1] = 0x81+cc; data[2] = 0x82+cc; data[3] = 0x83+cc; data[4] = 0x84+cc; data[5] = 0x85+cc;
            embot::app::canprotocol::frame_set_sender(frame, 0); // the sender is eth board
            embot::app::canprotocol::frame_set_clascmddestinationdata(frame, 
                                        embot::app::canprotocol::Clas::bootloader, static_cast<std::uint8_t>(embot::app::canprotocol::bldrCMD::DATA),
                                        0xf, // destination broadcast
                                        data, len); 
            embot::app::canprotocol::frame_set_size(frame, len+1);                                         
        }

        cc++;
        
        if(cc >= 12)
        {
            stop = true;
        }
    }
    else if((cnt > (start+1)) && (stop))
    {
        static bool first = true;
        if(first)
        {
            // START
            data[0] = 0; data[1] = 0; data[2] = 0; data[3] = 0; 
            embot::app::canprotocol::frame_set_sender(frame, 0); // the sender is eth board
            embot::app::canprotocol::frame_set_clascmddestinationdata(frame, 
                                        embot::app::canprotocol::Clas::bootloader, static_cast<std::uint8_t>(embot::app::canprotocol::bldrCMD::START),
                                        0xf, // destination broadcast
                                        data, 4);

            embot::app::canprotocol::frame_set_size(frame, 5);                                
        
            first =  false;
        }
        else
        {
            // END
            
            embot::app::canprotocol::frame_set_sender(frame, 0); // the sender is eth board
            embot::app::canprotocol::frame_set_clascmddestinationdata(frame, 
                                        embot::app::canprotocol::Clas::bootloader, static_cast<std::uint8_t>(embot::app::canprotocol::bldrCMD::END),
                                        0xf, // destination broadcast
                                        data, 0);
            embot::app::canprotocol::frame_set_size(frame, 1);                                        
            itisover = true;
        }
        
        
    }
    
    
    
    
    cnt++;
    
    return ret;
}

static std::uint8_t myaddress = 6;

static std::int32_t datalen = 0;

bool s_process_broadcast(const embot::hw::can::Frame &frame, embot::hw::can::Frame &reply)
{
    uint8_t sender = embot::app::canprotocol::frame2sender(frame);
    sender = sender;  

    embot::app::canprotocol::Message_bldr_BROADCAST msg;

    msg.load(frame);
    
    std::uint8_t s = msg.data.sizeofdatainframe;
    s = s;
    bool ret = false;   

    embot::app::canprotocol::Message_bldr_BROADCAST::ReplyInfo replyinfo;
    replyinfo.board = embot::app::canprotocol::Board::mtb4;
    replyinfo.firmware.major = 1;
    replyinfo.firmware.minor = 0;
    replyinfo.firmware.build = 255;
        
    ret = msg.reply(reply, myaddress, replyinfo);
    return ret;    
}

bool s_process_board(const embot::hw::can::Frame &frame, embot::hw::can::Frame &reply)
{
    uint8_t sender = embot::app::canprotocol::frame2sender(frame);
    sender = sender;    
    
    embot::app::canprotocol::Message_bldr_BOARD msg;

    msg.load(frame);
    
    std::uint8_t s = msg.data.sizeofdatainframe;
    s = s; 
    bool ret = false;    
    ret = msg.reply(reply, myaddress);
    return ret;      
}

bool s_process_address(const embot::hw::can::Frame &frame, embot::hw::can::Frame &reply)
{
    uint8_t sender = embot::app::canprotocol::frame2sender(frame);
    sender = sender;    
    
    embot::app::canprotocol::Message_bldr_ADDRESS msg;

    msg.load(frame);
    
    datalen = msg.info.datalen;
    
    std::uint8_t s = msg.data.sizeofdatainframe;
    s = s;   
    bool ret = false;    
    ret = msg.reply();
    return ret;      
}


bool s_process_start(const embot::hw::can::Frame &frame, embot::hw::can::Frame &reply)
{
    uint8_t sender = embot::app::canprotocol::frame2sender(frame);
    sender = sender;    
    
    embot::app::canprotocol::Message_bldr_START msg;

    msg.load(frame);
    
    std::uint8_t s = msg.data.sizeofdatainframe;
    s = s;  
    bool ret = false;    
    ret = msg.reply(reply, myaddress, true);
    return ret;      
}

bool s_process_data(const embot::hw::can::Frame &frame, embot::hw::can::Frame &reply)
{
    uint8_t sender = embot::app::canprotocol::frame2sender(frame);
    sender = sender;    
    
    embot::app::canprotocol::Message_bldr_DATA msg;

    msg.load(frame);
    
    
    
    std::uint8_t s = msg.data.sizeofdatainframe;
    s = s; 
    bool ret = false;    
    ret = msg.reply(reply, myaddress, true);
    
    // decrement datalen
    datalen -= msg.info.size;
    
    if(datalen > 0)
    {
        ret = false;
    }
    return ret;          
}

bool s_process_end(const embot::hw::can::Frame &frame, embot::hw::can::Frame &reply)
{
    uint8_t sender = embot::app::canprotocol::frame2sender(frame);
    sender = sender;    
    
    embot::app::canprotocol::Message_bldr_END msg;

    msg.load(frame);
    
    std::uint8_t s = msg.data.sizeofdatainframe;
    s = s; 
    bool ret = false;    
    ret = msg.reply(reply, myaddress, true);
    return ret;          
}



bool s_process_set_additional_info(const embot::hw::can::Frame &frame, embot::hw::can::Frame &reply)
{
    uint8_t sender = embot::app::canprotocol::frame2sender(frame);
    sender = sender;    
    
    embot::app::canprotocol::Message_mcpoll_SET_ADDITIONAL_INFO2 msg;

    msg.load(frame);
    
    std::uint8_t s = msg.data.sizeofdatainframe;
    s = s; 
    bool ret = false;    
    ret = msg.reply();
    return ret;          
}


bool s_process_get_additional_info(const embot::hw::can::Frame &frame, embot::hw::can::Frame &reply)
{
    uint8_t sender = embot::app::canprotocol::frame2sender(frame);
    sender = sender;   

    embot::app::canprotocol::Message_mcpoll_GET_ADDITIONAL_INFO::ReplyInfo rep;
    for(int j=0; j<32; j++)
    {
        rep.info32[j] = j+1;
    }
    
    embot::app::canprotocol::Message_mcpoll_GET_ADDITIONAL_INFO msg;

    msg.load(frame);
    
    std::uint8_t s = msg.data.sizeofdatainframe;
    s = s; 
    bool ret = false;    
    std::uint8_t num = msg.numberofreplies();
    for(int i=0; i<num; i++)
    {
        ret = msg.reply(reply, 6, rep);
    }
    
    return ret;          
}


void periodic_activity(embot::sys::Task *tsk, void *param)
{
    static uint32_t x = 0;
    if(true == embot::hw::button::pressed(embot::hw::button::BTN::zero))
    {
        x++;
        // we stop countdown and we stay in bootloader w/ reduced frequency of ....
        x = x; 
        
        // get canframe.
        // get its clas and cmd.
        // morph the canframe into a proper message. look at its content.
        embot::hw::can::Frame frame;
        embot::hw::can::Frame reply;
        
        bool transmit = false;
        if(true == get_canframe(frame))
        {
            // ok, we use the frame.
            
            embot::app::canprotocol::Clas cls = embot::app::canprotocol::frame2clas(frame);
            std::uint8_t cm = embot::app::canprotocol::frame2cmd(frame);
            
            if(embot::app::canprotocol::Clas::bootloader == cls)
            {
            
                embot::app::canprotocol::bldrCMD cmd = static_cast<embot::app::canprotocol::bldrCMD>(cm);
                
                switch(cmd)
                {
                    case embot::app::canprotocol::bldrCMD::BROADCAST:
                    {
                        transmit = s_process_broadcast(frame, reply);
                    } break;
                    
                    case embot::app::canprotocol::bldrCMD::BOARD:
                    {
                        transmit = s_process_board(frame, reply);
                    } break;  

                    case embot::app::canprotocol::bldrCMD::ADDRESS:
                    {
                        transmit = s_process_address(frame, reply);
                    } break;    

                    case embot::app::canprotocol::bldrCMD::START:
                    {
                        transmit = s_process_start(frame, reply);
                    } break;  

                    case embot::app::canprotocol::bldrCMD::DATA:
                    {
                        transmit = s_process_data(frame, reply);
                    } break;  

                    case embot::app::canprotocol::bldrCMD::END:
                    {
                        transmit = s_process_end(frame, reply);
                    } break;                
                }
            }
            else if(embot::app::canprotocol::Clas::pollingMotorControl == cls)
            {
                
                embot::app::canprotocol::mcpollCMD cmd = static_cast<embot::app::canprotocol::mcpollCMD>(cm);
        
                if(embot::app::canprotocol::mcpollCMD::SET_ADDITIONAL_INFO == cmd)
                {
                    transmit = s_process_set_additional_info(frame, reply);
                }
                else if(embot::app::canprotocol::mcpollCMD::GET_ADDITIONAL_INFO == cmd)
                {
                    transmit = s_process_get_additional_info(frame, reply);
                }
                
            }
            
            if(true == transmit)
            {
                static std::uint32_t xx = 0;
                xx++;                
            }
            
            
        }
      
    }    
}


#else

void periodic_activity(embot::sys::Task *tsk, void *param)
{
    static uint32_t x = 0;
    if(true == embot::hw::button::pressed(embot::hw::button::BTN::zero))
    {
        embot::app::theBootloader & bootloader = embot::app::theBootloader::getInstance();
        bootloader.stopcountdown();
        
        eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero, 10*EOK_reltime100ms, 0); 
        
        x++;
        // we stop countdown and we stay in bootloader w/ reduced frequency of ....
        x = x; 

#if 0        
        static std::uint8_t data[40] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};
        static std::uint32_t size = 6;
        static std::uint32_t adr = embot::hw::sys::addressOfApplication;
        static std::uint8_t *dd = data;
        
        if(1 == x)
        {
            size = 6;
            fb->add(adr, size, dd);    
            adr += size;  
            dd += size;
        }
        else if(2 == x)
        {
            size = 6;  
            fb->add(adr, size, dd);    
            adr += size;  
            dd += size;            
        }
        else if(3 == x)
        {
            size = 4; 
            fb->add(adr, size, dd);    
            adr = embot::hw::sys::addressOfApplication+2048;  
            dd += size;                        
        }
        else if(4 == x)
        {
            size = 6;
            fb->add(adr, size, dd);    
            adr += size;  
            dd += size;
            size = 6;  
            fb->add(adr, size, dd);    
            adr += size;  
            dd += size;    
            size = 4; 
            fb->add(adr, size, dd); 
            adr = embot::hw::sys::addressOfApplication+2048;  
            dd += size;             
        }
        
#endif        
    }    
}

#endif

///



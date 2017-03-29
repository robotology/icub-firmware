

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

struct ActivityParam
{
    uint32_t blinkingperiod;
};

static void bl_activity(void* param);

void periodic_activity(embot::sys::Task *tsk, void *param);

static ActivityParam activity_param = {0};

//static embot::hw::FlashBurner *fb = nullptr;

#define TEST_BL_CANFRAME

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
    
    //fb = new embot::hw::FlashBurner;
        
}



#if defined(TEST_BL_CANFRAME)

#include "embot_app_canprotocol.h"


static embot::hw::can::Frame s_canframe;

bool get_canframe(embot::hw::can::Frame &frame)
{
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
    
    if(0 == cnt)
    {
        // BROADCAST        
        embot::app::canprotocol::frame_set_sender(frame, 0); // the sender is eth board
        embot::app::canprotocol::frame_set_clascmddestinationdata(frame, 
                                    embot::app::canprotocol::Clas::bootloader, static_cast<std::uint8_t>(embot::app::canprotocol::bldrCMD::BROADCAST),
                                    0xf, // destination broadcast
                                    nullptr, 0);
        embot::app::canprotocol::frame_set_size(frame, 1); 
    }
    else if(1 == cnt)
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
    else if(!stop)
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
    else if(stop)
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
    replyinfo.major = 1;
    replyinfo.minor = 0;
    replyinfo.build = 255;
        
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



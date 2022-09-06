
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_skeleton_os_bootloader.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_core.h"
#include "embot_os_Thread.h"
#include "embot_app_theBootloader.h"
#include "embot_app_bootloader_theCANparser.h"
#include "embot_app_theLEDmanager.h"
#include "embot_hw_can.h"

#include "embot_hw_si7051.h"
#include "embot_hw_bno055.h"
#include "embot_hw_gpio.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace app { namespace skeleton { namespace os { namespace bootloader {
            
    struct ActivityParam
    {
        static const embot::core::relTime BlinkFastPeriod = 200*embot::core::time1millisec;
        static const embot::core::relTime BlinkSlowPeriod = 500*embot::core::time1millisec;
        static const embot::core::relTime BlinkMadlyPeriod = 50*embot::core::time1millisec;    
        
        embot::core::relTime blinkingperiod {BlinkFastPeriod};
        const embot::app::theCANboardInfo::bootloaderInfo *info {nullptr};
        
        ActivityParam() = default;
        ActivityParam(embot::core::relTime ti, const embot::app::theCANboardInfo::bootloaderInfo *in) : blinkingperiod(ti), info(in) {}
    };


}}}}}


namespace embot { namespace app { namespace skeleton { namespace os { namespace bootloader {
    
    constexpr embot::core::relTime timebeforeJump = 5 * embot::core::time1second;
    void activity_function(void* param);
    ActivityParam activity_param { ActivityParam::BlinkFastPeriod, nullptr};
    

    [[noreturn]] void run(const embot::app::theCANboardInfo::bootloaderInfo &info)
    {

        embot::app::theBootloader & bootloader = embot::app::theBootloader::getInstance();
            
//        embot::app::theBootloader::evalRes res = bootloader.eval();
//        
//        // if in here, it means that we must execute. let's fill the embot::app::theBootloader::Config according to res
//        activity_param.info = &info; // i copy into param the info about teh board
        embot::app::theBootloader::Config config { {activity_function, &activity_param}, timebeforeJump };
//            
//        switch(res)
//        {       
//            case embot::app::theBootloader::evalRes::jumpaftercountdown:
//            {
//                config.countdown = timebeforeJump;
//                activity_param.blinkingperiod = ActivityParam::BlinkFastPeriod;            
//            } break;

//            case embot::app::theBootloader::evalRes::stayforever:
//            {
//                config.countdown = 0;
//                activity_param.blinkingperiod = ActivityParam::BlinkSlowPeriod;            
//            } break;
//            
//            case embot::app::theBootloader::evalRes::jumpfailed:        
//            case embot::app::theBootloader::evalRes::unexected:
//            default:
//            {
//                config.countdown = 0;
//                activity_param.blinkingperiod = ActivityParam::BlinkMadlyPeriod;            
//            } break;
//        }
        config.countdown = 0;
        // and now we execute. this function is [[noreturn]]
        // it inits the hw::bsp and then starts the scheduler. 
        // the OS init thread will: start timer manager and callback manager, and finally call activity_function(&activity_param).
        // 
        bootloader.execute(config);
    
        for(;;);
    }

  
}}}}}




namespace embot { namespace app { namespace skeleton { namespace os { namespace bootloader {
    
		const uint8_t  Firmware_vers = 3;
		const uint8_t  Revision_vers = 0;
		const uint8_t  Build_number  = 0;
	
    constexpr std::uint8_t maxINPcanframes = 16;
    constexpr std::uint8_t maxOUTcanframes = 32;
    constexpr embot::os::Event evRXcanframe = 0x00000001;

    static void eventhread_init(embot::os::Thread *t, void *p);
    static void eventhread_onevent(embot::os::Thread *t, embot::os::Event evt, void *p);
    
    static void alerteventhread(void *arg);

    static std::vector<embot::prot::can::Frame> outframes;


    void activity_function(void* param)
    {
        ActivityParam* pp = reinterpret_cast<ActivityParam*>(param);    
        
        // manage the basic canboardinfo
        embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();   
        canbrdinfo.synch(*activity_param.info);    
        
        // manage the led blinking period
        embot::core::relTime period = 0;    
        if(nullptr != pp)
        {
            period = pp->blinkingperiod;
        }    

        // start the led pulser
			  static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one, embot::hw::LED::two};  
        embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
        theleds.init(allleds);    
        theleds.get(embot::hw::LED::one).on();
			  theleds.get(embot::hw::LED::two).on(); 
           

        // start thread waiting for can messages.  
        const embot::core::relTime waitEventTimeout = 50*1000; //50*1000; //5*1000*1000;            
        embot::os::EventThread::Config configEV;    
        configEV.startup = eventhread_init;
        configEV.onevent = eventhread_onevent;
        configEV.param = nullptr;
        configEV.stacksize = 4*1024;
        configEV.priority = embot::os::Priority::high40;
        configEV.timeout = waitEventTimeout;
        
        embot::os::EventThread* eventhread = new embot::os::EventThread; 
        eventhread->start(configEV);
            
        // start the canparser
        embot::app::bootloader::theCANparser &canparser = embot::app::bootloader::theCANparser::getInstance();
        embot::app::bootloader::theCANparser::Config config {embot::prot::can::Process::bootloader};
        canparser.initialise(config);  

        // finally init can. it will be enabled only inside teh startup() of the eventhread
        embot::hw::result_t r = embot::hw::resNOK;
        embot::hw::can::Config canconfig; 
        canconfig.rxcapacity = maxINPcanframes;
        canconfig.txcapacity = maxOUTcanframes;
        canconfig.onrxframe = embot::core::Callback(alerteventhread, eventhread); 
        embot::hw::can::init(embot::hw::CAN::one, canconfig);
        //embot::hw::can::setfilters(embot::hw::CAN::one, embot::app::theCANboardInfo::getInstance().getCANaddress());  // removed for test purpose (no filters)
    }
        
    static void alerteventhread(void *arg)
    {
        embot::os::EventThread* evtsk = reinterpret_cast<embot::os::EventThread*>(arg);
        if(nullptr != evtsk)
        {
            evtsk->setEvent(evRXcanframe);
        }
    }

    static void eventhread_init(embot::os::Thread *t, void *p)
    {
        embot::hw::result_t r = embot::hw::can::enable(embot::hw::CAN::one);  
        r = r;         
        outframes.reserve(maxOUTcanframes);
    }

		embot::hw::can::Frame canframe;

			
		void resetCanFrame()	{
		
						
			canframe.id = 1;
			canframe.size = 8;
			canframe.data[0] = 0xFF;
			canframe.data[1] = 0xFF;
			canframe.data[2] = 0xFF;
			canframe.data[3] = 0xFF;
			canframe.data[4] = 0xFF;
			canframe.data[5] = 0xFF;
			canframe.data[6] = 0xFF;
			canframe.data[7] = 0xFF;
			
			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);

		}
	
    void getFirmwareVersion()
    {
			embot::hw::can::Frame canframe;
						
			canframe.id = 0x551;         ;
			canframe.size = 8;
			canframe.data[0] = Firmware_vers;
			canframe.data[1] = Revision_vers;
			canframe.data[2] = Build_number;
				
			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);			
			
    }
		
		void testLeds(uint8_t on){
			 static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one, embot::hw::LED::two};  
        embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
        theleds.init(allleds);    
				
			  switch(on){
					case 0: 
						theleds.get(embot::hw::LED::one).off();
						theleds.get(embot::hw::LED::two).off();
						break;
					case 1: 
						theleds.get(embot::hw::LED::one).on();
						theleds.get(embot::hw::LED::two).on();
						break;
					default: break;
				}
			 
		}
	

		constexpr embot::hw::SI7051 thermometer = embot::hw::SI7051::one;
		constexpr embot::hw::si7051::Config thermometerconfig = {};
  	embot::hw::si7051::Temperature temp {0};
    volatile int i = 0;
		
		void testTemperature(uint8_t tempIN){
			
			embot::hw::can::Frame canframe;
			static bool init = false;
			
			if(!init) {
				embot::hw::si7051::init(thermometer, thermometerconfig);
				init = true;
			}
				
			embot::hw::si7051::acquisition(embot::hw::SI7051::one, {});
			for(;;)
			{
			if(true == embot::hw::si7051::operationdone(embot::hw::SI7051::one))
			{
			break;
			}
			}
			embot::hw::si7051::read(thermometer, temp);

			temp = temp/10;
			
						
			canframe.id = 0x551;         ;
			canframe.size = 8;
			canframe.data[1] = temp;
			canframe.data[2] = tempIN;
			
			if(temp < tempIN - ((tempIN*30)/100) || temp > tempIN +((tempIN*30)/100)) canframe.data[0] = 0xBB; // fail		 
			else canframe.data[0] = 0xAA; // pass		 
			
			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);		
			
			i++;
		
		}
		

		constexpr embot::hw::BNO055 imu = embot::hw::BNO055::one;
		constexpr embot::hw::bno055::Config imuconfig = {};
		embot::hw::bno055::Info info;
		char chip_id;
		
		static void testBNO055(){
				embot::hw::can::Frame canframe;
				static bool init = false;
				
				if(!init) {
					embot::hw::bno055::init(imu, imuconfig);
					init = true;
				}
				embot::hw::bno055::get(imu, info, 1000);
				
				for(;;)
			{
				if(true == embot::hw::bno055::operationdone(embot::hw::BNO055::one))
				{
				break;
				}
			}

			embot::hw::bno055::get(imu, info, 1000);
  
			canframe.id = 0x551;         ;
			canframe.size = 8;
			canframe.data[0] = info.chipID;
			
			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);		
			
			i++;
				
		}
	
		
		constexpr embot::hw::gpio::Config out {
			embot::hw::gpio::Mode::OUTPUTpushpull,
			embot::hw::gpio::Pull::pullup,
			embot::hw::gpio::Speed::high
		};
		
		constexpr embot::hw::GPIO GP1 {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::zero};
		constexpr embot::hw::GPIO GP2 {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::four};
  	constexpr embot::hw::GPIO GP3 {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::five};
		constexpr embot::hw::GPIO GP4 {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::six};
		constexpr embot::hw::GPIO GP5 {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::seven};
		constexpr embot::hw::GPIO GP6 {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::eight};
			
		static void initGPIO(){
			embot::hw::gpio::init(GP1, {out});
			embot::hw::gpio::init(GP2, {out});
			embot::hw::gpio::init(GP3, {out});
			embot::hw::gpio::init(GP4, {out});
			embot::hw::gpio::init(GP5, {out});
			embot::hw::gpio::init(GP6, {out});
				
			embot::hw::gpio::set(GP1, embot::hw::gpio::State::SET); 
			embot::hw::gpio::set(GP2, embot::hw::gpio::State::SET); 
			embot::hw::gpio::set(GP3, embot::hw::gpio::State::SET); 
			embot::hw::gpio::set(GP4, embot::hw::gpio::State::SET); 
			embot::hw::gpio::set(GP5, embot::hw::gpio::State::SET); 
			embot::hw::gpio::set(GP6, embot::hw::gpio::State::SET); 
		}
		
		
		static void testGPIO(){
			embot::hw::gpio::set(GP2, embot::hw::gpio::State::RESET); 
			embot::core::wait(500* embot::core::time1millisec);			
			embot::hw::gpio::set(GP6, embot::hw::gpio::State::RESET); 
			embot::core::wait(500* embot::core::time1millisec);			
			embot::hw::gpio::set(GP5, embot::hw::gpio::State::RESET); 
			embot::core::wait(500* embot::core::time1millisec);			
			embot::hw::gpio::set(GP4, embot::hw::gpio::State::RESET); 
			embot::core::wait(500* embot::core::time1millisec);			
			embot::hw::gpio::set(GP3, embot::hw::gpio::State::RESET); 
			embot::core::wait(500* embot::core::time1millisec);			
			embot::hw::gpio::set(GP1, embot::hw::gpio::State::RESET); 
			embot::core::wait(500* embot::core::time1millisec);			
		}

		static void testCAN(){

			embot::hw::can::Frame canframe;
			
			canframe.id = 0x551;         ;
			canframe.size = 8;
			canframe.data[0] = 0xAA;
			
			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);		
			
			i++;


		}		
		
		
    static void eventhread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *p)
    {
        if(0 == eventmask)
        {   // timeout ...     
            return;
        }
        
        // we clear the frames to be trasmitted
        outframes.clear();   
				std::uint8_t remaining = 0;
        if(true == embot::core::binary::mask::check(eventmask, evRXcanframe))
        {        
          embot::hw::can::Frame canframe;
			
					embot::hw::can::get(embot::hw::CAN::one, canframe, remaining);
										
					switch(canframe.data[0]){

						//Check revisione fw collaudo			      
						case 0x00 :  embot::core::wait(300* embot::core::time1millisec); getFirmwareVersion(); break;

						//Test led rosso e blu spenti
						case 0x01 :  embot::core::wait(300* embot::core::time1millisec); testLeds(0); break;
						
						//Test led rosso e blu accesi
						case 0x02 :  embot::core::wait(300* embot::core::time1millisec); testLeds(1); break;
	
						//Test sensore temperatura
  					case 0x03 : embot::core::wait(300* embot::core::time1millisec);	 testTemperature(canframe.data[1]); break;

						//Test BNO055
  					case 0x04 : embot::core::wait(300* embot::core::time1millisec);	 testBNO055(); break;

						//Test GPIO
  					case 0x05 : initGPIO(); embot::core::wait(300* embot::core::time1millisec);	 testGPIO(); break;
						
						//Test CAN
  					case 0x06 : embot::core::wait(300* embot::core::time1millisec);	 testCAN(); break;
						
						default : break;
					}
     
        }
        

     
    }
		



}}}}}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


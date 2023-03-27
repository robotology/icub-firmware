
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
#include "embot_hw_usb.h"
#include "embot_hw_bno055.h"
#include "embot_hw_gpio.h"
#include "embot_hw_spi.h"

#include "embot_hw_types.h"
#include <vector>


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
    
    constexpr std::uint8_t maxINPcanframes = 16;
    constexpr std::uint8_t maxOUTcanframes = 32;
    constexpr embot::os::Event evRXcanframe = 0x00000001;
    constexpr embot::os::Event evRXusbmessage = 0x00000010;

    static void eventhread_init(embot::os::Thread *t, void *p);
    static void eventhread_onevent(embot::os::Thread *t, embot::os::Event evt, void *p);
    
    static void alerteventhread(void *arg);
static void alerteventbasedthreadusb(void *arg);
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
				
				// init usb
				embot::hw::usb::Config configusb;
				configusb.rxcapacity = 16;
	      configusb.onrxmessage = embot::core::Callback(alerteventbasedthreadusb, eventhread);
				embot::hw::usb::init(embot::hw::usb::Port::one, configusb);
				embot::hw::usb::enable(embot::hw::usb::Port::one);
			
				
    }
        
    static void alerteventhread(void *arg)
    {
        embot::os::EventThread* evtsk = reinterpret_cast<embot::os::EventThread*>(arg);
        if(nullptr != evtsk)
        {
            evtsk->setEvent(evRXcanframe);
        }
    }
		
		static void alerteventbasedthreadusb(void *arg)
    {
				embot::hw::usb::Message msg;
			
        std::uint8_t remainingINrx = 0;
        if(embot::hw::resOK == embot::hw::usb::get(embot::hw::usb::Port::one, msg, remainingINrx))
        {
					int i;
					i++;

				}
			
				msg.data[0] = ' ';
				msg.data[1] = ' ';
				msg.data[2] = 'O';
				msg.data[3] = 'K';

				if(embot::hw::resOK == embot::hw::usb::transmitimmediately(embot::hw::usb::Port::one, msg))
        {
					int i;
					i++;
				}		
    }

    static void eventhread_init(embot::os::Thread *t, void *p)
    {
        embot::hw::result_t r = embot::hw::can::enable(embot::hw::CAN::one);  
        r = r;         
        outframes.reserve(maxOUTcanframes);
    }


		const uint8_t  Firmware_vers = 4;
		const uint8_t  Revision_vers = 0;
		const uint8_t  Build_number  = 0;
		
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
		
		static void test_CAN(){

			embot::hw::can::Frame canframe;
			
			canframe.id = 0x551;         ;
			canframe.size = 8;
			canframe.data[0] = 0xAA;
			
			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);		

		}		
		
		void test_Leds(uint8_t on){
			 static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
       embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
       theleds.init(allleds);    
				
			  switch(on){
					case 0: 
						theleds.get(embot::hw::LED::one).off();
						break;
					case 1: 
						theleds.get(embot::hw::LED::one).on();
						break;
					default: break;
				}
			 
		}
		


		void test_Usb(void){
			

			embot::hw::usb::Message msg;
			msg.data[0] = 'c';
			msg.data[1] = 'i';
			msg.data[2] = 'a';
			msg.data[3] = 'o';
			
			embot::hw::usb::transmitimmediately(embot::hw::usb::Port::one, msg);
      			 
		}
		
		constexpr embot::hw::BNO055 imu = embot::hw::BNO055::one;
		constexpr embot::hw::bno055::Config imuconfig = {};
		embot::hw::bno055::Info info;
		
		static void test_BNO055(){
			embot::hw::can::Frame canframe;
			static bool init = false;
			
			if(!init) {
				embot::hw::result_t ret = embot::hw::bno055::init(imu, imuconfig);
				init = true;
			}

			embot::hw::bno055::get(imu, info, 100*embot::core::time1millisec);

			canframe.id = 0x551;
			canframe.size = 8;
			canframe.data[0] = info.chipID;
			
			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);		
				
		}
		

		constexpr embot::hw::I2C i2c1 = embot::hw::I2C::one;
		constexpr embot::hw::I2C i2c2 = embot::hw::I2C::two;
		constexpr embot::hw::I2C i2c3 = embot::hw::I2C::three;
		constexpr embot::hw::I2C i2c4 = embot::hw::I2C::four;
		constexpr embot::hw::i2c::Config i2cconfig = {};

		constexpr embot::hw::SPI spi1 = embot::hw::SPI::one;
		constexpr embot::hw::SPI spi2 = embot::hw::SPI::two;
		constexpr embot::hw::spi::Config spiconfig = {};
			
		constexpr embot::hw::gpio::Config out {
			embot::hw::gpio::Mode::OUTPUTpushpull,
			embot::hw::gpio::Pull::pullup,
			embot::hw::gpio::Speed::high
		};

		constexpr embot::hw::gpio::Config input {
			embot::hw::gpio::Mode::INPUT,
			embot::hw::gpio::Pull::nopull,
			embot::hw::gpio::Speed::high
		};		
		
		constexpr embot::hw::GPIO GPA0 {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::zero};
		constexpr embot::hw::GPIO GPA5 {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::five};
		constexpr embot::hw::GPIO GPA15 {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::fifteen};

		constexpr embot::hw::GPIO GPB1 {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::one};
		
		static void initGPIO(embot::hw::GPIO gpio, embot::hw::gpio::Config conf, embot::hw::gpio::State state ){
			embot::hw::gpio::init(gpio, {conf});
			if(conf.mode == embot::hw::gpio::Mode::OUTPUTpushpull) embot::hw::gpio::set(gpio, state); 
		}
		
		static void test_MPU9250(){
			embot::hw::can::Frame canframe;
			static bool init = false;
   		uint16_t res = 0xAA;
			bool b;
			
			if(!embot::hw::i2c::initialised(i2c2)) {
				initGPIO(GPA15, out, embot::hw::gpio::State::SET); 
				embot::hw::i2c::init(i2c2, i2cconfig);
				init = true;
			}

  		embot::core::wait(300* embot::core::time1millisec);
      
			if(!embot::hw::i2c::ping(i2c2, 0xD2, 100*embot::core::time1millisec)) res = 0xBB;
//			std::vector<embot::hw::i2c::ADR> addresses;    
//			b = embot::hw::i2c::discover(i2c2, addresses);
//			int i = addresses.size();

			canframe.id = 0x551;         
			canframe.size = 8;
			canframe.data[0] = res;
			
			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);		
				
		}
		
		static void test_conn_servo(){
			embot::hw::can::Frame canframe;
			uint16_t res = 0xAA;
			
			initGPIO(GPA0, out, embot::hw::gpio::State::SET); 
			initGPIO(GPA5, input,  embot::hw::gpio::State::SET);
			
			if(embot::hw::gpio::get(GPA5) !=  embot::hw::gpio::State::SET) res = 0xBB; 
			
			initGPIO(GPA0, out, embot::hw::gpio::State::RESET); 
			initGPIO(GPA5, input,  embot::hw::gpio::State::RESET);
			
			if(embot::hw::gpio::get(GPA5) !=  embot::hw::gpio::State::RESET) res = 0xBB; 
			
			canframe.id = 0x551;         ;
			canframe.size = 8;
			canframe.data[0] = res;
			
			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);		
				
		}

		static void test_conn_i2c(embot::hw::I2C i2c){
			embot::hw::can::Frame canframe;
			uint16_t res = 0xAA;
			uint16_t FAP = 0xbc; // TO BE CHECKED!!!!!!!!!!!!!!!!!
			
			if(!embot::hw::i2c::initialised(i2c)) {
				initGPIO(GPB1, input,  embot::hw::gpio::State::SET); 	
				embot::hw::i2c::init(i2c, i2cconfig);
			}

			embot::core::wait(300* embot::core::time1millisec);
			
			if(embot::hw::gpio::get(GPB1) !=  embot::hw::gpio::State::SET) res = 0xBB; 
			if(!embot::hw::i2c::ping(i2c, 0xbc, 100*embot::core::time1millisec)) res = 0xBB;
			
			canframe.id = 0x551;  
			canframe.size = 8;
			canframe.data[0] = res;
			
			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);		
				
		}
		
		static void test_conn_spi(embot::hw::SPI spi){
			embot::hw::can::Frame canframe;
			uint16_t res = 0xAA;
		
			
			if(!embot::hw::spi::initialised(spi)) {
				embot::hw::spi::init(spi1, spiconfig);
			}

			//TBD w/ the external board (i.e. mouth rfe)!

			canframe.id = 0x551;         ;
			canframe.size = 8;
			canframe.data[0] = res;
			
			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);		
				
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

						//test CAN			      
						case 0x00 :  embot::core::wait(300* embot::core::time1millisec); test_CAN(); break;

						//Check test fw rev.			      
						case 0x01 :  embot::core::wait(300* embot::core::time1millisec); getFirmwareVersion(); break;
						
						//Test led blue off
						case 0x02 :  embot::core::wait(300* embot::core::time1millisec); test_Leds(0); break;
						
						//Test led blue on
						case 0x03 :  embot::core::wait(300* embot::core::time1millisec); test_Leds(1); break;

						//Test serial
						case 0x04 :  embot::core::wait(300* embot::core::time1millisec); test_Usb(); break;

						//Test IMU Bosch BNO055
						case 0x05 :  embot::core::wait(300* embot::core::time1millisec); test_BNO055(); break;

						//Test IMU MPU-9250
						case 0x06 :  embot::core::wait(300* embot::core::time1millisec); test_MPU9250(); break;						

						//Test Connector J2 (servo motor)
						case 0x07 :  embot::core::wait(300* embot::core::time1millisec); test_conn_servo(); break;								

						//Test Connector J10 (i2c1 + imu_sync sig.) attaching a FAP and pulling up the IMU_SYNC sig. w/ a 10k resistor
						case 0x08 :  embot::core::wait(300* embot::core::time1millisec); test_conn_i2c(i2c1); break;

						//Test Connector J11 (i2c2 + imu_sync sig.) attaching a FAP and pulling up the IMU_SYNC sig. w/ a 10k resistor
						case 0x09 :  embot::core::wait(300* embot::core::time1millisec); test_conn_i2c(i2c2); break;

						//Test Connector J12 (i2c3 + imu_sync sig.) attaching a FAP and pulling up the IMU_SYNC sig. w/ a 10k resistor
						case 0x0A :  embot::core::wait(300* embot::core::time1millisec); test_conn_i2c(i2c3); break;
						
						//Test Connector J13 (i2c4 + imu_sync sig.) attaching a FAP and pulling up the IMU_SYNC sig. w/ a 10k resistor
						case 0x0B :  embot::core::wait(300* embot::core::time1millisec); test_conn_i2c(i2c4); break;
						
						//Test Connector J1 (spi1) using a rfe_mouth board i.e.
						case 0x0C :  embot::core::wait(300* embot::core::time1millisec); test_conn_spi(spi1); break;

						//Test Connector J3 (spi2) using a rfe_mouth board i.e.
						case 0x0D :  embot::core::wait(300* embot::core::time1millisec); test_conn_spi(spi2); break;
						
						default : break;
					}
        }   
    }
		



}}}}}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Davide Tom�
 * email:   davide.tome@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_skeleton_os_bootloader.h"


constexpr extern uint8_t  Firmware_vers = 1;
constexpr extern uint8_t  Revision_vers = 0;
constexpr extern uint8_t  Build_number  = 0;

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_amcbldc.h"

#include "embot_core.h"
#include "embot_os_Thread.h"
#include "embot_app_theBootloader.h"
#include "embot_app_bootloader_theCANparser.h"
#include "embot_app_theLEDmanager.h"
#include "embot_hw_can.h"
#include "embot_hw_gpio.h"
#include "embot_hw_i2c.h"
#include "embot_hw_motor.h"
#include "embot_hw_encoder.h"
#include "embot_hw_types.h"

#include "stm32hal.h"

#include "analog.h"
#include "pwm.h"
#include "encoder.h"

#include "tests.h"

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
				activity_param.blinkingperiod = ActivityParam::BlinkSlowPeriod;      
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
        
//        // manage the led blinking period
//        embot::core::relTime period = 0;    
//        if(nullptr != pp)
//        {
//            period = pp->blinkingperiod;
//        }    

//        // start the led pulser
//        static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
//        embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
//        theleds.init(allleds);    
//        theleds.get(embot::hw::LED::one).pulse(period, 0); 
           

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
//        embot::app::bootloader::theCANparser &canparser = embot::app::bootloader::theCANparser::getInstance();
//        embot::app::bootloader::theCANparser::Config config {embot::prot::can::Process::bootloader};
//        canparser.initialise(config);  

        // finally init can. it will be enabled only inside teh startup() of the eventhread
        embot::hw::result_t r = embot::hw::resNOK;
        embot::hw::can::Config canconfig; 
        canconfig.rxcapacity = maxINPcanframes;
        canconfig.txcapacity = maxOUTcanframes;
        canconfig.onrxframe = embot::core::Callback(alerteventhread, eventhread); 
        embot::hw::can::init(embot::hw::CAN::one, canconfig);
//        embot::hw::can::setfilters(embot::hw::CAN::one, embot::app::theCANboardInfo::getInstance().getCANaddress());  

    embot::hw::motor::init(embot::hw::MOTOR::one, {});

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

//******************** TESTS ******************************************************************//
		
		static void sendCan(std::uint8_t data[8]){
			embot::hw::can::Frame canframe;
			
			canframe.id = 0x551;  
			canframe.size = 8;
			for(uint8_t i = 0; i < 8; i++) {canframe.data[i] = data[i];}
			
			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);	
			
		}
		
		static void testCan(){
			uint8_t data[8] {0};
			data[0] = 0xAA;
			
			sendCan(data);
		}

    void getFirmwareVersion()
    {
			uint8_t data[8] {0};
			data[0] = Firmware_vers;
			data[1] = Revision_vers;
			data[2] = Build_number;

			sendCan(data);
    }
		
	void testLeds(uint8_t on){
			 static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three};  
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

		void testIdCode(embot::hw::can::Frame frame){
			uint32_t id = DBGMCU->IDCODE & 0xFFF;
			uint32_t rev = (DBGMCU->IDCODE >> 16) & 0xFFFF;
		
			uint8_t data[8] {0};
			uint8_t rev1 = rev >> 8;
			uint8_t rev2 = rev;
			uint8_t id1 = id >> 8;
			uint8_t id2 = id;
			
//			if(rev1 == frame.data[1] &&
//				rev2 == frame.data[2] &&
//			  id1 == frame.data[3] &&
//			  id2 == frame.data[4]) data[0] = 0xAA;
//			else data[0] = 0xBB;
			
			data[0] = rev1;
			data[1] = rev2;
			data[2] = id1;
			data[3] = id2;
			
			sendCan(data);
		}
		
		void testVin(){
			uint8_t data[8] {0};
	    float vin {0.0};
			vin = embot::hw::bsp::amcbldc::getVIN();

			embot::core::print("Vin : " + std::to_string(vin));
			
			if(vin > 11.5 && vin < 12.5) data[0] = 0xAA;
			else data[0] = 0xBB;
			
  		sendCan(data);
		}
		
		void testCin(){
			uint8_t data[8] {0};
	    float cin {0.00};
			cin = embot::hw::bsp::amcbldc::getCIN();

			embot::core::print("Cin : " + std::to_string(cin));
			
			if(cin > 0.030 && cin < 0.060) data[0] = 0xAA;
			else data[0] = 0xBB;
			
  		sendCan(data);
		}
		

		constexpr embot::hw::GPIO VAUXOK {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::fourteen};

		void testVauxOk(){
			uint8_t data[8] {0};
			auto s = embot::hw::gpio::get(VAUXOK);

			if(s == embot::hw::gpio::State::SET){embot::core::print("OK"); data[0] = 0xAA;}
			else{embot::core::print("NOK"); data[0] = 0xBB;}
			
  		sendCan(data);
		}
		
		constexpr embot::hw::GPIO VCCOK {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::thirteen};

		void testVccOk(){
			uint8_t data[8] {0};
			auto s = embot::hw::gpio::get(VCCOK);
			
		  if(s == embot::hw::gpio::State::SET){embot::core::print("OK"); data[0] = 0xAA;}
			else{embot::core::print("NOK"); data[0] = 0xBB;}
			
  		sendCan(data);
		}
		
		constexpr embot::hw::GPIO EXTFAULT {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::fifteen};

		void testFault(uint8_t status){
			uint8_t data[8] {0};			
			auto s = embot::hw::gpio::get(EXTFAULT);

		if(status == 1)
		{	
			if(s == embot::hw::gpio::State::SET){embot::core::print("OK"); data[0] = 0xAA;}
			else{embot::core::print("NOK"); data[0] = 0xBB;}
			
		}
		else
		{
			if(s != embot::hw::gpio::State::SET){embot::core::print("OK"); data[0] = 0xAA;}
			else{embot::core::print("NOK"); data[0] = 0xBB;}
		}

  		sendCan(data);
		}
		
 		constexpr embot::hw::I2C i2c = embot::hw::I2C::four;
		constexpr embot::hw::i2c::Config i2cconfig = {};
			
		void testI2c(embot::hw::I2C i2c){
			uint8_t data[8] {0};			
			uint16_t FAP = 0xbc; // FAP i2c address
			
			if(!embot::hw::i2c::initialised(i2c)) {
				embot::hw::i2c::init(i2c, i2cconfig);
			}

			embot::core::wait(300* embot::core::time1millisec);
			
			if(embot::hw::i2c::ping(i2c, FAP, 100*embot::core::time1millisec)) data[0] = 0xAA;
			else data[0] = 0xBB;
			
  		sendCan(data);
		}
		
		
		void testHall(){
			uint8_t data[8] {0};		
			uint8_t res {0};

			for(int i=0; i<150; i++){	
				if((HAL_GPIO_ReadPin(HALL1_GPIO_Port, HALL1_Pin)     != GPIO_PIN_RESET)) res |= 1;
				else res |= 2;
				if((HAL_GPIO_ReadPin(HALL2_GPIO_Port, HALL2_Pin)     != GPIO_PIN_RESET)) res |= 4;
				else res |= 8;
				if((HAL_GPIO_ReadPin(HALL3_GPIO_Port, HALL3_Pin)     != GPIO_PIN_RESET)) res |= 16;
				else res |= 32;

				embot::core::wait(10* embot::core::time1millisec);
			
				embot::core::print(std::to_string(res));
				
			}

			if(res == 63) data[0] = 0xAA;
			else data[0] = 0xBB;
			
			sendCan(data);
		}
		
		void testEncoder(){
			uint8_t data[8] {0};		
			uint8_t res {0};

			for(int i=0; i<150; i++){	
				if((HAL_GPIO_ReadPin(ENCA_GPIO_Port, HALL1_Pin)     != GPIO_PIN_RESET)) res |= 1;
				else res |= 2;
				if((HAL_GPIO_ReadPin(ENCB_GPIO_Port, HALL2_Pin)     != GPIO_PIN_RESET)) res |= 4;
				else res |= 8;
				if((HAL_GPIO_ReadPin(ENCZ_GPIO_Port, HALL3_Pin)     != GPIO_PIN_RESET)) res |= 16;
				else res |= 32;

				embot::core::wait(10* embot::core::time1millisec);
			
				embot::core::print(std::to_string(res));
				
			}

			if(res == 47) data[0] = 0xAA;
			else data[0] = 0xBB;
			
			sendCan(data);
		}
		
//******************** END TESTS ******************************************************************//

		
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
						
						//Test CAN
  					case 0x00 : embot::core::wait(300* embot::core::time1millisec);	testCan(); break;
						
						//Check test fw revision		      
						case 0x01 : embot::core::wait(300* embot::core::time1millisec); getFirmwareVersion(); break;

						//Test LEDs off
						case 0x02 : embot::core::wait(300* embot::core::time1millisec); testLeds(0); break;

						//Test LEDs on
						case 0x03 : embot::core::wait(300* embot::core::time1millisec); testLeds(1); break;
						
						//Test micro DEV_ID
						case 0x04 : embot::core::wait(300* embot::core::time1millisec); testIdCode(canframe); break;
						
						//Test Vin
						case 0x05 : embot::core::wait(300* embot::core::time1millisec); testVin(); break;
						
						//Test Cin
						case 0x06 : embot::core::wait(300* embot::core::time1millisec); testCin(); break;
						
						//Test VAUXOK
						case 0x07 : embot::core::wait(300* embot::core::time1millisec); testVauxOk(); break;

						//Test VCCOK
						case 0x08 : embot::core::wait(300* embot::core::time1millisec); testVccOk(); break;
						
						//Test FAULT ON
						case 0x09 : embot::core::wait(300* embot::core::time1millisec); testFault(1); break;
						
  					//Test FAULT OFF
						case 0x0A : embot::core::wait(300* embot::core::time1millisec); testFault(0); break;

						//Test I2C
						case 0x0B : embot::core::wait(300* embot::core::time1millisec); testI2c(i2c); break;

						//Test HALL
						case 0x0C : embot::core::wait(300* embot::core::time1millisec); testHall(); break;

						//Test ECODER
						case 0x0D : embot::core::wait(300* embot::core::time1millisec); testEncoder(); break;
						
						default : break;
					}			
        }   
    }

}}}}}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


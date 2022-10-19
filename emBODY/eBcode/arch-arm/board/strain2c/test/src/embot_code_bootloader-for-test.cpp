
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Davide Tomé
 * email:   davide.tome@iit.it
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
#include "embot_hw_pga308.h"
#include "embot_hw_pga308_bsp.h"
#include "embot_hw_onewire_bsp.h"
#include "embot_hw_adc.h"
#include "embot_hw_adc_bsp.h"
#include "embot_prot_can.h"

#include "embot_core.h"
#include "embot_core_utils.h"
#include "embot_core_binary.h"
#include "embot_prot_can_analog_polling.h"


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
						break;
					case 1: 
						theleds.get(embot::hw::LED::one).on();
						break;
					default: break;
				}
			 
		}
	

		constexpr embot::hw::SI7051 thermometer1 = embot::hw::SI7051::one;
  	constexpr embot::hw::SI7051 thermometer2 = embot::hw::SI7051::two;

		constexpr embot::hw::si7051::Config thermometerconfig = {};
  	embot::hw::si7051::Temperature temp1 {0};
  	embot::hw::si7051::Temperature temp2 {0};

		
		void testTemperature(uint8_t tempIN){
			
			embot::hw::can::Frame canframe;
			static bool init = false;

			// leggo sensore temperatura 1
			if(!init) {
				embot::hw::si7051::init(thermometer1, thermometerconfig);
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
			embot::hw::si7051::read(thermometer1, temp1);

			temp1 = temp1/10;
			
			// leggo sensore temperatura 2
			init = false;
			if(!init) {
				embot::hw::si7051::init(thermometer2, thermometerconfig);
				init = true;
			}

			embot::hw::si7051::acquisition(embot::hw::SI7051::two, {});
			for(;;)
			{
			if(true == embot::hw::si7051::operationdone(embot::hw::SI7051::two))
			{
			break;
			}
			}
			embot::hw::si7051::read(thermometer2, temp2);

			temp2 = temp2/10;			
						
			canframe.id = 0x551;         ;
			canframe.size = 8;
			canframe.data[1] = tempIN;
			canframe.data[2] = temp1;
			canframe.data[3] = temp2;

			if(((temp1 < tempIN - ((tempIN*30)/100) || temp1 > tempIN +((tempIN*30)/100)) || (temp2 < tempIN - ((tempIN*30)/100) || temp2 > tempIN +((tempIN*30)/100)))) canframe.data[0] = 0xBB; // fail		 
			else canframe.data[0] = 0xAA; // pass		 
			
			embot::core::wait(500* embot::core::time1millisec);

			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);		
			
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
				
		}
	
			embot::hw::pga308::Config pga308cfg;
			std::uint16_t               dmabuffer[6];
			std::uint16_t               adcvalue[6]; 
		
		  volatile bool ready = false;
		
		  void alertdataisready(void *){
			ready = true;			
			}
		
		static void testPga308(){
			embot::hw::can::Frame canframe;
			static bool init = false;
			
      // common settings
			pga308cfg.powerongpio = embot::hw::pga308::getBSP().getPROP(embot::hw::PGA308::one)->poweron; // embot::hw::GPIO(EN_2V8_GPIO_Port, EN_2V8_Pin);
			pga308cfg.poweronstate = embot::hw::gpio::State::SET;
			pga308cfg.onewireconfig.rate = embot::hw::onewire::Rate::tenKbps;
			pga308cfg.onewireconfig.usepreamble = true;
			pga308cfg.onewireconfig.preamble = 0x55;
			
      // embot::hw::PGA308::one
			pga308cfg.onewirechannel = embot::hw::ONEWIRE::one;
			pga308cfg.onewireconfig.gpio = embot::hw::onewire::getBSP().getPROP(embot::hw::ONEWIRE::one)->gpio; // embot::hw::GPIO(W_STRAIN1_GPIO_Port, W_STRAIN1_Pin);
			embot::hw::pga308::init(embot::hw::PGA308::one, pga308cfg);
			
			// embot::hw::PGA308::two
			pga308cfg.onewirechannel = embot::hw::ONEWIRE::two;
			pga308cfg.onewireconfig.gpio = embot::hw::onewire::getBSP().getPROP(embot::hw::ONEWIRE::two)->gpio; //embot::hw::GPIO(W_STRAIN2_GPIO_Port, W_STRAIN2_Pin);
			embot::hw::pga308::init(embot::hw::PGA308::two, pga308cfg);
			
			// embot::hw::PGA308::three
			pga308cfg.onewirechannel = embot::hw::ONEWIRE::three;
			pga308cfg.onewireconfig.gpio = embot::hw::onewire::getBSP().getPROP(embot::hw::ONEWIRE::three)->gpio;; // embot::hw::GPIO(W_STRAIN3_GPIO_Port, W_STRAIN3_Pin);
			embot::hw::pga308::init(embot::hw::PGA308::three, pga308cfg);
					
			// embot::hw::PGA308::four
			pga308cfg.onewirechannel = embot::hw::ONEWIRE::four;
			pga308cfg.onewireconfig.gpio = embot::hw::onewire::getBSP().getPROP(embot::hw::ONEWIRE::four)->gpio; // embot::hw::GPIO(W_STRAIN4_GPIO_Port, W_STRAIN4_Pin);
			embot::hw::pga308::init(embot::hw::PGA308::four, pga308cfg);    
			
			// embot::hw::PGA308::five
			pga308cfg.onewirechannel = embot::hw::ONEWIRE::five;
			pga308cfg.onewireconfig.gpio = embot::hw::onewire::getBSP().getPROP(embot::hw::ONEWIRE::five)->gpio; // embot::hw::GPIO(W_STRAIN5_GPIO_Port, W_STRAIN5_Pin);
			embot::hw::pga308::init(embot::hw::PGA308::five, pga308cfg);     

			// embot::hw::PGA308::six
			pga308cfg.onewirechannel = embot::hw::ONEWIRE::six;
			pga308cfg.onewireconfig.gpio = embot::hw::onewire::getBSP().getPROP(embot::hw::ONEWIRE::six)->gpio; // embot::hw::GPIO(W_STRAIN6_GPIO_Port, W_STRAIN6_Pin);
			embot::hw::pga308::init(embot::hw::PGA308::six, pga308cfg);  
			
			// imporre al pga valori dei registri che abbiano un certo guadagno ed offset 
			embot::prot::can::analog::polling::PGA308cfg1 cfg1 {};
			embot::prot::can::analog::polling::PGA308cfg1 cfg2 {};
			embot::prot::can::analog::polling::PGA308cfg1 cfg3 {};
			
			//configdata.amplifiers_get(setindex, i, cfg1); 
      // alpha = 96, beta = 4k, where vout = alpha * vin + beta
			// valori presi dal documento protocollo a pagina 108
			cfg1.GD = 0x4000;
			cfg1.GI = 0x5;
			cfg1.S = 0x0;
			cfg1.GO = 0x6;
			cfg1.Voffsetcoarse = 0x10;
			cfg1.Vzerodac = 0x810f;
				
			//configdata.amplifiers_get(setindex, i, cfg2); 
      // alpha = 16, beta = 4k, where vout = alpha * vin + beta
			// valori presi dal documento protocollo a pagina 108
			cfg2.GD = 0x0000;
			cfg2.GI = 0x4;
			cfg2.S = 0x0;
			cfg2.GO = 0x2;
			cfg2.Voffsetcoarse = 0x5e;
			cfg2.Vzerodac = 0x8072;

		  //configdata.amplifiers_get(setindex, i, cfg3); 
      // alpha = 8, beta = 4k, where vout = alpha * vin + beta
			// valori presi dal documento protocollo a pagina 108
			cfg3.GD = 0x8000;
			cfg3.GI = 0x0;
			cfg3.S = 0x0;
			cfg3.GO = 0x2;
			cfg3.Voffsetcoarse = 0x64;
			cfg3.Vzerodac = 0x6229;
				
			embot::hw::pga308::TransferFunctionConfig tfc;
      tfc.load(cfg2.GD, cfg2.GI, cfg2.S, cfg2.GO, cfg2.Voffsetcoarse, cfg2.Vzerodac);   
			// verifica che apha e bet siano 96 e 4k
      float alpha = tfc.alpha();
      float beta = tfc.beta();
			// si impone che il pga abbia veramente i valori voluti

			embot::hw::pga308::set(embot::hw::PGA308::one, tfc);   
			embot::hw::pga308::set(embot::hw::PGA308::two, tfc);   
			embot::hw::pga308::set(embot::hw::PGA308::three, tfc);   
			embot::hw::pga308::set(embot::hw::PGA308::four, tfc);   
			embot::hw::pga308::set(embot::hw::PGA308::five, tfc);   
			embot::hw::pga308::set(embot::hw::PGA308::six, tfc);   


				
			//inizializzo ADC
			embot::hw::adc::Config adcConf;
			adcConf.numberofitems = 6;
			adcConf.destination = dmabuffer;
			adcConf.mode = embot::hw::adc::Mode::dma;
			adcConf.oncompletion = { alertdataisready, nullptr };
			embot::hw::adc::init(embot::hw::ADC::one, adcConf);
			ready = false;
			volatile int i{0};
			std::memset(dmabuffer, 0x1, sizeof(dmabuffer));
			embot::hw::adc::start(embot::hw::ADC::one);
			embot::core::wait(500* embot::core::time1millisec);

//			while(!ready || i < 1000) {
//  			embot::core::wait(10* embot::core::time1millisec);
//				i = 1 + 10;
//			}
			embot::hw::adc::stop(embot::hw::ADC::one);
			i++;
				
			canframe.id = 0x551;
			canframe.size = 8;
			canframe.data[0] = 0xAA;
			canframe.data[1] = 0xAA;
			canframe.data[2] = 0xAA;
			canframe.data[3] = 0xAA;
			canframe.data[4] = 0xAA;
			canframe.data[5] = 0xAA;
			canframe.data[6] = 0xAA;
			
			for(int k = 0; k < 6; k++){
				if(dmabuffer[k] < 3800 || dmabuffer[k] > 4200) { canframe.data[0] = 0xBB; canframe.data[k+1] = 0xBB;}			
			}
			

			embot::core::wait(500* embot::core::time1millisec);

			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);						
		}		
		
		
		static void testCAN(){

			embot::hw::can::Frame canframe;
			
			canframe.id = 0x551;         ;
			canframe.size = 8;
			canframe.data[0] = 0xAA;
			
			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);		

		}		
		

		
			
  	static void testStrain(){

			embot::hw::can::Frame canframe;
			
		

			canframe.id = 0x551;         
			canframe.size = 8;
			canframe.data[0] = 0xAA;
			
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

						//Check revisione fw collaudo
						case 0x00 :  embot::core::wait(300* embot::core::time1millisec); getFirmwareVersion(); break;

						//Test led spento
						case 0x01 :  embot::core::wait(300* embot::core::time1millisec); testLeds(0); break;
						
						//Test led  blu acceso
						case 0x02 :  embot::core::wait(300* embot::core::time1millisec); testLeds(1); break;
	
						//Test sensori temperatura
  					case 0x03 : embot::core::wait(300* embot::core::time1millisec);	 testTemperature(canframe.data[1]); break;

						//Test BNO055
  					case 0x04 : embot::core::wait(300* embot::core::time1millisec);	 testBNO055(); break;
			
						//Test pga308
  					case 0x05 : embot::core::wait(300* embot::core::time1millisec);	 testPga308(); break;

						//Test CAN
  					case 0x06 : embot::core::wait(300* embot::core::time1millisec);	 testCAN(); break;
						
						default : break;
					}
     
        }
        

     
    }
		



}}}}}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


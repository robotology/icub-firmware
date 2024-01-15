// dependencies
#include "embot_core.h"
#include "embot_os_theScheduler.h"

#include "embot_hw_can.h"
#include "embot_prot_can.h"
#include "embot_app_theLEDmanager.h"



namespace test::any {
    
		void init(){}
			
		static void sendCAN(uint8_t data[8]){
			embot::hw::can::Frame canframe;
			
			canframe.id = 0x551;         ;
			canframe.size = 8;
			
			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);	
			
		}
		static void testCAN(){
			uint8_t data[8] {0};
			data[0] = 0xAA;
			
			sendCAN(data);
		}


}

void test_hw_init()
{
    test::any::init();   
}



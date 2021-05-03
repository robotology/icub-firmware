/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#if 0

    This program is a basic demo for the new IPAL which uses the project developed by Tecnologix
    and moves it into the stm32hal + embot framework.
    
    The program launches three treads: 
    - IPALproc: inits an starts the ipal, inits a socket, executes function ipal_sys_process_communication()
    - IPALtick: periodically calls function ipal_sys_timetick_increment()
    - tUDPcomm: performs communication by sending a UDP packet to the host (10.0.1.104 or other) 
    
    

#endif

    // #if defined(TECNOLOGIX_CODE) contains the original code
    #undef TECNOLOGIX_CODE    


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os.h"
#include "embot_os_rtos.h"

// declarations

void initSystem(embot::os::Thread *t, void* initparam);
void onIdle(embot::os::Thread *t, void* idleparam);


int main(void)
{ 
    // steps:
    // 1. i init the embot::os
    // 2 i start the scheduler
        
    constexpr embot::os::InitThread::Config initcfg = { 6*1024, initSystem, nullptr };
    constexpr embot::os::IdleThread::Config idlecfg = { 4*1024, nullptr, nullptr, onIdle };
    constexpr embot::core::Callback onOSerror = { };
    constexpr embot::os::Config osconfig {embot::core::time1millisec, initcfg, idlecfg, onOSerror};
    
    // embot::os::init() internally calls embot::hw::init() which also calls embot::core::init()
    embot::os::init(osconfig);
    
    // now i start the os    
    embot::os::start();

    // just because i am paranoid (embot::os::start() never returns)
    for(;;);    
}



// extra include

#include "embot_os_theScheduler.h"
#include "embot_os_theTimerManager.h"
#include "embot_os_theCallbackManager.h"
#include "embot_app_theLEDmanager.h"


// definitions

constexpr embot::hw::LED redLED = embot::hw::LED::one;
constexpr embot::hw::LED yellowLED = embot::hw::LED::two;
constexpr embot::hw::LED otherLED = embot::hw::LED::three;

// static 
static void s_ipal_start();
static void s_start_IPALproc_thread();
static void s_start_UDPcomm_thread();


void onIdle(embot::os::Thread *t, void* idleparam)
{
    static uint32_t i = 0;
    i++;
}


void initSystem(embot::os::Thread *t, void* initparam)
{
    embot::core::print("this is the udpdemo basic");    
        
    embot::os::theTimerManager::getInstance().start({});     
    embot::os::theCallbackManager::getInstance().start({});  
        
    
    embot::core::print("creating the LED pulser: it will blink a LED at 1 Hz.");
        
    static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one, embot::hw::LED::two, embot::hw::LED::three, embot::hw::LED::four};  
    embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
    theleds.init(allleds);    
    theleds.get(yellowLED).pulse(embot::core::time1second); 
    theleds.get(otherLED).on();
    theleds.get(otherLED).off();
    
        
    // starting the thread IPALproc and IPALtick which manage the ipal. 
    // it is a thread similar to tecnologix's net_thread without the thread udp_comm_app_thread which we init here after         
    // that should be enough to get pinged. or not?        
    s_start_IPALproc_thread();
        
    // also starting the communication thread UDPcomm
    // it is a thread similar to tecnologix's udp_comm_app_thread
    s_start_UDPcomm_thread();    

    embot::core::print("quitting the INIT thread. Normal scheduling starts");    
}

#include "ipal.h"
#include "embot_prot_eth.h"
#include "main-embot.h"

// we use ipalcfg as configured in section ipal-config
extern const ipal_cfg_t *ipal_cfgMINE;
const ipal_cfg_t* ipalcfg = ipal_cfgMINE;

embot::prot::eth::IPv4 myIPaddress {"10.0.1.1"};

// for now IPALtick is not a dedicated periodic thread (as in EOtheIPnet) but a callback executed at expiry of a timer
embot::os::Timer *ipalTickTimer {nullptr};

void s_IPALtick(void *p)
{
    //#warning DEBUG: removed temporary the exec of ipal_sys_timetick_increment()
    ipal_sys_timetick_increment();
}

static void s_ipal_start();

 // server thread
 
 // using embot whenever it is possible

#include "eOcommon.h"
#include "EOpacket.h"

 
constexpr uint16_t capacityofUDPpacket {1200};
 
// the IPAL proc is an event-based thread which processes events from the ipal and some other
embot::os::EventThread *tIPALproc {nullptr};
// the IPAL tick is a periodic thread which ticks the ipal
embot::os::PeriodicThread *tIPALtick {nullptr};
// the UDP comm is an event based thread 
embot::os::EventThread *tUDPcomm {nullptr};


//constexpr embot::os::Event evt_CONNECT {1};
constexpr embot::os::Event evt_RXframe {2};
constexpr embot::os::Event evt_TXframe {4};


// needed by the ipal somewhere 
void eom_lwip_set_rx_event(void)
{
   tIPALproc->setEvent(evt_RXframe); 
}

void eom_lwip_set_tx_event(void)
{
   tIPALproc->setEvent(evt_TXframe); 
}


//void setevtCONNECT(void *p)
//{
//    embot::os::EventThread *t = reinterpret_cast<embot::os::EventThread*>(p);
//    t->setEvent(evt_CONNECT);   
//}
//embot::os::Timer *connTimer {nullptr};

void tIPALproc_startup(embot::os::Thread *t, void *param);
void tIPALproc_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param);
void t_IPALproc(void* p) { tIPALproc->run(); }
 
static void s_start_IPALproc_thread()
{        
    tIPALproc = new embot::os::EventThread;
    
    embot::os::EventThread::Config tConfig { 
        8*1024, 
        embot::os::Priority::high47, 
        tIPALproc_startup,
        nullptr,
        10*embot::core::time1millisec, // timeout embot::core::reltimeWaitForever
        tIPALproc_onevent,
        "tIPALproc"
    };
    
    tIPALproc->start(tConfig, t_IPALproc);      
}



static eObool_t eupdater_parser_process_mirror(EOpacket *rxpkt, EOpacket *txpkt);
static eObool_t eupdater_parser_process_replyinfo(EOpacket *rxpkt, EOpacket *txpkt);

using fpParser = eObool_t (*)(EOpacket*, EOpacket *);

void tIPALproc_startup(embot::os::Thread *t, void *param)
{ 

    embot::core::print(std::string("tIPALproc_startup(): @ ") + embot::core::TimeFormatter(embot::core::now()).to_string());   
    
    s_ipal_start();
    
//    // start now a periodic timer which ask the thread to attempt the connection or to check it
//    connTimer = new embot::os::Timer;
//    embot::os::Action act(embot::os::CallbackToThread(setevtCONNECT, t, nullptr));
//    embot::os::Timer::Config cfg{5*embot::core::time1second, act, embot::os::Timer::Mode::forever, 0};
//    connTimer->start(cfg);               
}


void tIPALproc_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{ 
    static volatile uint32_t cnt[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    
    cnt[0]++;
    
    //#warning DEBUG: removed temporary the exec of ipal_sys_process_communication()    
    // we run it on: timeout, reception of events of type TX or RX from low level 
    ipal_sys_process_communication();    
    
    if(0 == eventmask)
    {   // timeout ... 
        cnt[1]++;
        return;
    }
    
//    if(true == embot::core::binary::mask::check(eventmask, evt_CONNECT))
//    {        
//        embot::core::print(std::string("tIPALproc_onevent(evt_CONNECT): @ ") + embot::core::TimeFormatter(embot::core::now()).to_string());         
//    }
      
    if(true == embot::core::binary::mask::check(eventmask, evt_TXframe))
    {
        cnt[3]++;
//        embot::core::print(std::string("tIPALproc_onevent(evt_TXframe): @ ") + embot::core::TimeFormatter(embot::core::now()).to_string()); 
    }

    if(true == embot::core::binary::mask::check(eventmask, evt_RXframe))
    {
        cnt[4]++;
//        embot::core::print(std::string("tIPALproc_onevent(evt_RXframe): @ ") + embot::core::TimeFormatter(embot::core::now()).to_string()); 
    }   
               
}

static void s_ipal_start()
{
    myIPaddress = {ipalcfg->eth_ip};
    
    embot::core::print(std::string("ipal: @ ") + embot::core::TimeFormatter(embot::core::now()).to_string() + 
                       " starting with IP addr " + myIPaddress.to_string()); 
    
    // in here there is the first part of:
    // static void net_thread(void *argument) 
    
//	ipal_result_t res = ipal_res_NOK_generic;	
	
	/* Getsize */
    uint32_t ipal_mem_size = 0;
	if(ipal_base_memory_getsize(ipalcfg, &ipal_mem_size) > 0)
	{
		/* Allocate memory */
//        #warning marco.accame: in here it must be allocated the required memory. is there a change of behaviour?
//		__nop();
	}
	
	/* Initializes IPAL base */
    if(ipal_res_NOK_generic == ipal_base_initialise(ipalcfg, NULL))
    {     
        embot::core::print("ipal_base_initialise(): failed"); 
    }
         
	/* Starts IPAL */ 
    if(ipal_res_NOK_generic == ipal_sys_start())
    {     
        embot::core::print("ipal_sys_start(): failed"); 
    }   

    // start now a periodic timer which executes a callback that ticks the ipal
    // later on we may use a dedicated thread tIPALtick
    ipalTickTimer = new embot::os::Timer;
    embot::os::Action act(embot::os::CallbackToThread(s_IPALtick, nullptr, nullptr));
    embot::os::Timer::Config cfg{10*embot::core::time1millisec, act, embot::os::Timer::Mode::forever, 0};
    ipalTickTimer->start(cfg);       

}



// thread tUDPcomm

void tUDPcomm_startup(embot::os::Thread *t, void *param);
void tUDPcomm_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param);
void t_UDPcomm(void* p) { tUDPcomm->run(); }
 
static void s_start_UDPcomm_thread()
{        
    tUDPcomm = new embot::os::EventThread;
    
    embot::os::EventThread::Config tConfig { 
        6*1024, 
        embot::os::Priority::high40, 
        tUDPcomm_startup,
        nullptr,
        10*embot::core::time1millisec, // timeout embot::core::reltimeWaitForever
        tUDPcomm_onevent,
        "tUDPcomm"
    };
    
    tUDPcomm->start(tConfig, t_UDPcomm);      
}


// in here we have teccnologix's udp_comm_app_thread() split into startup() and forever loop


constexpr uint32_t UDP_APP_MAX_PAYLOAD = 1034;
uint8_t udp_app_send_payload[UDP_APP_MAX_PAYLOAD] = {0};
ipal_packet_t ipal_send_packet = {0};

ipal_udpsocket_t *ipal_udp_socket = nullptr;

static const ipal_tos_t ipal_udp_socket_TOS  = 
{
	.precedence         = ipal_prec_immediate,
	.lowdelay	        = 1,
	.highthroughput     = 1,
    .highreliability	= 0
};

static void udp_comm_app_error_handling(void)
{
    volatile uint32_t x = 0;
	while(1)
	{
        x++;
	}
}


#if defined(USE_PC104_ADDRESSING)
constexpr ipal_port_t localport = 3333;
constexpr ipal_ipv4addr_t remoteaddress = IPAL_ipv4addr(10, 0, 1, 104);
constexpr ipal_port_t remoteport = 11111;
#else
    #error choose your localport, remoteaddress, remoteport
#endif

static void udp_rcv_cb_func(void *arg, ipal_udpsocket_t *skt, ipal_packet_t *pkt, ipal_ipv4addr_t adr, ipal_port_t port);

static void send_datagram();


// the startup
void tUDPcomm_startup(embot::os::Thread *t, void *param)
{
#if TECNOLOGIX_CODE    
    uint32_t delay;
	ipal_result_t udp_err;
	ipal_packet_t ipal_send_packet;
	
	/* Just a counter */
	uint16_union_t cnt;
	
	/* Sets buffer in packet */
	ipal_send_packet.size = 4;
	ipal_send_packet.data = udp_app_send_payload;
#endif

    ipal_send_packet.size = 4;
    ipal_send_packet.data = udp_app_send_payload;
    
    
#if TECNOLOGIX_CODE   
	osDelay(5000);
	
	/* Getting an UDP Socket */
	printf("Getting UDP Socket ...\n");
	
	/* Gets UDP Socket */
	ipal_udp_socket = ipal_udpsocket_new(ipal_udp_socket_TOS);
	
	/* Check if failed */
	if (ipal_udp_socket == NULL)
	{
		printf("Failed getting UDP socket ...\n");
		
		udp_comm_app_error_handling();
	}
    
#endif

    embot::os::rtos::thread_sleep(t, 5000*embot::core::time1millisec);
    ipal_udp_socket = ipal_udpsocket_new(ipal_udp_socket_TOS);
    
    if(nullptr == ipal_udp_socket)
    {
        embot::core::print(std::string("Failed getting UDP socket ...: @ ") + embot::core::TimeFormatter(embot::core::now()).to_string()); 
        udp_comm_app_error_handling();
    }

#if TECNOLOGIX_CODE	
	/* Binding UDP Socket */
	printf("Binding UDP socket ...\n");
	
	/* Bind UDP Socket to local address and port */
	udp_err =	ipal_udpsocket_bind(ipal_udp_socket, 
																ipal_app_cfg.eth_ip,
																TLX_UDP_PORT);

	/* Checks for failure */
	if (udp_err != ipal_res_OK)
	{
		printf("Binding UDP socket ...\n");
		
		udp_comm_app_error_handling();
	}
#endif

	if(ipal_res_OK != ipal_udpsocket_bind(ipal_udp_socket, ipal_cfgMINE->eth_ip, localport))
	{
        embot::core::print(std::string("Failed Binding UDP socket ...: @ ") + embot::core::TimeFormatter(embot::core::now()).to_string()); 		
		udp_comm_app_error_handling();
	}
    
#if TECNOLOGIX_CODE    
	
	printf("Connecting to remote system ...\n");
	
	/* Connect to remote system */
	udp_err = ipal_udpsocket_connect(ipal_udp_socket, 
																	 tlx_ipal_get_IPAL_ipv4addr(
																			TLX_UDP_REMOTE_IP0, 
																			TLX_UDP_REMOTE_IP1, 
																			TLX_UDP_REMOTE_IP2, 
																			TLX_UDP_REMOTE_IP3), 
																	 TLX_UDP_REMOTE_PORT);
	
	if (udp_err != ipal_res_OK)
	{
		/* Opening an UDP Socket */
		printf("Impossible to connect to remote system ...\n");
		
		udp_comm_app_error_handling();
	}
#endif


	//remoteaddress =  IPAL_ipv4addr(10, 0, 1, 104);
	if(ipal_res_OK != ipal_udpsocket_connect(ipal_udp_socket, remoteaddress, remoteport))
	{
        embot::core::print(std::string("Impossible to connect to remote system ...: @ ") + embot::core::TimeFormatter(embot::core::now()).to_string()); 		
		udp_comm_app_error_handling();
	}
    
    
#if TECNOLOGIX_CODE
	printf("Registering receive callback ...\n");
	
	
	/* Register receive callback */
	udp_err = ipal_udpsocket_recv(ipal_udp_socket, udp_rcv_cb_func, NULL);
	
	if (udp_err != ipal_res_OK)
	{
		/* Opening an UDP Socket */
		printf("Impossible to register the receive callback ...\n");
		
		udp_comm_app_error_handling();
	}
    
#endif    
    
    if(ipal_res_OK != ipal_udpsocket_recv(ipal_udp_socket, udp_rcv_cb_func, NULL))
	{
        embot::core::print(std::string("Impossible to register the receive callback ...: @ ") + embot::core::TimeFormatter(embot::core::now()).to_string()); 		
		udp_comm_app_error_handling();
	}
    
    embot::core::print(std::string("socket created, binded, connected and callback registered: @ ") + embot::core::TimeFormatter(embot::core::now()).to_string()); 		
   
    send_datagram();
}

constexpr embot::os::Event evt_ANSWERreceived {1};

void tUDPcomm_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *param)
{
    if(0 == eventmask)
    {   // timeout ... 
        return;
    }
    
    if(true == embot::core::binary::mask::check(eventmask, evt_ANSWERreceived))
    {        
        embot::core::print(std::string("tUDPcomm_onevent(evt_ANSWERreceived): @ ") + embot::core::TimeFormatter(embot::core::now()).to_string());         
    }    
    
}



// service functions

volatile bool answer_received = false;

static void send_datagram()
{
    
//    embot::core::print(std::string("sending datagram: @ ") + embot::core::TimeFormatter(embot::core::now()).to_string()); 
    
	if(ipal_res_OK == ipal_udpsocket_send(ipal_udp_socket, &ipal_send_packet))
    {
        embot::core::print(std::string("succesfully sent datagram: @ ") + embot::core::TimeFormatter(embot::core::now()).to_string());         
    }
    else
    {
        embot::core::print(std::string("FAILED to sent datagram: @ ") + embot::core::TimeFormatter(embot::core::now()).to_string());
    }
    
}

static void udp_rcv_cb_func(void *arg, ipal_udpsocket_t *skt, ipal_packet_t *pkt, ipal_ipv4addr_t adr, ipal_port_t port)
{

    embot::core::print(std::string("udp frame received: @ ") + embot::core::TimeFormatter(embot::core::now()).to_string()); 
    answer_received = true;    
    tUDPcomm->setEvent(evt_ANSWERreceived);

#if TECNOLOGIX_CODE    
	static uint16_union_t old_cnt = { .value = 0};
	uint16_union_t cur_cnt;
	
	uint8_t remote_addr[4];
	
	uint8_t *rcv_buf = (uint8_t *)arg;
	
	memcpy(rcv_buf, pkt->data, pkt->size);
	
	tlx_ipal_IPALaddr_to_array(adr, remote_addr);
	
	answer_received = true;
	
	printf("Received a datagram from %d.%d.%d.%d:%d = %02X %02X\n", 
					remote_addr[0],
					remote_addr[1],
					remote_addr[2],
					remote_addr[3],
					port,
					rcv_buf[0], rcv_buf[1]);

	/* Coherency control */
	cur_cnt.bytes.lsb = rcv_buf[1];
	cur_cnt.bytes.msb = rcv_buf[0];
	
	if (cur_cnt.value != old_cnt.value)
	{
		/* Error HANDLER here */
		dbg_missed_frames++;
	
		old_cnt.value = cur_cnt.value;
		old_cnt.value += 1;		
		/* Error HANDLER here */
	}
	else 
	{
		old_cnt.value += 1;
	}
    
#endif    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


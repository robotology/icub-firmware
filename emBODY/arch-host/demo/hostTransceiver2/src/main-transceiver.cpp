
/* @file       main-transceiver.c
	@brief      This file implements ...
	@author     marco.accame@iit.it
    @date       04/07/2011
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include <string>
#include <signal.h>
#include <iostream>

using namespace std;
// Ace stuff
#include <ace/ACE.h>
#include "ace/SOCK_Dgram.h"
#include "ace/Addr.h"
#include "ace/Thread.h"
#include "ace/Logging_Strategy.h"	// for logging stuff

#include "main-transceiver.hpp"
#include "hostTransceiver.hpp"

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/CanBusInterface.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/RateThread.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include "yarp/dev/ControlBoardInterfacesImpl.inl" //ControlBoardHelper

extern "C" {
#include "EOnv_hid.h"
#include "EoMotionControl.h"
#include "eOcfg_EPs_eb7.h"
}




#define hal_trace_puts(arg)		printf("%s", arg)

// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static uint8_t reset = 0;
static void s_callback_button_1(void);
static void s_callback_button_2(void);
static void s_callback_button_3(void);
static void s_callback_button_4(void);
static void s_callback_button_5(void);

static void s_eom_hostprotoc_extra_protocoltransceiver_ask_the_board(void);

void copyPid2eo(Pid in, eOmc_PID_t *out);
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialization) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


// Connection infos
//DSocket	UDP_socket, UDP_socket2;

ACE_SOCK_Dgram				*ACE_socket;
ACE_SOCK_Dgram				*ACE_socket2;

ACE_UINT16					port;
ACE_INT8					flags = 0;

eOipv4addr_t remoteAddr;
Board_connect_info local, remote01, remote02;
uint8_t need2sendarop = 0;
hostTransceiver *transceiver;


int main(int argc, char *argv[])
{
	char str[SIZE];
	// Program data
	memset(&remote01.data, 0x00, SIZE);

	// Utility stuff
	int i;
	ACE_UINT32 ip1,ip2,ip3,ip4;

	//	Register handler to catch CTRL+C
	if(signal(SIGINT, sighandler) == SIG_IGN)
		signal(SIGINT, SIG_IGN);

	// Set default connection data
	local.address_string=string(DEFAULT_LAPTOP_IP);
	remote01.address_string = string(DEFAULT_EMS_IP);
	port = DEFAULT_PORT;


	uint8_t *udppkt_data = NULL;
	uint16_t udppkt_size = 0;


//	printf("%s", _AC_);

    // parse command line input argument
    if(argc > 1)
    {
    	for(i = 1; i < argc; i++)
    	{

    		if(strcmp("--reply", argv[i]) == 0 )
    		{
    			need2sendarop = 1;
    			printf("I'll reply to any received packet\n");
    			continue;
    		}

    		if(strcmp("--rem-ip", argv[i]) == 0 )
    		{
    			if(i < (argc - 1))
    				remote01.address_string = string(argv[++i]);
    			continue;
    		}

    		if(strcmp("--loc-ip", argv[i]) == 0 )
    		{
    			if(i < (argc - 1))
    				local.address_string = string(argv[++i]);
    			continue;
    		}

    		if(strcmp("--port", argv[i]) == 0 )
    		{
    			if(i < (argc - 1))
    				port = atoi(argv[++i]);
    			continue;
    		}

    		if((strcmp("-h", argv[i]) == 0) || (strcmp("--help", argv[i]) == 0))
    		{
    			usage();
    			return 0;
    		}
    		usage();
    		return ERROR;
    	}
    }

    usage();

    printf("\nConfiguration is : \n\n");
    printf("I need to send a reply = %d\n", need2sendarop);
    // start the udp socket using ace / winsock / psocks
    // Set my own address_string
    sscanf(local.address_string.c_str(),"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);
    local.addr.set(port, (ip1<<24)|(ip2<<16)|(ip3<<8)|ip4 );
    eOipv4addr_t localAddr = eo_common_ipv4addr(ip1,ip2,ip3,ip4);

    printf("local.address: %s\n", local.address_string.c_str());

	ACE_socket = new ACE_SOCK_Dgram();
	if (-1 == ACE_socket->open(local.addr) )
	{
		printf("eStikEtzi pensa che qualcosa non abbia funzionato!!!\n");
		return -1;
	}

    // Set destination address_string
    sscanf(remote01.address_string.c_str(),"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);
    remote01.addr.set(port, (ip1<<24)|(ip2<<16)|(ip3<<8)|ip4 );
    remote02.addr.set(3333, (ip1<<24)|(ip2<<16)|(ip3<<8)|ip4 );


    printf("remote01.address: %s\n", remote01.address_string.c_str());
    printf("port is : %d\n\n", port);

	remoteAddr = eo_common_ipv4addr(ip1,ip2,ip3,ip4); // marco (10, 255, 39, 151)
	eOipv4port_t eOport = port;



    // init object: one per ems
	//hostTransceiver_Init(localAddr,remoteAddr, eOport, EOK_HOSTTRANSCEIVER_capacityofpacket);
	transceiver= new hostTransceiver;
	transceiver->init(localAddr,remoteAddr, eOport, EOK_HOSTTRANSCEIVER_capacityofpacket);

	transceiver->getTransmit(&udppkt_data, &udppkt_size);
    ACE_UINT8		tmp = 1;

    // Start receiver thread
    //pthread_create(&thread, NULL, recvThread, (void*) &remote01);
	ACE_thread_t t_id;
    if(ACE_Thread::spawn((ACE_THR_FUNC)recvThread, NULL, THR_CANCEL_ENABLE, &t_id)==-1)
    	ACE_DEBUG((LM_DEBUG,"Error in spawning thread\n"));


	 // Send a packet to test dummy
	 while(keepGoingOn)
		{
			commands();
			char pressedkey = getchar();
			if (pressedkey != '\n')
			{
				getchar();
				switch (pressedkey)
				{
					case 'q':  	// quit
						keepGoingOn = FALSE;
						break;

					case '1':	//	send one ask rop
						s_callback_button_1();
						break;

					case '2':	// config/reset regular rops
						s_callback_button_2();
						break;

					case '3':	//	send empty rop
						s_callback_button_3();
						break;

					case '4':	//	send a set pid rop
						s_callback_button_4();
						break;

					case '5':	//	send a status sig
						s_callback_button_5();
						break;

					default:
						printf("Command not known -->> sending an empty ropframe!\n");
						break;
				}

				// Get the actual packet and write it into socket using udppkt_data, udppkt_size
				// if the command isn't known, an empty ropframe wil be sent -- I guess
				transceiver->getTransmit(&udppkt_data, &udppkt_size);
				ACE_socket->send(udppkt_data, udppkt_size, remote01.addr, flags);
				printf("Sent EmbObj packet, size = %d\n", udppkt_size);
			}
		}

    //pthread_cancel(thread);
	 ACE_Thread::cancel(t_id);
    return(0);
}


// --------------------------------------------------------------------------------------------------------------------
// - functions with internal scope
// --------------------------------------------------------------------------------------------------------------------

void *recvThread(void * arg)
{
  printf("Thread started\n");

  int  timeout = 2000;
  Board_connect_info		sender;
  ACE_UINT16 				udppkt_size = 0;
  size_t					maxBytes2Read = 512;
  ACE_UINT8					tmp = 0xAA;
  ACE_UINT8					bytesRecv = -1;


  char str[SIZE];


  while (keepGoingOn)
  {
	  // get from sockt
	  sender.addr.get_addr();
	  udppkt_size = ACE_socket->recv((void *) &sender.data, maxBytes2Read, remote01.addr, flags);
	  printf("Received new packet, size = %d\n", udppkt_size);

	  transceiver->SetReceived((ACE_UINT8 *)sender.data, udppkt_size);

	  if(1 == need2sendarop)
	  {
		// eo_ropframe_Clear(p->ropframereply);
		//  hostTransceiver_AddSetROP((ACE_UINT16 ) EOK_cfg_nvsEP_base_endpoint, (ACE_UINT16 ) EOK_cfg_nvsEP_base_NVID__forcerestart, &tmp, 1);
		  transceiver->getTransmit((ACE_UINT8 **)&sender.data, &udppkt_size);
		  transceiver->getTransmit((ACE_UINT8 **)&sender.data, &udppkt_size);  // repeated, just to be sure the packet got at the end is empty

		  // write into skt: udppkt_data, udppkt_size
		  ACE_socket->send(sender.data, udppkt_size, remote01.addr, flags);
		  printf("Sent EmbObj packet, size = %d\n", udppkt_size);
	  }
  }
  //pthread_exit(NULL);
  return NULL;
}



void commands(void)
{
  printf("q: quit\n");
  printf("1: send type 1 packet -> configure regulars (toggling)\n");
  printf("2: send type 2 packet -> set joint configuration\n");
  printf("3: send type 3 packet -> send position setPoint\n");
  printf("4: send type 4 packet -> empty packet\n");
  printf("4: send type 4 packet -> weak up EMS\n");
  printf("\n");
}

void usage(void)
{
  printf("usage: "
		 "\t\t--help		print this help\n");
  printf("\t\t--loc-ip  <xx>	set the ip address of the local machine - needed by yarp - by default %s\n", DEFAULT_LAPTOP_IP);
  printf("\t\t--rem-ip  <xx>	set the ip address of the remote device - by default %s\n", DEFAULT_EMS_IP);
  printf("\t\t--port    <xx>	set the socket port - by default %d\n", DEFAULT_PORT);
  printf("\t\t--reply		if present, then an empty ropframe is sent as an ack for each packet received - by default it's disabled\n");
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of callback functions
// --------------------------------------------------------------------------------------------------------------------

static void s_callback_button_1(void)
{
    char str[128];
	EOnv 		*cnv;
	EOarray *ropsigcfgassign;
    eOropSIGcfg_t sigcfg;



    snprintf(str, sizeof(str)-1, "called set<regulars>");
    hal_trace_puts(str);

//    eo_cfg_nvsEP_mc_endpoint_t ep = endpoint_mngmnt;
//    eo_cfg_nvsEP_mc_jointNVindex_t jNVindex = mngmntNVindex__ropsigcfgassign;

    // get nvid from parameters
//#warning "aggiornare chiamate a funzione"
    eOnvID_t nvid_ropsigcfgassign = eo_cfg_nvsEP_mngmnt_NVID_Get(mngmntNVindex__ropsigcfgassign);
    cnv = transceiver->getNVhandler(endpoint_mngmnt, nvid_ropsigcfgassign);
    ropsigcfgassign = (EOarray*) cnv->loc;

//    EOarray_of_18eOropSIGcfg *ropsigcfgassign;

    eo_array_Reset(ropsigcfgassign);
	eOnvID_t nvid;

 //   if(0 == reset)
    {
        nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(endpoint_mc_leftlowerleg, 0, jointNVindex_jstatus__basic);
    	sigcfg.ep = endpoint_mc_leftlowerleg;
    	sigcfg.id = nvid;
    	sigcfg.plustime = 0;
    	eo_array_PushBack(ropsigcfgassign, &sigcfg);

        nvid = eo_cfg_nvsEP_mc_motor_NVID_Get(endpoint_mc_leftlowerleg, 0, motorNVindex_mstatus__basic);
    	sigcfg.ep = endpoint_mc_leftlowerleg;
    	sigcfg.id = nvid;
    	sigcfg.plustime = 0;
    	eo_array_PushBack(ropsigcfgassign, &sigcfg);

        nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(endpoint_mc_leftlowerleg, 1, jointNVindex_jstatus__basic);
    	sigcfg.ep = endpoint_mc_leftlowerleg;
    	sigcfg.id = nvid;
    	sigcfg.plustime = 0;
    	eo_array_PushBack(ropsigcfgassign, &sigcfg);

        nvid = eo_cfg_nvsEP_mc_motor_NVID_Get(endpoint_mc_leftlowerleg, 1, motorNVindex_mstatus__basic);
    	sigcfg.ep = endpoint_mc_leftlowerleg;
    	sigcfg.id = nvid;
    	sigcfg.plustime = 0;
    	eo_array_PushBack(ropsigcfgassign, &sigcfg);
    }

    transceiver->load_occasional_rop(eo_ropcode_set, endpoint_mngmnt, nvid_ropsigcfgassign);

//    if(1 == reset)
    {
 //       printf( "added a set<__upto10rop2signal, empty-list>");
    }
//    else
    {
        printf("added a set<__upto10rop2signal, list>");
    }

//    reset = (0==reset) ? (1) : (0);
}


static void s_callback_button_2(void )
{
	char str[128];
	EOnv 		*nvRoot;

	eo_cfg_nvsEP_mc_endpoint_t ep = endpoint_mc_leftlowerleg;
	eo_cfg_nvsEP_mc_jointNumber_t j = 0;
	eo_cfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jconfig;

	// get nvid from parameters
    eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
    nvRoot = transceiver->getNVhandler(ep, nvid);

    eOmc_joint_config_t jData;
    memset(&jData, 0x00, sizeof(eOmc_joint_config_t));
    jData.pidposition.kp 				= 0x1AAA;
    jData.pidposition.ki 				= 0x1BBB;
    jData.pidposition.kd 				= 0x1CCC;
    jData.pidposition.limitonintegral 	= 0x1DDD;
    jData.pidposition.limitonoutput 	= 0X1EEE;
    jData.pidposition.offset 			= 0X1FFF;
    jData.pidposition.scale 			= 0X11;
    jData.minpositionofjoint			= 0x22333322;
    jData.controlmode					= 0x44;
	eo_nv_remoteSet(nvRoot, &jData, eo_nv_upd_ifneeded);

	// tell agent to prepare a rop to send
    transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);

    eo_cfg_nvsEP_mc_motorNVindex_t mNVindex = motorNVindex_mconfig;
    nvid = eo_cfg_nvsEP_mc_motor_NVID_Get(ep, j, mNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);

	eOmc_motor_config_t mData;
    memset(&mData, 0x00, sizeof(eOmc_motor_config_t));
	mData.pidcurrent.kp					= 0x6AAA;
	mData.pidcurrent.ki					= 0x6BBB;
	mData.pidcurrent.kd					= 0x6CCC;
	mData.pidcurrent.limitonintegral	= 0x6DDD;
	mData.pidcurrent.limitonoutput		= 0x6EEE;
	mData.pidcurrent.offset				= 0x6FFF;
	mData.pidcurrent.scale				= 0x66;
	mData.maxvelocityofmotor			= 0x77888877;
	mData.maxcurrentofmotor				= 0x9999;

	eo_nv_remoteSet(nvRoot, &mData, eo_nv_upd_ifneeded);

	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);

	snprintf(str, sizeof(str)-1, "called s_callback_button_2: set motort and joint config, index %d\n", j);
}


static void s_callback_button_3(void)
{
	char str[128];
	EOnv 		*nvRoot;

	eo_cfg_nvsEP_mc_endpoint_t ep = endpoint_mc_leftlowerleg;
	eo_cfg_nvsEP_mc_jointNumber_t j = 0;
	eo_cfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jcmmnds__setpoint;

	// get nvid from parameters
	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);

	eOmc_setpoint_t setPoint;
	setPoint.type = eomc_setpoint_position;
	setPoint.to.position.value 			= 0x0610;
	setPoint.to.position.withvelocity 	= 0x6996;

	eo_nv_remoteSet(nvRoot, &setPoint, eo_nv_upd_ifneeded);
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);

	snprintf(str, sizeof(str)-1, "called setPoint position\n");
}

static void s_callback_button_4(void )
{
	char str[128];

	snprintf(str, sizeof(str)-1, "called callback on BUTTON_WKUP: tx a ropframe\n");
}

static void s_callback_button_5(void )
{
	char str[128];
	int j = 0;

	// weak up EMS
	str[0] = 0x01;

	ACE_socket->send(str, 1, remote02.addr, flags);
	sleep(5);

	str[0] = 0x02;
	ACE_socket->send(str, 1, remote02.addr, flags);

	snprintf(str, sizeof(str)-1, "called weak up EMS\n", j);
}

// Utilities

void copyPid2eo(Pid in, eOmc_PID_t *out)
{
	out->kp = in.kp;
	out->ki = in.ki;
	out->kd = in.kd;
	out->limitonintegral = in.max_int;
	out->limitonoutput = in.max_output;
	out->offset = in.offset;
	out->scale = in.scale;
}
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

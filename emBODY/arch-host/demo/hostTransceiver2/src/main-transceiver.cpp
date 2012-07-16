
/* @file       main-transceiver.c
	@brief      This file implements ...
	@author     marco.accame@iit.it
    @date       04/07/2011
 **/

//
// Tested with revision 18333, with defined EO_NVSCFG_USE_CACHED_NVS in prot/src/EOnvsCfg.c
//

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

#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/CanBusInterface.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/RateThread.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include "yarp/dev/ControlBoardInterfacesImpl.inl" //ControlBoardHelper

#include "EOnv_hid.h"
#include "EoMotionControl.h"
#include "EoSkin.h"
#include "EoSensors.h"

#define _AC_
//#include "IRobotInterface.h"
//#include "FeatureInterface.h"
#include "SkinWrapper.h"
#include "debugFunctions.h"

#ifdef _LINUX_UDP_SOCKET_
#include "udp.h"
#endif

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
#define _MAIS_	1



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

static void s_callback_button_0(void);
static void s_callback_button_1(void);
static void s_callback_button_2(void);
static void s_callback_button_3(void);
static void s_callback_button_4(void);
static void s_callback_button_5(void);
static void s_callback_button_6(void );

//static void s_eom_hostprotoc_extra_protocoltransceiver_ask_the_board(void);

void copyPid2eo(Pid in, eOmc_PID_t *out);
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialization) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


// Connection infos
//DSocket	UDP_socket, UDP_socket2;

#ifdef _LINUX_UDP_SOCKET_
Udp							ACE_socket;
Udp							ACE_socket2;
#else
ACE_SOCK_Dgram				*ACE_socket;
ACE_SOCK_Dgram				*ACE_socket2;
#endif

ACE_UINT16					port;
ACE_INT8					flags = 0;

eOipv4addr_t remoteAddr;
Board_connect_info local, remote01, remote02;
uint8_t need2sendarop = 0;
hostTransceiver *transceiver;

uint8_t boardN = 4;
bool double_socket = false;


int main(int argc, char *argv[])
{

	//#define www(sname, ssize)    typedef uint8_t GUARD##sname[ ( ssize == sizeof(sname) ) ? (1) : ( 0x12345678 )];
	//	www(char, 2);

	yarp::os::Network yarp;

	//	EO_VERIFYproposition(name,  (sizeof(int*) == 4));

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

			if((strcmp("-b", argv[i]) == 0) || (strcmp("--board", argv[i]) == 0))
			{
				if(i < (argc - 1))
					boardN = atoi(argv[++i]);
				continue;
			}

			if( (strcmp("--double", argv[i]) == 0) )
			{
				double_socket = true;
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

#ifdef _LINUX_UDP_SOCKET_
	int ret = 0;
	printf("!!!! Using LINUX sockets!!!!\n");
	if(!ACE_socket.initLocal(port, local.address_string.c_str()) )
	{
		printf("Error initing Udp socket!!\n");
		return -1;
	}
	if(!(ret = ACE_socket.connect((char *)remote01.address_string.c_str(), port)))
	{
		printf("Error with UDP connection to socket1-%s:%d\n", (char *)remote01.address_string.c_str(),port);
		return 0;
	}
	else
	{
		ACE_socket.setBufferSize(EOK_HOSTTRANSCEIVER_capacityofpacket);
		printf("done.\n");
	}

	if(double_socket)
	{
		if(!(ret = ACE_socket2.connect((char *)remote01.address_string.c_str(), port, port + 1)))
		{
			printf("Error with UDP connection to socket2 %s:%d\n", (char *)remote01.address_string.c_str(),port);
			return 0;
		}
		else
		{
			ACE_socket2.setBufferSize(EOK_HOSTTRANSCEIVER_capacityofpacket);
			printf("Connected to socket2 %s:%d\n", (char *)remote01.address_string.c_str(),port);
		}
	}

#else
	printf("!!!! Using ACE sockets!!!!\n");
	ACE_socket = new ACE_SOCK_Dgram();
	if (-1 == ACE_socket->open(local.addr) )
	{
		printf("eStikEtzi pensa che qualcosa non abbia funzionato!!!\n");
		return -1;
	}
	int n;
	int m = sizeof(n);
	ACE_socket->get_option(SOL_SOCKET,SO_RCVBUF,(void *)&n, &m);
	printf("SO_RCVBUF %d\n", n);
	ACE_socket->get_option(SOL_SOCKET,SO_SNDBUF,(void *)&n, &m);
	printf("SO_SNDBUF %d\n", n);
	ACE_socket->get_option(SOL_SOCKET,SO_RCVLOWAT,(void *)&n, &m);
	printf("SO_RCVLOWAT %d\n", n);
#endif

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
	transceiver->init(localAddr,remoteAddr, eOport, EOK_HOSTTRANSCEIVER_capacityofpacket, boardN);

	transceiver->getTransmit(&udppkt_data, &udppkt_size);
	ACE_UINT8		tmp = 1;

	// Start receiver thread
	printf("Launching recvThread\n");
	ACE_thread_t id_recvThread;
	if(ACE_Thread::spawn((ACE_THR_FUNC)recvThread, NULL, THR_CANCEL_ENABLE, &id_recvThread)==-1)
		printf(("Error in spawning recvThread\n"));

	printf("Launching skinThread\n");
	ACE_thread_t id_skinThread;
	if(ACE_Thread::spawn((ACE_THR_FUNC)skinThread, NULL, THR_CANCEL_ENABLE, &id_skinThread)==-1)
		printf(("Error in spawning id_skinThread\n"));

	ACE_thread_t id_sendThread;
	if(ACE_Thread::spawn((ACE_THR_FUNC)sendThread, NULL, THR_CANCEL_ENABLE, &id_sendThread)==-1)
		printf(("Error in spawning sendThread\n"));

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

			case '0':
				s_callback_button_0();
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

			case 'p':  	// quit
				print_data();
				break;

			default:
				printf("Command not known -->> sending an empty ropframe!\n");
				break;
			}

			// Get the actual packet and write it into socket using udppkt_data, udppkt_size
			// if the command isn't known, an empty ropframe wil be sent -- I guess
			transceiver->getTransmit(&udppkt_data, &udppkt_size);
			if(!double_socket)
			{
#ifdef _LINUX_UDP_SOCKET_
				ACE_socket.send((char*) &udppkt_data, (ssize_t) udppkt_size);
#else
				ACE_socket->send(udppkt_data, udppkt_size, remote01.addr, flags);
#endif
			}
			else
			{
#ifdef _LINUX_UDP_SOCKET_
				ACE_socket2.send((char*) &udppkt_data, (ssize_t) udppkt_size);
#else
				ACE_socket2->send(udppkt_data, udppkt_size, remote01.addr, flags);
#endif
			}
			printf("Sent EmbObj packet, size = %d\n", udppkt_size);
		}
	}

	print_data();

#ifdef _LINUX_UDP_SOCKET_
	ACE_socket.disconnect();
	//ACE_socket2.disconnect();
#endif

	sleep(2);
	//pthread_cancel(thread);
	ACE_Thread::cancel(id_recvThread);
	ACE_Thread::cancel(id_sendThread);
	ACE_Thread::cancel(id_skinThread);
	return(0);
}


// --------------------------------------------------------------------------------------------------------------------
// - functions with internal scope
// --------------------------------------------------------------------------------------------------------------------

void *recvThread(void * arg)
{
	printf("recvThread started\n");

	FILE 						*outFile			= stdout;
	int  						timeout 			= 2000;
	Board_connect_info			sender;
	ACE_UINT16 					udppkt_size 		= 0;
	size_t						maxBytes2Read 		= 1500;
	ACE_UINT8					tmp = 0xAA;
	ACE_UINT8					bytesRecv 			= -1;
	char 						str[SIZE];

//	ACE_INET_Addr				eb2("10.0.1.2:3333");
//	ACE_INET_Addr				eb4("10.0.1.4:3333");
//
//	uint8_t						board;
//	uint32_t					prevNum[BOARD_NUM]			=	{0};
//	uint32_t					progNum[BOARD_NUM]			=	{0};
//
//	eOabstime_t					txtime;
//	ACE_UINT16 					size_ini[BOARD_NUM] 		= 	{0};
//	EOropframe					*ropFrame;
//
//	ropFrame = eo_ropframe_New();

	sockaddr					sender_addr;

	while(keepGoingOn)
	{
#ifdef _LINUX_UDP_SOCKET_
		udppkt_size = ACE_socket.recv((void *) &sender.data, &sender_addr);
		//sender.addr.set(&sender_addr, 512);
		if(!check_received_pkt(&sender_addr, (void *) sender.data, udppkt_size))
			printf("Error checking the packet!!!\n");
#else
		udppkt_size = ACE_socket->recv((void *) sender.data, maxBytes2Read, sender.addr, flags);

		if(!check_received_pkt(&sender.addr, (void *) sender.data, udppkt_size))
			printf("Error checking the packet!!!\n");
#endif
		//transceiver->SetReceived((ACE_UINT8 *)sender.data, udppkt_size);
	}



	pthread_exit(NULL);
	return NULL;
}


void *sendThread(void * arg)
{
    uint8_t 					*udppkt_data;
   	uint16_t 					udppkt_size;

	printf("sendThread started\n");
	while(keepGoingOn)
	{
		transceiver->getTransmit(&udppkt_data, &udppkt_size);
		if(!double_socket)
		{
#ifdef _LINUX_UDP_SOCKET_
			ACE_socket.send((char*) &udppkt_data, (ssize_t) udppkt_size);
#else
			ACE_socket->send(udppkt_data, udppkt_size, remote01.addr, flags);
#endif
		}
		else
		{
#ifdef _LINUX_UDP_SOCKET_
			ACE_socket2.send((char*) &udppkt_data, (ssize_t) udppkt_size);
#else
			ACE_socket2->send(udppkt_data, udppkt_size, remote01.addr, flags);
#endif
		}

		usleep(1000);
	}
}

void *skinThread(void * arg)
{
	printf("skinThread started\n");
	AnalogServer 		*analogServer;
	int 				period = 20;
	uint16_t			sizze;

	yarp::sig::Vector 	data;
	uint8_t 			msgtype = 0;
	uint32_t 			i, triangle = 0;

	uint32_t sensorsNum=16*12*7;
	data.resize(sensorsNum);

	for (i = 0; i< data.size(); i++ )
		data[i]=255;

	printf("before open\n");
	yarp::os::BufferedPort<yarp::sig::Vector> outPortHand;
	if(!outPortHand.open("/hand"))
	{
		printf("Error, cannot open /hand/test:o YARP port!!\n");
		return NULL;
	}
	else
		printf("Port opened!!\n");

	yarp::os::BufferedPort<yarp::sig::Vector> outPortFAU;
	if(!outPortFAU.open("/fau"))
	{
		printf("Error, cannot open /fau/test:o YARP port!!\n");
		return NULL;
	}
	else
		printf("Port opened!!\n");
	yarp::os::BufferedPort<yarp::sig::Vector> outPortFAL;
	if(!outPortFAL.open("/fal"))
	{
		printf("Error, cannot open outPortFAL YARP port!!\n");
		return NULL;
	}
	else
		printf("Port opened!!\n");

	yarp::os::BufferedPort<yarp::sig::Vector> outPortAI;
	if(!outPortAI.open("/ai"))
	{
		printf("Error, cannot open outPortAI YARP port!!\n");
		return NULL;
	}
	else
		printf("Port opened!!\n");

	yarp::os::BufferedPort<yarp::sig::Vector> outPortAE;
	if(!outPortAE.open("/ae"))
	{
		printf("Error, cannot open outPortAE YARP port!!\n");
		return NULL;
	}
	else
		printf("Port opened!!\n");

	yarp::os::BufferedPort<yarp::sig::Vector> outPortAU;
	if(!outPortAU.open("/au"))
	{
		printf("Error, cannot open outPortAU YARP port!!\n");
		return NULL;
	}
	else
		printf("Port opened!!\n");
	yarp::os::BufferedPort<yarp::sig::Vector> outPortAL;
	if(!outPortAL.open("/al"))
	{
		printf("Error, cannot open outPortAL YARP port!!\n");
		return NULL;
	}
	else
		printf("Port opened!!\n");


	EOarray_of_10canframes sk_array;
	eOnvID_t nvid;
	EOnv	*nvRoot;

	switch (boardN )
	{
	case 2:
		nvid = eo_cfg_nvsEP_sk_NVID_Get(endpoint_sk_emsboard_leftlowerarm, 0, skinNVindex_sstatus__arrayof10canframe);
		nvRoot = transceiver->getNVhandler( endpoint_sk_emsboard_leftlowerarm,  nvid);  //??
		break;

	case 4:
		nvid = eo_cfg_nvsEP_sk_NVID_Get(endpoint_sk_emsboard_rightlowerarm, 0, skinNVindex_sstatus__arrayof10canframe);
		nvRoot = transceiver->getNVhandler( endpoint_sk_emsboard_rightlowerarm,  nvid);  //??
		break;
	}

	bool valid = false;
	uint32_t cardId = 0;
	yarp::sig::Vector WholeData;
	WholeData.clear();

	while(keepGoingOn)
	{
		yarp::sig::Vector &vectorHand 	= outPortHand.prepare();
		yarp::sig::Vector &vectorFAU  	= outPortFAU.prepare();
		yarp::sig::Vector &vectorFAL 	= outPortFAL.prepare();
		yarp::sig::Vector &vectorAI 	= outPortAI.prepare();
		yarp::sig::Vector &vectorAE 	= outPortAE.prepare();
		yarp::sig::Vector &vectorAL 	= outPortAL.prepare();
		yarp::sig::Vector &vectorAU 	= outPortAU.prepare();


//		vectorHand.clear();
//		vectorFAU.clear();
//		vectorFAL.clear();
//		vectorAI.clear();
//		vectorAE.clear();
//		vectorAL.clear();
//		vectorAU.clear();


		//    	transceiver->getNVvalue(nvRoot, (uint8_t *)&sk_array, &sizze);
		yarp::os::Stamp lastStateStamp;				// the last reading time stamp

		memcpy( &sk_array, nvRoot->rem, sizeof(EOarray_of_10canframes));
		//    	if(0 != sk_array.head.size)
		//   		printf("\n--- ARRAY SIZE = %d  ---- \n", sk_array.head.size);



		for(i=0; i<sk_array.head.size; i++)
		{
			eOutil_canframe_t *canframe;
			int  j, mtbId = 0;

			canframe = (eOutil_canframe_t*) &sk_array.data[i*sizeof(eOutil_canframe_t)];
			valid = (((canframe->id & 0x0F00) >> 8) == 3) ? 1 : 0;

			if(valid)
			{
				cardId = (canframe->id & 0x00f0) >> 4;
				switch (cardId)
				{
				case 14:
					mtbId = 0;
					break;
				case 13:
					mtbId = 1;
					break;
				case 12:
					mtbId = 2;
					break;
				case 11:
					mtbId = 3;
					break;
				case 10:
					mtbId = 4;
					break;
				case 9:
					mtbId = 5;
					break;
				case 8:
					mtbId = 6;
					break;
				}
				triangle = (canframe->id & 0x000f);
				msgtype= ((canframe->data[0])& 0x80);

				int index=16*12*mtbId + triangle*12;

				//   			printf("%0X ", canframe->data[0]);
				if (msgtype)
				{
					for(int k=0;k<5;k++)
					{
						data[index+k+7]=canframe->data[k+1];
						//						printf("%0X ", canframe->data[k+1]);
					}
				}
				else
				{
					for(int k=0;k<7;k++)
					{
						data[index+k]=canframe->data[k+1];
						//						printf("%0X ", canframe->data[k+1]);
					}
				}
			}
			//else
			//printf("\nInvalid message\n");
		}

		WholeData = data;
		int first, last;
		// Hand
		first =	0; last = 191;
		vectorHand = WholeData.subVector(first, last);
		// FAU
		first =	192; last = 383;
		vectorFAU = WholeData.subVector(first, last);
		//FAL
		first =	384; last = 575;
		vectorFAL = WholeData.subVector(first, last);
		//AI
		first = 576; last = 767;
		vectorAI = WholeData.subVector(first, last);
		//AE
		first = 768; last = 959;
		vectorAE = WholeData.subVector(first, last);
		//AU
		first = 960; last = 1151;
		vectorAU = WholeData.subVector(first, last);
		//AL
		first =	1152; last = 1343;
		vectorAL = WholeData.subVector(first, last);

		lastStateStamp.update();




		outPortHand.write();
		outPortFAU.write();
		outPortFAL.write();
		outPortAI.write();
		outPortAE.write();
		outPortAL.write();
		outPortAU.write();
		usleep(1000);
	}
	outPortHand.close();
	outPortFAU.close();
	return NULL;
}

void commands(void)
{
	printf("\nq: quit\n");
	printf("0: send wake up packet -> set appl state to running\n");
	printf("1: send config  R/L ARM -> configure regulars\n");
	printf("2: send config  Left ARM -> configure regulars\n");
	printf("3: set leaves packet -> max current and maxposition\n");
	printf("4: send max position setPoint\n");
	printf("5: send position setPoint\n");
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

static void s_callback_button_0(void)
{
	char str[128];
	eOcfg_nvsEP_mn_applNumber_t dummy = 0;
	eOnvID_t nvid_go2state 		= eo_cfg_nvsEP_mn_appl_NVID_Get(endpoint_mn_appl, dummy, applNVindex_cmmnds__go2state);
	EOnv 	*nv_p 				= transceiver->getNVhandler(endpoint_mn_appl, nvid_go2state);
	eOmn_appl_state_t  desired 	= applstate_running;

	printf("nvid = %d (0X%04X", nvid_go2state, nvid_go2state);
	if( eores_OK != eo_nv_Set(nv_p, &desired, eobool_true, eo_nv_upd_dontdo))
		printf("error!!");
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, endpoint_mn_appl, nvid_go2state);
}

static void s_callback_button_1(void)
{
	char str[128];
	EOnv 						*cnv;
	eOmn_ropsigcfg_command_t 	*ropsigcfgassign;
	EOarray						*array;
	eOropSIGcfg_t 				sigcfg;
	eOcfg_nvsEP_mn_commNumber_t dummy = 0;
	eOnvID_t 					nvid = -1;
	EOnv 						*nvRoot = NULL;

	printf("Setting regulars!\n");

	//
	//  1: skin sconfig = 1
	//
	eOnvEP_t 			ep = endpoint_sk_emsboard_rightlowerarm;
	switch (boardN )
	{
	case 2:	// left
		ep = endpoint_sk_emsboard_leftlowerarm;
		printf("left\n");
		break;

	case 4:	// right
		ep = endpoint_sk_emsboard_rightlowerarm;
		printf("right\n");
		break;
	}

	nvid = eo_cfg_nvsEP_sk_NVID_Get((eOcfg_nvsEP_sk_endpoint_t)ep, dummy, skinNVindex_sconfig__sigmode);
	nvRoot = transceiver->getNVhandler(ep, nvid);
	if(NULL == nvRoot)
	{
		printf("\n>>> ERROR \ntransceiver->getNVhandler returned NULL!!\n");
		return;
	}
	uint8_t dat = 1;
	if( eores_OK != eo_nv_Set(nvRoot, &dat, eobool_true, eo_nv_upd_dontdo))
	{
		printf("\n>>> ERROR \neo_nv_Set !!\n");
		return;
	}
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);


	//
	// 2: Segnalazione spontanea
	//
	eOnvID_t nvid_ropsigcfgassign = eo_cfg_nvsEP_mn_comm_NVID_Get(endpoint_mn_comm, dummy, commNVindex__ropsigcfgcommand);
	cnv = transceiver->getNVhandler(endpoint_mn_comm, nvid_ropsigcfgassign);
	ropsigcfgassign = (eOmn_ropsigcfg_command_t*) cnv->loc;
	array = (EOarray*) &ropsigcfgassign->array;
	eo_array_Reset(array);
	array->head.capacity = NUMOFROPSIGCFG;
	array->head.itemsize = sizeof(eOropSIGcfg_t);
	ropsigcfgassign->cmmnd = ropsigcfg_cmd_assign;

	switch (boardN )
	{
	case 2:  // left
		ep = endpoint_mc_leftlowerarm;
		break;

	case 4:	// right
		ep = endpoint_mc_rightlowerarm;
		break;
	}

/*
	nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, 0, jointNVindex_jstatus__basic);
	sigcfg.ep = ep;
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

	nvid = eo_cfg_nvsEP_mc_motor_NVID_Get(ep, 0, motorNVindex_mstatus__basic);
	sigcfg.ep = ep;
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

	nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, 1, jointNVindex_jstatus__basic);
	sigcfg.ep = ep;
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

	nvid = eo_cfg_nvsEP_mc_motor_NVID_Get(ep, 1, motorNVindex_mstatus__basic);
	sigcfg.ep = endpoint_mc_rightlowerarm;
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

	nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, 0, jointNVindex_jconfig);
	sigcfg.ep = ep;
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

	nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, 0, jointNVindex_jconfig__maxpositionofjoint);
	sigcfg.ep = ep;
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

	nvid = eo_cfg_nvsEP_mc_motor_NVID_Get(ep, 0, motorNVindex_mconfig);
	sigcfg.ep = ep;
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);


	sigcfg.ep = ep;
	sigcfg.id = eo_cfg_nvsEP_mc_motor_NVID_Get((eOcfg_nvsEP_mc_endpoint_t)sigcfg.ep, 0, motorNVindex_mconfig__maxcurrentofmotor);
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);
 */

	switch (boardN )
	{
	case 2:  // left
		ep = endpoint_sk_emsboard_leftlowerarm;
		break;

	case 4:	// right
		ep = endpoint_sk_emsboard_rightlowerarm;
		break;
	}
	sigcfg.ep = ep;
	sigcfg.id = eo_cfg_nvsEP_sk_NVID_Get((eOcfg_nvsEP_sk_endpoint_t)sigcfg.ep, 0, skinNVindex_sstatus__arrayof10canframe);
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

#ifdef _MAIS_
	switch (boardN )
	{
	case 2:	// left
		ep = endpoint_as_leftlowerarm;
		break;

	case 4:	// right
		ep = endpoint_as_rightlowerarm;
		break;
	}

	sigcfg.ep = ep;
	sigcfg.id = eo_cfg_nvsEP_as_mais_NVID_Get((eOcfg_nvsEP_as_endpoint_t)sigcfg.ep, dummy, maisNVindex_mstatus__the15values);
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);
#endif
	transceiver->load_occasional_rop(eo_ropcode_set, endpoint_mn_comm, nvid_ropsigcfgassign);
	//	fine della configurazione delle variabili da segnalare spontaneamente : commNVindex__ropsigcfgcommand


#ifdef _MAIS_
	//
	//  3: mais datarate = 1millisec
	//
	switch (boardN )
	{
	case 2:	// left
		ep = endpoint_as_leftlowerarm;
		break;

	case 4:	// right
		ep = endpoint_as_rightlowerarm;
		break;
	}
	nvid = eo_cfg_nvsEP_as_mais_NVID_Get((eOcfg_nvsEP_as_endpoint_t)ep, dummy, maisNVindex_mconfig__datarate);
	nvRoot = transceiver->getNVhandler(ep, nvid);
	dat = 1;
	if( eores_OK != eo_nv_Set(nvRoot, &dat, eobool_true, eo_nv_upd_dontdo))
		printf("error!!");
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);


	//
	//  4: mais datarate = contnuosly
	//
	switch (boardN )
	{
	case 2:	// left
		ep = endpoint_as_leftlowerarm;
		break;

	case 4:	// right
		ep = endpoint_as_rightlowerarm;
		break;
	}
	nvid = eo_cfg_nvsEP_as_mais_NVID_Get((eOcfg_nvsEP_as_endpoint_t)ep, dummy, maisNVindex_mconfig__mode);
	nvRoot = transceiver->getNVhandler(ep, nvid);
	dat = 0;
	if( eores_OK != eo_nv_Set(nvRoot, &dat, eobool_true, eo_nv_upd_dontdo))
		printf("error!!");
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);




#endif
}

static void s_callback_button_2(void )
{
	char str[128];
	int j = 0;

	EOnv 						*cnv;
	eOmn_ropsigcfg_command_t 	*ropsigcfgassign;
	EOarray						*array;
	eOropSIGcfg_t 				sigcfg;
	eOcfg_nvsEP_mn_commNumber_t dummy = 0;


	// get nvid from parameters
	//#warning "aggiornare chiamate a funzione"
	eOnvID_t signal = eo_cfg_nvsEP_sk_NVID_Get(endpoint_sk_emsboard_leftlowerarm, dummy, skinNVindex_sconfig__sigmode);
	EOnv 		*nvRoot;
	nvRoot = transceiver->getNVhandler(endpoint_sk_emsboard_leftlowerarm, signal);

	if(NULL == nvRoot)
	{
		printf("\n>>> ERROR \ntransceiver->getNVhandler returned NULL!!\n");
		return;
	}
	uint8_t dat = 1;
	if( eores_OK != eo_nv_Set(nvRoot, &dat, eobool_true, eo_nv_upd_dontdo))
	{
		printf("\n>>> ERROR \neo_nv_Set!!\n");
		return;
	}
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, endpoint_sk_emsboard_leftlowerarm, signal);

	eOnvID_t nvid_ropsigcfgassign = eo_cfg_nvsEP_mn_comm_NVID_Get(endpoint_mn_comm, dummy, commNVindex__ropsigcfgcommand);
	cnv = transceiver->getNVhandler(endpoint_mn_comm, nvid_ropsigcfgassign);
	ropsigcfgassign = (eOmn_ropsigcfg_command_t*) cnv->loc;
	array = (EOarray*) &ropsigcfgassign->array;
	eo_array_Reset(array);
	array->head.capacity = NUMOFROPSIGCFG;
	array->head.itemsize = sizeof(eOropSIGcfg_t);
	ropsigcfgassign->cmmnd = ropsigcfg_cmd_assign;


	/*
	sigcfg.ep = endpoint_mc_rightlowerarm;
	nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(sigcfg.ep, 0, jointNVindex_jstatus__basic);
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);
	 */


	eOnvID_t nvid;
	sigcfg.ep = endpoint_sk_emsboard_leftlowerarm;
	nvid = eo_cfg_nvsEP_sk_NVID_Get(endpoint_sk_emsboard_leftlowerarm, 0, skinNVindex_sstatus__arrayof10canframe);
	sigcfg.id = nvid;
	sigcfg.plustime = 0;
	eo_array_PushBack(array, &sigcfg);

	transceiver->load_occasional_rop(eo_ropcode_set, endpoint_mn_comm, nvid_ropsigcfgassign);

	printf("added a set<__upto10rop2signal, list>");
}



static void s_callback_button_3(void )
{
	char str[128];
	EOnv 		*nvRoot;
	eOresult_t res;

	//--
	eOcfg_nvsEP_mc_endpoint_t ep = endpoint_mc_rightlowerarm;
	eOcfg_nvsEP_mc_jointNumber_t j = 0;
	eOcfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jconfig__maxpositionofjoint;

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
	jData.maxpositionofjoint			= 0x44555544,
			jData.controlmode					= 0x66;
	if( eores_OK != eo_nv_Set(nvRoot, &jData, eobool_true, eo_nv_upd_dontdo))
		printf("error!!");
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);

	//--
	// get nvid from parameters
	eOcfg_nvsEP_mc_motorNVindex_t mNVindex = motorNVindex_mconfig;
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
	if( eores_OK != eo_nv_Set(nvRoot, &mData, eobool_true, eo_nv_upd_dontdo))
		printf("error!!");
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);

	snprintf(str, sizeof(str)-1, "called set motor and joint config, index %d\n", j);
}

static void s_callback_button_4(void )
{
	char str[128];
	EOnv 		*nvRoot;
	eOmeas_position_t jDataPos;
	memset(&jDataPos, 0x00, sizeof(eOmeas_position_t));

	// joint Max Position
	eOcfg_nvsEP_mc_endpoint_t ep = endpoint_mc_rightlowerarm;
	eOcfg_nvsEP_mc_jointNumber_t j = 0;
	eOcfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jconfig__maxpositionofjoint;
	// get nvid from parameters
	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);
	// Set-up new value
	jDataPos					= 0x44556677;
	if( eores_OK != eo_nv_Set(nvRoot, &jDataPos, eobool_true, eo_nv_upd_dontdo))
		printf("error!!");
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);

	// Motor qualcosa
	//eo_cfg_nvsEP_mc_endpoint_t ep = endpoint_mc_rightlowerarm;
	eOcfg_nvsEP_mc_motorNumber_t j2 = 0;
	eOcfg_nvsEP_mc_motorNVindex_t jNVindex2 = motorNVindex_mconfig__maxcurrentofmotor;
	memset(&jDataPos, 0x00, sizeof(eOmeas_position_t));
	// get nvid from parameters
	nvid = eo_cfg_nvsEP_mc_motor_NVID_Get(ep, j2, jNVindex2);
	nvRoot = transceiver->getNVhandler(ep, nvid);
	// Set-up new value
	jDataPos					= 0xC1A0DAAC;
	if( eores_OK != eo_nv_Set(nvRoot, &jDataPos, eobool_true, eo_nv_upd_dontdo))
		printf("error!!");
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);

	snprintf(str, sizeof(str)-1, "called set jointNVindex_jconfig__maxpositionofjoint\n");
}

static void s_callback_button_5(void)
{
	char str[128];
	EOnv 		*nvRoot;

	eOcfg_nvsEP_mc_endpoint_t ep = endpoint_mc_rightlowerarm;
	eOcfg_nvsEP_mc_jointNumber_t j = 0;
	eOcfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jcmmnds__setpoint;

	// get nvid from parameters
	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);
	nvRoot = transceiver->getNVhandler(ep, nvid);

	eOmc_setpoint_t setPoint;
	setPoint.type = eomc_setpoint_position;
	setPoint.to.position.value 			= 0x0610;
	setPoint.to.position.withvelocity 	= 0x6996;

	eo_nv_Set(nvRoot, &setPoint, eo_nv_upd_ifneeded, eo_nv_upd_dontdo);
	// tell agent to prepare a rop to send
	transceiver->load_occasional_rop(eo_ropcode_set, ep, nvid);

	snprintf(str, sizeof(str)-1, "called setPoint position\n");
}

static void s_callback_button_6(void )
{
	char str[128];
	int j = 0;

	// weak up EMS
#ifdef _LINUX_UDP_SOCKET_
	// not used in this debug
#else
	str[0] = 0x01;

	ACE_socket->send(str, 1, remote02.addr, flags);
	//sleep(5);

	str[0] = 0x02;
	ACE_socket->send(str, 1, remote02.addr, flags);
#endif
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

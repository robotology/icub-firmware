
/* @file       main-transceiver.c
	@brief      This file implements ...
	@author     marco.accame@iit.it
    @date       04/07/2011
 **/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


using namespace std;

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include <string>
#include <signal.h>

// Ace stuff
#include <ace/ACE.h>
#include "ace/SOCK_Dgram.h"
#include "ace/Addr.h"




#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/CanBusInterface.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/RateThread.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include "yarp/dev/ControlBoardInterfacesImpl.inl" //ControlBoardHelper

#include "EOnv_hid.h"
#include "boardTransceiver.hpp"
#include "main_BOARD_transceiver.hpp"
#include "EoMotionControl.h"
#include "EOtheBOARDtransceiver.h"
#include "EoUtilities.h"



//BoardTransceiver 	boardTransceiver;

//#include "eOcfg_nvsEP_base_con.h"
//#include "eOcfg_nvsEP_mngmnt_con.h"
//#include "eOcfg_nvsEP_joint_con.h"

#define hal_trace_puts(arg)		printf("%s", arg)


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_callback_button_1(void);
static void s_callback_button_2(void);
static void s_callback_button_3(void);
static void s_callback_button_4(void);
static void s_callback_button_5(void);


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
ACE_UINT16					port;
ACE_INT8					flags = 0;

EOtransceiver 				*boardTransceiver = NULL;

Board_connect_info local, remote01;
uint8_t need2sendarop = 0;

pthread_t thread;
EOarray				 				*skinData;
uint8_t buffer[sizeof(EOarray_of_10canframes)] = {0};



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
	local.address_string=string(DEFAULT_EMS_IP);
	remote01.address_string = string(DEFAULT_LAPTOP_IP);
	port = DEFAULT_PORT;


	uint8_t *udppkt_data = NULL;
	uint16_t udppkt_size = 0;
	skinData = eo_array_New(10, sizeof(eOutil_canframe_t), buffer);

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

	printf("remote01.address: %s\n", remote01.address_string.c_str());
	printf("port is : %d\n\n", port);

	eOipv4addr_t remoteAddr = eo_common_ipv4addr(ip1,ip2,ip3,ip4); // marco (10, 255, 39, 151)
	eOipv4port_t eOport = port;

	// init object: it's a singleton, so run the program once for each ems you want to simulate
	boardTransceiver = boardTransceiver_new(localAddr,remoteAddr, eOport, EOK_BOARDTRANSCEIVER_capacityofpacket);


	// Start receiver thread
	pthread_create(&thread, NULL, recvThread, (void*) &remote01);

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
			boardTransceiver_GetTransmit(&udppkt_data, &udppkt_size);
			ACE_socket->send(udppkt_data, udppkt_size, remote01.addr, flags);
			printf("Sent EmbObj packet, size = %d\n", udppkt_size);
		}
	}

	pthread_cancel(thread);
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
	int 						ip1, ip2, ip3, ip4;

	char str[SIZE];

	uint8_t *udppkt_data = NULL;

	while (keepGoingOn)
	{
		udppkt_size = ACE_socket->recv((void *) &sender.data, EOK_BOARDTRANSCEIVER_capacityofpacket, remote01.addr, flags);
		printf("Received new packet, size = %d\n", udppkt_size);

		uint16_t remPort 	= remote01.addr.get_port_number();
		string remIp(remote01.addr.get_host_addr());
		sscanf(remIp.c_str(),"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);
		eOipv4addr_t remAddr = eo_common_ipv4addr(ip1,ip2,ip3,ip4);

		SetReceived((ACE_UINT8 *)sender.data, udppkt_size, remAddr, remPort);

		memset(sender.data, 0x00, EOK_BOARDTRANSCEIVER_capacityofpacket);
		// Recupera il pacchetto con una eventuale risposta
		//boardTransceiver_GetTransmit((ACE_UINT8 **)&sender.data, &udppkt_size);
		boardTransceiver_GetTransmit(&udppkt_data, &udppkt_size);

		// se ho una risposta da inviare, la dimensione del pacchetto sarà per forza > 20 byte... a meno che si voglia
		// volutamente mandare un pacchetto vuoto. Modo grezzo per vedere se c'è una risposta da mandare e nel caso farlo.
		// da miglorare
		if(20 < udppkt_size)
		{
			// write into skt: udppkt_data, udppkt_size
			ACE_socket->send(udppkt_data, udppkt_size, remote01.addr, flags);
			printf("Sent EmbObj packet, size = %d\n", udppkt_size);
		}
	}
	pthread_exit(NULL);
	return NULL;
}


void commands(void)
{
	printf("q: quit\n");
	printf("1: send type 1 packet -> ask application info\n");
	printf("2: send type 2 packet -> configure regulars (toggling)\n");
	printf("3: send type 3 packet -> send an empty rop frame\n");
	printf("4: send type 4 packet -> set joint configuration\n");
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


void load_occasional_rop(eOropcode_t opc, uint16_t ep, uint16_t nvid)
{
	eo_transceiver_ropinfo_t ropinfo;

	ropinfo.ropcfg      = eok_ropconfig_basic;
	ropinfo.ropcode     = opc;
	ropinfo.nvep        = ep;

	ropinfo.nvid 		= nvid;
	eo_transceiver_rop_occasional_Load(boardTransceiver, &ropinfo);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of callback functions
// --------------------------------------------------------------------------------------------------------------------



static void s_callback_button_1(void)
{
	char 		str[128];
	eOcfg_nvsEP_mc_endpoint_t ep = endpoint_mc_rightlowerarm;
	eOcfg_nvsEP_mc_jointNumber_t j = 0;
	eOcfg_nvsEP_mc_jointNVindex_t jNVindex = jointNVindex_jstatus__basic;

	// get nvid from parameters
	eOnvID_t nvid = eo_cfg_nvsEP_mc_joint_NVID_Get(ep, j, jNVindex);

	// tell agent to prepare a rop to send
	boardTransceiver_load_occasional_rop(eo_ropcode_sig, ep, nvid);

	snprintf(str, sizeof(str)-1, "signaling jointNVindex_jstatus__basic joint 0\n");
}

static void s_callback_button_2(void )
{
	char str[128];

	uint16_t							size;
	EOnv								*nvRoot = NULL;
	eo_transceiver_ropinfo_t 			ropinfo;
	//	EOarray_of_10canframes 				*skinData = (EOarray_of_10canframes *)nv->rem;
//	eOutil_canframe_t 					*canframe;





	//	ropinfo.nvep = endpoint_sk_emsboard_rightlowerarm;
	//	ropinfo.nvid = nvid;
	eOnvEP_t ep = endpoint_sk_emsboard_rightlowerarm;
	eOnvID_t nvid;

	nvid = eo_cfg_nvsEP_sk_NVID_Get(endpoint_sk_emsboard_rightlowerarm, 0, skinNVindex_sstatus__arrayof10canframe);
	nvRoot = boardTransceiver_getNVhandler(ep, nvid);

#if 1

	//	skinData.head.capacity = 10;
	//	skinData.head.itemsize = 16;
	//	skinData.head.size     = 8;

	uint8_t  boardAddr; // indica la MTB - CardId
	uint8_t  max_mtb 		= 14;
	uint8_t  max_triangle 	= 16;		// N triangoli nella mano destra
	uint8_t  max_pp		 	= 12;

	static uint32_t 	mtb 		= 8;
	static uint32_t	 	triangle 	= 0;  // indica... il triangolo - SensorId
	static uint32_t	 	pp			= 0;
	int index;
	static bool	first	= true;
	eOutil_canframe_t  canframe;

	eo_array_Reset(skinData);
//	for(int j=0; j<skinData->head.capacity; j++)
	{

		canframe.id =  ((mtb<<4) & 0x00f0) | (triangle & 0x000f);
		printf("SID = 0x%04x", canframe.id);
		printf("DATA: ");

		if (first == true)
		{
			canframe.data[0] =  0x80;
			canframe.size = 5;
		}
		else
		{
			canframe.data[0] =  0x00;
			canframe.size = 7;
			triangle++;
		}


		for(int c=1; c<=canframe.size; c++)
		{
			index = mtb*max_triangle*max_pp + triangle*max_pp;
			canframe.data[c] = 170; // 255 - (index+c &0xFF);
			printf(" %0x", canframe.data[c]);
		}

		first = !first;

		if(triangle == max_triangle)
		{
			pp = 0;
			triangle = 0;
			mtb++;
		}

//		if(mtb==max_mtb)
//		{
//			pp=0;
//			triangle=0;
//			mtb=0;
//		}

		printf("\n\n");
		eo_array_PushBack(skinData, &canframe);

	}


	if( eores_OK != eo_nv_Set(nvRoot, skinData, eobool_true, eo_nv_upd_dontdo))
		printf("error!!");

#else

	EOarray_of_10canframes * p = (EOarray_of_10canframes *)nvRoot->loc;
	p->head.capacity = 10;
	p->head.itemsize = 16;
	p->head.size     = 8;


	for(i=0; i<p->head.size; i++)
	{

		canframe = (eOutil_canframe_t*) &p->data[i*sizeof(eOutil_canframe_t)];
		canframe->id = i;
		canframe->size = 5;

		printf("SID = %0x  ", canframe->id);
		printf("DATA: ");
		for(j=0; j<canframe->size;j++)
		{
			canframe->data[j] = 0xA0 + i;
			printf(" %0x", canframe->data[j]);
		}
		printf("\n\n");

	}
#endif

	// tell agent to prepare a rop to send
	boardTransceiver_load_occasional_rop(eo_ropcode_sig, ep, nvid);

	printf("sent a skin frame\n");
}

static void s_callback_button_3(void )
{
	char str[128];

	snprintf(str, sizeof(str)-1, "called callback on BUTTON_WKUP: tx a ropframe\n");
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
	Pid pid;
	snprintf(str, sizeof(str)-1, "called callback on BUTTON_WKUP: tx a ropframe\n");
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

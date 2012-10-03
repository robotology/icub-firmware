
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

#include "main-transceiver.hpp"

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
//#define _MAIS_	1



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
extern bool enableSender;


int main(int argc, char *argv[])
{
	yarp::os::Network yarp;
	char str[SIZE];
	eOipv4port_t eOport;
	ACE_thread_t id_recvThread; //thread manages rx pkt
	ACE_thread_t id_skinThread; //opens yarp port and send data to app skin
	ACE_thread_t id_sendThread; //send data to ems every 1 milli

	// reset buffer remote board
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
	remoteAddr = eo_common_ipv4addr(ip1,ip2,ip3,ip4);

	printf("remote01.address: %s\n", remote01.address_string.c_str());
	printf("port is : %d\n\n", port);
	//check boardN
	if((boardN<1) || (boardN >9))
		boardN = 4;
	printf("ems board is : %d\n\n", boardN);

	remoteAddr = eo_common_ipv4addr(ip1,ip2,ip3,ip4); // marco (10, 255, 39, 151)
	eOport = port;


	// init object: one per ems
	transceiver= new hostTransceiver;
	transceiver->init(localAddr,remoteAddr, eOport, EOK_HOSTTRANSCEIVER_capacityofpacket, boardN);

	transceiver->getTransmit(&udppkt_data, &udppkt_size);


	// Start receiver thread
	printf("Launching recvThread\n");
	if(ACE_Thread::spawn((ACE_THR_FUNC)recvThread, NULL, THR_CANCEL_ENABLE, &id_recvThread)==-1)
		printf(("Error in spawning recvThread\n"));


	// Start skin thread
	printf("Launching skinThread\n");
	if(ACE_Thread::spawn((ACE_THR_FUNC)skinThread, NULL, THR_CANCEL_ENABLE, &id_skinThread)==-1)
		printf(("Error in spawning id_skinThread\n"));

	// Start send thread
	printf("Launching id_sendThread\n");
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

			case '6':	//	send a status sig
				s_callback_button_6();
				break;

			case '7':	//	send a status sig
				s_callback_button_7();
				break;

			case '8':	//	send a status sig
				s_callback_button_8();
				break;

			case '9':	//	send a status sig
				s_callback_button_9();
				break;

			case 'p':  	// print
				print_data();
				break;

			default:
				printf("Command not known -->> sending an empty ropframe!\n");
				break;
			}

			// Get the actual packet and write it into socket using udppkt_data, udppkt_size
			// if the command isn't known, an empty ropframe wil be sent -- I guess
			transceiver->getTransmit(&udppkt_data, &udppkt_size);

#ifdef _LINUX_UDP_SOCKET_
			ACE_socket.send((char*) &udppkt_data, (ssize_t) udppkt_size, 0);
#else
			ACE_socket->send(udppkt_data, udppkt_size, remote01.addr, flags);
#endif

			//printf("Sent EmbObj packet, size = %d\n", udppkt_size);
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
		memset(&sender.data, 0xAA, maxBytes2Read);

#ifdef _LINUX_UDP_SOCKET_
		udppkt_size = ACE_socket.recv((void *) &sender.data, &sender_addr);
		//sender.addr.set(&sender_addr, 512);sendThread
//		if(!check_received_pkt(&sender_addr, (void *) sender.data, udppkt_size))
//			printf("Error checking the packet!!!\n");
#else
		udppkt_size = ACE_socket->recv((void *) sender.data, maxBytes2Read, sender.addr, flags);

//		if(!check_received_pkt(&sender.addr, (void *) sender.data, udppkt_size))
//			printf("Error checking the packet!!!\n");
#endif
		transceiver->SetReceived((ACE_UINT8 *)sender.data, udppkt_size);
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
		if(enableSender)
		{
			transceiver->getTransmit(&udppkt_data, &udppkt_size);
#ifdef _LINUX_UDP_SOCKET_
			ACE_socket.send((char*) &udppkt_data, (ssize_t) udppkt_size, 0);
#else
			ACE_socket->send(udppkt_data, udppkt_size, remote01.addr, flags);
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



void usage(void)
{
	printf("usage: "
			"\t\t--help		print this help\n");
	printf("\t\t--loc-ip  <xx>	set the ip address of the local machine - needed by yarp - by default %s\n", DEFAULT_LAPTOP_IP);
	printf("\t\t--rem-ip  <xx>	set the ip address of the remote device - by default %s\n", DEFAULT_EMS_IP);
	printf("\t\t--port    <xx>	set the socket port - by default %d\n", DEFAULT_PORT);
	printf("\t\t--reply		if present, then an empty ropframe is sent as an ack for each packet received - by default it's disabled\n");
	printf("\t\t--board		indicates the ems used. [1-9] (default=4)\n");
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

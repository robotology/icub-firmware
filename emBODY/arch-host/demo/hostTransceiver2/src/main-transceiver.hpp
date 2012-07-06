/*
 * main-transceiver.hpp
 *
 *  Created on: Feb 20, 2012
 *      Author: icub
 */

#ifndef MAIN_TRANSCEIVER_HPP_
#define MAIN_TRANSCEIVER_HPP_

#define ERROR (-1)
#define OK 		0
#define TRUE	1
#define FALSE	0

#define DEFAULT_LAPTOP_IP	"10.255.37.155" // <- dhcp;   "10.0.0.1" da usare col pc104
//#define DEFAULT_EMS_IP 		"10.255.39.152" // ems
#define DEFAULT_EMS_IP 		"10.255.37.24" // ip della workstation qui dietro.

//#define DEFAULT_LAPTOP_IP		"10.0.0.1"
//#define DEFAULT_EMS_IP 		"10.0.0.2"

#define DEFAULT_PORT		3333
#define SIZE 				1500

#define PC104
#define _DEEP_DEBUG_

// Catch CTRL + C
static int keepGoingOn   = TRUE;

static void sighandler(int _signum)
{
  printf("Sighandler's got signal %d\n", _signum);
  keepGoingOn = FALSE;
}

static void usage(void);
void commands(void);
void *recvThread(void * arg);
void *sendThread(void * arg);
void *skinThread(void * arg);




typedef struct
{
		char field[SIZE];
}Data;

typedef struct
{
	string	 		address_string;
	ACE_INET_Addr	addr;
	char 			data[SIZE];
} Board_connect_info;



#endif /* MAIN_TRANSCEIVER_HPP_ */

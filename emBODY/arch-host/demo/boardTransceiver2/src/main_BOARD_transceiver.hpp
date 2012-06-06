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


#define DEFAULT_PORT				3333
#define STR_SIZE 					512
#define MAX_CAPACITY_OF_PACKET		640

#define _ON_AC_LAP_

#ifdef _ON_PC104_
	#define DEFAULT_LOC_IP 	"10.0.0.1"
	#define DEFAULT_REM_IP 	"10.0.1.1"
#endif

#ifdef _ON_AC_LAP_
	#define DEFAULT_LOC_IP 	"192.168.125.1"
	#define DEFAULT_REM_IP 	"192.168.202.1"
#endif

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include <string>
#include <signal.h>

// Ace stuff
#include <ace/ACE.h>
#include "ace/SOCK_Dgram.h"
#include "ace/Addr.h"


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

typedef struct
{
		char field[MAX_CAPACITY_OF_PACKET];
}Data;

typedef struct
{
	string	 		address_string;
	ACE_INET_Addr	addr;
	char 			data[MAX_CAPACITY_OF_PACKET];
} Board_connect_info;




#endif /* MAIN_TRANSCEIVER_HPP_ */

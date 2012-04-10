
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
#include "string.h"
#include "stdio.h"


// embobj-generic 
#include "EoCommon.h"

#include "hostTransceiver.h"


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

typedef struct MYtest_hid MYtest;


struct MYtest_hid 
{
    uint8_t                 initted;
    uint8_t                 dummy;
    uint32_t                dymmu;
    const void*				cfg;
}; 

typedef struct
{
    uint32_t        aa;
    MYtest          myt;
} mytest1_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static MYtest main_mytest = 
{
    EO_INIT(.initted)       0,
    EO_INIT(.dummy)         1,
    EO_INIT(.dymmu)         0x1234,
    EO_INIT(.cfg)           NULL
};

static const mytest1_t mmm =
{
    EO_INIT(.aa)       0,
    EO_INIT(.myt)       
    {
        EO_INIT(.initted)       1,
        EO_INIT(.dummy)         2,
        EO_INIT(.dymmu)         0x12345678,
        EO_INIT(.cfg)           NULL
    }
};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------




int main(void)
{
    eOipv4addr_t hostaddr = eo_common_ipv4addr(10, 0, 0, 1);
    eOipv4port_t port = 30003;

    eOipv4addr_t oneoftheems_ipaddr = eo_common_ipv4addr(10, 0, 0, 7);

    uint8_t *udppkt_data = NULL;
    uint16_t udppkt_size = 0;
    uint8_t dataarray[512];

    uint8_t need2sendarop = 0;

    printf("hello world\n");

    if(mmm.aa == 0)
    {
        main_mytest.dummy = 3;
    }



    // 1. init object: oen per ems
    hostTransceiver_Init(hostaddr, oneoftheems_ipaddr, port, 512);

    // 2. start the udp socket using ace / winsock / psocks
    // ...


    // 2.bis. initialise tehr egulars

    hostTransceiver_ConfigureRegularsOnRemote();
    hostTransceiver_GetTransmit(&udppkt_data, &udppkt_size);
    // write into skt: udppkt_data, udppkt_size   

    memcpy(dataarray, udppkt_data, udppkt_size);


    // 3. when a udp packet is received ... then call teh following

    for(;;)
    {
        udppkt_data = NULL; // get from skt
        udppkt_size = 0;    // get from skt
        hostTransceiver_SetReceived(udppkt_data, udppkt_size);

        if(1 == need2sendarop)
        {
            hostTransceiver_AddSetROP(43, 56, NULL, 0);
        }

        hostTransceiver_GetTransmit(&udppkt_data, &udppkt_size);

        // write into skt: udppkt_data, udppkt_size  
    }


















    
    for(;;);
    
    return(0);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

  


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




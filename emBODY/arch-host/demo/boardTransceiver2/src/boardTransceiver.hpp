
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _BOARDTRANSCEIVER_H_
#define _BOARDTRANSCEIVER_H_


/** @file       boardTransceiver.h
    @brief      This header file implements public interface to ...
    @author     marco.accame@iit.it
    @date       04/20/2011
 **/

/** @defgroup eo_cedcew Library boardTransceiver
    It is an example of how the embOBJ can operate as host trasceiver.

    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------


#include "EoCommon.h"
#include "EOtheBOARDtransceiver.h"

	// Boards configurations

#include "eOcfg_nvsEP_mn.h"
#include "eOcfg_nvsEP_sk.h"

#include "eOcfg_EPs_eb1.h"
#include "eOcfg_EPs_eb2.h"
#include "eOcfg_EPs_eb3.h"
#include "eOcfg_EPs_eb4.h"
#include "eOcfg_EPs_eb5.h"
#include "eOcfg_EPs_eb6.h"
#include "eOcfg_EPs_eb7.h"
#include "eOcfg_EPs_eb8.h"
#include "eOcfg_EPs_eb9.h"
// the endpoints on that particular ems


#include <yarp/dev/DeviceDriver.h>
using namespace yarp::dev;



static uint32_t            localipaddr;
static uint32_t            remoteipaddr;
static uint16_t            ipport;

static const eOipv4addr_t pc104_ipaddress       = EO_COMMON_IPV4ADDR(10, 255, 39, 152);
static const eOipv4port_t pc104_port  			= 3333;


extern EOtransceiver* boardTransceiver_new(uint32_t _localipaddr, uint32_t _remoteipaddr, uint16_t _ipport, uint16_t _pktsize);

void boardTransceiver_load_occasional_rop(eOropcode_t opc, uint16_t ep, uint16_t nvid);

// somebody passes the received packet
void SetReceived(uint8_t *data, uint16_t size, uint32_t remoteipaddr, uint16_t port);
// and Processes it
void onMsgReception(uint8_t *data, uint16_t size);

// somebody retrieves what must be transmitted
void boardTransceiver_GetTransmit(uint8_t **data, uint16_t *size);

EOnv* boardTransceiver_getNVhandler(uint16_t endpoint, uint16_t id);

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


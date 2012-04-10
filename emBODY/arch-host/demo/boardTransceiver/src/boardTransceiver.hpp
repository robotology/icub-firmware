
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

#ifdef __cplusplus
extern "C" {
#endif

#include "EoCommon.h"
#include "EOtheBOARDtransceiver.h"

// the endpoints on that particular ems
#include "eOcfg_EPs_loc_board_pc104.h"

#ifdef __cplusplus
}
#endif

#include <yarp/dev/DeviceDriver.h>
using namespace yarp::dev;



static uint32_t            localipaddr;
static uint32_t            remoteipaddr;
static uint16_t            ipport;

static const eOipv4addr_t pc104_ipaddress       = EO_COMMON_IPV4ADDR(10, 255, 39, 152);
static const eOipv4port_t pc104_port  			= 3333;


extern EOtransceiver* boardTransceiver_new(uint32_t _localipaddr, uint32_t _remoteipaddr, uint16_t _ipport, uint16_t _pktsize);

// somebody passes the received packet
void SetReceived(uint8_t *data, uint16_t size, uint32_t remoteipaddr, uint16_t port);
// and Processes it
void onMsgReception(uint8_t *data, uint16_t size);

// somebody retrieves what must be transmitted
void boardTransceiver_GetTransmit(uint8_t **data, uint16_t *size);


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


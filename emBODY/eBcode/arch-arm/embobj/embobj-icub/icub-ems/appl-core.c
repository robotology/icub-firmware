/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Alessandro Scalzo
 * email:   alessandro.scalzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

    // acemor: removed
//#include "eupdater_parser.h"


#include "appl-core.h"

#include "stdlib.h"
#include "string.h"

#include "hal.h"
#include "osal.h"
#include "ipal.h"

#include "shalBASE.h"
#include "shalPART.h"
#include "shalINFO.h"
 

typedef enum {
    CMD_SCAN    =0xFF,
    CMD_START   =0x01,
    CMD_DATA    =0x02,
    CMD_JUMP    =0x03,
    CMD_END     =0x04,
    CMD_BOOT    =0x05,
    CMD_RESET   =0x06,
    CMD_IPSET   =0x07,
    CMD_MASKSET =0x08,
    CMD_PROCS   =0x09,
    CMD_SHALS   =0x0A,
    CMD_BLINK   =0x0B,
    CMD_UPD_ONCE=0x0C
} canloader_opcode_t;

enum {
    UPD_OK        = 0,
    UPD_ERR_PROT  = 1,
    UPD_ERR_FLASH = 2,
    UPD_ERR_LOST  = 3,
    UPD_ERR_UNK   = 4,
    UPD_ERR_OOM   = 5
};

#define BOARD_TYPE_EMS 0x0A

void app_core_init(void)
{
    // acemor: removed
//    hal_gpio_init(hal_gpio_portE, hal_gpio_pin13, hal_gpio_dirOUT, hal_gpio_speed_low); 
}

#define PROGRAM_LOADER   0x55
#define PROGRAM_UPDATER  0xAA
#define PROGRAM_APP      0x5A

// acemor: 0 error or nothing to do. 1 ok. 2 ok with need of restart
// acemor: bytes: CMD_SCAN 14, 
uint8_t app_core_manage_cmd(uint8_t *pktin, uint8_t *pktout, uint16_t pktoutcapacity, uint16_t *sizeout)
{
    uint8_t opcode = pktin[1]; // use 0 ....
#warning --> CHANGE IN 0 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    *sizeout=0;
    
    uint8_t retval = 0;

    switch(opcode) // opcode
    {
        case CMD_SCAN:
        {
            //hal_trace_puts("CMD_SCAN");
            
            if(pktoutcapacity < 14)
            {
                *sizeout = 0;
                retval = 1;
            }
            else
            {
                eEmoduleInfo_t* module=(eEmoduleInfo_t*)(EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET);

                *sizeout = 14;

                pktout[0] = CMD_SCAN;
                pktout[1] = module->info.entity.version.major;
                pktout[2] = module->info.entity.version.minor;
                pktout[3] = BOARD_TYPE_EMS;
                
                const eEipnetwork_t *ipnetworkstrg;
                shalinfo_deviceinfo_part_get(shalinfo_ipnet, (const void**)&ipnetworkstrg);

                pktout[4] = (ipnetworkstrg->ipnetmask>>24) & 0xFF;
                pktout[5] = (ipnetworkstrg->ipnetmask>>16) & 0xFF;
                pktout[6] = (ipnetworkstrg->ipnetmask>>8)  & 0xFF;
                pktout[7] =  ipnetworkstrg->ipnetmask      & 0xFF;

                pktout[ 8] = (ipnetworkstrg->macaddress>>40) & 0xFF;
                pktout[ 9] = (ipnetworkstrg->macaddress>>32) & 0xFF;
                pktout[10] = (ipnetworkstrg->macaddress>>24) & 0xFF;
                pktout[11] = (ipnetworkstrg->macaddress>>16) & 0xFF;
                pktout[12] = (ipnetworkstrg->macaddress>>8)  & 0xFF;
                pktout[13] = (ipnetworkstrg->macaddress)     & 0xFF;

                //return 1;
                retval = 2;
            }
        } break;


        case CMD_RESET:
        {
            //hal_trace_puts("CMD_RESET");
            shalbase_system_restart();
            
            retval = 1;
   
        } break;



        case CMD_UPD_ONCE:
        {
     
            shalbase_ipc_gotoproc_set(ee_procUpdater);
            shalbase_system_restart();
            
            retval = 1;
            
        } break;

        default:
        {
            //hal_trace_puts("DEFAULT");
            
            retval = 0;
        } break;
    }

    return retval;
}


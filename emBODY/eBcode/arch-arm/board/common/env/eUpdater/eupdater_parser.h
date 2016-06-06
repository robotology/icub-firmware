/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Alessandro Scalzo
 * email:   marco.accame@iit.it, alessandro.scalzo@iit.it
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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EUPDATERPARSER_H_
#define _EUPDATERPARSER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eupdater_parser.h
    @brief      This header file implements ....
    @author     marco.accame@iit.it
    @date       01/11/2012
**/

/** @defgroup eupdaterinfo cedcew fcevw
    The embENV allows ...... 
 
    @todo acemor-facenda: do documentation.
    

    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOpacket.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------


extern void eupdater_parser_evalwhenjumping(void);

extern eObool_t eupdater_parser_process_ethcmd(EOpacket *rxpkt, EOpacket *txpkt);

extern eObool_t eupdater_parser_stay_forever(void);

extern void eupdater_parser_cangtw_activate(void);

extern eObool_t eupdater_parser_cangtw_isactivated(void);


extern void eupdater_parser_download_blinkled_start(void);
extern void eupdater_parser_download_toggleled(void);
extern void eupdater_parser_download_blinkled_stop(void);
 

/** @}            
    end of group eupdaterparser 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



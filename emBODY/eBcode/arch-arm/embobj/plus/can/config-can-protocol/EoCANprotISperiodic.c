/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EOVtheSystem.h"

#include "stdlib.h"
#include "EoCommon.h"

// in here is whatever is required to offer parsing of can frames and copying into protocol data structures.

#include "EoProtocol.h"
#include "EoProtocolAS.h"

// but also to retrieve information of other things ...

#include "EOtheCANmapping.h"

#include "EOtheInertials2.h"

#include "EOtheInertials3.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheCANprotocol_functions.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


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


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


EO_weak extern eObool_t eocanprotINperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_INERTIAL_MSG(eOcanframe_t *frame, eOcanport_t port)
{
    return(eobool_false);
}

EO_weak extern eObool_t eocanprotINperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_INERTIAL3_MSG(eOcanframe_t *frame, eOcanport_t port)
{
    return(eobool_false);
}

extern eOresult_t eocanprotINperiodic_parser_PER_IS_MSG__DIGITAL_GYROSCOPE(eOcanframe_t *frame, eOcanport_t port)
{
    if(eobool_true == eocanprotINperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_INERTIAL_MSG(frame, port))
    {
        return(eores_OK);
    }
    
    eo_inertials2_AcceptCANframe(eo_inertials2_GetHandle(), frame, port, eoas_inertial_gyros_mtb_ext);
    
    return(eores_OK);
}

extern eOresult_t eocanprotINperiodic_parser_PER_IS_MSG__DIGITAL_ACCELEROMETER(eOcanframe_t *frame, eOcanport_t port)
{
    
    if(eobool_true == eocanprotINperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_INERTIAL_MSG(frame, port))
    {
        return(eores_OK);
    }
    
    eo_inertials2_AcceptCANframe(eo_inertials2_GetHandle(), frame, port, eoas_inertial_accel_mtb_int);
    
    return(eores_OK);    
}


extern eOresult_t eocanprotINperiodic_parser_PER_IS_MSG__IMU_TRIPLE(eOcanframe_t *frame, eOcanport_t port)
{
    if(eobool_true == eocanprotINperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_INERTIAL3_MSG(frame, port))
    {
        return(eores_OK);
    }
       
    eOas_inertial3_type_t type = eoas_inertial3_canproto_to_imu(frame->data[1]);
    if(eoas_inertial3_unknown != type)
    {
        eo_inertials3_AcceptCANframe(eo_inertials3_GetHandle(), frame, port, type);
    }
    
    return(eores_OK);
}

extern eOresult_t eocanprotINperiodic_parser_PER_IS_MSG__IMU_QUATERNION(eOcanframe_t *frame, eOcanport_t port)
{
    if(eobool_true == eocanprotINperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_INERTIAL3_MSG(frame, port))
    {
        return(eores_OK);
    }
    
    eo_inertials3_AcceptCANframe(eo_inertials3_GetHandle(), frame, port, eoas_inertial3_imu_qua);

    return(eores_OK);
}

extern eOresult_t eocanprotINperiodic_parser_PER_IS_MSG__IMU_STATUS(eOcanframe_t *frame, eOcanport_t port)
{
    if(eobool_true == eocanprotINperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_INERTIAL3_MSG(frame, port))
    {
        return(eores_OK);
    }
    

    eo_inertials3_AcceptCANframe(eo_inertials3_GetHandle(), frame, port, eoas_inertial3_imu_status);

    return(eores_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






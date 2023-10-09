
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

//#include "EOVtheSystem.h"

#include "stdlib.h"
#include "EoCommon.h"

// in here is whatever is required to offer parsing of can frames and copying into protocol data structures.

#include "EoProtocol.h"
#include "EoProtocolAS.h"

// but also to retrieve information of other things ...

#include "EOtheCANmapping.h"

//#include "EOtheInertials2.h"
//#include "EOtheInertials3.h"

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


extern eOresult_t eocanprotINperiodic_parser_PER_IS_MSG__DIGITAL_GYROSCOPE(eOcanframe_t *frame, eOcanport_t port)
{
    return(eores_NOK_generic); 
}

extern eOresult_t eocanprotINperiodic_parser_PER_IS_MSG__DIGITAL_ACCELEROMETER(eOcanframe_t *frame, eOcanport_t port)
{
    return(eores_NOK_generic); 
}


extern eOresult_t eocanprotINperiodic_parser_PER_IS_MSG__IMU_TRIPLE(eOcanframe_t *frame, eOcanport_t port)
{
    return(eores_NOK_generic); 
}


extern eOresult_t eocanprotINperiodic_parser_PER_IS_MSG__IMU_QUATERNION(eOcanframe_t *frame, eOcanport_t port)
{
    return(eores_NOK_generic); 
}

extern eOresult_t eocanprotINperiodic_parser_PER_IS_MSG__IMU_STATUS(eOcanframe_t *frame, eOcanport_t port)
{
    return(eores_NOK_generic); 
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







/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - brief
//   it implements embobj C interface to the class embot::app::DiagnosticsNode 
//

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EO_EMBOTIF_DIAGNODE_H_
#define _EO_EMBOTIF_DIAGNODE_H_

#ifdef __cplusplus
extern "C" {
#endif
 

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoManagement.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
 

extern void embotIF_diagnode_init(void);

extern void embotIF_diagnode_add(uint8_t *ropstream, uint16_t size);
extern void embotIF_diagnode_add1(eOmn_info_basic_t *infobasic);
extern void embotIF_diagnode_add2(eOmn_info_basic_t *infobasic);

extern eObool_t embotIF_diagnode_prepare(uint16_t *sizeofropframe);

extern uint16_t embotIF_diagnode_retrieve(uint8_t *ropframe, uint16_t capacityofropframe);


extern void embotIF_TEST_diagnode_init(void);
extern void embotIF_TEST_diagnode_parse(uint8_t *ropframe, uint16_t sizeofropframe);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


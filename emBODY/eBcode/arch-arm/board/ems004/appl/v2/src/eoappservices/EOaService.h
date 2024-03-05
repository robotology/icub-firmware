
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------
#ifndef __EOASERVICE_H_
#define __EOASERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "EoCommon.h"

typedef void EOaService;


// typedef eOresult_t (*eOservice_onendofoperation_fun_t) (EOaService* p, eObool_t operationisok);
typedef void (*eOservice_onendofoperation_fun_t) (EOaService* p, eObool_t operationisok);



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



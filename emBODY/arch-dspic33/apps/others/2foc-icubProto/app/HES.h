
//   
//  Hes management routines
//
#ifndef __HES_H
#define __HES_H

#include <p33FJ128MC802.h>
#include <DSP.h>

#include "UserTypes.h"
#include "Status.h"
#include "iControl.h"
#include "Park.h"
#include "System.h"
#include "Faults.h"


extern unsigned UseHES;
extern tSysError SysError;
extern tCtrlReferences CtrlReferences;extern SFRAC16 TorqueTogglingReference;extern SFRAC16 OmegaTogglingReference;extern tParkParm ParkParm;extern  tGulp Gulp;
#endif

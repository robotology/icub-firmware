
//   
//  Hes management routines
//
#ifndef __DHES_H
#define __DHES_H

#include <p33FJ128MC802.h>
#include <DSP.h>

#include "UserTypes.h"
#include "Status.h"
#include "System.h"
#include "Faults.h"
#include "UserParms.h"

extern tSysError SysError;
extern void DHESInit();
extern SFRAC16 DHESRead();

extern int g_hes_state;

#endif

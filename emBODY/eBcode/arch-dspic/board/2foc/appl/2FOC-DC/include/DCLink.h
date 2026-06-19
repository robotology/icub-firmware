//
// Routines for DC Link checks
// 
#include "UserTypes.h"
#include "System.h"
#include "Faults.h"

#ifndef __DCLINK_H__
#define __DCLINK_H__

void DCLinkReset();
void DCLinkCheck(unsigned int VDCLink100mV);

#endif

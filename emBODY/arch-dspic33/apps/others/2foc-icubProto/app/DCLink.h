//
// Routines for DC Link checks
// 
#include "UserTypes.h"
#include "System.h"
#include "Faults.h"

#ifndef __DCLink_H
#define __DCLink_H

void DCLinkCheck();
void DCLinkReset();

unsigned int ADCVDCLinkTo100mV(SFRAC16 vadc);
void FaultConditionsHandler(void);

extern tSysError SysError;extern unsigned int vdc;
extern volatile SFRAC16 VDCLink; // DC Bus measured continuously and stored in this variable

#endif

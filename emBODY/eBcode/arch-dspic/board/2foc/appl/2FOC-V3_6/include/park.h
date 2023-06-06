 /**********************************************************************
 *                                                                     *
 *                        Software License Agreement                   *
 *                                                                     *
 *    The software supplied herewith by Microchip Technology           *
 *    Incorporated (the "Company") for its dsPIC controller            *
 *    is intended and supplied to you, the Company's customer,         *
 *    for use solely and exclusively on Microchip dsPIC                *
 *    products. The software is owned by the Company and/or its        *
 *    supplier, and is protected under applicable copyright laws. All  *
 *    rights are reserved. Any use in violation of the foregoing       *
 *    restrictions may subject the user to criminal sanctions under    *
 *    applicable laws, as well as to civil liability for the breach of *
 *    the terms and conditions of this license.                        *
 *                                                                     *
 *    THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION.  NO           *
 *    WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING,    *
 *    BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND    *
 *    FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE     *
 *    COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL,  *
 *    INCIDENTAL OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.  *
 *                                                                     *
 ***********************************************************************
 *                                                                     *
 *    Filename:       park.h                                        *
 *    Date:           10/01/08                                         *
 *                                                                     *
 *    Tools used:     MPLAB IDE -> 8.14                                *
 *                    C30 -> 3.10                                      *
 *    Linker File:    p33FJ256MC710.gld                                 *
 *                                                                     *
 **********************************************************************/
#ifndef Park_H
#define Park_H
//------------------  C API for Park Clarke routines ---------------------

typedef struct {
    short   qAngle;
    short   qSin;
    short   qCos;
    short   qIa;
    short   qIb;
    short   qIc;
    short   qIalpha;
    short   qIbeta;
    short   qId;
    short   qIq;
    short   qVd;
    short   qVq;
    short   qValpha;
    short   qVbeta;
    short   qV1;
    short   qV2;
    short   qV3;
    } tParkParm;

//void SinCos(void);      // Calculate qSin,qCos from iAngle
//void ClarkePark(void);  // Calculate qId,qIq from qCos,qSin,qIa,qIb
//void InvPark(void);     // Calculate qValpha, qVbeta from qSin,qCos,qVd,qVq

extern volatile tParkParm ParkParm;
#endif





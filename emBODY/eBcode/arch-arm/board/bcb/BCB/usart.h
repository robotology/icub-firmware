 /****************************************************
 *  BCB board - iCub Facility - 2013                 *
 *  USART configuration                              *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/

#ifndef __SERIAL_H
#define __SERIAL_H

void USART1_Init (void);
void USART1_IRQHandler (void);
void USART2_Init (void);
void USART2_IRQHandler (void);
void buffer_Init (void);
int SendChar1 (int c);
int GetKey1 (void);
int SendChar2 (int c);
int SendUart2 (uint8_t *array, char len);
int GetKey2 (void);

#endif

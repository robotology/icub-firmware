/* 
 * File:   i2cTsens.h
 * Author: Alessandro Scalzo
 *
 * Created on 18 febbraio 2015, 13.46
 */

#ifndef I2CTSENS_H
#define	I2CTSENS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "UserTypes.h"

#define isActiveI2CTsens() (I2C1CONbits.I2CEN)
    
int setupI2CTsens(void);
int readI2CTsens(volatile int* temperature);
// function used for generating synthetic temperature data useful for testing 
void generateI2CTsensSynthetic(volatile int* temperature);

#ifdef	__cplusplus
}
#endif

#endif	/* I2CTSENS_H */

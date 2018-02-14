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

BOOL setupI2CTsens(void);
int  readI2CTsens(void);

#ifdef	__cplusplus
}
#endif

#endif	/* I2CTSENS_H */


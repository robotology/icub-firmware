/* 
 * File:   2FOC.h
 * Author: Alessandro Scalzo
 *
 * Created on 26 marzo 2015, 12.13
 */

#ifndef __2FOC_H__
#define	__2FOC_H__

#ifdef	__cplusplus
extern "C" {
#endif

//#define HAS_QE      0x0001
//#define HAS_HALL    0x0002
//#define HAS_TSENS   0x0004
//#define HAS_INDEX   0x0008

typedef struct
{
  int VqRef;     // Vd (flux) reference value
  int IqRef;     // Vq (torque) reference value
  int WRef;      // Omega (speed) reference value
} tCtrlReferences;

typedef struct
{
    //int Param; // max nominal current. For implementation 1
                 // filter K * 2048. For implementation 2

    //int IThreshold; // Error limit for I2T (implementation 1) or
                      // filtered current threshold for implementation 2

    //unsigned int Acc[3]; // accumulator state (ACCAL, ACCAH, ACCAU)
    int IQMeasured; // measued current value
    int IDMeasured; // measued current value
} tI2T;

typedef union
{
    struct
    {
        unsigned has_qe:1;
        unsigned has_hall:1;
        unsigned has_tsens:1;
        unsigned has_index:1;
        
        unsigned has_speed_qe:1;
        unsigned verbose:1;
        unsigned unused:1;
        unsigned configured:1;
    };

    unsigned int bitmask;
} __attribute__((__packed__)) tMotorConfig;

volatile extern tMotorConfig MotorConfig;

volatile extern tCtrlReferences CtrlReferences;
volatile extern tI2T I2Tdata;
volatile extern unsigned char gControlMode;
volatile extern int gTemperature;
volatile extern unsigned int i2cERRORS;
volatile extern long gQEPosition;
volatile extern int  gQEVelocity;

volatile extern int Ipeak;

extern void DisableDrive(void);
extern void I2Twatcher(void);

extern void setIPid(int kp,int ki, char shift);
extern void setSPid(int kp,int ki, char shift);
extern void setMaxCurrent(int nom, int peak, int ovr);
extern void RequestControlMode(char cm);
extern BOOL updateOdometry();

extern void ZeroControlReferences(void);

volatile extern int gSetpointWatchdog;
#define SETPOINT_WATCHDOG_REARM() gSetpointWatchdog = 1000;

#ifdef	__cplusplus
}
#endif

#endif	/* 2FOC_H */


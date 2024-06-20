/*
 * AD7147.h
 *
 *  Created on: Oct 16, 2015
 *      Author: mmaggiali
 */

#ifndef APPLICATION_USER_AD7147_H_
#define APPLICATION_USER_AD7147_H_

#ifdef __cplusplus
extern "C" {
#endif
    

// - external dependencies --------------------------------------------------------------------------------------------
    
#include "if2hw_common.h"
    
    

// - public #define  --------------------------------------------------------------------------------------------------
    
#define triangles_max_num   16
#define triangles_add_num   4
#define MAXVAL 255
#define MINVAL   0
#define NOLOAD 245
  
    
// - declaration of public user-defined types -------------------------------------------------------------------------
    
enum 
{
    ad7147_triangles_numberof = 20,
    ad7147_dots_numberof = 12,  // in each triangle
    ad7147_dot_value_max = 255,
    ad7147_dot_value_min = 0,
    ad7147_dot_value_noload = 245    
};
    
#if defined(if2hw_common_AD7147_USE_EXTERNALCONTROLTYPES)
// in this case we decide what to uses
#else

typedef struct error_cap
{
    unsigned int error_outofrange;
    unsigned int error;
} error_cap;

enum Errors
    {
      error_ok,
      error_noack,
      error_notconnected
    };


typedef struct
{
    unsigned int                    shift;
    unsigned int                    indexInSet; //triangle index in set
    unsigned int                    setNum; //equal to sda num
    unsigned int                    isToUpdate; //if =1 triangle needs to be recalibrated
    unsigned int                    CDCoffset;
}triangle_cfg_t;

#endif // defined(if2hw_common_AD7147_USE_EXTERNALCONTROLTYPES)



// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

#if defined(if2hw_common_AD7147_USE_EXTERNALDATA)

// we dont have AD7147Registers and CapOffset. 
// use ad7147_get12rawvaluesoftriangle() and ad7147_get12capoffsetsoftriangle() to retrieve their values

#else

extern if2hw_data_ad7147_t AD7147Registers[20][12];	// il primo campo rappresenta il numero dei canali (non c'e' +)
extern if2hw_data_ad7147_t CapOffset[20][12];

#endif

#if defined(if2hw_common_AD7147_USE_EXTERNALCONTROLTYPES)

// we dont have teh tringle config because we manage that externally

#else

extern triangle_cfg_t triangle_cfg_list[16];
extern error_cap err[16];

#endif


// - declaration of extern public functions ---------------------------------------------------------------------------
  
// legacy api
void ServiceAD7147Isr(unsigned char Channel);
void TrianglesInit(unsigned char Channel, uint8_t applycdcoffset);
void SetCDCoffsetOnAllTriangles(unsigned char Channel, if2hw_data_ad7147_t cdcOffset);
     
 
// new api
extern void ad7147_init(
      if2hw_data_ad7147_t ext_rawvalues[][12], if2hw_data_ad7147_t ext_capoffsets[][12],
      if2hw_data_ad7147_t ext_rawvalues5th[][12], if2hw_data_ad7147_t ext_capoffsets5th[][12]);
extern void ad7147_calibrate(void);
extern void ad7147_set_cdcoffset(uint8_t trg, uint16_t cdcoffset);
extern uint16_t ad7147_gettrianglesconnectedmask(void);
extern void ad7147_acquire(void);
extern uint8_t ad7147_istriangleconnected(uint8_t trg);
extern if2hw_data_ad7147_t * ad7147_get12rawvaluesoftriangle(uint16_t trg); 
extern if2hw_data_ad7147_t * ad7147_get12capoffsetsoftriangle(uint16_t trg);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 


#endif /* APPLICATION_USER_AD7147_H_ */


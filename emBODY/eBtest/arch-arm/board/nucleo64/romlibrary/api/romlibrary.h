
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _ROMLIBRARY_H_
#define _ROMLIBRARY_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "stdint.h"


extern uint32_t romlib_hex_numberoflines(void);

extern const char * romlib_hex_line(uint32_t l);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


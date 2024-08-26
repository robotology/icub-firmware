/* Copyright 2022-2023 The MathWorks, Inc. */

/****************************************************
*                                                   *   
* Wrapper functions for CMSIS Custom functions             *
*                                                   *  
****************************************************/

#ifndef MW_CMSIS_IMPL_H
#define MW_CMSIS_IMPL_H

#include "arm_math.h"
#include "rtwtypes.h"

#pragma GCC diagnostic ignored "-Wstrict-aliasing"

#define mw_arm_add_q15_fractional(pSrcA, pSrcB, pDst, blockSize, shiftBits) \
do { \
  uint32_t blkCnt; \
  blkCnt = blockSize >> 2u; \
  q15_t* pSrcAptr = pSrcA; \
  q15_t* pSrcBptr = pSrcB; \
  q15_t* pDstptr = pDst; \
  int8_t shiftBitsF = shiftBits; \
  if(shiftBitsF > 0){ \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)(*pSrcAptr++) + (q31_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),16); \
        *pDstptr++ = (q15_t) __SSAT((((q31_t)(*pSrcAptr++) + (q31_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),16); \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)(*pSrcAptr++) + (q31_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),16); \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)(*pSrcAptr++) + (q31_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),16); \
     	blkCnt--; \
  	} \
  	blkCnt = blockSize % 0x4u; \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)(*pSrcAptr++)  + (q31_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),16); \
    	blkCnt--; \
  	} \
  } \
  else { \
    shiftBitsF = -shiftBits; \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)((*pSrcAptr++) << shiftBitsF) + (q31_t)(*pSrcBptr++)) >> shiftBitsF),16); \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)((*pSrcAptr++) << shiftBitsF) + (q31_t)(*pSrcBptr++)) >> shiftBitsF),16); \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)((*pSrcAptr++) << shiftBitsF) + (q31_t)(*pSrcBptr++)) >> shiftBitsF),16); \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)((*pSrcAptr++) << shiftBitsF) + (q31_t)(*pSrcBptr++)) >> shiftBitsF),16); \
     	blkCnt--; \
  	} \
  	blkCnt = blockSize % 0x4u; \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)((*pSrcAptr++) << shiftBitsF) + (q31_t)(*pSrcBptr++)) >> shiftBitsF),16); \
    	blkCnt--; \
  	} \
  } \
} while (0)

#define mw_arm_sub_q15_fractional(pSrcA, pSrcB, pDst, blockSize, shiftBits)\
do { \
  uint32_t blkCnt; \
  blkCnt = blockSize >> 2u; \
  q15_t* pSrcAptr = pSrcA; \
  q15_t* pSrcBptr = pSrcB; \
  q15_t* pDstptr = pDst; \
  int8_t shiftBitsF = shiftBits; \
  if(shiftBitsF > 0){ \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)(*pSrcAptr++) - (q31_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),16); \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)(*pSrcAptr++) - (q31_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),16); \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)(*pSrcAptr++) - (q31_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),16); \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)(*pSrcAptr++) - (q31_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),16); \
     	blkCnt--; \
  	} \
  	blkCnt = blockSize % 0x4u; \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)(*pSrcAptr++) - (q31_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),16); \
    	blkCnt--; \
  	} \
  } \
  else { \
    shiftBitsF = -shiftBits; \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)((*pSrcAptr++) << shiftBitsF) - (q31_t)(*pSrcBptr++)) >> shiftBitsF),16); \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)((*pSrcAptr++) << shiftBitsF) - (q31_t)(*pSrcBptr++)) >> shiftBitsF),16); \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)((*pSrcAptr++) << shiftBitsF) - (q31_t)(*pSrcBptr++)) >> shiftBitsF),16); \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)((*pSrcAptr++) << shiftBitsF) - (q31_t)(*pSrcBptr++)) >> shiftBitsF),16); \
     	blkCnt--; \
  	} \
  	blkCnt = blockSize % 0x4u; \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q15_t) __SSAT((((q31_t)((*pSrcAptr++) << shiftBitsF) - (q31_t)(*pSrcBptr++)) >> shiftBitsF),16); \
    	blkCnt--; \
  	} \
  } \
} while (0)

#if defined (ARM_MATH_DSP)

#define mw_arm_add_q7_fractional(pSrcA, pSrcB, pDst, blockSize, shiftBits) \
do { \
  uint32_t blkCnt; \
  blkCnt = blockSize >> 2u; \
  q7_t* pSrcAptr = pSrcA; \
  q7_t* pSrcBptr = pSrcB; \
  q7_t* pDstptr = pDst; \
  int8_t shiftBitsF = shiftBits; \
  q31_t out1,out2; \
  q15_t srcA, srcB; \
  if(shiftBitsF > 0){ \
  	while(blkCnt > 0u){ \
    	srcA = (q15_t)(*pSrcAptr++); \
    	srcB = (q15_t)(*pSrcBptr++); \
    	out1 = (__PKHBT(srcA + (srcB << shiftBitsF), (q15_t)(*pSrcAptr++)+(q15_t)(*pSrcBptr++ << shiftBitsF), 16)); \
    	srcA = (q15_t)(*pSrcAptr++); \
    	srcB = (q15_t)(*pSrcBptr++); \
    	out2 = (__PKHBT(srcA + (srcB << shiftBitsF), (q15_t)(*pSrcAptr++)+(q15_t)(*pSrcBptr++ << shiftBitsF), 16)); \
    	*__SIMD32(pDstptr)++ = __PACKq7(__SSAT(((q15_t)out1 >> shiftBitsF), 8), __SSAT((out1 >> (16 + shiftBitsF)), 8) , __SSAT(((q15_t)out2 >> shiftBitsF), 8), __SSAT((out2 >> (16 + shiftBitsF)), 8)); \
     	blkCnt--; \
  	} \
  	blkCnt = blockSize % 0x4u; \
  	while(blkCnt > 0u){ \
		*pDstptr++ = (q7_t) __SSAT((((q15_t)(*pSrcAptr++)  + (q15_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),8); \
    	blkCnt--; \
  	} \
  } \
  else { \
    shiftBitsF = -shiftBits; \
  	while(blkCnt > 0u){ \
    	srcA = (q15_t)(*pSrcAptr++); \
    	srcB = (q15_t)(*pSrcBptr++); \
    	out1 = (__PKHBT((srcA << shiftBitsF)+ srcB, (q15_t)(*pSrcAptr++ << shiftBitsF)+(q15_t)(*pSrcBptr++), 16)); \
    	srcA = (q15_t)(*pSrcAptr++); \
    	srcB = (q15_t)(*pSrcBptr++); \
		out2 = (__PKHBT((srcA << shiftBitsF)+ srcB, (q15_t)(*pSrcAptr++ << shiftBitsF)+(q15_t)(*pSrcBptr++), 16)); \
		*__SIMD32(pDstptr)++ = __PACKq7(__SSAT(((q15_t)out1 >> shiftBitsF), 8), __SSAT((out1 >> (16 + shiftBitsF)), 8) , __SSAT(((q15_t)out2 >> shiftBitsF), 8), __SSAT((out2 >> (16 + shiftBitsF)), 8)); \
     	blkCnt--; \
  	} \
  	blkCnt = blockSize % 0x4u; \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)((*pSrcAptr++) << shiftBitsF) + (q15_t)(*pSrcBptr++)) >> shiftBitsF),8); \
    	blkCnt--; \
  	} \
  } \
} while (0)

#define mw_arm_sub_q7_fractional(pSrcA, pSrcB, pDst, blockSize, shiftBits)\
do { \
  uint32_t blkCnt; \
  blkCnt = blockSize >> 2u; \
  q7_t* pSrcAptr = pSrcA; \
  q7_t* pSrcBptr = pSrcB; \
  q7_t* pDstptr = pDst; \
  int8_t shiftBitsF = shiftBits; \
  q31_t out1,out2; \
  q15_t srcA, srcB; \
  if(shiftBitsF > 0){ \
  	while(blkCnt > 0u){ \
	    srcA = (q15_t)(*pSrcAptr++); \
    	srcB = (q15_t)(*pSrcBptr++); \
    	out1 = (__PKHBT(srcA - (srcB << shiftBitsF), (q15_t)(*pSrcAptr++)-(q15_t)(*pSrcBptr++ << shiftBitsF), 16)); \
    	srcA = (q15_t)(*pSrcAptr++); \
    	srcB = (q15_t)(*pSrcBptr++); \
    	out2 = (__PKHBT(srcA - (srcB << shiftBitsF), (q15_t)(*pSrcAptr++)-(q15_t)(*pSrcBptr++ << shiftBitsF), 16)); \
    	*__SIMD32(pDstptr)++ = __PACKq7(__SSAT(((q15_t)out1 >> shiftBitsF), 8), __SSAT((out1 >> (16 + shiftBitsF)), 8) , __SSAT(((q15_t)out2 >> shiftBitsF), 8), __SSAT((out2 >> (16 + shiftBitsF)), 8)); \
     	blkCnt--; \
  	} \
  	blkCnt = blockSize % 0x4u; \
  	while(blkCnt > 0u){ \
		*pDstptr++ = (q7_t) __SSAT((((q15_t)(*pSrcAptr++)  - (q15_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),8); \
    	blkCnt--; \
  	} \
  } \
  else { \
    shiftBitsF = -shiftBits; \
  	while(blkCnt > 0u){ \
    	srcA = (q15_t)(*pSrcAptr++); \
    	srcB = (q15_t)(*pSrcBptr++); \
    	out1 = (__PKHBT((srcA << shiftBitsF)- srcB, (q15_t)(*pSrcAptr++ << shiftBitsF)-(q15_t)(*pSrcBptr++), 16)); \
    	srcA = (q15_t)(*pSrcAptr++); \
    	srcB = (q15_t)(*pSrcBptr++); \
		out2 = (__PKHBT((srcA << shiftBitsF)- srcB, (q15_t)(*pSrcAptr++ << shiftBitsF)-(q15_t)(*pSrcBptr++), 16)); \
		*__SIMD32(pDstptr)++ = __PACKq7(__SSAT(((q15_t)out1 >> shiftBitsF), 8), __SSAT((out1 >> (16 + shiftBitsF)), 8) , __SSAT(((q15_t)out2 >> shiftBitsF), 8), __SSAT((out2 >> (16 + shiftBitsF)), 8)); \
     	blkCnt--; \
  	} \
  	blkCnt = blockSize % 0x4u; \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)((*pSrcAptr++) << shiftBitsF) - (q15_t)(*pSrcBptr++)) >> shiftBitsF),8); \
    	blkCnt--; \
  	} \
  } \
} while (0)

#else

#define mw_arm_add_q7_fractional(pSrcA, pSrcB, pDst, blockSize, shiftBits) \
do { \
  uint32_t blkCnt; \
  blkCnt = blockSize >> 2u; \
  q7_t* pSrcAptr = pSrcA; \
  q7_t* pSrcBptr = pSrcB; \
  q7_t* pDstptr = pDst; \
  int8_t shiftBitsF = shiftBits; \
  if(shiftBitsF > 0){ \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)(*pSrcAptr++) + (q15_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),8); \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)(*pSrcAptr++) + (q15_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),8); \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)(*pSrcAptr++) + (q15_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),8); \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)(*pSrcAptr++) + (q15_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),8); \
     	blkCnt--; \
  	} \
  	blkCnt = blockSize % 0x4u; \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)(*pSrcAptr++)  + (q15_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),8); \
    	blkCnt--; \
  	} \
  } \
  else { \
    shiftBitsF = -shiftBits; \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)((*pSrcAptr++) << shiftBitsF) + (q15_t)(*pSrcBptr++)) >> shiftBitsF),8); \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)((*pSrcAptr++) << shiftBitsF) + (q15_t)(*pSrcBptr++)) >> shiftBitsF),8); \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)((*pSrcAptr++) << shiftBitsF) + (q15_t)(*pSrcBptr++)) >> shiftBitsF),8); \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)((*pSrcAptr++) << shiftBitsF) + (q15_t)(*pSrcBptr++)) >> shiftBitsF),8); \
     	blkCnt--; \
  	} \
  	blkCnt = blockSize % 0x4u; \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)((*pSrcAptr++) << shiftBitsF) + (q15_t)(*pSrcBptr++)) >> shiftBitsF),8); \
    	blkCnt--; \
  	} \
  } \
} while (0)

#define mw_arm_sub_q7_fractional(pSrcA, pSrcB, pDst, blockSize, shiftBits)\
do { \
  uint32_t blkCnt; \
  blkCnt = blockSize >> 2u; \
  q7_t* pSrcAptr = pSrcA; \
  q7_t* pSrcBptr = pSrcB; \
  q7_t* pDstptr = pDst; \
  int8_t shiftBitsF = shiftBits; \
  if(shiftBitsF > 0){ \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)(*pSrcAptr++) - (q15_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),8); \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)(*pSrcAptr++) - (q15_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),8); \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)(*pSrcAptr++) - (q15_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),8); \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)(*pSrcAptr++) - (q15_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),8); \
     	blkCnt--; \
  	} \
  	blkCnt = blockSize % 0x4u; \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)(*pSrcAptr++)  - (q15_t)((*pSrcBptr++) << shiftBitsF)) >> shiftBitsF),8); \
    	blkCnt--; \
  	} \
  } \
  else { \
    shiftBitsF = -shiftBits; \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)((*pSrcAptr++) << shiftBitsF) - (q15_t)(*pSrcBptr++)) >> shiftBitsF),8); \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)((*pSrcAptr++) << shiftBitsF) - (q15_t)(*pSrcBptr++)) >> shiftBitsF),8); \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)((*pSrcAptr++) << shiftBitsF) - (q15_t)(*pSrcBptr++)) >> shiftBitsF),8); \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)((*pSrcAptr++) << shiftBitsF) - (q15_t)(*pSrcBptr++)) >> shiftBitsF),8); \
     	blkCnt--; \
  	} \
  	blkCnt = blockSize % 0x4u; \
  	while(blkCnt > 0u){ \
    	*pDstptr++ = (q7_t) __SSAT((((q15_t)((*pSrcAptr++) << shiftBitsF) - (q15_t)(*pSrcBptr++)) >> shiftBitsF),8); \
    	blkCnt--; \
  	} \
  } \
} while (0)
#endif


#endif

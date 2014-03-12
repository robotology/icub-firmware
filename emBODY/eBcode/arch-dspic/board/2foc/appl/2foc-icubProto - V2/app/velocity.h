#ifndef __VELOCITY_H
#define __VELOCITY_H

#include "UserParms.h"
#include "UserTypes.h"
#include "system.h"

// TODO: move this three in userparams

// Number of PAST samples (already used once) to keep in the buffer
// Warning! this number influences velocity algo weight!
// When speed is (near to) zero, the algo become more cpu-intensive as
// this number grown! 
#define POSITIONS_BUFFER_PAST_HISTORY 5

// Min number of _real_ samples (not repeated) that we can still accept
// without digging in the past
// Warning, this should never exceed number of samples that the SPI encoder
// is able to provide in one FOC PWM round, otherwise we will always look at
// in the past!
#define VELOCITY_SAMPLE_COUNT_THRESHOLD 10

// free space in the buffer to allow the writer to still put samples
// in the buffer even if the reader is a bit "lazy" 
#define POSITIONS_BUFFER_SAFETY_MARGIN 20

// actual buffer dimension: new samples for each turn (depending by PWM loop freq and VEL loop freq)
// plus the free space explained above, plus the "past samples" space explained above
#define POSITIONS_BUFFER_NUM (IRP_PERCALC + POSITIONS_BUFFER_SAFETY_MARGIN + POSITIONS_BUFFER_PAST_HISTORY)

typedef struct {
  unsigned long positions_buffer[POSITIONS_BUFFER_NUM];
  unsigned int positions_buffer_index;
  // TODO: make main loop check this 
  unsigned int positions_buffer_index_r;
  int Velocity;
  int Initializing;
} tVelocityParm;

void VelocityInit();

extern volatile tVelocityParm VelocityParm;
extern volatile long Current_position;

#endif

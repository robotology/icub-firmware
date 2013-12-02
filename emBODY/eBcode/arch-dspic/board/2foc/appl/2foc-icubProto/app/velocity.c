//
// prende da una array circolare i punti di posizione salvati nell'irq foc utilizzando 
// il puntatore all'ultimo elemento inserito e tornando indietro di n (variabile in funzione del
// numero di punti diversi disponibili) e ne fa la media, indi calcola la vel. sapendo il t
// 

#include <p33FJ128MC802.h>
#include "velocity.h"
#include "status.h"

// TODO: delme extern void OmegaControl(void);
extern tSysStatus SysStatus;

#ifdef COMPLEX_VELOCITY_CALCULATION
// 
//  complex velocity calculation algorithm
//
void __attribute__((__interrupt__, no_auto_psv)) _T2Interrupt(void)
// Timer 2 isr, used to do velocity calculation.
{
  int vel; 
  int space = 0;
  long time;
  int last_idx, start_idx, stop_idx, r_idx;
  int i,cnt;
  long mean;

  // gets a local copy of the last element filled in the buffer
  last_idx = VelocityParm.positions_buffer_index;

  // start and stop indexes for mean calc.
  start_idx = (last_idx - IRP_PERCALC );
  // as mean is calculated on deltas, and the last
  // element has not a "next", then stop one round sooner
  stop_idx = (last_idx - 1);
 
  // as we applied subtractions to indexes, they might
  // had become less than zero. Since we are using a circular
  // buffer, we must recirculate backward in the buffer
  // Note that it can't happen that the number is more negative
  // than POSISIONS_BUFFER_NUM because we subtract 1 or 
  // IRP_PERCALC, and POSITIONS_BUFFER_NUM is defined bigger
  // than IRP_PERCALC 
  if(start_idx < 0)
  {
     start_idx += POSITIONS_BUFFER_NUM;
  }

  if(stop_idx < 0)
  {
     stop_idx += POSITIONS_BUFFER_NUM;
  }

  // initialize mean accumulator
  mean = 0;
  // initialize mean element counter
  cnt = 0;

  // process elements from start_idx to stop_idx in the ring buffer.
  // We increment index in modular fashion
  for(i=start_idx; i!=stop_idx; i = (i+1) % POSITIONS_BUFFER_NUM)
  {
    // calc delta between one sample and it's next
    space = VelocityParm.positions_buffer[(i+1) % POSITIONS_BUFFER_NUM] - 
            VelocityParm.positions_buffer[i];

#ifdef ENCODER_ABS
    // TODO: ? remove dead cats. please read note in as5045.h
    if(space == 0 )
      continue;
#endif

    // accumulate the delta on mean accum
    mean += space;
    // increments divisor for mean;
    cnt++;

    // Do not take other samples if we are running so fast
    // that mean accumulator become huge!
    if(mean > 0x7FFFFFFF)
      break;
  }

  // if we did not collect the expected number of samples (running so slow that 
  // we got a lot of repeated samples. Furthermore we are so much paranoid as 
  // we check for mean accumulator value not to overflow (even if it is unlikely 
  // if we did not get enough samples.....
  if((cnt < VELOCITY_SAMPLE_COUNT_THRESHOLD) && (mean <= 0x7FFFFFFF) )
  { 
    // Look back in the past start and stop indexes for mean calc.
    // we setup start and stop pointers to consider the previous samples, up to
    // POSITIONS_BUFFER_PAST_HISTORY back
    stop_idx = (start_idx - 1);

    if(stop_idx < 0)
    {
      stop_idx += POSITIONS_BUFFER_NUM;
    }

    start_idx = (stop_idx - POSITIONS_BUFFER_PAST_HISTORY);
   
    if(start_idx < 0)
    {
     start_idx += POSITIONS_BUFFER_NUM;
    }

    // Since we might decide to stop when we have enough samples
    // we should go backward.
    // Furthermore as speed decreases, we expect to get very little
    // delta, and lot of equals samples, so we calc delta on
    // a bigger time range (now 3 samples)
    for(i=stop_idx; i!=start_idx; i = i-3)
    {
      // as we are moving backwards..
      if(i < 0)
      {
        i += POSITIONS_BUFFER_NUM;
      }
      // calc delta between one sample and one other (not it's next, we consider a longer time)
      space = VelocityParm.positions_buffer[(i+3) % POSITIONS_BUFFER_NUM] - 
        VelocityParm.positions_buffer[i];

#ifdef ENCODER_ABS
      // remove dead cats. please read note in as5045.h
      if(space == 0 )
        continue;
#endif

      // accumulate the delta on mean accum
      mean += space;
      // increments mean elements counter (we are calculating on 3X time);
      cnt+=3;
      // check if we collected enough samples to calculate mean
      if(cnt >= VELOCITY_SAMPLE_COUNT_THRESHOLD)
        break;
    }
  } 

 // finish calculate mean by dividing by number of elements accumulated
  if(cnt > 1)
    mean /= cnt;
  
  // despite how many samples we used, time is always the same. (we use
  // mean of position change between each sample and it's next)
  time = 1;

  // mean represent space. Calc velocity.
  vel = mean / time;
  
  // adjust the reader index in the ringbuffer.
  // trow away old old samples, but keep the new used this round, as they might be used later again
  // The writer can write safely until it reach this position.
  r_idx = last_idx - POSITIONS_BUFFER_PAST_HISTORY;
  
  // as we did subtractions on the ring buffer pointer.
  if(r_idx < 0)
  {
    r_idx += POSITIONS_BUFFER_NUM;
  }
  
  // update the global copy of the reader index.
  // TODO rx index is currently not used.
  VelocityParm.positions_buffer_index_r = r_idx;
  
  // stores calculated val in the global velocity structure
  // we currently filters a bit more by avaraging with the 
  // previous value; 
  VelocityParm.last_velocity = (vel + VelocityParm.last_velocity)/2;

  // clear isr flag 
  IFS0bits.T2IF = 0; 
}
#endif

#ifndef COMPLEX_VELOCITY_CALCULATION
//
// symplified velocity calculation
// 
void __attribute__((__interrupt__, no_auto_psv)) _T2Interrupt(void)
// Timer 2 isr, used to do velocity calculation.
{

//  unsigned long Current_position;
//  static unsigned long Previous_position;
// Current_position = position;
// VelocityParm.Velocity = Current_position-Previous_position;
// Previous_position = Current_position;

  IFS0bits.T2IF = 0; 
}
#endif

void VelocityInit()
// initialize data for velocity calculation: 
//  starting position in the position buffer
{
// Initializes variables for velocity calculation module
  int i;
  
  // initialize ring buffer to the first buffer position
  VelocityParm.positions_buffer_index = -1;
  
  // initialize ring buffer elem to TO THE CURRENT POSITION! 
  // otherwise the velocity calculation doesn't make sense
  for(i=0;i<POSITIONS_BUFFER_NUM;i++)
  {
    VelocityParm.positions_buffer[i] = Encoder_RotorStartingPosition;
  }
  // avoid inaccurate velocity values prior to first irq completion  
  VelocityParm.Velocity = 0;
}

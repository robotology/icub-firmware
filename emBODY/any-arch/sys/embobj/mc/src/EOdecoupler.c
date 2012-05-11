
/*  @file       EOdecoupler.c
    @brief      This file implements internal implementation of motor-joint decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       07/05/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"

#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
#include "EOVtheSystem.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOdecoupler.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOdecoupler_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOdecoupler";

extern float decoupler_shoulder_pos[4][4] = 
{
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}
};

float decoupler_shoulder_trq[4][4];
float decoupler_shoulder_pwm[4][4];
float decoupler_waist_pwm[4][4];

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOdecoupler* eo_decoupler_New(uint8_t dim, float matrix[4][4]) 
{
    if (dim > DEC_MAX_SIZE) return NULL;

    EOdecoupler *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOdecoupler), 1);

    if (o)
    {
        o->n = dim;

        for (int i=0; i<DEC_MAX_SIZE; ++i)
        {
            for (int j=0; j<DEC_MAX_SIZE; ++j)
            {
                o->M[i][j] = matrix[i][j];
            }
        }
    }

    return o;
}

/*  @fn         extern float eo_trajectory_Step(EOtrajectory *o)
    @brief      Executes a trajectory step.
    @param o    The pointer to the trajectory object.
    @return     The actual trajectory point value.
 **/

extern void eo_decoupler_Mult(EOdecoupler *o, float *src, float *dst)
{
    uint8_t i,j;

    for (i=0; i<o->n; ++i)
    {
        dst[i] = 0.0f;

        for (j=0; j<o->n; ++j)
        {
            dst[i] += o->M[i][j] * src[j];
        }    
    }
}

extern uint8_t eo_decoupler_IsCoupled(EOdecoupler *o, uint8_t joint, uint8_t motor)
{
    return o->M[joint][motor] != 0.0f;
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





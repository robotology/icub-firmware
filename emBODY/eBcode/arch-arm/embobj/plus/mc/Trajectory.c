#include "Trajectory.h"

Trajectory* Trajectory_new(uint8_t n) 
{
    Trajectory *o = NEW(Trajectory, n);

    for (int i=0; i<n; ++i)
    {
        o[i].pos_min = 0;
        o[i].pos_max = TICKS_PER_REVOLUTION;
        o[i].vel_max = TICKS_PER_REVOLUTION/3;
        o[i].acc_max = TICKS_PER_REVOLUTION;
        
        Trajectory_init(o+i, 0, 0, 0);
    }
    
    return o;
}

void Trajectory_config_limits(Trajectory *o, float pos_min, float pos_max, float vel_max, float acc_max)
{
    o->pos_min = pos_min;
    o->pos_max = pos_max;
    if (vel_max != 0.0f) o->vel_max = vel_max;
    if (acc_max != 0.0f) o->acc_max = acc_max;
}

void Trajectory_init(Trajectory *o, int32_t p0, int32_t v0, int32_t a0)
{
    o->xX = p0;
    o->xV = v0;
    o->xA = a0;
    
    o->xTimer = o->xT = 0.0f;
    
    o->vX = 0.0f;
    o->vV = 0.0f;
    o->vA = 0.0f;
    
    o->vTimer = o->vT = 0.0f;
    
    o->bVelocityMove = FALSE;
}

void Trajectory_set_pos_raw(Trajectory *o, float p0)
{
    if (o->pos_min != o->pos_max) LIMIT2(o->pos_min, p0, o->pos_max)
    
    o->xX = p0;
    o->xV = 0.0f;
    o->xA = 0.0f;
    
    o->xTimer = o->xT = 0.0f;
    
    o->vX = 0.0f;
    o->vV = 0.0f;
    o->vA = 0.0f;
    
    o->vTimer = o->vT = 0.0f;
    
    o->bVelocityMove = eobool_false;
}

void Trajectory_set_pos_end(Trajectory *o, /*float x0,*/ float xStar, float velAvg)
{
    //if (velAvg == 0.0f) return FALSE;
    
    if (o->bVelocityMove || (o->vTimer < o->vT)) Trajectory_velocity_stop(o);

    //LIMIT2(o->pos_min, xStar, o->pos_max)
    
    //LIMIT(velAvg, o->vel_max)
    
    ///////////
    //o->xX = x0;
    ///////////
    
    float D = xStar - o->xX;
    
    if ((velAvg>0.0f) ^ (D>0.0f)) velAvg = -velAvg;
    
    //float V = 2.0f*velAvg;

    velAvg *= 2.0f;
    
    //o->xT = D/V;
    o->xT = D/velAvg;
    
    if (o->xT < CTRL_LOOP_PERIOD)
    {
        o->xTimer = o->xT = 0.0f;
        
        o->xX = (float)xStar;
		
		o->xV = o->xA = 0.0f;
        
        return;
    }
    
    o->xTimer = -o->xT;
    
    o->xInvT = 1.0f/o->xT;
    
    o->xK2 =  0.75f*(o->xV*o->xInvT+o->xA);
    //o->xK3 = -3.75f*(o->xV-V)*o->xInvT-1.25f*o->xA;
    o->xK3 = -3.75f*(o->xV-velAvg)*o->xInvT-1.25f*o->xA;
    float O5A = 0.5f*o->xA;
    o->xK0 = -o->xK2+O5A;
    o->xK1 = -o->xK3-O5A;
}

void Trajectory_set_vel_end(Trajectory *o, float vStar, float accAvg)
{
    o->bVelocityMove = eobool_true;

    //LIMIT(vStar, o->vel_max)
    
    //if (accAvg == 0.0f) return;
    
    //LIMIT(accAvg, o->acc_max)

    float D = vStar - o->vV;
    
    if ((accAvg>0.0f) ^ (D>0.0f)) accAvg=-accAvg;

    //float A = (float)(2*accAvg);
    
    accAvg *= 2.0f;
    
    o->vT = D/accAvg;

    if (o->vT < CTRL_LOOP_PERIOD)
    {
        o->vTimer = o->vT = 0.0f;
        
        o->vV = vStar;
		
		o->vA = 0.0f;  
        
        return;
    }
    
    o->vTimer = -o->vT;
    o->vInvT  = 1.0f/o->vT;
    
    o->vK1 = -0.5f*o->vA;
    o->vK0 = 0.75f*accAvg+0.5f*o->vK1;
    o->vK2 = -o->vK0-o->vK1;
}

void Trajectory_stop(Trajectory *o, int32_t pos)
{
    Trajectory_set_pos_raw(o, pos);
}

void Trajectory_velocity_stop(Trajectory *o)
{
    o->bVelocityMove = eobool_false;
    
    o->vTimer = o->vT = 0.0f;

    o->xX += o->vX;
    //o->xV += o->vV;
    //o->xA += o->vA;

    o->vX = o->vV = o->vA = 0.0f;    
}

//////////////////////////////////////////////////////////////////////////////////

int8_t Trajectory_do_step(Trajectory* o, float *p, float *v, float *a)
{
    if (o->xTimer < o->xT)
    {
        float t = o->xTimer*o->xInvT;
        
        o->xA = (((o->xK3*t+o->xK2)*t+o->xK1)*t)+o->xK0;
        o->xV += CTRL_LOOP_PERIOD*o->xA;
        o->xX += CTRL_LOOP_PERIOD*o->xV;
        
        o->xTimer += CTRL_LOOP_PERIOD;
    }
    else
    {
        o->xV = o->xA = 0.0f;
    }
    
    if (o->vTimer < o->vT)
    {
        float t = o->vTimer*o->vInvT;
        
        o->vA = ((o->vK2*t+o->vK1)*t)+o->vK0;
        o->vV += CTRL_LOOP_PERIOD*o->vA;
        o->vX += CTRL_LOOP_PERIOD*o->vV;

        *p = o->xX + o->vX;
        *v = o->xV + o->vV;
        *a = o->xA + o->vA;
        
        o->vTimer += CTRL_LOOP_PERIOD;
    }
    else if (o->bVelocityMove)
    {
        o->vA = 0.0f;
        o->vX += CTRL_LOOP_PERIOD*o->vV;

        *p = o->xX + o->vX;
        *v = o->xV + o->vV;
        *a = o->xA;
    }
    else
    {
        *p = o->xX;
        *v = o->xV;
        *a = o->xA;
    }
    
    if (o->pos_min != o->pos_max)
    {    
        if (*p <= o->pos_min)
        {
            if (*v < 0.0f)
            {
                if (o->bVelocityMove || (o->vTimer < o->vT)) Trajectory_velocity_stop(o);

                *v = 0.0f;
            }
		
            if (*a < 0.0f) *a = 0.0f;
        
            *p = o->pos_min;
        
            return -1;
        }
        else if (*p >= o->pos_max)
        {
            if (*v > 0.0f)
            {
                if (o->bVelocityMove || (o->vTimer < o->vT)) Trajectory_velocity_stop(o);
          
                *v = 0.0f;
            }
		
            if (*a > 0.0f) *a = 0.0f;
             
            *p = o->pos_max;
        
            return  1;
        }
    }
    
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////

BOOL Trajectory_is_done(Trajectory* o)
{
    return o->xTimer >= o->xT;
}

int32_t Trajectory_get_pos_ref(Trajectory* o)
{
    return (int32_t)(o->xX + o->vX);
}

int32_t Trajectory_get_vel_ref(Trajectory* o)
{
    return (int32_t)(o->xV + o->vV);
}

int32_t Trajectory_get_acc_ref(Trajectory* o)
{
    return (int32_t)(o->xA + o->vA);
}

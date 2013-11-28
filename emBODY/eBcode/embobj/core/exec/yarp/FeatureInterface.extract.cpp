
// ACE stuff
#include <ace/ACE.h>
#include <ace/config.h>
#include <ace/Recursive_Thread_Mutex.h>



// returns a void pointer to the allocated ACE_Recursive_Thread_Mutex 
void* ace_mutex_new(void)
{
    return(ACE_Recursive_Thread_Mutex());
}

// returns 0 on success to take mutex, -3 on failure upon timeout, -2 on failure upon null pointer. m is pointer obtained w/ ace_mutex_new(), tout_usec is in microsec (no timeout is 0xffffffff).
int8_t ace_mutex_take(void* m, uint32_t tout_usec)
{
    ACE_Recursive_Thread_Mutex* acemtx = (ACE_Recursive_Thread_Mutex*)m;
    if(NULL == acemtx)
    {
        return(-2);
    }
    
    acemtx->acquire();
    
    return(0);
}

// returns 0 on success to take mutex, -1 on genric failure of releasing mutex, -2 on failure upon null pointer. m is pointer obtained w/ ace_mutex_new(),
int8_t ace_mutex_release(void* m)
{
    ACE_Recursive_Thread_Mutex* acemtx = (ACE_Recursive_Thread_Mutex*)m;
    if(NULL == acemtx)
    {
        return(-2);
    }
    
    acemtx->release();
    
    return(0);
}


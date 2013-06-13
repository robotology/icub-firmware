/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "string.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOmatrix3d.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOmatrix3d_hid.h" 


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

//static const char s_eobj_ownname[] = "EOmatrix3d";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOmatrix3d* eo_matrix3d_New(uint16_t sizeofitem, uint16_t capacity1)
{
    EOmatrix3d *retptr = NULL;    

    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eOmatrix3d_head_t) + capacity1*sizeof(eOmatrix3d_node_t*), 1);


    retptr->head.capacity       = capacity1;
    retptr->head.itemsize       = sizeofitem;
    retptr->head.size           = 0;

    return(retptr);
}

extern eOresult_t eo_matrix3d_Level1_PushBack(EOmatrix3d *p, uint16_t capacity2)
{
    eOmatrix3d_node_t *ptr = NULL;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(p->head.size == p->head.capacity)
    {
        return(eores_NOK_generic);
    }
    

    ptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eOmatrix3d_head_t) + capacity2*sizeof(eOmatrix3d_node_t*), 1);
    ptr->head.capacity          = capacity2;
    ptr->head.itemsize          = p->head.itemsize;
    ptr->head.size              = 0;
    
    p->node[p->head.size] =          ptr;
    p->head.size++;
    
    return(eores_OK);
}

extern eOresult_t eo_matrix3d_Level2_PushBack(EOmatrix3d *p, uint16_t onindex1, uint16_t capacity3)
{
    eOmatrix3d_node_t *ptri1 = NULL;
    eOmatrix3d_end_t *ptre = NULL;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(onindex1 >= p->head.size)
    {
        return(eores_NOK_generic);
    }
    ptri1 = p->node[onindex1];
 
    if(ptri1->head.size == ptri1->head.capacity)
    {
        return(eores_NOK_generic);
    }
    
    
    ptre = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eOmatrix3d_head_t) + capacity3*p->head.itemsize, 1);
    ptre->head.capacity          = capacity3;
    ptre->head.itemsize          = p->head.itemsize;
    ptre->head.size              = 0;
    
    ptri1->node[ptri1->head.size] =  (eOmatrix3d_node_t*) ptre;
    ptri1->head.size++;    
    
    return(eores_OK);
}

extern eOresult_t eo_matrix3d_Level3_PushBack(EOmatrix3d *p, uint16_t onindex1, uint16_t onindex2, void *pitem)
{
    eOmatrix3d_node_t *ptri1 = NULL;
    eOmatrix3d_node_t *ptri2 = NULL;
    eOmatrix3d_end_t *ptre = NULL;
     
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(onindex1 >= p->head.size)
    {
        return(eores_NOK_generic);
    }
    ptri1 = p->node[onindex1];
    
    if(onindex2 >= ptri1->head.size)
    {
        return(eores_NOK_generic);
    } 
    ptri2 = ptri1->node[onindex2];    
 
    if(ptri2->head.size == ptri2->head.capacity)
    {
        return(eores_NOK_generic);
    }

    ptre = (eOmatrix3d_end_t*)ptri2;
    
    memcpy(&(ptre->data[ptre->head.size*p->head.itemsize]), pitem, p->head.itemsize); 
    ptre->head.size++;
    
    return(eores_OK);
}

extern void* eo_matrix3d_At(EOmatrix3d *p, uint16_t i1, uint16_t i2, uint16_t i3)
{
    eOmatrix3d_node_t *ptri1 = NULL;
    eOmatrix3d_node_t *ptri2 = NULL;
    eOmatrix3d_end_t  *ptre  = NULL;
    
    if(NULL == p)
    {
        return(NULL);
    }
    
    if(i1 >= p->head.size)
    {
        return(NULL);
    }
    ptri1 = p->node[i1];
    
    if(i2 >= ptri1->head.size)
    {
        return(NULL);
    } 
    ptri2 = ptri1->node[i2];    
 
    if(i3 >= ptri2->head.size)
    {
        return(NULL);
    }
    
    ptre = (eOmatrix3d_end_t*) ptri2;
    
    
    return(&(ptre->data[i3*p->head.itemsize]));
}

extern uint16_t eo_matrix3d_ItemSize(EOmatrix3d *p)
{
    if(NULL == p)
    {
        return(0);
    }
 
    return(p->head.itemsize);
}

extern uint16_t eo_matrix3d_Level1_Capacity(EOmatrix3d *p)
{  
    if(NULL == p)
    {
        return(0);
    }
    return(p->head.capacity);

}

extern uint16_t eo_matrix3d_Level1_Size(EOmatrix3d *p)
{
    if(NULL == p)
    {
        return(0);
    }
    return(p->head.size);

}

extern uint16_t eo_matrix3d_Level2_Capacity(EOmatrix3d *p, uint16_t onindex1)
{
    eOmatrix3d_node_t *ptri1 = NULL;
    
    if(NULL == p)
    {
        return(0);
    }
    if(onindex1 >= p->head.size)
    {
        return(0);
    }
    ptri1 = p->node[onindex1];  
    
    return(ptri1->head.capacity);
}

extern uint16_t eo_matrix3d_Level2_Size(EOmatrix3d *p, uint16_t onindex1)
{
    eOmatrix3d_node_t *ptri1 = NULL;
    
    if(NULL == p)
    {
        return(0);
    }
    if(onindex1 >= p->head.size)
    {
        return(0);
    }
    ptri1 = p->node[onindex1]; 
    
    return(ptri1->head.size);   
}

extern uint16_t eo_matrix3d_Level3_Capacity(EOmatrix3d *p, uint16_t onindex1, uint16_t onindex2)
{
    eOmatrix3d_node_t *ptri1 = NULL;
    eOmatrix3d_node_t *ptri2 = NULL;
    
    if(NULL == p)
    {
        return(0);
    }
    if(onindex1 >= p->head.size)
    {
        return(0);
    }
    ptri1 = p->node[onindex1]; 
    if(onindex2 >= ptri1->head.size)
    {
        return(0);
    } 
    ptri2 = ptri1->node[onindex2];     
        
    return(ptri2->head.capacity);   
}

extern uint16_t eo_matrix3d_Level3_Size(EOmatrix3d *p, uint16_t onindex1, uint16_t onindex2)
{
    eOmatrix3d_node_t *ptri1 = NULL;
    eOmatrix3d_node_t *ptri2 = NULL;
    
    if(NULL == p)
    {
        return(0);
    }
    if(onindex1 >= p->head.size)
    {
        return(0);
    }
    ptri1 = p->node[onindex1]; 
    if(onindex2 >= ptri1->head.size)
    {
        return(0);
    } 
    ptri2 = ptri1->node[onindex2];     
        
    return(ptri2->head.size);   
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





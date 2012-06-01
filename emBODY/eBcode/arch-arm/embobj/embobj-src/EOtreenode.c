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




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtreenode.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtreenode_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------



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

//static const char s_eobj_ownname[] = "EOnv";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern uint16_t eo_treenode_GetIndex(EOtreenode *node)
{
    if(NULL == node)
    {
        return(EOK_uint16dummy);
    }
    
    return(node->index);
}

extern void* eo_treenode_GetData(EOtreenode *node)
{
    if(NULL == node)
    {
        return(NULL);
    }
    
    return(node->data);
}

extern uint8_t eo_treenode_GetNumberOfChilden(EOtreenode *node)
{
    if(NULL == node)
    {
        return(0);
    }
    
    return(node->nchildren);
}

// extern uint16_t eo_treenode_GetIndexOfChild(EOtreenode *node, uint8_t childpos)
// {
//     if((NULL == node) || (childpos >= node->nchildren))
//     {
//         return(EOK_uint16dummy);
//     }
//     
//     return(node->ichildren[childpos]);
// }


extern EOtreenode* eo_treenode_GetChild(EOtreenode *node, uint8_t childpos)
{
    if((NULL == node) || (childpos >= node->nchildren))
    {
        return(NULL);
    }
    
    EOtreenode* root = node - node->index;
    
//    return(node->pchildren[childpos]);                          // needs more const data
//    return(&node->root[node->ichildren[childpos]]);             // uses less const data, but makes one more dereference
    return(&root[node->ichildren[childpos]]);
}


extern eObool_t eo_treenode_isLeaf(EOtreenode *node)
{
    if((NULL == node) || (0 != node->nchildren))
    {
        return(eobool_false);
    }
    else
    {
        return(eobool_true);
    }
} 

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

//extern EOtreenode * eo_treenode_hid_New(void)
//{
//    EOtreenode *retptr = NULL;    
//
//    // i get the memory for the object
//    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOtreenode), 1);
//    
//    retptr->nchildren = 0;
//    memset(retptr->children, EOK_uint08dummy, EOTREENODE_NCHILDREN);
//    retptr->nv = NULL;
//    
//    return(retptr);
//}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





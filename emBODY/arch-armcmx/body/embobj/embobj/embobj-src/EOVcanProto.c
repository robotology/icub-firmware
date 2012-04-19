
// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "string.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
#include "EOconstLookupTbl.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOVcanProto.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOVcanProto_hid.h" 


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
static eOresult_t s_eov_canFrame_parseCanFrame_dummy(EOVcanProtoDerived *d, eOcanframe_t *frame, eOcanport_t canPortRx);
static eOresult_t s_eov_canFrame_formCanFrame_dummy(EOVcanProtoDerived *d, eo_canProto_msgCommand_t command, 
											eo_canProto_msgDestination_t dest, void *value,
											eOcanframe_t *frame);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOVcanProto";



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern eOpurevirtual eOresult_t eov_canProto_ParseCanFrame(EOVcanProtoDerived *d, eOcanframe_t *frame, eOcanport_t canPortRx)
{   
    EOVcanProto 					*canProto;
    eOres_fp_canProto_parserFn_t 		fptr;

    
    canProto = eo_common_getbaseobject(d);
	
	if(NULL == canProto) 
	{
		return(eores_NOK_nullpointer); 
	}

    // get take function
    fptr = (eOres_fp_canProto_parserFn_t)canProto->vtable[VF00_parse]; 

    // call funtion of derived object. it cant be NULL
    return(fptr(d,frame, canPortRx));
}


extern eOpurevirtual eOresult_t eov_canProto_FormCanFrame(EOVcanProtoDerived *d, eo_canProto_msgCommand_t command, 
											eo_canProto_msgDestination_t dest, void *value,
											eOcanframe_t *frame)
{
    EOVcanProto 					*canProto;
    eOres_fp_canProto_formerFn_t 		fptr;

    
    canProto = eo_common_getbaseobject(d);
	
	if(NULL == canProto) 
	{
		return(eores_NOK_nullpointer); 
	}

    // get take function
    fptr = (eOres_fp_canProto_formerFn_t)canProto->vtable[VF01_form]; 

    // call funtion of derived object. it cant be NULL
    return(fptr(d, command, dest, value, frame));
	
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


extern EOVcanProto* eov_canProto_hid_New(void)
{
	EOVcanProto *retptr = NULL;	

	// i get the memory for the object
	retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOVcanProto), 1);
	// now the obj has valid memory. i need to initialise it with user-defined data
    
    // vtable
    retptr->vtable[VF00_parse]       = NULL;
    retptr->vtable[VF01_form]        = NULL;
    // other stuff
   // empty section

    eov_canProto_hid_SetVTABLE(retptr, s_eov_canFrame_parseCanFrame_dummy, s_eov_canFrame_formCanFrame_dummy);

    return(retptr);	
}

extern eOresult_t eov_canProto_hid_SetVTABLE(EOVcanProto *p, eOres_fp_canProto_parserFn_t v_parse, eOres_fp_canProto_formerFn_t v_form)
{

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != v_parse), s_eobj_ownname, "v_parse is NULL");
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != v_form), s_eobj_ownname, "v_form is NULL");

    p->vtable[VF00_parse]        = v_parse;
    p->vtable[VF01_form]         = v_form;

    return(eores_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
eOresult_t s_eov_canFrame_parseCanFrame_dummy(EOVcanProtoDerived *d, eOcanframe_t *frame, eOcanport_t canPortRx)
{
     return(eores_NOK_generic);
}




eOresult_t s_eov_canFrame_formCanFrame_dummy(EOVcanProtoDerived *d, eo_canProto_msgCommand_t command, 
											eo_canProto_msgDestination_t dest, void *value,
											eOcanframe_t *frame)
{
     return(eores_NOK_generic);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





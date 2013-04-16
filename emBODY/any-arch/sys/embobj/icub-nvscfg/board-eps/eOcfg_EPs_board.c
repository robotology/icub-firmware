/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       eOcfg_EPs_board.c
    @brief      This file keeps ...
    @author     marco.accame@iit.it
    @date       09/10/2012
 **/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EOnvsCfg.h"
#include "EOconstvector_hid.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOnv_hid.h"
#include "eOcfg_EPs_board.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const uint8_t s_dd[1] = {0};

static const EOconstvector s_eo_cfg_nvsEP_board_empty = 
{
    EO_INIT(.size)                  0,
    EO_INIT(.item_size)             1,
    EO_INIT(.item_array_data)       s_dd
};

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern const EOconstvector* eo_cfg_nvsEP_board_EPs_nvscfgep_get(uint8_t boardNum)
{
	const EOconstvector* EPvector = &s_eo_cfg_nvsEP_board_empty;
	switch(boardNum)
	{
		case 1:
        {
			EPvector = eo_cfg_EPs_vectorof_eb1;
        } break;

		case 2:
		{
            EPvector = eo_cfg_EPs_vectorof_eb2;
        } break;

		case 3:
		{
            EPvector = eo_cfg_EPs_vectorof_eb3;
        } break;

		case 4:
		{
            EPvector = eo_cfg_EPs_vectorof_eb4;
        } break;

		case 5:
		{ 
            EPvector = eo_cfg_EPs_vectorof_eb5;
        } break;

		case 6:
        {
            EPvector = eo_cfg_EPs_vectorof_eb6;
        } break;

		case 7:
        {
			EPvector = eo_cfg_EPs_vectorof_eb7;
        } break;

		case 8:
		{
            EPvector = eo_cfg_EPs_vectorof_eb8;
        } break;

		case 9:
		{ 
            EPvector = eo_cfg_EPs_vectorof_eb9;
        } break;

	}
	return(EPvector);
}

extern uint16_t eo_cfg_nvsEP_board_EPs_numberof_get(const EOconstvector* constvectorof_nvscfgep)
{
	return(eo_constvector_Size(constvectorof_nvscfgep));
}

extern eOnvscfg_EP_t* eo_cfg_nvsEP_board_EPs_cfg_get(const EOconstvector* constvectorof_nvscfgep, uint16_t epindex)
{
	return((eOnvscfg_EP_t*) eo_constvector_At(constvectorof_nvscfgep, epindex));
}

extern eOnvEP_t eo_cfg_nvsEP_board_EPs_endpoint_get(const EOconstvector* constvectorof_nvscfgep, uint16_t epindex)
{
	eOnvEP_t endpoint = EOK_uint16dummy;
	eOnvscfg_EP_t* epcfg = (eOnvscfg_EP_t*) eo_constvector_At(constvectorof_nvscfgep, epindex);
	if(NULL != epcfg)
	{
		endpoint = epcfg->endpoint;
	}

	return(endpoint);
}

extern uint16_t eo_cfg_nvsEP_board_EPs_epindex_get(uint8_t boardNum, eOnvEP_t ep)
{
	uint16_t epindex;

	switch(boardNum)
	{
		case 1:
			epindex = eo_cfg_nvsEP_eb1_hashfunction_ep2index(ep);
			break;
		case 2:
			epindex = eo_cfg_nvsEP_eb2_hashfunction_ep2index(ep);
			break;
		case 3:
			epindex = eo_cfg_nvsEP_eb3_hashfunction_ep2index(ep);
			break;
		case 4:
			epindex = eo_cfg_nvsEP_eb4_hashfunction_ep2index(ep);
			break;
		case 5:
			epindex = eo_cfg_nvsEP_eb5_hashfunction_ep2index(ep);
			break;
		case 6:
			epindex = eo_cfg_nvsEP_eb6_hashfunction_ep2index(ep);
			break;
		case 7:
			epindex = eo_cfg_nvsEP_eb7_hashfunction_ep2index(ep);
			break;
		case 8:
			epindex = eo_cfg_nvsEP_eb8_hashfunction_ep2index(ep);
			break;
		case 9:
			epindex = eo_cfg_nvsEP_eb9_hashfunction_ep2index(ep);
			break;
	}
	return epindex;
}

extern uint16_t eo_cfg_nvsEP_board_NVs_total_numberof_get(const EOconstvector* constvectorof_nvscfgep)
{    
	uint16_t number = 0;
	uint16_t tmp = 0;
	eOsizecntnr_t i;
	eOsizecntnr_t numofeps;
	eOnvscfg_EP_t *epscfg = NULL;

	numofeps =  eo_constvector_Size(constvectorof_nvscfgep);
	for(i=0; i<numofeps; i++)
	{
		epscfg = (eOnvscfg_EP_t*) eo_constvector_At(constvectorof_nvscfgep, i);
		if(NULL != epscfg)
		{
			tmp = eo_constvector_Size(epscfg->constvector_of_treenodes_EOnv_con);
			number += tmp;
		}
	}
	return(number);
}

extern uint16_t eo_cfg_nvsEP_board_NVs_endpoint_numberof_get(const EOconstvector* constvectorof_nvscfgep, uint16_t epindex)
{  
	uint16_t number = 0;
	eOnvscfg_EP_t *epscfg = NULL;

	if(EOK_uint16dummy == epindex)
	{
		return(0);
	}

	epscfg = (eOnvscfg_EP_t*) eo_constvector_At(constvectorof_nvscfgep, epindex);

	if(NULL == epscfg)
	{
		return(0);
	}

	number = eo_constvector_Size(epscfg->constvector_of_treenodes_EOnv_con);

	return(number);
}

extern eOnvID_t eo_cfg_nvsEP_board_NVs_endpoint_NVID_get(const EOconstvector* constvectorof_nvscfgep, eOnvEP_t epindex, uint16_t nvindex)
{
	//   const EOconstvector* constvectorof_nvscfgep = eo_cfg_EPs_vectorof_eb1;

	eOnvID_t nvid = EOK_uint16dummy;
	eOsizecntnr_t number;
	eOnvscfg_EP_t *epscfg = NULL;
	EOtreenode *treenode = NULL;
	EOnv_con_t *nvcon;

	if(EOK_uint16dummy == epindex)
	{
		return(nvid);
	}

	epscfg = (eOnvscfg_EP_t*) eo_constvector_At(constvectorof_nvscfgep, epindex);

	if(NULL == epscfg)
	{
		return(nvid);
	}

	number = eo_constvector_Size(epscfg->constvector_of_treenodes_EOnv_con);
	if(nvindex >= number)
	{
		return(nvid);
	}

	treenode = (EOtreenode*) eo_constvector_At(epscfg->constvector_of_treenodes_EOnv_con, nvindex);

	if(NULL == treenode)
	{
		return(nvid);
	}

	nvcon = (EOnv_con_t*) eo_treenode_GetData(treenode);

	return(nvcon->id);
}

extern uint16_t eo_cfg_nvsEP_board_NVs_endpoint_Nvindex_get(const eOnvscfg_EP_t* EPcfg, eOnvID_t nvid)
{
	uint16_t nvindex = EOK_uint16dummy;

    if(NULL == EPcfg)
    {
        return(nvindex);
    }

	nvindex = EPcfg->hashfunction_id2index(nvid);
	return nvindex;
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




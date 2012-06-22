
/* @file       boardTransceiver.c
    @brief      This file implements internal implementation of a nv object.
    @author     marco.accame@iit.it
    @date       09/03/2010
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

using namespace std;

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "EOtransceiver.h"
#include "boardTransceiver.hpp"
#include "EOtheBOARDtransceiver_hid.h"
#include "main_BOARD_transceiver.hpp"
#include "EoCommon.h"
#include "EOnv_hid.h"


// static EOtransceiver *ems00txrx = NULL;
static EOtransceiver *boardTransceiver = NULL;
static EOnvsCfg *nvsCfg = NULL;
EOpacket*      pkt;

extern EOtransceiver* boardTransceiver_new(uint32_t _localipaddr, uint32_t _remoteipaddr, uint16_t _ipport, uint8_t _board_n)
{
	eo_transceiver_sizes_t	my_sizes =
	{
			EO_INIT(.capacityofpacket) 				MAX_CAPACITY_OF_PACKET,
			EO_INIT(.capacityofrop) 				256,
			EO_INIT(.capacityofropframeregulars) 	384,
			EO_INIT(.capacityofropframeoccasionals)	256,
			EO_INIT(.capacityofropframereplies)		128,
			EO_INIT(.maxnumberofregularrops)		32
	};

    eOboardtransceiver_cfg_t boardTransceiverCfg =
    {
    		EO_INIT(.vectorof_endpoint_cfg)         eo_cfg_EPs_vectorof_eb4,
    		EO_INIT(.hashfunction_ep2index)       	eo_cfg_nvsEP_eb4_fptr_hashfunction_ep2index,
    		EO_INIT(.remotehostipv4addr)            _remoteipaddr,
    		EO_INIT(.remotehostipv4port)            _ipport,
    		EO_INIT(.sizes)							my_sizes
    };

	switch(_board_n)
	{
	case 1:
		boardTransceiverCfg.vectorof_endpoint_cfg = eo_cfg_EPs_vectorof_eb1;
		boardTransceiverCfg.hashfunction_ep2index = eo_cfg_nvsEP_eb1_fptr_hashfunction_ep2index;
		break;
	case 2:
		boardTransceiverCfg.vectorof_endpoint_cfg = eo_cfg_EPs_vectorof_eb2;
		boardTransceiverCfg.hashfunction_ep2index = eo_cfg_nvsEP_eb2_fptr_hashfunction_ep2index;
		break;
	case 3:
		boardTransceiverCfg.vectorof_endpoint_cfg = eo_cfg_EPs_vectorof_eb3;
		boardTransceiverCfg.hashfunction_ep2index = eo_cfg_nvsEP_eb3_fptr_hashfunction_ep2index;
		break;
	case 4:
		boardTransceiverCfg.vectorof_endpoint_cfg = eo_cfg_EPs_vectorof_eb4;
		boardTransceiverCfg.hashfunction_ep2index = eo_cfg_nvsEP_eb4_fptr_hashfunction_ep2index;
		break;
	case 5:
		boardTransceiverCfg.vectorof_endpoint_cfg = eo_cfg_EPs_vectorof_eb5;
		boardTransceiverCfg.hashfunction_ep2index = eo_cfg_nvsEP_eb5_fptr_hashfunction_ep2index;
		break;
	case 6:
		boardTransceiverCfg.vectorof_endpoint_cfg = eo_cfg_EPs_vectorof_eb6;
		boardTransceiverCfg.hashfunction_ep2index = eo_cfg_nvsEP_eb6_fptr_hashfunction_ep2index;
		break;
	case 7:
		boardTransceiverCfg.vectorof_endpoint_cfg = eo_cfg_EPs_vectorof_eb7;
		boardTransceiverCfg.hashfunction_ep2index = eo_cfg_nvsEP_eb7_fptr_hashfunction_ep2index;
		break;
	case 8:
		boardTransceiverCfg.vectorof_endpoint_cfg = eo_cfg_EPs_vectorof_eb8;
		boardTransceiverCfg.hashfunction_ep2index = eo_cfg_nvsEP_eb8_fptr_hashfunction_ep2index;
		break;
	case 9:
		boardTransceiverCfg.vectorof_endpoint_cfg = eo_cfg_EPs_vectorof_eb9;
		boardTransceiverCfg.hashfunction_ep2index = eo_cfg_nvsEP_eb9_fptr_hashfunction_ep2index;
		break;
	default:
		printf(">>>ERROR!!!\nUnknown board number... using EB4!!!\n");
	}



    boardTransceiver = eo_boardtransceiver_Initialise(&boardTransceiverCfg);
    nvsCfg = eo_boardtransceiver_hid_GetNvsCfg();
    pkt = eo_packet_New(MAX_CAPACITY_OF_PACKET);
    return (boardTransceiver);
}


// somebody passes the received packet - this is used just as an interface
void onMsgReception(uint8_t *data, uint16_t size)
{
	//this->pkt = data;
//	SetReceived(data, size);
}

// and Processes it
void SetReceived(uint8_t *data, uint16_t size, uint32_t remoteipaddr, uint16_t port)
{
    uint16_t numofrops;
    uint64_t txtime;

    eo_packet_Payload_Set(pkt, data, size);
    eo_packet_Addressing_Set(pkt, remoteipaddr, port);
    eo_transceiver_Receive(boardTransceiver, pkt, &numofrops, &txtime);
    printf("Number of rop received: %d\n", numofrops);
}

void boardTransceiver_load_occasional_rop(eOropcode_t opc, uint16_t ep, uint16_t nvid)
{
    eo_transceiver_ropinfo_t ropinfo;

    ropinfo.ropcfg      = eok_ropconfig_basic;
    ropinfo.ropcode     = opc;
    ropinfo.nvep        = ep;

    ropinfo.nvid = nvid;
    eo_transceiver_rop_occasional_Load(boardTransceiver, &ropinfo);

}

// somebody retrieves what must be transmitted
void boardTransceiver_GetTransmit(uint8_t **data, uint16_t *size)
{
    uint16_t numofrops;

    eo_transceiver_Transmit(boardTransceiver, &pkt, &numofrops);

    eo_packet_Payload_Get(pkt, data, size);

}

EOnv* boardTransceiver_getNVhandler(uint16_t endpoint, uint16_t id)
{
	uint16_t		ondevindex = 0, onendpointindex = 0 , onidindex = 0;
	EOtreenode	*nvTreenodeRoot;
	EOnv 		*nvRoot;
	EOnv		tmp;
    eOresult_t res;

    // convetire come parametro, oppure mettere direttam l'indirizzo ip come parametro...
 //   eOnvOwnership_t	nvownership = eo_nv_ownership_remote;

	res = eo_nvscfg_GetIndices(	nvsCfg, EO_COMMON_IPV4ADDR_LOCALHOST, endpoint, id, &ondevindex,	&onendpointindex, &onidindex);

	// if the nvscfg does not have the triple (ip, ep, id) then we return an error
	if(eores_OK != res)
	{
		// Do something about this case
		printf("Error!! Unable to get the nv pointer");
	}
	else
	{
		// 2 passaggi e copia della nv
		// we need a treenode of the nv
		//nvTreenodeRoot = eo_nvscfg_GetTreeNode(this->pc104nvscfg, ondevindex, onendpointindex, onidindex);  //  ?? non usato??
		// but also the handle to the nv.
		//eo_nvscfg_GetNV(this->pc104nvscfg, ondevindex, onendpointindex, onidindex, nvTreenodeRoot, &nvRoot);

		// più diretta e restituisce solo il puntatore che è ciò che mi serve qui, ma non ho accesso a nvsCfg_allnvs
		//nvRoot = eo_matrix3d_At(pc104nvscfg->allnvs, ondevindex, onendpointindex, onidindex);
		nvRoot = eo_nvscfg_GetNV(nvsCfg, ondevindex, onendpointindex, onidindex, nvTreenodeRoot, &tmp);

		//getNVvalue(nvRoot, data, size);
	}
	return(nvRoot);
}

#if 0

void BoardTransceiver::init(uint32_t _localipaddr, uint32_t _remoteipaddr, uint16_t _ipport, uint16_t _pktsize)
{
    // the configuration of the transceiver: it is specific of a given remote board
    eOboardtransceiver_cfg_t boardTransceiverCfg =
    {
    		EO_INIT(.vectorof_endpoint_cfg)           eo_cfg_EPs_vectorof_loc_board,
    		EO_INIT(.hashfunction_ep2index)           eo_cfg_nvsEP_loc_board_fptr_hashfunction_ep2index,
    		EO_INIT(.remotehostipv4addr)              _remoteipaddr,
    		EO_INIT(.remotehostipv4port)              _ipport
    };
    
    localipaddr  = _localipaddr;
    remoteipaddr = _remoteipaddr;
    ipport       = _ipport;
    
    // initialise the transceiver: it creates a EOtransceiver and its nvsCfg by loading all the endpoints and returns the pointer
    boardTransceiver     = eo_boardtransceiver_Initialise(&boardTransceiverCfg);
    
    // retrieve teh transceiver
    //pc104txrx    = eo_boardtransceiver_GetHandle(boardTransceiver);
    
    // retrieve the nvscfg
    boardnvscfg  = eo_boardTransceiver_NVsCfg(boardTransceiver);

}

void BoardTransceiver::s_eom_hostprotoc_extra_protocoltransceiver_load_occasional_rop(eOropcode_t opc, uint16_t ep, uint16_t nvid)
{
    eo_transceiver_ropinfo_t ropinfo;

    ropinfo.ropcfg      = eok_ropconfig_basic;
    ropinfo.ropcode     = opc;
    ropinfo.nvep        = ep;

    ropinfo.nvid = nvid;
    eo_transceiver_rop_occasional_Load(pc104txrx, &ropinfo);
}

/*
void BoardTransceiver::s_eom_hostprotoc_extra_protocoltransceiver_configure_regular_rops_on_board(void)
{
    EOarray *upto10 = (EOarray*) & eo_cfg_nvsEP_mngmnt_usr_rem_board_mem_local->upto10rop2signal;
    eOropSIGcfg_t sigcfg;
    char str[128];
    static uint8_t reset = 0;

    eo_array_Reset(upto10);

    if(0 == reset)
    {

        sigcfg.ep = EOK_cfg_nvsEP_base_endpoint;
        sigcfg.id = EOK_cfg_nvsEP_base_NVID_ipnetwork;
        sigcfg.plustime = 1;
        eo_array_PushBack(upto10, &sigcfg);


        sigcfg.ep = EOK_cfg_nvsEP_base_endpoint;
        sigcfg.id = EOK_cfg_nvsEP_base_NVID__bootprocess;
        sigcfg.plustime = 1;
        eo_array_PushBack(upto10, &sigcfg);

        sigcfg.ep = EOK_cfg_nvsEP_base_endpoint;
        sigcfg.id = EOK_cfg_nvsEP_base_NVID__applicationinfo;
        sigcfg.plustime = 0;
        eo_array_PushBack(upto10, &sigcfg);

        sigcfg.ep = EOK_cfg_nvsEP_base_endpoint;
        sigcfg.id = EOK_cfg_nvsEP_base_NVID__boardinfo;
        sigcfg.plustime = 0;
        eo_array_PushBack(upto10, &sigcfg);

        sigcfg.ep = EOK_cfg_nvsEP_base_endpoint;
        sigcfg.id = EOK_cfg_nvsEP_base_NVID_ipnetwork__ipnetmask;
        sigcfg.plustime = 0;
        eo_array_PushBack(upto10, &sigcfg);

        sigcfg.ep = EOK_cfg_nvsEP_base_endpoint;
        sigcfg.id = EOK_cfg_nvsEP_base_NVID_ipnetwork__ipaddress;
        sigcfg.plustime = 0;
        eo_array_PushBack(upto10, &sigcfg);

        sigcfg.ep = EOK_cfg_nvsEP_base_endpoint;
        sigcfg.id = EOK_cfg_nvsEP_base_NVID__remoteipaddress;
        sigcfg.plustime = 0;
        eo_array_PushBack(upto10, &sigcfg);

        sigcfg.ep = EOK_cfg_nvsEP_base_endpoint;
        sigcfg.id = EOK_cfg_nvsEP_base_NVID__remoteipport;
        sigcfg.plustime = 0;
        eo_array_PushBack(upto10, &sigcfg);

    }

    s_eom_hostprotoc_extra_protocoltransceiver_load_occasional_rop(eo_ropcode_set, EOK_cfg_nvsEP_mngmnt_endpoint, EOK_cfg_nvsEP_mngmnt_NVID__upto10rop2signal);

    if(1 == reset)
    {
        printf( "added a set<__upto10rop2signal, empty-list>");
    }
    else
    {
        printf("added a set<__upto10rop2signal, list>");
    }
    reset = (0==reset) ? (1) : (0);
}


// vecchio //
void BoardTransceiver::boardTransceiver_ConfigureRegularsOnRemote(void)
{
    // 1. at first write the nv that we want to set: the EOK_cfg_nvsEP_mngmnt_NVID__upto15epid2signal
    //    there are two alternative ways to do that:
    //    A. QUICK MODE - to write directly onto the local struct of the endpoint. WE USE THIS SECOND WAY because we know
    //       the name of the specific variable to use in the rop
    //    B. FORMAL MODE (and more generic): to use teh nvscfg to retrieve the handle of a EOnv which is relative to the 
    //       pair (endpoint, id) which we wnat to manipulate, and then to use the methods of EOnv to set the value of that variable.
    //       we shall use this mode in function hostTransceiver_AddSetROP()
    
    EOarray *upto15 = (EOarray*) & eo_cfg_nvsEP_mngmnt_usr_rem_board_mem_local->upto10rop2signal;
#warning "cambiato?? per compilare scommentato definizione eOnvEPID_t in EOnv.h"
//    eOnvEPID_t epid;

    // clear the variable.
//    eo_array_Reset(upto15);

    // push back in teh array the first nv that the remote board shall signal
//    epid.ep = EOK_cfg_nvsEP_base_endpoint;
//    epid.id = EOK_cfg_nvsEP_base_NVID_ipnetwork__ipaddress;
//    eo_array_PushBack(upto15, &epid);

    // push back in teh array the second nv that the remote board shall signal
//    epid.ep = EOK_cfg_nvsEP_base_endpoint;
//    epid.id = EOK_cfg_nvsEP_base_NVID__bootprocess;
//    eo_array_PushBack(upto15, &epid);
    
    // 2. then put the rop set<upto15epid2signal, data> inside the transceiver
//    s_hostTransceiver_AddSetROP_with_data_already_set(EOK_cfg_nvsEP_mngmnt_endpoint, EOK_cfg_nvsEP_mngmnt_NVID__upto10rop2signal);
}
*/

// somebody adds a set-rop  plus data.
void BoardTransceiver::boardTransceiver_AddSetROP(uint16_t ep, uint16_t id, uint8_t* data, uint16_t size)
{
    uint16_t ss;
    EOnv nv;

    // 1. search for teh EOnv with pair (ep, id)
            
 //   if(eores_OK != eo_nvscfg_GetNV(thehosttransceiver.boardnvscfg, thehosttransceiver.remoteipaddr, ep, id, &nv))
    {
        // there is no such variable with (remoteipaddr-ep-id) 
 //       return;
    }
    
    // 1bis. verify that the datasize is correct.
    ss = eo_nv_Size(&nv, data);
    if(ss < size)
    {
        // non faccio nulla intanto quando scrivo uso la capacita' della nv.
        // sarebbe bene pero' emettere un warning        
    }
    
    
    // 2. set the data. dont force the set so that we dont write if the nv is readonly.
    
    if(eores_OK != eo_nv_Set(&nv, data, eobool_false, eo_nv_upd_dontdo))
    {   
        // teh nv is not writeable
        return;
    }
    
    
    // 3. add the rop 
    s_boardTransceiver_AddSetROP_with_data_already_set(ep, id);

}


// somebody passes the received packet
void BoardTransceiver::onMsgReception(uint8_t *data, uint16_t size)
{
	//this->pkt = data;
	SetReceived(data, size);
}

void BoardTransceiver::SetReceived(uint8_t *data, uint16_t size)
{
    uint16_t numofrops;
    uint64_t txtime;
    pkt = eo_packet_New(size);
    eo_packet_Payload_Set(pkt, data, size);
    eo_packet_Addressing_Set(pkt, remoteipaddr, ipport);
         
    eo_transceiver_Receive(pc104txrx, pkt, &numofrops, &txtime);
    free(pkt);
}


// somebody retrieves what must be transmitted
void BoardTransceiver::getTransmit(uint8_t **data, uint16_t *size)
{
    uint16_t numofrops;
    
    eo_transceiver_Transmit(pc104txrx, &pkt, &numofrops);
    
    eo_packet_Payload_Get(pkt, data, size);
    
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

void BoardTransceiver::s_boardTransceiver_AddSetROP_with_data_already_set(uint16_t ep, uint16_t id)
{
    eo_transceiver_ropinfo_t ropinfo;

    ropinfo.ropcfg      = eok_ropconfig_basic;
    ropinfo.ropcode     = eo_ropcode_set;
    ropinfo.nvep        = ep;
    ropinfo.nvid        = id;
    
    eo_transceiver_rop_occasional_Load(pc104txrx, &ropinfo);
}

void BoardTransceiver::s_boardTransceiver_AddGetROP(uint16_t ep, uint16_t id)
{
    eo_transceiver_ropinfo_t ropinfo;

    ropinfo.ropcfg      = eok_ropconfig_basic;
    ropinfo.ropcode     = eo_ropcode_ask;
    ropinfo.nvep        = ep;
    ropinfo.nvid        = id;

    eo_transceiver_rop_occasional_Load(pc104txrx, &ropinfo);
}

#endif

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





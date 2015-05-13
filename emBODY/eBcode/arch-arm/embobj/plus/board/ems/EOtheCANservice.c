/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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


#include "EOtheErrorManager.h"
#include "EoError.h"

#include "EOtheCANmapping.h"
#include "EOtheCANprotocol.h"
#include "EoProtocol.h"

#include "osal.h"
#include "hal.h"
#include "hal_mpu_name_stm32f407ig.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheCANservice.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheCANservice_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------



const eOcanserv_cfg_t eo_canserv_DefaultCfg = 
{    
    .mode             = eocanserv_mode_ondemand,
    .rxqueuesize        = {64, 64},
    .txqueuesize        = {64, 64},
    .onrxcallback       = {NULL, NULL},
    .onrxargument       = {NULL, NULL}
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_canserv_peripheral_init(EOtheCANservice *p);
static eOresult_t s_eo_canserv_otherdata_init(EOtheCANservice *p);
static void s_eo_canserv_onrx_can(void *arg);
static void s_eo_canserv_ontx_can(void *arg);
static void s_eo_canserv_onerror_can(void *arg);
static eOresult_t s_eo_canserv_send_frame_simplemode(EOtheCANservice *p, eOcanport_t port, eOcanframe_t *frame);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOtheCANservice";

 
static EOtheCANservice s_eo_canserv_singleton = 
{    
    .initted            = eobool_false,
	.config             = {0},
    .locktilltxall      = {0}
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheCANservice * eo_canserv_Initialise(const eOcanserv_cfg_t *cfg)
{
    EOtheCANservice *ret = NULL;
    
    if(eobool_true == s_eo_canserv_singleton.initted)
    {
        return(&s_eo_canserv_singleton);
    }
    
    if(NULL == cfg)
    {
        cfg = &eo_canserv_DefaultCfg;    
    }

    memcpy(&s_eo_canserv_singleton.config, cfg, sizeof(eOcanserv_cfg_t));
    
    
    if(eores_OK != s_eo_canserv_peripheral_init(&s_eo_canserv_singleton))
    {
        #warning --> put diagnostics .... maybe return NULL
    }
    else if(eores_OK != s_eo_canserv_otherdata_init(&s_eo_canserv_singleton))
    {  
        #warning --> put diagnostics .... maybe return NULL
    }
    else
    {    
        s_eo_canserv_singleton.initted = eobool_true;
        ret = &s_eo_canserv_singleton;        
    }
        
    return(ret);
}


extern EOtheCANservice* eo_canserv_GetHandle(void) 
{
    if(eobool_true == s_eo_canserv_singleton.initted)
    {
        return(&s_eo_canserv_singleton);
    }
    else
    {
        return(NULL);
    }
}

extern eOresult_t eo_canserv_SetMode(EOtheCANservice *p, eOcanserv_mode_t mode)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    p->config.mode = mode;
    
    return(eores_OK);
}


extern eOresult_t eo_canserv_TXstart(EOtheCANservice *p, eOcanport_t port, uint8_t *sizeofTXqueue)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eocanserv_mode_straight == p->config.mode)
    {
        hal_can_transmit((hal_can_port_t)port);
        return(eores_OK);
    }
    
    hal_irqn_t irqn = (eOcanport1 == port)? hal_mpu_name_stm32f407ig_CAN1_TX_IRQn : hal_mpu_name_stm32f407ig_CAN2_TX_IRQn;
    uint8_t numofoutframes = 0;
    
    hal_sys_irqn_disable(irqn);
    hal_can_out_get((hal_can_port_t)port, &numofoutframes);
    p->locktilltxall[port].numoftxframes = numofoutframes; // keep it protected by irq tx disable because this VOLATILE variable is modified by isr    
    p->locktilltxall[port].totaltxframes = numofoutframes;
    hal_sys_irqn_enable(irqn);
    
    if(0 != numofoutframes)
    {
        hal_can_transmit((hal_can_port_t)port);
    }
    
    if(NULL != sizeofTXqueue)
    {
        *sizeofTXqueue = numofoutframes;
    }
    return(eores_OK);    
}


extern eOresult_t eo_canserv_TXwaituntildone(EOtheCANservice *p, eOcanport_t port, uint32_t timeout)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eocanserv_mode_straight == p->config.mode)
    {   // dont wait ...
        return(eores_OK);
    } 
    
    osal_result_t osal_res = osal_res_OK ;   
    
    if(p->locktilltxall[port].numoftxframes > 0)
    {
        if(osal_res_OK != (osal_res = osal_semaphore_decrement(p->locktilltxall[port].locksemaphore, 3*osal_reltime1ms)))
        {            
            uint8_t sizeoftxfifo = 0;
            hal_can_out_get((hal_can_port_t)port, &sizeoftxfifo);
            eOerrmanDescriptor_t errdes = {0};
            errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_txbusfailure);
            errdes.par16                = 0x0001;
            errdes.par16                |= ((uint16_t)sizeoftxfifo << 8);
            errdes.par64                = 0; // dont knw what to send up           
            errdes.sourcedevice         = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
            errdes.sourceaddress        = 0;                                   
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);    
        }
    }
    return((eOresult_t)osal_res);
}

extern uint8_t eo_canserv_NumberOfFramesInRXqueue(EOtheCANservice *p, eOcanport_t port)
{
    if(NULL == p)
    {
        return(0);
    }

    uint8_t number = 0; 
    hal_can_received((hal_can_port_t)port, &number);
    return(number);
}


extern eOresult_t eo_canserv_Parse(EOtheCANservice *p, eOcanport_t port, uint8_t maxnumofcanframes, uint8_t *numofreadcanframes)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    eOresult_t res = eores_NOK_generic;
    hal_result_t halres = hal_res_NOK_nodata;
    hal_can_frame_t canframe = {0};
    uint8_t readcanframes = 0;
    uint8_t i = 0;
    
    for(i=0; i<maxnumofcanframes; i++)
    {
        memset(&canframe, 0, sizeof(hal_can_frame_t));
        if(hal_res_OK != (halres = hal_can_get((hal_can_port_t)port, &canframe, NULL)))
        {
            break;      // marco.accame on 12 jan 2015: changed the original continue in a break because:
                        // if we have a NOK then we cannot go on because the fifo is surely empty.                     
        } 
        
        readcanframes++;
        
        // now parse the frame.
        if(eores_OK != (res = eo_canprot_Parse(eo_canprot_GetHandle(), (eOcanframe_t*)&canframe, port))) 
        {  
            eOerrmanDescriptor_t errdes = {0};
            errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_parsingfailure);
            errdes.par16                = (canframe.id & 0x0fff) | ((canframe.size & 0x000f) << 12);
            errdes.par64                = eo_common_canframe_data2u64((eOcanframe_t*)&canframe);
            errdes.sourcedevice         = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
            errdes.sourceaddress        =  EOCANPROT_FRAME_GET_SOURCE(&canframe);                
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &errdes);             
        }    
    }
    
    if(NULL != numofreadcanframes)
    {
        *numofreadcanframes = readcanframes;    
    }

    return(eores_OK);
}

#warning ---> it is ok for all but for skin .......
extern eOresult_t eo_canserv_SendCommandToEntity(EOtheCANservice *p, eOprotID32_t id32, eOcanprot_descriptor_t *command)
{
    eOerrmanDescriptor_t errdes = {0};

    if((NULL == p) || (NULL == command))
    {
        return(eores_NOK_nullpointer);
    }

    // now i find the location of the entity
    eOcanmap_location_t location = {0};   
    if(eores_OK != eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &location, NULL, NULL))
    {   // error ...
        #warning --> put diagnostics
        return(eores_NOK_generic);
    }
    
    // here is the addressing
    eOcanport_t port = (eOcanport_t)location.port; 
    command->destinationaddress = location.addr;
    command->internalindex = location.insideindex;
    
    // here is the frame
    eOcanframe_t frame = {0};
    
    if(eores_OK != eo_canprot_Form(eo_canprot_GetHandle(), command, &frame))
    {   // error ...
        errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_formingfailure);
        errdes.par16                = (command->msgclass << 8) | (command->msgtype);
        errdes.par64                = 0;
        errdes.sourcedevice         = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        errdes.sourceaddress        = command->destinationaddress;           
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);               
        return(eores_NOK_generic);
    }
    
    // ok now i can sent the frame over can. what i do depends on the mode.
    return(s_eo_canserv_send_frame_simplemode(p, port, &frame));
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_canserv_peripheral_init(EOtheCANservice *p)
{
    hal_can_cfg_t can_cfg = {0};

    // can 1
    can_cfg.runmode                     = hal_can_runmode_isr_1txq1rxq;
    can_cfg.baudrate                    = hal_can_baudrate_1mbps; 
    can_cfg.priorx                      = hal_int_priority11;
    can_cfg.priotx                      = hal_int_priority11;
 
    can_cfg.capacityofrxfifoofframes    = p->config.rxqueuesize[hal_can_port1];
    can_cfg.capacityoftxfifoofframes    = p->config.txqueuesize[hal_can_port1];
    can_cfg.capacityoftxfifohighprio    = 0;
     
    can_cfg.callback_on_rx              = s_eo_canserv_onrx_can;
    can_cfg.arg_cb_rx                   = (void*)hal_can_port1;
    can_cfg.callback_on_tx              = s_eo_canserv_ontx_can;
    can_cfg.arg_cb_tx                   = (void*)hal_can_port1;
    can_cfg.callback_on_err             = s_eo_canserv_onerror_can;
    can_cfg.arg_cb_err                  = (void*)hal_can_port1;

    if(hal_res_OK != hal_can_init(hal_can_port1, &can_cfg))
    {
        return(eores_NOK_generic);
    }
    
    if(hal_res_OK != hal_can_enable(hal_can_port1))
    {
        return(eores_NOK_generic);
    }    
    
    // can 2    
    can_cfg.runmode            = hal_can_runmode_isr_1txq1rxq;
    can_cfg.baudrate           = hal_can_baudrate_1mbps; 
    can_cfg.priorx             = hal_int_priority11;
    can_cfg.priotx             = hal_int_priority11;
 
    can_cfg.capacityofrxfifoofframes    = p->config.rxqueuesize[hal_can_port2];
    can_cfg.capacityoftxfifoofframes    = p->config.txqueuesize[hal_can_port2];
    can_cfg.capacityoftxfifohighprio   = 0;    
 
    can_cfg.callback_on_rx              = s_eo_canserv_onrx_can;
    can_cfg.arg_cb_rx                   = (void*)hal_can_port2;
    can_cfg.callback_on_tx              = s_eo_canserv_ontx_can;
    can_cfg.arg_cb_tx                   = (void*)hal_can_port2;
    can_cfg.callback_on_err             = s_eo_canserv_onerror_can;
    can_cfg.arg_cb_err                  = (void*)hal_can_port2;

    if(hal_res_OK != hal_can_init(hal_can_port2, &can_cfg))
    {
        return(eores_NOK_generic);
    }

    if(hal_res_OK != hal_can_enable(hal_can_port2))
    {
        return(eores_NOK_generic);
    }    

    return(eores_OK);
}

static void s_eo_canserv_onrx_can(void *arg)
{
    // i look at the mode. 
    // if ondemand i do nothing because the user will retrieve the frame only when ready.
    // if straigth i call the callback which very likely will send an event to wake up a task which retrieves the frame
    if(eocanserv_mode_straight == s_eo_canserv_singleton.config.mode)
    {
        uint32_t n = (uint32_t)arg;
        hal_can_port_t port = (hal_can_port_t)n; // either hal_can_port1 or hal_can_port2
        eOcallback_t onrx = s_eo_canserv_singleton.config.onrxcallback[port];
        if(NULL != onrx)
        {
            onrx(s_eo_canserv_singleton.config.onrxargument[port]);
        }
    }    
}

#warning TDO: think of how to implement the eo_appCanSP_EmptyCanOutputQueue so that it is blocking. maybe we just set mode to eocanserv_mode_ondemand and start the tx blocking until done.
static void s_eo_canserv_ontx_can(void *arg)
{
    // i look at the mode.
    // if straigth i do nothing because i dont need to wait for the end of transmission.
    // if ondemand i must increment a semaphore when all frames are transmitted.
    if(eocanserv_mode_ondemand == s_eo_canserv_singleton.config.mode)
    {
        uint32_t n = (uint32_t)arg;
        hal_can_port_t port = (hal_can_port_t)n; // either hal_can_port1 or hal_can_port2
        eOcanserv_lockdata_t* locktilltxall = &s_eo_canserv_singleton.locktilltxall[port];
        if(locktilltxall->numoftxframes > 0)
        {
            locktilltxall->numoftxframes --;
            if(0 == locktilltxall->numoftxframes)
            {
                osal_semaphore_increment(locktilltxall->locksemaphore, osal_callerISR);
            }        
        }
    }    
}


static void s_eo_canserv_onerror_can(void *arg)
{
    uint32_t n = (uint32_t)arg;
    hal_can_port_t port = (hal_can_port_t)n; // either hal_can_port1 or hal_can_port2
    #warning TODO: add whatever is needed by can error
}


static eOresult_t s_eo_canserv_otherdata_init(EOtheCANservice *p)
{
    p->locktilltxall[hal_can_port1].totaltxframes = 0;
    p->locktilltxall[hal_can_port1].numoftxframes = 0;
    if(NULL == (p->locktilltxall[hal_can_port1].locksemaphore = osal_semaphore_new(255/*maxtokens*/, 0/*current num of token*/)))
    {
        return(eores_NOK_generic);
    }
    //osal_semaphore_set(p->locktilltxall[hal_can_port1].locksemaphore, 0);
    
    p->locktilltxall[hal_can_port2].totaltxframes = 0;
    p->locktilltxall[hal_can_port2].numoftxframes = 0;
    if(NULL == (p->locktilltxall[hal_can_port2].locksemaphore = osal_semaphore_new(255/*maxtokens*/, 0/*current num of token*/)))
    {
        return(eores_NOK_generic);
    }
    //osal_semaphore_set(p->locktilltxall[hal_can_port2].locksemaphore, 0);

    return(eores_OK);
}


static eOresult_t s_eo_canserv_send_frame_simplemode(EOtheCANservice *p, eOcanport_t port, eOcanframe_t *frame)
{
    eOresult_t res = eores_NOK_generic;
    hal_can_send_mode_t sendmode = (eocanserv_mode_ondemand == p->config.mode) ? (hal_can_send_normprio_later) : (hal_can_send_normprio_now);
     
    if(hal_res_OK == hal_can_put((hal_can_port_t)port, (hal_can_frame_t*)frame, sendmode))
    {
        // we are all happy
        res = eores_OK;
    }
    else
    {
        // problems ... it is worth sending at least a warning.
        eOerrmanDescriptor_t errdes = {0};  
        //hal_can_out_get((hal_can_port_t)port, &sizeoftxfifo);
        errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_txfifooverflow);
        errdes.par16                = (frame->id & 0x0fff) | ((frame->size & 0x000f) << 12);
        errdes.par64                = eo_common_canframe_data2u64(frame);
        errdes.sourcedevice         = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        errdes.sourceaddress        = EOCANPROT_FRAME_GET_SOURCE(frame);                                     
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);       

        // in simple mode i do not attempt any recovery ........... i may do it but only if ...... see old s_eo_appCanSP_formAndSendFrame()
    }
    
    return(res);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






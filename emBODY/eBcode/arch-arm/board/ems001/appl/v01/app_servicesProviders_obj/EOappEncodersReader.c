/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
// - doxy
// --------------------------------------------------------------------------------------------------------------------

/* @file       eo_appl_encodersReaderModule.c
    @brief      This file implements emcodersReader Module.
    @author    valentina.gaggero@iit.it
    @date       02/17/2012
**/



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stdlib.h"
#include "string.h"
#include "hal.h"
//#include "hal_debugPin.h"

#include "EOtheMemoryPool.h"

#include "OPCprotocolManager_Cfg.h" 
#include "EOtheEMSapplDiagnostics.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOappEncodersReader.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "EOappEncodersReader_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
//#define CHECK_ENC_IS_CONNECTED(p, enc)      ((1<<enc) == ((1<<enc)&(p->cfg.connectedEncodersMask)))
#define CHECK_ENC_IS_CONNECTED(p, enc)      ((1<<enc) == ((1<<enc)&(p->halConfigEncMask)))
#define ENCODER_NULL                        255
#define INTPRIO_SPI_ENCODERS                hal_int_priority05
#define DGN_COUNT_MAX                       10000 //1 sec
#define DGN_THRESHOLD                       0



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static void s_eo_appEncReader_isrCbk_onEncRead(void *arg);
static void s_eo_appEncReader_isrCbk_onLastEncRead_SPI1(void *arg);
static void s_eo_appEncReader_isrCbk_onLastEncRead_SPI3(void *arg);

static void s_eo_appEncReader_readConnectedEncConfg(EOappEncReader *p, EOappEncReader_configEncSPIXReadSequence_hid_t *cfgEncSPIX, hal_encoder_t startEnc, hal_encoder_t endEnc);
static void s_eo_appEncReader_configureConnectedEncoders(EOappEncReader *p, hal_encoder_t startEnc, hal_encoder_t endEnc, EOappEncReader_configEncSPIXReadSequence_hid_t *cfgEncSPIX);
static eOboolvalues_t s_eo_appEncReader_IsValidValue(uint32_t *valueraw, eOappEncReader_errortype_t *error);
static void s_eo_appEncReader_mapAppNumbering2HalNumbering(EOappEncReader *p);

static void s_eo_appEncReader_check(EOappEncReader *p);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

#if     defined(HAL_USE_VERSION_2)

#define HALv1_encoder1      hal_encoder1
#define HALv1_encoder2      hal_encoder2
#define HALv1_encoder3      hal_encoder3
#define HALv1_encoder4      hal_encoder1
#define HALv1_encoder5      hal_encoder1
#define HALv1_encoder6      hal_encoder1
#define HALv1_encoder7      hal_encoder4
#define HALv1_encoder8      hal_encoder5
#define HALv1_encoder9      hal_encoder6

static const hal_encoder_t encoderMap[eOeOappEncReader_encoderMaxNum] =
{
    /* 0 */     hal_encoder1,
    /* 1 */     hal_encoder1,
    /* 2 */     hal_encoder1,
    /* 3 */     hal_encoder1,
    /* 4 */     hal_encoder1,
    /* 5 */     hal_encoder1
};

#warning --> in HAL2 the mapping of encoders is to be reviewed

#else

#define HALv1_encoder1      hal_encoder1
#define HALv1_encoder2      hal_encoder2
#define HALv1_encoder3      hal_encoder3
#define HALv1_encoder4      hal_encoder4
#define HALv1_encoder5      hal_encoder5
#define HALv1_encoder6      hal_encoder6
#define HALv1_encoder7      hal_encoder7
#define HALv1_encoder8      hal_encoder8
#define HALv1_encoder9      hal_encoder9

static const hal_encoder_t encoderMap[eOeOappEncReader_encoderMaxNum] =
{
    /* 0 */     hal_encoder1,
    /* 1 */     hal_encoder7,
    /* 2 */     hal_encoder2,
    /* 3 */     hal_encoder8,
    /* 4 */     hal_encoder3,
    /* 5 */     hal_encoder9
};

#endif



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOappEncReader* eo_appEncReader_New(eOappEncReader_cfg_t *cfg)
{
    EOappEncReader *retptr = NULL;

    if(NULL == cfg)
    {
        return(retptr);
    }
    
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOappEncReader), 1);

    //save in obj its configuration
    memcpy(&(retptr->cfg), cfg, sizeof(eOappEncReader_cfg_t));

    //map application encoder num in hal econder num
    s_eo_appEncReader_mapAppNumbering2HalNumbering(retptr);
    //configure connected encoders
    s_eo_appEncReader_readConnectedEncConfg(retptr, &(retptr->configuredEnc_SPI1.readSeq), HALv1_encoder1, HALv1_encoder3 );
    s_eo_appEncReader_readConnectedEncConfg(retptr, &(retptr->configuredEnc_SPI3.readSeq), HALv1_encoder7, HALv1_encoder9);

    //configure connected encoders
    s_eo_appEncReader_configureConnectedEncoders(retptr, HALv1_encoder1, HALv1_encoder3, &(retptr->configuredEnc_SPI1.readSeq));
    s_eo_appEncReader_configureConnectedEncoders(retptr, HALv1_encoder7, HALv1_encoder9, &(retptr->configuredEnc_SPI3.readSeq));
    
    //reset diagnostics info
    memset(&retptr->dgninfo, 0, sizeof(eOappEncReader_diagnosticsinfo_t));
    
    return(retptr);
}


extern eOresult_t eo_appEncReader_StartRead(EOappEncReader *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    s_eo_appEncReader_check(p);

    if(ENCODER_NULL != p->configuredEnc_SPI1.readSeq.first)
    {   
        p->configuredEnc_SPI1.st = eOEncReader_readSt__started;
        hal_encoder_read_start((hal_encoder_t)p->configuredEnc_SPI1.readSeq.first);  
    }

    if(ENCODER_NULL != p->configuredEnc_SPI3.readSeq.first)
    {   
        p->configuredEnc_SPI3.st = eOEncReader_readSt__started;
        hal_encoder_read_start((hal_encoder_t)p->configuredEnc_SPI3.readSeq.first);  
    }
     
    return(eores_OK);
}

extern eOresult_t  eo_appEncReader_getValuesRaw(EOappEncReader *p, uint32_t *data_ptr)
{
    if((NULL == p) || (NULL == data_ptr))
    {
        return(eores_NOK_nullpointer);
    }

    hal_encoder_get_value(HALv1_encoder1, &data_ptr[0]);
    hal_encoder_get_value(HALv1_encoder2, &data_ptr[2]);
    hal_encoder_get_value(HALv1_encoder3, &data_ptr[4]);

    hal_encoder_get_value(HALv1_encoder7, &data_ptr[1]);
    hal_encoder_get_value(HALv1_encoder8, &data_ptr[3]);
    hal_encoder_get_value(HALv1_encoder9, &data_ptr[5]);

    return(eores_OK);
}


extern eOresult_t  eo_appEncReader_GetValue(EOappEncReader *p, eOappEncReader_encoder_t enc, uint32_t *value)
{
    uint32_t val_raw;
    eOresult_t res;
    eOappEncReader_errortype_t errortype;
    
    res = (eOresult_t)hal_encoder_get_value(encoderMap[enc], &val_raw);
    if(eores_OK != res)
    {
        *value = (uint32_t)err_onReadFromSpi;
        p->dgninfo.enclist[enc][err_onReadFromSpi]++;
        return(res);
    }
   // eOboolvalues_t res = s_eo_appEncReader_IsValidValue(&val_raw, &errortype); 

    if (eobool_false == s_eo_appEncReader_IsValidValue(&val_raw, &errortype))
    {
        *value = (eOappEncReader_errortype_t)errortype;  
        p->dgninfo.enclist[enc][errortype]++;
        return(eores_NOK_generic);
    }

    //val_raw >>= 6;
    //*value = (val_raw & 0x0FFF);
    //*value <<= 4; // 65536 ticks/revolution normalization;
    
    *value = (val_raw>>2) & 0xFFF0; 
    
    return(eores_OK);
}



__inline extern eOboolvalues_t eo_appEncReader_isReady(EOappEncReader *p)
{
    if((eOEncReader_readSt__finished == p->configuredEnc_SPI3.st) && (eOEncReader_readSt__finished == p->configuredEnc_SPI1.st))
    {
        return(eobool_true);
    }
    return(eobool_false);
}


extern eOappEncReader_diagnosticsinfo_t* eo_appEncReader_GetDiagnosticsHandle(EOappEncReader *p)
{
    if(NULL == p)
    {
        return(NULL);
    }

    return(&p->dgninfo);
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_appEncReader_readConnectedEncConfg(EOappEncReader *p, EOappEncReader_configEncSPIXReadSequence_hid_t *cfgEncSPIX, hal_encoder_t startEnc, hal_encoder_t endEnc)
{
    uint8_t i = 0, j = 0;

    //rest data struct
    cfgEncSPIX->first = ENCODER_NULL;
    cfgEncSPIX->list[0] = ENCODER_NULL;
    cfgEncSPIX->list[1] = ENCODER_NULL;
    cfgEncSPIX->list[2] = ENCODER_NULL;

    for(i = startEnc; i <= endEnc; i++)
    {
        if(CHECK_ENC_IS_CONNECTED(p,i))
        {
            if(ENCODER_NULL == cfgEncSPIX->first)
            {
                cfgEncSPIX->first = i;    
            }
            for(j =i+1; j<=endEnc; j++)
            {
                if(CHECK_ENC_IS_CONNECTED(p,j))       
                {
                    cfgEncSPIX->list[(i-startEnc)] = j ;
                    break;    
                }
            }
        }
    }
}



static void s_eo_appEncReader_configureConnectedEncoders(EOappEncReader *p, hal_encoder_t startEnc, hal_encoder_t endEnc, EOappEncReader_configEncSPIXReadSequence_hid_t *cfgEncSPIX)
{
    uint8_t i;
#if     defined(HAL_USE_VERSION_2)    
    hal_encoder_cfg_t enc_cfg =
    {
        .priority           = INTPRIO_SPI_ENCODERS,
        .callback_on_rx     = NULL,
        .arg                = NULL
    };
#else
    hal_encoder_cfg_t enc_cfg =
    {
        .priority = INTPRIO_SPI_ENCODERS,
        .bitrate = hal_encoder_bitrate_500kbps
    };
#endif    

    for(i=startEnc; i<=endEnc; i++)
    {
        if(CHECK_ENC_IS_CONNECTED(p,i))
        {
            if(ENCODER_NULL != cfgEncSPIX->list[(i-startEnc)])
            {
                enc_cfg.callback_on_rx = s_eo_appEncReader_isrCbk_onEncRead;
                enc_cfg.arg = &(cfgEncSPIX->list[(((hal_encoder_t)i)-startEnc)]);
                hal_encoder_init((hal_encoder_t)i,  &enc_cfg);
            }
            else
            {
                //is the last
                if(startEnc == HALv1_encoder1 )
                {
                    enc_cfg.callback_on_rx = s_eo_appEncReader_isrCbk_onLastEncRead_SPI1;
                }
                else
                {
                    enc_cfg.callback_on_rx = s_eo_appEncReader_isrCbk_onLastEncRead_SPI3;
                }
                enc_cfg.arg = p;
                hal_encoder_init((hal_encoder_t)i, &enc_cfg);

            }
        }
    }
}


static void s_eo_appEncReader_isrCbk_onEncRead(void *arg)
{
    hal_encoder_read_start(*((hal_encoder_t*)(arg)));
}

static void s_eo_appEncReader_isrCbk_onLastEncRead_SPI1(void *arg)
{
   EOappEncReader *p = (EOappEncReader *)arg;
 
   p->configuredEnc_SPI1.st = eOEncReader_readSt__finished;
   //if reading on spi3 are already fnished and the user has configured a callback ==> then invoke it
   if((eOEncReader_readSt__finished == p->configuredEnc_SPI3.st) && (NULL != p->cfg.callbackOnLastRead))
   {
        p->cfg.callbackOnLastRead(p->cfg.callback_arg);
   }
}

static void s_eo_appEncReader_isrCbk_onLastEncRead_SPI3(void *arg)
{
   EOappEncReader *p = (EOappEncReader *)arg;

   //set status of reding on spi3
   p->configuredEnc_SPI3.st = eOEncReader_readSt__finished;

   //if reading on spi1 are already fnished and the user has configured a callback ==> then invoke it
   if((eOEncReader_readSt__finished == p->configuredEnc_SPI1.st) && (NULL != p->cfg.callbackOnLastRead))
   {
        p->cfg.callbackOnLastRead(p->cfg.callback_arg);
   }
}

//return 0 in case of error else 1
static eOboolvalues_t s_eo_appEncReader_IsValidValue(uint32_t *valueraw, eOappEncReader_errortype_t *error)
{
    uint8_t parity_error = 0;
    uint8_t b = 0;
    
    for (b=0; b<18; ++b)
    {
        parity_error ^= (*valueraw)>>b;
    }
    
    if (parity_error & 1) 
    { 
        *error = err_onParityError;
        return(eobool_false);
    }
    
    if ((0x38 & *valueraw) != 0x20)
    {
        *error = err_onInvalidValue;
        return(eobool_false);

    }
    
    return(eobool_true);
    
    //return(eobool_true);
}


static void s_eo_appEncReader_mapAppNumbering2HalNumbering(EOappEncReader *p)
{
    uint16_t i;
    p->halConfigEncMask = 0;
    for(i=0; i<eOeOappEncReader_encoderMaxNum; i++)
    {
        if((1<<i) == ((1<<i)&(p->cfg.connectedEncodersMask)))
        {
            p->halConfigEncMask |= (1 << encoderMap[i]);
        }           
    }

}

static void s_eo_appEncReader_check(EOappEncReader *p)
{
   uint32_t index, errtype;
   eOresult_t res;
   uint8_t signal_error = 0;
    
    if(p->dgninfo.count < DGN_COUNT_MAX)
    {
        p->dgninfo.count++;
        return;
    }
        
    for(index=0; (index<eOeOappEncReader_encoderMaxNum) && (0 == signal_error); index++)
    {
        for(errtype=0; (errtype<eOappEncReader_errtype_MaxNum)  && (0 == signal_error); errtype++)
        {
            if(p->dgninfo.enclist[index][errtype] > DGN_THRESHOLD)
            {
                signal_error = 1;
            }
        }
    }
    
    if(1 == signal_error)
    {
        res = eo_theEMSdgn_UpdateApplWithMc(eo_theEMSdgn_GetHandle(), p, p->dgninfo.count);
        if(eores_OK == res)
        {
           eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsapplmc , 0);
        }
        else
        {
            ;//if some error occured while updating var to send, signaling error comes to naught.
        }
    }

    //reset all statistics 
    memset(&p->dgninfo, 0, sizeof(eOappEncReader_diagnosticsinfo_t));
    p->dgninfo.count = 0;
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




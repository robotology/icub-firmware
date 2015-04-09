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

#include "osal.h"

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
//#define CHECK_ENC_IS_CONNECTED(p, enc)    (p->halConfigEncMask[enc] != hal_encoder_tundefined)
#define CHECK_ENC_IS_CONNECTED(p, joint)    ((p->cfg.joints[joint] == eo_appEncReader_enc_type_AEA) || (p->cfg.joints[joint] == eo_appEncReader_enc_type_AMO))
#define CHECK_ENC_IS_ON_SPI(joint_enc)       ((joint_enc == eo_appEncReader_enc_type_AEA) || (joint_enc == eo_appEncReader_enc_type_AMO))

#define INTPRIO_SPI_ENCODERS                hal_int_priority05
#define DGN_COUNT_MAX                       10000 //1 sec
#define DGN_THRESHOLD                       0

#define ENCODER_NULL                        255
#define ENCODER_NOT_SUPPORTED               0xFFFFFFFF

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

static void s_eo_appEncReader_prepareSPIEncodersList(EOappEncReader *p, EOappEncReader_configEncSPIXReadSequence_hid_t *cfgEncSPIX, eo_appEncReader_stream_number_t stream_number);
static void s_eo_appEncReader_prepareSPIEncodersList_new(EOappEncReader *p);
static void s_eo_appEncReader_configure_initSPIConnectedEncoders(EOappEncReader *p, EOappEncReader_configEncSPIXReadSequence_hid_t *cfgEncSPIX, eo_appEncReader_stream_number_t stream_number);
static eOboolvalues_t s_eo_appEncReader_IsValidValue_AEA(uint32_t *valueraw, eOappEncReader_errortype_t *error);
static void s_eo_appEncReader_mapStreams2HalNumbering(EOappEncReader *p);
static void s_eo_appEncReader_configureOtherEncoders(EOappEncReader *p);

static void s_eo_appEncReader_check(EOappEncReader *p);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
#define HALv2_encoder1      hal_encoder1
#define HALv2_encoder2      hal_encoder2
#define HALv2_encoder3      hal_encoder3
#define HALv2_encoder4      hal_encoder4
#define HALv2_encoder5      hal_encoder5
#define HALv2_encoder6      hal_encoder6

// SPI Encoders MAP for output purpose (Port number ordering)
static const hal_encoder_t SPIencodersMap[eOappEncReader_joint_numberof] =
{
    /* 0 */     HALv2_encoder1,
    /* 1 */     HALv2_encoder4,
    /* 2 */     HALv2_encoder2,
    /* 3 */     HALv2_encoder5,
    /* 4 */     HALv2_encoder3,
    /* 5 */     HALv2_encoder6
};


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
		
    //check encoders connected and fill out the list of the two SPI streams
    //s_eo_appEncReader_prepareSPIEncodersList(retptr, &(retptr->configuredEnc_SPI1.readSeq), eo_appEncReader_stream0);
    //s_eo_appEncReader_prepareSPIEncodersList(retptr, &(retptr->configuredEnc_SPI3.readSeq), eo_appEncReader_stream1);
    
    s_eo_appEncReader_prepareSPIEncodersList_new(retptr);

    //configure&init connected SPI encoders
    s_eo_appEncReader_configure_initSPIConnectedEncoders(retptr, &(retptr->configuredEnc_SPI1.readSeq), eo_appEncReader_stream0);
    s_eo_appEncReader_configure_initSPIConnectedEncoders(retptr, &(retptr->configuredEnc_SPI3.readSeq), eo_appEncReader_stream1);
   
    //initialize the other encoders
    s_eo_appEncReader_configureOtherEncoders(retptr);
    //reset diagnostics info
    memset(&retptr->dgninfo, 0, sizeof(eOappEncReader_diagnosticsinfo_t));
    
    memset(&retptr->times[0], 0, 4); // spi1
    memset(&retptr->times[1], 0, 4); // spi3
    
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
        memset(&p->times[0], 0, 4); // spi1  
        p->times[0][0] = osal_system_abstime_get();
        p->configuredEnc_SPI1.st = eOEncReader_readSt__started;
        hal_encoder_read_start((hal_encoder_t)p->configuredEnc_SPI1.readSeq.first);  
    }

    if(ENCODER_NULL != p->configuredEnc_SPI3.readSeq.first)
    {   
        memset(&p->times[1], 0, 4); // spi3   
        p->times[1][0] = osal_system_abstime_get();        
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
		// Feature disabled at the moment
		
    //hal_encoder_get_value(HALv2_encoder1, &data_ptr[0]);
    //hal_encoder_get_value(HALv2_encoder2, &data_ptr[2]);
    //hal_encoder_get_value(HALv2_encoder3, &data_ptr[4]);

    //hal_encoder_get_value(HALv2_encoder7, &data_ptr[1]);
    //hal_encoder_get_value(HALv2_encoder8, &data_ptr[3]);
    //hal_encoder_get_value(HALv2_encoder9, &data_ptr[5]);

    return(eores_OK);
}


extern eOresult_t  eo_appEncReader_GetJointValue(EOappEncReader *p, uint8_t joint_number, uint32_t *primary_value,
                                            uint32_t *secondary_value, hal_encoder_errors_flags *flags)
{
    uint32_t val_raw;
    eOresult_t res = eores_NOK_generic;
    eOappEncReader_errortype_t errortype;
    eOappEncReader_joint_t this_joint;
	
    //search the right joint in the array
    //surely this is not the best way to do it...
    for (uint8_t i = 0; i < p->cfg.joints_number ; i++)
    {
        if (p->cfg.joints[i].joint_pos == joint_number)
        {
            this_joint = p->cfg.joints[i];
            break;
        }
    }
    
    //check existence
    if (this_joint.primary_encoder != eo_appEncReader_enc_type_NONE)
    {
        switch (this_joint.primary_encoder)
        {
            case eo_appEncReader_enc_type_AEA:
            {
                EOappEncReader_configEncSPIXReadSequence_hid_t read_stream; 
                if (this_joint.SPI_stream_associated == eo_appEncReader_stream0)
                {
                    read_stream = p->configuredEnc_SPI1.readSeq;
                }
                else if (this_joint.SPI_stream_associated == eo_appEncReader_stream1)
                {
                    read_stream = p->configuredEnc_SPI3.readSeq;
                }
                else
                {
                    return eores_NOK_generic;
                }

                if (this_joint.SPI_stream_position == 0)
                {
                    res = (eOresult_t)hal_encoder_get_value(read_stream.first, &val_raw, flags);
                }
                else
                {
                    res = (eOresult_t)hal_encoder_get_value(read_stream.list[this_joint.SPI_stream_position], &val_raw, flags);
                }
                if(eores_OK != res)
                {
                    //*value = (uint32_t)err_onReadFromSpi;
                    p->dgninfo.enclist[joint_number][err_onReadFromSpi]++;
                    //return(res);
                }

                if (eobool_false == s_eo_appEncReader_IsValidValue_AEA(&val_raw, &errortype))
                {
                    //*value = (eOappEncReader_errortype_t)errortype;  
                    p->dgninfo.enclist[joint_number][errortype]++;
                    flags->data_error = 1;
                    //return(eores_NOK_generic);
                }

                //val_raw >>= 6;
                //*value = (val_raw & 0x0FFF);
                //*value <<= 4; // 65536 ticks/revolution normalization;
                *primary_value = (val_raw>>2) & 0xFFF0;
                break;
            }   			

            case eo_appEncReader_enc_type_AMO:
            {
                EOappEncReader_configEncSPIXReadSequence_hid_t read_stream; 
                if (this_joint.SPI_stream_associated == eo_appEncReader_stream0)
                {
                    read_stream = p->configuredEnc_SPI1.readSeq;
                }
                else if (this_joint.SPI_stream_associated == eo_appEncReader_stream1)
                {
                    read_stream = p->configuredEnc_SPI3.readSeq;
                }
                else
                {
                    return eores_NOK_generic;
                }
                
                if (this_joint.SPI_stream_position == 0)
                {
                    res = (eOresult_t)hal_encoder_get_value(read_stream.first, &val_raw, flags);
                }
                else
                {
                    res = (eOresult_t)hal_encoder_get_value(read_stream.list[this_joint.SPI_stream_position], &val_raw, flags);
                }
                if(eores_OK != res)
                {
                    //*value = (uint32_t)err_onReadFromSpi;
                    p->dgninfo.enclist[joint_number][err_onReadFromSpi]++;
                    //return(res);
                }
            
                *primary_value = (val_raw>>4) & 0xFFFF;
                break;
            }
            case eo_appEncReader_enc_type_INC:
            {
                *primary_value = hal_quad_enc_getCounter(joint_number);
                break;
            }
            default:
                return eores_NOK_generic;
            // should handle the other cases...
        }	
        return((eOresult_t)res); 
    }
    return eores_NOK_generic;
}
extern eOresult_t  eo_appEncReader_GetValue(EOappEncReader *p, eOappEncReader_encoder_t enc, uint32_t *value, hal_encoder_errors_flags *flags)
{
    uint32_t val_raw;
    eOresult_t res = eores_NOK_generic;
    eOappEncReader_errortype_t errortype;
			
    //If AEA encoder
    //if(p->halConfigEncMask[encoderMap[enc]] == hal_encoder_t1)
    {
    //	res = (eOresult_t)hal_encoder_get_value(encoderMap[enc], &val_raw, flags);
        if(eores_OK != res)
        {
                //*value = (uint32_t)err_onReadFromSpi;
                p->dgninfo.enclist[enc][err_onReadFromSpi]++;
                //return(res);
        }

        if (eobool_false == s_eo_appEncReader_IsValidValue_AEA(&val_raw, &errortype))
        {
                //*value = (eOappEncReader_errortype_t)errortype;  
                p->dgninfo.enclist[enc][errortype]++;
                flags->data_error = 1;
                //return(eores_NOK_generic);
        }

        //val_raw >>= 6;
        //*value = (val_raw & 0x0FFF);
        //*value <<= 4; // 65536 ticks/revolution normalization;
        *value = (val_raw>>2) & 0xFFF0;
    }			

    //If AMO encoder
    //if(p->halConfigEncMask[encoderMap[enc]] == hal_encoder_t2)
    {
    //	res = (eOresult_t)hal_encoder_get_value(encoderMap[enc], &val_raw, flags);
        if(eores_OK != res)
        {
                //*value = (uint32_t)err_onReadFromSpi;
                p->dgninfo.enclist[enc][err_onReadFromSpi]++;
                //return(res);
        }
        
        *value = (val_raw>>4) & 0xFFFF;
    }
		
    return((eOresult_t)res);
}

extern uint64_t eo_appEncReader_startSPI1(EOappEncReader *p)
{
    return p->times[0][0];
}

extern uint64_t eo_appEncReader_startSPI3(EOappEncReader *p)
{
    return p->times[1][0];
}

extern uint32_t eo_appEncReader_deltaSPI1(EOappEncReader *p)
{
    uint64_t d = p->times[0][3] - p->times[0][0];
    return((uint32_t)d);
}

extern uint32_t eo_appEncReader_deltaSPI3(EOappEncReader *p)
{
    uint64_t d = p->times[1][3] - p->times[1][0];
    return((uint32_t)d);    
}

__inline extern eOboolvalues_t eo_appEncReader_isReady(EOappEncReader *p)
{
  if((p->configuredEnc_SPI1.readSeq.first != ENCODER_NULL) && (p->configuredEnc_SPI3.readSeq.first != ENCODER_NULL))
	{
		if((eOEncReader_readSt__finished == p->configuredEnc_SPI3.st) && (eOEncReader_readSt__finished == p->configuredEnc_SPI1.st))
			{
					return(eobool_true);
			}
			return(eobool_false);
	}
	else if (p->configuredEnc_SPI1.readSeq.first == ENCODER_NULL)
	{
		return eo_appEncReader_isReadySPI3(p);
	}
	else if (p->configuredEnc_SPI3.readSeq.first == ENCODER_NULL)
	{
		return eo_appEncReader_isReadySPI1(p);
	}
	return(eobool_false);
}

__inline extern eOboolvalues_t eo_appEncReader_isReadySPI1(EOappEncReader *p)
{
    if((eOEncReader_readSt__finished == p->configuredEnc_SPI1.st))
    {
        return(eobool_true);
    }
    return(eobool_false);
}

__inline extern eOboolvalues_t eo_appEncReader_isReadySPI3(EOappEncReader *p)
{
    if((eOEncReader_readSt__finished == p->configuredEnc_SPI3.st))
    {
        return(eobool_true);
    }
    return(eobool_false);
}

extern eOresult_t eo_appEncReader_QuadEnc_init (EOappEncReader *p, uint8_t enc_number)
{
    hal_quad_enc_single_init(enc_number);
    return(eores_OK);
}

extern eOresult_t eo_appEncReader_QuadEnc_GetValue (EOappEncReader *p, uint8_t enc_number, uint32_t *value)
{
 //   if((p->cfg.quad_enc_mask >> enc_number) & 0x01)
    {
        *value = hal_quad_enc_getCounter(enc_number);
    }
        return(eores_OK);
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

static void s_eo_appEncReader_prepareSPIEncodersList(EOappEncReader *p, EOappEncReader_configEncSPIXReadSequence_hid_t *cfgEncSPIX, eo_appEncReader_stream_number_t stream_number)
{
    uint8_t i = 0, j = 0;
    
    //initialize data struct
    cfgEncSPIX->first   = ENCODER_NULL;
    cfgEncSPIX->list[0] = ENCODER_NULL;
    cfgEncSPIX->list[1] = ENCODER_NULL;
    cfgEncSPIX->list[2] = ENCODER_NULL;
    cfgEncSPIX->list[3] = ENCODER_NULL; //is dummy, but useful cause it signals the end of the list

    //reference to useful info
    eOappEncReader_stream_t this_stream = p->cfg.SPI_streams[stream_number];
    uint8_t enc_numbers = p->cfg.SPI_streams[stream_number].numberof;
    
    for (i = 0; i < enc_numbers; i++)
    {
         //set the first if it's still NULL
         if((ENCODER_NULL == cfgEncSPIX->first) && (this_stream.encoders[i] != hal_encoderNONE))
         {
            cfgEncSPIX->first = this_stream.encoders[i];    
         }
         // set all the other encoders in list
         for(j = i; j < enc_numbers; j++)
         {
            if (this_stream.encoders[j] != hal_encoderNONE)
            {
                cfgEncSPIX->list[j] = this_stream.encoders[j];
            }
            break;
         }
    }
    
    /*
    for(i = 0; i < p->cfg.joints_number; i++)
    {
        if (CHECK_ENC_IS_ON_SPI(p->cfg.joints[i].primary_encoder))
        {
            //set the first if it's still NULL
            if(ENCODER_NULL == cfgEncSPIX->first)
            {
                cfgEncSPIX->first = p->cfg.SPI_streams[stream_number].encoders;    
            }
            for(j = i+1; j<= p->cfg.joints_number; j++)
            {
                if (CHECK_ENC_IS_ON_SPI(p->cfg.joints[j].primary_encoder))       
                {
                    cfgEncSPIX->list[(i-p->cfg.joints_number)] = SPIencodersMap[j] ;
                    break;    
                }
            }
        }
    }
    */
}

static void s_eo_appEncReader_prepareSPIEncodersList_new(EOappEncReader *p)
{
    uint8_t joints_number = p->cfg.joints_number;
    eOappEncReader_joint_t current_joint;
 
    //initialize data struct
    p->configuredEnc_SPI1.readSeq.first  = ENCODER_NULL, p->configuredEnc_SPI3.readSeq.first  = ENCODER_NULL;
    p->configuredEnc_SPI1.readSeq.list[0] = ENCODER_NULL, p->configuredEnc_SPI3.readSeq.list[0] = ENCODER_NULL;
    p->configuredEnc_SPI1.readSeq.list[1] = ENCODER_NULL, p->configuredEnc_SPI3.readSeq.list[1] = ENCODER_NULL;
    p->configuredEnc_SPI1.readSeq.list[2] = ENCODER_NULL, p->configuredEnc_SPI3.readSeq.list[2] = ENCODER_NULL;
    p->configuredEnc_SPI1.readSeq.list[3] = ENCODER_NULL, p->configuredEnc_SPI3.readSeq.list[3] = ENCODER_NULL; //is dummy, but useful cause it signals the end of the list
    
    //cycle for all the joints
    for (uint8_t i = 0; i<joints_number; i++)
    {
        current_joint = p->cfg.joints[i];
        
        //if it's an SPI encoders
        if ((current_joint.primary_encoder == eo_appEncReader_enc_type_AEA) || (current_joint.primary_encoder == eo_appEncReader_enc_type_AMO))
        {
            // fill out the correct list of stream
            // stream0
            if (current_joint.SPI_stream_associated == eo_appEncReader_stream0)
            {
                if (current_joint.SPI_stream_position == 0)
                {
                    p->configuredEnc_SPI1.readSeq.first     = current_joint.SPI_encoder;
                    p->configuredEnc_SPI1.readSeq.list[0]   = current_joint.SPI_encoder;
                }
                else
                {
                    p->configuredEnc_SPI1.readSeq.list[current_joint.SPI_stream_position]   = current_joint.SPI_encoder;
                }                    
            }
            
            // stream1
            else if (current_joint.SPI_stream_associated == eo_appEncReader_stream1)
            {
                if (current_joint.SPI_stream_position == 0)
                {
                    p->configuredEnc_SPI3.readSeq.first     = current_joint.SPI_encoder;
                    p->configuredEnc_SPI3.readSeq.list[0]   = current_joint.SPI_encoder;
                }
                else
                {
                    p->configuredEnc_SPI3.readSeq.list[current_joint.SPI_stream_position]   = current_joint.SPI_encoder;
                }                    
            }
        }
    }
}


static void s_eo_appEncReader_configure_initSPIConnectedEncoders(EOappEncReader *p, EOappEncReader_configEncSPIXReadSequence_hid_t *cfgEncSPIX, eo_appEncReader_stream_number_t stream_number)
{
    uint8_t i;
	hal_encoder_cfg_t enc_cfg;
    
    //reference to useful info
    eOappEncReader_stream_t this_stream = p->cfg.SPI_streams[stream_number];
    uint8_t enc_numbers = p->cfg.SPI_streams[stream_number].numberof;

    for(i=0; i<enc_numbers; i++)
    {
      //AEA Encoders config
      if(this_stream.type == hal_encoder_t1)
      {
          enc_cfg.priority     		= INTPRIO_SPI_ENCODERS;
          enc_cfg.callback_on_rx    = NULL;
          enc_cfg.arg               = NULL;
          enc_cfg.type				= hal_encoder_t1;
          enc_cfg.reg_address	    = NULL;
          enc_cfg.sdata_precheck    = hal_false;
      }
      //AMO Encoder config
      else if(this_stream.type == hal_encoder_t2)
      {
          enc_cfg.priority      	= INTPRIO_SPI_ENCODERS;
          enc_cfg.callback_on_rx    = NULL;
          enc_cfg.arg               = NULL;
          enc_cfg.type				= hal_encoder_t2;
          enc_cfg.reg_address		= 0x77;
          enc_cfg.sdata_precheck	= hal_false;
      }
      else
      {
         return; //case not supported 
      }
      
      //normal init if it's not the last
      if(ENCODER_NULL != cfgEncSPIX->list[i+1])
      {
          enc_cfg.callback_on_rx = s_eo_appEncReader_isrCbk_onEncRead;
          // the argument is the next encoder to be started
          enc_cfg.arg = &(cfgEncSPIX->list[i+1]);
          //init low level object
          hal_encoder_init((hal_encoder_t)i,  &enc_cfg);
      }
      //if it's the last
      else
      {
          if(stream_number == eo_appEncReader_stream0 )
          {
            enc_cfg.callback_on_rx = s_eo_appEncReader_isrCbk_onLastEncRead_SPI1;
          }
          else if (stream_number == eo_appEncReader_stream1 )
          {
            enc_cfg.callback_on_rx = s_eo_appEncReader_isrCbk_onLastEncRead_SPI3;
          }
          enc_cfg.arg = p;
          hal_encoder_init(this_stream.encoders[i], &enc_cfg);
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
    
   p->times[0][3] = osal_system_abstime_get(); 
 
   p->configuredEnc_SPI1.st = eOEncReader_readSt__finished;
   //if reading on spi3 are already fnished and the user has configured a callback ==> then invoke it
   if((eOEncReader_readSt__finished == p->configuredEnc_SPI3.st) && (NULL != p->cfg.SPI_callbackOnLastRead))
   {
        p->cfg.SPI_callbackOnLastRead(p->cfg.SPI_callback_arg);
   }
}

static void s_eo_appEncReader_isrCbk_onLastEncRead_SPI3(void *arg)
{
   EOappEncReader *p = (EOappEncReader *)arg;
    
   p->times[1][3] = osal_system_abstime_get();

   //set status of reding on spi3
   p->configuredEnc_SPI3.st = eOEncReader_readSt__finished;

   //if reading on spi1 are already fnished and the user has configured a callback ==> then invoke it
   if((eOEncReader_readSt__finished == p->configuredEnc_SPI1.st) && (NULL != p->cfg.SPI_callbackOnLastRead))
   {
        p->cfg.SPI_callbackOnLastRead(p->cfg.SPI_callback_arg);
   }
}

//return 0 in case of error else 1
static eOboolvalues_t s_eo_appEncReader_IsValidValue_AEA(uint32_t *valueraw, eOappEncReader_errortype_t *error)
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
static void s_eo_appEncReader_mapStreams2HalNumbering(EOappEncReader *p)
{
	  // New version: array of hal_encoder_type 
	  // Apply a remapping based on hal numbering
	
	  uint8_t i,j;
	  //uint8_t complete_stream_length = eOeOappEncReader_encoderMaxNum/eo_appEncReader_streams_numberof;
		// For all the possible streams
		for (j=0; j < eo_appEncReader_streams_numberof; j++)
		{
			// Cycle on all the possible encoder indexes
	//		for(i=0; i < complete_stream_length; i++)
			{
				// Check if present
	//			if(p->cfg.streams[j].encoders[i] == hal_encoderNONE)
				{
	//				p->halConfigEncMask[i+j*complete_stream_length] = hal_encoder_tundefined;
					continue;
				}
				
				//Set the right type
		//		if (p->cfg.streams[j].type == hal_encoder_t1)
				{
		//			p->halConfigEncMask[i+j*complete_stream_length] = hal_encoder_t1;
					//p->halConfigEncMask[p->cfg.streams[j].encoders[i]] = hal_encoder_t1;
				}
	//			else if(p->cfg.streams[j].type == hal_encoder_t2)
				{
		//			p->halConfigEncMask[i+j*complete_stream_length] = hal_encoder_t2;
					//p->halConfigEncMask[p->cfg.streams[j].encoders[i]] = hal_encoder_t2;
				}
			}
		}
}


static void s_eo_appEncReader_configureOtherEncoders(EOappEncReader *p)
{
    uint8_t joints_number = p->cfg.joints_number;
    
    for (uint8_t i = 0; i< joints_number; i++)
    {
        eOappEncReader_joint_t current_joint = p->cfg.joints[i];
        if ((current_joint.primary_encoder == eo_appEncReader_enc_type_INC) || (current_joint.extra_encoder == eo_appEncReader_enc_type_INC))
        {
            hal_quad_enc_single_init(current_joint.joint_pos);
        }
        /* handle other cases...
        else if
        {
            
        }
        */
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
        
//    for(index=0; (index<eOeOappEncReader_encoderMaxNum) && (0 == signal_error); index++)
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




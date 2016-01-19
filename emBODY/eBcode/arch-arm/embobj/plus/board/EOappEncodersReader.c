/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Davide Pollarolo
 * email:   valentina.gaggero@iit.it, davide.pollarolo@iit.it
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
    @author    valentina.gaggero@iit.it, davide.pollarolo@iit.it
    @date       02/17/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_encoder.h"


#include "EOtheMemoryPool.h"

#include "OPCprotocolManager_Cfg.h" 

#include "EOemsControllerCfg.h"
#include "osal.h"

#include "EoProtocol.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOappEncodersReader.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOappEncodersReader_hid.h"


// - part which must be taken from hal by means of a proper functions. for now we wrap them 
extern const hal_encoder_spimapping_t * fake_hal_encoder_spimapping_get(void);

extern void fake_hal_quad_enc_single_init(uint8_t port);

extern uint32_t fake_hal_quad_enc_getCounter(uint8_t port);

extern uint32_t fake_hal_adc_get_hall_sensor_analog_input_mV(uint8_t port);


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define CHECK_ENC_IS_CONNECTED(joint_enc)       (joint_enc != eo_appEncReader_enc_type_NONE)
#define CHECK_ENC_IS_ON_SPI(joint_enc)          ((joint_enc == eo_appEncReader_enc_type_AEA) || (joint_enc == eo_appEncReader_enc_type_AMO))

#define INTPRIO_SPI_ENCODERS                    hal_int_priority05
#define DGN_COUNT_MAX                           10000 //1 sec
#define DGN_THRESHOLD                           0

#define ENCODER_NULL                            255
#define ENCODER_VALUE_NOT_SUPPORTED             0xFFFFFFFF

#define RESCALE_IN_ICUB_DEGREES(val,fullscale)  (TICKS_PER_REVOLUTION*val)/fullscale    

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

static void s_eo_clear_SPI_streams(EOappEncReader *p);

static eObool_t s_eo_prepare_SPI_streams(EOappEncReader *p);

static void s_eo_deconfigure_SPI_encoders(EOappEncReader *p);
static eObool_t s_eo_configure_SPI_encoders(EOappEncReader *p);

static void s_eo_appEncReader_isrCbk_onEncRead(void *arg);
static void s_eo_appEncReader_isrCbk_onLastEncRead_SPI_stream0(void *arg);
static void s_eo_appEncReader_isrCbk_onLastEncRead_SPI_stream1(void *arg);

static void s_eo_appEncReader_deinit_ListOfSPIencoders(EOappEncReader *p,  EOappEncReader_confEncDataPerSPI_hid_t *cfgSPIX, eo_appEncReader_stream_number_t stream_number);
static void s_eo_appEncReader_init_ListOfSPIencoders(EOappEncReader *p, EOappEncReader_confEncDataPerSPI_hid_t *cfgSPIX, eo_appEncReader_stream_number_t stream_number);

static void s_eo_appEncReader_deinit_SPIencoders(EOappEncReader *p, EOappEncReader_confEncDataPerSPI_hid_t *cfgSPIX, eo_appEncReader_stream_number_t stream_number);
static void s_eo_appEncReader_init_SPIencoders(EOappEncReader *p, EOappEncReader_confEncDataPerSPI_hid_t *cfgSPIX, eo_appEncReader_stream_number_t stream_number);

static eObool_t s_eo_appEncReader_IsValidValue_AEA(uint32_t *valueraw, eOappEncReader_errortype_t *error);

static void s_eo_appEncReader_deconfigure_NONSPI_encoders(EOappEncReader *p);
static void s_eo_appEncReader_configure_NONSPI_encoders(EOappEncReader *p);

static uint32_t s_eo_appEncReader_rescale2icubdegrees(uint32_t val_raw, uint8_t jomo, eo_appEncReader_encoder_place_t pos_type);


EO_extern_inline eObool_t eo_appEncReader_isReadySPI_stream0(EOappEncReader *p);
EO_extern_inline eObool_t eo_appEncReader_isReadySPI_stream1(EOappEncReader *p);


static eo_appEncReader_stream_number_t s_eo_appEncReader_get_spi_stream(EOappEncReader* p, uint8_t port);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static EOappEncReader s_eo_theappencreader =
{
    .initted                    = eobool_false,
    .active                     = eobool_false,
    .spimap                     = NULL,
    .config                     = {0},
    .SPI_streams                = {{.type = hal_encoder_typeNONE, .numberof = 0}, {.type = hal_encoder_typeNONE, .numberof = 0}},
    .configuredEnc_SPI_stream0  = {0},
    .configuredEnc_SPI_stream1  = {0},
    .times                      = {0}   
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOappEncReader* eo_appEncReader_Initialise(void)
{
    if(eobool_true == s_eo_theappencreader.initted)
    {
        return(&s_eo_theappencreader);
    }
    
    // do what ever is needed apart from what depends on the configuration
    
    // step 1: retrieve the spi mapping from hal
    
    s_eo_theappencreader.spimap = fake_hal_encoder_spimapping_get();
    
    if(NULL == s_eo_theappencreader.spimap)
    {
        // marco.accame: put a diagnostics message about the failure
        return(NULL);
    }   

    s_eo_clear_SPI_streams(&s_eo_theappencreader);   
    
    // nothing else to do in here. everything else must be done with a proper configuration
    
    s_eo_theappencreader.initted = eobool_true;
    return(&s_eo_theappencreader);
}



extern EOappEncReader* eo_appEncReader_GetHandle(void)
{
    if(eobool_true == s_eo_theappencreader.initted)
    {
        return(&s_eo_theappencreader);
    }
    
    return(NULL); 
}


extern eOresult_t eo_appEncReader_Deactivate(EOappEncReader *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == s_eo_theappencreader.active)
    {
        return(eores_OK);        
    } 
    
  
    // clean the things prepared with _Activate() ... in reverse order
    
    s_eo_appEncReader_deconfigure_NONSPI_encoders(p);

    s_eo_deconfigure_SPI_encoders(p);
    
    memset(&p->config, 0, sizeof(eOappEncReader_cfg_t));
    p->config.numofjomos = 0;
    
    s_eo_theappencreader.active = eobool_false;    
    return(eores_OK);
}


extern eOresult_t eo_appEncReader_Activate(EOappEncReader *p, eOappEncReader_cfg_t *cfg)
{
    if((NULL == p) || (NULL == cfg))
    {
        return(eores_NOK_nullpointer);
    }    
    
    if(eobool_true == s_eo_theappencreader.active)
    {
        eo_appEncReader_Deactivate(p);        
    }    

    // ok, now the rest of the things
    
    // basic check ...
    if(cfg->numofjomos > eOappEncReader_jomos_maxnumberof)
    {
        return(eores_NOK_generic);
    }
    
    // 1. copy the config
    memcpy(&p->config, cfg, sizeof(eOappEncReader_cfg_t));
    
    // 2. configure spi encoders   
    s_eo_configure_SPI_encoders(p);

    // 3. configure other encoders
    s_eo_appEncReader_configure_NONSPI_encoders(p);
        
    s_eo_theappencreader.active = eobool_true;    
    return(eores_OK);
}


//extern EOappEncReader* eo_appEncReader_New(eOappEncReader_cfg_t *cfg)
//{
//    EOappEncReader *retptr = NULL;

//    if(NULL == cfg)
//    {
//        return(retptr);
//    }
//    
//    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOappEncReader), 1);

//    //save in obj its configuration
//    memcpy(&(retptr->config), cfg, sizeof(eOappEncReader_cfg_t));
//    
//    
//    // step 1: retrieve the spi mapping from hal
//    
//    retptr->spimap = fake_hal_encoder_spimapping_get();
//    
//    if(NULL == retptr->spimap)
//    {
//        // marco.accame: put a diagnostics message or a failure.
//        return(NULL);
//    }
//    
//    // prepare SPI_streams
//    if(eobool_false == s_eo_prepare_SPI_streams(retptr))
//    {
//        return(NULL);
//    }
//		
//    //check encoders connected and fill out the list of the two SPI streams    
//    s_eo_appEncReader_init_ListOfSPIencoders(retptr, &(retptr->configuredEnc_SPI_stream0), eo_appEncReader_stream0);
//    s_eo_appEncReader_init_ListOfSPIencoders(retptr, &(retptr->configuredEnc_SPI_stream1), eo_appEncReader_stream1);

//    //configure&init connected SPI encoders
//    s_eo_appEncReader_init_SPIencoders(retptr, &(retptr->configuredEnc_SPI_stream0), eo_appEncReader_stream0);
//    s_eo_appEncReader_init_SPIencoders(retptr, &(retptr->configuredEnc_SPI_stream1), eo_appEncReader_stream1);
//   
//    //initialize the other encoders
//    s_eo_appEncReader_configure_NONSPI_encoders(retptr);
//    
//    memset(&retptr->times[0], 0, 4); // spi first stream
//    memset(&retptr->times[1], 0, 4); // spi second stream
//    
//    return(retptr);
//}


extern eOresult_t eo_appEncReader_StartRead(EOappEncReader *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    //s_eo_appEncReader_check(p);

    if(ENCODER_NULL != p->configuredEnc_SPI_stream0.readSeq.first)
    { 
        memset(&p->times[0], 0, 4); // spi stream0
        p->times[0][0] = osal_system_abstime_get();
        p->configuredEnc_SPI_stream0.st = eOEncReader_readSt__started;
        hal_encoder_read_start((hal_encoder_t)p->configuredEnc_SPI_stream0.readSeq.first);  
    }

    if(ENCODER_NULL != p->configuredEnc_SPI_stream1.readSeq.first)
    {   
        memset(&p->times[1], 0, 4); // spi stream1   
        p->times[1][0] = osal_system_abstime_get();        
        p->configuredEnc_SPI_stream1.st = eOEncReader_readSt__started;
        hal_encoder_read_start((hal_encoder_t)p->configuredEnc_SPI_stream1.readSeq.first);  
    }
     
    return(eores_OK);
}



extern eOresult_t eo_appEncReader_GetValue(EOappEncReader *p, uint8_t jomo, uint32_t *primary_value,
                                            uint32_t *extra_value, hal_encoder_errors_flags *flags)
{    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    hal_encoder_position_t val_raw = 0; // marco.accame: it should be is a hal_encoder_position_t 
    eOresult_t res1 = eores_OK;
    eOresult_t res2 = eores_OK;
    eOappEncReader_errortype_t errortype = err_NONE;
    eOappEncReader_jomoconfig_t this_jomoconfig = p->config.jomoconfig[jomo];
	 
    // check existence for primary encoder
    if (CHECK_ENC_IS_CONNECTED(this_jomoconfig.primary_encoder_type))
    {
        switch (this_jomoconfig.primary_encoder_type)
        {
            case eo_appEncReader_enc_type_AEA:
            {
                EOappEncReader_configEncSPIXReadSequence_hid_t read_stream; 
                if (p->spimap->usedstream[this_jomoconfig.primary_encoder_port] == eo_appEncReader_stream0)
                {
                    read_stream = p->configuredEnc_SPI_stream0.readSeq;
                }
                else if (p->spimap->usedstream[this_jomoconfig.primary_encoder_port] == eo_appEncReader_stream1)
                {
                    read_stream = p->configuredEnc_SPI_stream1.readSeq;
                }
                else
                {
                    return eores_NOK_generic;
                }

                res1 = (eOresult_t)hal_encoder_get_value((hal_encoder_t)read_stream.list[p->spimap->indexinstream[this_jomoconfig.primary_encoder_port]], &val_raw, flags);
                
                if(eores_OK != res1)
                {
                    *primary_value = (uint32_t)err_onReadFromSpi;
//                    p->dgninfo.enclist[jomo][err_onReadFromSpi]++;
                    return((eOresult_t)res1);
                }

                if(eobool_false == s_eo_appEncReader_IsValidValue_AEA(&val_raw, &errortype))
                {
                    *primary_value = (eOappEncReader_errortype_t)errortype;  
//                    p->dgninfo.enclist[jomo][errortype]++;
                    flags->data_error = 1;
                    return(eores_NOK_generic);
                }

                //val_raw >>= 6;
                //*value = (val_raw & 0x0FFF);
                //*value <<= 4; // 65536 ticks/revolution normalization;
                //val_raw = (val_raw>>2) & 0xFFF0;
                //*primary_value = RESCALE_IN_ICUB_DEGREES(val_raw, encoders_fullscales[eo_appEncReader_enc_type_AEA]);
                // marco.accame: hal_encoder_get_value() gives back a value in uint32_t with only 18 bits of information (internally masked with 0x03FFFF).
                // only the 12 most significant bits contain a position reading. to obtain the ticks we should do:
                // ticks = (val_raw >> 6) & 0x0FFF;
                // the resolution is now 4096 ticks per revolution.
                // however, wrong or rigth, historically there has been representation of AEA readings with resolution of 16 bits per revolution, so that they are represented in icub-degrees.
                // hence a further factor 16 (a shift of 4) is applied:
                // ticks <<= 4;
                // all this is equivalent to perform only one shift of 2 and to set to zero the least significant 4 bits
                // val_raw = (val_raw>>2) & 0xFFF0;
                
                #warning: marco.accame: if we want to rescale the aea reading using the GENERAL:Encoders factor, then instead of the RESCALE_IN...  macro we use the following:
                val_raw = (val_raw >> 6) & 0x0FFF;
                *primary_value = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, this_jomoconfig.primary_encoder_place);                
               
            } break; 			

            case eo_appEncReader_enc_type_AMO:
            {
                EOappEncReader_configEncSPIXReadSequence_hid_t read_stream; 
                if (p->spimap->usedstream[this_jomoconfig.primary_encoder_port] == eo_appEncReader_stream0)
                {
                    read_stream = p->configuredEnc_SPI_stream0.readSeq;
                }
                else if (p->spimap->usedstream[this_jomoconfig.primary_encoder_port] == eo_appEncReader_stream1)
                {
                    read_stream = p->configuredEnc_SPI_stream1.readSeq;
                }
                else
                {
                    return eores_NOK_generic;
                }
                
                
                res1 = (eOresult_t)hal_encoder_get_value((hal_encoder_t)read_stream.list[p->spimap->indexinstream[this_jomoconfig.primary_encoder_port]], &val_raw, flags);
            
                if(eores_OK != res1)
                {
                    *primary_value = (uint32_t)err_onReadFromSpi;
//                    p->dgninfo.enclist[jomo][err_onReadFromSpi]++;
                    return((eOresult_t)res1);
                }
            
                val_raw = (val_raw>>4) & 0xFFFF;
                //*primary_value = RESCALE_IN_ICUB_DEGREES(val_raw, encoders_fullscales[eo_appEncReader_enc_type_AMO]);
                *primary_value = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, this_jomoconfig.primary_encoder_place);
                
            } break;
            
            case eo_appEncReader_enc_type_INC:
            {
#if 1                
                val_raw = fake_hal_quad_enc_getCounter(this_jomoconfig.primary_encoder_port);
                if(ENCODER_VALUE_NOT_SUPPORTED == val_raw)
                {
                    *primary_value = 0;
                }
                else
                {
                    *primary_value = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, this_jomoconfig.primary_encoder_place);
                }    
#else                
//                #if defined(EOAPPENCODERREADER_DONTUSE_INC)
//                *primary_value = 0;
//                #else
//                val_raw = fake_hal_quad_enc_getCounter(this_jomoconfig.primary_encoder_port);
//                //val_raw = val_raw & 0xFFFF;
//                //*primary_value = RESCALE_IN_ICUB_DEGREES(val_raw, encoders_fullscales[eo_appEncReader_enc_type_INC]);
//                // marco.accame: by this we use the encoder ticks and rescale them in icubdegrees using the factor in GENERAL:Encoders
//                *primary_value = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, this_jomoconfig.primary_encoder_place);
//                #endif
#endif                
                res1 = eores_OK;
                
            } break;
            
            case eo_appEncReader_enc_type_ADH:
            {
                // get the voltage from the motor port (0 - 3300mV)
#if 1                
                val_raw = fake_hal_adc_get_hall_sensor_analog_input_mV(this_jomoconfig.primary_encoder_port);        
                if(ENCODER_VALUE_NOT_SUPPORTED == val_raw)
                {
                    *primary_value = ENCODER_VALUE_NOT_SUPPORTED;
                    res1 = eores_NOK_generic;                    
                }
                else
                {
                    // convert to iCubDegrees
                    *primary_value = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, this_jomoconfig.primary_encoder_place);
                    
                    if(0 != val_raw)
                    {   // marco.accame: boh ... e non si mette NOK_generic in caso contrario ??? per ora non lo cambio.
                        res1 = eores_OK;
                    }                        
                }
#else                
//                #if defined(USE_MC4PLUS)
//                //get the voltage from the motor port (0 - 3300mV)
//                val_raw = fake_hal_adc_get_hall_sensor_analog_input_mV(this_jomoconfig.primary_encoder_port);
//                
//                //convert to iCubDegrees
//                *primary_value = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, this_jomoconfig.primary_encoder_place);
//                
//                if(val_raw != 0)
//                    res1 = eores_OK;
//                #elif defined (USE_EMS4RD)
//                *primary_value = ENCODER_VALUE_NOT_SUPPORTED;
//                res1 = eores_NOK_generic;
//                #endif
#endif                
               
            } break;
                
            default:
            {
                *primary_value = ENCODER_VALUE_NOT_SUPPORTED;
                
            } break;
            
            // should handle the other cases...
        }	
    }
    else
    {
        *primary_value = ENCODER_VALUE_NOT_SUPPORTED;
    }
    
    // check existence for extra encoder
    if (CHECK_ENC_IS_CONNECTED(this_jomoconfig.secondary_encoder_type))
    {
        switch (this_jomoconfig.secondary_encoder_type)
        {
            case eo_appEncReader_enc_type_AEA:
            {
                EOappEncReader_configEncSPIXReadSequence_hid_t read_stream; 
                if (p->spimap->usedstream[this_jomoconfig.secondary_encoder_port] == eo_appEncReader_stream0)
                {
                    read_stream = p->configuredEnc_SPI_stream0.readSeq;
                }
                else if (p->spimap->usedstream[this_jomoconfig.secondary_encoder_port] == eo_appEncReader_stream1)
                {
                    read_stream = p->configuredEnc_SPI_stream1.readSeq;
                }
                else
                {
                    return eores_NOK_generic;
                }

                res2 = (eOresult_t)hal_encoder_get_value((hal_encoder_t)read_stream.list[p->spimap->indexinstream[this_jomoconfig.secondary_encoder_port]], &val_raw, flags);
                
                if(eores_OK != res2)
                {
                    *extra_value = (uint32_t)err_onReadFromSpi;
//                    p->dgninfo.enclist[jomo][err_onReadFromSpi]++;
                    return((eOresult_t)res2);
                }

                if (eobool_false == s_eo_appEncReader_IsValidValue_AEA(&val_raw, &errortype))
                {
                    *extra_value = (eOappEncReader_errortype_t)errortype;  
//                    p->dgninfo.enclist[jomo][errortype]++;
                    flags->data_error = 1;
                    return(eores_NOK_generic);
                }

                //val_raw >>= 6;
                //*value = (val_raw & 0x0FFF);
                //*value <<= 4; // 65536 ticks/revolution normalization;
                //val_raw = (val_raw>>2) & 0xFFF0;
                //*extra_value = RESCALE_IN_ICUB_DEGREES(val_raw, encoders_fullscales[eo_appEncReader_enc_type_AEA]);
                val_raw = (val_raw >> 6) & 0x0FFF;
                *extra_value = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, this_jomoconfig.secondary_encoder_place);  
                
            } break;  			

            case eo_appEncReader_enc_type_AMO:
            {
                EOappEncReader_configEncSPIXReadSequence_hid_t read_stream; 
                if (p->spimap->usedstream[this_jomoconfig.secondary_encoder_port] == eo_appEncReader_stream0)
                {
                    read_stream = p->configuredEnc_SPI_stream0.readSeq;
                }
                else if (p->spimap->usedstream[this_jomoconfig.secondary_encoder_port] == eo_appEncReader_stream1)
                {
                    read_stream = p->configuredEnc_SPI_stream1.readSeq;
                }
                else
                {
                    return eores_NOK_generic;
                }
                    
                res2 = (eOresult_t)hal_encoder_get_value((hal_encoder_t)read_stream.list[p->spimap->indexinstream[this_jomoconfig.secondary_encoder_port]], &val_raw, flags);
            
                if(eores_OK != res2)
                {
                    *extra_value = (uint32_t)err_onReadFromSpi;
//                    p->dgninfo.enclist[jomo][err_onReadFromSpi]++;
                    return((eOresult_t)res2);
                }
            
                val_raw = (val_raw>>4) & 0xFFFF;
                //*extra_value = RESCALE_IN_ICUB_DEGREES(val_raw, encoders_fullscales[eo_appEncReader_enc_type_AMO]);
                *extra_value = s_eo_appEncReader_rescale2icubdegrees( val_raw, jomo, this_jomoconfig.secondary_encoder_place);
               
            } break;
            
            case eo_appEncReader_enc_type_INC:
            {
#if 1                
                val_raw = fake_hal_quad_enc_getCounter(this_jomoconfig.secondary_encoder_port);
                if(ENCODER_VALUE_NOT_SUPPORTED == val_raw)
                {
                    *extra_value = 0;
                }
                else
                {
                    *extra_value = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, this_jomoconfig.secondary_encoder_place);
                }                
#else                                
//                #if defined(EOAPPENCODERREADER_DONTUSE_INC)
//                *extra_value = 0;
//                #else
//                val_raw = fake_hal_quad_enc_getCounter(this_jomoconfig.secondary_encoder_port);
//                *extra_value = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, this_jomoconfig.secondary_encoder_place);
//                #endif
//                res2 = eores_OK;
#endif                
                
            } break;
            
            case eo_appEncReader_enc_type_ADH:
            {
                // get the voltage from the motor port (0 - 3300mV)
#if 1                
                val_raw = fake_hal_adc_get_hall_sensor_analog_input_mV(this_jomoconfig.secondary_encoder_port);        
                if(ENCODER_VALUE_NOT_SUPPORTED == val_raw)
                {
                    *extra_value = ENCODER_VALUE_NOT_SUPPORTED;
                    res2 = eores_NOK_generic;                    
                }
                else
                {
                    // convert to iCubDegrees
                    *extra_value = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, this_jomoconfig.secondary_encoder_place);
                    
                    if(0 != val_raw)
                    {   // marco.accame: boh ... e non si mette NOK_generic in caso contrario ??? per ora non lo cambio.
                        res2 = eores_OK;
                    }                        
                } 
#else                
//                #if     defined(USE_MC4PLUS)
//                 //get the voltage from the motor port (0 - 3300mV)
//                val_raw = fake_hal_adc_get_hall_sensor_analog_input_mV(this_jomoconfig.secondary_encoder_port);
//                
//                //convert to iCubDegrees
//                *extra_value = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, this_jomoconfig.secondary_encoder_place);
//                
//                if(val_raw != 0)
//                    res2 = eores_OK;
//                #elif defined (USE_EMS4RD)
//                *extra_value = ENCODER_VALUE_NOT_SUPPORTED;
//                res2 = eores_NOK_generic;
//                #endif
#endif                
                
            } break;
            
            default:
            {
                *extra_value = ENCODER_VALUE_NOT_SUPPORTED;                
            } break;
            
            // should handle the other cases...
        }	
    }
    else
    {
        *extra_value = ENCODER_VALUE_NOT_SUPPORTED;
    }
    
    //return value based only on the reading of the primary encoder
    return res1;
}

//extern eOresult_t  eo_appEncReader_GetValue(EOappEncReader *p, eOappEncReader_encoder_t enc, uint32_t *value, hal_encoder_errors_flags *flags)
//{
//    
//    if(NULL == p)
//    {
//        return(eores_NOK_nullpointer);
//    }
//    
//    uint32_t val_raw;
//    eOresult_t res = eores_NOK_generic;
//    eOappEncReader_errortype_t errortype;
//        
//    //work-around (fully working) for backward compatibility:
//    //if enc is = 0,2,4 --> stream0
//    //if enc is = 1,3,5 --> stream1
//    // the rule is valid for both EMS & MC4plus
//    
//    eo_appEncReader_stream_number_t current_stream;

//#if     defined(USE_MC4PLUS)    
//    if (enc % 2 == 0)
//    {
//        current_stream = eo_appEncReader_stream0;
//    }
//    else
//    {
//        current_stream = eo_appEncReader_stream1;
//    }
//#elif   defined(USE_EMS4RD)
//    if (enc <= 2)
//    {
//        current_stream = eo_appEncReader_stream0;
//    }
//    else
//    {
//        current_stream = eo_appEncReader_stream1;
//    }
//#else
//    #error use either USE_MC4PLUS or USE_EMS4RD
//#endif    
//    //If AEA encoder
//    if(p->config.SPI_streams[current_stream].type == hal_encoder_t1)
//    {
//        res = (eOresult_t)hal_encoder_get_value((hal_encoder_t)enc, &val_raw, flags);
//        if(eores_OK != res)
//        {
//                //*value = (uint32_t)err_onReadFromSpi;
//                p->dgninfo.enclist[enc][err_onReadFromSpi]++;
//                //return(res);
//        }

//        if (eobool_false == s_eo_appEncReader_IsValidValue_AEA(&val_raw, &errortype))
//        {
//                //*value = (eOappEncReader_errortype_t)errortype;  
//                p->dgninfo.enclist[enc][errortype]++;
//                flags->data_error = 1;
//                //return(eores_NOK_generic);
//        }

//        //val_raw >>= 6;
//        //*value = (val_raw & 0x0FFF);
//        //*value <<= 4; // 65536 ticks/revolution normalization;
//        *value = (val_raw>>2) & 0xFFF0;
//    }			

//    //If AMO encoder
//    if(p->config.SPI_streams[current_stream].type == hal_encoder_t2)
//    {
//        res = (eOresult_t)hal_encoder_get_value((hal_encoder_t)enc, &val_raw, flags);
//        if(eores_OK != res)
//        {
//                //*value = (uint32_t)err_onReadFromSpi;
//                p->dgninfo.enclist[enc][err_onReadFromSpi]++;
//                //return(res);
//        }
//        
//        *value = (val_raw>>4) & 0xFFFF;
//    }
//		
//    return((eOresult_t)res);
//}

//extern uint64_t eo_appEncReader_startSPI_stream0(EOappEncReader *p)
//{
//    if(NULL == p)
//    {
//        return(0);
//    }
//    
//    return p->times[0][0];
//}

//extern uint64_t eo_appEncReader_startSPI_stream1(EOappEncReader *p)
//{
//    if(NULL == p)
//    {
//        return(0);
//    }
//    return p->times[1][0];
//}

//extern uint32_t eo_appEncReader_deltaSPI_stream0(EOappEncReader *p)
//{
//    if(NULL == p)
//    {
//        return(0);
//    }
//    uint64_t d = p->times[0][3] - p->times[0][0];
//    return((uint32_t)d);
//}

//extern uint32_t eo_appEncReader_deltaSPI_stream1(EOappEncReader *p)
//{
//    if(NULL == p)
//    {
//        return(0);
//    }
//    uint64_t d = p->times[1][3] - p->times[1][0];
//    return((uint32_t)d);    
//}

extern eObool_t eo_appEncReader_isReady(EOappEncReader *p)
{
    if(NULL == p)
    {
        return(eobool_true);
    }
    // no SPI encoders
    if((p->configuredEnc_SPI_stream0.readSeq.first == ENCODER_NULL) && (p->configuredEnc_SPI_stream1.readSeq.first == ENCODER_NULL))
    {
        return eobool_true;
    }
    // both streams has SPI encoders
    else if((p->configuredEnc_SPI_stream0.readSeq.first != ENCODER_NULL) && (p->configuredEnc_SPI_stream1.readSeq.first != ENCODER_NULL))
    {
        if((eOEncReader_readSt__finished == p->configuredEnc_SPI_stream1.st) && (eOEncReader_readSt__finished == p->configuredEnc_SPI_stream0.st))
		{
            return(eobool_true);
		}
		return(eobool_false);
	}
    // SPI encoders only on stream1
    else if (p->configuredEnc_SPI_stream0.readSeq.first == ENCODER_NULL)
    {
		return eo_appEncReader_isReadySPI_stream1(p);
	}
    // SPI encoders only on stream0
	else if (p->configuredEnc_SPI_stream1.readSeq.first == ENCODER_NULL)
	{
		return eo_appEncReader_isReadySPI_stream0(p);
	}
	return(eobool_false);
}

__inline extern eObool_t eo_appEncReader_isReadySPI_stream0(EOappEncReader *p)
{
    if(NULL == p)
    {
        return(eobool_true);
    }
    if((eOEncReader_readSt__finished == p->configuredEnc_SPI_stream0.st))
    {
        return(eobool_true);
    }
    return(eobool_false);
}

__inline extern eObool_t eo_appEncReader_isReadySPI_stream1(EOappEncReader *p)
{
    if(NULL == p)
    {
        return(eobool_false);
    }
    if((eOEncReader_readSt__finished == p->configuredEnc_SPI_stream1.st))
    {
        return(eobool_true);
    }
    return(eobool_false);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

#warning TODO: develop proper hal functions to change these....
// later on ... remove the USE_MC4PLUS macro and develop proper hal functions ... this code must be independent from board
#if defined(USE_MC4PLUS)
#include "hal_quad_enc.h"
#include "hal_adc.h"
#endif

extern const hal_encoder_spimapping_t * fake_hal_encoder_spimapping_get(void)
{
#if defined(USE_MC4PLUS)

    static const hal_encoder_spimapping_t s_hal_encoder_spi_mapping = 
    {
        .numberofspiencoders        = 2,
        .numberofspiencodersStream0 = 1,
        .numberofspiencodersStream1 = 1,
        .maskofsupported            = {1, 1, 0, 0, 0, 0},
        .usedstream                 = {hal_encoder_spistream0, hal_encoder_spistream1, eo_appEncReader_streamNONE, eo_appEncReader_streamNONE, eo_appEncReader_streamNONE, eo_appEncReader_streamNONE},
        //.usedstream ... as found in old code                = {hal_encoder_spistream0, hal_encoder_spistream1, hal_encoder_spistream0, hal_encoder_spistream1, hal_encoder_spistream0, hal_encoder_spistream1},
        .indexinstream              = {0, 0, 255, 255, 255, 255}
        //.indexinstream ... as found in old code              = {0, 0, 1, 1, 2, 2}        
    };


    // SPI Encoders MAP
    //static const eo_appEncReader_stream_number_t SPIencodersMap[eo_appEncReader_encoder_position_numberof] =
    //{
    //    /* 0 */     eo_appEncReader_stream0,
    //    /* 1 */     eo_appEncReader_stream1,
    //    /* 2 */     eo_appEncReader_stream0,
    //    /* 3 */     eo_appEncReader_stream1,
    //    /* 4 */     eo_appEncReader_stream0,
    //    /* 5 */     eo_appEncReader_stream1
    //};

    //// SPI Stream Positioning MAP
    //static const eo_appEncReader_stream_position_t SPIstreams_positioning[eo_appEncReader_encoder_position_numberof] =
    //{
    //    /* 0 */     eo_appEncReader_stream_position0,
    //    /* 1 */     eo_appEncReader_stream_position0,
    //    /* 2 */     eo_appEncReader_stream_position1,
    //    /* 3 */     eo_appEncReader_stream_position1,
    //    /* 4 */     eo_appEncReader_stream_position2,
    //    /* 5 */     eo_appEncReader_stream_position2
    //};
    //static const uint8_t numOfSupportedSPIencoders = 2;
    //static const uint8_t maskOfSupportedSPIencoders[hal_encoders_number] = {1, 1, 0, 0, 0, 0};

#elif defined(USE_EMS4RD)

    static const hal_encoder_spimapping_t s_hal_encoder_spi_mapping = 
    {
        .numberofspiencoders        = 6,
        .numberofspiencodersStream0 = 3,
        .numberofspiencodersStream1 = 3,
        .maskofsupported            = {1, 1, 1, 1, 1, 1},
        .usedstream                 = {hal_encoder_spistream0, hal_encoder_spistream0, hal_encoder_spistream0, hal_encoder_spistream1, hal_encoder_spistream1, hal_encoder_spistream1},
        .indexinstream              = {0, 1, 2, 0, 1, 2}   
    };

    //// SPI Encoders MAP
    //static const eo_appEncReader_stream_number_t SPIencodersMap[eo_appEncReader_encoder_position_maxnumberof] =
    //{
    //    /* 0 */     eo_appEncReader_stream0,
    //    /* 1 */     eo_appEncReader_stream0,
    //    /* 2 */     eo_appEncReader_stream0,
    //    /* 3 */     eo_appEncReader_stream1,
    //    /* 4 */     eo_appEncReader_stream1,
    //    /* 5 */     eo_appEncReader_stream1
    //};


    //// SPI Stream Positioning MAP
    //static const eo_appEncReader_stream_position_t SPIstreams_positioning[eo_appEncReader_encoder_position_maxnumberof] =
    //{
    //    /* 0 */     eo_appEncReader_stream_position0,
    //    /* 1 */     eo_appEncReader_stream_position1,
    //    /* 2 */     eo_appEncReader_stream_position2,
    //    /* 3 */     eo_appEncReader_stream_position0,
    //    /* 4 */     eo_appEncReader_stream_position1,
    //    /* 5 */     eo_appEncReader_stream_position2
    //};

    //static const uint8_t numOfSupportedSPIencoders = 6;
    //static const uint8_t maskOfSupportedSPIencoders[hal_encoders_number] = {1, 1, 1, 1, 1, 1};

#else
    #error -> either USE_EMS4RD or USE_MC4PLUS
#endif
    
    return(&s_hal_encoder_spi_mapping);
}


extern void fake_hal_quad_enc_single_init(uint8_t port)
{   // use only if mc4plus and the macro EOAPPENCODERREADER_DONTUSE_INC is not define
#if defined(USE_MC4PLUS) && !defined(EOAPPENCODERREADER_DONTUSE_INC)
    hal_quad_enc_single_init(port);   
#else
    // do nothing
#endif
}

extern uint32_t fake_hal_quad_enc_getCounter(uint8_t port)
{
#if defined(USE_MC4PLUS) && !defined(EOAPPENCODERREADER_DONTUSE_INC)
    return(hal_quad_enc_getCounter(port));   
#else
    return(ENCODER_VALUE_NOT_SUPPORTED);
#endif    
}

extern uint32_t fake_hal_adc_get_hall_sensor_analog_input_mV(uint8_t port)
{
#if defined(USE_MC4PLUS)
    return(hal_adc_get_hall_sensor_analog_input_mV(port));   
#else
    return(ENCODER_VALUE_NOT_SUPPORTED);
#endif   
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eo_clear_SPI_streams(EOappEncReader *p)
{
    uint8_t i = 0;
    for(i=0; i<eo_appEncReader_streams_numberof; i++)
    {    
        p->SPI_streams[i].numberof = 0;
        p->SPI_streams[i].type = hal_encoder_typeNONE;
    }        
}

static eObool_t s_eo_prepare_SPI_streams(EOappEncReader *p)
{           
    uint8_t i = 0;
    
    s_eo_clear_SPI_streams(p);
    
    uint8_t numberofSPIbased = 0;
    eObool_t portError = eobool_false;
    uint8_t spiports[hal_encoders_number] = {0};
    
    for(i=0; i<p->config.numofjomos; i++)
    {
        const eOappEncReader_jomoconfig_t *jdes = &p->config.jomoconfig[i];
        
        // if in the joint i-th we have any spi-based encoder which is primary or secondary, then we put it into the proper spi-stream

        if(CHECK_ENC_IS_ON_SPI(jdes->primary_encoder_type))
        //if((eo_appEncReader_enc_type_AEA == jdes->primary_encoder_type) || (eo_appEncReader_enc_type_AMO == jdes->primary_encoder_type))
        {   
            if((eo_appEncReader_encoder_portNONE != jdes->primary_encoder_port) && (0 == spiports[jdes->primary_encoder_port]))
            {
                spiports[jdes->primary_encoder_port] = 1;
            }
            else
            {
                portError = eobool_true;
                //return(portError);
            }
            
            eo_appEncReader_stream_number_t streamnumber = s_eo_appEncReader_get_spi_stream(p, jdes->primary_encoder_port);
            if(eo_appEncReader_streamNONE != streamnumber)
            {
                numberofSPIbased++;
                p->SPI_streams[streamnumber].numberof++;
                p->SPI_streams[streamnumber].type = hal_encoder_typeAEA; // TODO ... dovrei mettere AEA oppure AMO e poi non posso mischiare nello stesso stream un AMO ed un AEA
            }
            else
            {
                //return(portError);               
            }
        }

        if(CHECK_ENC_IS_ON_SPI(jdes->secondary_encoder_type))
        //if((eo_appEncReader_enc_type_AEA == jdes->secondary_encoder_type) || (eo_appEncReader_enc_type_AMO == jdes->secondary_encoder_type))
        {   
            if((eo_appEncReader_encoder_portNONE != jdes->secondary_encoder_port) && (0 == spiports[jdes->secondary_encoder_port]))
            {
                spiports[jdes->secondary_encoder_port] = 1;
            }
            else
            {
                portError = eobool_true;
                //return(portError);
            }
            
            eo_appEncReader_stream_number_t streamnumber = s_eo_appEncReader_get_spi_stream(p, jdes->secondary_encoder_port);
            if(eo_appEncReader_streamNONE != streamnumber)
            {
                numberofSPIbased++;
                p->SPI_streams[streamnumber].numberof++;
                p->SPI_streams[streamnumber].type = hal_encoder_typeAEA; // TODO ... dovrei mettere AEA oppure AMO e poi non posso mischiare nello stesso stream un AMO ed un AEA
            }
            else
            {
                //return(portError);               
            }
        }         

    } 

    // TODO: we should check that numberofSPIbased is not bigger than 6 for ems or 2 for mc4plus ......
    if(numberofSPIbased > p->spimap->numberofspiencoders)
    {
        // put diagnostics ...
        return(eobool_false);
    }
    
    return(eobool_true);    
}

static void s_eo_deconfigure_SPI_encoders(EOappEncReader *p)
{
    #warning TODO: make some tests about initting and deinitting hal etc.
        
    // deinit connected spi encoders
    s_eo_appEncReader_deinit_SPIencoders(p, &(p->configuredEnc_SPI_stream1), eo_appEncReader_stream1);    
    s_eo_appEncReader_deinit_SPIencoders(p, &(p->configuredEnc_SPI_stream0), eo_appEncReader_stream0);
    
    // clear list of the two spi streams
    s_eo_appEncReader_deinit_ListOfSPIencoders(p, &(p->configuredEnc_SPI_stream0), eo_appEncReader_stream0);
    s_eo_appEncReader_deinit_ListOfSPIencoders(p, &(p->configuredEnc_SPI_stream1), eo_appEncReader_stream1);

    // clear the streams
    s_eo_clear_SPI_streams(p);    
}

static eObool_t s_eo_configure_SPI_encoders(EOappEncReader *p)
{   
    // 1. prepare spi streams
    if(eobool_false == s_eo_prepare_SPI_streams(p))
    {
        return(eobool_false);
    }   
        
    // fill the list of the two SPI streams depending on the spi encoders which we want to manage   
    s_eo_appEncReader_init_ListOfSPIencoders(p, &(p->configuredEnc_SPI_stream0), eo_appEncReader_stream0);
    s_eo_appEncReader_init_ListOfSPIencoders(p, &(p->configuredEnc_SPI_stream1), eo_appEncReader_stream1);

    // init the spi encoders
    s_eo_appEncReader_init_SPIencoders(p, &(p->configuredEnc_SPI_stream0), eo_appEncReader_stream0);
    s_eo_appEncReader_init_SPIencoders(p, &(p->configuredEnc_SPI_stream1), eo_appEncReader_stream1);
    
    
    return(eobool_true);
}


static void s_eo_appEncReader_deinit_ListOfSPIencoders(EOappEncReader *p,  EOappEncReader_confEncDataPerSPI_hid_t *cfgSPIX, eo_appEncReader_stream_number_t stream_number)
{
    uint8_t i = 0;
    
    cfgSPIX->st = eOEncReader_readSt__idle;
    cfgSPIX->enc_type = eo_appEncReader_enc_type_NONE;
    cfgSPIX->enc_numbers = 0;
    cfgSPIX->enc_number_supported = 0;
 
    cfgSPIX->readSeq.first     = ENCODER_NULL;    
    for(i=0; i<(eo_appEncReader_stream_position_numberof+1); i++)
    {
        cfgSPIX->readSeq.list[i] = ENCODER_NULL; // doing also in last pos (i=3) is dummy but useful because it signals the last encoder
    }
}

static void s_eo_appEncReader_init_ListOfSPIencoders(EOappEncReader *p,  EOappEncReader_confEncDataPerSPI_hid_t *cfgSPIX, eo_appEncReader_stream_number_t stream_number)
{
    // deinit so that we start from a clean situation
    s_eo_appEncReader_deinit_ListOfSPIencoders(p, cfgSPIX, stream_number);
    
    // start assigning proper values
    cfgSPIX->st = eOEncReader_readSt__idle;
    cfgSPIX->enc_type = (eo_appEncReader_encoder_type_t)p->SPI_streams[stream_number].type;
    cfgSPIX->enc_numbers = p->SPI_streams[stream_number].numberof;
    cfgSPIX->enc_number_supported = (eo_appEncReader_stream0 == stream_number) ? (p->spimap->numberofspiencodersStream0) : (p->spimap->numberofspiencodersStream1);
     
    uint8_t i = 0;
    // cycle for all the jomos to fill the sequence 
    for(i=0; i<eOappEncReader_jomos_maxnumberof; i++)
    {
        eOappEncReader_jomoconfig_t* jmcfg = &p->config.jomoconfig[i];
        
        //if the primary encoder is on SPI
        if (CHECK_ENC_IS_ON_SPI(jmcfg->primary_encoder_type))
        {
            // check if stream is correct, otherwise do nothing
            if (p->spimap->usedstream[jmcfg->primary_encoder_port] == stream_number)
            {
                if (cfgSPIX->readSeq.first == ENCODER_NULL)
                {
                    cfgSPIX->readSeq.first     = jmcfg->primary_encoder_port;
                    cfgSPIX->readSeq.list[0]   = jmcfg->primary_encoder_port;
                }
                // if out of bound, discard it
                else if (p->spimap->indexinstream[jmcfg->primary_encoder_port] < cfgSPIX->enc_number_supported)
                {
                    cfgSPIX->readSeq.list[p->spimap->indexinstream[jmcfg->primary_encoder_port]] = jmcfg->primary_encoder_port;
                }
            }
        }
        
        // if the extra encoder is on SPI
        if (CHECK_ENC_IS_ON_SPI(jmcfg->secondary_encoder_type))
        {
            // check if stream is correct, otherwise do nothing
            if (p->spimap->usedstream[jmcfg->secondary_encoder_port] == stream_number)
            {
                if (cfgSPIX->readSeq.first == ENCODER_NULL)
                {
                    cfgSPIX->readSeq.first     = jmcfg->secondary_encoder_port;
                    cfgSPIX->readSeq.list[0]   = jmcfg->secondary_encoder_port;
                }
                //if out of bound, discard it
                else if (p->spimap->indexinstream[jmcfg->secondary_encoder_port] < cfgSPIX->enc_number_supported)
                {
                    cfgSPIX->readSeq.list[p->spimap->indexinstream[jmcfg->secondary_encoder_port]] = jmcfg->secondary_encoder_port;
                }
            }
        }
    }
}


static void s_eo_appEncReader_deinit_SPIencoders(EOappEncReader *p, EOappEncReader_confEncDataPerSPI_hid_t *cfgSPIX, eo_appEncReader_stream_number_t stream_number)
{
    uint8_t i;

    for(i=0; i<cfgSPIX->enc_numbers; i++)
    {        
        // we simply deinit the hal encoder
        hal_encoder_deinit((hal_encoder_t)cfgSPIX->readSeq.list[i]);
    }
}



static void s_eo_appEncReader_init_SPIencoders(EOappEncReader *p, EOappEncReader_confEncDataPerSPI_hid_t *cfgSPIX, eo_appEncReader_stream_number_t stream_number)
{
    uint8_t i;
	hal_encoder_cfg_t enc_cfg = {0};
    
    for(i=0; i < cfgSPIX->enc_numbers; i++)
    {
        // we define the basic configuration
        
        if(cfgSPIX->enc_type == (eo_appEncReader_encoder_type_t)hal_encoder_typeAEA)
        {   // AEA encoders 
            enc_cfg.priority     	    = INTPRIO_SPI_ENCODERS;
            enc_cfg.callback_on_rx      = NULL;
            enc_cfg.arg                 = NULL;
            enc_cfg.type				= hal_encoder_t1;
            enc_cfg.reg_address	        = 0; // not meaningful
            enc_cfg.sdata_precheck      = hal_false;
        }      
        else if(cfgSPIX->enc_type == (eo_appEncReader_encoder_type_t)hal_encoder_t2)
        {   // AMO encoders
            enc_cfg.priority      	    = INTPRIO_SPI_ENCODERS;
            enc_cfg.callback_on_rx      = NULL;
            enc_cfg.arg                 = NULL;
            enc_cfg.type			    = hal_encoder_typeAMO;
            enc_cfg.reg_address		    = 0x77;
            enc_cfg.sdata_precheck	    = hal_false;
        }
        else
        {
            return; 
        }
        
        // now we now must define what callback and argument each encoder must have. 
        // the first encoder must activates the next, until reaching the last encoder which stops the chain and marks the reading DONE!
              
        if(ENCODER_NULL != cfgSPIX->readSeq.list[i+1])
        {   // normal init if it's not the last
            enc_cfg.callback_on_rx = s_eo_appEncReader_isrCbk_onEncRead;
            // the argument is the next encoder to be started
            enc_cfg.arg = &(cfgSPIX->readSeq.list[i+1]);
        }
        else
        {    //if it's the last
            if(stream_number == eo_appEncReader_stream0)
            {
                enc_cfg.callback_on_rx = s_eo_appEncReader_isrCbk_onLastEncRead_SPI_stream0;
            }
            else if(stream_number == eo_appEncReader_stream1)
            {
                enc_cfg.callback_on_rx = s_eo_appEncReader_isrCbk_onLastEncRead_SPI_stream1;
            }
            enc_cfg.arg = p;
        }
        
        // finally we init the hal encoder with the filled configuration
        hal_encoder_init((hal_encoder_t)cfgSPIX->readSeq.list[i], &enc_cfg);
    }
}


static void s_eo_appEncReader_isrCbk_onEncRead(void *arg)
{
    hal_encoder_read_start(*((hal_encoder_t*)(arg)));
}

static void s_eo_appEncReader_isrCbk_onLastEncRead_SPI_stream0(void *arg)
{
   EOappEncReader *p = (EOappEncReader *)arg;
    
   p->times[0][3] = osal_system_abstime_get(); 
 
   p->configuredEnc_SPI_stream0.st = eOEncReader_readSt__finished;
   //if the user has configured a callback and reading on spi stream1 are already finished ==> then invoke it
   if (NULL != p->config.SPI_callbackOnLastRead)
   {
       if((eOEncReader_readSt__finished == p->configuredEnc_SPI_stream1.st) || (eOEncReader_readSt__idle == p->configuredEnc_SPI_stream1.st)) 
       {
            p->config.SPI_callbackOnLastRead(p->config.SPI_callback_arg);
       }
   }
}

static void s_eo_appEncReader_isrCbk_onLastEncRead_SPI_stream1(void *arg)
{
   EOappEncReader *p = (EOappEncReader *)arg;
    
   p->times[1][3] = osal_system_abstime_get();

   //set status of reding on spi stream1
   p->configuredEnc_SPI_stream1.st = eOEncReader_readSt__finished;

   //if the user has configured a callback and reading on spi stream0 are already finished ==> then invoke it
   if (NULL != p->config.SPI_callbackOnLastRead)
   {
       if((eOEncReader_readSt__finished == p->configuredEnc_SPI_stream0.st) || (eOEncReader_readSt__idle == p->configuredEnc_SPI_stream0.st)) 
       {
            p->config.SPI_callbackOnLastRead(p->config.SPI_callback_arg);
       }
   }
}

//return 0 in case of error else 1
static eObool_t s_eo_appEncReader_IsValidValue_AEA(uint32_t *valueraw, eOappEncReader_errortype_t *error)
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
}

static void s_eo_appEncReader_deconfigure_NONSPI_encoders(EOappEncReader *p)
{
    // marco.accame on 14 jan 2016: boh .. for now i dont deinit anything. i must check if the hal has proper deinit() methods for quad enc and others   
}

static void s_eo_appEncReader_configure_NONSPI_encoders(EOappEncReader *p)
{    
    uint8_t i = 0;
    
    for(i=0; i< eOappEncReader_jomos_maxnumberof; i++)
    {
        eOappEncReader_jomoconfig_t* jmcfg = &p->config.jomoconfig[i];
        if(jmcfg->primary_encoder_type == eo_appEncReader_enc_type_INC)
        {
            fake_hal_quad_enc_single_init(jmcfg->primary_encoder_port);
        }
        if(jmcfg->secondary_encoder_type == eo_appEncReader_enc_type_INC)
        {
            fake_hal_quad_enc_single_init(jmcfg->secondary_encoder_port);
        }
        // handle other cases...
        /*
        else if
        {
            
        }
        */
    }
}


#undef TEST_THE_RESCALE

static uint32_t s_eo_appEncReader_rescale2icubdegrees(uint32_t val_raw, uint8_t jomo, eo_appEncReader_encoder_place_t pos_type)
{
#if defined(TEST_THE_RESCALE)

// define/undef macros used for test in here
#define DONT_RESCALE

#if defined(DONT_RESCALE)
    //return(val_raw);
    const float tacchePERgrado = 293.333f;
    const float icubdegreePERgrado = 182.044;
    float tmp = (float)val_raw;
    tmp = tmp / tacchePERgrado;
    
    tmp *= icubdegreePERgrado;
    
    const float factorFROMtaccheTOicubdegree = 0.6206; // [icubdeg/tacche]
    
    const float factorFROMicubdegreeTOtacche = 1.611; // [tacche/icubdegree]
    
    tmp = val_raw * factorFROMtaccheTOicubdegree;
    
    return(tmp);
    
#elif defined(OLD_RESCALE)  
    return(RESCALE_IN_ICUB_DEGREES(val_raw, encoders_fullscales[eo_appEncReader_enc_type_INC]));
#endif    

#else

    // this is the correct code: we divide by the encoderconversionfactor ...
    // formulas are:
    // in xml file there is GENERAL:Encoders = td expressed in [ticks/deg]
    // robotInterface sets joint->config.encoderconversionfactor = (td / idegconv), expressed in [ticks/icubdeg]
    // idegconv = 182.044 = (64*1024/360) is the conversion from degrees to icubdeg and is expressed as [icubdeg/deg]
    // thus, to obtain the icub-degress in here we must divide the reading of the encoder expressed in [ticks] by
    // divider = joint->config.encoderconversionfactor.

    // this code does apply also to AEA encoders. 
    // so far the hal_encoder_get_value() has retrieved ticks with a resulution of 64K ticks / 360 degrees (the upscale from 16K to 64 is done internally)
    // in such a case the GENERAL:Encoders value in xml must be 182.044, so that divider = 1.
    // in cases where we want to apply a motor-reduction (e.g., in eyelids there is 100/42 between the aea and the joint) we may put it inside this parameter.
    
    // moreover .... if the encoderconversionfactor is negative, then i assume it is positive. because its sign is managed internally in the ems-controller


    uint32_t retval = val_raw;
    float divider = 1.0f;

    if (pos_type == eo_appEncReader_encoder_place_atjoint)
    {
        eOmc_joint_t *joint = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, jomo);
                
        if(NULL == joint)
        {
            return(2000);
        }
        
        /*divider = eo_common_Q17_14_to_float(joint->config.DEPRECATED_encoderconversionfactor); NO MORE NEEDED */
        divider = joint->config.jntEncoderResolution;
    }
    else if (pos_type == eo_appEncReader_encoder_place_atmotor)
    {
        eOmc_motor_t *motor = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, jomo);
                
        if(NULL == motor)
        {
            return(2000);
        }
        
        divider = motor->config.rotorEncoderResolution;
    }
    
    //check divider validity
    if(0.0f == divider)
    {
        return(3000);       
    }
    
    if(divider < 0)
    {
        divider = -divider;
    }

    retval = (float)val_raw * 65535.0f / divider;
    
    return(retval);

#endif
}


static eo_appEncReader_stream_number_t s_eo_appEncReader_get_spi_stream(EOappEncReader* p, uint8_t port)
{
    if(eo_appEncReader_encoder_portNONE == port)
    {
        return(eo_appEncReader_streamNONE);
    }
    // eo_appEncReader_stream_position_numberof is 3, so that we can map eo_appEncReader_encoder_position[1, 2, 3, 4, 5, 6] into: eo_appEncReader_stream[0, 0, 0, 1, 1, 1]
    
    return((eo_appEncReader_stream_number_t)p->spimap->usedstream[port]);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




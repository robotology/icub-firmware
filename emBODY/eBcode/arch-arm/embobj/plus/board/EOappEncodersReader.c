/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Davide Pollarolo, marco accame
 * email:   valentina.gaggero@iit.it, davide.pollarolo@iit.it, marco.accame@iit.it
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
    @author    valentina.gaggero@iit.it, davide.pollarolo@iit.it, marco.accame@iit.it
    @date       02/17/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "hal_spiencoder.h"


#include "EOtheMemoryPool.h"

#include "OPCprotocolManager_Cfg.h" 

#include "EOemsControllerCfg.h"
#include "osal.h"

#include "EoProtocol.h"

#include "hal_quadencoder.h"
#include "hal_adc.h"


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

#define CHECK_ENC_IS_CONNECTED(joint_enc)           (joint_enc != eo_appEncReader_enc_type_NONE)
#define CHECK_ENC_IS_ON_STREAMED_SPI(joint_enc)     ((joint_enc == eo_appEncReader_enc_type_AEA) || (joint_enc == eo_appEncReader_enc_type_AMO))
#define CHECK_ENC_IS_CHAINED_SPI(joint_enc)         ((joint_enc == eo_appEncReader_enc_type_SPICHAINOF2))


#define ENCODER_VALUE_NOT_SUPPORTED                 hal_NA32


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

static void s_eo_clear_SPI_chains(EOappEncReader *p);
static eObool_t s_eo_prepare_SPI_chains(EOappEncReader *p);

static void s_eo_deconfigure_SPI_encoders(EOappEncReader *p);
static eObool_t s_eo_configure_SPI_encoders(EOappEncReader *p);

static void s_eo_appEncReader_isrCbk_onEncRead(void *arg);
static void s_eo_appEncReader_isrCbk_onLastEncRead_SPI_stream0(void *arg);
static void s_eo_appEncReader_isrCbk_onLastEncRead_SPI_stream1(void *arg);

static void s_eo_appEncReader_deinit_ListOfSPIencoders(EOappEncReader *p,  EOappEncReader_SPIstream_config_t *cfgSPIX, hal_spiencoder_stream_t stream_number);
static void s_eo_appEncReader_init_ListOfSPIencoders(EOappEncReader *p, EOappEncReader_SPIstream_config_t *cfgSPIX, hal_spiencoder_stream_t stream_number);

static void s_eo_appEncReader_deinit_streamofSPIencoders(EOappEncReader *p, EOappEncReader_SPIstream_config_t *cfgSPIX, hal_spiencoder_stream_t stream_number);
static void s_eo_appEncReader_init_streamofSPIencoders(EOappEncReader *p, EOappEncReader_SPIstream_config_t *cfgSPIX, hal_spiencoder_stream_t stream_number);

static void s_eo_appEncReader_init_chainofSPIencoders(EOappEncReader *p);
static void s_eo_appEncReader_deinit_chainofSPIencoders(EOappEncReader *p);

static eObool_t s_eo_appEncReader_IsValidValue_AEA(uint32_t *valueraw, eOappEncReader_errortype_t *error);
static eObool_t s_eo_appEncReader_IsValidValue_SPICHAIN2(uint32_t *valueraw, eOappEncReader_errortype_t *error);

static void s_eo_appEncReader_deconfigure_NONSPI_encoders(EOappEncReader *p);
static void s_eo_appEncReader_configure_NONSPI_encoders(EOappEncReader *p);

static uint32_t s_eo_appEncReader_rescale2icubdegrees(uint32_t val_raw, uint8_t jomo, eo_appEncReader_encoder_place_t pos_type);


//static eObool_t s_eo_appEncReader_isReadySPI_stream0(EOappEncReader *p);
//static eObool_t s_eo_appEncReader_isReadySPI_stream1(EOappEncReader *p);


static hal_spiencoder_stream_t s_eo_appEncReader_get_spi_stream(EOappEncReader* p, uint8_t port);

static void s_fake_hal_quadencoder_single_init(uint8_t port);

static uint32_t s_fake_hal_quadencoder_getCounter(uint8_t port);

static uint32_t s_fake_hal_adc_get_hall_sensor_analog_input_mV(uint8_t port);

static void s_eo_appEncReader_spichain_reading_is_finished(void* p);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static EOappEncReader s_eo_theappencreader =
{
    .initted            = eobool_false,
    .active             = eobool_false,
    .totalnumberofencoders = 0,
    .stream_map         = NULL,
    .config             = {0},
    .SPI_streams        = {{.type = hal_spiencoder_typeNONE, .numberof = 0, .isacquiring = eobool_false}},
    .configofSPIstream0 = {.spiStatus = encreader_spistatus_idle, .encType = eo_appEncReader_enc_type_NONE, .numberofencoders = 0, .maxsupportedencoders = 0, .sequence = {hal_spiencoderNONE}},
    .configofSPIstream1 = {.spiStatus = encreader_spistatus_idle, .encType = eo_appEncReader_enc_type_NONE, .numberofencoders = 0, .maxsupportedencoders = 0, .sequence = {hal_spiencoderNONE}},
    .configofSPIChained = {{.spiStatus = encreader_spistatus_idle, .encType = eo_appEncReader_enc_type_NONE, .numberofencoders = 0, .maxsupportedencoders = 0, .singleport = hal_spiencoderNONE}},
    .encodertype        = {eo_appEncReader_enc_type_NONE},
    .times              = {0}   
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
    
    s_eo_theappencreader.totalnumberofencoders = 0;
    
    // step 1: retrieve the spi mapping from hal
    
    s_eo_theappencreader.stream_map = hal_spiencoder_stream_map_get();
    
    if(NULL == s_eo_theappencreader.stream_map)
    {
        // marco.accame: put a diagnostics message about the failure
        return(NULL);
    }   

    s_eo_clear_SPI_streams(&s_eo_theappencreader);   
    s_eo_clear_SPI_chains(&s_eo_theappencreader);   
    
    memset(s_eo_theappencreader.encodertype, eo_appEncReader_enc_type_NONE, sizeof(s_eo_theappencreader.encodertype));
    
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
    
    memset(s_eo_theappencreader.encodertype, eo_appEncReader_enc_type_NONE, sizeof(s_eo_theappencreader.encodertype));
    
    memset(&p->config, 0, sizeof(eOappEncReader_cfg_t));
    p->config.numofjomos = 0;
    
    s_eo_theappencreader.totalnumberofencoders = 0;
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


extern eOresult_t eo_appEncReader_StartRead(EOappEncReader *p)
{
    // i can start reading at most two spi encoders which are mapped into different streams
    // in case we did not have any chained spi encoders, it woul be enough to set two bits now (one for stream0 and one for stream1),
    // to make the last ISR of each stream reset it at end of teh chain of encoder acquistion.
    // in this mode, the check of eo_appEncReader_isReady() is easy: we just compare a mask to zero.
    // with the use of chained spi encoders, everything goes to which encoders we use. to make it quick, i use a dedicated mask of 8 bits.
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    uint8_t i = 0;

    // reset isacquiring status of the two spi streams 
    for(i=0; i<hal_spiencoder_streams_number; i++)
    {
        p->SPI_streams[i].isacquiring = eobool_false;
    }    

    // eval spistream 0: aea or amo
    p->configofSPIstream0.spiStatus = encreader_spistatus_idle;
    if(hal_spiencoderNONE != p->configofSPIstream0.sequence.first)
    { 
        memset(&p->times[0], 0, 4); // spi stream0
        p->times[0][0] = osal_system_abstime_get();
        p->configofSPIstream0.spiStatus = encreader_spistatus_reading;
        p->SPI_streams[hal_spiencoder_stream0].isacquiring = eobool_true;
        hal_spiencoder_read_start(p->configofSPIstream0.sequence.first);  
    }
    
    // eval spistream 1: aea or amo
    p->configofSPIstream1.spiStatus = encreader_spistatus_idle;
    if(hal_spiencoderNONE != p->configofSPIstream1.sequence.first)
    {   
        memset(&p->times[1], 0, 4); // spi stream1   
        p->times[1][0] = osal_system_abstime_get();
        p->SPI_streams[hal_spiencoder_stream1].isacquiring = eobool_true;        
        p->configofSPIstream1.spiStatus = encreader_spistatus_reading;
        hal_spiencoder_read_start(p->configofSPIstream1.sequence.first);  
    }
   
    
    // globally eval the two streams 
    //p->maskofacquiringspichained = 0;   
    for(i=0; i<hal_spiencoder_streams_number; i++)
    {
        p->configofSPIChained[i].spiStatus = encreader_spistatus_idle;
        
        if(eo_appEncReader_enc_type_SPICHAINOF2 == p->configofSPIChained[i].encType)
        {
            //eo_common_byte_bitset((uint8_t*)&p->maskofacquiringspichained, p->configofSPIChained[i].port); 
            p->SPI_streams[i].isacquiring = eobool_true; 
            p->configofSPIChained[i].spiStatus = encreader_spistatus_reading;
            hal_spiencoder_read_start(p->configofSPIChained[i].singleport);  
        }
    }
     
    return(eores_OK);
}



extern eOresult_t eo_appEncReader_GetValue(EOappEncReader *p, uint8_t jomo, uint32_t *primary_value, uint32_t *extra_value, hal_spiencoder_errors_flags *flags)
{    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    hal_spiencoder_position_t val_raw = 0; // marco.accame: it should be is a hal_spiencoder_position_t 
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
                EOappEncReader_streamedSPIsequence_t read_stream = {hal_spiencoderNONE}; 
                if (p->stream_map->encoder2stream[this_jomoconfig.primary_encoder_port] == hal_spiencoder_stream0)
                {   // ma come? si assegna usa struct ora? orrore.
                    read_stream = p->configofSPIstream0.sequence;
                }
                else if (p->stream_map->encoder2stream[this_jomoconfig.primary_encoder_port] == hal_spiencoder_stream1)
                {
                    read_stream = p->configofSPIstream1.sequence;
                }
                else
                {
                    return(eores_NOK_generic);
                }

                res1 = (eOresult_t)hal_spiencoder_get_value(read_stream.list[p->stream_map->encoder2indexinstream[this_jomoconfig.primary_encoder_port]], &val_raw, flags);
                
                if(eores_OK != res1)
                {
                    *primary_value = (uint32_t)err_onReadFromSpi;
                    return((eOresult_t)res1);
                }

                if(eobool_false == s_eo_appEncReader_IsValidValue_AEA(&val_raw, &errortype))
                {
                    *primary_value = (eOappEncReader_errortype_t)errortype;  
                    flags->data_error = 1;
                    return(eores_NOK_generic);
                }

                // marco.accame: hal_spiencoder_get_value() gives back a value in uint32_t with only 18 bits of information (internally masked with 0x03FFFF).
                // only the 12 most significant bits contain a position reading. to obtain the ticks we should do:
                // ticks = (val_raw >> 6) & 0x0FFF;
                // the resolution is now 4096 ticks per revolution.
                
                val_raw = (val_raw >> 6) & 0x0FFF;
                *primary_value = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, this_jomoconfig.primary_encoder_place);                
               
            } break; 			

            case eo_appEncReader_enc_type_AMO:
            {
                EOappEncReader_streamedSPIsequence_t read_stream = {hal_spiencoderNONE}; 
                if (p->stream_map->encoder2stream[this_jomoconfig.primary_encoder_port] == hal_spiencoder_stream0)
                {
                    read_stream = p->configofSPIstream0.sequence;
                }
                else if (p->stream_map->encoder2stream[this_jomoconfig.primary_encoder_port] == hal_spiencoder_stream1)
                {
                    read_stream = p->configofSPIstream1.sequence;
                }
                else
                {
                    return(eores_NOK_generic);
                }
                
                
                res1 = (eOresult_t)hal_spiencoder_get_value(read_stream.list[p->stream_map->encoder2indexinstream[this_jomoconfig.primary_encoder_port]], &val_raw, flags);
            
                if(eores_OK != res1)
                {
                    *primary_value = (uint32_t)err_onReadFromSpi;
                    return((eOresult_t)res1);
                }
            
                val_raw = (val_raw>>4) & 0xFFFF;
                *primary_value = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, this_jomoconfig.primary_encoder_place);
                
            } break;
            
            case eo_appEncReader_enc_type_SPICHAINOF2:
            {
                res1 = (eOresult_t)hal_spiencoder_get_value(this_jomoconfig.primary_encoder_port, &val_raw, flags);
                
                if(eores_OK != res1)
                {
                    *primary_value = (uint32_t)err_onReadFromSpi;
                    return((eOresult_t)res1);
                }

                if(eobool_false == s_eo_appEncReader_IsValidValue_SPICHAIN2(&val_raw, &errortype))
                {
                    *primary_value = (eOappEncReader_errortype_t)errortype;  
                    flags->data_error = 1;
                    return(eores_NOK_generic);
                }

                #warning TODO: rescaling of SPICHAIN2 must be defined
                val_raw = (val_raw >> 6) & 0x0FFF; 
                *primary_value = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, this_jomoconfig.primary_encoder_place);                
               
            } break; 	            
            
            case eo_appEncReader_enc_type_INC:
            {
                
                val_raw = s_fake_hal_quadencoder_getCounter(this_jomoconfig.primary_encoder_port);
                if(ENCODER_VALUE_NOT_SUPPORTED == val_raw)
                {
                    *primary_value = 0;
                }
                else
                {
                    *primary_value = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, this_jomoconfig.primary_encoder_place);
                }    
         
                res1 = eores_OK;
                
            } break;
            
            case eo_appEncReader_enc_type_ADH:
            {
                // get the voltage from the motor port (0 - 3300mV)
            
                val_raw = s_fake_hal_adc_get_hall_sensor_analog_input_mV(this_jomoconfig.primary_encoder_port);        
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
                EOappEncReader_streamedSPIsequence_t read_stream = {hal_spiencoderNONE}; 
                if (p->stream_map->encoder2stream[this_jomoconfig.secondary_encoder_port] == hal_spiencoder_stream0)
                {
                    read_stream = p->configofSPIstream0.sequence;
                }
                else if (p->stream_map->encoder2stream[this_jomoconfig.secondary_encoder_port] == hal_spiencoder_stream1)
                {
                    read_stream = p->configofSPIstream1.sequence;
                }
                else
                {
                    return eores_NOK_generic;
                }

                res2 = (eOresult_t)hal_spiencoder_get_value(read_stream.list[p->stream_map->encoder2indexinstream[this_jomoconfig.secondary_encoder_port]], &val_raw, flags);
                
                if(eores_OK != res2)
                {
                    *extra_value = (uint32_t)err_onReadFromSpi;
                    return((eOresult_t)res2);
                }

                if (eobool_false == s_eo_appEncReader_IsValidValue_AEA(&val_raw, &errortype))
                {
                    *extra_value = (eOappEncReader_errortype_t)errortype;  
                    flags->data_error = 1;
                    return(eores_NOK_generic);
                }

                val_raw = (val_raw >> 6) & 0x0FFF;
                *extra_value = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, this_jomoconfig.secondary_encoder_place);  
                
            } break;  			

            case eo_appEncReader_enc_type_AMO:
            {
                EOappEncReader_streamedSPIsequence_t read_stream = {hal_spiencoderNONE}; 
                if (p->stream_map->encoder2stream[this_jomoconfig.secondary_encoder_port] == hal_spiencoder_stream0)
                {
                    read_stream = p->configofSPIstream0.sequence;
                }
                else if (p->stream_map->encoder2stream[this_jomoconfig.secondary_encoder_port] == hal_spiencoder_stream1)
                {
                    read_stream = p->configofSPIstream1.sequence;
                }
                else
                {
                    return(eores_NOK_generic);
                }
                    
                res2 = (eOresult_t)hal_spiencoder_get_value(read_stream.list[p->stream_map->encoder2indexinstream[this_jomoconfig.secondary_encoder_port]], &val_raw, flags);
            
                if(eores_OK != res2)
                {
                    *extra_value = (uint32_t)err_onReadFromSpi;
                    return((eOresult_t)res2);
                }
            
                val_raw = (val_raw>>4) & 0xFFFF;
                *extra_value = s_eo_appEncReader_rescale2icubdegrees( val_raw, jomo, this_jomoconfig.secondary_encoder_place);
               
            } break;
            
            case eo_appEncReader_enc_type_SPICHAINOF2:
            {                    
                res2 = (eOresult_t)hal_spiencoder_get_value(this_jomoconfig.secondary_encoder_port, &val_raw, flags);
            
                if(eores_OK != res2)
                {
                    *extra_value = (uint32_t)err_onReadFromSpi;
                    return((eOresult_t)res2);
                }
            
                #warning TODO: rescaling of SPICHAIN2 must be defined
                val_raw = (val_raw>>4) & 0xFFFF;
                *extra_value = s_eo_appEncReader_rescale2icubdegrees( val_raw, jomo, this_jomoconfig.secondary_encoder_place);
 
            } break; 	            
            
            case eo_appEncReader_enc_type_INC:
            {
             
                val_raw = s_fake_hal_quadencoder_getCounter(this_jomoconfig.secondary_encoder_port);
                if(ENCODER_VALUE_NOT_SUPPORTED == val_raw)
                {
                    *extra_value = 0;
                }
                else
                {
                    *extra_value = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, this_jomoconfig.secondary_encoder_place);
                }                
              
                
            } break;
            
            case eo_appEncReader_enc_type_ADH:
            {
                // get the voltage from the motor port (0 - 3300mV)
           
                val_raw = s_fake_hal_adc_get_hall_sensor_analog_input_mV(this_jomoconfig.secondary_encoder_port);        
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
    
    // return value based only on the reading of the primary encoder
    return(res1);
}


//extern eObool_t eo_appEncReader_isReadyOLD(EOappEncReader *p)
//{
//    if(NULL == p)
//    {
//        return(eobool_true);
//    }
//    
//    // no SPI encoders
//    if((p->configofSPIstream0.sequence.first == hal_spiencoderNONE) && (p->configofSPIstream1.sequence.first == hal_spiencoderNONE))
//    {
//        return eobool_true;
//    }
//    // both streams has SPI encoders
//    else if((p->configofSPIstream0.sequence.first != hal_spiencoderNONE) && (p->configofSPIstream1.sequence.first != hal_spiencoderNONE))
//    {
//        if((encreader_spistatus_finished == p->configofSPIstream1.spiStatus) && (encreader_spistatus_finished == p->configofSPIstream0.spiStatus))
//		{
//            return(eobool_true);
//		}
//		return(eobool_false);
//	}
//    // SPI encoders only on stream1
//    else if (p->configofSPIstream0.sequence.first == hal_spiencoderNONE)
//    {
//		return s_eo_appEncReader_isReadySPI_stream1(p);
//	}
//    // SPI encoders only on stream0
//	else if (p->configofSPIstream1.sequence.first == hal_spiencoderNONE)
//	{
//		return s_eo_appEncReader_isReadySPI_stream0(p);
//	}
//	return(eobool_false);
//}


extern eObool_t eo_appEncReader_isReady(EOappEncReader *p)
{
    if(NULL == p)
    {
        return(eobool_true);
    }
    
    eObool_t ready = eobool_true;

//    if((0 != p->maskofacquiringspichained) || (encreader_spistatus_reading == p->configofSPIstream0.spiStatus) || (encreader_spistatus_reading == p->configofSPIstream1.spiStatus))
//    if((0 != p->maskofacquiringspichained) || (eobool_true == p->SPI_streams[hal_spiencoder_stream0].isacquiring) || (eobool_true == p->SPI_streams[hal_spiencoder_stream1].isacquiring))
    if((eobool_true == p->SPI_streams[hal_spiencoder_stream0].isacquiring) || (eobool_true == p->SPI_streams[hal_spiencoder_stream1].isacquiring))
    {
        ready = eobool_false;
    }


	return(ready);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

//static eObool_t s_eo_appEncReader_isReadySPI_stream0(EOappEncReader *p)
//{
//    if((encreader_spistatus_finished == p->configofSPIstream0.spiStatus))
//    {
//        return(eobool_true);
//    }
//    return(eobool_false);
//}

//static eObool_t s_eo_appEncReader_isReadySPI_stream1(EOappEncReader *p)
//{
//    if((encreader_spistatus_finished == p->configofSPIstream1.spiStatus))
//    {
//        return(eobool_true);
//    }
//    return(eobool_false);
//}

static void s_eo_clear_SPI_streams(EOappEncReader *p)
{
    uint8_t i = 0;
    for(i=0; i<hal_spiencoder_streams_number; i++)
    {    
        p->SPI_streams[i].numberof = 0;
        p->SPI_streams[i].type = hal_spiencoder_typeNONE;
        p->SPI_streams[i].isacquiring = eobool_false;
    }        
}


static eObool_t s_eo_prepare_SPI_streams(EOappEncReader *p)
{           
    uint8_t i = 0;
    
    s_eo_clear_SPI_streams(p);
    
    uint8_t numberofSPIbased = 0;
    eObool_t portError = eobool_false;
    uint8_t spiports[hal_spiencoders_number] = {0};
    
    for(i=0; i<p->config.numofjomos; i++)
    {
        const eOappEncReader_jomoconfig_t *jdes = &p->config.jomoconfig[i];
        
        // if in the joint i-th we have any spi-based encoder which is primary or secondary, then we put it into the proper spi-stream

        if(CHECK_ENC_IS_ON_STREAMED_SPI(jdes->primary_encoder_type))
        {   
            if((eo_appEncReader_encoder_portNONE != jdes->primary_encoder_port) && (0 == spiports[jdes->primary_encoder_port]))
            {
                spiports[jdes->primary_encoder_port] = 1;
            }
            else
            {
                portError = eobool_true;
                portError = portError;
                //return(portError);
            }
            
            hal_spiencoder_stream_t streamnumber = s_eo_appEncReader_get_spi_stream(p, jdes->primary_encoder_port);
            if(hal_spiencoder_streamNONE != streamnumber)
            {              
                p->encodertype[p->totalnumberofencoders++] = jdes->primary_encoder_type;
                
                numberofSPIbased++;
                p->SPI_streams[streamnumber].numberof++;
                p->SPI_streams[streamnumber].type = hal_spiencoder_typeAEA; // TODO ... dovrei mettere AEA oppure AMO e poi non posso mischiare nello stesso stream un AMO ed un AEA
            }
            else
            {
                //return(portError);               
            }
        }

        if(CHECK_ENC_IS_ON_STREAMED_SPI(jdes->secondary_encoder_type))
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
            
            hal_spiencoder_stream_t streamnumber = s_eo_appEncReader_get_spi_stream(p, jdes->secondary_encoder_port);
            if(hal_spiencoder_streamNONE != streamnumber)
            {
                p->encodertype[p->totalnumberofencoders++] = jdes->secondary_encoder_type;
                
                numberofSPIbased++;
                p->SPI_streams[streamnumber].numberof++;
                p->SPI_streams[streamnumber].type = hal_spiencoder_typeAEA; // TODO ... dovrei mettere AEA oppure AMO e poi non posso mischiare nello stesso stream un AMO ed un AEA
            }
            else
            {
                //return(portError);               
            }
        }         

    } 

    // TODO: we should check that numberofSPIbased is not bigger than 6 for ems or 2 for mc4plus ......
    if(numberofSPIbased > p->stream_map->totalnumberofencoders)
    {
        // put diagnostics ...
        return(eobool_false);
    }
    
    return(eobool_true);    
}


static void s_eo_clear_SPI_chains(EOappEncReader *p)
{
    uint8_t i = 0;
    for(i=0; i<hal_spiencoder_streams_number; i++)
    {    
        p->configofSPIChained[i].spiStatus = encreader_spistatus_idle;
        p->configofSPIChained[i].encType = eo_appEncReader_enc_type_NONE;
        p->configofSPIChained[i].numberofencoders = 0;
        p->configofSPIChained[i].maxsupportedencoders = 1; // one per stream so far
        p->configofSPIChained[i].singleport = hal_spiencoderNONE;
    }       
}

// so far we assume that the config is well formed: a chained is not mixed up with aea / amo encoder etc. .... that avoids doing complex controls ...
static eObool_t s_eo_prepare_SPI_chains(EOappEncReader *p)
{           
    uint8_t i = 0;
    
    s_eo_clear_SPI_chains(p);
        
    for(i=0; i<p->config.numofjomos; i++)
    {
        const eOappEncReader_jomoconfig_t *jdes = &p->config.jomoconfig[i];
        
        // if in the joint i-th we have any spi-encoder which is primary or secondary, then we assign it in the relevant port

        if(CHECK_ENC_IS_CHAINED_SPI(jdes->primary_encoder_type))
        {   
            if(eo_appEncReader_encoder_portNONE != jdes->primary_encoder_port)
            {
                p->encodertype[p->totalnumberofencoders++] = jdes->primary_encoder_type;
                
                hal_spiencoder_stream_t stream = p->stream_map->encoder2stream[jdes->primary_encoder_port];
                p->configofSPIChained[stream].spiStatus = encreader_spistatus_idle;
                p->configofSPIChained[stream].encType = jdes->primary_encoder_type;
                p->configofSPIChained[stream].singleport = (hal_spiencoder_t)jdes->primary_encoder_port;
                p->configofSPIChained[stream].numberofencoders++;
                if(p->configofSPIChained[stream].maxsupportedencoders != p->configofSPIChained[stream].numberofencoders)
                {
                    // error: we cannot have more than one per stream 
                //    return(eobool_false);                   
                }
            }
        }

        if(CHECK_ENC_IS_CHAINED_SPI(jdes->secondary_encoder_type))
        {   
            if(eo_appEncReader_encoder_portNONE != jdes->secondary_encoder_port) 
            {
                p->encodertype[p->totalnumberofencoders++] = jdes->secondary_encoder_type;
                                
                hal_spiencoder_stream_t stream = p->stream_map->encoder2stream[jdes->secondary_encoder_port];
                p->configofSPIChained[stream].spiStatus = encreader_spistatus_idle;
                p->configofSPIChained[stream].encType = jdes->primary_encoder_type;
                p->configofSPIChained[stream].singleport = (hal_spiencoder_t)jdes->primary_encoder_port;                
                p->configofSPIChained[stream].numberofencoders++;
                if(p->configofSPIChained[stream].maxsupportedencoders != p->configofSPIChained[stream].numberofencoders)
                {
                    // error: we cannot have more than one per stream 
                //    return(eobool_false);                   
                }                
            }
        }         

    } 

    return(eobool_true);    
}


static void s_eo_deconfigure_SPI_encoders(EOappEncReader *p)
{
    #warning TODO: make some tests about initting and deinitting hal etc.
        
    // 1.a deinit the stream spi encoders (aea /amao)
    s_eo_appEncReader_deinit_streamofSPIencoders(p, &(p->configofSPIstream1), hal_spiencoder_stream1);    
    s_eo_appEncReader_deinit_streamofSPIencoders(p, &(p->configofSPIstream0), hal_spiencoder_stream0);
    
    // 1.b clear list of the two spi streams
    s_eo_appEncReader_deinit_ListOfSPIencoders(p, &(p->configofSPIstream0), hal_spiencoder_stream0);
    s_eo_appEncReader_deinit_ListOfSPIencoders(p, &(p->configofSPIstream1), hal_spiencoder_stream1);

    // 1.c clear the streams
    s_eo_clear_SPI_streams(p);

    // 2.a deinit the chain encoders
    s_eo_appEncReader_deinit_chainofSPIencoders(p);
    // 2.b clear the chains
    s_eo_clear_SPI_chains(p);    
}

static eObool_t s_eo_configure_SPI_encoders(EOappEncReader *p)
{   
    // they are: 
    // 1. streamed encoders (aea or amo), for which we read one encoder then another on a stream of consecutitive encoders
    // 2. chained encoders (spichainof2, ...), for which we just do a single read from a port. 
    
    // 1.a prepare spi streams    
    if(eobool_false == s_eo_prepare_SPI_streams(p))
    {
        return(eobool_false);
    }   
        
    // 1.b fill the list of the two SPI streams depending on the spi encoders which we want to manage   
    s_eo_appEncReader_init_ListOfSPIencoders(p, &(p->configofSPIstream0), hal_spiencoder_stream0);
    s_eo_appEncReader_init_ListOfSPIencoders(p, &(p->configofSPIstream1), hal_spiencoder_stream1);
    
    // 1.c init streamed spi encoders
    s_eo_appEncReader_init_streamofSPIencoders(p, &(p->configofSPIstream0), hal_spiencoder_stream0);
    s_eo_appEncReader_init_streamofSPIencoders(p, &(p->configofSPIstream1), hal_spiencoder_stream1);    
    
    
    // 2.a prepare spi chains
    if(eobool_false == s_eo_prepare_SPI_chains(p))
    {
        return(eobool_false);
    }  

    // 2.b init the chained spi encoders
    s_eo_appEncReader_init_chainofSPIencoders(p);
    
    
    return(eobool_true);
}


static void s_eo_appEncReader_deinit_ListOfSPIencoders(EOappEncReader *p,  EOappEncReader_SPIstream_config_t *cfgSPIX, hal_spiencoder_stream_t stream_number)
{
    uint8_t i = 0;
    
    cfgSPIX->spiStatus = encreader_spistatus_idle;
    cfgSPIX->encType = eo_appEncReader_enc_type_NONE;
    cfgSPIX->numberofencoders = 0;
    cfgSPIX->maxsupportedencoders =  p->stream_map->stream2numberofencoders[stream_number];
 
    cfgSPIX->sequence.first = hal_spiencoderNONE;    
    for(i=0; i<(hal_spiencoder_maxnumber_in_stream+1); i++)
    {
        cfgSPIX->sequence.list[i] = hal_spiencoderNONE; // doing also in last pos (i=3) is dummy but useful because it signals the last encoder
    }
}


static void s_eo_appEncReader_init_ListOfSPIencoders(EOappEncReader *p,  EOappEncReader_SPIstream_config_t *cfgSPIX, hal_spiencoder_stream_t stream_number)
{
    // deinit so that we start from a clean situation
    s_eo_appEncReader_deinit_ListOfSPIencoders(p, cfgSPIX, stream_number);
    
    // start assigning proper values
    cfgSPIX->spiStatus = encreader_spistatus_idle;
    cfgSPIX->encType = (eo_appEncReader_encoder_type_t)p->SPI_streams[stream_number].type;
    cfgSPIX->numberofencoders = p->SPI_streams[stream_number].numberof;
    cfgSPIX->maxsupportedencoders = p->stream_map->stream2numberofencoders[stream_number];
         
    uint8_t i = 0;
    // cycle for all the jomos to fill the sequence 
    for(i=0; i<eOappEncReader_jomos_maxnumberof; i++)
    {
        eOappEncReader_jomoconfig_t* jmcfg = &p->config.jomoconfig[i];
        
        //if the primary encoder is on SPI
        if (CHECK_ENC_IS_ON_STREAMED_SPI(jmcfg->primary_encoder_type))
        {
            // check if stream is correct, otherwise do nothing
            if (p->stream_map->encoder2stream[jmcfg->primary_encoder_port] == stream_number)
            {
                if (cfgSPIX->sequence.first == hal_spiencoderNONE)
                {
                    cfgSPIX->sequence.first     = (hal_spiencoder_t)jmcfg->primary_encoder_port;
                    cfgSPIX->sequence.list[0]   = (hal_spiencoder_t)jmcfg->primary_encoder_port;
                }
                // if out of bound, discard it
                else if (p->stream_map->encoder2indexinstream[jmcfg->primary_encoder_port] < cfgSPIX->maxsupportedencoders)
                {   
                    #warning -> baco: sembra che non siano previsti buchi nella sequanza dentro lo stream. ovvero: non posso mettere enc1, enc3 senza mettere enc2 ...... verifica
                    cfgSPIX->sequence.list[p->stream_map->encoder2indexinstream[jmcfg->primary_encoder_port]] = (hal_spiencoder_t)jmcfg->primary_encoder_port;
                }
            }
        }
        
        // if the extra encoder is on SPI
        if (CHECK_ENC_IS_ON_STREAMED_SPI(jmcfg->secondary_encoder_type))
        {
            // check if stream is correct, otherwise do nothing
            if (p->stream_map->encoder2stream[jmcfg->secondary_encoder_port] == stream_number)
            {
                if (cfgSPIX->sequence.first == hal_spiencoderNONE)
                {
                    cfgSPIX->sequence.first     = (hal_spiencoder_t)jmcfg->secondary_encoder_port;
                    cfgSPIX->sequence.list[0]   = (hal_spiencoder_t)jmcfg->secondary_encoder_port;
                }
                //if out of bound, discard it
                else if (p->stream_map->encoder2indexinstream[jmcfg->secondary_encoder_port] < cfgSPIX->maxsupportedencoders)
                {
                    cfgSPIX->sequence.list[p->stream_map->encoder2indexinstream[jmcfg->secondary_encoder_port]] = (hal_spiencoder_t)jmcfg->secondary_encoder_port;
                }
            }
        }
    }
}


static void s_eo_appEncReader_deinit_streamofSPIencoders(EOappEncReader *p, EOappEncReader_SPIstream_config_t *cfgSPIX, hal_spiencoder_stream_t stream_number)
{
    uint8_t i;

    for(i=0; i<cfgSPIX->numberofencoders; i++)
    {        
        // we simply deinit the hal encoders
        hal_spiencoder_deinit(cfgSPIX->sequence.list[i]);
    }
}



static void s_eo_appEncReader_init_streamofSPIencoders(EOappEncReader *p, EOappEncReader_SPIstream_config_t *cfgSPIX, hal_spiencoder_stream_t stream_number)
{
    uint8_t i;
	hal_spiencoder_cfg_t enc_cfg = {.priority = hal_int_priorityNONE, .callback_on_rx = NULL, .arg = NULL, .type = hal_spiencoder_typeNONE, .reg_address = 0, .sdata_precheck = hal_false};
    
    for(i=0; i < cfgSPIX->numberofencoders; i++)
    {
        // we define the basic configuration
        
        if(cfgSPIX->encType == (eo_appEncReader_encoder_type_t)hal_spiencoder_typeAEA)
        {   // AEA encoders 
            enc_cfg.priority     	    = hal_int_priority05;
            enc_cfg.callback_on_rx      = NULL;
            enc_cfg.arg                 = NULL;
            enc_cfg.type				= hal_spiencoder_typeAEA;
            enc_cfg.reg_address	        = 0; // not meaningful
            enc_cfg.sdata_precheck      = hal_false;
        }      
        else if(cfgSPIX->encType == (eo_appEncReader_encoder_type_t)hal_spiencoder_typeAMO)
        {   // AMO encoders
            enc_cfg.priority      	    = hal_int_priority05;
            enc_cfg.callback_on_rx      = NULL;
            enc_cfg.arg                 = NULL;
            enc_cfg.type			    = hal_spiencoder_typeAMO;
            enc_cfg.reg_address		    = 0x77;
            enc_cfg.sdata_precheck	    = hal_false;
        }
        else
        {
            return; 
        }
        
        // now we now must define what callback and argument each encoder must have. 
        // the first encoder must activates the next, until reaching the last encoder which stops the chain and marks the reading DONE!
              
        if(hal_spiencoderNONE != cfgSPIX->sequence.list[i+1])
        {   // normal init if it's not the last
            enc_cfg.callback_on_rx = s_eo_appEncReader_isrCbk_onEncRead;
            // the argument is the next encoder to be started
            enc_cfg.arg = &(cfgSPIX->sequence.list[i+1]);
        }
        else
        {    //if it's the last
            if(stream_number == hal_spiencoder_stream0)
            {
                enc_cfg.callback_on_rx = s_eo_appEncReader_isrCbk_onLastEncRead_SPI_stream0;
            }
            else if(stream_number == hal_spiencoder_stream1)
            {
                enc_cfg.callback_on_rx = s_eo_appEncReader_isrCbk_onLastEncRead_SPI_stream1;
            }
            enc_cfg.arg = p;
        }
        
        // finally we init the hal encoder with the filled configuration
        hal_spiencoder_init(cfgSPIX->sequence.list[i], &enc_cfg);
    }
}


static void s_eo_appEncReader_isrCbk_onEncRead(void *arg)
{
    hal_spiencoder_read_start(*((hal_spiencoder_t*)(arg)));
}

static void s_eo_appEncReader_isrCbk_onLastEncRead_SPI_stream0(void *arg)
{
    EOappEncReader *p = (EOappEncReader *)arg;
    
    p->times[0][3] = osal_system_abstime_get(); 
 
    p->SPI_streams[hal_spiencoder_stream0].isacquiring = eobool_false;
    p->configofSPIstream0.spiStatus = encreader_spistatus_finished;
}

static void s_eo_appEncReader_isrCbk_onLastEncRead_SPI_stream1(void *arg)
{
    EOappEncReader *p = (EOappEncReader *)arg;
    
    p->times[1][3] = osal_system_abstime_get();

    p->SPI_streams[hal_spiencoder_stream1].isacquiring = eobool_false;
    p->configofSPIstream1.spiStatus = encreader_spistatus_finished;
}



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

static eObool_t s_eo_appEncReader_IsValidValue_SPICHAIN2(uint32_t *valueraw, eOappEncReader_errortype_t *error)
{
    #warning -> to be defined    
    return(eobool_true);
}


static void s_eo_appEncReader_spichain_reading_is_finished(void* p)
{
    uint32_t tmp = (uint32_t) p;
    hal_spiencoder_t port = (hal_spiencoder_t) tmp;
    hal_spiencoder_stream_t stream = s_eo_theappencreader.stream_map->encoder2stream[port];
    
    s_eo_theappencreader.configofSPIChained[stream].spiStatus = encreader_spistatus_finished;
    s_eo_theappencreader.SPI_streams[stream].isacquiring = eobool_false;
    //eo_common_byte_bitclear((uint8_t*)&s_eo_theappencreader.maskofacquiringspichained, port);
}

static void s_eo_appEncReader_init_chainofSPIencoders(EOappEncReader *p)
{
	hal_spiencoder_cfg_t config = 
    {
        .priority = hal_int_priority05, 
        .callback_on_rx = s_eo_appEncReader_spichain_reading_is_finished, 
        .arg = NULL, 
        .type = hal_spiencoder_typeCHAINof2, 
        .reg_address = 0, 
        .sdata_precheck = hal_false
    };
    
    for(uint8_t i=0; i < hal_spiencoder_streams_number; i++)
    {       
        if((eo_appEncReader_enc_type_SPICHAINOF2 == p->configofSPIChained[i].encType) && (hal_spiencoderNONE != p->configofSPIChained[i].singleport))
        {
            config.callback_on_rx = s_eo_appEncReader_spichain_reading_is_finished;
            config.arg = (void*)p->configofSPIChained[i].singleport;            
            hal_spiencoder_init(p->configofSPIChained[i].singleport, &config);
        }
    }
}


static void s_eo_appEncReader_deinit_chainofSPIencoders(EOappEncReader *p)
{
    for(uint8_t i=0; i < hal_spiencoder_streams_number; i++)
    {       
        if((eo_appEncReader_enc_type_SPICHAINOF2 == p->configofSPIChained[i].encType) && (hal_spiencoderNONE != p->configofSPIChained[i].singleport))
        {          
            hal_spiencoder_deinit(p->configofSPIChained[i].singleport);
        }
    }
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
            p->encodertype[p->totalnumberofencoders++] = eo_appEncReader_enc_type_INC;
            
            s_fake_hal_quadencoder_single_init(jmcfg->primary_encoder_port);
        }
        
        if(jmcfg->secondary_encoder_type == eo_appEncReader_enc_type_INC)
        {
            p->encodertype[p->totalnumberofencoders++] = eo_appEncReader_enc_type_INC;
            
            s_fake_hal_quadencoder_single_init(jmcfg->secondary_encoder_port);
        }
        // handle other cases... with a switch case
       
    }
}


static uint32_t s_eo_appEncReader_rescale2icubdegrees(uint32_t val_raw, uint8_t jomo, eo_appEncReader_encoder_place_t pos_type)
{

    // this is the correct code: we divide by the encoderconversionfactor ...
    // formulas are:
    // in xml file there is GENERAL:Encoders = td expressed in [ticks/deg]
    // robotInterface sets joint->config.encoderconversionfactor = (td / idegconv), expressed in [ticks/icubdeg]
    // idegconv = 182.044 = (64*1024/360) is the conversion from degrees to icubdeg and is expressed as [icubdeg/deg]
    // thus, to obtain the icub-degress in here we must divide the reading of the encoder expressed in [ticks] by
    // divider = joint->config.encoderconversionfactor.

    // this code does apply also to AEA encoders. 
    // so far the hal_spiencoder_get_value() has retrieved ticks with a resulution of 64K ticks / 360 degrees (the upscale from 16K to 64 is done internally)
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

}


static hal_spiencoder_stream_t s_eo_appEncReader_get_spi_stream(EOappEncReader* p, uint8_t port)
{
    if(eo_appEncReader_encoder_portNONE == port)
    {
        return(hal_spiencoder_streamNONE);
    }
    // eo_appEncReader_stream_position_numberof is 3, so that we can map eo_appEncReader_encoder_position[1, 2, 3, 4, 5, 6] into: eo_appEncReader_stream[0, 0, 0, 1, 1, 1]
    
    return((hal_spiencoder_stream_t)p->stream_map->encoder2stream[port]);
}


static void s_fake_hal_quadencoder_single_init(uint8_t port)
{
    hal_quadencoder_init((hal_quadencoder_t)port);    
}


static uint32_t s_fake_hal_quadencoder_getCounter(uint8_t port)
{
#if defined(EOAPPENCODERREADER_DONTUSE_INC)
    return(ENCODER_VALUE_NOT_SUPPORTED);
#else
    return(hal_quadencoder_get_counter((hal_quadencoder_t)port)); 
#endif       
}

static uint32_t s_fake_hal_adc_get_hall_sensor_analog_input_mV(uint8_t port)
{
    return(hal_adc_get_hall_sensor_analog_input_mV(port));     
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




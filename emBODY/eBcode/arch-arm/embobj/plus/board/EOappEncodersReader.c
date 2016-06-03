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

#include "hal_common.h"
#include "hal_spiencoder.h"
#include "hal_quadencoder.h"
#include "hal_adc.h"

#include "EOtheEntities.h"
#include "EOconstarray.h"

#include "EoError.h"
#include "EOtheErrorManager.h"


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

#define CHECK_ENC_IS_CONNECTED(type)                    (eomn_serv_mc_sensor_none != (type))
#define CHECK_ENC_IS_ON_SPI(type)                       ((eomn_serv_mc_sensor_encoder_aea == (type)) || (eomn_serv_mc_sensor_encoder_amo == (type)) || (eomn_serv_mc_sensor_encoder_spichainof2 == (type)))
#define CHECK_ENC_IS_ON_STREAMED_SPI_WITHOTHERS(type)   ((eomn_serv_mc_sensor_encoder_aea == (type)) || (eomn_serv_mc_sensor_encoder_amo == (type)))
#define CHECK_ENC_IS_ON_STREAMED_SPI_ALONE(type)        ((eomn_serv_mc_sensor_encoder_spichainof2 == (type)))


#define ENCODER_VALUE_NOT_SUPPORTED                     (hal_NA32)


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

static void s_eo_deconfigure_SPI_encoders(EOappEncReader *p);
static eObool_t s_eo_configure_SPI_encoders(EOappEncReader *p);

static eObool_t s_eo_appEncReader_IsValidValue_AEA(uint32_t *valueraw, eOappEncReader_errortype_t *error);
static eObool_t s_eo_appEncReader_IsValidValue_SPICHAIN2(uint32_t *valueraw, eOappEncReader_errortype_t *error);
static eObool_t s_eo_appEncReader_IsValidValue_SPICHAIN3(uint32_t *valueraw, eOappEncReader_errortype_t *error);

static void s_eo_appEncReader_deconfigure_NONSPI_encoders(EOappEncReader *p);
static void s_eo_appEncReader_configure_NONSPI_encoders(EOappEncReader *p);

static uint32_t s_eo_appEncReader_rescale2icubdegrees(uint32_t val_raw, uint8_t jomo, eOmn_serv_mc_sensor_position_t pos);
static uint32_t s_eo_appEncReader_mais_rescale2icubdegrees(EOappEncReader* p, uint32_t val_raw, uint8_t jomo);
static uint32_t s_eo_appEncReader_hallAdc_rescale2icubdegrees(EOappEncReader* p, uint32_t val_raw, uint8_t jomo);

static hal_spiencoder_stream_t s_eo_appEncReader_get_spi_stream(EOappEncReader* p, uint8_t port);


static void s_eo_appEncReader_init_halSPIencoders(EOappEncReader *p);

static void s_eo_appEncReader_deinit_halSPIencoders(EOappEncReader *p);

static void s_eo_appEncReader_anotherSPIread(void* arg);
static void s_eo_appEncReader_stopSPIread(void* arg);

static uint32_t s_eo_read_mais_for_port(EOappEncReader *p, uint8_t port);

static hal_spiencoder_type_t s_eo_appEncReader_map_encodertype_to_halspiencodertype(eOmn_serv_mc_sensor_type_t encodertype);



static eOresult_t s_eo_appEncReader_Diagnostics_Config(EOappEncReader *p, eo_appEncReader_diagnostics_cfg_t* cfg);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static EOappEncReader s_eo_theappencreader =
{
    .initted                = eobool_false,
    .active                 = eobool_false,
    .totalnumberofencoders  = 0,
    .stream_map             = NULL,
    .config                 = {0},
    .SPI_streams            = {{.type = hal_spiencoder_typeNONE, .numberof = 0, .maxsupported = 0, .isacquiring = eobool_false, .id = {hal_spiencoderNONE}}},
    .diagnostics            =
    {
        .config = 
        {
            .jomomask       = 0,
            .periodOKreport = 0,
            .periodKOreport = 0,
            .errorled       = eo_ledpulser_led_none,
            .errorgpio      = { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE }
        },
        .par64              = 0,
        .par16              = 0
    },
    .maisCoversionFactors   = {1.0, 1.0, 1.0, 1.0},
    .hallAdcConversionData  = 
    {
        .offsets            = {0,   0,   0,   0},
        .factors            = {1.0, 1.0, 1.0, 1.0}
    }
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

    memset(&s_eo_theappencreader.config, 0, sizeof(s_eo_theappencreader.config));
    
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
    
    
    memset(&p->config, 0, sizeof(s_eo_theappencreader.config));
    p->config.numofjomos = 0;
    
    s_eo_theappencreader.totalnumberofencoders = 0;
    s_eo_theappencreader.active = eobool_false;    
    return(eores_OK);
}


extern eOresult_t eo_appEncReader_Activate(EOappEncReader *p, const eOmn_serv_arrayof_4jomodescriptors_t *arrayofjomodes)
{
    if((NULL == p) || (NULL == arrayofjomodes))
    {
        return(eores_NOK_nullpointer);
    }   

    EOconstarray* carray = eo_constarray_Load((EOarray*)arrayofjomodes);
    
    if(eobool_true == s_eo_theappencreader.active)
    {
        eo_appEncReader_Deactivate(p);        
    }    

    // ok, now the rest of the things
    
    // basic check ...
    if(eo_constarray_Size(carray) > eOappEncReader_jomos_maxnumberof)
    {
        return(eores_NOK_generic);
    }
    
    // 1. prepare the config
    p->config.numofjomos = eo_constarray_Size(carray);
    for(uint8_t i=0; i<eOappEncReader_jomos_maxnumberof; i++)
    {
        const eOmn_serv_jomo_descriptor_t *jomodes = (eOmn_serv_jomo_descriptor_t*) eo_constarray_At(carray, i);
        p->config.jomoconfig[i].primary = jomodes->sensor;
        p->config.jomoconfig[i].secondary = jomodes->extrasensor;
    }

    
    // 2. configure spi encoders   
    s_eo_configure_SPI_encoders(p);

    // 3. configure other encoders
    s_eo_appEncReader_configure_NONSPI_encoders(p);
    
        
    s_eo_theappencreader.active = eobool_true;    
    return(eores_OK);
}

extern eOresult_t eo_appEncReader_UpdatedMaisConversionFactors(EOappEncReader *p, uint8_t jomo, float convFactor)
{
    eOappEncReader_jomoconfig_t this_jomoconfig = p->config.jomoconfig[jomo];

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    

    // check existence for primary encoder
    if (eomn_serv_mc_sensor_mais != this_jomoconfig.primary.type)
    {
        return(eores_NOK_unsupported);
    }
    
    p->maisCoversionFactors[jomo] = convFactor;
    
    return(eores_OK);
}


extern eOresult_t eo_appEncReader_UpdatedHallAdcConversionFactors(EOappEncReader *p, uint8_t jomo, float convFactor)
{
    eOappEncReader_jomoconfig_t this_jomoconfig = p->config.jomoconfig[jomo];

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    

    // check existence for primary encoder
    if (eomn_serv_mc_sensor_encoder_absanalog != this_jomoconfig.primary.type)
    {
        return(eores_NOK_unsupported);
    }
    
    p->hallAdcConversionData.factors[jomo] = convFactor;
    
    return(eores_OK);
}
extern eOresult_t eo_appEncReader_UpdatedHallAdcOffset(EOappEncReader *p, uint8_t jomo, int32_t offset)
{
    eOappEncReader_jomoconfig_t this_jomoconfig = p->config.jomoconfig[jomo];

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    

    // check existence for primary encoder
    if (eomn_serv_mc_sensor_encoder_absanalog != this_jomoconfig.primary.type)
    {
        return(eores_NOK_unsupported);
    }
    
    p->hallAdcConversionData.offsets[jomo] = offset;
    
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
    
    // reset isacquiring status of the two spi streams, then start acquisition
    for(uint8_t i=0; i<hal_spiencoder_streams_number; i++)
    {
        eOappEncReader_stream_t* thestream = &p->SPI_streams[i];
        hal_spiencoder_t firstencoder = thestream->id[0];
        
        thestream->isacquiring = eobool_false;
        
        if(hal_spiencoderNONE != firstencoder)
        {
            thestream->isacquiring = eobool_true;
            hal_spiencoder_read_start(firstencoder);
        }            
    }    
     
    return(eores_OK);
}


extern eOresult_t eo_appEncReader_Diagnostics_Enable(EOappEncReader *p, eObool_t on)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    eo_appEncReader_diagnostics_cfg_t config =
    {
        .jomomask = 0x0f,
        .periodOKreport = 0,
        .periodKOreport = 1,
        //.errorled = eo_ledpulser_led_five,
        //.errorgpio = { .port = hal_gpio_portC, .pin = hal_gpio_pin13 }  
        .errorled = eo_ledpulser_led_none,
        .errorgpio = { .port = hal_gpio_portNONE, .pin = hal_gpio_pinNONE }          
    };
    
    if(eobool_false == on)
    {
        config.jomomask = 0;
        config.periodKOreport = 0;
        config.periodOKreport = 0;
    }

    return(s_eo_appEncReader_Diagnostics_Config(p, &config));    
}


extern eOresult_t eo_appEncReader_Diagnostics_Tick(EOappEncReader *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(0 != p->diagnostics.config.jomomask)
    {
        eOerrmanDescriptor_t errdes = {0};
        
        if(0 != p->diagnostics.par16)
        {   // we have something wrong to signal
            
            // the gpio is put on and it stays on forever, no matter what.
            hal_gpio_setval(p->diagnostics.config.errorgpio, hal_gpio_valHIGH);  
            
            // the led is made pulsing once for 100 ms. we pulse again only if the situation stays wrong
            if(eobool_false == eo_ledpulser_IsOn(eo_ledpulser_GetHandle(), p->diagnostics.config.errorled))
            {
                eo_ledpulser_Start(eo_ledpulser_GetHandle(), p->diagnostics.config.errorled, eok_reltime100ms, 1);
            }
            
            // we transmit a diagnostics message
            errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag07);
            errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
            errdes.sourceaddress    = 0;
            errdes.par16            = p->diagnostics.par16;
            errdes.par64            = p->diagnostics.par64;
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &errdes);                        
        }
        
        // reset par16 / par64
        p->diagnostics.par16 = 0;
        p->diagnostics.par64 = 0;     
        
    }
    
    return(eores_OK);
}

extern eOresult_t eo_appEncReader_GetValue(EOappEncReader *p, uint8_t jomo, eOencoderreader_valueInfo_t *primary, eOencoderreader_valueInfo_t *secondary)
{    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
//    eOerrmanDescriptor_t errdes = {0};
    
    hal_spiencoder_position_t val_raw = 0; // marco.accame: it should be is a hal_spiencoder_position_t 
    eOresult_t res1 = eores_OK;
    eOresult_t res2 = eores_OK;
    eOappEncReader_errortype_t errortype = err_NONE;
    eOappEncReader_jomoconfig_t this_jomoconfig = p->config.jomoconfig[jomo];
    hal_spiencoder_errors_flags flags = {0};
    
    
    primary->composedof = eomn_mc_sensor_getnumofcomponets((eOmn_serv_mc_sensor_type_t)this_jomoconfig.primary.type);
    secondary->composedof = eomn_mc_sensor_getnumofcomponets((eOmn_serv_mc_sensor_type_t)this_jomoconfig.secondary.type);
    primary->errortype = encreader_err_NONE;
    secondary->errortype = encreader_err_NONE;
    
    
    // check existence for primary encoder
    if (CHECK_ENC_IS_CONNECTED(this_jomoconfig.primary.type))
    {
        switch (this_jomoconfig.primary.type)
        {
            case eomn_serv_mc_sensor_encoder_aea:
            {
                eObool_t evaldiagnostics = eo_common_byte_bitcheck(p->diagnostics.config.jomomask, jomo);

                res1 = (eOresult_t)hal_spiencoder_get_value((hal_spiencoder_t)this_jomoconfig.primary.port, &val_raw, &flags);
                
                if(eores_OK != res1)
                {
                    if(eobool_true == evaldiagnostics)
                    {   // i assume someone has reset the bits previously
                        p->diagnostics.par16 |= (AEAerror_SPI<<(4*jomo));       // shift by nibbles ..
                        p->diagnostics.par64 |= (0xffff<<(16*jomo));            // shift by two bytes                   
                    }                    
                                        
//                    errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag07);
//                    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//                    errdes.sourceaddress    = 0;
//                    errdes.par16            = jomo;
//                    errdes.par64            = 0x666;
//                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, NULL, &errdes);
                    
                    //*primaryvalue = 0;
                    primary->errortype = encreader_err_READING;
                    return(eores_NOK_generic);
                }

                if(eobool_false == s_eo_appEncReader_IsValidValue_AEA(&val_raw, &errortype))
                {
                                       
                    if(eobool_true == evaldiagnostics)
                    {
                        if(err_onParityError == errortype)
                        {   // i assume someone has reset the bits previously
                            p->diagnostics.par16 |= (AEAerror_CRC<<(4*jomo));       // shift by nibbles ..
                            p->diagnostics.par64 &= (0xe000<<(16*jomo));            // shift by two bytes   
                        }
                        else
                        {   // i assume someone has reset the bits previously
                            uint16_t value = (val_raw >> 6) & 0x0FFF;
                            p->diagnostics.par16 |= (AEAerror_CHIP<<(4*jomo));      // shift by nibbles ..
                            p->diagnostics.par64 &= (value<<(16*jomo));             // shift by two bytes                               
                        }
                    }
                    
//                    errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag07);
//                    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//                    errdes.sourceaddress    = 0;
//                    errdes.par16            = jomo;
//                    errdes.par64            = (err_onParityError == errortype) ? (0x777) : (0x888);
//                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, NULL, &errdes);  
                    
                    //*primaryvalue = 0;  
                    primary->errortype = (eOencoderreader_errortype_t)errortype;
                    return(eores_NOK_generic);
                }

                // marco.accame: hal_spiencoder_get_value() gives back a value in uint32_t with only 18 bits of information (internally masked with 0x03FFFF).
                // only the 12 most significant bits contain a position reading. to obtain the ticks we should do:
                // ticks = (val_raw >> 6) & 0x0FFF;
                // the resolution is now 4096 ticks per revolution.
                
                val_raw = (val_raw >> 6) & 0x0FFF;
                primary->value[0] = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, (eOmn_serv_mc_sensor_position_t)this_jomoconfig.primary.pos);                
               
            } break; 

            case eomn_serv_mc_sensor_encoder_amo:
            {
                
                res1 = (eOresult_t)hal_spiencoder_get_value((hal_spiencoder_t)this_jomoconfig.primary.port, &val_raw, &flags);
            
                if(eores_OK != res1)
                {
                    // marco.accame: verify if the hal for amo encoder returns error also for parity or else ... so far, i just return a spi error
                    //*primaryvalue = 0;
                    primary->errortype = encreader_err_READING;
                    return(eores_NOK_generic);
                }
            
                //val_raw = (val_raw>>4) & 0xFFFF;
                primary->value[0] = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, (eOmn_serv_mc_sensor_position_t)this_jomoconfig.primary.pos);
                
            } break;
            
            case eomn_serv_mc_sensor_encoder_spichainof2:
            {
                res1 = (eOresult_t)hal_spiencoder_get_value((hal_spiencoder_t)this_jomoconfig.primary.port, &val_raw, &flags);
                
                if(eores_OK != res1)
                {
                    //*primaryvalue = 0;
                    primary->errortype = encreader_err_READING;
                    return(eores_NOK_generic);
                }

                if(eobool_false == s_eo_appEncReader_IsValidValue_SPICHAIN2(&val_raw, &errortype))
                {
                    //*primaryvalue = 0;  
                    primary->errortype = (eOencoderreader_errortype_t)errortype;
                    return(eores_NOK_generic);
                }
                
                //uint32_t val1 = (val_raw >>  2) & 0x0fff; // it is the first encoder in the chain
                //uint32_t val2 = (val_raw >> 18) & 0x0fff; // it is the second encoder in the chain
                //val_raw = val1 + val2; // we give back the sum of the two
                
                //*primaryvalue = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, (eOmn_serv_mc_sensor_position_t)this_jomoconfig.primary.pos);                

                primary->value[0] = (val_raw >>  2) & 0x0fff; // it is the first encoder in the chain
                primary->value[1] = (val_raw >> 18) & 0x0fff; // it is the second encoder in the chain
                
            } break;
            
            case eomn_serv_mc_sensor_encoder_inc:
            {
                
                val_raw = hal_quadencoder_get_counter((hal_quadencoder_t)this_jomoconfig.primary.port);
                if(ENCODER_VALUE_NOT_SUPPORTED == val_raw)
                {
                    primary->value[0] = 0;
                }
                else
                {
                    primary->value[0] = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, (eOmn_serv_mc_sensor_position_t)this_jomoconfig.primary.pos);
                }    
         
                res1 = eores_OK;
                
            } break;
            
            case eomn_serv_mc_sensor_encoder_absanalog:
            {
                // get the voltage from the motor port (0 - 3300mV)
            
                val_raw = hal_adc_get_hall_sensor_analog_input_mV(this_jomoconfig.primary.port);        
                if(ENCODER_VALUE_NOT_SUPPORTED == val_raw)
                {
                    primary->value[0] = ENCODER_VALUE_NOT_SUPPORTED;
                    res1 = eores_NOK_generic;                    
                }
                else
                {
                    // convert to iCubDegrees
                    primary->value[0] = s_eo_appEncReader_hallAdc_rescale2icubdegrees(p, val_raw, jomo);
                    
                    if(0 != val_raw)
                    {   // marco.accame: boh ... e non si mette NOK_generic in caso contrario ??? per ora non lo cambio.
                        res1 = eores_OK;
                    }                        
                }
              
               
            } break;
            
            case eomn_serv_mc_sensor_mais:
            {

                val_raw =  s_eo_read_mais_for_port(p, this_jomoconfig.primary.port); 
  
                if(ENCODER_VALUE_NOT_SUPPORTED == val_raw)
                {
                    primary->value[0] = ENCODER_VALUE_NOT_SUPPORTED;
                    res1 = eores_NOK_generic;                    
                }
                else
                {
                    // convert to iCubDegrees
                    primary->value[0] = s_eo_appEncReader_mais_rescale2icubdegrees(p, val_raw, jomo);
                    res1 = eores_OK;                    
                }                             
               
            } break;
            
            case eomn_serv_mc_sensor_encoder_spichainof3:
            {
                res1 = (eOresult_t)hal_spiencoder_get_value((hal_spiencoder_t)this_jomoconfig.primary.port, &val_raw, &flags);
                
                if(eores_OK != res1)
                {
                    //*primaryvalue = 0;
                    primary->errortype = encreader_err_READING;
                    return(eores_NOK_generic);
                }

                if(eobool_false == s_eo_appEncReader_IsValidValue_SPICHAIN3(&val_raw, &errortype))
                {
                    //*primaryvalue = 0;  
                    primary->errortype = (eOencoderreader_errortype_t)errortype;
                    return(eores_NOK_generic);
                }
                

                primary->value[0] = (val_raw >>  2) & 0x0fff; // it is the first encoder in the chain
                primary->value[1] = (val_raw >> 18) & 0x0fff; // it is the second encoder in the chain
                primary->value[2] = 0;
            
            }break;
            
            default:
            {
                primary->value[0] = ENCODER_VALUE_NOT_SUPPORTED;
                
            } break;
            
            // should handle the other cases...
        }
    }
    else
    {
        primary->value[0] = ENCODER_VALUE_NOT_SUPPORTED;
    }
    
    // check existence for extra encoder
    if (CHECK_ENC_IS_CONNECTED(this_jomoconfig.secondary.type))
    {
        switch (this_jomoconfig.secondary.type)
        {
            case eomn_serv_mc_sensor_encoder_aea:
            {
                res2 = (eOresult_t)hal_spiencoder_get_value((hal_spiencoder_t)this_jomoconfig.secondary.port, &val_raw, &flags);
                
                if(eores_OK != res2)
                {
                    //*secondaryvalue = 0;
                    secondary->errortype = encreader_err_READING;
                    return(eores_NOK_generic);
                }

                if (eobool_false == s_eo_appEncReader_IsValidValue_AEA(&val_raw, &errortype))
                {
                    //*secondaryvalue =0;  
                     secondary->errortype = (eOencoderreader_errortype_t)errortype;
                    return(eores_NOK_generic);
                }

                val_raw = (val_raw >> 6) & 0x0FFF;
                secondary->value[0] = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, (eOmn_serv_mc_sensor_position_t)this_jomoconfig.secondary.pos);  
                
            } break;

            case eomn_serv_mc_sensor_encoder_amo:
            {
                    
                res2 = (eOresult_t)hal_spiencoder_get_value((hal_spiencoder_t)this_jomoconfig.secondary.port, &val_raw, &flags);
            
                if(eores_OK != res2)
                {
                    //*secondaryvalue = 0;
                    secondary->errortype = encreader_err_READING;
                    return(eores_NOK_generic);
                }
            
                val_raw = (val_raw>>4) & 0xFFFF;
                secondary->value[0] = s_eo_appEncReader_rescale2icubdegrees( val_raw, jomo, (eOmn_serv_mc_sensor_position_t)this_jomoconfig.secondary.pos);
               
            } break;
            
            case eomn_serv_mc_sensor_encoder_spichainof2:
            {                    
                res2 = (eOresult_t)hal_spiencoder_get_value((hal_spiencoder_t)this_jomoconfig.secondary.port, &val_raw, &flags);
            
                if(eores_OK != res2)
                {
                    //*secondaryvalue = 0;
                    secondary->errortype = encreader_err_READING;
                    return(eores_NOK_generic);
                }
                
                if(eobool_false == s_eo_appEncReader_IsValidValue_SPICHAIN2(&val_raw, &errortype))
                {
                    //*secondaryvalue = 0;  
                    secondary->errortype = (eOencoderreader_errortype_t)errortype;
                    return(eores_NOK_generic);
                }                
            
                //uint16_t val1 = (val_raw >> 2) & 0x0fff; // it is the first encoder in the chain
                //uint16_t val2 = (val_raw >> 18) & 0x0fff; // it is the second encoder in the chain
                //val_raw = val1 + val2; // we give back the sum of the two
                
                //*secondaryvalue = s_eo_appEncReader_rescale2icubdegrees( val_raw, jomo, (eOmn_serv_mc_sensor_position_t)this_jomoconfig.secondary.pos);
                secondary->value[0] = (val_raw >> 2) & 0x0fff; // it is the first encoder in the chain
                secondary->value[1] = (val_raw >> 18) & 0x0fff; // it is the second encoder in the chain
            } break; 	            
            
            case eomn_serv_mc_sensor_encoder_inc:
            {
             
                val_raw = hal_quadencoder_get_counter((hal_quadencoder_t)this_jomoconfig.secondary.port);
                if(ENCODER_VALUE_NOT_SUPPORTED == val_raw)
                {
                    secondary->value[0] = 0;
                }
                else
                {
                    secondary->value[0] = s_eo_appEncReader_rescale2icubdegrees(val_raw, jomo, (eOmn_serv_mc_sensor_position_t)this_jomoconfig.secondary.pos);
                }                
              
                
            } break;
            
            case eomn_serv_mc_sensor_encoder_absanalog:
            {
                // get the voltage from the motor port (0 - 3300mV)
           
                val_raw = hal_adc_get_hall_sensor_analog_input_mV(this_jomoconfig.secondary.port);        
                if(ENCODER_VALUE_NOT_SUPPORTED == val_raw)
                {
                    secondary->value[0] = ENCODER_VALUE_NOT_SUPPORTED;
                    res2 = eores_NOK_generic;                    
                }
                else
                {
                    // convert to iCubDegrees
                    
                    secondary->value[0] = s_eo_appEncReader_hallAdc_rescale2icubdegrees(p, val_raw, jomo);
                    if(0 != val_raw)
                    {   // marco.accame: boh ... e non si mette NOK_generic in caso contrario ??? per ora non lo cambio.
                        res2 = eores_OK;
                    }                        
                } 
               
                
            } break;
            
            case eomn_serv_mc_sensor_mais:
            {

                val_raw =  s_eo_read_mais_for_port(p, this_jomoconfig.secondary.port); 
  
                if(ENCODER_VALUE_NOT_SUPPORTED == val_raw)
                {
                    secondary->value[0] = ENCODER_VALUE_NOT_SUPPORTED;
                    res1 = eores_NOK_generic;                    
                }
                else
                {
                    // convert to iCubDegrees
                    secondary->value[0] = s_eo_appEncReader_mais_rescale2icubdegrees(p, val_raw, jomo);
                    res1 = eores_OK;                    
                }                             
               
            } break;            
            
            case eomn_serv_mc_sensor_encoder_spichainof3:
            {
                res1 = (eOresult_t)hal_spiencoder_get_value((hal_spiencoder_t)this_jomoconfig.secondary.port, &val_raw, &flags);
                
                if(eores_OK != res1)
                {
                    //*primaryvalue = 0;
                    secondary->errortype = encreader_err_READING;
                    return(eores_NOK_generic);
                }

                if(eobool_false == s_eo_appEncReader_IsValidValue_SPICHAIN3(&val_raw, &errortype))
                {
                    //*primaryvalue = 0;  
                    secondary->errortype = (eOencoderreader_errortype_t)errortype;
                    return(eores_NOK_generic);
                }
                

                secondary->value[0] = (val_raw >>  2) & 0x0fff; // it is the first encoder in the chain
                secondary->value[1] = (val_raw >> 18) & 0x0fff; // it is the second encoder in the chain
                secondary->value[2] = 0;
            
            }break;
            
            default:
            {
                secondary->value[0] = ENCODER_VALUE_NOT_SUPPORTED;                
            } break;
            
            // should handle the other cases...
        }	
    }
    else
    {
        secondary->value[0] = ENCODER_VALUE_NOT_SUPPORTED;
    }
    
    // return value based only on the reading of the primary encoder ... mah ... non sarebbe meglio usare una funzione dedicata al primary ed una la secondary? pensaci.
    return(res1);
}




extern eObool_t eo_appEncReader_isReady(EOappEncReader *p)
{
    if(NULL == p)
    {
        return(eobool_true);
    }
    
    eObool_t ready = eobool_true;

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


static void s_eo_clear_SPI_streams(EOappEncReader *p)
{
    for(uint8_t i=0; i<hal_spiencoder_streams_number; i++)
    {    
        p->SPI_streams[i].type = hal_spiencoder_typeNONE;        
        p->SPI_streams[i].numberof = 0;
        p->SPI_streams[i].maxsupported = 0;
        p->SPI_streams[i].isacquiring = eobool_false;
        for(uint8_t j=0; j<(hal_spiencoder_maxnumber_in_stream+1); j++)
        {
            p->SPI_streams[i].id[j] = hal_spiencoderNONE;
        }
    }        
}


static eObool_t s_eo_prepare_SPI_streams(EOappEncReader *p)
{           
    uint8_t i = 0;
    uint8_t spiencoderportisused[hal_spiencoders_number] = {0};
    
    s_eo_clear_SPI_streams(p);
    
    uint8_t numberofSPIbased = 0;

    for(i=0; i<p->config.numofjomos; i++)
    {
        const eOappEncReader_jomoconfig_t *jdes = &p->config.jomoconfig[i];
        
        // if in the joint i-th we have any spi-based encoder which is primary or secondary, then we put it into the proper spi-stream        

        if(CHECK_ENC_IS_ON_STREAMED_SPI_WITHOTHERS(jdes->primary.type))
        {    
            uint8_t port = jdes->primary.port;
            hal_spiencoder_stream_t streamnumber = s_eo_appEncReader_get_spi_stream(p, port);
            if(hal_spiencoder_streamNONE != streamnumber)
            {    
                p->SPI_streams[streamnumber].type = s_eo_appEncReader_map_encodertype_to_halspiencodertype((eOmn_serv_mc_sensor_type_t)jdes->primary.type);   
                p->SPI_streams[streamnumber].maxsupported = p->stream_map->stream2numberofencoders[streamnumber];
                // i must check if there is already encoder specified by jdes->primary.port inside ... if already, then we dont add it.
                // we do that because we may have a primary and secondary which have the same aea on the same port.
                if(0 == spiencoderportisused[port])
                {
                    if(p->SPI_streams[streamnumber].numberof < p->SPI_streams[streamnumber].maxsupported)
                    {
                        spiencoderportisused[port] = 1;
                        p->SPI_streams[streamnumber].id[p->SPI_streams[streamnumber].numberof++] = (hal_spiencoder_t)port;
                    }
                    else
                    {
                        // error ....
                    }
                    numberofSPIbased++;
                } // else it means that another jomo is using it ... 
                
            }
            else
            {
                // error;               
            }
        }

        if(CHECK_ENC_IS_ON_STREAMED_SPI_WITHOTHERS(jdes->secondary.type))
        {   
            uint8_t port = jdes->secondary.port;
            hal_spiencoder_stream_t streamnumber = s_eo_appEncReader_get_spi_stream(p, port);
            if(hal_spiencoder_streamNONE != streamnumber)
            {    
                p->SPI_streams[streamnumber].type = s_eo_appEncReader_map_encodertype_to_halspiencodertype((eOmn_serv_mc_sensor_type_t)jdes->secondary.type);   
                p->SPI_streams[streamnumber].maxsupported = p->stream_map->stream2numberofencoders[streamnumber];
                if(0 == spiencoderportisused[port])
                {
                    if(p->SPI_streams[streamnumber].numberof < p->SPI_streams[streamnumber].maxsupported)
                    {
                        spiencoderportisused[port] = 1;
                        p->SPI_streams[streamnumber].id[p->SPI_streams[streamnumber].numberof++] = (hal_spiencoder_t)port;
                    }
                    else
                    {
                        // error ....
                    }
                    numberofSPIbased++;
                } // else it means that another jomo is using it ...                
                
            }
            else
            {
                // error;               
            }
        }  
        
        if(CHECK_ENC_IS_ON_STREAMED_SPI_ALONE(jdes->primary.type))
        { 
            uint8_t port = jdes->primary.port;
            hal_spiencoder_stream_t streamnumber = s_eo_appEncReader_get_spi_stream(p, port);
            if(hal_spiencoder_streamNONE != streamnumber)
            {    
                p->SPI_streams[streamnumber].type = s_eo_appEncReader_map_encodertype_to_halspiencodertype((eOmn_serv_mc_sensor_type_t)jdes->primary.type);   
                p->SPI_streams[streamnumber].maxsupported = 1;
                if(0 == spiencoderportisused[port])
                {
                    if(p->SPI_streams[streamnumber].numberof < p->SPI_streams[streamnumber].maxsupported)
                    {
                        spiencoderportisused[port] = 1;
                        p->SPI_streams[streamnumber].id[p->SPI_streams[streamnumber].numberof++] = (hal_spiencoder_t)port;
                    }
                    else
                    {
                        // error ....
                    }
                    numberofSPIbased++;
                }  // else it means that another jomo is using it ... 
               
            }
            else
            {
                // error;               
            }
        }

        if(CHECK_ENC_IS_ON_STREAMED_SPI_ALONE(jdes->secondary.type))
        {  
            uint8_t port = jdes->secondary.port;            
            hal_spiencoder_stream_t streamnumber = s_eo_appEncReader_get_spi_stream(p, port);
            if(hal_spiencoder_streamNONE != streamnumber)
            {    
                p->SPI_streams[streamnumber].type = s_eo_appEncReader_map_encodertype_to_halspiencodertype((eOmn_serv_mc_sensor_type_t)jdes->secondary.type);   
                p->SPI_streams[streamnumber].maxsupported = 1;
                if(0 == spiencoderportisused[port])
                {
                    if(p->SPI_streams[streamnumber].numberof < p->SPI_streams[streamnumber].maxsupported)
                    {
                        spiencoderportisused[port] = 1;
                        p->SPI_streams[streamnumber].id[p->SPI_streams[streamnumber].numberof++] = (hal_spiencoder_t)port;
                    }
                    else
                    {
                        // error ....
                    }                
                    numberofSPIbased++;
                } // else it means that another jomo is using it ... 
            }
            else
            {
                // error;               
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



static void s_eo_deconfigure_SPI_encoders(EOappEncReader *p)
{
    #warning TODO: make some tests about initting and deinitting hal etc.
        
    s_eo_appEncReader_deinit_halSPIencoders(p);
        
    s_eo_clear_SPI_streams(p);

}

static eObool_t s_eo_configure_SPI_encoders(EOappEncReader *p)
{      
    // 1.a prepare spi streams    
    if(eobool_false == s_eo_prepare_SPI_streams(p))
    {
        return(eobool_false);
    }   
        
    s_eo_appEncReader_init_halSPIencoders(p);
    
    return(eobool_true);
}



static void s_eo_appEncReader_deinit_halSPIencoders(EOappEncReader *p)
{
   
    for(uint8_t i=0; i < hal_spiencoder_streams_number; i++)
    {
        eOappEncReader_stream_t* thestream = & p->SPI_streams[i];
         
        /// now we do each encoder in the stream
        
        for(uint8_t j=0; j<hal_spiencoder_maxnumber_in_stream; j++)
        {
            hal_spiencoder_t id = thestream->id[j];
            if(hal_spiencoderNONE != id)
            {
                hal_spiencoder_deinit(id);
            }                        
        }
    }
        
}


static void s_eo_appEncReader_init_halSPIencoders(EOappEncReader *p)
{
	hal_spiencoder_cfg_t config = {.priority = hal_int_priorityNONE, .callback_on_rx = NULL, .arg = NULL, .type = hal_spiencoder_typeNONE, .reg_address = 0, .sdata_precheck = hal_false};
    
    for(uint8_t i=0; i < hal_spiencoder_streams_number; i++)
    {
        eOappEncReader_stream_t* thestream = & p->SPI_streams[i];
        
        // adjust basic config
        if(hal_spiencoder_typeAEA == thestream->type)
        {
            config.priority     	    = hal_int_priority05;
            config.callback_on_rx       = NULL;
            config.arg                  = NULL;
            config.type				    = hal_spiencoder_typeAEA;
            config.reg_address	        = 0; // not meaningful
            config.sdata_precheck       = hal_false;            
        }
        else if(hal_spiencoder_typeAMO == thestream->type)
        {
            config.priority      	    = hal_int_priority05;
            config.callback_on_rx       = NULL;
            config.arg                  = NULL;
            config.type			        = hal_spiencoder_typeAMO;
            config.reg_address		    = 0x77;
            config.sdata_precheck	    = hal_false;            
        }
        else if(hal_spiencoder_typeCHAINof2 == thestream->type)
        {
            config.priority = hal_int_priority05; 
            config.callback_on_rx = s_eo_appEncReader_stopSPIread; 
            config.arg = (void*) thestream; 
            config.type = hal_spiencoder_typeCHAINof2; 
            config.reg_address = 0; 
            config.sdata_precheck = hal_false;
            
            hal_spiencoder_init(thestream->id[0], &config);
            // if the stream is chainof2 we init only one encoder
            continue;            
            
        }
        else
        {
            continue;
        }
        
        /// now we do each encoder in the stream
        
        for(uint8_t j=0; j<hal_spiencoder_maxnumber_in_stream; j++)
        {
            hal_spiencoder_t id = thestream->id[j];
            hal_spiencoder_t next = thestream->id[j+1];
            if(hal_spiencoderNONE != id)
            {
                // both aea and amo have the same mechanism
                
                if(hal_spiencoderNONE != next)
                {   // there is another one after
                    config.callback_on_rx = s_eo_appEncReader_anotherSPIread;
                    config.arg = (void*)&thestream->id[j+1];
                }
                else
                {   // it is the last  
                    config.callback_on_rx = s_eo_appEncReader_stopSPIread;
                    config.arg = (void*) thestream;                   
                }
                
                hal_spiencoder_init(id, &config);
            }                        
        }
    }
        
}



static void s_eo_appEncReader_anotherSPIread(void* arg)
{
    hal_spiencoder_read_start(*((hal_spiencoder_t*)(arg)));
}


static void s_eo_appEncReader_stopSPIread(void* arg)
{
    eOappEncReader_stream_t* thestream = (eOappEncReader_stream_t*) arg;    
    thestream->isacquiring = eobool_false;
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
    uint16_t first = *valueraw & 0xffff;
    uint16_t second = ((*valueraw) >> 16) & 0xffff;   
       
    if(1 == (eo_common_hlfword_bitsetcount(first) % 2))
    { 
        *error = err_onParityError;
        return(eobool_false);
    }
    
    if(1 == (eo_common_hlfword_bitsetcount(second) % 2))
    { 
        *error = err_onParityError;
        return(eobool_false);
    }  

    if(eobool_true == eo_common_hlfword_bitcheck(first, 1))
    {
        *error = err_onReadFromSpi;
        return(eobool_false);    
    }
    
    if(eobool_true == eo_common_hlfword_bitcheck(second, 1))
    {
        *error = err_onReadFromSpi;
        return(eobool_false);    
    }    
    
    return(eobool_true);
}

static eObool_t s_eo_appEncReader_IsValidValue_SPICHAIN3(uint32_t *valueraw, eOappEncReader_errortype_t *error)
{    
     //to be implemented
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
        
        if(jmcfg->primary.type == eomn_serv_mc_sensor_encoder_inc)
        {  
            hal_quadencoder_init((hal_quadencoder_t)jmcfg->primary.port);            
        }
        
        if(jmcfg->secondary.type == eomn_serv_mc_sensor_encoder_inc)
        { 
            hal_quadencoder_init((hal_quadencoder_t)jmcfg->secondary.port);           
        }
        
        // for adh: do things ...
        
        //  for mais-based encoder ... do nothing because wa get direct access to mais memory
       
    }
}

static uint32_t s_eo_appEncReader_mais_rescale2icubdegrees(EOappEncReader* p, uint32_t val_raw, uint8_t jomo)
{
    uint32_t retval = val_raw;
    
    float divider = p->maisCoversionFactors[jomo];
    
    if(0.0f == divider)
    {
        return(3000);
    }
    
    if(divider < 0)
    {
        divider = -divider;
    }

    retval = (float)val_raw / divider;
    
    return(retval);
}

static uint32_t s_eo_appEncReader_hallAdc_rescale2icubdegrees(EOappEncReader* p, uint32_t val_raw, uint8_t jomo)
{
    uint32_t retval = val_raw;
    
    float divider = p->hallAdcConversionData.factors[jomo];
    
    if(0.0f == divider)
    {
        return(3000);
    }
    
    if(divider < 0)
    {
        divider = -divider;
    }

    
    retval = (float)retval / divider;
    retval -=p->hallAdcConversionData.offsets[jomo];
    
    return(retval);
}

static uint32_t s_eo_appEncReader_rescale2icubdegrees(uint32_t val_raw, uint8_t jomo, eOmn_serv_mc_sensor_position_t pos)
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

    
    if(eomn_serv_mc_sensor_pos_atjoint == pos)
    {
        eOmc_joint_t *joint = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, jomo);
                
        if(NULL == joint)
        {
            return(2000);
        }
        
        /*divider = eo_common_Q17_14_to_float(joint->config.DEPRECATED_encoderconversionfactor); NO MORE NEEDED */
        divider = joint->config.jntEncoderResolution;
    }
    else if(eomn_serv_mc_sensor_pos_atmotor == pos)
    {
        eOmc_motor_t *motor = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, jomo);
                
        if(NULL == motor)
        {
            return(2000);
        }
        
        divider = motor->config.rotorEncoderResolution;
    }
    else
    {
        return(0);
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
    if(port >= eomn_serv_mc_port_none)
    {
        return(hal_spiencoder_streamNONE);
    }

    return((hal_spiencoder_stream_t)p->stream_map->encoder2stream[port]);
}



static uint32_t s_eo_read_mais_for_port(EOappEncReader *p, uint8_t port)
{
    uint32_t val_raw = ENCODER_VALUE_NOT_SUPPORTED;
    
    // get the mais status and then combine its values
    eOas_mais_t *mais = eo_entities_GetMais(eo_entities_GetHandle(), 0); 
    if(NULL == mais)
    {   // it is possible to have NULL if we call the encoder-reader before we have called eo_mais_Activate(). theus, the motion-controller must verify and activate MAIS before the encoders.
        return(0);
    }
    EOarray* array = (EOarray*)&mais->status.the15values; // even better would be to treat it as a const-array. maybe put it in constructor of the object
    
    uint8_t value1 = 0;
    uint8_t value2 = 0;
    uint8_t value3 = 0;
    uint8_t *tmp = NULL;
    if(eomn_serv_mc_port_mais_thumb_proximal == port)
    {
        tmp = (uint8_t*)eo_array_At(array, 0);
        value1 = (NULL == tmp) ? (0) : (*tmp);
        val_raw = value1;        
    }
    else if(eomn_serv_mc_port_mais_thumb_distal == port)
    {
        tmp = (uint8_t*)eo_array_At(array, 1);
        value1 = (NULL == tmp) ? (0) : (*tmp);
        tmp = (uint8_t*)eo_array_At(array, 2);
        value2 = (NULL == tmp) ? (0) : (*tmp);
        val_raw = value1 + value2;                                             
    }
    else if(eomn_serv_mc_port_mais_index_proximal == port)
    {
        tmp = (uint8_t*)eo_array_At(array, 3);
        value1 = (NULL == tmp) ? (0) : (*tmp);
        val_raw = value1;                                             
    }
    else if(eomn_serv_mc_port_mais_index_distal == port)
    {
        tmp = (uint8_t*)eo_array_At(array, 4);
        value1 = (NULL == tmp) ? (0) : (*tmp);
        tmp = (uint8_t*)eo_array_At(array, 5);
        value2 = (NULL == tmp) ? (0) : (*tmp);
        val_raw = value1 + value2;                                             
    } 
    else if(eomn_serv_mc_port_mais_medium_proximal == port)
    {
        tmp = (uint8_t*)eo_array_At(array, 6);
        value1 = (NULL == tmp) ? (0) : (*tmp);
        val_raw = value1;                                             
    }  
    else if(eomn_serv_mc_port_mais_medium_distal == port)
    {
        tmp = (uint8_t*)eo_array_At(array, 7);
        value1 = (NULL == tmp) ? (0) : (*tmp);
        tmp = (uint8_t*)eo_array_At(array, 8);
        value2 = (NULL == tmp) ? (0) : (*tmp);
        val_raw = value1 + value2;                                             
    }                 
    else if(eomn_serv_mc_port_mais_littlefingers == port)
    {
        tmp = (uint8_t*)eo_array_At(array, 11);
        value1 = (NULL == tmp) ? (0) : (*tmp);
        tmp = (uint8_t*)eo_array_At(array, 12);
        value2 = (NULL == tmp) ? (0) : (*tmp);
        tmp = (uint8_t*)eo_array_At(array, 13);
        value3 = (NULL == tmp) ? (0) : (*tmp);
        val_raw = value1 + value2 + value3;                                             
    }
    else
    {
        val_raw = ENCODER_VALUE_NOT_SUPPORTED;
    }    
    
    return(val_raw);
}


static hal_spiencoder_type_t s_eo_appEncReader_map_encodertype_to_halspiencodertype(eOmn_serv_mc_sensor_type_t encodertype)
{
    hal_spiencoder_type_t ret = hal_spiencoder_typeNONE;
    
    switch(encodertype)
    {
        case eomn_serv_mc_sensor_encoder_aea:           ret = hal_spiencoder_typeAEA;       break;
        case eomn_serv_mc_sensor_encoder_amo:           ret = hal_spiencoder_typeAMO;       break;
        case eomn_serv_mc_sensor_encoder_spichainof2:   ret = hal_spiencoder_typeCHAINof2;  break;
        default:                                        ret = hal_spiencoder_typeNONE;      break;
    }
    
    return(ret);
}


static eOresult_t s_eo_appEncReader_Diagnostics_Config(EOappEncReader *p, eo_appEncReader_diagnostics_cfg_t* cfg)
{
    if((NULL == p) || (NULL == cfg))
    {
        return(eores_NOK_nullpointer);
    }

    memcpy(&p->diagnostics.config, cfg, sizeof(eo_appEncReader_diagnostics_cfg_t));
    if(eo_ledpulser_led_none != p->diagnostics.config.errorled)
    {
        eo_ledpulser_Off(eo_ledpulser_GetHandle(), p->diagnostics.config.errorled);       
    }
    
    if((hal_gpio_portNONE != p->diagnostics.config.errorgpio.port) && (hal_gpio_pinNONE != p->diagnostics.config.errorgpio.pin))
    {
        hal_gpio_init(p->diagnostics.config.errorgpio, NULL); // NULL forces it to be output ...
        hal_gpio_setval(p->diagnostics.config.errorgpio, hal_gpio_valLOW);      
    }  

    p->diagnostics.par16 = 0;
    p->diagnostics.par64 = 0;    
    
    return(eores_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




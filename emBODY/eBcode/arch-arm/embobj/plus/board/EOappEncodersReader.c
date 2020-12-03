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

#define CHECK_ENC_IS_ON_SPI(type)                       ((eomc_enc_aea == (type)) || (eomc_enc_amo == (type)) || (eomc_enc_spichainof2 == (type)) || (eomc_enc_spichainof3 == (type)))
#define CHECK_ENC_IS_ON_STREAMED_SPI_WITHOTHERS(type)   ((eomc_enc_aea == (type)) || (eomc_enc_amo == (type)))
#define CHECK_ENC_IS_ON_STREAMED_SPI_ALONE(type)        ((eomc_enc_spichainof2 == (type)) || (eomc_enc_spichainof3 == (type)))


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    eOmc_encoder_descriptor_t*   descriptor;
    eOencoderreader_valueInfo_t* valueinfo;
} eOencoderProperties_t;   



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eObool_t s_eo_isconnected(eOmc_encoder_descriptor_t *des)
{
    return ((eomc_enc_none != (des->type)) && (eobrd_port_nolocal != (des->port))) ? eobool_true : eobool_false;
}

static void s_eo_clear_SPI_streams(EOappEncReader *p);

static void s_eo_deconfigure_SPI_encoders(EOappEncReader *p);
static eObool_t s_eo_configure_SPI_encoders(EOappEncReader *p);

static eObool_t s_eo_appEncReader_IsValidValue_AEA(uint32_t *valueraw, eOencoderreader_errortype_t *error);
static eObool_t s_eo_appEncReader_IsValidValue_SPICHAIN2(uint32_t *valueraw, eOencoderreader_errortype_t *error);
static eObool_t s_eo_appEncReader_IsValidValue_SPICHAIN3(uint32_t *valueraw, eOencoderreader_errortype_t *error);

static void s_eo_appEncReader_deconfigure_NONSPI_encoders(EOappEncReader *p);
static void s_eo_appEncReader_configure_NONSPI_encoders(EOappEncReader *p);

static uint32_t s_eo_appEncReader_rescale2icubdegrees(uint32_t val_raw, uint8_t jomo, eOmc_position_t pos);
static uint32_t s_eo_appEncReader_mais_rescale2icubdegrees(EOappEncReader* p, uint32_t val_raw, uint8_t jomo);
static uint32_t s_eo_appEncReader_psc_rescale2icubdegrees(EOappEncReader* p, int16_t val_raw);
static uint32_t s_eo_appEncReader_hallAdc_rescale2icubdegrees(EOappEncReader* p, uint32_t val_raw, uint8_t jomo);

static hal_spiencoder_stream_t s_eo_appEncReader_get_spi_stream(EOappEncReader* p, uint8_t port);


static void s_eo_appEncReader_init_halSPIencoders(EOappEncReader *p);

static void s_eo_appEncReader_deinit_halSPIencoders(EOappEncReader *p);

static void s_eo_appEncReader_anotherSPIread(void* arg);
static void s_eo_appEncReader_stopSPIread(void* arg);

static uint32_t s_eo_read_mais_for_port(EOappEncReader *p, uint8_t port);

static eObool_t s_eo_read_psc_for_port(EOappEncReader *p, eObrd_portpsc_t port, eOencoderreader_valueInfo_t *valueInfo);

static eObool_t s_eo_read_pos_for_port(EOappEncReader *p, eObrd_portpos_t port, eOencoderreader_valueInfo_t *valueInfo);

static hal_spiencoder_type_t s_eo_appEncReader_map_encodertype_to_halspiencodertype(eOmc_encoder_t encodertype);



static eOresult_t s_eo_appEncReader_Diagnostics_Config(EOappEncReader *p, eo_appEncReader_diagnostics_cfg_t* cfg);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static EOappEncReader s_eo_theappencreader =
{
    EO_INIT(.initted                ) eobool_false,
    EO_INIT(.active                 ) eobool_false,
    EO_INIT(.totalnumberofencoders  ) 0,
    EO_INIT(.stream_map             ) NULL,
    EO_INIT(.config                 ) {0},
    EO_INIT(.SPI_streams            ) {{EO_INIT(.type ) hal_spiencoder_typeNONE, EO_INIT(.numberof ) 0, EO_INIT(.maxsupported ) 0, EO_INIT(.isacquiring ) eobool_false, EO_INIT(.id ) {hal_spiencoderNONE}}},
    EO_INIT(.diagnostics            )
    {
        EO_INIT(.config ) 
        {
            EO_INIT(.jomomask       ) 0,
            EO_INIT(.periodOKreport ) 0,
            EO_INIT(.periodKOreport ) 0,
            EO_INIT(.errorled       ) eo_ledpulser_led_none,
            EO_INIT(.errorgpio      ) { EO_INIT(.port ) hal_gpio_portNONE, EO_INIT(.pin ) hal_gpio_pinNONE }
        },
        EO_INIT(.par64              ) {0},
        EO_INIT(.par16              ) {0}
    },
    EO_INIT(.maisConversionFactors   ) {1.0, 1.0, 1.0, 1.0},
    EO_INIT(.hallAdcConversionData  ) 
    {
        EO_INIT(.factors            ) {1.0f, 1.0f, 1.0f, 1.0f},
        EO_INIT(.offsets            ) {0,   0,   0,   0}
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


extern eOresult_t eo_appEncReader_Activate(EOappEncReader *p, const eOmc_arrayof_4jomodescriptors_t *arrayofjomodes)
{
    if((NULL == p) || (NULL == arrayofjomodes))
    {
        return(eores_NOK_nullpointer);
    }   

    EOconstarray* carray = eo_constarray_Load((const EOarray*)arrayofjomodes);
    
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
        const eOmc_jomo_descriptor_t *jomodes = (eOmc_jomo_descriptor_t*) eo_constarray_At(carray, i);
        p->config.jomoconfig[i].encoder1des = jomodes->encoder1;
        p->config.jomoconfig[i].encoder2des = jomodes->encoder2;
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
    if((eomc_enc_mais != this_jomoconfig.encoder1des.type) && (eomc_enc_mais != this_jomoconfig.encoder2des.type))
    {
        return(eores_NOK_unsupported);
    }
    
    p->maisConversionFactors[jomo] = convFactor;
    
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
    if (eomc_enc_absanalog != this_jomoconfig.encoder1des.type)
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
    if (eomc_enc_absanalog != this_jomoconfig.encoder1des.type)
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
    // to make the last ISR of each stream reset it at end of the chain of encoder acquistion.
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
    
    eo_appEncReader_diagnostics_cfg_t config;

    config.jomomask = 0x0f;
    config.periodOKreport = 0;
    config.periodKOreport = 1;
    config.errorled = eo_ledpulser_led_none;
    config.errorgpio.port = hal_gpio_portNONE;
    config.errorgpio.pin = hal_gpio_pinNONE;        

    
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
        
        if((0 != p->diagnostics.par16[0]) || (0 != p->diagnostics.par16[1]))
        {   // we have something wrong to signal
            
            // the gpio is put on and it stays on forever, no matter what.
            hal_gpio_setval(p->diagnostics.config.errorgpio, hal_gpio_valHIGH);  
            
            // the led is made pulsing once for 100 ms. we pulse again only if the situation stays wrong
            if(eobool_false == eo_ledpulser_IsOn(eo_ledpulser_GetHandle(), p->diagnostics.config.errorled))
            {
                eo_ledpulser_Start(eo_ledpulser_GetHandle(), p->diagnostics.config.errorled, eok_reltime100ms, 1);
            }
            
            // we transmit a diagnostics message for encoder1 and/or encoder2
            if(0 != p->diagnostics.par16[0])
            {
                errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag07);
                errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
                errdes.sourceaddress    = 0;
                errdes.par16            = p->diagnostics.par16[0];
                errdes.par64            = p->diagnostics.par64[0];
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &errdes);   
            }  
            if(0 != p->diagnostics.par16[1])
            {
                errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag07);
                errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
                errdes.sourceaddress    = 1;
                errdes.par16            = p->diagnostics.par16[1];
                errdes.par64            = p->diagnostics.par64[1];
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &errdes);   
            }             
        }
        
        // reset par16[] / par64[]
        p->diagnostics.par16[0] = p->diagnostics.par16[1] = 0;
        p->diagnostics.par64[0] = p->diagnostics.par64[1] = 0;     
        
    }
    
    return(eores_OK);
}


extern eOresult_t eo_appEncReader_GetValue(EOappEncReader *p, uint8_t jomo, eOencoderreader_valueInfo_t *valueInfo1, eOencoderreader_valueInfo_t *valueInfo2)
{   
    // reviewed by marco.accame on 07jun17 
    // changes: 
    // - return value is always OK apart form NULL params
    // - error in reading is now contained inside eOencoderreader_valueInfo_t::errortype. in case of error eOencoderreader_valueInfo_t::value is .... zero.
    // - eOencoderreader_errortype_t has gained many more values to better describe what is happening
    // - unified equal code for encoder1 and encoder2

    if((NULL == p) || (NULL == valueInfo1) || (NULL == valueInfo2))
    {
        return(eores_NOK_nullpointer);
    }
    
    
    eOencoderProperties_t encProp[2] = {NULL};
    
    encProp[0].descriptor = &p->config.jomoconfig[jomo].encoder1des;
    encProp[0].valueinfo = valueInfo1;
    
    encProp[1].descriptor = &p->config.jomoconfig[jomo].encoder2des;
    encProp[1].valueinfo = valueInfo2;
    
    for(uint8_t i=0; i<2; i++)
    {   // for each of the two encoders ....
           
        eOencoderProperties_t prop = encProp[i]; 
        uint16_t errorparam = 0;
        
        // assign composedof and position
        prop.valueinfo->composedof = eomc_encoder_get_numberofcomponents((eOmc_encoder_t)prop.descriptor->type);
        prop.valueinfo->position = (eOmc_position_t)prop.descriptor->pos;
        
        // so far we assume no errors and we assign 0 to all values
        prop.valueinfo->errortype = encreader_err_NONE;
        memset(prop.valueinfo->value, 0, sizeof(prop.valueinfo->value)); // was not 0 but: hal_NA32;
        
        // now we compute the value(s) and adjust if we detect any errors
        
        if(eobool_false == s_eo_isconnected(prop.descriptor))
        {   // the encoder is not connected: adjust its error type and continue with other encoders in the loop
            prop.valueinfo->errortype = encreader_err_NOTCONNECTED;
            continue;   // KEEP this continue! IT'S IMPORTANT!
        } 
        
        // ok, we have a connected encoder. we see what type it is and we perform the proper acquisition
        
        switch(prop.descriptor->type)
        {
            // they are eomc_enc_aea, eomc_enc_amo, eomc_enc_spichainof2, eomc_enc_spichainof3, eomc_enc_qenc, eomc_enc_absanalog, eomc_enc_mais
            
            case eomc_enc_aea:
            {
                hal_spiencoder_position_t spiRawValue = 0; 
                hal_spiencoder_errors_flags flags = {0};
                
                if(hal_res_OK == hal_spiencoder_get_value((hal_spiencoder_t)prop.descriptor->port, &spiRawValue, &flags))
                {   // ok, the hal reads correctly
                    if(eobool_true == s_eo_appEncReader_IsValidValue_AEA(&spiRawValue, &prop.valueinfo->errortype))
                    {   // the spi raw reading from hal is valid. i just need to rescale it.

                        // marco.accame: hal_spiencoder_get_value() gives back a value in uint32_t with only 18 bits of information (internally masked with 0x03FFFF).
                        // only the 12 most significant bits contain a position reading. to obtain the ticks we should do:
                        // ticks = (spiRawValue >> 6) & 0x0FFF;
                        // the resolution is now 4096 ticks per revolution.
                        
                        // GOOD VALUE:
                        uint32_t ticks = (spiRawValue >> 6) & 0x0FFF;
                        prop.valueinfo->value[0] = s_eo_appEncReader_rescale2icubdegrees(ticks, jomo, (eOmc_position_t)prop.descriptor->pos);                           
                    }
                    else
                    {   // we have a valid raw value from hal but ... it is not valid after a check                        
                        prop.valueinfo->errortype = prop.valueinfo->errortype;
                        errorparam = (spiRawValue >> 6) & 0x0FFF;                                           
                    }                    
                }
                else
                {   // we dont even have a valid reading from hal
                    prop.valueinfo->errortype = encreader_err_AEA_READING;
                    errorparam = 0xffff;                                         
                }                
               
            } break; 
            
            
            case eomc_enc_amo:
            {
                hal_spiencoder_position_t spiRawValue = 0; 
                hal_spiencoder_errors_flags flags = {0};
                
                if(hal_res_OK == hal_spiencoder_get_value((hal_spiencoder_t)prop.descriptor->port, &spiRawValue, &flags))
                {   // the spi raw reading is ok. i just need to rescale it.                   
                    //spiRawValue = (spiRawValue>>4) & 0xFFFF; marco.accame on 07jun17: why is there this comment? shall we remove it?
                    
                    // GOOD VALUE
                    prop.valueinfo->value[0] = s_eo_appEncReader_rescale2icubdegrees(spiRawValue, jomo, (eOmc_position_t)prop.descriptor->pos);
                }
                else
                {   // we dont even have a valid reading from hal .....                    
                    // marco.accame: verify if the hal for amo encoder returns error also for parity or else ... so far, i just return a generic amo error
                    prop.valueinfo->errortype = encreader_err_AMO_GENERIC;
                    errorparam = 0; // we shall expand it later on...
                }
                
            } break;


            case eomc_enc_spichainof2:
            {
                hal_spiencoder_position_t spiRawValue = 0; 
                hal_spiencoder_errors_flags flags = {0};
                
                if(hal_res_OK == hal_spiencoder_get_value((hal_spiencoder_t)prop.descriptor->port, &spiRawValue, &flags))
                {   // ok, the hal reads correctly
                    if(eobool_true == s_eo_appEncReader_IsValidValue_SPICHAIN2(&spiRawValue, &prop.valueinfo->errortype))
                    {   // the hal value is valid. i just need to rescale it.
                        
                        // GOOD VALUE
                        // marco.accame on 07jun17: why is it not rescaled to icub-degrees?
                        prop.valueinfo->value[0] = (spiRawValue <<  2) & 0xfff0; // it is the first encoder in the chain
                        prop.valueinfo->value[1] = (spiRawValue >> 14) & 0xfff0; // it is the second encoder in the chain                        
                    }
                    else
                    {   // we have a valid value from hal but ... it is not valid after a check                    
                        prop.valueinfo->errortype = prop.valueinfo->errortype;  
                        errorparam = spiRawValue & 0xffff;
                    }                    
                }
                else
                {   // we dont even have a valid reading from hal .....                    
                    prop.valueinfo->errortype = encreader_err_SPICHAINOF2_GENERIC;
                    errorparam = 0xffff;
                }                
                
            } break;
 

            case eomc_enc_spichainof3:
            {
                hal_spiencoder_position_t arrayof3[3] = {0};
                hal_spiencoder_errors_flags flags = {0};
                
                if(hal_res_OK == hal_spiencoder_get_value((hal_spiencoder_t)prop.descriptor->port, arrayof3, &flags))
                {   // ok, the hal reads correctly
                    if(eobool_true == s_eo_appEncReader_IsValidValue_SPICHAIN3(arrayof3, &prop.valueinfo->errortype))
                    {   // the hal value is valid. i just need to rescale it.
                        
                        // GOOD VALUE
                        // marco.accame on 07jun17: why is it not rescaled to icub-degrees?
                        prop.valueinfo->value[0] = (arrayof3[0] << 2) & 0xfff0; // it is the first encoder in the chain
                        prop.valueinfo->value[1] = (arrayof3[1] << 2) & 0xfff0; // it is the second encoder in the chain
                        prop.valueinfo->value[2] = (arrayof3[2] << 2) & 0xfff0; // it is the third encoder in the chain                         
                    }
                    else
                    {   // we have a valid value from hal but ... it is not valid after a check                    
                        prop.valueinfo->errortype = prop.valueinfo->errortype; 
                        errorparam = arrayof3[0] & 0xffff;                        
                    }                    
                }
                else
                {   // we dont even have a valid reading from hal .....                    
                    prop.valueinfo->errortype = encreader_err_SPICHAINOF3_GENERIC;
                    errorparam = 0xffff;
                }                
                
            } break;
            

            case eomc_enc_qenc:
            {
                uint32_t val = hal_quadencoder_get_counter((hal_quadencoder_t)prop.descriptor->port);
                
                if(hal_NA32 != val)
                {   // the hal reading is ok. i just need to rescale it. 
                    // GOOD VALUE
                    prop.valueinfo->value[0] = s_eo_appEncReader_rescale2icubdegrees(val, jomo, (eOmc_position_t)prop.descriptor->pos);
                } 
                else
                {   // the hal has detected a problem
                    prop.valueinfo->errortype = encreader_err_QENC_GENERIC;
                    errorparam = val & 0xffff;
                }   
                
            } break;    

            
            case eomc_enc_absanalog:
            {
                uint32_t val = hal_adc_get_hall_sensor_analog_input_mV((uint8_t)prop.descriptor->port);      
                
                if(hal_NA32 != val)
                {   // the hal reading is ok. it is the voltage from the motor port (0 - 3300mV). i just need to rescale it
                    // GOOD VALUE
                    prop.valueinfo->value[0] = s_eo_appEncReader_hallAdc_rescale2icubdegrees(p, val, jomo);                        
                }  
                else 
                {   // the hal has detected a problem
                     prop.valueinfo->errortype = encreader_err_ABSANALOG_GENERIC;  
                     errorparam = val & 0xffff;                    
                }              
               
            } break;  
            

            case eomc_enc_mais:
            {

                uint32_t val = s_eo_read_mais_for_port(p, prop.descriptor->port); 
  
                //#warning marco.accame on 07jun17: when it fails s_eo_read_mais_for_port() return either hal_NA32 or 0. what is to be evaluated?
                if(hal_NA32 != val)
                {   // the hal reading is ok. it may be also 0. i just need to rescale it
                    // GOOD VALUE
                    prop.valueinfo->value[0] = s_eo_appEncReader_mais_rescale2icubdegrees(p, val, jomo);                 
                }                     
                else
                {   // the port is not correct for a mais.
                    prop.valueinfo->errortype = encreader_err_MAIS_GENERIC;
                    errorparam = val & 0xffff;                    
                }                           
               
            } break;   
            
            case eomc_enc_psc:
            {
                eObool_t ret = s_eo_read_psc_for_port(p, (eObrd_portpsc_t)prop.descriptor->port, prop.valueinfo); 
  
                if(eobool_false == ret)
                {   // the port is not correct for a PSC.
                    prop.valueinfo->errortype = encreader_err_PSC_GENERIC;              
                }                           
               
            } break;    


            case eomc_enc_pos:
            {
                eObool_t ret = s_eo_read_pos_for_port(p, (eObrd_portpos_t)prop.descriptor->port, prop.valueinfo); 
  
                if(eobool_false == ret)
                {   // the port is not correct for a POS.
                    prop.valueinfo->errortype = encreader_err_POS_GENERIC;              
                }                           
               
            } break;    
            
            default:
            {   // we have not recognised any valid encoder type
                prop.valueinfo->errortype = encreader_err_GENERIC;   
                errorparam = 0;                
            } break;             
              
            
        }
        
        // now we see if there is any diagnostics to send up. we eval the errortype

        eObool_t filldiagnostics = eo_common_byte_bitcheck(p->diagnostics.config.jomomask, jomo);        
        if(eobool_true == filldiagnostics)
        {   
            switch(prop.valueinfo->errortype)
            {
                case encreader_err_NONE:
                case encreader_err_NOTCONNECTED:
                {   // we do nothing because reading is ok or is not done
                } break;
                
                default:
                case encreader_err_GENERIC:
                {   // we dont know what is happening ... we just set the flag in par16[i].
                    p->diagnostics.par16[i] |= (encreader_err_GENERIC<<(4*jomo));       // shift by nibbles ..                    
                } break;
                
                case encreader_err_AEA_READING:               
                case encreader_err_AEA_PARITY:
                case encreader_err_AEA_CHIP:
                case encreader_err_QENC_GENERIC:
                case encreader_err_ABSANALOG_GENERIC:
                case encreader_err_MAIS_GENERIC:
                case encreader_err_PSC_GENERIC:
                case encreader_err_POS_GENERIC:
                case encreader_err_AMO_GENERIC:
                case encreader_err_SPICHAINOF2_GENERIC:
                case encreader_err_SPICHAINOF3_GENERIC:
                {   // in such cases, we report the errortype and the errorparam that someone has prepared 
                    p->diagnostics.par16[i] |= (prop.valueinfo->errortype<<(4*jomo));   // shift by nibbles ..
                    p->diagnostics.par64[i] &= (errorparam<<(16*jomo));                 // shift by two bytes              
                } break;                                
            }            
        }
        
        // ok, we now go to next encoder or ... we terminate the for() loop
        
    } // for()
        
    
    // now the return value. we return always OK          
    return eores_OK;
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
        
        // if in the joint i-th we have any spi-based encoder which is primary or value2, then we put it into the proper spi-stream        

        if(CHECK_ENC_IS_ON_STREAMED_SPI_WITHOTHERS(jdes->encoder1des.type))
        {    
            uint8_t port = jdes->encoder1des.port;
            hal_spiencoder_stream_t streamnumber = s_eo_appEncReader_get_spi_stream(p, port);
            if(hal_spiencoder_streamNONE != streamnumber)
            {    
                p->SPI_streams[streamnumber].type = s_eo_appEncReader_map_encodertype_to_halspiencodertype((eOmc_encoder_t)jdes->encoder1des.type);   
                p->SPI_streams[streamnumber].maxsupported = p->stream_map->stream2numberofencoders[streamnumber];
                // i must check if there is already encoder specified by jdes->encoder1des.port inside ... if already, then we dont add it.
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

        if(CHECK_ENC_IS_ON_STREAMED_SPI_WITHOTHERS(jdes->encoder2des.type))
        {   
            uint8_t port = jdes->encoder2des.port;
            hal_spiencoder_stream_t streamnumber = s_eo_appEncReader_get_spi_stream(p, port);
            if(hal_spiencoder_streamNONE != streamnumber)
            {    
                p->SPI_streams[streamnumber].type = s_eo_appEncReader_map_encodertype_to_halspiencodertype((eOmc_encoder_t)jdes->encoder2des.type);   
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
        
        if(CHECK_ENC_IS_ON_STREAMED_SPI_ALONE(jdes->encoder1des.type))
        { 
            uint8_t port = jdes->encoder1des.port;
            hal_spiencoder_stream_t streamnumber = s_eo_appEncReader_get_spi_stream(p, port);
            if(hal_spiencoder_streamNONE != streamnumber)
            {    
                p->SPI_streams[streamnumber].type = s_eo_appEncReader_map_encodertype_to_halspiencodertype((eOmc_encoder_t)jdes->encoder1des.type);   
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

        if(CHECK_ENC_IS_ON_STREAMED_SPI_ALONE(jdes->encoder2des.type))
        {  
            uint8_t port = jdes->encoder2des.port;            
            hal_spiencoder_stream_t streamnumber = s_eo_appEncReader_get_spi_stream(p, port);
            if(hal_spiencoder_streamNONE != streamnumber)
            {    
                p->SPI_streams[streamnumber].type = s_eo_appEncReader_map_encodertype_to_halspiencodertype((eOmc_encoder_t)jdes->encoder2des.type);   
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
	hal_spiencoder_cfg_t config;
    config.priority = hal_int_priorityNONE;
    config.callback_on_rx = NULL; 
    config.arg = NULL;;
    config.reg_address = 0;
    config.sdata_precheck = hal_false;
    
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
        else if(hal_spiencoder_typeCHAINof3 == thestream->type)
        {
            config.priority = hal_int_priority05; 
            config.callback_on_rx = s_eo_appEncReader_stopSPIread; 
            config.arg = (void*) thestream; 
            config.type = hal_spiencoder_typeCHAINof3; 
            config.reg_address = 0; 
            config.sdata_precheck = hal_false;
            
            hal_spiencoder_init(thestream->id[0], &config);
            // if the stream is chainof3 we init only one encoder
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


static eObool_t s_eo_appEncReader_IsValidValue_AEA(uint32_t *valueraw, eOencoderreader_errortype_t *error)
{
    uint8_t parity_error = 0;
    uint8_t b = 0;
    
    *error = encreader_err_NONE;
    
    for (b=0; b<18; ++b)
    {
        parity_error ^= (*valueraw)>>b;
    }
    
    if (parity_error & 1) 
    { 
        *error = encreader_err_AEA_PARITY;
        return(eobool_false);
    }
    
    if ((0x38 & *valueraw) != 0x20)
    {
        *error = encreader_err_AEA_CHIP;
        return(eobool_false);

    }
    
    return(eobool_true);
}


static eObool_t s_eo_appEncReader_IsValidValue_SPICHAIN2(uint32_t *valueraw, eOencoderreader_errortype_t *error)
{    
    uint16_t first = *valueraw & 0xffff;
    uint16_t second = ((*valueraw) >> 16) & 0xffff;   
    
    *error = encreader_err_NONE;
       
    if(1 == (eo_common_hlfword_bitsetcount(first) % 2))
    { 
        *error = encreader_err_AEA_PARITY;
        return(eobool_false);
    }
    
    if(1 == (eo_common_hlfword_bitsetcount(second) % 2))
    { 
        *error = encreader_err_AEA_PARITY;
        return(eobool_false);
    }  

    if(eobool_true == eo_common_hlfword_bitcheck(first, 1))
    {
        *error = encreader_err_AEA_READING;
        return(eobool_false);    
    }
    
    if(eobool_true == eo_common_hlfword_bitcheck(second, 1))
    {
        *error = encreader_err_AEA_READING;
        return(eobool_false);    
    }    
    
    return(eobool_true);
}

static eObool_t s_eo_appEncReader_IsValidValue_SPICHAIN3(uint32_t *valueraw, eOencoderreader_errortype_t *error)
{    
    uint16_t first = valueraw[0] & 0xffff;
    uint16_t second = valueraw[1] & 0xffff;   
    uint16_t third = valueraw[2] & 0xffff;   
    
    *error = encreader_err_NONE;
       
    if(1 == (eo_common_hlfword_bitsetcount(first) % 2))
    { 
        *error = encreader_err_AEA_PARITY;
        return(eobool_false);
    }
    
    if(1 == (eo_common_hlfword_bitsetcount(second) % 2))
    { 
        *error = encreader_err_AEA_PARITY;
        return(eobool_false);
    }  

    if(1 == (eo_common_hlfword_bitsetcount(third) % 2))
    { 
        *error = encreader_err_AEA_PARITY;
        return(eobool_false);
    }  
    
    if(eobool_true == eo_common_hlfword_bitcheck(first, 1))
    {
        *error = encreader_err_AEA_READING;
        return(eobool_false);    
    }
    
    if(eobool_true == eo_common_hlfword_bitcheck(second, 1))
    {
        *error = encreader_err_AEA_READING;
        return(eobool_false);    
    }    

    if(eobool_true == eo_common_hlfword_bitcheck(third, 1))
    {
        *error = encreader_err_AEA_READING;
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
        
        if(jmcfg->encoder1des.type == eomc_enc_qenc)
        {  
            hal_quadencoder_init((hal_quadencoder_t)jmcfg->encoder1des.port);            
        }

        
        if(jmcfg->encoder2des.type == eomc_enc_qenc)
        { 
            hal_quadencoder_init((hal_quadencoder_t)jmcfg->encoder2des.port);           
        }
        
        // marco.accame on 3 dec 2020: i see that in here it is manages only the case eomc_enc_qenc. 
        // but there are others ...
        
        // for eomc_enc_roie, eomc_enc_absanalog, eomc_enc_hallmotor
        // marco.accame on 3 dec 2020: well, so far nothing was done so i assume it is ok.
        
        // for eomc_enc_mais, eomc_enc_psc, eomc_enc_pos 
        // we verify, activate, start with the proper service. that should be done elsewhere ...              
    }
}

static uint32_t s_eo_appEncReader_mais_rescale2icubdegrees(EOappEncReader* p, uint32_t val_raw, uint8_t jomo)
{
    uint32_t retval = val_raw;
    
    float divider = p->maisConversionFactors[jomo];
    
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


static uint32_t s_eo_appEncReader_psc_rescale2icubdegrees(EOappEncReader* p, int16_t val_raw)
{
    #warning currently we don't use the encoder factor for psc
    //currently the user can't configure the conversion factor of psc.
    //The psc boards send dec degree value in int16
    
    return EOMEAS_DEG2IDG(((float)(val_raw))/10);
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

static uint32_t s_eo_appEncReader_rescale2icubdegrees(uint32_t val_raw, uint8_t jomo, eOmc_position_t pos)
{

    // this is the correct code: we divide by the encoderconversionfactor ...
    // formulas are:
    // in xml file there is GENERAL:Encoders = tidegconv = 182.044 = (64*1024/360) is the conversion from degrees to icubdeg and is expressed as [icubdeg/deg]
    // In joint->config.jntEncoderResolution and motor->config.rotorEncoderResolution there are the resolutions of joint and motor encoders,
    // that is number of ticks per round angle.
    // 
    // Thus, to obtain the icub-degress in here we must divide the reading of the encoder expressed in [ticks] by
    // divider and multiply for 65535. (divider is joint->config.jntEncoderResolution or motor->config.rotorEncoderResolution)

    // moreover .... if the encoderconversionfactor is negative, then i assume it is positive. because its sign is managed internally in the ems-controller


    uint32_t retval = val_raw;
    int32_t divider = 1;

    
    if(eomc_pos_atjoint == pos)
    {
        eOmc_joint_t *joint = (eOmc_joint_t*) eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, jomo);
                
        if(NULL == joint)
        {
            return(2000);
        }
        
        divider = joint->config.jntEncoderResolution;
    }
    else if(eomc_pos_atmotor == pos)
    {
        eOmc_motor_t *motor = (eOmc_motor_t*) eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, jomo);
                
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

    uint64_t aux = (uint64_t)val_raw* 65535;
    
    retval = aux /divider;
    return(retval);

}


static hal_spiencoder_stream_t s_eo_appEncReader_get_spi_stream(EOappEncReader* p, uint8_t port)
{
    if(port >= eobrd_port_none)
    {
        return(hal_spiencoder_streamNONE);
    }

    return((hal_spiencoder_stream_t)p->stream_map->encoder2stream[port]);
}


// it returns hal_NA32 if ... port is not valid. it returns 0 if we dont have values from the MAIS
static uint32_t s_eo_read_mais_for_port(EOappEncReader *p, uint8_t port)
{
    uint32_t val_raw = hal_NA32;
    
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
    
    if(eobrd_portmais_thumbproximal == port)
    {
        tmp = (uint8_t*)eo_array_At(array, 0);
        value1 = (NULL == tmp) ? (0) : (*tmp);
        val_raw = value1;        
    }
    else if(eobrd_portmais_thumbdistal == port)
    {
        tmp = (uint8_t*)eo_array_At(array, 1);
        value1 = (NULL == tmp) ? (0) : (*tmp);
        tmp = (uint8_t*)eo_array_At(array, 2);
        value2 = (NULL == tmp) ? (0) : (*tmp);
        val_raw = value1 + value2;                                             
    }
    else if(eobrd_portmais_indexproximal == port)
    {
        tmp = (uint8_t*)eo_array_At(array, 3);
        value1 = (NULL == tmp) ? (0) : (*tmp);
        val_raw = value1;                                             
    }
    else if(eobrd_portmais_indexdistal == port)
    {
        tmp = (uint8_t*)eo_array_At(array, 4);
        value1 = (NULL == tmp) ? (0) : (*tmp);
        tmp = (uint8_t*)eo_array_At(array, 5);
        value2 = (NULL == tmp) ? (0) : (*tmp);
        val_raw = value1 + value2;                                             
    } 
    else if(eobrd_portmais_mediumproximal == port)
    {
        tmp = (uint8_t*)eo_array_At(array, 6);
        value1 = (NULL == tmp) ? (0) : (*tmp);
        val_raw = value1;                                             
    }  
    else if(eobrd_portmais_mediumdistal == port)
    {
        tmp = (uint8_t*)eo_array_At(array, 7);
        value1 = (NULL == tmp) ? (0) : (*tmp);
        tmp = (uint8_t*)eo_array_At(array, 8);
        value2 = (NULL == tmp) ? (0) : (*tmp);
        val_raw = value1 + value2;                                             
    }                 
    else if(eobrd_portmais_littlefingers == port)
    {
        tmp = (uint8_t*)eo_array_At(array, 14);
        value1 = (NULL == tmp) ? (0) : (*tmp);
        tmp = (uint8_t*)eo_array_At(array, 12);
        value2 = (NULL == tmp) ? (0) : (*tmp);
        tmp = (uint8_t*)eo_array_At(array, 13);
        value3 = (NULL == tmp) ? (0) : (*tmp);
        val_raw = value1 + value2 + value3;                                             
    }
    else
    {
        val_raw = hal_NA32;
    }    
    
    return(val_raw);
}


static eObool_t s_eo_read_psc_for_port(EOappEncReader *p, eObrd_portpsc_t port, eOencoderreader_valueInfo_t *valueInfo)
{
    eObool_t ret=eobool_false;

    // get the psc status and then read its values
    eOas_psc_t *psc = eo_entities_GetPSC(eo_entities_GetHandle(), 0); 
    if(NULL == psc)
    {   // it is possible to have NULL if we call EOtheEncoderReader before we have called eo_psc_Activate(). 
        // thus, EOtheMotionController must verify and activate PSC service before the encoders.
        return(ret);
    }
    
    eOas_psc_arrayof_data_t* array = &psc->status.arrayofdata;
    
    if(array->head.size != eOas_psc_data_maxnumber)
    {
        return(ret); 
        // VALE: are you sure that if no readings the array is empty? 
        // marco.accame: 
        // the array must have size = eOas_psc_data_maxnumber because inside eoprot_fun_INIT_as_psc_status() defined in EOthePSC.c 
        // we init the array to have that size, that capacity and all items = 0. Then the size is never changed.
    }
    
    if(eobrd_portpsc_finger0 == port)
    {
        valueInfo->value[0] = s_eo_appEncReader_psc_rescale2icubdegrees(p, array->data[0].value);
        valueInfo->value[1] = s_eo_appEncReader_psc_rescale2icubdegrees(p, array->data[1].value);
        valueInfo->value[2] = s_eo_appEncReader_psc_rescale2icubdegrees(p, array->data[2].value);
        valueInfo->value[3] = s_eo_appEncReader_psc_rescale2icubdegrees(p, array->data[3].value);
        valueInfo->composedof=4;
        ret=eobool_true;
    }
    else if(eobrd_portpsc_finger1 == port) //thumb
    {
        valueInfo->value[0] = s_eo_appEncReader_psc_rescale2icubdegrees(p, array->data[4].value);
        valueInfo->value[1] = s_eo_appEncReader_psc_rescale2icubdegrees(p, array->data[5].value);
        valueInfo->value[2] = 0;
        valueInfo->value[3] = 0;
        valueInfo->composedof=2;
        ret=eobool_true;
    }
    
    return ret;
}


static eObool_t s_eo_read_pos_for_port(EOappEncReader *p, eObrd_portpos_t port, eOencoderreader_valueInfo_t *valueInfo)
{
    eObool_t ret = eobool_false;
    
    valueInfo->value[0] = 0;
    valueInfo->composedof = 1;

    // get the pos status and then read its values
    eOas_pos_t *pos = eo_entities_GetPOS(eo_entities_GetHandle(), 0); 
    if(NULL == pos)
    {   // it is possible to have NULL if we call the encoder-reader before we have called eo_mais_Activate(). theus, the motion-controller must verify and activate MAIS before the encoders.
        return(ret);
    }
    
    EOarray *array = (EOarray*)&pos->status.arrayofdata;
    
    // with that i verify that the array has valid items, 
    // so that eo_array_At(array, n) w/ n = [0, eOas_pos_data_maxnumber-1] shall always return a valid pointer
    // i am sure that the following is always false because the function eoprot_fun_INIT_as_pos_status() 
    // [defined inside EOthePOS.c and called at initialization of the endpoint analogsensors inside EOtheServices.c with eo_nvset_LoadEP()]
    // inits the array to have capacity = size = eOas_pos_data_maxnumber
    if(eo_array_Size(array) != eOas_pos_data_maxnumber)
    {
        return ret;
    }
           
    switch(port)
    {
        case eobrd_portpos_hand_thumb:
        {
            eOas_pos_data_t *data0 = (eOas_pos_data_t*) eo_array_At(array, 0);
            if(NULL != data0)
            {
                valueInfo->value[0] = s_eo_appEncReader_psc_rescale2icubdegrees(p, data0->value);
                valueInfo->composedof = 1;
                ret = eobool_true; 
            }                
        } break;

        case eobrd_portpos_hand_index:
        {
            eOas_pos_data_t *data1 = (eOas_pos_data_t*) eo_array_At(array, 1);
            if(NULL != data1)
            {
                valueInfo->value[0] = s_eo_appEncReader_psc_rescale2icubdegrees(p, data1->value);
                valueInfo->composedof = 1;
                ret = eobool_true; 
            }                
        } break;    

        case eobrd_portpos_hand_medium:
        {
            eOas_pos_data_t *data2 = (eOas_pos_data_t*) eo_array_At(array, 2);
            if(NULL != data2)
            {
                valueInfo->value[0] = s_eo_appEncReader_psc_rescale2icubdegrees(p, data2->value);
                valueInfo->composedof = 1;
                ret = eobool_true;  
            }                
        } break;      

        case eobrd_portpos_hand_pinky:
        {
            eOas_pos_data_t *data3 = (eOas_pos_data_t*) eo_array_At(array, 3);
            if(NULL != data3)
            {
                valueInfo->value[0] = s_eo_appEncReader_psc_rescale2icubdegrees(p, data3->value);
                valueInfo->composedof = 1;
                ret = eobool_true; 
            }                
        } break;  

        default:
        {
            valueInfo->value[0] = 0;    
            valueInfo->composedof = 0;  
            ret = eobool_false;            
        } break;        
    }

    return ret;
}



static hal_spiencoder_type_t s_eo_appEncReader_map_encodertype_to_halspiencodertype(eOmc_encoder_t encodertype)
{
    hal_spiencoder_type_t ret = hal_spiencoder_typeNONE;
    
    switch(encodertype)
    {
        case eomc_enc_aea:              ret = hal_spiencoder_typeAEA;       break;
        case eomc_enc_amo:              ret = hal_spiencoder_typeAMO;       break;
        case eomc_enc_spichainof2:      ret = hal_spiencoder_typeCHAINof2;  break;
        case eomc_enc_spichainof3:      ret = hal_spiencoder_typeCHAINof3;  break;
        default:                        ret = hal_spiencoder_typeNONE;      break;
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

    // reset par16[] / par64[]
    p->diagnostics.par16[0] = p->diagnostics.par16[1] = 0;
    p->diagnostics.par64[0] = p->diagnostics.par64[1] = 0;  

    return(eores_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




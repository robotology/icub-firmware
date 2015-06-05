/*  @file       EOemsControllerCfg.c
    @brief      This file implements functions to retrieve constant factors used by the Controller and depending on the joint encoder
    @author     davide.pollarolo@iit.it
    @date       06/05/2015
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EoMCConfigurations.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOemsControllerCfg.h"


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    int32_t ticks;
    eo_appEncReader_enc_type_t enc_type;
}   eo_emsControllerCfg_ticksencoder_t;     EO_VERIFYsizeof(eo_emsControllerCfg_ticksencoder_t, 8);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

const eo_emsControllerCfg_ticksencoder_t ticks_per_revolution[] =
{
    {65536, eo_appEncReader_enc_type_AEA},  //AEA
    {0,     eo_appEncReader_enc_type_AMO},  //AMO
    {28672, eo_appEncReader_enc_type_INC},  //INC
    {0,     eo_appEncReader_enc_type_ADH}   //ADH
};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

// These interface are common for all the controllers object

//get ticks per revolution depending on the encoder type
extern int32_t joint2ticksperrevolution (uint8_t joint_n)
{
    if (joint_n >= MAX_NAXLES)
        return 0;
        
    eOmcconfig_code_t code = eOmcconfig_Get_Active_Code();
    if (code == eOmcconfig_code_dummy)
        return 0;
    
    const eOmcconfig_jomo_cfg_t* jomoscfg = eOmcconfig_code2config(code);
    
    if (jomoscfg != NULL)
    {
        if( jomoscfg[joint_n].encoder.etype >= (sizeof(ticks_per_revolution)/sizeof(eo_emsControllerCfg_ticksencoder_t)))
            return 0;
        
        return ticks_per_revolution[jomoscfg[joint_n].encoder.etype].ticks;
    }
    return 0;
}

//get the encoder type given a joint number
extern eo_appEncReader_enc_type_t joint2encodertype (uint8_t joint_n)
{
    if (joint_n >= MAX_NAXLES)
        return eo_appEncReader_enc_type_NONE;
    
    eOmcconfig_code_t code = eOmcconfig_Get_Active_Code();
    if (code == eOmcconfig_code_dummy)
        return eo_appEncReader_enc_type_NONE;
    
    const eOmcconfig_jomo_cfg_t* jomoscfg = eOmcconfig_code2config(code);
    
    if (jomoscfg != NULL)
    {
       if( jomoscfg[joint_n].encoder.etype >= (sizeof(ticks_per_revolution)/sizeof(eo_emsControllerCfg_ticksencoder_t)))
            return eo_appEncReader_enc_type_NONE;
       
       return (eo_appEncReader_enc_type_t)jomoscfg[joint_n].encoder.etype;
    }
    
    return eo_appEncReader_enc_type_NONE;
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

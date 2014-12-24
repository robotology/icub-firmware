/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

#include "hal_led.h"
#include "EOtheLEDpulser.h"
#include "EOtheErrorManager.h"

#include "EOVtheSystem.h"

#include "EOMtheEMSappl_hid.h"
#include "EOMtheEMSapplCfg.h"

#include "EOtheEMSapplBody.h"

#include "EOtheEMSapplDiagnostics.h"
#include "EOaction_hid.h"
#include "hl_bits.h"

#include "EOMtheEMSapplCfg_cfg.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


#if     defined(EOMTHEEMSAPPLCFG_USE_EB2) || defined(EOMTHEEMSAPPLCFG_USE_EB4)
    #define EOMTHEEMSAPPLCFG_EBX_hasSKIN    eobool_true
    #define EOMTHEEMSAPPLCFG_EBX_hasMC4     eobool_true
    #define EOMTHEEMSAPPLCFG_EBX_has2FOC    eobool_false
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB1) || defined(EOMTHEEMSAPPLCFG_USE_EB3) || defined(EOMTHEEMSAPPLCFG_USE_EB5) || defined(EOMTHEEMSAPPLCFG_USE_EB6) || defined(EOMTHEEMSAPPLCFG_USE_EB7) || defined(EOMTHEEMSAPPLCFG_USE_EB8) || defined(EOMTHEEMSAPPLCFG_USE_EB9)
    #define EOMTHEEMSAPPLCFG_EBX_hasSKIN    eobool_false
    #define EOMTHEEMSAPPLCFG_EBX_hasMC4     eobool_false
    #define EOMTHEEMSAPPLCFG_EBX_has2FOC    eobool_true
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB10) || defined(EOMTHEEMSAPPLCFG_USE_EB11)
    #define EOMTHEEMSAPPLCFG_EBX_hasSKIN    eobool_true
    #define EOMTHEEMSAPPLCFG_EBX_hasMC4     eobool_false
    #define EOMTHEEMSAPPLCFG_EBX_has2FOC    eobool_false
#else
    #error --> you must define an EBx
#endif


#if     defined(EOMTHEEMSAPPLCFG_USE_EB1) || defined(EOMTHEEMSAPPLCFG_USE_EB3)  || defined(EOMTHEEMSAPPLCFG_USE_EB6)  || defined(EOMTHEEMSAPPLCFG_USE_EB8)
        #define EOMTHEEMSAPPLCFG_EBX_encodersMASK   (   (1 << eOeOappEncReader_encoder0) |  \
                                                        (1 << eOeOappEncReader_encoder1) |  \
                                                        (1 << eOeOappEncReader_encoder2) |  \
                                                        (1 << eOeOappEncReader_encoder3) |  \
                                                        (0 << eOeOappEncReader_encoder4) |  \
                                                        (0 << eOeOappEncReader_encoder5)    \
                                                    )
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB7) || defined(EOMTHEEMSAPPLCFG_USE_EB9)
        #define EOMTHEEMSAPPLCFG_EBX_encodersMASK   (   (1 << eOeOappEncReader_encoder0) |  \
                                                        (1 << eOeOappEncReader_encoder1) |  \
                                                        (0 << eOeOappEncReader_encoder2) |  \
                                                        (0 << eOeOappEncReader_encoder3) |  \
                                                        (0 << eOeOappEncReader_encoder4) |  \
                                                        (0 << eOeOappEncReader_encoder5)    \
                                                    )
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB5)
        #define EOMTHEEMSAPPLCFG_EBX_encodersMASK   (   (1 << eOeOappEncReader_encoder0) |  \
                                                        (1 << eOeOappEncReader_encoder1) |  \
                                                        (1 << eOeOappEncReader_encoder2) |  \
                                                        (0 << eOeOappEncReader_encoder3) |  \
                                                        (0 << eOeOappEncReader_encoder4) |  \
                                                        (0 << eOeOappEncReader_encoder5)    \
                                                    )
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB2) || defined(EOMTHEEMSAPPLCFG_USE_EB4) || defined(EOMTHEEMSAPPLCFG_USE_EB10) || defined(EOMTHEEMSAPPLCFG_USE_EB11)
        #define EOMTHEEMSAPPLCFG_EBX_encodersMASK   (   (0 << eOeOappEncReader_encoder0) |  \
                                                        (0 << eOeOappEncReader_encoder1) |  \
                                                        (0 << eOeOappEncReader_encoder2) |  \
                                                        (0 << eOeOappEncReader_encoder3) |  \
                                                        (0 << eOeOappEncReader_encoder4) |  \
                                                        (0 << eOeOappEncReader_encoder5)    \
                                                    )

#else
    #error --> you must define an EBx
#endif


// encoders
#if     defined(EOMTHEEMSAPPLCFG_USE_EB1) || defined(EOMTHEEMSAPPLCFG_USE_EB3)  || defined(EOMTHEEMSAPPLCFG_USE_EB6)  || defined(EOMTHEEMSAPPLCFG_USE_EB8)
 
    #define encstream0_type         hal_encoder_type1
    #define encstream0_numberof     2
    #define encstream0_encoders0    hal_encoder1
    #define encstream0_encoders1    hal_encoder3
    #define encstream0_encoders2    hal_encoderNONE
    #define encstream0_encoders3    hal_encoderNONE
    #define encstream0_encoders4    hal_encoderNONE
    #define encstream0_encoders5    hal_encoderNONE

    
    #define encstream1_type         hal_encoder_type1
    #define encstream1_numberof     2
    #define encstream1_encoders0    hal_encoder2
    #define encstream1_encoders1    hal_encoder4
    #define encstream1_encoders2    hal_encoderNONE
    #define encstream1_encoders3    hal_encoderNONE
    #define encstream1_encoders4    hal_encoderNONE
    #define encstream1_encoders5    hal_encoderNONE

#elif   defined(EOMTHEEMSAPPLCFG_USE_EB7) || defined(EOMTHEEMSAPPLCFG_USE_EB9)

    #define encstream0_type         hal_encoder_type1
    #define encstream0_numberof     1
    #define encstream0_encoders0    hal_encoder1
    #define encstream0_encoders1    hal_encoderNONE
    #define encstream0_encoders2    hal_encoderNONE
    #define encstream0_encoders3    hal_encoderNONE
    #define encstream0_encoders4    hal_encoderNONE
    #define encstream0_encoders5    hal_encoderNONE

    
    #define encstream1_type         hal_encoder_type1
    #define encstream1_numberof     1
    #define encstream1_encoders0    hal_encoder2
    #define encstream1_encoders1    hal_encoderNONE
    #define encstream1_encoders2    hal_encoderNONE
    #define encstream1_encoders3    hal_encoderNONE
    #define encstream1_encoders4    hal_encoderNONE
    #define encstream1_encoders5    hal_encoderNONE
    
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB5)

    #define encstream0_type         hal_encoder_type1
    #define encstream0_numberof     2
    #define encstream0_encoders0    hal_encoder1
    #define encstream0_encoders1    hal_encoder3
    #define encstream0_encoders2    hal_encoderNONE
    #define encstream0_encoders3    hal_encoderNONE
    #define encstream0_encoders4    hal_encoderNONE
    #define encstream0_encoders5    hal_encoderNONE

    
    #define encstream1_type         hal_encoder_type1
    #define encstream1_numberof     1
    #define encstream1_encoders0    hal_encoder2
    #define encstream1_encoders1    hal_encoderNONE
    #define encstream1_encoders2    hal_encoderNONE
    #define encstream1_encoders3    hal_encoderNONE
    #define encstream1_encoders4    hal_encoderNONE
    #define encstream1_encoders5    hal_encoderNONE
                                                    
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB2) || defined(EOMTHEEMSAPPLCFG_USE_EB4) || defined(EOMTHEEMSAPPLCFG_USE_EB10) || defined(EOMTHEEMSAPPLCFG_USE_EB11)

    #define encstream0_type         hal_encoder_type1
    #define encstream0_numberof     0
    #define encstream0_encoders0    hal_encoderNONE
    #define encstream0_encoders1    hal_encoderNONE
    #define encstream0_encoders2    hal_encoderNONE
    #define encstream0_encoders3    hal_encoderNONE
    #define encstream0_encoders4    hal_encoderNONE
    #define encstream0_encoders5    hal_encoderNONE

    
    #define encstream1_type         hal_encoder_type1
    #define encstream1_numberof     0
    #define encstream1_encoders0    hal_encoderNONE
    #define encstream1_encoders1    hal_encoderNONE
    #define encstream1_encoders2    hal_encoderNONE
    #define encstream1_encoders3    hal_encoderNONE
    #define encstream1_encoders4    hal_encoderNONE
    #define encstream1_encoders5    hal_encoderNONE
                                                    

#else
    #error --> you must define an EBx
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

static const eOemsapplbody_cfg_t theemsapplbodycfg =
{
    .encoderstream0 =
    {
        .type       = encstream0_type,
        .numberof   = encstream0_numberof,
        .encoders   = { encstream0_encoders0, encstream0_encoders1, encstream0_encoders2, encstream0_encoders3, encstream0_encoders4, encstream0_encoders5 }        
    },
    .encoderstream1 =
    {
        .type       = encstream1_type,
        .numberof   = encstream1_numberof,
        .encoders   = { encstream1_encoders0, encstream1_encoders1, encstream1_encoders2, encstream1_encoders3, encstream1_encoders4, encstream1_encoders5 }        
    },
    .hasdevice  =
    {
        EOMTHEEMSAPPLCFG_EBX_hasSKIN, EOMTHEEMSAPPLCFG_EBX_hasMC4, EOMTHEEMSAPPLCFG_EBX_has2FOC
    },
    .icubcanprotoimplementedversion =
    {
        .major                      = 1,
        .minor                      = 2
    },
    .connectedEncodersMask          = EOMTHEEMSAPPLCFG_EBX_encodersMASK,   
    .configdataofMC4boards          =
    {
        .shiftvalues    =
        {
            .jointVelocityShift                 = 8,
            .jointVelocityEstimationShift       = 8,
            .jointAccelerationEstimationShift   = 5
        },
        .bcastpolicy            =
        {
            .val2bcastList      =
            {
            /* 0 */ ICUBCANPROTO_PER_MC_MSG__POSITION,
            /* 1 */ ICUBCANPROTO_PER_MC_MSG__STATUS,
            /* 2 */ ICUBCANPROTO_PER_MC_MSG__PRINT,
            /* 3 */ 0
            }
        }
    }       
};

const EOVtheEMSapplCfgBody theapplbodyconfig = 
{
    .type               =   0,
    .thetrueconfig      =   (void*) &theemsapplbodycfg
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void overridden_appl_led_error_init(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// marco.accame on Nov 26 2014
// this is the function used by EOtheErrorManager from startup until redefinition done by the EOMtheEMSappl 
// in this first phase, the EOtheErrorManager shall just print a string into the trace port and in case of a fatal error 
// it stops execution and start blinking leds
// in the second phase, the EOtheEMSappl redefines begaviours so that error messages are inserted into a sig<> ROP which 
// is sent to the remote host inside the standard EOMtheEMSsocket


// this funtion is exactly the same as the default one except for the blinking of leds

extern void eom_emsapplcfg_hid_userdef_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{
    const char empty[] = "EO?";
    const char *err = eo_errman_ErrorStringGet(eo_errman_GetHandle(), errtype);
    char str[128];
    EOMtheEMSapplCfg *emsapplcfg = eom_emsapplcfg_GetHandle();
    
    const char *eobjstr = (NULL != caller) ? (caller->eobjstr) : (empty);
    uint32_t taskid = (NULL != caller) ? (caller->taskid) : (0);

    if(emsapplcfg->errmng_haltrace_enabled)
    {
        snprintf(str, sizeof(str), "[%s] %s in tsk %d: %s", err, eobjstr, taskid, info);
        hal_trace_puts(str);
    }
    if(errtype <= eo_errortype_error)
    {
        return;
    }
    
    eov_sys_Stop(eov_sys_GetHandle());
    
    // init leds
    overridden_appl_led_error_init();
    
    // compute the mask of led to be toggled.
    uint8_t ledmask = 0xff;    
    if((NULL != caller) && (NULL != caller->eobjstr))
    {
        if(0 == strcmp(caller->eobjstr, "HAL"))
        {
            hl_bits_byte_bitclear(&ledmask, 0);           
        }
        else if(0 == strcmp(caller->eobjstr, "OSAL"))
        {
            hl_bits_byte_bitclear(&ledmask, 1);  
        }
        else if(0 == strcmp(caller->eobjstr, "IPAL"))
        {
            hl_bits_byte_bitclear(&ledmask, 2);  
        }
    }

    for(;;)
    {
        hal_sys_delay(100);
        
        if(hl_true == hl_bits_byte_bitcheck(ledmask, 0))
        {
            hal_led_toggle(hal_led0);
        }
        if(hl_true == hl_bits_byte_bitcheck(ledmask, 1))
        {
            hal_led_toggle(hal_led1);
        }
        if(hl_true == hl_bits_byte_bitcheck(ledmask, 2))
        {            
            hal_led_toggle(hal_led2);
        }
        
        hal_led_toggle(hal_led3);
        hal_led_toggle(hal_led4);
        hal_led_toggle(hal_led5);        
    }
}


// marco.accame: this function is called inside eom_emsappl_Initialise() just before to run the state machine
// which enters in the CFG state. it is the place where to launch new services.

extern void eom_emsappl_hid_userdef_initialise(EOMtheEMSappl* p)
{  
    EOMtheEMSapplCfg* emsapplcfg = eom_emsapplcfg_GetHandle();
    // the led-pulser is initted as first thing
     
    // pulse led3 forever at 20 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/20, 0);

    
    // initialise diagnostics
    eo_theEMSdgn_Initialize();
    
    // start the application body   
    //const eOemsapplbody_cfg_t *applbodycfg = &theemsapplbodycfg;   
    const eOemsapplbody_cfg_t * applbodycfg   = (const eOemsapplbody_cfg_t *)emsapplcfg->applbodycfg->thetrueconfig;
    eo_emsapplBody_Initialise(applbodycfg);       
}


extern void eom_emsappl_hid_userdef_on_entry_CFG(EOMtheEMSappl* p)
{
    
    // pulse led3 forever at 0.50 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, 2*EOK_reltime1sec, 0);
        
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eo_appCanSP_SetRunMode(appCanSP_ptr, eo_appCanSP_runMode__onEvent);
  
    eo_appCanSP_EmptyCanInputQueue(appCanSP_ptr, eOcanport1);
    eo_appCanSP_EmptyCanInputQueue(appCanSP_ptr, eOcanport2);
}

extern void eom_emsappl_hid_userdef_on_exit_CFG(EOMtheEMSappl* p)
{
    eOresult_t res;
    res = eo_emsapplBody_EnableTxAllJointOnCan(eo_emsapplBody_GetHandle());
    if(eores_OK != res)
    {
        ; //gestisci errore
    }
}
extern void eom_emsappl_hid_userdef_on_entry_RUN(EOMtheEMSappl* p)
{
    // pulse led3 forever at 1 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/1, 0); 
        

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    //Before changing appCanRunMode is important be sure can output queues are empty
    eo_appCanSP_EmptyCanOutputQueue(appCanSP_ptr, eOcanport1);
    eo_appCanSP_EmptyCanOutputQueue(appCanSP_ptr, eOcanport2);
 
    eo_appCanSP_SetRunMode(eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle()), eo_appCanSP_runMode__onDemand);
}

extern void eom_emsappl_hid_userdef_on_exit_RUN(EOMtheEMSappl* p)
{
    eOresult_t res;
    //set run mode on demand in order to send message to disable all joints on can
    eo_appCanSP_SetRunMode(eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle()), eo_appCanSP_runMode__onEvent);

    res = eo_emsapplBody_DisableTxAllJointOnCan(eo_emsapplBody_GetHandle());
    if(eores_OK != res)
    {
        ; //gestisci errore
    }
    eo_emsapplBody_StopSkin(eo_emsapplBody_GetHandle());
}

extern void eom_emsappl_hid_userdef_on_entry_ERR(EOMtheEMSappl* p)
{
    // pulse led3 forever at 4 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/4, 0);
   
    eo_appCanSP_SetRunMode(eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle()), eo_appCanSP_runMode__onEvent);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void overridden_appl_led_error_init(void)
{
    // marco.accame: we init all the leds and we switch them off
    hal_led_init(hal_led0, NULL);
    hal_led_init(hal_led1, NULL);
    hal_led_init(hal_led2, NULL);
    hal_led_init(hal_led3, NULL);
    hal_led_init(hal_led4, NULL);
    hal_led_init(hal_led5, NULL);
    
    hal_led_off(hal_led0);
    hal_led_off(hal_led1);
    hal_led_off(hal_led2);
    hal_led_off(hal_led3);
    hal_led_off(hal_led4);
    hal_led_off(hal_led5);   
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


// -- oldies

#if 0


//static void s_eom_emsrunner_emsappl_toogleled(osal_timer_t* tmr, void* par);
//static void s_eom_emsappl_startBlinkLed(uint32_t counttick);
//static void s_eom_emsappl_InitLeds(void);

#ifdef _TEST_SEQNUM_
extern void my_cbk_onErrorSeqNum(eOethLowLevParser_packetInfo_t *pktInfo_ptr, uint32_t rec_seqNum, uint32_t expected_seqNum);
extern void my_cbk_onNVfound(eOethLowLevParser_packetInfo_t *pktInfo_ptr, eODeb_eoProtoParser_ropAdditionalInfo_t *ropAddInfo_ptr);
//this variable is a function ptr.if is is not null, the isr invoked on rx pkt, will call the function ponted by this variable
extern void (*hal_eth_lowLevelUsePacket_ptr)(uint8_t* pkt_ptr, uint32_t size);
extern void EthLowLevParser_callbackInHAL(uint8_t *packet, uint32_t size);
static void s_eom_emsappl_ethLowLevelParser_configure(void)
{
    
    hal_eth_lowLevelUsePacket_ptr = EthLowLevParser_callbackInHAL;
//     //4.1) init application parser: embObjParser
//     const eODeb_eoProtoParser_cfg_t  deb_eoParserCfg = 
//     {
//         EO_INIT(.checks)
//         {
//             EO_INIT(.seqNum)
//             {
//                 EO_INIT(.cbk_onErrSeqNum)           my_cbk_onErrorSeqNum,
//             },
//             
//             EO_INIT(.nv)                            {0},
//             
//             EO_INIT(.invalidRopFrame)               {0}
//         }
//     };
    
    
        //4.1) init application parser: embObjParser 
    const eODeb_eoProtoParser_cfg_t  deb_eoParserCfg =  
    { 
        EO_INIT(.checks) 
        { 
            EO_INIT(.seqNum) 
            { 
                EO_INIT(.cbk_onErrSeqNum)           my_cbk_onErrorSeqNum, 
            }, 
             
            EO_INIT(.nv) 
            { 
                EO_INIT(.NVs2searchArray) 
                { 
                    EO_INIT(.head) 
                    { 
                        EO_INIT(.capacity)       eODeb_eoProtoParser_maxNV2find, 
                        EO_INIT(.itemsize)       sizeof(eODeb_eoProtoParser_nvidEp_couple_t), 
                        EO_INIT(.size)           1, 
                    }, 
                    EO_INIT(.data) 
                    { 
                        {0x14, 0x9c00} 
                    } 
                 
                }, 
                EO_INIT(.cbk_onNVfound)            my_cbk_onNVfound 
            }, 
             
            EO_INIT(.invalidRopFrame)               {0} 
        } 
    };

    
    eODeb_eoProtoParser_Initialise(&deb_eoParserCfg);
    
    
    //4.2) init low level parser: eOethLowLevParser
/*    const eOethLowLevParser_cfg_t  ethLowLevParserCfg = 
    {
        EO_INIT(.conFiltersData) 
        {
            EO_INIT(.filtersEnable)     0,
            EO_INIT(.filters)           {0}, //use pcap filter
        },
        
        EO_INIT(.appParserData)
        {
            EO_INIT(.func)             eODeb_eoProtoParser_RopFrameDissect
            EO_INIT(.arg)              eODeb_eoProtoParser_GetHandle(),
        }
    };
*/
    //currently use thelow level parser and appl paser separately
    const eOethLowLevParser_cfg_t  ethLowLevParserCfg = {0}; 
    eo_ethLowLevParser_Initialise(&ethLowLevParserCfg);

}
#endif

#ifdef _TEST_SEQNUM_
static void s_eom_emsrunner_emsappl_toogleallled(osal_timer_t* tmr, void* par)
{

    hal_led_toggle(emsappl_ledgreen);
    hal_led_toggle(emsappl_ledyellow);
    #warning acemor asks: who sets the variable can_out_queue_full to true? 
    if(can_out_queue_full)
    {
        hal_led_toggle(emsappl_ledred);
    }
}
#endif

//static void s_eom_emsappl_InitLeds(void)
//{
//    hal_led_cfg_t cfg = {.dummy=0};
//    
//    hal_led_init(emsappl_ledred, &cfg);
//    hal_led_off(emsappl_ledred);
//    hal_led_init(emsappl_ledgreen, &cfg); //led green
//    hal_led_off(emsappl_ledgreen);
//    hal_led_init(emsappl_ledyellow, &cfg);
//    hal_led_off(emsappl_ledyellow);
//    hal_led_init(emsappl_ledorange, &cfg);
//    hal_led_off(emsappl_ledorange); 
//
////     eOmledpulser_cfg_t ledpulsercfg = 
////     {
////         .numberofleds   = 3,
////         .leds           = { eom_ledpulser_led_zero, eom_ledpulser_led_one, eom_ledpulser_led_two }
////     };
//
////     eom_ledpulser_Initialise(&ledpulsercfg);
//
//    //eom_ledpulser_Start(eom_ledpulser_GetHandle(), eom_ledpulser_led_zero, 400*1000, 10);
//}

//static void s_eom_emsappl_startBlinkLed(uint32_t counttick)
//{
//    osal_timer_timing_t timing;
//    osal_timer_onexpiry_t onexpiry;
//    
//    timing.startat  = OSAL_abstimeNONE;
//    timing.count    = counttick * osal_info_get_tick(); 
//    timing.mode     = osal_tmrmodeFOREVER; 
//
//    onexpiry.cbk    = s_eom_emsrunner_emsappl_toogleled;
//    onexpiry.par    = eom_emsappl_GetHandle();        
//
//
//    if(NULL == timer4led)
//    {
//        timer4led = osal_timer_new(); 
//    }
//    osal_timer_start(timer4led, &timing, &onexpiry, osal_callerTSK);
//}

//static void s_eom_emsrunner_emsappl_toogleled(osal_timer_t* tmr, void* par)
//{
//    hal_led_toggle(emsappl_ledorange);
//}

#endif






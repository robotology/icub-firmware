
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theUpdaterParser.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EoUpdaterProtocol.h"
#include "embot_os_Timer.h"
#include "eEsharedServices.h" 
#include "eEmemorymap.h" 
#include "EoBoards.h"
#include "embot_hw_sys.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------
 

struct embot::app::eth::theUpdaterParser::Impl
{
    using uprot_fp_process_t = uint8_t (*)(Impl*, eOuprot_opcodes_t, uint8_t *, uint16_t, eOipv4addr_t, uint8_t *, uint16_t, uint16_t *);

    static constexpr embot::core::relTime shutdowntime {20*embot::core::time1millisec};
    
    EOpacket *replypkt {nullptr};
    bool transmit {false};
    bool s_restart {false};
    embot::os::Timer *shutdowntmr {nullptr};

    Config _config {};
        
    Impl() = default;    

    bool initialise(const Config &config);
    bool parse(EOpacket* rxpkt);    
    bool getreply(EOpacket** txpkt);
        
private:
    bool s_parse_and_form(uint8_t *data, uint8_t size, eOipv4addr_t remaddr, eOipv4port_t remport);
    uint8_t s_appl_parse(uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout, eOipv4port_t *port, bool *restart);

    // the ... parser functions of the opcodes
    static uint8_t s_uprot_proc_NONE(Impl* impl, eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
    static uint8_t s_uprot_proc_UNSUPP(Impl* impl, eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
    static uint8_t s_uprot_proc_DISCOVER(Impl* impl, eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
    static uint8_t s_uprot_proc_MOREINFO(Impl* impl, eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
    static uint8_t s_uprot_proc_RESTART(Impl* impl, eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
    static uint8_t s_uprot_proc_CANGATEWAY(Impl* impl, eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);
    // the former for the DISCOVER opcode
    uint16_t s_discover_fill(eOuprot_cmd_DISCOVER_REPLY_t *reply, eOuprot_opcodes_t opcode2use, uint16_t sizeofreply2use);
    static uint16_t s_add_description(char *data, uint16_t capacity, const char *prefix);

    static void shutdown2updater(void *p)
    {       
        //osal_system_scheduling_suspend();
        embot::hw::sys::irq_disable();
        volatile eEprocess_t procupdt = ee_procUpdater;
        ee_sharserv_ipc_gotoproc_set(procupdt);
        ee_sharserv_ipc_gotoproc_set(procupdt);
        ee_sharserv_ipc_gotoproc_set(procupdt);
        ee_sharserv_sys_restart();        
    }
};

//embot::app::eth::theUpdaterParser::Impl::Impl()
//{
//    // we need it because ... we dont have default values for the C structs and better doing  
//}

bool embot::app::eth::theUpdaterParser::Impl::initialise(const Config &config)
{
    if(nullptr != shutdowntmr)
    {
        return true;
    }
    
    _config = config;
    
    replypkt = eo_packet_New(_config.txpktcapacity);
    shutdowntmr = new embot::os::Timer;
    
    return true;
}

bool embot::app::eth::theUpdaterParser::Impl::parse(EOpacket* rxpkt)
{
    if(nullptr == rxpkt)
    {
        return false;
    }
    
    bool ret {false};
    
    uint8_t *data = nullptr;
    uint16_t size = 0;
    eOipv4addr_t remaddr {0};
    eOipv4port_t remport {0};
            
    eo_packet_Payload_Get(rxpkt, &data, &size);
    eo_packet_Addressing_Get(rxpkt, &remaddr, &remport);
    
    if((embot::app::eth::IPlocalhost.v != _config.host.addr.v) && (_config.host.addr.v != remaddr))
    {   // _config.host.addr is not eok_ipv4addr_localhost, so we can parse packets coming only from the host 
        transmit = false;
        ret = false;
    }
    else
    {   // ok can parse the packet    
        transmit = s_parse_and_form(data, size, remaddr, remport);        
        ret = true;        
    } 
    
    return ret;
}

bool embot::app::eth::theUpdaterParser::Impl::getreply(EOpacket** txpkt)
{
    if(nullptr == txpkt)
    {
        return false;
    }
    
    *txpkt = replypkt;
    
    bool res = transmit;
    
    transmit = false;
    return res;
}

bool embot::app::eth::theUpdaterParser::Impl::s_parse_and_form(uint8_t *data, uint8_t size, eOipv4addr_t remaddr, eOipv4port_t remport)
{
    bool dotx = false;

    uint8_t *txdata;
    uint16_t txsize = 0;
    eo_packet_Payload_Get(replypkt, &txdata, &txsize);      
        
    eOipv4port_t port = _config.host.port;        
    bool restart2updater {true};
    
    uint8_t r = s_appl_parse(data, size, remaddr, txdata, _config.txpktcapacity, &txsize, &port, &restart2updater);                
    
    switch(r)
    {
        case 1:
        {
            if(0 != txsize)
            {
                dotx = true;
            }
            else
            {
                dotx = false;
            }
            
            // timer di shutdown
            if(true == restart2updater)
            {
                // start a timer which will execute the shutdown
                embot::os::Action act {embot::os::CallbackToThread({shutdown2updater, nullptr}, nullptr)}; //(embot::os::CallbackToThread());
                embot::os::Timer::Config cfg{shutdowntime, act, embot::os::Timer::Mode::oneshot, 0};
                shutdowntmr->start(cfg);
            }
        
        } break;
        
        
        default:
        case 0:
        {
            dotx = false;                
        } break;                       
    }
    
    if(true == dotx)
    {       
        eOipv4addr_t address = _config.host.addr.v;
        if(eok_ipv4addr_localhost == address)
        {
            address = remaddr;
        }
                
        // there is a msg to send back
        eo_packet_Size_Set(replypkt, txsize); 
        eo_packet_Addressing_Set(replypkt, address, port);   
    }                               

    return dotx;
}

uint8_t embot::app::eth::theUpdaterParser::Impl::s_appl_parse(uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout, eOipv4port_t *port, bool *restart)
{  
    eOuprot_opcodes_t opc = uprot_OPC_NONE;    
    uprot_fp_process_t process = s_uprot_proc_NONE;
    
    
    *restart = s_restart = false;
    
    switch(pktin[0])
    {        
        case uprot_OPC_LEGACY_SCAN:
        case uprot_OPC_DISCOVER:
        {
            opc = (sizeof(eOuprot_cmd_LEGACY_SCAN_t) == pktinsize) ? uprot_OPC_LEGACY_SCAN : uprot_OPC_DISCOVER;
            process = s_uprot_proc_DISCOVER;    
            *port = _config.host.port;            
        } break;

        case uprot_OPC_LEGACY_PROCS:
        case uprot_OPC_MOREINFO:
        {
            opc = (sizeof(eOuprot_cmd_LEGACY_PROCS_t) == pktinsize) ? uprot_OPC_LEGACY_PROCS : uprot_OPC_MOREINFO;
            process = s_uprot_proc_MOREINFO;    
            *port = _config.host.port;            
        } break;

        
        case uprot_OPC_LEGACY_CANGATEWAY:
        case uprot_OPC_CANGATEWAY:
        {
            opc = (sizeof(eOuprot_cmd_LEGACY_CANGATEWAY_t) == pktinsize) ? uprot_OPC_LEGACY_CANGATEWAY : uprot_OPC_CANGATEWAY;
            process = s_uprot_proc_CANGATEWAY; 
            *port = 3334;            
        } break;
        
        case uprot_OPC_RESTART:
        {            
            opc = uprot_OPC_RESTART; 
            process = s_uprot_proc_RESTART;
            *port = _config.host.port;
        } break;
 
        default:
        {
            opc = uprot_OPC_NONE;
            process = s_uprot_proc_NONE;
            *port = _config.host.port;
        } break;
        
    }
    
    
    uint8_t ret = process(this, opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout);
    
    // s_restart is changed inside process()
    *restart = s_restart;
    
    return ret;
}


uint8_t embot::app::eth::theUpdaterParser::Impl::s_uprot_proc_NONE(Impl* impl, eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{  
    // no reply
    *sizeout = 0;    
    return(0);   
}


uint8_t embot::app::eth::theUpdaterParser::Impl::s_uprot_proc_UNSUPP(Impl* impl, eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{ 
#if 1
    // no reply
    *sizeout = 0;    
    return(0); 
#else    
    eOuprot_cmdREPLY_t *reply =  (eOuprot_cmdREPLY_t*)pktout;        
    reply->opc = opc;
    reply->res = uprot_RES_ERR_UNSUPPORTED;
    reply->protversion = EOUPROT_PROTOCOL_VERSION;
    reply->sizeofextra = 0;
    *sizeout = sizeof(eOuprot_cmdREPLY_t);  
    
    return 1;
#endif    
}



// both uprot_OPC_DISCOVER and uprot_OPC_LEGACY_SCAN. the only difference is that the reply to the former is more structured
uint8_t embot::app::eth::theUpdaterParser::Impl::s_uprot_proc_DISCOVER(Impl* impl, eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{    
    if(eobool_false == eouprot_can_process_opcode(eApplication, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(impl, opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }
        
    const eEmoduleInfo_t* module = (const eEmoduleInfo_t*)(EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET);

    if(opc == uprot_OPC_DISCOVER)
    {
        eOuprot_cmd_DISCOVER_t * cmd = (eOuprot_cmd_DISCOVER_t*)pktin;
        // shall i restart? let's read it
        impl->s_restart = (1 == cmd->jump2updater) ? (eobool_true) : (eobool_false);
        //s_restart = eobool_true;
        
        // prepare the reply.
        eOuprot_cmd_DISCOVER_REPLY_t *reply = (eOuprot_cmd_DISCOVER_REPLY_t*) pktout;    
        if(capacityout > sizeof(eOuprot_cmd_DISCOVER_REPLY_t))
        {   // yes, we have enough capacity to reply           
            *sizeout = impl->s_discover_fill(reply, uprot_OPC_DISCOVER, sizeof(eOuprot_cmd_DISCOVER_REPLY_t));
            return 1; 
        }
        else
        {   // i must reply with old protocol because i dont have enough capacity
            opc = uprot_OPC_LEGACY_SCAN;
        }
    }
    else
    {
        impl->s_restart = eobool_true;
    }
    
                 
    if(uprot_OPC_LEGACY_SCAN == opc)
    {    
        const uint8_t BOARD_TYPE_EMS = 0x0A;
        // eupdater_info_trace("CORE", "CMD_SCAN");
        *sizeout = 14;
        
        pktout[ 0] = uprot_OPC_LEGACY_SCAN;
        pktout[ 1] = module->info.entity.version.major;
        pktout[ 2] = module->info.entity.version.minor;
        pktout[ 3] = BOARD_TYPE_EMS;
        
        const eEipnetwork_t *ipnetworkstrg;
        ee_sharserv_info_deviceinfo_item_get(sharserv_info_ipnet, (const void**)&ipnetworkstrg);

        pktout[ 4] = (ipnetworkstrg->ipnetmask>>24) & 0xFF;
        pktout[ 5] = (ipnetworkstrg->ipnetmask>>16) & 0xFF;
        pktout[ 6] = (ipnetworkstrg->ipnetmask>>8)  & 0xFF;
        pktout[ 7] =  ipnetworkstrg->ipnetmask      & 0xFF;

        pktout[ 8] = (ipnetworkstrg->macaddress>>40) & 0xFF;
        pktout[ 9] = (ipnetworkstrg->macaddress>>32) & 0xFF;
        pktout[10] = (ipnetworkstrg->macaddress>>24) & 0xFF;
        pktout[11] = (ipnetworkstrg->macaddress>>16) & 0xFF;
        pktout[12] = (ipnetworkstrg->macaddress>>8)  & 0xFF;
        pktout[13] = (ipnetworkstrg->macaddress)     & 0xFF;        
    }
    
    return 1;    
}



uint8_t embot::app::eth::theUpdaterParser::Impl::s_uprot_proc_MOREINFO(Impl* impl, eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{       
    if(eobool_false == eouprot_can_process_opcode(eApplication, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(impl, opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }
    
    eOuprot_cmd_MOREINFO_t * cmd = (eOuprot_cmd_MOREINFO_t*)pktin;
    // shall i restart? let's read it asap
    impl->s_restart = (1 == cmd->jump2updater) ? (eobool_true) : (eobool_false);
    impl->s_restart = eobool_false;
    
    // prepare the reply.
    eOuprot_cmd_MOREINFO_REPLY_t *reply = (eOuprot_cmd_MOREINFO_REPLY_t*) pktout;
    
    // surely there is the discover part.
    if(capacityout > sizeof(eOuprot_cmd_MOREINFO_REPLY_t))
    {   // yes, we have enough capacity to reply    
        *sizeout = impl->s_discover_fill(&reply->discover, uprot_OPC_MOREINFO, sizeof(eOuprot_cmd_MOREINFO_REPLY_t));
    }
    else
    {
        return(s_uprot_proc_UNSUPP(impl, opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }    
    

    // but there may be also the description
    reply->hasdescription = cmd->plusdescription;
       
    if(1 == reply->hasdescription)
    {
        // 1. add the description in reply->description
        //const char *prefix = (eobool_false == s_restart) ? "MOREINFO-APP\r\n" : "MOREINFO-UPD\r\n";
        const char *prefix = "MOREINFO-APP\r\n";

        uint16_t sizeofdescription = s_add_description((char*)reply->description, capacityout - sizeof(eOuprot_cmd_MOREINFO_REPLY_t), prefix);
        
        // 2. increase some sizes  
        reply->discover.reply.sizeofextra += sizeofdescription;
        *sizeout += sizeofdescription;                
    }   

    return 1;    
}


uint8_t embot::app::eth::theUpdaterParser::Impl::s_uprot_proc_RESTART(Impl* impl, eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{
    if(eobool_false == eouprot_can_process_opcode(eApplication, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(impl, opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }
    
    ee_sharserv_sys_restart();

    // no reply
    *sizeout = 0;   
    return(0);      
}


uint8_t embot::app::eth::theUpdaterParser::Impl::s_uprot_proc_CANGATEWAY(Impl* impl, eOuprot_opcodes_t opc, uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout)
{
    if(eobool_false == eouprot_can_process_opcode(eApplication, EOUPROT_PROTOCOL_VERSION, opc, 0))
    {
        return(s_uprot_proc_UNSUPP(impl, opc, pktin, pktinsize, remaddr, pktout, capacityout, sizeout));
    }

    eOuprot_cmdREPLY_t * reply = (eOuprot_cmdREPLY_t*) pktout;
    
    reply->opc = uprot_OPC_CANGATEWAY;
    reply->protversion = EOUPROT_PROTOCOL_VERSION;
    reply->res = uprot_RES_ERR_TRYAGAIN;
    reply->sizeofextra = 0;

    // we send a ko message.
    *sizeout = sizeof(eOuprot_cmdREPLY_t);   
    
    // we force a restart to eUpdater
    return(1);      
}

// used to fill the reply with the extra processes
static uint16_t s_discover_fill2(eOuprot_cmd_DISCOVER_REPLY2_t* reply2, eOuprot_opcodes_t opcode2use, uint16_t sizeofreply2use)
{
    reply2->discoveryreply.reply.opc = opcode2use;
    
    uint8_t nprocs = 0;
    const eEprocess_t *s_proctable = NULL;
    ee_sharserv_part_proc_allavailable_get(&s_proctable, &nprocs);
    constexpr uint8_t maxNumberOfProcessesOnSingleCore {3};
    
    // here we iterate only for the extra processes
    for(uint8_t i=0, j=maxNumberOfProcessesOnSingleCore; j < nprocs; i++, j++)
    {
        const eEmoduleInfo_t *s_modinfo = NULL;
        ee_sharserv_part_proc_get(s_proctable[j], &s_modinfo);
        reply2->extraprocs[i].type = s_proctable[j];
        reply2->extraprocs[i].filler[0] = EOUPROT_VALUE_OF_UNUSED_BYTE;
        reply2->extraprocs[i].version.major = s_modinfo->info.entity.version.major;
        reply2->extraprocs[i].version.minor = s_modinfo->info.entity.version.minor;
        memcpy(&reply2->extraprocs[i].date, &s_modinfo->info.entity.builddate, sizeof(eOdate_t));
        
        reply2->extraprocs[i].compilationdate.year = 1999;
        reply2->extraprocs[i].compilationdate.month = 9;
        reply2->extraprocs[i].compilationdate.day = 9;
        reply2->extraprocs[i].compilationdate.hour = 9;
        reply2->extraprocs[i].compilationdate.min = 9;
        
        // TODO: can we use embot::hw::flash?
        //const embot::hw::flash::Partition& pp = embot::hw::flash::bsp::partition(s_modinfo->info.rom.addr);
        
        volatile eEmoduleExtendedInfo_t * extinfo = (volatile eEmoduleExtendedInfo_t*)(s_modinfo->info.rom.addr+EENV_MODULEINFO_OFFSET);
        if(ee_res_OK == ee_is_extendemoduleinfo_valid((eEmoduleExtendedInfo_t*)extinfo))
        {
            eo_common_compiler_string_to_date((const char*)extinfo->compilationdatetime, &reply2->extraprocs[i].compilationdate);
        }
       
        reply2->extraprocs[i].rom_addr_kb = s_modinfo->info.rom.addr / 1024;
        reply2->extraprocs[i].rom_size_kb = s_modinfo->info.rom.size / 1024;
      
    }
    return sizeof(eOuprot_cmd_DISCOVER_REPLY2_t);
}

uint16_t embot::app::eth::theUpdaterParser::Impl::s_discover_fill(eOuprot_cmd_DISCOVER_REPLY_t *reply, eOuprot_opcodes_t opcode2use, uint16_t sizeofreply2use)
{
    uint16_t size = sizeof(eOuprot_cmd_DISCOVER_REPLY_t);  
    
    reply->reply.opc = opcode2use;
    reply->reply.res = uprot_RES_OK;
    reply->reply.protversion = EOUPROT_PROTOCOL_VERSION;
    reply->reply.sizeofextra = sizeofreply2use - sizeof(eOuprot_cmdREPLY_t);   

    // ok, now i fill the values
    
    const eEipnetwork_t *ipnetworkstrg = NULL;
    ee_sharserv_info_deviceinfo_item_get(sharserv_info_ipnet, (const void**)&ipnetworkstrg);
    
    const eEboardInfo_t* boardinfo = NULL;
    ee_sharserv_info_boardinfo_get(&boardinfo);
    // now get the board type.
    uint8_t boardtype = 255; // use eobrd_ethtype_unknown later on
    if(0x11 == boardinfo->info.entity.signature)
    {   // old eLoader upto version 2.11 of build date 2015 May 26 11:11. it have info about board type in a string
        
        if(0 == strcmp((const char*)boardinfo->info.name, "ems4rd"))
        {
            boardtype = eobrd_ethtype_ems4; // use eobrd_ethtype_ems4 later on        
        }
        else if(0 == strcmp((const char*)boardinfo->info.name, "mc4plus"))
        {
             boardtype = eobrd_ethtype_mc4plus; // use eobrd_ethtype_mc4plus later on 
        }        
    }
    else
    {   // after eLoader version 2.12 the fields boardinfo->info.entity.signature contains the board type. 
        boardtype = boardinfo->info.entity.signature;
    }
    
    eEprocess_t startup = ee_procNone;
    eEprocess_t def2run = ee_procNone;
    ee_sharserv_part_proc_startup_get(&startup);
    ee_sharserv_part_proc_def2run_get(&def2run);
    
    uint8_t nprocs = 0;
    const eEprocess_t *s_proctable = NULL;
    ee_sharserv_part_proc_allavailable_get(&s_proctable, &nprocs);
    
    reply->boardtype = boardtype;
#if defined(USE_EMBOT_runner)  
    #warning USE_EMBOT_runner is defined............. we must understand if the runner is running or not.
    reply->unused[0] = 0x00;
#else    
    reply->unused[0] = (eobool_true == eom_emsrunner_IsRunning(eom_emsrunner_GetHandle())) ? 0x01 : (0x00);
#endif
    
    reply->capabilities = eouprot_get_capabilities(eApplication, EOUPROT_PROTOCOL_VERSION);;
    reply->mac48[0] = (ipnetworkstrg->macaddress>>40) & 0xFF;
    reply->mac48[1] = (ipnetworkstrg->macaddress>>32) & 0xFF;
    reply->mac48[2] = (ipnetworkstrg->macaddress>>24) & 0xFF;
    reply->mac48[3] = (ipnetworkstrg->macaddress>>16) & 0xFF;
    reply->mac48[4] = (ipnetworkstrg->macaddress>>8)  & 0xFF;
    reply->mac48[5] = (ipnetworkstrg->macaddress)     & 0xFF;
    reply->processes.numberofthem = nprocs;
    reply->processes.startup = startup;
    reply->processes.def2run = def2run;
    reply->processes.runningnow = eApplication;
    
    constexpr uint8_t maxNumberOfProcessesOnSingleCore {3};
    uint8_t number = std::min(nprocs, maxNumberOfProcessesOnSingleCore);

    for(uint8_t i=0; i<nprocs; i++)
    {
        const eEmoduleInfo_t *s_modinfo = NULL;
        ee_sharserv_part_proc_get(s_proctable[i], &s_modinfo);
        reply->processes.info[i].type = s_proctable[i];
        reply->processes.info[i].filler[0] = EOUPROT_VALUE_OF_UNUSED_BYTE;
        reply->processes.info[i].version.major = s_modinfo->info.entity.version.major;
        reply->processes.info[i].version.minor = s_modinfo->info.entity.version.minor;
        memcpy(&reply->processes.info[i].date, &s_modinfo->info.entity.builddate, sizeof(eOdate_t));
        
        reply->processes.info[i].compilationdate.year = 1999;
        reply->processes.info[i].compilationdate.month = 9;
        reply->processes.info[i].compilationdate.day = 9;
        reply->processes.info[i].compilationdate.hour = 9;
        reply->processes.info[i].compilationdate.min = 9;
        
        volatile eEmoduleExtendedInfo_t * extinfo = (volatile eEmoduleExtendedInfo_t*)(s_modinfo->info.rom.addr+EENV_MODULEINFO_OFFSET);            
        if(ee_res_OK == ee_is_extendemoduleinfo_valid((eEmoduleExtendedInfo_t*)extinfo))
        {
            eo_common_compiler_string_to_date((const char*)extinfo->compilationdatetime, &reply->processes.info[i].compilationdate);
        }
        
        reply->processes.info[i].rom_addr_kb = s_modinfo->info.rom.addr / 1024;
        reply->processes.info[i].rom_size_kb = s_modinfo->info.rom.size / 1024;  
      
    }
    
    // In case of multi core boards we have to add more processes
    if(nprocs >= maxNumberOfProcessesOnSingleCore)
    {
        // prepare for the extended reply
        eOuprot_cmd_DISCOVER_REPLY2_t* reply2 = reinterpret_cast<eOuprot_cmd_DISCOVER_REPLY2_t*>(reply);
        size = s_discover_fill2(reply2, uprot_OPC_DISCOVER2, sizeof(eOuprot_cmd_DISCOVER_REPLY2_t));
    }

    // now boardinfo32
    const void *page = NULL;
    ee_sharserv_info_deviceinfo_item_get(sharserv_info_page32, &page);
    memcpy(reply->boardinfo32, page, 32);   
    
    return size;
}

#define MYABS(x) ( ((x)>0) ? (x) : (0) )
uint16_t embot::app::eth::theUpdaterParser::Impl::s_add_description(char *data, uint16_t capacity, const char *prefix)
{
    uint16_t size = 0;        
    size+=snprintf(data+size, MYABS(capacity-size), "%s", prefix);    
    return size;
}

// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theUpdaterParser& embot::app::eth::theUpdaterParser::getInstance()
{
    static theUpdaterParser* p = new theUpdaterParser();
    return *p;
}

embot::app::eth::theUpdaterParser::theUpdaterParser()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::eth::theUpdaterParser::~theUpdaterParser() { }
 

bool embot::app::eth::theUpdaterParser::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

bool embot::app::eth::theUpdaterParser::parse(EOpacket* rxpkt)
{
    return pImpl->parse(rxpkt);
} 

bool embot::app::eth::theUpdaterParser::getreply(EOpacket** txpkt)
{
    return pImpl->getreply(txpkt);
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



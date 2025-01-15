

/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_THEICCSERVICEROP_H_
#define __EMBOT_APP_ETH_THEICCSERVICEROP_H_

#include "embot_core.h"
#include "embot_app_eth.h"
#include "embot_app_icc.h"

#include "embot_app_eth_theICCservice.h"

#include <array>

namespace embot::app::eth::icc {
  
    
    struct ItemROP : public Item
    {
        enum class CMD : uint8_t { ask = 0, say = 1, set = 2, sig = 3, ack = 4, nak = 5, ping = 6, none = 31 };
        using ID = uint8_t;
        static constexpr ID IDnone {0};

        static constexpr ID IDsignature {252};
        static constexpr ID IDunique64 {253};
        static constexpr ID IDdummyUINT32 {254};
        static constexpr ID IDtimeoflife {255};
        
        
        struct Descriptor 
        {
            ID id {IDnone};
            uint8_t size {0}; 
            constexpr Descriptor(ID i, uint8_t s) : id(i), size(s) {} 
            constexpr Descriptor() = default;  
            void clear()
            {
                id = IDnone;
                size = 0;
            }    
        };
                
        struct Variable 
        {
            Descriptor descriptor {IDnone, 0};
            void * memory {nullptr};
            constexpr Variable(ID i, uint8_t s, void *m = nullptr) : descriptor({i,s}), memory(m) {} 
            constexpr Variable(const Descriptor &d, void *m = nullptr) : descriptor(d), memory(m) {}
            constexpr Variable() = default;
            void clear() 
            {
                descriptor.clear();
                memory = nullptr;
            }                
        };
        
        
        CMD cmd {CMD::none};     // also inside filler[0]
        Variable var {};        // var.id is inside filler[1], var.size in filler[2], var.memory points to &data[0]
                        
        constexpr ItemROP() 
        {
            type = ItemType::ROP;
            memset(filler, 0, sizeof(filler));
            memset(data, 0, sizeof(data));
        }
            
        constexpr ItemROP(CMD c, const Variable &v) : cmd(c), var(v)
        {
            type = ItemType::ROP;
            
            var.descriptor.size = std::min(v.descriptor.size, static_cast<uint8_t>(sizeof(data)));
            
            filler[0] = embot::core::tointegral(cmd);
            filler[1] = var.descriptor.id;
            filler[2] = var.descriptor.size;
            
            if(nullptr != var.memory)
            {
                std::memmove(&data[0], var.memory, var.descriptor.size);
            }
        } 
        
        constexpr ItemROP(const Item &i)
        {
            load(i);                 
        } 
        
        void clear()
        {
            type = ItemType::ROP;
            memset(filler, 0, sizeof(filler));
            memset(data, 0, sizeof(data));
            
            cmd = CMD::none;
            var.clear();            
        }
        
        void fill(CMD c, const Variable &v)
        {
            cmd = c;
            var = v;
            var.descriptor.size = std::min(v.descriptor.size, static_cast<uint8_t>(sizeof(data)));
            
            filler[0] = embot::core::tointegral(cmd);
            filler[1] = var.descriptor.id;
            filler[2] = var.descriptor.size;
            
            if(nullptr != var.memory)
            {
                std::memmove(data, var.memory, var.descriptor.size);
            }             
        }
        
        void load(const Item &i)
        {
            if(ItemType::ROP == i.type)
            {
                type = ItemType::ROP;
                std::memmove(filler, i.filler, sizeof(filler));
                std::memmove(data, i.data, sizeof(data));
                // and now the shortcuts
                cmd = static_cast<CMD>(filler[0]);
                var.descriptor.id = filler[1];
                var.descriptor.size = filler[2];
                var.memory = &data[0];
            }                 
        }
        
        Item item() const
        {
            Item i {};
            i.type = type;
            std::memmove(i.filler, filler, sizeof(filler));
            std::memmove(i.data, data, sizeof(data));
            return i;                    
        }

    };   
    
    
    constexpr ItemROP::Descriptor DesROPsignature {ItemROP::IDsignature, sizeof(embot::app::icc::Signature)}; // 16
    constexpr ItemROP::Descriptor DesROPunique64 {ItemROP::IDunique64, 8};
    constexpr ItemROP::Descriptor DesROPdummyUINT32 {ItemROP::IDdummyUINT32, 4};
    constexpr ItemROP::Descriptor DesROPtimeoflife {ItemROP::IDtimeoflife, 8};  
    
        
    class theICCserviceROP
    {
    public:
        
        static theICCserviceROP& getInstance();
    
        using fpOnROPrx = bool (*)(const ItemROP &rxrop, ItemROP &reply); 
    
        struct Config
        {
            theICCservice::Pipe pipe {theICCservice::Pipe::two};  
            // Config::onroprx is used (if not nullptr) to process the rx ROP and prepare a reply ROP.
            // So, in case of ask<ID> this function must: retrieve the value associated to ID and if available prepare a say<ID, value>
            // if nullptr the default function is used that always sends back a nak
            fpOnROPrx onroprx {nullptr};            
            
            constexpr Config() = default;
            
            constexpr Config(theICCservice::Pipe p, fpOnROPrx o = defOnROPrx) : pipe(p), onroprx(o) {}
                                
            bool isvalid() const { return true; } // for now always valid. but we check vs it inside initialise()
        };      
        
        bool initialise(const Config &config);
        
        bool set(fpOnROPrx onroprx);  
        
        bool ping(embot::core::relTime timeout = 10*embot::core::time1millisec);                
        bool ask(ItemROP::Variable &var, embot::core::relTime timeout = embot::core::reltimeWaitForever);
        bool set(const ItemROP::Variable &var, embot::core::relTime timeout = embot::core::reltimeWaitForever);
        bool sig(const ItemROP::Variable &var);

        
        
        // this one manages only the ping<>, the ask<IDtimeoflife>, the ask<dummyUINT32> and the set<dummyUINT32>. 
        static bool defOnROPrx(const ItemROP &rxrop, ItemROP &reply) 
        {
            bool r {false};
            // it is mandatory that the memory of the varibles stays alive after defOnROPrx() returns so in here we mark it as static 
            // to move it away from being a temporary variable allocated on the stack
            // a proper implementation however requires to have these variables somehow global.
            static embot::core::Time timeoflife {0};
            static uint32_t dummyUINT32 {0};

            // we fill the default w/ nothing to transmit back 
            reply.fill(embot::app::eth::icc::ItemROP::CMD::none, rxrop.var);
            
            switch(rxrop.cmd)
            {               
                case embot::app::eth::icc::ItemROP::CMD::ping:
                {
                    reply.fill(embot::app::eth::icc::ItemROP::CMD::ack, rxrop.var);
                } break;
                
                case embot::app::eth::icc::ItemROP::CMD::ask:
                {
                    switch(rxrop.var.descriptor.id)
                    {
                        case embot::app::eth::icc::ItemROP::IDtimeoflife:
                        {
                            timeoflife = embot::core::now();
                            reply.fill(embot::app::eth::icc::ItemROP::CMD::say, {{embot::app::eth::icc::ItemROP::IDtimeoflife, 8}, &timeoflife});
                        } break;
                        
                        case embot::app::eth::icc::ItemROP::IDdummyUINT32:
                        {
                            reply.fill(embot::app::eth::icc::ItemROP::CMD::say, {{embot::app::eth::icc::ItemROP::IDdummyUINT32, 4}, &dummyUINT32});                            
                        } break;
                        
                        // ... other managed cases
                        
                        default: 
                        {   // if the ID is not managed we do nothing but we send a nak back   
                            reply.fill(embot::app::eth::icc::ItemROP::CMD::nak, rxrop.var);
                        } break;                     
                    }                    
                } break;
            
                case embot::app::eth::icc::ItemROP::CMD::set:
                {
                    switch(rxrop.var.descriptor.id)
                    {                        
                        case embot::app::eth::icc::ItemROP::IDdummyUINT32:
                        {
                            dummyUINT32 = *reinterpret_cast<uint32_t*>(rxrop.var.memory);
                            reply.fill(embot::app::eth::icc::ItemROP::CMD::ack, {{embot::app::eth::icc::ItemROP::IDdummyUINT32, 4}, &dummyUINT32});                                                      
                        } break;
                        
                        // ... other managed cases
                        
                        default: 
                        {   // if the ID is not managed we do nothing but we send a nak back   
                            reply.fill(embot::app::eth::icc::ItemROP::CMD::nak, rxrop.var);
                        } break;                          
                    }
                    
                } break;                

                case embot::app::eth::icc::ItemROP::CMD::sig:
                {
                    switch(rxrop.var.descriptor.id)
                    {                        
                        case embot::app::eth::icc::ItemROP::IDdummyUINT32:
                        {
                            dummyUINT32 = *reinterpret_cast<uint32_t*>(rxrop.var.memory);
                            // i dont send any ack back                                                      
                        } break;
                        
                        // ... other managed cases
                        
                        default: 
                        {   // if the ID is not managed we do nothing and we send nothing back   
                            reply.fill(embot::app::eth::icc::ItemROP::CMD::none, rxrop.var);
                        } break;                        
                    }
                    
                } break;  
                
                default: { } break; // we dont manage any other received commands
                
            }

            r = (embot::app::eth::icc::ItemROP::CMD::none != rxrop.cmd);
            
            return r;
        }
        
    
    private:
        theICCserviceROP(); 
        ~theICCserviceROP(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    }; 

} // namespace embot::app::eth:icc {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

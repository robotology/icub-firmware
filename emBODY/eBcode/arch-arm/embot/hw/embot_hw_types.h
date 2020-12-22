
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_TYPES_H_
#define _EMBOT_HW_TYPES_H_

#include "embot_hw.h"
#include "embot_core.h"
#include "embot_core_binary.h"



namespace embot { namespace hw {
    
    // the result type
    
    enum class result_t : std::int8_t { OK = 0, NOK = -1, NOKtimeout = -2 };
    
    constexpr result_t resOK            = result_t::OK;
    constexpr result_t resNOK           = result_t::NOK;
    constexpr result_t resNOKtimeout    = result_t::NOKtimeout;
    
    
    // list of IDs for peripherals, drivers, ...
    
    enum ANY : std::uint8_t { none = 31,
                              one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eight = 7, nine = 8, ten = 9, 
                              eleven = 10, twelve = 11, thirteen = 12, fourteen = 13, fifteen = 14, sixteen = 15, seventeen = 16, 
                              eighteen = 17, nineteen = 18, twenty = 19, twentyone = 20, twentytwo = 21, tweentythree = 22, 
                              twentyfour = 23
                            };
    
    enum class CLOCK : std::uint8_t { syscore = 0, pclk1 = 1, pclk2 = 2, none = 31, maxnumberof = 3 };
    
    enum class LED : std::uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eight = 7, none = 31, maxnumberof = 8 };
            
    enum class CAN : std::uint8_t { one = 0, two = 1, none = 31, maxnumberof = 2};
            
    enum class FLASH : std::uint8_t { whole = 0, bootloader = 1, application = 2, sharedstorage = 3, applicationstorage = 4, none = 31, maxnumberof = 5 };
    
    enum class BTN : std::uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eight = 7, none = 31, maxnumberof = 8 };
    
    enum class PGA308 : std::uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, none = 31, maxnumberof = 6 };
    
    enum class SI7051 : std::uint8_t { one = 0, two = 1, none = 31, maxnumberof = 2 };
    
    enum class ADC : std::uint8_t { one = 0, none = 31, maxnumberof = 1};
    
    enum class ONEWIRE : std::uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, none = 31, maxnumberof = 6 };
    
    enum class TIMER : std::uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eight = 7, nine = 8, 
                                      ten = 9, eleven = 10, twelve = 11, thirteen = 12, fourteen = 13, fifteen = 14, sixteen = 15, 
                                      none = 31, maxnumberof = 16 }; 

    enum class I2C : std::uint8_t { one = 0, two = 1, three = 2, four = 3, none = 31, maxnumberof = 4 };     

    enum class BNO055 : std::uint8_t { one = 0, none = 31, maxnumberof = 1 };    
    
    enum class TLV493D : std::uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, none = 31, maxnumberof = 6 };
    
    enum class ADS122C04 : std::uint8_t { one = 0, none = 31, maxnumberof = 1 };
    
    enum class AD7147 : std::uint8_t { one = 0, two = 1, none = 31, maxnumberof = 2 };
  
  
    // definition of more complex data structures
    
    struct GPIO
    { 
        enum class PORT : std::uint8_t { A = 0, B = 1, C = 2, D = 3, E = 4, F = 5, G = 6, H = 7, I = 8, J = 9, K = 10, none = 31, maxnumberof = 11 };   
        enum class PIN : std::uint8_t { zero = 0, one = 1, two = 2, three = 3, four = 4, five = 5, six = 6, seven = 7, eight = 8, nine = 9, 
                                        ten = 10, eleven = 11, twelve = 12, thirteen = 13, fourteen = 14, fifteen = 15, none = 31, maxnumberof = 16 };
            
        PORT port {PORT::none};
        PIN pin {PIN::none};
        
        constexpr GPIO(PORT po, PIN pi) : port(po), pin(pi) {}
        constexpr GPIO() : port(PORT::none), pin(PIN::none) {}            
        constexpr bool isvalid() const { return (PORT::none == port) ? false : true; }       
    };
    
    
    struct Partition
    {
        std::uint32_t   address {0};
        std::uint32_t   maxsize {0}; 
        std::uint32_t   pagesize {0}; 
        constexpr Partition() = default;
        constexpr Partition(uint32_t a, uint32_t m, uint32_t p) : address(a), maxsize(m), pagesize(p) {}
        bool isvalid() const { if((0 == address) || (0 == maxsize) || (0 == pagesize)) { return false; } else { return true; } }
    }; 
        
        
    
    // REG<> is a template type for modeling a register of a given size and with a given number of fields
    // rTyp is one of regXXtype below, nFld is the number of fields 
    // for 8-bit register w/ 3 fields use:
    // REG<reg08type, 3>    
    using reg08type = uint8_t;
    using reg16type = uint8_t;
    using reg32type = uint32_t;
    template<typename rTyp, uint8_t nFld>
    struct REG
    {        
        // a way to describe a field inside the register
        struct Field
        {   
            uint8_t pos {0};    // the first position 
            rTyp msk {0};          // their 1-mask starting from pos
            constexpr Field(uint8_t firstpos, uint8_t numofbits) : pos(firstpos), msk(embot::core::binary::mask::ones<rTyp>(numofbits)) {}
            constexpr Field(uint8_t firstpos, uint8_t dummy, rTyp mask) : pos(firstpos), msk(mask) {}
            Field() = default;
        };
         
        struct Fields
        {
            std::array<Field, nFld> ff;
            static constexpr Field fnone {0, 0, 0};           
            constexpr Fields(const std::initializer_list<uint8_t> &sizes)
            {
                if(nFld >= sizes.size())
                {                    
                    uint8_t p = 0;
                    uint8_t i = 0;
                    for(const auto & s : sizes)
                    {
                        ff[i] = {p, s};
                        p += s;
                        i++;                    
                    }
                }                
            }
            
            const Field & get(uint8_t pos) const
            {
                return (pos < nFld) ? ff[pos] : fnone;
            }
        };
               
        bool set(const Field &f, const rTyp &value)
        {   // it sets one value into memory
            if(nullptr == memory)
            {
                return false;
            }
            embot::core::binary::mask::clear(*memory, f.msk, f.pos);
            (*memory) |= embot::core::binary::mask::place(value, f.msk, f.pos);
            return true;
        } 
        rTyp get(const Field &f) const
        {   // it gets one value from memory
            if(nullptr == memory)
            {
                return 42;
            }
            return embot::core::binary::mask::extract(*memory, f.msk, f.pos);
        }
        
        bool setfield(const uint8_t field, const rTyp &value)
        {
            if(field >= nFld)
            {
                return false;
            }
            return REG::set(pfields->get(field), value);
        }
        
        rTyp getfield(const uint8_t field) 
        { 
            return REG::get(pfields->get(field)); 
        } 
        
        // the memory of the register
        rTyp* memory {nullptr};
        // the description of its internal
        const Fields *pfields {nullptr};
        
        REG(rTyp *m = nullptr, const Fields *f = nullptr) : memory(m), pfields(f) {};
    };  

    #if 0
    //  usage:
    //  8-bit register w/ 3 fields
    struct REGexample : public REG<reg08type, 3>
    {   // [ field3:4 | field2:3 | field:1 ]
        // put in here the mane of the fields starting from right and using 0, 1, 2, ...
        enum class FIELD : uint8_t { field1 = 0, fields2 = 1, field2 = 2 }; 
        // it keeps the number of bits of each of the above fields
        static constexpr REG::Fields fields {1, 3, 4};  
        // the constructor accepts external memory
        REGexample(reg08type *mem = nullptr) : REG<reg08type, 3>(mem, &fields) {}
        // the address of teh register    
        static constexpr uint8_t address {0};
        // a method for assigning external memory         
        void load(reg08type *mem) { memory = mem; } 
        // by this method we assign a value to a field         
        bool set(const FIELD tag, const uint8_t value) { return setfield(embot::core::tointegral(tag), value); }
        // by this method we get a value from a field
        reg08type get(const FIELD tag) { return getfield(embot::core::tointegral(tag)); }
    };
    #endif
      

}} // namespace embot { namespace hw {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _TESTNAMESPACES_H_
#define _TESTNAMESPACES_H_


// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       test-namemspaces.h
    @brief      ceecwe
    @author     marco.accame@iit.it
    @date       09/09/2011
**/

/** @defgroup asse Evedwdnt Vieddddwer

    The cewcwev edde. 
    
    @{        
 **/

#include "stdlib.h" 
#include "stdint.h" 

#include <cstddef>


//namespace std
//{
//	typedef decltype(nullptr) nullptr_t;
//}

//#define nullptr NULL

namespace embot
{
    
    namespace if2hw
    {
        class storage
        {
        public:
            virtual bool read(uint32_t address, uint32_t size, uint8_t *data) = 0;
            virtual bool write(uint32_t address, uint32_t size, const uint8_t *data) = 0;   
            virtual bool erase(uint32_t address, uint32_t size) = 0;
            virtual bool erase() = 0;          
        };        
        
    }
    
    
    
    namespace hw
    {    
        class EEPROM : public if2hw::storage
        {
        public:
            struct Config
            {
                uint32_t startaddress;
                uint32_t size;
                uint32_t type_etc;                
            };
            EEPROM(){}
            ~EEPROM(){}
            bool init(const EEPROM::Config &config) { if(0 == config.size){ return false; } return true; }
            bool uninit() { return true; }
            bool isInitialised() { return true; }
            virtual bool read(uint32_t address, uint32_t size, uint8_t *data) { return true; }     
            virtual bool write(uint32_t address, uint32_t size, const uint8_t *data) { return true; }  
            virtual bool erase(uint32_t address, uint32_t size) { return true; }  
            virtual bool erase() { return true; }             
        };  

        class emulEEPROM : public if2hw::storage
        {
        public:
            struct Config
            {
                //typedef enum { nowhere = 0, inRAM = 1, inFLASH = 2 } emulPlace;
                //enum class emulPlace: std::uint8_t { nowhere = 0, inRAM = 1, inFLASH = 2 };
                enum class emulPlace { nowhere = 0, inRAM = 1, inFLASH = 2 };
                emulPlace place;
                uint32_t addressofzero;
                uint32_t capacity;
                uint32_t pagesize;   
                Config() {
                    reset(); 
                };
                Config(emulPlace _place, uint32_t _addressofzero, uint32_t _capacity, uint32_t _pagesize) {
                    load(_place, _addressofzero, _capacity, _pagesize); 
                }                    
                void load(emulPlace _place, uint32_t _addressofzero, uint32_t _capacity, uint32_t _pagesize) { 
                    place = _place; addressofzero = _addressofzero; capacity = _capacity; pagesize = _pagesize; 
                }                    
                void reset() { load(emulPlace::nowhere, 0, 0, 0); }                    
            };
            emulEEPROM();
            ~emulEEPROM();
            bool init(const emulEEPROM::Config &config);
            bool uninit();
            bool isInitialised();
            virtual bool read(uint32_t address, uint32_t size, uint8_t *data);     
            virtual bool write(uint32_t address, uint32_t size, const uint8_t *data);
            virtual bool erase(uint32_t address, uint32_t size);  
            virtual bool erase();            
            
            private:                
                struct Impl;
                Impl *pImpl;            
        };          
    }
    
    namespace bsp
    {
        class display
        {
        public:
            display(){};
            ~display(){};
            bool show(uint8_t *data, uint16_t sizex, int16_t sizey) { return true; }           
        };
        
    }    
    
}

namespace embot
{
    namespace sys
    {
        class theEnvironment
        {
        public:
            theEnvironment(){}
            ~theEnvironment(){}
            bool init(void) { return true; }    
            bool start(void) { return true; }                      
        };
        
        class theHeap
        {
        public:
            theHeap(){};
            ~theHeap(){};
            void * get(void) { return nullptr; }     
            void release(void *) {  }                       
        };  

//        class Timer
//        {
//        public:
//            Timer(){};
//            ~Timer(){};
//            bool config(void) { return true; };                         
//        };
//        
//        class theTimerManager
//        {
//        public:
//            theTimerManager(){};
//            ~theTimerManager(){};
//            bool add(Timer &t) { return true; };     
//            bool rem(Timer &t) { return true; };                       
//        };    

        class theStorage
        {
        public:
            static theStorage& getInstance()
            {
                static theStorage* p = new theStorage();
                return *p;
            }
            
        public:
            bool load(embot::if2hw::storage &strg);
            bool read(uint32_t address, uint32_t size, uint8_t *data);    
            bool write(uint32_t address, uint32_t size, const uint8_t *data);   
            bool erase(uint32_t address, uint32_t size);  
            bool erase();        

        private:
            theStorage();  

        public:
            // remove copy constructors
            theStorage(const theStorage&) = delete;
            theStorage(theStorage&) = delete;
            // remove copy assignment operator
            void operator=(const theStorage&) = delete;
            void operator=(theStorage&) = delete;

        private:    
            struct Impl;
            Impl *pImpl;        
        };          
                
        
    }
        
}

 
 /** @}            
    end of group asse  
 **/
 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


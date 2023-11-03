
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "test.h"


#include "embot_core.h"

#include "hal2.h"
#include "hal_can.h"

#include "embot_os_rtos.h"

namespace test::can {


    constexpr hal_can_port_t port {hal_can_port1};
        
    embot::os::rtos::semaphore_t *lok {nullptr};
    
    void s_eo_canserv_onerror_can(void *p)
    {
        
    }
    
    bool rec {false};
    void s_eo_canserv_onrx_can(void *p)
    {
        // alert a thread or mark a variable
        rec = true;
    }    

    constexpr size_t burstTXsize {16};
    size_t rxn {0};
    size_t frames2wait {0};
    void s_eo_canserv_ontx_can(void *p)
    {
        // mark a variable
        rxn++;
        if(rxn >= frames2wait)
        {
            embot::os::rtos::semaphore_release(lok);
        }
    }  
    
    void waitprepare(size_t n)
    {
        rxn = 0;
        frames2wait = n;
        // embot::os::rtos::semaphore_acquire(lok, embot::core::reltimeWaitForever);        
    }
    
    void wait()
    {
        embot::os::rtos::semaphore_acquire(lok, embot::core::reltimeWaitForever);
        // embot::os::rtos::semaphore_release(lok);
    }
    
    void load(hal_can_cfg_t &can_cfg)
    {
        can_cfg.runmode                     = hal_can_runmode_isr_1txq1rxq;
        can_cfg.baudrate                    = hal_can_baudrate_1mbps; 
        can_cfg.priorx                      = hal_int_priority11;
        can_cfg.priotx                      = hal_int_priority11;
     
        can_cfg.capacityofrxfifoofframes    = 32;
        can_cfg.capacityoftxfifoofframes    = 32;
        can_cfg.capacityoftxfifohighprio    = 0;
         
        can_cfg.callback_on_rx              = s_eo_canserv_onrx_can;
        can_cfg.arg_cb_rx                   = (void*)hal_can_port1;
        can_cfg.callback_on_tx              = s_eo_canserv_ontx_can;
        can_cfg.arg_cb_tx                   = (void*)hal_can_port1;
        can_cfg.callback_on_err             = s_eo_canserv_onerror_can;
        can_cfg.arg_cb_err                  = (void*)hal_can_port1;        
    }
    
    
    void init()
    {
        hal_can_cfg_t cfg {};
        load(cfg);
                       
        hal_can_init(port, &cfg);   
        hal_can_enable(port);
            
        lok = embot::os::rtos::semaphore_new(10, 0);
    }

    void deinit()
    {
        // deinit is not available
        hal_can_disable(port);
    }


    void fill(hal_can_frame_t &f, const uint16_t id, const std::initializer_list<uint8_t> &data)
    {
        f.id = id;
        f.id_type = hal_can_frameID_std;
        f.frame_type = hal_can_frame_data;
        f.size = data.size();
        uint8_t i {0};
        std::memset(f.data, 0, sizeof(f.data));
        for(const auto &d: data)
        {
            if(i<8)
            {
                f.data[i] = d;
                i++;
            }
        }
    }

    std::string to_string(const hal_can_frame_t &f)
    {
        char i[8] = {0};
        char d[40] = {0};
        snprintf(i, sizeof(i), "%03X", f.id);
        snprintf(d, sizeof(d), "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", 
                 f.data[0], f.data[1], f.data[2], f.data[3], f.data[4], f.data[5], f.data[6], f.data[7]);
        return std::string("[ID, size, data] = [") + i + ", " + std::to_string(f.size) + ", " + d + "]";       
    }    
    

    void tick()
    {
        static uint8_t tt {0};
        tt++;
        
        uint8_t num = 0;
        if(true == rec)
        {
            rec = false;                        
        }
        
        hal_can_received(port, &num);
        
        if(num > 0)
        {
            uint8_t rem {0};
            for(size_t i=0; i<num; i++)
            {
                hal_can_frame_t frame {};
                hal_can_get(port, &frame, &rem);
                embot::core::print("rx frame is: " + to_string(frame));
            }
            
        }
        
        hal_can_frame_t frame1 {};
        
        for(uint8_t n=0; n<burstTXsize; n++)
        {
            fill(frame1, 0x001, {n, tt, 1, 1, 1, 1, 1, 1});    
            embot::core::print("addind tx frame: " + to_string(frame1));
            hal_can_put(port, &frame1, hal_can_send_normprio_later);
        }



        uint8_t nn {0};
        hal_can_out_get(port, &nn);
        
        waitprepare(nn);

        embot::core::Time start {embot::core::now()};
        embot::core::TimeFormatter tf {start};
        embot::core::print(tf.to_string() + ": transmitting " + std::to_string(nn) + " frames");
        
        hal_can_transmit(port);
        
        wait();
        embot::core::Time endt {embot::core::now()};
        embot::core::print(embot::core::TimeFormatter(endt).to_string() + ": done in " + embot::core::TimeFormatter(endt-start).to_string());
        
    }

}


void test_embot_hw_init()
{
    test::can::init();   
}


void test_embot_hw_tick()
{   
    test::can::tick();
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


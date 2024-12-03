/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#if 0

### How to use `embot::app::eth::theApplication`

Just ...

```c++
#include "embot_app_eth_theApplication.h"
#include "embot_hw_bsp_amc.h"
   
int main(void)
{
    embot::hw::bsp::amc::set(embot::hw::bsp::amc::OnSpecApplication);
    embot::app::eth::theApplication::getInstance().start({});
}
```

and ... produce a file named `theApplication_config.h`  w/ following definition:

```C++
    constexpr theApplication::Config theApplication_Config
    {
        .property =
        {
            Process::eApplication,
            {3, 0}, 
            {2022, Month::Apr, Day::fourteen, 15, 16}
        },
        .OStick = 1000*embot::core::time1microsec,
        ... 
        etc
```


#endif


#include "embot_app_eth_theApplication.h"
 
#include "embot_hw_dualcore.h"

constexpr embot::hw::dualcore::Config dcc {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::release }; // donothing };


int main(void)
{
    embot::hw::dualcore::config(dcc);
    embot::app::eth::theApplication::getInstance().start({});
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


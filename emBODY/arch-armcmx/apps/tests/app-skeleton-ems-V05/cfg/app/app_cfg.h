
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _APP_CFG_H_
#define _APP_CFG_H_

// --------------------------------------------------------------------------------------------------------------------
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------

// <h> Configuration of application 
// <i> It holds configuration app-skeleton-ems v 5




// <h> Ethernet network info
// <i> ... 


//   <o> number of datagram in queue         <2-5>
//   <i> ....
#ifndef DGRAMPAYLOAD_NUM  
 #define DGRAMPAYLOAD_NUM   2
#endif


//   <o> control datagram payload size         <64-1000>
//   <i> ....
#ifndef DGRAMPAYLOAD_SIZE   
 #define DGRAMPAYLOAD_SIZE   256
#endif



//   <o> local port where waiting datagram         <0-65535>
//   <i> ....
#ifndef LOCALPORT_ONRX            
 #define LOCALPORT_ONRX   3333
#endif


//  <h> Fixed remote host IP address
//  <i> ....
//  <i> ....

//   <o> IP0 <0-255> 
#ifndef REM_IP0
 #define REM_IP0        10
#endif
//   <o> IP1 <0-255> 
#ifndef REM_IP1
 #define REM_IP1        0
#endif
//   <o> IP2 <0-255> 
#ifndef REM_IP2
 #define REM_IP2        5
#endif
//   <o> IP3 <0-255> 
#ifndef REM_IP3
 #define REM_IP3       2
#endif
//  </h> Fixed IP address

//   <o> remote port where send         <0-65535>
//   <i> ....
#ifndef REMPORT             
 #define REMPORT   3333
#endif

// </h> Ethernet network info


// <h> encoders reader
// <i> .....

// <h> connected encoders 
// <e> encoder 1          
// <i> .........

#ifndef ENC1_ENABLE
 #define ENC1_ENABLE  0     
#endif
// </e>

// <e> encoder 2          
// <i> .......
#ifndef ENC2_ENABLE
 #define ENC2_ENABLE  0     
#endif
// </e>

// <e> encoder 3          
// <i> ........
#ifndef ENC3_ENABLE
 #define ENC3_ENABLE  0     
#endif
// </e>

// <e> encoder 4          
// <i> not present on EMS
#ifndef ENC4_ENABLE
 #define ENC4_ENABLE  0     
#endif
// </e>

// <e> encoder 5          
// <i> not present on EMS
#ifndef ENC5_ENABLE
 #define ENC5_ENABLE  0     
#endif
// </e>


// <e> encoder 6          
// <i> not present on EMS
#ifndef ENC6_ENABLE
 #define ENC6_ENABLE  0     
#endif
// </e>

// <e> encoder 7          
// <i> sssssssss
#ifndef ENC7_ENABLE
 #define ENC7_ENABLE  0     
#endif
// </e>

// <e> encoder 8          
// <i> sssssssss
#ifndef ENC8_ENABLE
 #define ENC8_ENABLE  0     
#endif
// </e>

// <e> encoder 9          
// <i> sssssssss
#ifndef ENC9_ENABLE
 #define ENC9_ENABLE  0     
#endif
// </e>
// </h> Encoders connected


// <h> Endpoint configuration
// <i> .....
// </h>

// </h>






// --------------------------------------------------------------------------------------------------------------------
//------------- <<< end of configuration section >>> ------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



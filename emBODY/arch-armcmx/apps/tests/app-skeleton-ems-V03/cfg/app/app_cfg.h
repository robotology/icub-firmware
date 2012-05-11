
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _APP_CFG_H_
#define _APP_CFG_H_

// --------------------------------------------------------------------------------------------------------------------
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------

// <h> Configuration of application modules
// <i> It holds configuration app-skeleton-ems v 3



// <h> system Controller
// <i> ... 

//   <o> number of control datagram payload in queue         <2-5>
//   <i> ....
#ifndef SYSCNTRL_DGRAMPAYLOAD_NUM  
 #define SYSCNTRL_DGRAMPAYLOAD_NUM   2
#endif


//   <o> control datagram payload size         <64-256>
//   <i> .....
#ifndef SYSCNTRL_DGRAMPAYLOAD_SIZE   
 #define SYSCNTRL_DGRAMPAYLOAD_SIZE   64
#endif


//   <o> local port where waiting datagram         <0-65535>
//   <i> ....
#ifndef SYSCNTRL_LOCALPORT_ONRX             
 #define SYSCNTRL_LOCALPORT_ONRX   3333
#endif

//  <h> Fixed remote IP address
//  <i> 
//  <i>

//   <o> IP0 <0-255> 
#ifndef SYSCNTRL_REM_IP0
 #define SYSCNTRL_REM_IP0        1
#endif
//   <o> IP1 <0-255> 
#ifndef SYSCNTRL_REM_IP1
 #define SYSCNTRL_REM_IP1        1
#endif
//   <o> IP2 <0-255> 
#ifndef SYSCNTRL_REM_IP2
 #define SYSCNTRL_REM_IP2        1
#endif
//   <o> IP3 <0-255> 
#ifndef SYSCNTRL_REM_IP3
 #define SYSCNTRL_REM_IP3       1
#endif
//  </h> Fixed remote IP address

//   <o> remote port where send         <0-65535>
//   <i> ....
#ifndef SYSCNTRL_REMPORT             
 #define SYSCNTRL_REMPORT   3333
#endif

// </h>system Controller



// <h> motor controller
// <i> ... 

//   <o> number of datagram in queue         <2-5>
//   <i> ....
#ifndef MC_DGRAMPAYLOAD_NUM  
 #define MC_DGRAMPAYLOAD_NUM   2
#endif


//   <o> control datagram payload size         <64-256>
//   <i> ....
#ifndef MC_DGRAMPAYLOAD_SIZE   
 #define MC_DGRAMPAYLOAD_SIZE   64
#endif


//   <o> local port where waiting datagram         <0-65535>
//   <i> ....
#ifndef MC_LOCALPORT_ONRX            
 #define MC_LOCALPORT_ONRX   3334
#endif


//  <h> Fixed remote IP address
//  <i> ....
//  <i> ....

//   <o> IP0 <0-255> 
#ifndef MC_REM_IP0
 #define MC_REM_IP0        1
#endif
//   <o> IP1 <0-255> 
#ifndef MC_REM_IP1
 #define MC_REM_IP1        1
#endif
//   <o> IP2 <0-255> 
#ifndef MC_REM_IP2
 #define MC_REM_IP2        1
#endif
//   <o> IP3 <0-255> 
#ifndef MC_REM_IP3
 #define MC_REM_IP3       1
#endif
//  </h> Fixed IP address

//   <o> remote port where send         <0-65535>
//   <i> ....
#ifndef MC_REMPORT             
 #define MC_REMPORT   3334
#endif

// </h> motor controller


// <h> encoders reader
// <i> .....

//   <o> connected encoder mask         <0x0-0x1FF>
//   <i> default value
#ifndef ENCREADER_ENCCONNECTEDMASK  
 #define ENCREADER_ENCCONNECTEDMASK   0x1C7
#endif

// <h> connected encoders 
// <e> encoder 1          
// <i> .........

#ifndef ENC1_ENABLE
 #define ENC1_ENABLE  1     
#endif
// </e>

// <e> encoder 2          
// <i> .......
#ifndef ENC2_ENABLE
 #define ENC2_ENABLE  1     
#endif
// </e>

// <e> encoder 3          
// <i> ........
#ifndef ENC3_ENABLE
 #define ENC3_ENABLE  1     
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
 #define ENC7_ENABLE  1     
#endif
// </e>

// <e> encoder 8          
// <i> sssssssss
#ifndef ENC8_ENABLE
 #define ENC8_ENABLE  1     
#endif
// </e>

// <e> encoder 9          
// <i> sssssssss
#ifndef ENC9_ENABLE
 #define ENC9_ENABLE  1     
#endif
// </e>
// </h> Encoders connected


// </h>






// --------------------------------------------------------------------------------------------------------------------
//------------- <<< end of configuration section >>> ------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



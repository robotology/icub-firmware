
marco.accame on 23jan19:
i removed stm32l4xx_hal_msp.c from compilation in here because ... 
i have added it in the .lib because other projects (bootloader and updaterofbootloader) needs it
from the library i have removed the dangerous file stm32l4xx_hal_msp_template.c which contains
empty (and sic, not weak) definition of HAL_MspInit(). i believe that this latter was the problem.

if the usb cannot be seen, however, we can simply add again this stm32l4xx_hal_msp.c in here.
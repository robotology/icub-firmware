/*
 * cmsisrtx_irq.c
 *
 * cmsisrtx/embot: wrapper which lets uVision build the GNU-syntax RTX handlers (SVC, PendSV, SysTick) with armclang.
 * uVision decides the tool by file extension: a .S file goes to armasm (which does not understand GNU syntax),
 * a .c file goes to armclang. So this .c file is compiled with the per-file option "-x assembler-with-cpp" and it
 * only includes the original, unmodified RTX file. Keep only C-style comments here: the file is seen as assembler.
 */
#include "../CMSIS-RTX/Source/GCC/irq_armv7m.S"

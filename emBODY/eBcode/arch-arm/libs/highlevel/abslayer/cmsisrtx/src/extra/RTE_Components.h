// RTE_Components.h
// Hand-written replacement of the file that the CMSIS-Pack RTE would generate. This project does not use the RTE:
// the RTX and CMSIS sources are in the repository, under cmsisrtx/src. RTX includes it unconditionally.

#ifndef RTE_COMPONENTS_H
#define RTE_COMPONENTS_H

#define CMSIS_device_header     "cmsisrtx_device.h"

// CMSISRTX_FAST (targets fast.cm4 / fast.cm7): the lib for speed and for the measures. It is imposed here, because every RTX
// source includes this file before it looks at the macros: no Event Viewer bridge (no ITM write at every thread switch),
// and RTX_Config.h switches off the stack check and the stack watermark.
#if defined(CMSISRTX_FAST) && defined(CMSISRTX_EVENTVIEWER)
#undef CMSISRTX_EVENTVIEWER
#endif

#endif

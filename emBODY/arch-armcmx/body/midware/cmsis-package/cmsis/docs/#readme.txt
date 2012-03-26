
-----------------------------------------------------------------------------------------------------------------------
- CMSIS
-----------------------------------------------------------------------------------------------------------------------


The CMSIS is taken from ARM distribution plus extension from silicon vendors. 


-----------------------------------------------------------------------------------------------------------------------
- HOW TO INSTALL
-----------------------------------------------------------------------------------------------------------------------

- copy files from cmsis-src into directory C:\iEmb\midware\cmsis\src
- copy files from cmsis-api into directory C:\iEmb\midware\cmsis\api
- copy files from cmsis-cfg into directory C:\iEmb\midware\cmsis\cfg

and set them read-only.


-----------------------------------------------------------------------------------------------------------------------
- HOW TO USE
-----------------------------------------------------------------------------------------------------------------------

From src include (or copy w/out changing) the following into your project:
- core_cm3.c, system_stm32f10x.c (if stm32).

From cfg copy into a local directory the following and include into your project to change according to you needs: 
- startup_stm32f10x_cl.s (if stm32 communication line) or other.

Include the following files to see the API:
- system_stm32f10x.c (if stm32).

Set include path to C:\iEmb\midware\cmsis\api

Define env variable STM32F10X_CL in compiler (if stm32 communication line) or other.


-----------------------------------------------------------------------------------------------------------------------
- HOW IT WAS DEVELOPED
-----------------------------------------------------------------------------------------------------------------------

core part:

It was taken the CMSIS package from ARM, v2.00 and moved the files in the relevant directories.  This directory
reorganisation is done to avoid the copy of gloabl files into private projects.

device part:

From CMSIS v2.00, the silicon vendor's porting is not provided by ARM anymore. For this reason, it was taken the latest
version of the files from ST's distribution of the stm32f10x_stdperiph_lib.



-----------------------------------------------------------------------------------------------------------------------
- HOW TO UPGRADE IT
-----------------------------------------------------------------------------------------------------------------------

Just copy new files form a new distribution into relevant directories.




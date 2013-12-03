#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/appl-strg-embobj.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/appl-strg-embobj.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../app/main.c ../../app/eos_applembobj_info.c ../../app/eos_applembobj_spec.c ../../../../../../../embobj/core/core/EOVmutex.c ../../../../../../../embobj/core/core/EOVtask.c ../../../../../../../embobj/core/core/EOVtheCallbackManager.c ../../../../../../../embobj/core/core/EOVtheSystem.c ../../../../../../../embobj/core/core/EOVtheTimerManager.c ../../../../../../../embobj/core/core/EOaction.c ../../../../../../../embobj/core/core/EOarray.c ../../../../../../../embobj/core/core/EOconstarray.c ../../../../../../../embobj/core/core/EOconstvector.c ../../../../../../../embobj/core/core/EOdeque.c ../../../../../../../embobj/core/core/EOfifo.c ../../../../../../../embobj/core/core/EOfifoByte.c ../../../../../../../embobj/core/core/EOfifoWord.c ../../../../../../../embobj/core/core/EOlist.c ../../../../../../../embobj/core/core/EOtheErrorManager.c ../../../../../../../embobj/core/core/EOtheMemoryPool.c ../../../../../../../embobj/core/core/EOtimer.c ../../../../../../../embobj/core/core/EOvector.c ../../../../../../../embobj/core/core/EoCommon.c ../../../../../../../embobj/core/exec/singletask/EOSmutex.c ../../../../../../../embobj/core/exec/singletask/EOStheCallbackManager.c ../../../../../../../embobj/core/exec/singletask/EOStheFOOP.c ../../../../../../../embobj/core/exec/singletask/EOStheSystem.c ../../../../../../../embobj/core/exec/singletask/EOStheTimerManager.c ../../../../../../libs/abslayer/hal/src/utils/crc16.c ../../../../../../libs/abslayer/hal/src/utils/crc32.c ../../../../../../libs/abslayer/hal/src/hal_brdcfg.c ../../../../../../libs/abslayer/hal/src/hal_dspic_arch.c ../../../../../../libs/abslayer/hal/src/hal_dspic_base.c ../../../../../../libs/abslayer/hal/src/hal_dspic_can.c ../../../../../../libs/abslayer/hal/src/hal_dspic_crc.c ../../../../../../libs/abslayer/hal/src/hal_dspic_eeprom.c ../../../../../../libs/abslayer/hal/src/hal_dspic_flash.c ../../../../../../libs/abslayer/hal/src/hal_dspic_gpio.c ../../../../../../libs/abslayer/hal/src/hal_dspic_led.c ../../../../../../libs/abslayer/hal/src/hal_dspic_sys.c ../../../../../../libs/abslayer/hal/src/hal_dspic_timer.c ../../hal-cfg/hal_cfg.c ../../hal-stg/hal_dspic_startup.c ../../../../../../libs/services/common/src/eEcommon.c ../../../../../../libs/services/storage/private/src/eEprivateStorage.c ../../../../../../libs/services/storage/shared/src/eEpermdata.c ../../../../../../libs/services/storage/shared/src/eEsharedInfo.c ../../../../../../libs/services/storage/shared/src/eEsharedStorage.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1445257345/main.o ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o ${OBJECTDIR}/_ext/773597708/EOVmutex.o ${OBJECTDIR}/_ext/773597708/EOVtask.o ${OBJECTDIR}/_ext/773597708/EOVtheCallbackManager.o ${OBJECTDIR}/_ext/773597708/EOVtheSystem.o ${OBJECTDIR}/_ext/773597708/EOVtheTimerManager.o ${OBJECTDIR}/_ext/773597708/EOaction.o ${OBJECTDIR}/_ext/773597708/EOarray.o ${OBJECTDIR}/_ext/773597708/EOconstarray.o ${OBJECTDIR}/_ext/773597708/EOconstvector.o ${OBJECTDIR}/_ext/773597708/EOdeque.o ${OBJECTDIR}/_ext/773597708/EOfifo.o ${OBJECTDIR}/_ext/773597708/EOfifoByte.o ${OBJECTDIR}/_ext/773597708/EOfifoWord.o ${OBJECTDIR}/_ext/773597708/EOlist.o ${OBJECTDIR}/_ext/773597708/EOtheErrorManager.o ${OBJECTDIR}/_ext/773597708/EOtheMemoryPool.o ${OBJECTDIR}/_ext/773597708/EOtimer.o ${OBJECTDIR}/_ext/773597708/EOvector.o ${OBJECTDIR}/_ext/773597708/EoCommon.o ${OBJECTDIR}/_ext/1361983106/EOSmutex.o ${OBJECTDIR}/_ext/1361983106/EOStheCallbackManager.o ${OBJECTDIR}/_ext/1361983106/EOStheFOOP.o ${OBJECTDIR}/_ext/1361983106/EOStheSystem.o ${OBJECTDIR}/_ext/1361983106/EOStheTimerManager.o ${OBJECTDIR}/_ext/757185051/crc16.o ${OBJECTDIR}/_ext/757185051/crc32.o ${OBJECTDIR}/_ext/1116683527/hal_brdcfg.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_arch.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_base.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_can.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_crc.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_eeprom.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_flash.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_gpio.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_led.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_sys.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_timer.o ${OBJECTDIR}/_ext/1482636426/hal_cfg.o ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o ${OBJECTDIR}/_ext/1673993744/eEcommon.o ${OBJECTDIR}/_ext/883723884/eEprivateStorage.o ${OBJECTDIR}/_ext/339291006/eEpermdata.o ${OBJECTDIR}/_ext/339291006/eEsharedInfo.o ${OBJECTDIR}/_ext/339291006/eEsharedStorage.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1445257345/main.o.d ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o.d ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o.d ${OBJECTDIR}/_ext/773597708/EOVmutex.o.d ${OBJECTDIR}/_ext/773597708/EOVtask.o.d ${OBJECTDIR}/_ext/773597708/EOVtheCallbackManager.o.d ${OBJECTDIR}/_ext/773597708/EOVtheSystem.o.d ${OBJECTDIR}/_ext/773597708/EOVtheTimerManager.o.d ${OBJECTDIR}/_ext/773597708/EOaction.o.d ${OBJECTDIR}/_ext/773597708/EOarray.o.d ${OBJECTDIR}/_ext/773597708/EOconstarray.o.d ${OBJECTDIR}/_ext/773597708/EOconstvector.o.d ${OBJECTDIR}/_ext/773597708/EOdeque.o.d ${OBJECTDIR}/_ext/773597708/EOfifo.o.d ${OBJECTDIR}/_ext/773597708/EOfifoByte.o.d ${OBJECTDIR}/_ext/773597708/EOfifoWord.o.d ${OBJECTDIR}/_ext/773597708/EOlist.o.d ${OBJECTDIR}/_ext/773597708/EOtheErrorManager.o.d ${OBJECTDIR}/_ext/773597708/EOtheMemoryPool.o.d ${OBJECTDIR}/_ext/773597708/EOtimer.o.d ${OBJECTDIR}/_ext/773597708/EOvector.o.d ${OBJECTDIR}/_ext/773597708/EoCommon.o.d ${OBJECTDIR}/_ext/1361983106/EOSmutex.o.d ${OBJECTDIR}/_ext/1361983106/EOStheCallbackManager.o.d ${OBJECTDIR}/_ext/1361983106/EOStheFOOP.o.d ${OBJECTDIR}/_ext/1361983106/EOStheSystem.o.d ${OBJECTDIR}/_ext/1361983106/EOStheTimerManager.o.d ${OBJECTDIR}/_ext/757185051/crc16.o.d ${OBJECTDIR}/_ext/757185051/crc32.o.d ${OBJECTDIR}/_ext/1116683527/hal_brdcfg.o.d ${OBJECTDIR}/_ext/1116683527/hal_dspic_arch.o.d ${OBJECTDIR}/_ext/1116683527/hal_dspic_base.o.d ${OBJECTDIR}/_ext/1116683527/hal_dspic_can.o.d ${OBJECTDIR}/_ext/1116683527/hal_dspic_crc.o.d ${OBJECTDIR}/_ext/1116683527/hal_dspic_eeprom.o.d ${OBJECTDIR}/_ext/1116683527/hal_dspic_flash.o.d ${OBJECTDIR}/_ext/1116683527/hal_dspic_gpio.o.d ${OBJECTDIR}/_ext/1116683527/hal_dspic_led.o.d ${OBJECTDIR}/_ext/1116683527/hal_dspic_sys.o.d ${OBJECTDIR}/_ext/1116683527/hal_dspic_timer.o.d ${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o.d ${OBJECTDIR}/_ext/1673993744/eEcommon.o.d ${OBJECTDIR}/_ext/883723884/eEprivateStorage.o.d ${OBJECTDIR}/_ext/339291006/eEpermdata.o.d ${OBJECTDIR}/_ext/339291006/eEsharedInfo.o.d ${OBJECTDIR}/_ext/339291006/eEsharedStorage.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1445257345/main.o ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o ${OBJECTDIR}/_ext/773597708/EOVmutex.o ${OBJECTDIR}/_ext/773597708/EOVtask.o ${OBJECTDIR}/_ext/773597708/EOVtheCallbackManager.o ${OBJECTDIR}/_ext/773597708/EOVtheSystem.o ${OBJECTDIR}/_ext/773597708/EOVtheTimerManager.o ${OBJECTDIR}/_ext/773597708/EOaction.o ${OBJECTDIR}/_ext/773597708/EOarray.o ${OBJECTDIR}/_ext/773597708/EOconstarray.o ${OBJECTDIR}/_ext/773597708/EOconstvector.o ${OBJECTDIR}/_ext/773597708/EOdeque.o ${OBJECTDIR}/_ext/773597708/EOfifo.o ${OBJECTDIR}/_ext/773597708/EOfifoByte.o ${OBJECTDIR}/_ext/773597708/EOfifoWord.o ${OBJECTDIR}/_ext/773597708/EOlist.o ${OBJECTDIR}/_ext/773597708/EOtheErrorManager.o ${OBJECTDIR}/_ext/773597708/EOtheMemoryPool.o ${OBJECTDIR}/_ext/773597708/EOtimer.o ${OBJECTDIR}/_ext/773597708/EOvector.o ${OBJECTDIR}/_ext/773597708/EoCommon.o ${OBJECTDIR}/_ext/1361983106/EOSmutex.o ${OBJECTDIR}/_ext/1361983106/EOStheCallbackManager.o ${OBJECTDIR}/_ext/1361983106/EOStheFOOP.o ${OBJECTDIR}/_ext/1361983106/EOStheSystem.o ${OBJECTDIR}/_ext/1361983106/EOStheTimerManager.o ${OBJECTDIR}/_ext/757185051/crc16.o ${OBJECTDIR}/_ext/757185051/crc32.o ${OBJECTDIR}/_ext/1116683527/hal_brdcfg.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_arch.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_base.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_can.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_crc.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_eeprom.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_flash.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_gpio.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_led.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_sys.o ${OBJECTDIR}/_ext/1116683527/hal_dspic_timer.o ${OBJECTDIR}/_ext/1482636426/hal_cfg.o ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o ${OBJECTDIR}/_ext/1673993744/eEcommon.o ${OBJECTDIR}/_ext/883723884/eEprivateStorage.o ${OBJECTDIR}/_ext/339291006/eEpermdata.o ${OBJECTDIR}/_ext/339291006/eEsharedInfo.o ${OBJECTDIR}/_ext/339291006/eEsharedStorage.o

# Source Files
SOURCEFILES=../../app/main.c ../../app/eos_applembobj_info.c ../../app/eos_applembobj_spec.c ../../../../../../../embobj/core/core/EOVmutex.c ../../../../../../../embobj/core/core/EOVtask.c ../../../../../../../embobj/core/core/EOVtheCallbackManager.c ../../../../../../../embobj/core/core/EOVtheSystem.c ../../../../../../../embobj/core/core/EOVtheTimerManager.c ../../../../../../../embobj/core/core/EOaction.c ../../../../../../../embobj/core/core/EOarray.c ../../../../../../../embobj/core/core/EOconstarray.c ../../../../../../../embobj/core/core/EOconstvector.c ../../../../../../../embobj/core/core/EOdeque.c ../../../../../../../embobj/core/core/EOfifo.c ../../../../../../../embobj/core/core/EOfifoByte.c ../../../../../../../embobj/core/core/EOfifoWord.c ../../../../../../../embobj/core/core/EOlist.c ../../../../../../../embobj/core/core/EOtheErrorManager.c ../../../../../../../embobj/core/core/EOtheMemoryPool.c ../../../../../../../embobj/core/core/EOtimer.c ../../../../../../../embobj/core/core/EOvector.c ../../../../../../../embobj/core/core/EoCommon.c ../../../../../../../embobj/core/exec/singletask/EOSmutex.c ../../../../../../../embobj/core/exec/singletask/EOStheCallbackManager.c ../../../../../../../embobj/core/exec/singletask/EOStheFOOP.c ../../../../../../../embobj/core/exec/singletask/EOStheSystem.c ../../../../../../../embobj/core/exec/singletask/EOStheTimerManager.c ../../../../../../libs/abslayer/hal/src/utils/crc16.c ../../../../../../libs/abslayer/hal/src/utils/crc32.c ../../../../../../libs/abslayer/hal/src/hal_brdcfg.c ../../../../../../libs/abslayer/hal/src/hal_dspic_arch.c ../../../../../../libs/abslayer/hal/src/hal_dspic_base.c ../../../../../../libs/abslayer/hal/src/hal_dspic_can.c ../../../../../../libs/abslayer/hal/src/hal_dspic_crc.c ../../../../../../libs/abslayer/hal/src/hal_dspic_eeprom.c ../../../../../../libs/abslayer/hal/src/hal_dspic_flash.c ../../../../../../libs/abslayer/hal/src/hal_dspic_gpio.c ../../../../../../libs/abslayer/hal/src/hal_dspic_led.c ../../../../../../libs/abslayer/hal/src/hal_dspic_sys.c ../../../../../../libs/abslayer/hal/src/hal_dspic_timer.c ../../hal-cfg/hal_cfg.c ../../hal-stg/hal_dspic_startup.c ../../../../../../libs/services/common/src/eEcommon.c ../../../../../../libs/services/storage/private/src/eEprivateStorage.c ../../../../../../libs/services/storage/shared/src/eEpermdata.c ../../../../../../libs/services/storage/shared/src/eEsharedInfo.c ../../../../../../libs/services/storage/shared/src/eEsharedStorage.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/appl-strg-embobj.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ128MC802
MP_LINKER_FILE_OPTION=,--script="..\eElinkerscript_applic.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1445257345/main.o: ../../app/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/main.c  -o ${OBJECTDIR}/_ext/1445257345/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o: ../../app/eos_applembobj_info.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/eos_applembobj_info.c  -o ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o: ../../app/eos_applembobj_spec.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/eos_applembobj_spec.c  -o ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOVmutex.o: ../../../../../../../embobj/core/core/EOVmutex.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVmutex.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVmutex.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOVmutex.c  -o ${OBJECTDIR}/_ext/773597708/EOVmutex.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOVmutex.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOVmutex.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOVtask.o: ../../../../../../../embobj/core/core/EOVtask.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVtask.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVtask.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOVtask.c  -o ${OBJECTDIR}/_ext/773597708/EOVtask.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOVtask.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOVtask.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOVtheCallbackManager.o: ../../../../../../../embobj/core/core/EOVtheCallbackManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVtheCallbackManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVtheCallbackManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOVtheCallbackManager.c  -o ${OBJECTDIR}/_ext/773597708/EOVtheCallbackManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOVtheCallbackManager.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOVtheCallbackManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOVtheSystem.o: ../../../../../../../embobj/core/core/EOVtheSystem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVtheSystem.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVtheSystem.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOVtheSystem.c  -o ${OBJECTDIR}/_ext/773597708/EOVtheSystem.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOVtheSystem.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOVtheSystem.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOVtheTimerManager.o: ../../../../../../../embobj/core/core/EOVtheTimerManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVtheTimerManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVtheTimerManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOVtheTimerManager.c  -o ${OBJECTDIR}/_ext/773597708/EOVtheTimerManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOVtheTimerManager.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOVtheTimerManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOaction.o: ../../../../../../../embobj/core/core/EOaction.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOaction.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOaction.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOaction.c  -o ${OBJECTDIR}/_ext/773597708/EOaction.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOaction.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOaction.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOarray.o: ../../../../../../../embobj/core/core/EOarray.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOarray.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOarray.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOarray.c  -o ${OBJECTDIR}/_ext/773597708/EOarray.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOarray.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOarray.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOconstarray.o: ../../../../../../../embobj/core/core/EOconstarray.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOconstarray.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOconstarray.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOconstarray.c  -o ${OBJECTDIR}/_ext/773597708/EOconstarray.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOconstarray.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOconstarray.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOconstvector.o: ../../../../../../../embobj/core/core/EOconstvector.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOconstvector.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOconstvector.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOconstvector.c  -o ${OBJECTDIR}/_ext/773597708/EOconstvector.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOconstvector.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOconstvector.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOdeque.o: ../../../../../../../embobj/core/core/EOdeque.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOdeque.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOdeque.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOdeque.c  -o ${OBJECTDIR}/_ext/773597708/EOdeque.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOdeque.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOdeque.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOfifo.o: ../../../../../../../embobj/core/core/EOfifo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOfifo.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOfifo.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOfifo.c  -o ${OBJECTDIR}/_ext/773597708/EOfifo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOfifo.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOfifo.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOfifoByte.o: ../../../../../../../embobj/core/core/EOfifoByte.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOfifoByte.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOfifoByte.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOfifoByte.c  -o ${OBJECTDIR}/_ext/773597708/EOfifoByte.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOfifoByte.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOfifoByte.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOfifoWord.o: ../../../../../../../embobj/core/core/EOfifoWord.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOfifoWord.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOfifoWord.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOfifoWord.c  -o ${OBJECTDIR}/_ext/773597708/EOfifoWord.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOfifoWord.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOfifoWord.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOlist.o: ../../../../../../../embobj/core/core/EOlist.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOlist.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOlist.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOlist.c  -o ${OBJECTDIR}/_ext/773597708/EOlist.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOlist.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOlist.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOtheErrorManager.o: ../../../../../../../embobj/core/core/EOtheErrorManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOtheErrorManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOtheErrorManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOtheErrorManager.c  -o ${OBJECTDIR}/_ext/773597708/EOtheErrorManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOtheErrorManager.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOtheErrorManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOtheMemoryPool.o: ../../../../../../../embobj/core/core/EOtheMemoryPool.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOtheMemoryPool.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOtheMemoryPool.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOtheMemoryPool.c  -o ${OBJECTDIR}/_ext/773597708/EOtheMemoryPool.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOtheMemoryPool.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOtheMemoryPool.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOtimer.o: ../../../../../../../embobj/core/core/EOtimer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOtimer.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOtimer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOtimer.c  -o ${OBJECTDIR}/_ext/773597708/EOtimer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOtimer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOtimer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOvector.o: ../../../../../../../embobj/core/core/EOvector.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOvector.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOvector.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOvector.c  -o ${OBJECTDIR}/_ext/773597708/EOvector.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOvector.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOvector.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EoCommon.o: ../../../../../../../embobj/core/core/EoCommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EoCommon.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EoCommon.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EoCommon.c  -o ${OBJECTDIR}/_ext/773597708/EoCommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EoCommon.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EoCommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1361983106/EOSmutex.o: ../../../../../../../embobj/core/exec/singletask/EOSmutex.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1361983106 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOSmutex.o.d 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOSmutex.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/exec/singletask/EOSmutex.c  -o ${OBJECTDIR}/_ext/1361983106/EOSmutex.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1361983106/EOSmutex.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1361983106/EOSmutex.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1361983106/EOStheCallbackManager.o: ../../../../../../../embobj/core/exec/singletask/EOStheCallbackManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1361983106 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOStheCallbackManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOStheCallbackManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/exec/singletask/EOStheCallbackManager.c  -o ${OBJECTDIR}/_ext/1361983106/EOStheCallbackManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1361983106/EOStheCallbackManager.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1361983106/EOStheCallbackManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1361983106/EOStheFOOP.o: ../../../../../../../embobj/core/exec/singletask/EOStheFOOP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1361983106 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOStheFOOP.o.d 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOStheFOOP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/exec/singletask/EOStheFOOP.c  -o ${OBJECTDIR}/_ext/1361983106/EOStheFOOP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1361983106/EOStheFOOP.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1361983106/EOStheFOOP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1361983106/EOStheSystem.o: ../../../../../../../embobj/core/exec/singletask/EOStheSystem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1361983106 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOStheSystem.o.d 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOStheSystem.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/exec/singletask/EOStheSystem.c  -o ${OBJECTDIR}/_ext/1361983106/EOStheSystem.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1361983106/EOStheSystem.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1361983106/EOStheSystem.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1361983106/EOStheTimerManager.o: ../../../../../../../embobj/core/exec/singletask/EOStheTimerManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1361983106 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOStheTimerManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOStheTimerManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/exec/singletask/EOStheTimerManager.c  -o ${OBJECTDIR}/_ext/1361983106/EOStheTimerManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1361983106/EOStheTimerManager.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1361983106/EOStheTimerManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/757185051/crc16.o: ../../../../../../libs/abslayer/hal/src/utils/crc16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/757185051 
	@${RM} ${OBJECTDIR}/_ext/757185051/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/757185051/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/utils/crc16.c  -o ${OBJECTDIR}/_ext/757185051/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/757185051/crc16.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/757185051/crc16.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/757185051/crc32.o: ../../../../../../libs/abslayer/hal/src/utils/crc32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/757185051 
	@${RM} ${OBJECTDIR}/_ext/757185051/crc32.o.d 
	@${RM} ${OBJECTDIR}/_ext/757185051/crc32.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/utils/crc32.c  -o ${OBJECTDIR}/_ext/757185051/crc32.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/757185051/crc32.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/757185051/crc32.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_brdcfg.o: ../../../../../../libs/abslayer/hal/src/hal_brdcfg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_brdcfg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_brdcfg.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_brdcfg.c  -o ${OBJECTDIR}/_ext/1116683527/hal_brdcfg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_brdcfg.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_brdcfg.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_arch.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_arch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_arch.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_arch.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_arch.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_arch.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_arch.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_arch.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_base.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_base.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_base.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_base.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_base.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_base.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_base.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_base.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_can.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_can.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_can.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_can.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_can.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_can.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_can.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_crc.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_crc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_crc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_crc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_crc.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_crc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_crc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_crc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_eeprom.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_eeprom.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_eeprom.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_eeprom.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_eeprom.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_eeprom.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_eeprom.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_flash.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_flash.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_flash.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_flash.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_flash.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_flash.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_gpio.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_gpio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_gpio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_gpio.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_gpio.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_gpio.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_gpio.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_gpio.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_led.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_led.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_led.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_led.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_led.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_led.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_led.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_led.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_sys.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_sys.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_sys.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_sys.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_sys.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_sys.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_sys.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_sys.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_timer.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_timer.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_timer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_timer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1482636426/hal_cfg.o: ../../hal-cfg/hal_cfg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1482636426 
	@${RM} ${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1482636426/hal_cfg.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../hal-cfg/hal_cfg.c  -o ${OBJECTDIR}/_ext/1482636426/hal_cfg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o: ../../hal-stg/hal_dspic_startup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1482652236 
	@${RM} ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../hal-stg/hal_dspic_startup.c  -o ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1673993744/eEcommon.o: ../../../../../../libs/services/common/src/eEcommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1673993744 
	@${RM} ${OBJECTDIR}/_ext/1673993744/eEcommon.o.d 
	@${RM} ${OBJECTDIR}/_ext/1673993744/eEcommon.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/services/common/src/eEcommon.c  -o ${OBJECTDIR}/_ext/1673993744/eEcommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1673993744/eEcommon.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1673993744/eEcommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/883723884/eEprivateStorage.o: ../../../../../../libs/services/storage/private/src/eEprivateStorage.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/883723884 
	@${RM} ${OBJECTDIR}/_ext/883723884/eEprivateStorage.o.d 
	@${RM} ${OBJECTDIR}/_ext/883723884/eEprivateStorage.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/services/storage/private/src/eEprivateStorage.c  -o ${OBJECTDIR}/_ext/883723884/eEprivateStorage.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/883723884/eEprivateStorage.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/883723884/eEprivateStorage.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/339291006/eEpermdata.o: ../../../../../../libs/services/storage/shared/src/eEpermdata.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/339291006 
	@${RM} ${OBJECTDIR}/_ext/339291006/eEpermdata.o.d 
	@${RM} ${OBJECTDIR}/_ext/339291006/eEpermdata.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/services/storage/shared/src/eEpermdata.c  -o ${OBJECTDIR}/_ext/339291006/eEpermdata.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/339291006/eEpermdata.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/339291006/eEpermdata.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/339291006/eEsharedInfo.o: ../../../../../../libs/services/storage/shared/src/eEsharedInfo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/339291006 
	@${RM} ${OBJECTDIR}/_ext/339291006/eEsharedInfo.o.d 
	@${RM} ${OBJECTDIR}/_ext/339291006/eEsharedInfo.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/services/storage/shared/src/eEsharedInfo.c  -o ${OBJECTDIR}/_ext/339291006/eEsharedInfo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/339291006/eEsharedInfo.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/339291006/eEsharedInfo.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/339291006/eEsharedStorage.o: ../../../../../../libs/services/storage/shared/src/eEsharedStorage.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/339291006 
	@${RM} ${OBJECTDIR}/_ext/339291006/eEsharedStorage.o.d 
	@${RM} ${OBJECTDIR}/_ext/339291006/eEsharedStorage.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/services/storage/shared/src/eEsharedStorage.c  -o ${OBJECTDIR}/_ext/339291006/eEsharedStorage.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/339291006/eEsharedStorage.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/339291006/eEsharedStorage.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1445257345/main.o: ../../app/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/main.c  -o ${OBJECTDIR}/_ext/1445257345/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/main.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o: ../../app/eos_applembobj_info.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/eos_applembobj_info.c  -o ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o: ../../app/eos_applembobj_spec.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/eos_applembobj_spec.c  -o ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOVmutex.o: ../../../../../../../embobj/core/core/EOVmutex.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVmutex.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVmutex.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOVmutex.c  -o ${OBJECTDIR}/_ext/773597708/EOVmutex.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOVmutex.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOVmutex.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOVtask.o: ../../../../../../../embobj/core/core/EOVtask.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVtask.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVtask.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOVtask.c  -o ${OBJECTDIR}/_ext/773597708/EOVtask.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOVtask.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOVtask.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOVtheCallbackManager.o: ../../../../../../../embobj/core/core/EOVtheCallbackManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVtheCallbackManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVtheCallbackManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOVtheCallbackManager.c  -o ${OBJECTDIR}/_ext/773597708/EOVtheCallbackManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOVtheCallbackManager.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOVtheCallbackManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOVtheSystem.o: ../../../../../../../embobj/core/core/EOVtheSystem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVtheSystem.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVtheSystem.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOVtheSystem.c  -o ${OBJECTDIR}/_ext/773597708/EOVtheSystem.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOVtheSystem.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOVtheSystem.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOVtheTimerManager.o: ../../../../../../../embobj/core/core/EOVtheTimerManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVtheTimerManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOVtheTimerManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOVtheTimerManager.c  -o ${OBJECTDIR}/_ext/773597708/EOVtheTimerManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOVtheTimerManager.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOVtheTimerManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOaction.o: ../../../../../../../embobj/core/core/EOaction.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOaction.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOaction.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOaction.c  -o ${OBJECTDIR}/_ext/773597708/EOaction.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOaction.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOaction.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOarray.o: ../../../../../../../embobj/core/core/EOarray.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOarray.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOarray.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOarray.c  -o ${OBJECTDIR}/_ext/773597708/EOarray.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOarray.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOarray.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOconstarray.o: ../../../../../../../embobj/core/core/EOconstarray.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOconstarray.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOconstarray.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOconstarray.c  -o ${OBJECTDIR}/_ext/773597708/EOconstarray.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOconstarray.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOconstarray.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOconstvector.o: ../../../../../../../embobj/core/core/EOconstvector.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOconstvector.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOconstvector.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOconstvector.c  -o ${OBJECTDIR}/_ext/773597708/EOconstvector.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOconstvector.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOconstvector.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOdeque.o: ../../../../../../../embobj/core/core/EOdeque.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOdeque.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOdeque.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOdeque.c  -o ${OBJECTDIR}/_ext/773597708/EOdeque.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOdeque.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOdeque.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOfifo.o: ../../../../../../../embobj/core/core/EOfifo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOfifo.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOfifo.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOfifo.c  -o ${OBJECTDIR}/_ext/773597708/EOfifo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOfifo.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOfifo.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOfifoByte.o: ../../../../../../../embobj/core/core/EOfifoByte.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOfifoByte.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOfifoByte.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOfifoByte.c  -o ${OBJECTDIR}/_ext/773597708/EOfifoByte.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOfifoByte.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOfifoByte.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOfifoWord.o: ../../../../../../../embobj/core/core/EOfifoWord.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOfifoWord.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOfifoWord.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOfifoWord.c  -o ${OBJECTDIR}/_ext/773597708/EOfifoWord.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOfifoWord.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOfifoWord.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOlist.o: ../../../../../../../embobj/core/core/EOlist.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOlist.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOlist.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOlist.c  -o ${OBJECTDIR}/_ext/773597708/EOlist.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOlist.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOlist.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOtheErrorManager.o: ../../../../../../../embobj/core/core/EOtheErrorManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOtheErrorManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOtheErrorManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOtheErrorManager.c  -o ${OBJECTDIR}/_ext/773597708/EOtheErrorManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOtheErrorManager.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOtheErrorManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOtheMemoryPool.o: ../../../../../../../embobj/core/core/EOtheMemoryPool.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOtheMemoryPool.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOtheMemoryPool.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOtheMemoryPool.c  -o ${OBJECTDIR}/_ext/773597708/EOtheMemoryPool.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOtheMemoryPool.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOtheMemoryPool.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOtimer.o: ../../../../../../../embobj/core/core/EOtimer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOtimer.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOtimer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOtimer.c  -o ${OBJECTDIR}/_ext/773597708/EOtimer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOtimer.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOtimer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EOvector.o: ../../../../../../../embobj/core/core/EOvector.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOvector.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EOvector.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EOvector.c  -o ${OBJECTDIR}/_ext/773597708/EOvector.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EOvector.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EOvector.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/773597708/EoCommon.o: ../../../../../../../embobj/core/core/EoCommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/773597708 
	@${RM} ${OBJECTDIR}/_ext/773597708/EoCommon.o.d 
	@${RM} ${OBJECTDIR}/_ext/773597708/EoCommon.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/core/EoCommon.c  -o ${OBJECTDIR}/_ext/773597708/EoCommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/773597708/EoCommon.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/773597708/EoCommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1361983106/EOSmutex.o: ../../../../../../../embobj/core/exec/singletask/EOSmutex.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1361983106 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOSmutex.o.d 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOSmutex.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/exec/singletask/EOSmutex.c  -o ${OBJECTDIR}/_ext/1361983106/EOSmutex.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1361983106/EOSmutex.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1361983106/EOSmutex.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1361983106/EOStheCallbackManager.o: ../../../../../../../embobj/core/exec/singletask/EOStheCallbackManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1361983106 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOStheCallbackManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOStheCallbackManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/exec/singletask/EOStheCallbackManager.c  -o ${OBJECTDIR}/_ext/1361983106/EOStheCallbackManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1361983106/EOStheCallbackManager.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1361983106/EOStheCallbackManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1361983106/EOStheFOOP.o: ../../../../../../../embobj/core/exec/singletask/EOStheFOOP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1361983106 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOStheFOOP.o.d 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOStheFOOP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/exec/singletask/EOStheFOOP.c  -o ${OBJECTDIR}/_ext/1361983106/EOStheFOOP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1361983106/EOStheFOOP.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1361983106/EOStheFOOP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1361983106/EOStheSystem.o: ../../../../../../../embobj/core/exec/singletask/EOStheSystem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1361983106 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOStheSystem.o.d 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOStheSystem.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/exec/singletask/EOStheSystem.c  -o ${OBJECTDIR}/_ext/1361983106/EOStheSystem.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1361983106/EOStheSystem.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1361983106/EOStheSystem.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1361983106/EOStheTimerManager.o: ../../../../../../../embobj/core/exec/singletask/EOStheTimerManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1361983106 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOStheTimerManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/1361983106/EOStheTimerManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../../embobj/core/exec/singletask/EOStheTimerManager.c  -o ${OBJECTDIR}/_ext/1361983106/EOStheTimerManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1361983106/EOStheTimerManager.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1361983106/EOStheTimerManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/757185051/crc16.o: ../../../../../../libs/abslayer/hal/src/utils/crc16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/757185051 
	@${RM} ${OBJECTDIR}/_ext/757185051/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/757185051/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/utils/crc16.c  -o ${OBJECTDIR}/_ext/757185051/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/757185051/crc16.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/757185051/crc16.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/757185051/crc32.o: ../../../../../../libs/abslayer/hal/src/utils/crc32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/757185051 
	@${RM} ${OBJECTDIR}/_ext/757185051/crc32.o.d 
	@${RM} ${OBJECTDIR}/_ext/757185051/crc32.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/utils/crc32.c  -o ${OBJECTDIR}/_ext/757185051/crc32.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/757185051/crc32.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/757185051/crc32.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_brdcfg.o: ../../../../../../libs/abslayer/hal/src/hal_brdcfg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_brdcfg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_brdcfg.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_brdcfg.c  -o ${OBJECTDIR}/_ext/1116683527/hal_brdcfg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_brdcfg.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_brdcfg.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_arch.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_arch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_arch.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_arch.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_arch.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_arch.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_arch.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_arch.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_base.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_base.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_base.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_base.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_base.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_base.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_base.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_base.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_can.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_can.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_can.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_can.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_can.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_can.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_can.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_crc.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_crc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_crc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_crc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_crc.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_crc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_crc.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_crc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_eeprom.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_eeprom.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_eeprom.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_eeprom.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_eeprom.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_eeprom.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_eeprom.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_flash.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_flash.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_flash.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_flash.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_flash.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_flash.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_gpio.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_gpio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_gpio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_gpio.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_gpio.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_gpio.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_gpio.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_gpio.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_led.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_led.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_led.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_led.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_led.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_led.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_led.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_led.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_sys.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_sys.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_sys.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_sys.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_sys.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_sys.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_sys.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_sys.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1116683527/hal_dspic_timer.o: ../../../../../../libs/abslayer/hal/src/hal_dspic_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1116683527 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116683527/hal_dspic_timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/abslayer/hal/src/hal_dspic_timer.c  -o ${OBJECTDIR}/_ext/1116683527/hal_dspic_timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1116683527/hal_dspic_timer.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1116683527/hal_dspic_timer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1482636426/hal_cfg.o: ../../hal-cfg/hal_cfg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1482636426 
	@${RM} ${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1482636426/hal_cfg.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../hal-cfg/hal_cfg.c  -o ${OBJECTDIR}/_ext/1482636426/hal_cfg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o: ../../hal-stg/hal_dspic_startup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1482652236 
	@${RM} ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../hal-stg/hal_dspic_startup.c  -o ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1673993744/eEcommon.o: ../../../../../../libs/services/common/src/eEcommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1673993744 
	@${RM} ${OBJECTDIR}/_ext/1673993744/eEcommon.o.d 
	@${RM} ${OBJECTDIR}/_ext/1673993744/eEcommon.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/services/common/src/eEcommon.c  -o ${OBJECTDIR}/_ext/1673993744/eEcommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1673993744/eEcommon.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1673993744/eEcommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/883723884/eEprivateStorage.o: ../../../../../../libs/services/storage/private/src/eEprivateStorage.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/883723884 
	@${RM} ${OBJECTDIR}/_ext/883723884/eEprivateStorage.o.d 
	@${RM} ${OBJECTDIR}/_ext/883723884/eEprivateStorage.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/services/storage/private/src/eEprivateStorage.c  -o ${OBJECTDIR}/_ext/883723884/eEprivateStorage.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/883723884/eEprivateStorage.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/883723884/eEprivateStorage.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/339291006/eEpermdata.o: ../../../../../../libs/services/storage/shared/src/eEpermdata.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/339291006 
	@${RM} ${OBJECTDIR}/_ext/339291006/eEpermdata.o.d 
	@${RM} ${OBJECTDIR}/_ext/339291006/eEpermdata.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/services/storage/shared/src/eEpermdata.c  -o ${OBJECTDIR}/_ext/339291006/eEpermdata.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/339291006/eEpermdata.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/339291006/eEpermdata.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/339291006/eEsharedInfo.o: ../../../../../../libs/services/storage/shared/src/eEsharedInfo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/339291006 
	@${RM} ${OBJECTDIR}/_ext/339291006/eEsharedInfo.o.d 
	@${RM} ${OBJECTDIR}/_ext/339291006/eEsharedInfo.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/services/storage/shared/src/eEsharedInfo.c  -o ${OBJECTDIR}/_ext/339291006/eEsharedInfo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/339291006/eEsharedInfo.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/339291006/eEsharedInfo.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/339291006/eEsharedStorage.o: ../../../../../../libs/services/storage/shared/src/eEsharedStorage.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/339291006 
	@${RM} ${OBJECTDIR}/_ext/339291006/eEsharedStorage.o.d 
	@${RM} ${OBJECTDIR}/_ext/339291006/eEsharedStorage.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/services/storage/shared/src/eEsharedStorage.c  -o ${OBJECTDIR}/_ext/339291006/eEsharedStorage.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/339291006/eEsharedStorage.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../env/cfg" -I"../../../../../../libs/abslayer/hal/api" -I"../../../../../../libs/abslayer/hal/src" -I"../../../../../../libs/services/common/api" -I"../../../../../../libs/services/storage/shared/api" -I"../../../../../../libs/services/storage/shared/src" -I"../../../../../../libs/services/storage/private/api" -I"../../../../../../../embobj/core/core" -I"../../../../../../../embobj/core/exec/singletask" -I"../../../../../../libs/services/storage/shared/api" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SGor2FOC -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/339291006/eEsharedStorage.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/appl-strg-embobj.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ../eElinkerscript_applic.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/appl-strg-embobj.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,$(MP_LINKER_FILE_OPTION),--heap=4096,--stack=1024,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library-path="..",--no-force-link,--smart-io,-Map="${DISTDIR}/appl-strg-embobj.X.${IMAGE_TYPE}.map",--report-mem,--warn-section-align$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/appl-strg-embobj.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ../eElinkerscript_applic.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/appl-strg-embobj.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -Wl,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--heap=4096,--stack=1024,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library-path="..",--no-force-link,--smart-io,-Map="${DISTDIR}/appl-strg-embobj.X.${IMAGE_TYPE}.map",--report-mem,--warn-section-align$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/appl-strg-embobj.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf 
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif

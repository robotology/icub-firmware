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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../app/memory.s ../../app/memory_manager.c ../../hal-cfg/hal_cfg.c ../../app/board_func.c ../../app/bootloader.c ../../app/bootloader_main.c ../../app/bootloader_parser.c ../../app/bootloader_sm.c ../../../../../../any-arch/body/embenv/envcom/eEcommon.c ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc16.c ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc32.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_brdcfg.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_arch.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_base.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_can.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_crc.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_eeprom.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_flash.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_gpio.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_led.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_sys.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_timer.c ../../../../../body/embenv/envshalib/eEsharedStorage.c ../../../../../body/embenv/envcom/eEpermdata.c ../../../../../body/embenv/envcom/eEsharedInfo.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1445257345/memory.o ${OBJECTDIR}/_ext/1445257345/memory_manager.o ${OBJECTDIR}/_ext/1482636426/hal_cfg.o ${OBJECTDIR}/_ext/1445257345/board_func.o ${OBJECTDIR}/_ext/1445257345/bootloader.o ${OBJECTDIR}/_ext/1445257345/bootloader_main.o ${OBJECTDIR}/_ext/1445257345/bootloader_parser.o ${OBJECTDIR}/_ext/1445257345/bootloader_sm.o ${OBJECTDIR}/_ext/189820923/eEcommon.o ${OBJECTDIR}/_ext/1986321371/crc16.o ${OBJECTDIR}/_ext/1986321371/crc32.o ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o ${OBJECTDIR}/_ext/1864751918/eEpermdata.o ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1445257345/memory.o.d ${OBJECTDIR}/_ext/1445257345/memory_manager.o.d ${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d ${OBJECTDIR}/_ext/1445257345/board_func.o.d ${OBJECTDIR}/_ext/1445257345/bootloader.o.d ${OBJECTDIR}/_ext/1445257345/bootloader_main.o.d ${OBJECTDIR}/_ext/1445257345/bootloader_parser.o.d ${OBJECTDIR}/_ext/1445257345/bootloader_sm.o.d ${OBJECTDIR}/_ext/189820923/eEcommon.o.d ${OBJECTDIR}/_ext/1986321371/crc16.o.d ${OBJECTDIR}/_ext/1986321371/crc32.o.d ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o.d ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o.d ${OBJECTDIR}/_ext/1864751918/eEpermdata.o.d ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1445257345/memory.o ${OBJECTDIR}/_ext/1445257345/memory_manager.o ${OBJECTDIR}/_ext/1482636426/hal_cfg.o ${OBJECTDIR}/_ext/1445257345/board_func.o ${OBJECTDIR}/_ext/1445257345/bootloader.o ${OBJECTDIR}/_ext/1445257345/bootloader_main.o ${OBJECTDIR}/_ext/1445257345/bootloader_parser.o ${OBJECTDIR}/_ext/1445257345/bootloader_sm.o ${OBJECTDIR}/_ext/189820923/eEcommon.o ${OBJECTDIR}/_ext/1986321371/crc16.o ${OBJECTDIR}/_ext/1986321371/crc32.o ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o ${OBJECTDIR}/_ext/1864751918/eEpermdata.o ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o

# Source Files
SOURCEFILES=../../app/memory.s ../../app/memory_manager.c ../../hal-cfg/hal_cfg.c ../../app/board_func.c ../../app/bootloader.c ../../app/bootloader_main.c ../../app/bootloader_parser.c ../../app/bootloader_sm.c ../../../../../../any-arch/body/embenv/envcom/eEcommon.c ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc16.c ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc32.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_brdcfg.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_arch.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_base.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_can.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_crc.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_eeprom.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_flash.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_gpio.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_led.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_sys.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_timer.c ../../../../../body/embenv/envshalib/eEsharedStorage.c ../../../../../body/embenv/envcom/eEpermdata.c ../../../../../body/embenv/envcom/eEsharedInfo.c


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
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ128MC802
MP_LINKER_FILE_OPTION=,--script="..\eElinkerscript_loader.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1445257345/memory_manager.o: ../../app/memory_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/memory_manager.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/memory_manager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/memory_manager.c  -o ${OBJECTDIR}/_ext/1445257345/memory_manager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/memory_manager.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/memory_manager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1482636426/hal_cfg.o: ../../hal-cfg/hal_cfg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1482636426 
	@${RM} ${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1482636426/hal_cfg.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../hal-cfg/hal_cfg.c  -o ${OBJECTDIR}/_ext/1482636426/hal_cfg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/board_func.o: ../../app/board_func.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/board_func.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/board_func.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/board_func.c  -o ${OBJECTDIR}/_ext/1445257345/board_func.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/board_func.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/board_func.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/bootloader.o: ../../app/bootloader.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/bootloader.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/bootloader.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/bootloader.c  -o ${OBJECTDIR}/_ext/1445257345/bootloader.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/bootloader.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/bootloader.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/bootloader_main.o: ../../app/bootloader_main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/bootloader_main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/bootloader_main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/bootloader_main.c  -o ${OBJECTDIR}/_ext/1445257345/bootloader_main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/bootloader_main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/bootloader_main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/bootloader_parser.o: ../../app/bootloader_parser.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/bootloader_parser.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/bootloader_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/bootloader_parser.c  -o ${OBJECTDIR}/_ext/1445257345/bootloader_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/bootloader_parser.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/bootloader_parser.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/bootloader_sm.o: ../../app/bootloader_sm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/bootloader_sm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/bootloader_sm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/bootloader_sm.c  -o ${OBJECTDIR}/_ext/1445257345/bootloader_sm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/bootloader_sm.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/bootloader_sm.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/189820923/eEcommon.o: ../../../../../../any-arch/body/embenv/envcom/eEcommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/189820923 
	@${RM} ${OBJECTDIR}/_ext/189820923/eEcommon.o.d 
	@${RM} ${OBJECTDIR}/_ext/189820923/eEcommon.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/body/embenv/envcom/eEcommon.c  -o ${OBJECTDIR}/_ext/189820923/eEcommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/189820923/eEcommon.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/189820923/eEcommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1986321371/crc16.o: ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1986321371 
	@${RM} ${OBJECTDIR}/_ext/1986321371/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1986321371/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc16.c  -o ${OBJECTDIR}/_ext/1986321371/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1986321371/crc16.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1986321371/crc16.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1986321371/crc32.o: ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1986321371 
	@${RM} ${OBJECTDIR}/_ext/1986321371/crc32.o.d 
	@${RM} ${OBJECTDIR}/_ext/1986321371/crc32.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc32.c  -o ${OBJECTDIR}/_ext/1986321371/crc32.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1986321371/crc32.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1986321371/crc32.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_brdcfg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_brdcfg.c  -o ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_arch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_arch.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_base.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_base.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_can.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_crc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_crc.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_eeprom.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_flash.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_gpio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_gpio.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_led.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_led.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_sys.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_sys.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_timer.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o: ../../../../../body/embenv/envshalib/eEsharedStorage.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1266031256 
	@${RM} ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o.d 
	@${RM} ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/embenv/envshalib/eEsharedStorage.c  -o ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864751918/eEpermdata.o: ../../../../../body/embenv/envcom/eEpermdata.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864751918 
	@${RM} ${OBJECTDIR}/_ext/1864751918/eEpermdata.o.d 
	@${RM} ${OBJECTDIR}/_ext/1864751918/eEpermdata.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/embenv/envcom/eEpermdata.c  -o ${OBJECTDIR}/_ext/1864751918/eEpermdata.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864751918/eEpermdata.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864751918/eEpermdata.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o: ../../../../../body/embenv/envcom/eEsharedInfo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864751918 
	@${RM} ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/embenv/envcom/eEsharedInfo.c  -o ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1445257345/memory_manager.o: ../../app/memory_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/memory_manager.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/memory_manager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/memory_manager.c  -o ${OBJECTDIR}/_ext/1445257345/memory_manager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/memory_manager.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/memory_manager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1482636426/hal_cfg.o: ../../hal-cfg/hal_cfg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1482636426 
	@${RM} ${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1482636426/hal_cfg.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../hal-cfg/hal_cfg.c  -o ${OBJECTDIR}/_ext/1482636426/hal_cfg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/board_func.o: ../../app/board_func.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/board_func.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/board_func.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/board_func.c  -o ${OBJECTDIR}/_ext/1445257345/board_func.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/board_func.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/board_func.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/bootloader.o: ../../app/bootloader.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/bootloader.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/bootloader.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/bootloader.c  -o ${OBJECTDIR}/_ext/1445257345/bootloader.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/bootloader.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/bootloader.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/bootloader_main.o: ../../app/bootloader_main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/bootloader_main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/bootloader_main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/bootloader_main.c  -o ${OBJECTDIR}/_ext/1445257345/bootloader_main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/bootloader_main.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/bootloader_main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/bootloader_parser.o: ../../app/bootloader_parser.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/bootloader_parser.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/bootloader_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/bootloader_parser.c  -o ${OBJECTDIR}/_ext/1445257345/bootloader_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/bootloader_parser.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/bootloader_parser.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/bootloader_sm.o: ../../app/bootloader_sm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/bootloader_sm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/bootloader_sm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/bootloader_sm.c  -o ${OBJECTDIR}/_ext/1445257345/bootloader_sm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/bootloader_sm.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/bootloader_sm.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/189820923/eEcommon.o: ../../../../../../any-arch/body/embenv/envcom/eEcommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/189820923 
	@${RM} ${OBJECTDIR}/_ext/189820923/eEcommon.o.d 
	@${RM} ${OBJECTDIR}/_ext/189820923/eEcommon.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/body/embenv/envcom/eEcommon.c  -o ${OBJECTDIR}/_ext/189820923/eEcommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/189820923/eEcommon.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/189820923/eEcommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1986321371/crc16.o: ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1986321371 
	@${RM} ${OBJECTDIR}/_ext/1986321371/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1986321371/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc16.c  -o ${OBJECTDIR}/_ext/1986321371/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1986321371/crc16.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1986321371/crc16.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1986321371/crc32.o: ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1986321371 
	@${RM} ${OBJECTDIR}/_ext/1986321371/crc32.o.d 
	@${RM} ${OBJECTDIR}/_ext/1986321371/crc32.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc32.c  -o ${OBJECTDIR}/_ext/1986321371/crc32.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1986321371/crc32.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1986321371/crc32.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_brdcfg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_brdcfg.c  -o ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_arch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_arch.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_base.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_base.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_can.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_crc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_crc.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_eeprom.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_flash.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_gpio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_gpio.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_led.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_led.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_sys.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_sys.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_timer.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o: ../../../../../body/embenv/envshalib/eEsharedStorage.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1266031256 
	@${RM} ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o.d 
	@${RM} ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/embenv/envshalib/eEsharedStorage.c  -o ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864751918/eEpermdata.o: ../../../../../body/embenv/envcom/eEpermdata.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864751918 
	@${RM} ${OBJECTDIR}/_ext/1864751918/eEpermdata.o.d 
	@${RM} ${OBJECTDIR}/_ext/1864751918/eEpermdata.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/embenv/envcom/eEpermdata.c  -o ${OBJECTDIR}/_ext/1864751918/eEpermdata.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864751918/eEpermdata.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864751918/eEpermdata.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o: ../../../../../body/embenv/envcom/eEsharedInfo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864751918 
	@${RM} ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/embenv/envcom/eEsharedInfo.c  -o ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o.d"      -g -omf=elf -O2 -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../body/embenv/envcom" -I"../../hal-cfg" -I"../../app" -I".." -DHAL_USE_ARCH_DSPIC -D_DSPIC_ -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1445257345/memory.o: ../../app/memory.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/memory.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/memory.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/memory.s  -o ${OBJECTDIR}/_ext/1445257345/memory.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/memory.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/memory.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1445257345/memory.o: ../../app/memory.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/memory.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/memory.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/memory.s  -o ${OBJECTDIR}/_ext/1445257345/memory.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/memory.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/memory.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ../eElinkerscript_loader.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,$(MP_LINKER_FILE_OPTION),--heap=4096,--stack=1024,--check-sections,--data-init,--pack-data,--handles,--isr,--gc-sections,--fill-upper=0,--stackguard=16,--library-path="..",--no-force-link,--smart-io,-Map="${DISTDIR}/bootloader.X.${IMAGE_TYPE}.map",--report-mem,--warn-section-align$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ../eElinkerscript_loader.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -Wl,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--heap=4096,--stack=1024,--check-sections,--data-init,--pack-data,--handles,--isr,--gc-sections,--fill-upper=0,--stackguard=16,--library-path="..",--no-force-link,--smart-io,-Map="${DISTDIR}/bootloader.X.${IMAGE_TYPE}.map",--report-mem,--warn-section-align$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf 
	
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

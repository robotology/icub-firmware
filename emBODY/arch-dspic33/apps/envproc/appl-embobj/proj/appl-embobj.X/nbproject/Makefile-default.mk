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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/appl-embobj.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/appl-embobj.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../hal-cfg/hal_cfg.c ../../app/main.c ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc16.c ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc32.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_brdcfg.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_arch.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_base.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_can.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_crc.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_eeprom.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_flash.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_gpio.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_led.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_sys.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_timer.c ../../../../../body/embenv/envcom/eEpermdata.c ../../../../../body/embenv/envcom/eEsharedInfo.c ../../hal-stg/hal_dspic_startup.c ../../../../../body/embenv/envshalib/eEsharedStorage.c ../../../../../../any-arch/body/embenv/envcom/eEcommon.c ../../../../../../any-arch/sys/embobj/core/src/EOaction.c ../../../../../../any-arch/sys/embobj/core/src/EOarray.c ../../../../../../any-arch/sys/embobj/core/src/EoCommon.c ../../../../../../any-arch/sys/embobj/core/src/EOconstarray.c ../../../../../../any-arch/sys/embobj/core/src/EOconstvector.c ../../../../../../any-arch/sys/embobj/core/src/EOdeque.c ../../../../../../any-arch/sys/embobj/core/src/EOfifo.c ../../../../../../any-arch/sys/embobj/core/src/EOfifoByte.c ../../../../../../any-arch/sys/embobj/core/src/EOfifoWord.c ../../../../../../any-arch/sys/embobj/core/src/EOlist.c ../../../../../../any-arch/sys/embobj/core/src/EOtheErrorManager.c ../../../../../../any-arch/sys/embobj/core/src/EOtheMemoryPool.c ../../../../../../any-arch/sys/embobj/core/src/EOtimer.c ../../../../../../any-arch/sys/embobj/core/src/EOvector.c ../../../../../../any-arch/sys/embobj/core-vee/src/EOVmutex.c ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtask.c ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheCallbackManager.c ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheSystem.c ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheTimerManager.c ../../../../../../any-arch/sys/embobj/core-see/src/EOSmutex.c ../../../../../../any-arch/sys/embobj/core-see/src/EOStheCallbackManager.c ../../../../../../any-arch/sys/embobj/core-see/src/EOStheFOOP.c ../../../../../../any-arch/sys/embobj/core-see/src/EOStheSystem.c ../../../../../../any-arch/sys/embobj/core-see/src/EOStheTimerManager.c ../../app/eos_applembobj_info.c ../../app/eos_applembobj_spec.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1482636426/hal_cfg.o ${OBJECTDIR}/_ext/1445257345/main.o ${OBJECTDIR}/_ext/1986321371/crc16.o ${OBJECTDIR}/_ext/1986321371/crc32.o ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o ${OBJECTDIR}/_ext/1864751918/eEpermdata.o ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o ${OBJECTDIR}/_ext/189820923/eEcommon.o ${OBJECTDIR}/_ext/471949716/EOaction.o ${OBJECTDIR}/_ext/471949716/EOarray.o ${OBJECTDIR}/_ext/471949716/EoCommon.o ${OBJECTDIR}/_ext/471949716/EOconstarray.o ${OBJECTDIR}/_ext/471949716/EOconstvector.o ${OBJECTDIR}/_ext/471949716/EOdeque.o ${OBJECTDIR}/_ext/471949716/EOfifo.o ${OBJECTDIR}/_ext/471949716/EOfifoByte.o ${OBJECTDIR}/_ext/471949716/EOfifoWord.o ${OBJECTDIR}/_ext/471949716/EOlist.o ${OBJECTDIR}/_ext/471949716/EOtheErrorManager.o ${OBJECTDIR}/_ext/471949716/EOtheMemoryPool.o ${OBJECTDIR}/_ext/471949716/EOtimer.o ${OBJECTDIR}/_ext/471949716/EOvector.o ${OBJECTDIR}/_ext/910568117/EOVmutex.o ${OBJECTDIR}/_ext/910568117/EOVtask.o ${OBJECTDIR}/_ext/910568117/EOVtheCallbackManager.o ${OBJECTDIR}/_ext/910568117/EOVtheSystem.o ${OBJECTDIR}/_ext/910568117/EOVtheTimerManager.o ${OBJECTDIR}/_ext/1751942926/EOSmutex.o ${OBJECTDIR}/_ext/1751942926/EOStheCallbackManager.o ${OBJECTDIR}/_ext/1751942926/EOStheFOOP.o ${OBJECTDIR}/_ext/1751942926/EOStheSystem.o ${OBJECTDIR}/_ext/1751942926/EOStheTimerManager.o ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d ${OBJECTDIR}/_ext/1445257345/main.o.d ${OBJECTDIR}/_ext/1986321371/crc16.o.d ${OBJECTDIR}/_ext/1986321371/crc32.o.d ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o.d ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o.d ${OBJECTDIR}/_ext/1864751918/eEpermdata.o.d ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o.d ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o.d ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o.d ${OBJECTDIR}/_ext/189820923/eEcommon.o.d ${OBJECTDIR}/_ext/471949716/EOaction.o.d ${OBJECTDIR}/_ext/471949716/EOarray.o.d ${OBJECTDIR}/_ext/471949716/EoCommon.o.d ${OBJECTDIR}/_ext/471949716/EOconstarray.o.d ${OBJECTDIR}/_ext/471949716/EOconstvector.o.d ${OBJECTDIR}/_ext/471949716/EOdeque.o.d ${OBJECTDIR}/_ext/471949716/EOfifo.o.d ${OBJECTDIR}/_ext/471949716/EOfifoByte.o.d ${OBJECTDIR}/_ext/471949716/EOfifoWord.o.d ${OBJECTDIR}/_ext/471949716/EOlist.o.d ${OBJECTDIR}/_ext/471949716/EOtheErrorManager.o.d ${OBJECTDIR}/_ext/471949716/EOtheMemoryPool.o.d ${OBJECTDIR}/_ext/471949716/EOtimer.o.d ${OBJECTDIR}/_ext/471949716/EOvector.o.d ${OBJECTDIR}/_ext/910568117/EOVmutex.o.d ${OBJECTDIR}/_ext/910568117/EOVtask.o.d ${OBJECTDIR}/_ext/910568117/EOVtheCallbackManager.o.d ${OBJECTDIR}/_ext/910568117/EOVtheSystem.o.d ${OBJECTDIR}/_ext/910568117/EOVtheTimerManager.o.d ${OBJECTDIR}/_ext/1751942926/EOSmutex.o.d ${OBJECTDIR}/_ext/1751942926/EOStheCallbackManager.o.d ${OBJECTDIR}/_ext/1751942926/EOStheFOOP.o.d ${OBJECTDIR}/_ext/1751942926/EOStheSystem.o.d ${OBJECTDIR}/_ext/1751942926/EOStheTimerManager.o.d ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o.d ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1482636426/hal_cfg.o ${OBJECTDIR}/_ext/1445257345/main.o ${OBJECTDIR}/_ext/1986321371/crc16.o ${OBJECTDIR}/_ext/1986321371/crc32.o ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o ${OBJECTDIR}/_ext/1864751918/eEpermdata.o ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o ${OBJECTDIR}/_ext/189820923/eEcommon.o ${OBJECTDIR}/_ext/471949716/EOaction.o ${OBJECTDIR}/_ext/471949716/EOarray.o ${OBJECTDIR}/_ext/471949716/EoCommon.o ${OBJECTDIR}/_ext/471949716/EOconstarray.o ${OBJECTDIR}/_ext/471949716/EOconstvector.o ${OBJECTDIR}/_ext/471949716/EOdeque.o ${OBJECTDIR}/_ext/471949716/EOfifo.o ${OBJECTDIR}/_ext/471949716/EOfifoByte.o ${OBJECTDIR}/_ext/471949716/EOfifoWord.o ${OBJECTDIR}/_ext/471949716/EOlist.o ${OBJECTDIR}/_ext/471949716/EOtheErrorManager.o ${OBJECTDIR}/_ext/471949716/EOtheMemoryPool.o ${OBJECTDIR}/_ext/471949716/EOtimer.o ${OBJECTDIR}/_ext/471949716/EOvector.o ${OBJECTDIR}/_ext/910568117/EOVmutex.o ${OBJECTDIR}/_ext/910568117/EOVtask.o ${OBJECTDIR}/_ext/910568117/EOVtheCallbackManager.o ${OBJECTDIR}/_ext/910568117/EOVtheSystem.o ${OBJECTDIR}/_ext/910568117/EOVtheTimerManager.o ${OBJECTDIR}/_ext/1751942926/EOSmutex.o ${OBJECTDIR}/_ext/1751942926/EOStheCallbackManager.o ${OBJECTDIR}/_ext/1751942926/EOStheFOOP.o ${OBJECTDIR}/_ext/1751942926/EOStheSystem.o ${OBJECTDIR}/_ext/1751942926/EOStheTimerManager.o ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o

# Source Files
SOURCEFILES=../../hal-cfg/hal_cfg.c ../../app/main.c ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc16.c ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc32.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_brdcfg.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_arch.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_base.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_can.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_crc.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_eeprom.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_flash.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_gpio.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_led.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_sys.c ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_timer.c ../../../../../body/embenv/envcom/eEpermdata.c ../../../../../body/embenv/envcom/eEsharedInfo.c ../../hal-stg/hal_dspic_startup.c ../../../../../body/embenv/envshalib/eEsharedStorage.c ../../../../../../any-arch/body/embenv/envcom/eEcommon.c ../../../../../../any-arch/sys/embobj/core/src/EOaction.c ../../../../../../any-arch/sys/embobj/core/src/EOarray.c ../../../../../../any-arch/sys/embobj/core/src/EoCommon.c ../../../../../../any-arch/sys/embobj/core/src/EOconstarray.c ../../../../../../any-arch/sys/embobj/core/src/EOconstvector.c ../../../../../../any-arch/sys/embobj/core/src/EOdeque.c ../../../../../../any-arch/sys/embobj/core/src/EOfifo.c ../../../../../../any-arch/sys/embobj/core/src/EOfifoByte.c ../../../../../../any-arch/sys/embobj/core/src/EOfifoWord.c ../../../../../../any-arch/sys/embobj/core/src/EOlist.c ../../../../../../any-arch/sys/embobj/core/src/EOtheErrorManager.c ../../../../../../any-arch/sys/embobj/core/src/EOtheMemoryPool.c ../../../../../../any-arch/sys/embobj/core/src/EOtimer.c ../../../../../../any-arch/sys/embobj/core/src/EOvector.c ../../../../../../any-arch/sys/embobj/core-vee/src/EOVmutex.c ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtask.c ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheCallbackManager.c ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheSystem.c ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheTimerManager.c ../../../../../../any-arch/sys/embobj/core-see/src/EOSmutex.c ../../../../../../any-arch/sys/embobj/core-see/src/EOStheCallbackManager.c ../../../../../../any-arch/sys/embobj/core-see/src/EOStheFOOP.c ../../../../../../any-arch/sys/embobj/core-see/src/EOStheSystem.c ../../../../../../any-arch/sys/embobj/core-see/src/EOStheTimerManager.c ../../app/eos_applembobj_info.c ../../app/eos_applembobj_spec.c


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
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/appl-embobj.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ128MC802
MP_LINKER_FILE_OPTION=,--script="..\eElinkerscript_applic.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1482636426/hal_cfg.o: ../../hal-cfg/hal_cfg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1482636426 
	@${RM} ${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1482636426/hal_cfg.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../hal-cfg/hal_cfg.c  -o ${OBJECTDIR}/_ext/1482636426/hal_cfg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/main.o: ../../app/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/main.c  -o ${OBJECTDIR}/_ext/1445257345/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1986321371/crc16.o: ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1986321371 
	@${RM} ${OBJECTDIR}/_ext/1986321371/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1986321371/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc16.c  -o ${OBJECTDIR}/_ext/1986321371/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1986321371/crc16.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1986321371/crc16.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1986321371/crc32.o: ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1986321371 
	@${RM} ${OBJECTDIR}/_ext/1986321371/crc32.o.d 
	@${RM} ${OBJECTDIR}/_ext/1986321371/crc32.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc32.c  -o ${OBJECTDIR}/_ext/1986321371/crc32.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1986321371/crc32.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1986321371/crc32.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_brdcfg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_brdcfg.c  -o ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_arch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_arch.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_base.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_base.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_can.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_crc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_crc.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_eeprom.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_flash.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_gpio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_gpio.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_led.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_led.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_sys.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_sys.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_timer.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864751918/eEpermdata.o: ../../../../../body/embenv/envcom/eEpermdata.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864751918 
	@${RM} ${OBJECTDIR}/_ext/1864751918/eEpermdata.o.d 
	@${RM} ${OBJECTDIR}/_ext/1864751918/eEpermdata.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/embenv/envcom/eEpermdata.c  -o ${OBJECTDIR}/_ext/1864751918/eEpermdata.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864751918/eEpermdata.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864751918/eEpermdata.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o: ../../../../../body/embenv/envcom/eEsharedInfo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864751918 
	@${RM} ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/embenv/envcom/eEsharedInfo.c  -o ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o: ../../hal-stg/hal_dspic_startup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1482652236 
	@${RM} ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../hal-stg/hal_dspic_startup.c  -o ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o: ../../../../../body/embenv/envshalib/eEsharedStorage.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1266031256 
	@${RM} ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o.d 
	@${RM} ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/embenv/envshalib/eEsharedStorage.c  -o ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/189820923/eEcommon.o: ../../../../../../any-arch/body/embenv/envcom/eEcommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/189820923 
	@${RM} ${OBJECTDIR}/_ext/189820923/eEcommon.o.d 
	@${RM} ${OBJECTDIR}/_ext/189820923/eEcommon.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/body/embenv/envcom/eEcommon.c  -o ${OBJECTDIR}/_ext/189820923/eEcommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/189820923/eEcommon.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/189820923/eEcommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOaction.o: ../../../../../../any-arch/sys/embobj/core/src/EOaction.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOaction.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOaction.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOaction.c  -o ${OBJECTDIR}/_ext/471949716/EOaction.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOaction.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOaction.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOarray.o: ../../../../../../any-arch/sys/embobj/core/src/EOarray.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOarray.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOarray.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOarray.c  -o ${OBJECTDIR}/_ext/471949716/EOarray.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOarray.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOarray.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EoCommon.o: ../../../../../../any-arch/sys/embobj/core/src/EoCommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EoCommon.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EoCommon.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EoCommon.c  -o ${OBJECTDIR}/_ext/471949716/EoCommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EoCommon.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EoCommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOconstarray.o: ../../../../../../any-arch/sys/embobj/core/src/EOconstarray.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOconstarray.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOconstarray.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOconstarray.c  -o ${OBJECTDIR}/_ext/471949716/EOconstarray.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOconstarray.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOconstarray.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOconstvector.o: ../../../../../../any-arch/sys/embobj/core/src/EOconstvector.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOconstvector.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOconstvector.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOconstvector.c  -o ${OBJECTDIR}/_ext/471949716/EOconstvector.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOconstvector.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOconstvector.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOdeque.o: ../../../../../../any-arch/sys/embobj/core/src/EOdeque.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOdeque.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOdeque.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOdeque.c  -o ${OBJECTDIR}/_ext/471949716/EOdeque.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOdeque.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOdeque.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOfifo.o: ../../../../../../any-arch/sys/embobj/core/src/EOfifo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOfifo.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOfifo.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOfifo.c  -o ${OBJECTDIR}/_ext/471949716/EOfifo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOfifo.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOfifo.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOfifoByte.o: ../../../../../../any-arch/sys/embobj/core/src/EOfifoByte.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOfifoByte.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOfifoByte.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOfifoByte.c  -o ${OBJECTDIR}/_ext/471949716/EOfifoByte.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOfifoByte.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOfifoByte.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOfifoWord.o: ../../../../../../any-arch/sys/embobj/core/src/EOfifoWord.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOfifoWord.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOfifoWord.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOfifoWord.c  -o ${OBJECTDIR}/_ext/471949716/EOfifoWord.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOfifoWord.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOfifoWord.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOlist.o: ../../../../../../any-arch/sys/embobj/core/src/EOlist.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOlist.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOlist.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOlist.c  -o ${OBJECTDIR}/_ext/471949716/EOlist.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOlist.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOlist.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOtheErrorManager.o: ../../../../../../any-arch/sys/embobj/core/src/EOtheErrorManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOtheErrorManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOtheErrorManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOtheErrorManager.c  -o ${OBJECTDIR}/_ext/471949716/EOtheErrorManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOtheErrorManager.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOtheErrorManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOtheMemoryPool.o: ../../../../../../any-arch/sys/embobj/core/src/EOtheMemoryPool.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOtheMemoryPool.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOtheMemoryPool.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOtheMemoryPool.c  -o ${OBJECTDIR}/_ext/471949716/EOtheMemoryPool.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOtheMemoryPool.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOtheMemoryPool.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOtimer.o: ../../../../../../any-arch/sys/embobj/core/src/EOtimer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOtimer.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOtimer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOtimer.c  -o ${OBJECTDIR}/_ext/471949716/EOtimer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOtimer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOtimer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOvector.o: ../../../../../../any-arch/sys/embobj/core/src/EOvector.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOvector.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOvector.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOvector.c  -o ${OBJECTDIR}/_ext/471949716/EOvector.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOvector.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOvector.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/910568117/EOVmutex.o: ../../../../../../any-arch/sys/embobj/core-vee/src/EOVmutex.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/910568117 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVmutex.o.d 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVmutex.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-vee/src/EOVmutex.c  -o ${OBJECTDIR}/_ext/910568117/EOVmutex.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/910568117/EOVmutex.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/910568117/EOVmutex.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/910568117/EOVtask.o: ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtask.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/910568117 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVtask.o.d 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVtask.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtask.c  -o ${OBJECTDIR}/_ext/910568117/EOVtask.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/910568117/EOVtask.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/910568117/EOVtask.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/910568117/EOVtheCallbackManager.o: ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheCallbackManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/910568117 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVtheCallbackManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVtheCallbackManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheCallbackManager.c  -o ${OBJECTDIR}/_ext/910568117/EOVtheCallbackManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/910568117/EOVtheCallbackManager.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/910568117/EOVtheCallbackManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/910568117/EOVtheSystem.o: ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheSystem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/910568117 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVtheSystem.o.d 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVtheSystem.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheSystem.c  -o ${OBJECTDIR}/_ext/910568117/EOVtheSystem.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/910568117/EOVtheSystem.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/910568117/EOVtheSystem.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/910568117/EOVtheTimerManager.o: ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheTimerManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/910568117 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVtheTimerManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVtheTimerManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheTimerManager.c  -o ${OBJECTDIR}/_ext/910568117/EOVtheTimerManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/910568117/EOVtheTimerManager.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/910568117/EOVtheTimerManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1751942926/EOSmutex.o: ../../../../../../any-arch/sys/embobj/core-see/src/EOSmutex.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1751942926 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOSmutex.o.d 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOSmutex.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-see/src/EOSmutex.c  -o ${OBJECTDIR}/_ext/1751942926/EOSmutex.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1751942926/EOSmutex.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1751942926/EOSmutex.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1751942926/EOStheCallbackManager.o: ../../../../../../any-arch/sys/embobj/core-see/src/EOStheCallbackManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1751942926 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOStheCallbackManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOStheCallbackManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-see/src/EOStheCallbackManager.c  -o ${OBJECTDIR}/_ext/1751942926/EOStheCallbackManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1751942926/EOStheCallbackManager.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1751942926/EOStheCallbackManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1751942926/EOStheFOOP.o: ../../../../../../any-arch/sys/embobj/core-see/src/EOStheFOOP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1751942926 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOStheFOOP.o.d 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOStheFOOP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-see/src/EOStheFOOP.c  -o ${OBJECTDIR}/_ext/1751942926/EOStheFOOP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1751942926/EOStheFOOP.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1751942926/EOStheFOOP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1751942926/EOStheSystem.o: ../../../../../../any-arch/sys/embobj/core-see/src/EOStheSystem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1751942926 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOStheSystem.o.d 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOStheSystem.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-see/src/EOStheSystem.c  -o ${OBJECTDIR}/_ext/1751942926/EOStheSystem.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1751942926/EOStheSystem.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1751942926/EOStheSystem.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1751942926/EOStheTimerManager.o: ../../../../../../any-arch/sys/embobj/core-see/src/EOStheTimerManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1751942926 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOStheTimerManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOStheTimerManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-see/src/EOStheTimerManager.c  -o ${OBJECTDIR}/_ext/1751942926/EOStheTimerManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1751942926/EOStheTimerManager.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1751942926/EOStheTimerManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o: ../../app/eos_applembobj_info.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/eos_applembobj_info.c  -o ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o: ../../app/eos_applembobj_spec.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/eos_applembobj_spec.c  -o ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1482636426/hal_cfg.o: ../../hal-cfg/hal_cfg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1482636426 
	@${RM} ${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1482636426/hal_cfg.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../hal-cfg/hal_cfg.c  -o ${OBJECTDIR}/_ext/1482636426/hal_cfg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1482636426/hal_cfg.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/main.o: ../../app/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/main.c  -o ${OBJECTDIR}/_ext/1445257345/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/main.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1986321371/crc16.o: ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1986321371 
	@${RM} ${OBJECTDIR}/_ext/1986321371/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1986321371/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc16.c  -o ${OBJECTDIR}/_ext/1986321371/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1986321371/crc16.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1986321371/crc16.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1986321371/crc32.o: ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1986321371 
	@${RM} ${OBJECTDIR}/_ext/1986321371/crc32.o.d 
	@${RM} ${OBJECTDIR}/_ext/1986321371/crc32.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/utils/crc32.c  -o ${OBJECTDIR}/_ext/1986321371/crc32.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1986321371/crc32.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1986321371/crc32.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_brdcfg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_brdcfg.c  -o ${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_brdcfg.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_arch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_arch.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_arch.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_base.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_base.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_base.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_can.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_can.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_crc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_crc.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_crc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_eeprom.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_eeprom.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_flash.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_flash.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_gpio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_gpio.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_gpio.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_led.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_led.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_led.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_sys.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_sys.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_sys.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o: ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2032153785 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/abslayer/hal-package/hal/hal-src/hal_dspic_timer.c  -o ${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/2032153785/hal_dspic_timer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864751918/eEpermdata.o: ../../../../../body/embenv/envcom/eEpermdata.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864751918 
	@${RM} ${OBJECTDIR}/_ext/1864751918/eEpermdata.o.d 
	@${RM} ${OBJECTDIR}/_ext/1864751918/eEpermdata.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/embenv/envcom/eEpermdata.c  -o ${OBJECTDIR}/_ext/1864751918/eEpermdata.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864751918/eEpermdata.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864751918/eEpermdata.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o: ../../../../../body/embenv/envcom/eEsharedInfo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864751918 
	@${RM} ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/embenv/envcom/eEsharedInfo.c  -o ${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864751918/eEsharedInfo.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o: ../../hal-stg/hal_dspic_startup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1482652236 
	@${RM} ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../hal-stg/hal_dspic_startup.c  -o ${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1482652236/hal_dspic_startup.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o: ../../../../../body/embenv/envshalib/eEsharedStorage.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1266031256 
	@${RM} ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o.d 
	@${RM} ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../body/embenv/envshalib/eEsharedStorage.c  -o ${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1266031256/eEsharedStorage.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/189820923/eEcommon.o: ../../../../../../any-arch/body/embenv/envcom/eEcommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/189820923 
	@${RM} ${OBJECTDIR}/_ext/189820923/eEcommon.o.d 
	@${RM} ${OBJECTDIR}/_ext/189820923/eEcommon.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/body/embenv/envcom/eEcommon.c  -o ${OBJECTDIR}/_ext/189820923/eEcommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/189820923/eEcommon.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/189820923/eEcommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOaction.o: ../../../../../../any-arch/sys/embobj/core/src/EOaction.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOaction.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOaction.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOaction.c  -o ${OBJECTDIR}/_ext/471949716/EOaction.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOaction.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOaction.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOarray.o: ../../../../../../any-arch/sys/embobj/core/src/EOarray.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOarray.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOarray.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOarray.c  -o ${OBJECTDIR}/_ext/471949716/EOarray.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOarray.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOarray.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EoCommon.o: ../../../../../../any-arch/sys/embobj/core/src/EoCommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EoCommon.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EoCommon.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EoCommon.c  -o ${OBJECTDIR}/_ext/471949716/EoCommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EoCommon.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EoCommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOconstarray.o: ../../../../../../any-arch/sys/embobj/core/src/EOconstarray.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOconstarray.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOconstarray.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOconstarray.c  -o ${OBJECTDIR}/_ext/471949716/EOconstarray.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOconstarray.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOconstarray.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOconstvector.o: ../../../../../../any-arch/sys/embobj/core/src/EOconstvector.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOconstvector.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOconstvector.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOconstvector.c  -o ${OBJECTDIR}/_ext/471949716/EOconstvector.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOconstvector.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOconstvector.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOdeque.o: ../../../../../../any-arch/sys/embobj/core/src/EOdeque.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOdeque.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOdeque.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOdeque.c  -o ${OBJECTDIR}/_ext/471949716/EOdeque.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOdeque.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOdeque.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOfifo.o: ../../../../../../any-arch/sys/embobj/core/src/EOfifo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOfifo.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOfifo.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOfifo.c  -o ${OBJECTDIR}/_ext/471949716/EOfifo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOfifo.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOfifo.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOfifoByte.o: ../../../../../../any-arch/sys/embobj/core/src/EOfifoByte.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOfifoByte.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOfifoByte.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOfifoByte.c  -o ${OBJECTDIR}/_ext/471949716/EOfifoByte.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOfifoByte.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOfifoByte.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOfifoWord.o: ../../../../../../any-arch/sys/embobj/core/src/EOfifoWord.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOfifoWord.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOfifoWord.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOfifoWord.c  -o ${OBJECTDIR}/_ext/471949716/EOfifoWord.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOfifoWord.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOfifoWord.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOlist.o: ../../../../../../any-arch/sys/embobj/core/src/EOlist.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOlist.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOlist.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOlist.c  -o ${OBJECTDIR}/_ext/471949716/EOlist.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOlist.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOlist.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOtheErrorManager.o: ../../../../../../any-arch/sys/embobj/core/src/EOtheErrorManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOtheErrorManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOtheErrorManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOtheErrorManager.c  -o ${OBJECTDIR}/_ext/471949716/EOtheErrorManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOtheErrorManager.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOtheErrorManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOtheMemoryPool.o: ../../../../../../any-arch/sys/embobj/core/src/EOtheMemoryPool.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOtheMemoryPool.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOtheMemoryPool.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOtheMemoryPool.c  -o ${OBJECTDIR}/_ext/471949716/EOtheMemoryPool.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOtheMemoryPool.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOtheMemoryPool.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOtimer.o: ../../../../../../any-arch/sys/embobj/core/src/EOtimer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOtimer.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOtimer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOtimer.c  -o ${OBJECTDIR}/_ext/471949716/EOtimer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOtimer.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOtimer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/471949716/EOvector.o: ../../../../../../any-arch/sys/embobj/core/src/EOvector.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/471949716 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOvector.o.d 
	@${RM} ${OBJECTDIR}/_ext/471949716/EOvector.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core/src/EOvector.c  -o ${OBJECTDIR}/_ext/471949716/EOvector.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/471949716/EOvector.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/471949716/EOvector.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/910568117/EOVmutex.o: ../../../../../../any-arch/sys/embobj/core-vee/src/EOVmutex.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/910568117 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVmutex.o.d 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVmutex.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-vee/src/EOVmutex.c  -o ${OBJECTDIR}/_ext/910568117/EOVmutex.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/910568117/EOVmutex.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/910568117/EOVmutex.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/910568117/EOVtask.o: ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtask.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/910568117 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVtask.o.d 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVtask.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtask.c  -o ${OBJECTDIR}/_ext/910568117/EOVtask.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/910568117/EOVtask.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/910568117/EOVtask.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/910568117/EOVtheCallbackManager.o: ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheCallbackManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/910568117 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVtheCallbackManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVtheCallbackManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheCallbackManager.c  -o ${OBJECTDIR}/_ext/910568117/EOVtheCallbackManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/910568117/EOVtheCallbackManager.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/910568117/EOVtheCallbackManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/910568117/EOVtheSystem.o: ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheSystem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/910568117 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVtheSystem.o.d 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVtheSystem.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheSystem.c  -o ${OBJECTDIR}/_ext/910568117/EOVtheSystem.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/910568117/EOVtheSystem.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/910568117/EOVtheSystem.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/910568117/EOVtheTimerManager.o: ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheTimerManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/910568117 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVtheTimerManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/910568117/EOVtheTimerManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-vee/src/EOVtheTimerManager.c  -o ${OBJECTDIR}/_ext/910568117/EOVtheTimerManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/910568117/EOVtheTimerManager.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/910568117/EOVtheTimerManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1751942926/EOSmutex.o: ../../../../../../any-arch/sys/embobj/core-see/src/EOSmutex.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1751942926 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOSmutex.o.d 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOSmutex.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-see/src/EOSmutex.c  -o ${OBJECTDIR}/_ext/1751942926/EOSmutex.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1751942926/EOSmutex.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1751942926/EOSmutex.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1751942926/EOStheCallbackManager.o: ../../../../../../any-arch/sys/embobj/core-see/src/EOStheCallbackManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1751942926 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOStheCallbackManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOStheCallbackManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-see/src/EOStheCallbackManager.c  -o ${OBJECTDIR}/_ext/1751942926/EOStheCallbackManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1751942926/EOStheCallbackManager.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1751942926/EOStheCallbackManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1751942926/EOStheFOOP.o: ../../../../../../any-arch/sys/embobj/core-see/src/EOStheFOOP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1751942926 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOStheFOOP.o.d 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOStheFOOP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-see/src/EOStheFOOP.c  -o ${OBJECTDIR}/_ext/1751942926/EOStheFOOP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1751942926/EOStheFOOP.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1751942926/EOStheFOOP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1751942926/EOStheSystem.o: ../../../../../../any-arch/sys/embobj/core-see/src/EOStheSystem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1751942926 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOStheSystem.o.d 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOStheSystem.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-see/src/EOStheSystem.c  -o ${OBJECTDIR}/_ext/1751942926/EOStheSystem.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1751942926/EOStheSystem.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1751942926/EOStheSystem.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1751942926/EOStheTimerManager.o: ../../../../../../any-arch/sys/embobj/core-see/src/EOStheTimerManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1751942926 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOStheTimerManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/1751942926/EOStheTimerManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../any-arch/sys/embobj/core-see/src/EOStheTimerManager.c  -o ${OBJECTDIR}/_ext/1751942926/EOStheTimerManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1751942926/EOStheTimerManager.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1751942926/EOStheTimerManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o: ../../app/eos_applembobj_info.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/eos_applembobj_info.c  -o ${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/eos_applembobj_info.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o: ../../app/eos_applembobj_spec.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/eos_applembobj_spec.c  -o ${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o.d"      -g -omf=elf -O0 -I".." -I"../../app" -I"../../hal-cfg" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I"../../../../../body/abslayer/hal-package/hal/hal-src" -I"../../../../../../porting" -I"../../../../../../any-arch/body/embenv/envcom" -I"../../../../../../any-arch/sys/abslayer/api" -I"../../../../../../any-arch/sys/embobj/core/api" -I"../../../../../../any-arch/sys/embobj/core/src" -I"../../../../../../any-arch/sys/embobj/core-vee/api" -I"../../../../../../any-arch/sys/embobj/core-vee/src" -I"../../../../../../any-arch/sys/embobj/core-see/api" -I"../../../../../../any-arch/sys/embobj/core-see/src" -D_DSPIC_ -DHAL_USE_ARCH_DSPIC -DHAL_BOARD_6SG -DHAL_DONT_INLINE -msmart-io=1 -Wall -msfr-warn=off -finline
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/eos_applembobj_spec.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
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
dist/${CND_CONF}/${IMAGE_TYPE}/appl-embobj.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ../eElinkerscript_applic.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/appl-embobj.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--heap=4096,--stack=1024,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library-path="..",--no-force-link,--smart-io,-Map="${DISTDIR}/appl-embobj.X.${IMAGE_TYPE}.map",--report-mem,--warn-section-align$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/appl-embobj.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ../eElinkerscript_applic.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/appl-embobj.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -Wl,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--heap=4096,--stack=1024,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library-path="..",--no-force-link,--smart-io,-Map="${DISTDIR}/appl-embobj.X.${IMAGE_TYPE}.map",--report-mem,--warn-section-align$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/appl-embobj.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf 
	
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

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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/SKIN.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/SKIN.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../can_interface.c ../I2C.c ../LED.c ../main_V3.c ../traps.c ../eeprom.c ../timers.c ../ADC.c ../lis331dlh.c ../l3g4200d.c ../AD7147_Configuration.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/can_interface.o ${OBJECTDIR}/_ext/1472/I2C.o ${OBJECTDIR}/_ext/1472/LED.o ${OBJECTDIR}/_ext/1472/main_V3.o ${OBJECTDIR}/_ext/1472/traps.o ${OBJECTDIR}/_ext/1472/eeprom.o ${OBJECTDIR}/_ext/1472/timers.o ${OBJECTDIR}/_ext/1472/ADC.o ${OBJECTDIR}/_ext/1472/lis331dlh.o ${OBJECTDIR}/_ext/1472/l3g4200d.o ${OBJECTDIR}/_ext/1472/AD7147_Configuration.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/can_interface.o.d ${OBJECTDIR}/_ext/1472/I2C.o.d ${OBJECTDIR}/_ext/1472/LED.o.d ${OBJECTDIR}/_ext/1472/main_V3.o.d ${OBJECTDIR}/_ext/1472/traps.o.d ${OBJECTDIR}/_ext/1472/eeprom.o.d ${OBJECTDIR}/_ext/1472/timers.o.d ${OBJECTDIR}/_ext/1472/ADC.o.d ${OBJECTDIR}/_ext/1472/lis331dlh.o.d ${OBJECTDIR}/_ext/1472/l3g4200d.o.d ${OBJECTDIR}/_ext/1472/AD7147_Configuration.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/can_interface.o ${OBJECTDIR}/_ext/1472/I2C.o ${OBJECTDIR}/_ext/1472/LED.o ${OBJECTDIR}/_ext/1472/main_V3.o ${OBJECTDIR}/_ext/1472/traps.o ${OBJECTDIR}/_ext/1472/eeprom.o ${OBJECTDIR}/_ext/1472/timers.o ${OBJECTDIR}/_ext/1472/ADC.o ${OBJECTDIR}/_ext/1472/lis331dlh.o ${OBJECTDIR}/_ext/1472/l3g4200d.o ${OBJECTDIR}/_ext/1472/AD7147_Configuration.o

# Source Files
SOURCEFILES=../can_interface.c ../I2C.c ../LED.c ../main_V3.c ../traps.c ../eeprom.c ../timers.c ../ADC.c ../lis331dlh.c ../l3g4200d.c ../AD7147_Configuration.c


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
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/SKIN.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=30F4011
MP_LINKER_FILE_OPTION=,--script="..\p30f4011.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/can_interface.o: ../can_interface.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/can_interface.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/can_interface.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../can_interface.c  -o ${OBJECTDIR}/_ext/1472/can_interface.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/can_interface.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/can_interface.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/I2C.o: ../I2C.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/I2C.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/I2C.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../I2C.c  -o ${OBJECTDIR}/_ext/1472/I2C.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/I2C.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/I2C.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/LED.o: ../LED.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/LED.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/LED.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../LED.c  -o ${OBJECTDIR}/_ext/1472/LED.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/LED.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/LED.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/main_V3.o: ../main_V3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main_V3.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main_V3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../main_V3.c  -o ${OBJECTDIR}/_ext/1472/main_V3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/main_V3.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main_V3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/traps.o: ../traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../traps.c  -o ${OBJECTDIR}/_ext/1472/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/traps.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/eeprom.o: ../eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/eeprom.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/eeprom.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../eeprom.c  -o ${OBJECTDIR}/_ext/1472/eeprom.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/eeprom.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/eeprom.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/timers.o: ../timers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/timers.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/timers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../timers.c  -o ${OBJECTDIR}/_ext/1472/timers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/timers.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/timers.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/ADC.o: ../ADC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../ADC.c  -o ${OBJECTDIR}/_ext/1472/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/ADC.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/ADC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/lis331dlh.o: ../lis331dlh.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/lis331dlh.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/lis331dlh.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lis331dlh.c  -o ${OBJECTDIR}/_ext/1472/lis331dlh.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/lis331dlh.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/lis331dlh.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/l3g4200d.o: ../l3g4200d.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/l3g4200d.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/l3g4200d.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../l3g4200d.c  -o ${OBJECTDIR}/_ext/1472/l3g4200d.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/l3g4200d.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/l3g4200d.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/AD7147_Configuration.o: ../AD7147_Configuration.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/AD7147_Configuration.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/AD7147_Configuration.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../AD7147_Configuration.c  -o ${OBJECTDIR}/_ext/1472/AD7147_Configuration.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/AD7147_Configuration.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/AD7147_Configuration.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1472/can_interface.o: ../can_interface.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/can_interface.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/can_interface.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../can_interface.c  -o ${OBJECTDIR}/_ext/1472/can_interface.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/can_interface.o.d"      -g -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/can_interface.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/I2C.o: ../I2C.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/I2C.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/I2C.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../I2C.c  -o ${OBJECTDIR}/_ext/1472/I2C.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/I2C.o.d"      -g -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/I2C.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/LED.o: ../LED.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/LED.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/LED.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../LED.c  -o ${OBJECTDIR}/_ext/1472/LED.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/LED.o.d"      -g -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/LED.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/main_V3.o: ../main_V3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main_V3.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main_V3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../main_V3.c  -o ${OBJECTDIR}/_ext/1472/main_V3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/main_V3.o.d"      -g -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main_V3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/traps.o: ../traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../traps.c  -o ${OBJECTDIR}/_ext/1472/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/traps.o.d"      -g -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/eeprom.o: ../eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/eeprom.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/eeprom.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../eeprom.c  -o ${OBJECTDIR}/_ext/1472/eeprom.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/eeprom.o.d"      -g -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/eeprom.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/timers.o: ../timers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/timers.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/timers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../timers.c  -o ${OBJECTDIR}/_ext/1472/timers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/timers.o.d"      -g -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/timers.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/ADC.o: ../ADC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../ADC.c  -o ${OBJECTDIR}/_ext/1472/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/ADC.o.d"      -g -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/ADC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/lis331dlh.o: ../lis331dlh.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/lis331dlh.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/lis331dlh.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lis331dlh.c  -o ${OBJECTDIR}/_ext/1472/lis331dlh.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/lis331dlh.o.d"      -g -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/lis331dlh.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/l3g4200d.o: ../l3g4200d.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/l3g4200d.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/l3g4200d.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../l3g4200d.c  -o ${OBJECTDIR}/_ext/1472/l3g4200d.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/l3g4200d.o.d"      -g -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/l3g4200d.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/AD7147_Configuration.o: ../AD7147_Configuration.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/AD7147_Configuration.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/AD7147_Configuration.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../AD7147_Configuration.c  -o ${OBJECTDIR}/_ext/1472/AD7147_Configuration.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/AD7147_Configuration.o.d"      -g -omf=elf -O3 -I".." -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/AD7147_Configuration.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
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
dist/${CND_CONF}/${IMAGE_TYPE}/SKIN.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ../p30f4011.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/SKIN.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf  -mreserve=data@0x800:0x81F -mreserve=data@0x820:0x821 -mreserve=data@0x822:0x823 -mreserve=data@0x824:0x84F   -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,$(MP_LINKER_FILE_OPTION),--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library-path="..",--no-force-link,--smart-io,-Map="${DISTDIR}/SKIN.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/SKIN.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ../p30f4011.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/SKIN.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -Wl,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library-path="..",--no-force-link,--smart-io,-Map="${DISTDIR}/SKIN.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/SKIN.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf 
	
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

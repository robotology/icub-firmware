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

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/can_interface.o ${OBJECTDIR}/_ext/1472/can_protocol.o ${OBJECTDIR}/_ext/1472/I2C.o ${OBJECTDIR}/_ext/1472/LED.o ${OBJECTDIR}/_ext/1472/main_V3.o ${OBJECTDIR}/_ext/1472/traps.o "${OBJECTDIR}/_ext/1472/AD7147 - Configuration.o" ${OBJECTDIR}/_ext/1472/eeprom.o ${OBJECTDIR}/_ext/1472/timers.o ${OBJECTDIR}/_ext/1472/ADC.o ${OBJECTDIR}/_ext/1472/lis331dlh.o ${OBJECTDIR}/_ext/1472/l3g4200d.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/can_interface.o.d ${OBJECTDIR}/_ext/1472/can_protocol.o.d ${OBJECTDIR}/_ext/1472/I2C.o.d ${OBJECTDIR}/_ext/1472/LED.o.d ${OBJECTDIR}/_ext/1472/main_V3.o.d ${OBJECTDIR}/_ext/1472/traps.o.d "${OBJECTDIR}/_ext/1472/AD7147 - Configuration.o.d" ${OBJECTDIR}/_ext/1472/eeprom.o.d ${OBJECTDIR}/_ext/1472/timers.o.d ${OBJECTDIR}/_ext/1472/ADC.o.d ${OBJECTDIR}/_ext/1472/lis331dlh.o.d ${OBJECTDIR}/_ext/1472/l3g4200d.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/can_interface.o ${OBJECTDIR}/_ext/1472/can_protocol.o ${OBJECTDIR}/_ext/1472/I2C.o ${OBJECTDIR}/_ext/1472/LED.o ${OBJECTDIR}/_ext/1472/main_V3.o ${OBJECTDIR}/_ext/1472/traps.o ${OBJECTDIR}/_ext/1472/AD7147\ -\ Configuration.o ${OBJECTDIR}/_ext/1472/eeprom.o ${OBJECTDIR}/_ext/1472/timers.o ${OBJECTDIR}/_ext/1472/ADC.o ${OBJECTDIR}/_ext/1472/lis331dlh.o ${OBJECTDIR}/_ext/1472/l3g4200d.o


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
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/can_interface.o: ../can_interface.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/can_interface.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/can_interface.o.ok ${OBJECTDIR}/_ext/1472/can_interface.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/can_interface.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/can_interface.o.d" -o ${OBJECTDIR}/_ext/1472/can_interface.o ../can_interface.c    
	
${OBJECTDIR}/_ext/1472/can_protocol.o: ../can_protocol.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/can_protocol.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/can_protocol.o.ok ${OBJECTDIR}/_ext/1472/can_protocol.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/can_protocol.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/can_protocol.o.d" -o ${OBJECTDIR}/_ext/1472/can_protocol.o ../can_protocol.c    
	
${OBJECTDIR}/_ext/1472/I2C.o: ../I2C.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/I2C.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/I2C.o.ok ${OBJECTDIR}/_ext/1472/I2C.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/I2C.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/I2C.o.d" -o ${OBJECTDIR}/_ext/1472/I2C.o ../I2C.c    
	
${OBJECTDIR}/_ext/1472/LED.o: ../LED.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/LED.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/LED.o.ok ${OBJECTDIR}/_ext/1472/LED.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/LED.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/LED.o.d" -o ${OBJECTDIR}/_ext/1472/LED.o ../LED.c    
	
${OBJECTDIR}/_ext/1472/main_V3.o: ../main_V3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main_V3.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main_V3.o.ok ${OBJECTDIR}/_ext/1472/main_V3.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main_V3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/main_V3.o.d" -o ${OBJECTDIR}/_ext/1472/main_V3.o ../main_V3.c    
	
${OBJECTDIR}/_ext/1472/traps.o: ../traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/traps.o.ok ${OBJECTDIR}/_ext/1472/traps.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/traps.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/traps.o.d" -o ${OBJECTDIR}/_ext/1472/traps.o ../traps.c    
	
${OBJECTDIR}/_ext/1472/AD7147\ -\ Configuration.o: ../AD7147\ -\ Configuration.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/AD7147\ -\ Configuration.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/AD7147\ -\ Configuration.o.ok ${OBJECTDIR}/_ext/1472/AD7147\ -\ Configuration.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/AD7147 - Configuration.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/AD7147 - Configuration.o.d" -o "${OBJECTDIR}/_ext/1472/AD7147 - Configuration.o" "../AD7147 - Configuration.c"    
	
${OBJECTDIR}/_ext/1472/eeprom.o: ../eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/eeprom.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/eeprom.o.ok ${OBJECTDIR}/_ext/1472/eeprom.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/eeprom.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/eeprom.o.d" -o ${OBJECTDIR}/_ext/1472/eeprom.o ../eeprom.c    
	
${OBJECTDIR}/_ext/1472/timers.o: ../timers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/timers.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/timers.o.ok ${OBJECTDIR}/_ext/1472/timers.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/timers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/timers.o.d" -o ${OBJECTDIR}/_ext/1472/timers.o ../timers.c    
	
${OBJECTDIR}/_ext/1472/ADC.o: ../ADC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/ADC.o.ok ${OBJECTDIR}/_ext/1472/ADC.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/ADC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/ADC.o.d" -o ${OBJECTDIR}/_ext/1472/ADC.o ../ADC.c    
	
${OBJECTDIR}/_ext/1472/lis331dlh.o: ../lis331dlh.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/lis331dlh.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/lis331dlh.o.ok ${OBJECTDIR}/_ext/1472/lis331dlh.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/lis331dlh.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/lis331dlh.o.d" -o ${OBJECTDIR}/_ext/1472/lis331dlh.o ../lis331dlh.c    
	
${OBJECTDIR}/_ext/1472/l3g4200d.o: ../l3g4200d.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/l3g4200d.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/l3g4200d.o.ok ${OBJECTDIR}/_ext/1472/l3g4200d.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/l3g4200d.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/l3g4200d.o.d" -o ${OBJECTDIR}/_ext/1472/l3g4200d.o ../l3g4200d.c    
	
else
${OBJECTDIR}/_ext/1472/can_interface.o: ../can_interface.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/can_interface.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/can_interface.o.ok ${OBJECTDIR}/_ext/1472/can_interface.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/can_interface.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/can_interface.o.d" -o ${OBJECTDIR}/_ext/1472/can_interface.o ../can_interface.c    
	
${OBJECTDIR}/_ext/1472/can_protocol.o: ../can_protocol.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/can_protocol.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/can_protocol.o.ok ${OBJECTDIR}/_ext/1472/can_protocol.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/can_protocol.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/can_protocol.o.d" -o ${OBJECTDIR}/_ext/1472/can_protocol.o ../can_protocol.c    
	
${OBJECTDIR}/_ext/1472/I2C.o: ../I2C.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/I2C.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/I2C.o.ok ${OBJECTDIR}/_ext/1472/I2C.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/I2C.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/I2C.o.d" -o ${OBJECTDIR}/_ext/1472/I2C.o ../I2C.c    
	
${OBJECTDIR}/_ext/1472/LED.o: ../LED.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/LED.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/LED.o.ok ${OBJECTDIR}/_ext/1472/LED.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/LED.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/LED.o.d" -o ${OBJECTDIR}/_ext/1472/LED.o ../LED.c    
	
${OBJECTDIR}/_ext/1472/main_V3.o: ../main_V3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main_V3.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main_V3.o.ok ${OBJECTDIR}/_ext/1472/main_V3.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main_V3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/main_V3.o.d" -o ${OBJECTDIR}/_ext/1472/main_V3.o ../main_V3.c    
	
${OBJECTDIR}/_ext/1472/traps.o: ../traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/traps.o.ok ${OBJECTDIR}/_ext/1472/traps.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/traps.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/traps.o.d" -o ${OBJECTDIR}/_ext/1472/traps.o ../traps.c    
	
${OBJECTDIR}/_ext/1472/AD7147\ -\ Configuration.o: ../AD7147\ -\ Configuration.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/AD7147\ -\ Configuration.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/AD7147\ -\ Configuration.o.ok ${OBJECTDIR}/_ext/1472/AD7147\ -\ Configuration.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/AD7147 - Configuration.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/AD7147 - Configuration.o.d" -o "${OBJECTDIR}/_ext/1472/AD7147 - Configuration.o" "../AD7147 - Configuration.c"    
	
${OBJECTDIR}/_ext/1472/eeprom.o: ../eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/eeprom.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/eeprom.o.ok ${OBJECTDIR}/_ext/1472/eeprom.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/eeprom.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/eeprom.o.d" -o ${OBJECTDIR}/_ext/1472/eeprom.o ../eeprom.c    
	
${OBJECTDIR}/_ext/1472/timers.o: ../timers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/timers.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/timers.o.ok ${OBJECTDIR}/_ext/1472/timers.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/timers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/timers.o.d" -o ${OBJECTDIR}/_ext/1472/timers.o ../timers.c    
	
${OBJECTDIR}/_ext/1472/ADC.o: ../ADC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/ADC.o.ok ${OBJECTDIR}/_ext/1472/ADC.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/ADC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/ADC.o.d" -o ${OBJECTDIR}/_ext/1472/ADC.o ../ADC.c    
	
${OBJECTDIR}/_ext/1472/lis331dlh.o: ../lis331dlh.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/lis331dlh.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/lis331dlh.o.ok ${OBJECTDIR}/_ext/1472/lis331dlh.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/lis331dlh.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/lis331dlh.o.d" -o ${OBJECTDIR}/_ext/1472/lis331dlh.o ../lis331dlh.c    
	
${OBJECTDIR}/_ext/1472/l3g4200d.o: ../l3g4200d.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/l3g4200d.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/l3g4200d.o.ok ${OBJECTDIR}/_ext/1472/l3g4200d.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/l3g4200d.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -O3 -fno-schedule-insns -fno-schedule-insns2 -MMD -MF "${OBJECTDIR}/_ext/1472/l3g4200d.o.d" -o ${OBJECTDIR}/_ext/1472/l3g4200d.o ../l3g4200d.c    
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/SKIN.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ../p30f4011.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/SKIN.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,-L"..",-Map="${DISTDIR}/SKIN.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1
else
dist/${CND_CONF}/${IMAGE_TYPE}/SKIN.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ../p30f4011.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/SKIN.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,-L"..",-Map="${DISTDIR}/SKIN.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}\\pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/SKIN.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=elf
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

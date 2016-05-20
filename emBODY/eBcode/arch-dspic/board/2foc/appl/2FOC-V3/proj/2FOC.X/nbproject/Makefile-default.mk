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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/2FOC.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/2FOC.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../app/2FOC.c ../../app/ADC.c ../../app/DCLink.c ../../app/Faults.c ../../app/PWM.c ../../app/System.c ../../app/crc16.c ../../app/ecan.c ../../app/qep.c ../../app/traps.c ../../app/i2cTsens.c ../../app/DHES.c ../../app/MeasCurr.s ../../app-added/can_icubProto.c ../../app-added/can_icubProto_parser.c ../../app-added/can_icubProto_trasmitter.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1445257345/2FOC.o ${OBJECTDIR}/_ext/1445257345/ADC.o ${OBJECTDIR}/_ext/1445257345/DCLink.o ${OBJECTDIR}/_ext/1445257345/Faults.o ${OBJECTDIR}/_ext/1445257345/PWM.o ${OBJECTDIR}/_ext/1445257345/System.o ${OBJECTDIR}/_ext/1445257345/crc16.o ${OBJECTDIR}/_ext/1445257345/ecan.o ${OBJECTDIR}/_ext/1445257345/qep.o ${OBJECTDIR}/_ext/1445257345/traps.o ${OBJECTDIR}/_ext/1445257345/i2cTsens.o ${OBJECTDIR}/_ext/1445257345/DHES.o ${OBJECTDIR}/_ext/1445257345/MeasCurr.o ${OBJECTDIR}/_ext/1722765132/can_icubProto.o ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1445257345/2FOC.o.d ${OBJECTDIR}/_ext/1445257345/ADC.o.d ${OBJECTDIR}/_ext/1445257345/DCLink.o.d ${OBJECTDIR}/_ext/1445257345/Faults.o.d ${OBJECTDIR}/_ext/1445257345/PWM.o.d ${OBJECTDIR}/_ext/1445257345/System.o.d ${OBJECTDIR}/_ext/1445257345/crc16.o.d ${OBJECTDIR}/_ext/1445257345/ecan.o.d ${OBJECTDIR}/_ext/1445257345/qep.o.d ${OBJECTDIR}/_ext/1445257345/traps.o.d ${OBJECTDIR}/_ext/1445257345/i2cTsens.o.d ${OBJECTDIR}/_ext/1445257345/DHES.o.d ${OBJECTDIR}/_ext/1445257345/MeasCurr.o.d ${OBJECTDIR}/_ext/1722765132/can_icubProto.o.d ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o.d ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1445257345/2FOC.o ${OBJECTDIR}/_ext/1445257345/ADC.o ${OBJECTDIR}/_ext/1445257345/DCLink.o ${OBJECTDIR}/_ext/1445257345/Faults.o ${OBJECTDIR}/_ext/1445257345/PWM.o ${OBJECTDIR}/_ext/1445257345/System.o ${OBJECTDIR}/_ext/1445257345/crc16.o ${OBJECTDIR}/_ext/1445257345/ecan.o ${OBJECTDIR}/_ext/1445257345/qep.o ${OBJECTDIR}/_ext/1445257345/traps.o ${OBJECTDIR}/_ext/1445257345/i2cTsens.o ${OBJECTDIR}/_ext/1445257345/DHES.o ${OBJECTDIR}/_ext/1445257345/MeasCurr.o ${OBJECTDIR}/_ext/1722765132/can_icubProto.o ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o

# Source Files
SOURCEFILES=../../app/2FOC.c ../../app/ADC.c ../../app/DCLink.c ../../app/Faults.c ../../app/PWM.c ../../app/System.c ../../app/crc16.c ../../app/ecan.c ../../app/qep.c ../../app/traps.c ../../app/i2cTsens.c ../../app/DHES.c ../../app/MeasCurr.s ../../app-added/can_icubProto.c ../../app-added/can_icubProto_parser.c ../../app-added/can_icubProto_trasmitter.c


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
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/2FOC.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ128MC802
MP_LINKER_FILE_OPTION=,--script="..\eElinkerscript_appl_2FOC.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1445257345/2FOC.o: ../../app/2FOC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/2FOC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/2FOC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/2FOC.c  -o ${OBJECTDIR}/_ext/1445257345/2FOC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/2FOC.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/2FOC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/ADC.o: ../../app/ADC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/ADC.c  -o ${OBJECTDIR}/_ext/1445257345/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/ADC.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/ADC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/DCLink.o: ../../app/DCLink.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/DCLink.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/DCLink.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/DCLink.c  -o ${OBJECTDIR}/_ext/1445257345/DCLink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/DCLink.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/DCLink.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/Faults.o: ../../app/Faults.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Faults.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Faults.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/Faults.c  -o ${OBJECTDIR}/_ext/1445257345/Faults.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/Faults.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/Faults.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/PWM.o: ../../app/PWM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/PWM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/PWM.c  -o ${OBJECTDIR}/_ext/1445257345/PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/PWM.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/PWM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/System.o: ../../app/System.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/System.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/System.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/System.c  -o ${OBJECTDIR}/_ext/1445257345/System.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/System.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/System.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/crc16.o: ../../app/crc16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/crc16.c  -o ${OBJECTDIR}/_ext/1445257345/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/crc16.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/crc16.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/ecan.o: ../../app/ecan.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/ecan.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/ecan.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/ecan.c  -o ${OBJECTDIR}/_ext/1445257345/ecan.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/ecan.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/ecan.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/qep.o: ../../app/qep.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/qep.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/qep.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/qep.c  -o ${OBJECTDIR}/_ext/1445257345/qep.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/qep.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/qep.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/traps.o: ../../app/traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/traps.c  -o ${OBJECTDIR}/_ext/1445257345/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/traps.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/i2cTsens.o: ../../app/i2cTsens.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/i2cTsens.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/i2cTsens.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/i2cTsens.c  -o ${OBJECTDIR}/_ext/1445257345/i2cTsens.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/i2cTsens.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/i2cTsens.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/DHES.o: ../../app/DHES.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/DHES.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/DHES.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/DHES.c  -o ${OBJECTDIR}/_ext/1445257345/DHES.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/DHES.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/DHES.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1722765132/can_icubProto.o: ../../app-added/can_icubProto.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1722765132 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto.o.d 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app-added/can_icubProto.c  -o ${OBJECTDIR}/_ext/1722765132/can_icubProto.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1722765132/can_icubProto.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1722765132/can_icubProto.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o: ../../app-added/can_icubProto_parser.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1722765132 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o.d 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app-added/can_icubProto_parser.c  -o ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o: ../../app-added/can_icubProto_trasmitter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1722765132 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app-added/can_icubProto_trasmitter.c  -o ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1445257345/2FOC.o: ../../app/2FOC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/2FOC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/2FOC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/2FOC.c  -o ${OBJECTDIR}/_ext/1445257345/2FOC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/2FOC.o.d"      -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/2FOC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/ADC.o: ../../app/ADC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/ADC.c  -o ${OBJECTDIR}/_ext/1445257345/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/ADC.o.d"      -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/ADC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/DCLink.o: ../../app/DCLink.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/DCLink.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/DCLink.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/DCLink.c  -o ${OBJECTDIR}/_ext/1445257345/DCLink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/DCLink.o.d"      -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/DCLink.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/Faults.o: ../../app/Faults.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Faults.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Faults.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/Faults.c  -o ${OBJECTDIR}/_ext/1445257345/Faults.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/Faults.o.d"      -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/Faults.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/PWM.o: ../../app/PWM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/PWM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/PWM.c  -o ${OBJECTDIR}/_ext/1445257345/PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/PWM.o.d"      -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/PWM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/System.o: ../../app/System.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/System.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/System.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/System.c  -o ${OBJECTDIR}/_ext/1445257345/System.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/System.o.d"      -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/System.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/crc16.o: ../../app/crc16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/crc16.c  -o ${OBJECTDIR}/_ext/1445257345/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/crc16.o.d"      -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/crc16.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/ecan.o: ../../app/ecan.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/ecan.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/ecan.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/ecan.c  -o ${OBJECTDIR}/_ext/1445257345/ecan.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/ecan.o.d"      -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/ecan.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/qep.o: ../../app/qep.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/qep.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/qep.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/qep.c  -o ${OBJECTDIR}/_ext/1445257345/qep.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/qep.o.d"      -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/qep.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/traps.o: ../../app/traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/traps.c  -o ${OBJECTDIR}/_ext/1445257345/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/traps.o.d"      -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/i2cTsens.o: ../../app/i2cTsens.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/i2cTsens.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/i2cTsens.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/i2cTsens.c  -o ${OBJECTDIR}/_ext/1445257345/i2cTsens.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/i2cTsens.o.d"      -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/i2cTsens.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/DHES.o: ../../app/DHES.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/DHES.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/DHES.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/DHES.c  -o ${OBJECTDIR}/_ext/1445257345/DHES.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/DHES.o.d"      -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/DHES.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1722765132/can_icubProto.o: ../../app-added/can_icubProto.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1722765132 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto.o.d 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app-added/can_icubProto.c  -o ${OBJECTDIR}/_ext/1722765132/can_icubProto.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1722765132/can_icubProto.o.d"      -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1722765132/can_icubProto.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o: ../../app-added/can_icubProto_parser.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1722765132 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o.d 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app-added/can_icubProto_parser.c  -o ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o.d"      -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o: ../../app-added/can_icubProto_trasmitter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1722765132 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app-added/can_icubProto_trasmitter.c  -o ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o.d"      -omf=elf -mlarge-data -O2 -I"../../app-added" -I"../../app" -I"../../../../../../../../../../icub-firmware-shared/can/canProtocolLib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1445257345/MeasCurr.o: ../../app/MeasCurr.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/MeasCurr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/MeasCurr.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/MeasCurr.s  -o ${OBJECTDIR}/_ext/1445257345/MeasCurr.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/MeasCurr.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/MeasCurr.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1445257345/MeasCurr.o: ../../app/MeasCurr.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/MeasCurr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/MeasCurr.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/MeasCurr.s  -o ${OBJECTDIR}/_ext/1445257345/MeasCurr.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/MeasCurr.o.d",--defsym=__MPLAB_BUILD=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/MeasCurr.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/2FOC.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../../libdsp-elf.a  ../eElinkerscript_appl_2FOC.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/2FOC.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}    ..\..\libdsp-elf.a  -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--gc-sections,--fill-upper=0,--stackguard=16,-s,--library-path="..",--no-force-link,--smart-io,-Map="${DISTDIR}/2FOC.X.${IMAGE_TYPE}.map",--report-mem,--warn-section-align$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/2FOC.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../../libdsp-elf.a ../eElinkerscript_appl_2FOC.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/2FOC.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}    ..\..\libdsp-elf.a  -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -Wl,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--gc-sections,--fill-upper=0,--stackguard=16,-s,--library-path="..",--no-force-link,--smart-io,-Map="${DISTDIR}/2FOC.X.${IMAGE_TYPE}.map",--report-mem,--warn-section-align$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/2FOC.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf 
	
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

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
SOURCEFILES_QUOTED_IF_SPACED=../../app/2FOC.c ../../app/ADC.c ../../app/Controller_edl_pid.c ../../app/Controller_mc_pi.c ../../app/Controller_mc_pid.c ../../app/DCLink.c ../../app/DHES.c ../../app/Encoder.c ../../app/Faults.c ../../app/PWM.c ../../app/System.c ../../app/as5045.c ../../app/crc16.c ../../app/ecan.c ../../app/qep.c ../../app/rotor_alignment.c ../../app/tle5012.c ../../app/traps.c ../../app/velocity.c ../../app/CalcRef.s ../../app/I2T.s ../../app/InvPark.s ../../app/MeasCurr.s ../../app/SVGen.s ../../app/clrkpark.s ../../app/pi.s ../../app/pid.s ../../app/pid2.s ../../app/trig.s ../../app-added/can_icubProto.c ../../app-added/can_icubProto_parser.c ../../app-added/can_icubProto_trasmitter.c ../../../../../../libs/services/storage/basic/src/eEbasicStorage.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1445257345/2FOC.o ${OBJECTDIR}/_ext/1445257345/ADC.o ${OBJECTDIR}/_ext/1445257345/Controller_edl_pid.o ${OBJECTDIR}/_ext/1445257345/Controller_mc_pi.o ${OBJECTDIR}/_ext/1445257345/Controller_mc_pid.o ${OBJECTDIR}/_ext/1445257345/DCLink.o ${OBJECTDIR}/_ext/1445257345/DHES.o ${OBJECTDIR}/_ext/1445257345/Encoder.o ${OBJECTDIR}/_ext/1445257345/Faults.o ${OBJECTDIR}/_ext/1445257345/PWM.o ${OBJECTDIR}/_ext/1445257345/System.o ${OBJECTDIR}/_ext/1445257345/as5045.o ${OBJECTDIR}/_ext/1445257345/crc16.o ${OBJECTDIR}/_ext/1445257345/ecan.o ${OBJECTDIR}/_ext/1445257345/qep.o ${OBJECTDIR}/_ext/1445257345/rotor_alignment.o ${OBJECTDIR}/_ext/1445257345/tle5012.o ${OBJECTDIR}/_ext/1445257345/traps.o ${OBJECTDIR}/_ext/1445257345/velocity.o ${OBJECTDIR}/_ext/1445257345/CalcRef.o ${OBJECTDIR}/_ext/1445257345/I2T.o ${OBJECTDIR}/_ext/1445257345/InvPark.o ${OBJECTDIR}/_ext/1445257345/MeasCurr.o ${OBJECTDIR}/_ext/1445257345/SVGen.o ${OBJECTDIR}/_ext/1445257345/clrkpark.o ${OBJECTDIR}/_ext/1445257345/pi.o ${OBJECTDIR}/_ext/1445257345/pid.o ${OBJECTDIR}/_ext/1445257345/pid2.o ${OBJECTDIR}/_ext/1445257345/trig.o ${OBJECTDIR}/_ext/1722765132/can_icubProto.o ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o ${OBJECTDIR}/_ext/1000681919/eEbasicStorage.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1445257345/2FOC.o.d ${OBJECTDIR}/_ext/1445257345/ADC.o.d ${OBJECTDIR}/_ext/1445257345/Controller_edl_pid.o.d ${OBJECTDIR}/_ext/1445257345/Controller_mc_pi.o.d ${OBJECTDIR}/_ext/1445257345/Controller_mc_pid.o.d ${OBJECTDIR}/_ext/1445257345/DCLink.o.d ${OBJECTDIR}/_ext/1445257345/DHES.o.d ${OBJECTDIR}/_ext/1445257345/Encoder.o.d ${OBJECTDIR}/_ext/1445257345/Faults.o.d ${OBJECTDIR}/_ext/1445257345/PWM.o.d ${OBJECTDIR}/_ext/1445257345/System.o.d ${OBJECTDIR}/_ext/1445257345/as5045.o.d ${OBJECTDIR}/_ext/1445257345/crc16.o.d ${OBJECTDIR}/_ext/1445257345/ecan.o.d ${OBJECTDIR}/_ext/1445257345/qep.o.d ${OBJECTDIR}/_ext/1445257345/rotor_alignment.o.d ${OBJECTDIR}/_ext/1445257345/tle5012.o.d ${OBJECTDIR}/_ext/1445257345/traps.o.d ${OBJECTDIR}/_ext/1445257345/velocity.o.d ${OBJECTDIR}/_ext/1445257345/CalcRef.o.d ${OBJECTDIR}/_ext/1445257345/I2T.o.d ${OBJECTDIR}/_ext/1445257345/InvPark.o.d ${OBJECTDIR}/_ext/1445257345/MeasCurr.o.d ${OBJECTDIR}/_ext/1445257345/SVGen.o.d ${OBJECTDIR}/_ext/1445257345/clrkpark.o.d ${OBJECTDIR}/_ext/1445257345/pi.o.d ${OBJECTDIR}/_ext/1445257345/pid.o.d ${OBJECTDIR}/_ext/1445257345/pid2.o.d ${OBJECTDIR}/_ext/1445257345/trig.o.d ${OBJECTDIR}/_ext/1722765132/can_icubProto.o.d ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o.d ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o.d ${OBJECTDIR}/_ext/1000681919/eEbasicStorage.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1445257345/2FOC.o ${OBJECTDIR}/_ext/1445257345/ADC.o ${OBJECTDIR}/_ext/1445257345/Controller_edl_pid.o ${OBJECTDIR}/_ext/1445257345/Controller_mc_pi.o ${OBJECTDIR}/_ext/1445257345/Controller_mc_pid.o ${OBJECTDIR}/_ext/1445257345/DCLink.o ${OBJECTDIR}/_ext/1445257345/DHES.o ${OBJECTDIR}/_ext/1445257345/Encoder.o ${OBJECTDIR}/_ext/1445257345/Faults.o ${OBJECTDIR}/_ext/1445257345/PWM.o ${OBJECTDIR}/_ext/1445257345/System.o ${OBJECTDIR}/_ext/1445257345/as5045.o ${OBJECTDIR}/_ext/1445257345/crc16.o ${OBJECTDIR}/_ext/1445257345/ecan.o ${OBJECTDIR}/_ext/1445257345/qep.o ${OBJECTDIR}/_ext/1445257345/rotor_alignment.o ${OBJECTDIR}/_ext/1445257345/tle5012.o ${OBJECTDIR}/_ext/1445257345/traps.o ${OBJECTDIR}/_ext/1445257345/velocity.o ${OBJECTDIR}/_ext/1445257345/CalcRef.o ${OBJECTDIR}/_ext/1445257345/I2T.o ${OBJECTDIR}/_ext/1445257345/InvPark.o ${OBJECTDIR}/_ext/1445257345/MeasCurr.o ${OBJECTDIR}/_ext/1445257345/SVGen.o ${OBJECTDIR}/_ext/1445257345/clrkpark.o ${OBJECTDIR}/_ext/1445257345/pi.o ${OBJECTDIR}/_ext/1445257345/pid.o ${OBJECTDIR}/_ext/1445257345/pid2.o ${OBJECTDIR}/_ext/1445257345/trig.o ${OBJECTDIR}/_ext/1722765132/can_icubProto.o ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o ${OBJECTDIR}/_ext/1000681919/eEbasicStorage.o

# Source Files
SOURCEFILES=../../app/2FOC.c ../../app/ADC.c ../../app/Controller_edl_pid.c ../../app/Controller_mc_pi.c ../../app/Controller_mc_pid.c ../../app/DCLink.c ../../app/DHES.c ../../app/Encoder.c ../../app/Faults.c ../../app/PWM.c ../../app/System.c ../../app/as5045.c ../../app/crc16.c ../../app/ecan.c ../../app/qep.c ../../app/rotor_alignment.c ../../app/tle5012.c ../../app/traps.c ../../app/velocity.c ../../app/CalcRef.s ../../app/I2T.s ../../app/InvPark.s ../../app/MeasCurr.s ../../app/SVGen.s ../../app/clrkpark.s ../../app/pi.s ../../app/pid.s ../../app/pid2.s ../../app/trig.s ../../app-added/can_icubProto.c ../../app-added/can_icubProto_parser.c ../../app-added/can_icubProto_trasmitter.c ../../../../../../libs/services/storage/basic/src/eEbasicStorage.c


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
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/2FOC.c  -o ${OBJECTDIR}/_ext/1445257345/2FOC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/2FOC.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/2FOC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/ADC.o: ../../app/ADC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/ADC.c  -o ${OBJECTDIR}/_ext/1445257345/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/ADC.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/ADC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/Controller_edl_pid.o: ../../app/Controller_edl_pid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Controller_edl_pid.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Controller_edl_pid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/Controller_edl_pid.c  -o ${OBJECTDIR}/_ext/1445257345/Controller_edl_pid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/Controller_edl_pid.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/Controller_edl_pid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/Controller_mc_pi.o: ../../app/Controller_mc_pi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Controller_mc_pi.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Controller_mc_pi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/Controller_mc_pi.c  -o ${OBJECTDIR}/_ext/1445257345/Controller_mc_pi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/Controller_mc_pi.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/Controller_mc_pi.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/Controller_mc_pid.o: ../../app/Controller_mc_pid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Controller_mc_pid.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Controller_mc_pid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/Controller_mc_pid.c  -o ${OBJECTDIR}/_ext/1445257345/Controller_mc_pid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/Controller_mc_pid.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/Controller_mc_pid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/DCLink.o: ../../app/DCLink.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/DCLink.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/DCLink.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/DCLink.c  -o ${OBJECTDIR}/_ext/1445257345/DCLink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/DCLink.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/DCLink.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/DHES.o: ../../app/DHES.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/DHES.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/DHES.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/DHES.c  -o ${OBJECTDIR}/_ext/1445257345/DHES.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/DHES.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/DHES.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/Encoder.o: ../../app/Encoder.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Encoder.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Encoder.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/Encoder.c  -o ${OBJECTDIR}/_ext/1445257345/Encoder.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/Encoder.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/Encoder.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/Faults.o: ../../app/Faults.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Faults.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Faults.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/Faults.c  -o ${OBJECTDIR}/_ext/1445257345/Faults.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/Faults.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/Faults.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/PWM.o: ../../app/PWM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/PWM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/PWM.c  -o ${OBJECTDIR}/_ext/1445257345/PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/PWM.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/PWM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/System.o: ../../app/System.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/System.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/System.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/System.c  -o ${OBJECTDIR}/_ext/1445257345/System.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/System.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/System.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/as5045.o: ../../app/as5045.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/as5045.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/as5045.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/as5045.c  -o ${OBJECTDIR}/_ext/1445257345/as5045.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/as5045.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/as5045.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/crc16.o: ../../app/crc16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/crc16.c  -o ${OBJECTDIR}/_ext/1445257345/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/crc16.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/crc16.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/ecan.o: ../../app/ecan.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/ecan.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/ecan.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/ecan.c  -o ${OBJECTDIR}/_ext/1445257345/ecan.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/ecan.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/ecan.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/qep.o: ../../app/qep.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/qep.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/qep.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/qep.c  -o ${OBJECTDIR}/_ext/1445257345/qep.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/qep.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/qep.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/rotor_alignment.o: ../../app/rotor_alignment.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/rotor_alignment.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/rotor_alignment.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/rotor_alignment.c  -o ${OBJECTDIR}/_ext/1445257345/rotor_alignment.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/rotor_alignment.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/rotor_alignment.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/tle5012.o: ../../app/tle5012.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/tle5012.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/tle5012.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/tle5012.c  -o ${OBJECTDIR}/_ext/1445257345/tle5012.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/tle5012.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/tle5012.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/traps.o: ../../app/traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/traps.c  -o ${OBJECTDIR}/_ext/1445257345/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/traps.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/velocity.o: ../../app/velocity.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/velocity.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/velocity.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/velocity.c  -o ${OBJECTDIR}/_ext/1445257345/velocity.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/velocity.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/velocity.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1722765132/can_icubProto.o: ../../app-added/can_icubProto.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1722765132 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto.o.d 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app-added/can_icubProto.c  -o ${OBJECTDIR}/_ext/1722765132/can_icubProto.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1722765132/can_icubProto.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1722765132/can_icubProto.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o: ../../app-added/can_icubProto_parser.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1722765132 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o.d 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app-added/can_icubProto_parser.c  -o ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o: ../../app-added/can_icubProto_trasmitter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1722765132 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app-added/can_icubProto_trasmitter.c  -o ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1000681919/eEbasicStorage.o: ../../../../../../libs/services/storage/basic/src/eEbasicStorage.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1000681919 
	@${RM} ${OBJECTDIR}/_ext/1000681919/eEbasicStorage.o.d 
	@${RM} ${OBJECTDIR}/_ext/1000681919/eEbasicStorage.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/services/storage/basic/src/eEbasicStorage.c  -o ${OBJECTDIR}/_ext/1000681919/eEbasicStorage.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1000681919/eEbasicStorage.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1000681919/eEbasicStorage.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1445257345/2FOC.o: ../../app/2FOC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/2FOC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/2FOC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/2FOC.c  -o ${OBJECTDIR}/_ext/1445257345/2FOC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/2FOC.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/2FOC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/ADC.o: ../../app/ADC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/ADC.c  -o ${OBJECTDIR}/_ext/1445257345/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/ADC.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/ADC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/Controller_edl_pid.o: ../../app/Controller_edl_pid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Controller_edl_pid.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Controller_edl_pid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/Controller_edl_pid.c  -o ${OBJECTDIR}/_ext/1445257345/Controller_edl_pid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/Controller_edl_pid.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/Controller_edl_pid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/Controller_mc_pi.o: ../../app/Controller_mc_pi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Controller_mc_pi.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Controller_mc_pi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/Controller_mc_pi.c  -o ${OBJECTDIR}/_ext/1445257345/Controller_mc_pi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/Controller_mc_pi.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/Controller_mc_pi.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/Controller_mc_pid.o: ../../app/Controller_mc_pid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Controller_mc_pid.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Controller_mc_pid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/Controller_mc_pid.c  -o ${OBJECTDIR}/_ext/1445257345/Controller_mc_pid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/Controller_mc_pid.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/Controller_mc_pid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/DCLink.o: ../../app/DCLink.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/DCLink.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/DCLink.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/DCLink.c  -o ${OBJECTDIR}/_ext/1445257345/DCLink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/DCLink.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/DCLink.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/DHES.o: ../../app/DHES.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/DHES.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/DHES.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/DHES.c  -o ${OBJECTDIR}/_ext/1445257345/DHES.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/DHES.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/DHES.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/Encoder.o: ../../app/Encoder.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Encoder.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Encoder.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/Encoder.c  -o ${OBJECTDIR}/_ext/1445257345/Encoder.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/Encoder.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/Encoder.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/Faults.o: ../../app/Faults.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Faults.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/Faults.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/Faults.c  -o ${OBJECTDIR}/_ext/1445257345/Faults.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/Faults.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/Faults.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/PWM.o: ../../app/PWM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/PWM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/PWM.c  -o ${OBJECTDIR}/_ext/1445257345/PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/PWM.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/PWM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/System.o: ../../app/System.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/System.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/System.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/System.c  -o ${OBJECTDIR}/_ext/1445257345/System.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/System.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/System.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/as5045.o: ../../app/as5045.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/as5045.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/as5045.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/as5045.c  -o ${OBJECTDIR}/_ext/1445257345/as5045.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/as5045.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/as5045.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/crc16.o: ../../app/crc16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/crc16.c  -o ${OBJECTDIR}/_ext/1445257345/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/crc16.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/crc16.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/ecan.o: ../../app/ecan.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/ecan.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/ecan.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/ecan.c  -o ${OBJECTDIR}/_ext/1445257345/ecan.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/ecan.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/ecan.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/qep.o: ../../app/qep.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/qep.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/qep.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/qep.c  -o ${OBJECTDIR}/_ext/1445257345/qep.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/qep.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/qep.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/rotor_alignment.o: ../../app/rotor_alignment.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/rotor_alignment.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/rotor_alignment.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/rotor_alignment.c  -o ${OBJECTDIR}/_ext/1445257345/rotor_alignment.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/rotor_alignment.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/rotor_alignment.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/tle5012.o: ../../app/tle5012.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/tle5012.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/tle5012.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/tle5012.c  -o ${OBJECTDIR}/_ext/1445257345/tle5012.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/tle5012.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/tle5012.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/traps.o: ../../app/traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/traps.c  -o ${OBJECTDIR}/_ext/1445257345/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/traps.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445257345/velocity.o: ../../app/velocity.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/velocity.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/velocity.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app/velocity.c  -o ${OBJECTDIR}/_ext/1445257345/velocity.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445257345/velocity.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/velocity.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1722765132/can_icubProto.o: ../../app-added/can_icubProto.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1722765132 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto.o.d 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app-added/can_icubProto.c  -o ${OBJECTDIR}/_ext/1722765132/can_icubProto.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1722765132/can_icubProto.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1722765132/can_icubProto.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o: ../../app-added/can_icubProto_parser.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1722765132 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o.d 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app-added/can_icubProto_parser.c  -o ${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1722765132/can_icubProto_parser.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o: ../../app-added/can_icubProto_trasmitter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1722765132 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../app-added/can_icubProto_trasmitter.c  -o ${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1722765132/can_icubProto_trasmitter.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1000681919/eEbasicStorage.o: ../../../../../../libs/services/storage/basic/src/eEbasicStorage.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1000681919 
	@${RM} ${OBJECTDIR}/_ext/1000681919/eEbasicStorage.o.d 
	@${RM} ${OBJECTDIR}/_ext/1000681919/eEbasicStorage.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../../libs/services/storage/basic/src/eEbasicStorage.c  -o ${OBJECTDIR}/_ext/1000681919/eEbasicStorage.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1000681919/eEbasicStorage.o.d"      -g -omf=elf -mlarge-data -O3 -I"../../app" -I"../../app-added" -I"../../envcom" -I"../../../../../body/embenv/envcom" -I"../../../../../body/embenv/envshalib" -I".." -I"../../../../../../../../../canProtocolLib" -I"../../../../../../libs/services/storage/basic/api" -I"../../../../../any/env/cfg" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1000681919/eEbasicStorage.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1445257345/CalcRef.o: ../../app/CalcRef.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/CalcRef.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/CalcRef.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/CalcRef.s  -o ${OBJECTDIR}/_ext/1445257345/CalcRef.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/CalcRef.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/CalcRef.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/I2T.o: ../../app/I2T.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/I2T.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/I2T.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/I2T.s  -o ${OBJECTDIR}/_ext/1445257345/I2T.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/I2T.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/I2T.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/InvPark.o: ../../app/InvPark.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/InvPark.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/InvPark.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/InvPark.s  -o ${OBJECTDIR}/_ext/1445257345/InvPark.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/InvPark.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/InvPark.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/MeasCurr.o: ../../app/MeasCurr.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/MeasCurr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/MeasCurr.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/MeasCurr.s  -o ${OBJECTDIR}/_ext/1445257345/MeasCurr.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/MeasCurr.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/MeasCurr.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/SVGen.o: ../../app/SVGen.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/SVGen.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/SVGen.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/SVGen.s  -o ${OBJECTDIR}/_ext/1445257345/SVGen.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/SVGen.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/SVGen.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/clrkpark.o: ../../app/clrkpark.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/clrkpark.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/clrkpark.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/clrkpark.s  -o ${OBJECTDIR}/_ext/1445257345/clrkpark.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/clrkpark.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/clrkpark.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/pi.o: ../../app/pi.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/pi.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/pi.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/pi.s  -o ${OBJECTDIR}/_ext/1445257345/pi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/pi.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/pi.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/pid.o: ../../app/pid.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/pid.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/pid.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/pid.s  -o ${OBJECTDIR}/_ext/1445257345/pid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/pid.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/pid.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/pid2.o: ../../app/pid2.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/pid2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/pid2.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/pid2.s  -o ${OBJECTDIR}/_ext/1445257345/pid2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/pid2.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/pid2.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/trig.o: ../../app/trig.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/trig.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/trig.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/trig.s  -o ${OBJECTDIR}/_ext/1445257345/trig.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/trig.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/trig.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1445257345/CalcRef.o: ../../app/CalcRef.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/CalcRef.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/CalcRef.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/CalcRef.s  -o ${OBJECTDIR}/_ext/1445257345/CalcRef.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/CalcRef.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/CalcRef.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/I2T.o: ../../app/I2T.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/I2T.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/I2T.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/I2T.s  -o ${OBJECTDIR}/_ext/1445257345/I2T.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/I2T.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/I2T.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/InvPark.o: ../../app/InvPark.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/InvPark.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/InvPark.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/InvPark.s  -o ${OBJECTDIR}/_ext/1445257345/InvPark.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/InvPark.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/InvPark.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/MeasCurr.o: ../../app/MeasCurr.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/MeasCurr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/MeasCurr.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/MeasCurr.s  -o ${OBJECTDIR}/_ext/1445257345/MeasCurr.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/MeasCurr.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/MeasCurr.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/SVGen.o: ../../app/SVGen.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/SVGen.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/SVGen.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/SVGen.s  -o ${OBJECTDIR}/_ext/1445257345/SVGen.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/SVGen.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/SVGen.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/clrkpark.o: ../../app/clrkpark.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/clrkpark.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/clrkpark.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/clrkpark.s  -o ${OBJECTDIR}/_ext/1445257345/clrkpark.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/clrkpark.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/clrkpark.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/pi.o: ../../app/pi.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/pi.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/pi.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/pi.s  -o ${OBJECTDIR}/_ext/1445257345/pi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/pi.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/pi.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/pid.o: ../../app/pid.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/pid.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/pid.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/pid.s  -o ${OBJECTDIR}/_ext/1445257345/pid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/pid.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/pid.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/pid2.o: ../../app/pid2.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/pid2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/pid2.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/pid2.s  -o ${OBJECTDIR}/_ext/1445257345/pid2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/pid2.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/pid2.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1445257345/trig.o: ../../app/trig.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445257345 
	@${RM} ${OBJECTDIR}/_ext/1445257345/trig.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445257345/trig.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../app/trig.s  -o ${OBJECTDIR}/_ext/1445257345/trig.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -I"../../app" -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1445257345/trig.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445257345/trig.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/2FOC.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../../../../../../libs/midware/microchip/libdsp-elf.a ../../../../../../libs/midware/microchip/libq-elf.a  ../eElinkerscript_appl_2FOC.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/2FOC.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}    ..\..\..\..\..\..\libs\midware\microchip\libdsp-elf.a ..\..\..\..\..\..\libs\midware\microchip\libq-elf.a  -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf  -mreserve=data@0x800:0x81F -mreserve=data@0x820:0x821 -mreserve=data@0x822:0x823 -mreserve=data@0x824:0x825 -mreserve=data@0x826:0x84F   -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library-path="C:/Program Files/Microchip/MPLAB C30/lib",--library-path="..",--no-force-link,--smart-io,-Map="${DISTDIR}/2FOC.X.${IMAGE_TYPE}.map",--report-mem,--warn-section-align$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/2FOC.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../../../../../../libs/midware/microchip/libdsp-elf.a ../../../../../../libs/midware/microchip/libq-elf.a ../eElinkerscript_appl_2FOC.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/2FOC.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}    ..\..\..\..\..\..\libs\midware\microchip\libdsp-elf.a ..\..\..\..\..\..\libs\midware\microchip\libq-elf.a  -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -Wl,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library-path="C:/Program Files/Microchip/MPLAB C30/lib",--library-path="..",--no-force-link,--smart-io,-Map="${DISTDIR}/2FOC.X.${IMAGE_TYPE}.map",--report-mem,--warn-section-align$(MP_EXTRA_LD_POST) 
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

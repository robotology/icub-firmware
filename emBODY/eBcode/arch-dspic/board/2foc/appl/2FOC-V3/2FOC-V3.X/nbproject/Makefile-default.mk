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
FINAL_IMAGE=${DISTDIR}/2FOC-V3.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/2FOC-V3.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../src/2FOC.c ../src/ADC.c ../src/DCLink.c ../src/DHES.c ../src/Faults.c ../src/PWM.c ../src/System.c ../src/can_icubProto.c ../src/can_icubProto_parser.c ../src/can_icubProto_trasmitter.c ../src/crc16.c ../src/ecan.c ../src/i2cTsens.c ../src/qep.c ../src/traps.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1360937237/2FOC.o ${OBJECTDIR}/_ext/1360937237/ADC.o ${OBJECTDIR}/_ext/1360937237/DCLink.o ${OBJECTDIR}/_ext/1360937237/DHES.o ${OBJECTDIR}/_ext/1360937237/Faults.o ${OBJECTDIR}/_ext/1360937237/PWM.o ${OBJECTDIR}/_ext/1360937237/System.o ${OBJECTDIR}/_ext/1360937237/can_icubProto.o ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o ${OBJECTDIR}/_ext/1360937237/crc16.o ${OBJECTDIR}/_ext/1360937237/ecan.o ${OBJECTDIR}/_ext/1360937237/i2cTsens.o ${OBJECTDIR}/_ext/1360937237/qep.o ${OBJECTDIR}/_ext/1360937237/traps.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1360937237/2FOC.o.d ${OBJECTDIR}/_ext/1360937237/ADC.o.d ${OBJECTDIR}/_ext/1360937237/DCLink.o.d ${OBJECTDIR}/_ext/1360937237/DHES.o.d ${OBJECTDIR}/_ext/1360937237/Faults.o.d ${OBJECTDIR}/_ext/1360937237/PWM.o.d ${OBJECTDIR}/_ext/1360937237/System.o.d ${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d ${OBJECTDIR}/_ext/1360937237/crc16.o.d ${OBJECTDIR}/_ext/1360937237/ecan.o.d ${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d ${OBJECTDIR}/_ext/1360937237/qep.o.d ${OBJECTDIR}/_ext/1360937237/traps.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1360937237/2FOC.o ${OBJECTDIR}/_ext/1360937237/ADC.o ${OBJECTDIR}/_ext/1360937237/DCLink.o ${OBJECTDIR}/_ext/1360937237/DHES.o ${OBJECTDIR}/_ext/1360937237/Faults.o ${OBJECTDIR}/_ext/1360937237/PWM.o ${OBJECTDIR}/_ext/1360937237/System.o ${OBJECTDIR}/_ext/1360937237/can_icubProto.o ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o ${OBJECTDIR}/_ext/1360937237/crc16.o ${OBJECTDIR}/_ext/1360937237/ecan.o ${OBJECTDIR}/_ext/1360937237/i2cTsens.o ${OBJECTDIR}/_ext/1360937237/qep.o ${OBJECTDIR}/_ext/1360937237/traps.o

# Source Files
SOURCEFILES=../src/2FOC.c ../src/ADC.c ../src/DCLink.c ../src/DHES.c ../src/Faults.c ../src/PWM.c ../src/System.c ../src/can_icubProto.c ../src/can_icubProto_parser.c ../src/can_icubProto_trasmitter.c ../src/crc16.c ../src/ecan.c ../src/i2cTsens.c ../src/qep.c ../src/traps.c



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
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/2FOC-V3.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ128MC802
MP_LINKER_FILE_OPTION=,--script="..\eElinkerscript_appl_2FOC.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1360937237/2FOC.o: ../src/2FOC.c  .generated_files/flags/default/92ee7cef1d731c255d021ffa5cc7fa63e572e73 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/2FOC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/2FOC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/2FOC.c  -o ${OBJECTDIR}/_ext/1360937237/2FOC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/2FOC.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/ADC.o: ../src/ADC.c  .generated_files/flags/default/4a783ba633f7b1464f26e984efa97a6ca543d4f1 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ADC.c  -o ${OBJECTDIR}/_ext/1360937237/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/ADC.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/DCLink.o: ../src/DCLink.c  .generated_files/flags/default/f2e86ba9b1f88d57e95fae2cf52c5deea230fe03 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DCLink.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DCLink.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DCLink.c  -o ${OBJECTDIR}/_ext/1360937237/DCLink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/DCLink.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/DHES.o: ../src/DHES.c  .generated_files/flags/default/33c7cce92e6aaf336793e236789ef81e6debfdb6 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DHES.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DHES.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DHES.c  -o ${OBJECTDIR}/_ext/1360937237/DHES.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/DHES.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/Faults.o: ../src/Faults.c  .generated_files/flags/default/76179e583848622db36fcf5349e1b229146db59c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Faults.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Faults.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Faults.c  -o ${OBJECTDIR}/_ext/1360937237/Faults.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/Faults.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/PWM.o: ../src/PWM.c  .generated_files/flags/default/40e239b657dc2be0a6560455fdfabc33f9c4eb16 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PWM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/PWM.c  -o ${OBJECTDIR}/_ext/1360937237/PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PWM.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/System.o: ../src/System.c  .generated_files/flags/default/6c6a6753a18a6eccf0272f9906ce4bf8f978d94b .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/System.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/System.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/System.c  -o ${OBJECTDIR}/_ext/1360937237/System.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/System.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto.o: ../src/can_icubProto.c  .generated_files/flags/default/8f1987451a0982927e643e58d8fffe9d923754cc .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o: ../src/can_icubProto_parser.c  .generated_files/flags/default/6a9d1b250e942485db3f97dd5b122b3aa915b626 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto_parser.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o: ../src/can_icubProto_trasmitter.c  .generated_files/flags/default/c27547fb134f054137ac1dc7924ed04a4945669f .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto_trasmitter.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/crc16.o: ../src/crc16.c  .generated_files/flags/default/7b15620a95c17929924d13dadc734b58c8bfbf34 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/crc16.c  -o ${OBJECTDIR}/_ext/1360937237/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/crc16.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/ecan.o: ../src/ecan.c  .generated_files/flags/default/4d5aae57a6070d1b4c2510417182b4e1fae52cb6 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ecan.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ecan.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ecan.c  -o ${OBJECTDIR}/_ext/1360937237/ecan.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/ecan.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/i2cTsens.o: ../src/i2cTsens.c  .generated_files/flags/default/e0af33be0ad8ef78cf13e161b4e4159c3c0696ca .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/i2cTsens.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/i2cTsens.c  -o ${OBJECTDIR}/_ext/1360937237/i2cTsens.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/qep.o: ../src/qep.c  .generated_files/flags/default/225cd9dec1213cdba5bbd9f95f90074ee98da5b8 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/qep.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/qep.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/qep.c  -o ${OBJECTDIR}/_ext/1360937237/qep.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/qep.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/traps.o: ../src/traps.c  .generated_files/flags/default/ea2fdbdc5d73307aa2dff5f789ad047c1d42eb87 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/traps.c  -o ${OBJECTDIR}/_ext/1360937237/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/traps.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
else
${OBJECTDIR}/_ext/1360937237/2FOC.o: ../src/2FOC.c  .generated_files/flags/default/30002eddb69b2d6348b9a4efc81626d5a8422336 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/2FOC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/2FOC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/2FOC.c  -o ${OBJECTDIR}/_ext/1360937237/2FOC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/2FOC.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/ADC.o: ../src/ADC.c  .generated_files/flags/default/6479c30523b682ba47e70b8f223b0820b8fab0c0 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ADC.c  -o ${OBJECTDIR}/_ext/1360937237/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/ADC.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/DCLink.o: ../src/DCLink.c  .generated_files/flags/default/2db38b44c1f4ea2981d658ecf5354452fd31275a .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DCLink.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DCLink.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DCLink.c  -o ${OBJECTDIR}/_ext/1360937237/DCLink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/DCLink.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/DHES.o: ../src/DHES.c  .generated_files/flags/default/8bf6e4bd492546d33f97b0ec98caf62f226d81f6 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DHES.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DHES.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DHES.c  -o ${OBJECTDIR}/_ext/1360937237/DHES.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/DHES.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/Faults.o: ../src/Faults.c  .generated_files/flags/default/709d2f6c01c0edaa636939c76668fd3c01610265 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Faults.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Faults.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Faults.c  -o ${OBJECTDIR}/_ext/1360937237/Faults.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/Faults.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/PWM.o: ../src/PWM.c  .generated_files/flags/default/bc01ad82b477193543812137490acf2bbbafbc7 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PWM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/PWM.c  -o ${OBJECTDIR}/_ext/1360937237/PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PWM.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/System.o: ../src/System.c  .generated_files/flags/default/68d91e89705b96b7d602067b87e725620c92e21a .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/System.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/System.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/System.c  -o ${OBJECTDIR}/_ext/1360937237/System.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/System.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto.o: ../src/can_icubProto.c  .generated_files/flags/default/b68b0e894f5e322f876ea21e7a8bf58d71be21dc .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o: ../src/can_icubProto_parser.c  .generated_files/flags/default/f38387dbb4dbe82b03675a743cf272aae43e6e7c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto_parser.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o: ../src/can_icubProto_trasmitter.c  .generated_files/flags/default/59d58461c401a978523617860d9c4dd276004215 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto_trasmitter.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/crc16.o: ../src/crc16.c  .generated_files/flags/default/7b071a0c6f1c0719f6c5224d8e4f955698496fbf .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/crc16.c  -o ${OBJECTDIR}/_ext/1360937237/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/crc16.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/ecan.o: ../src/ecan.c  .generated_files/flags/default/cb4b452861d0e0f5616170df86b3c15c2cb20d62 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ecan.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ecan.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ecan.c  -o ${OBJECTDIR}/_ext/1360937237/ecan.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/ecan.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/i2cTsens.o: ../src/i2cTsens.c  .generated_files/flags/default/94c00fbb0c414e62f00dbc4dc6e818dadfa427a8 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/i2cTsens.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/i2cTsens.c  -o ${OBJECTDIR}/_ext/1360937237/i2cTsens.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/qep.o: ../src/qep.c  .generated_files/flags/default/11f185d9fbdf5f68fcd1ae1ad62351cb6e87b581 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/qep.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/qep.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/qep.c  -o ${OBJECTDIR}/_ext/1360937237/qep.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/qep.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/traps.o: ../src/traps.c  .generated_files/flags/default/53375be728a69072aab5a4da2bc215b62f11b688 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/traps.c  -o ${OBJECTDIR}/_ext/1360937237/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/traps.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
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
${DISTDIR}/2FOC-V3.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ../eElinkerscript_appl_2FOC.gld
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o ${DISTDIR}/2FOC-V3.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG=__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)      -Wl,,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)  
	
else
${DISTDIR}/2FOC-V3.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ../eElinkerscript_appl_2FOC.gld
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o ${DISTDIR}/2FOC-V3.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -Wl,,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)  
	${MP_CC_DIR}\\xc16-bin2hex ${DISTDIR}/2FOC-V3.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf   
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(wildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif

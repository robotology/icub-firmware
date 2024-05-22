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
SOURCEFILES_QUOTED_IF_SPACED=../asm/MeasCurr.s ../src/2FOC.c ../src/ADC.c ../src/DCLink.c ../src/DHES.c ../src/Faults.c ../src/PWM.c ../src/System.c ../src/can_icubProto.c ../src/can_icubProto_parser.c ../src/can_icubProto_trasmitter.c ../src/crc16.c ../src/ecan.c ../src/i2cTsens.c ../src/qep.c ../src/traps.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1360919980/MeasCurr.o ${OBJECTDIR}/_ext/1360937237/2FOC.o ${OBJECTDIR}/_ext/1360937237/ADC.o ${OBJECTDIR}/_ext/1360937237/DCLink.o ${OBJECTDIR}/_ext/1360937237/DHES.o ${OBJECTDIR}/_ext/1360937237/Faults.o ${OBJECTDIR}/_ext/1360937237/PWM.o ${OBJECTDIR}/_ext/1360937237/System.o ${OBJECTDIR}/_ext/1360937237/can_icubProto.o ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o ${OBJECTDIR}/_ext/1360937237/crc16.o ${OBJECTDIR}/_ext/1360937237/ecan.o ${OBJECTDIR}/_ext/1360937237/i2cTsens.o ${OBJECTDIR}/_ext/1360937237/qep.o ${OBJECTDIR}/_ext/1360937237/traps.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1360919980/MeasCurr.o.d ${OBJECTDIR}/_ext/1360937237/2FOC.o.d ${OBJECTDIR}/_ext/1360937237/ADC.o.d ${OBJECTDIR}/_ext/1360937237/DCLink.o.d ${OBJECTDIR}/_ext/1360937237/DHES.o.d ${OBJECTDIR}/_ext/1360937237/Faults.o.d ${OBJECTDIR}/_ext/1360937237/PWM.o.d ${OBJECTDIR}/_ext/1360937237/System.o.d ${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d ${OBJECTDIR}/_ext/1360937237/crc16.o.d ${OBJECTDIR}/_ext/1360937237/ecan.o.d ${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d ${OBJECTDIR}/_ext/1360937237/qep.o.d ${OBJECTDIR}/_ext/1360937237/traps.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1360919980/MeasCurr.o ${OBJECTDIR}/_ext/1360937237/2FOC.o ${OBJECTDIR}/_ext/1360937237/ADC.o ${OBJECTDIR}/_ext/1360937237/DCLink.o ${OBJECTDIR}/_ext/1360937237/DHES.o ${OBJECTDIR}/_ext/1360937237/Faults.o ${OBJECTDIR}/_ext/1360937237/PWM.o ${OBJECTDIR}/_ext/1360937237/System.o ${OBJECTDIR}/_ext/1360937237/can_icubProto.o ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o ${OBJECTDIR}/_ext/1360937237/crc16.o ${OBJECTDIR}/_ext/1360937237/ecan.o ${OBJECTDIR}/_ext/1360937237/i2cTsens.o ${OBJECTDIR}/_ext/1360937237/qep.o ${OBJECTDIR}/_ext/1360937237/traps.o

# Source Files
SOURCEFILES=../asm/MeasCurr.s ../src/2FOC.c ../src/ADC.c ../src/DCLink.c ../src/DHES.c ../src/Faults.c ../src/PWM.c ../src/System.c ../src/can_icubProto.c ../src/can_icubProto_parser.c ../src/can_icubProto_trasmitter.c ../src/crc16.c ../src/ecan.c ../src/i2cTsens.c ../src/qep.c ../src/traps.c



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
${OBJECTDIR}/_ext/1360937237/2FOC.o: ../src/2FOC.c  .generated_files/flags/default/55343147a05fc3e68541b04ba27272590dc08973 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/2FOC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/2FOC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/2FOC.c  -o ${OBJECTDIR}/_ext/1360937237/2FOC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/2FOC.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/ADC.o: ../src/ADC.c  .generated_files/flags/default/3f29b49fc2e9e3677fc22765b4c32dabf3429ea3 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ADC.c  -o ${OBJECTDIR}/_ext/1360937237/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/ADC.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/DCLink.o: ../src/DCLink.c  .generated_files/flags/default/957f21def998040c52443d0bb8416fc9727edde4 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DCLink.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DCLink.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DCLink.c  -o ${OBJECTDIR}/_ext/1360937237/DCLink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/DCLink.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/DHES.o: ../src/DHES.c  .generated_files/flags/default/1401f18a939e5a134f027c2c01c1f2ed12189dd .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DHES.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DHES.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DHES.c  -o ${OBJECTDIR}/_ext/1360937237/DHES.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/DHES.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/Faults.o: ../src/Faults.c  .generated_files/flags/default/b6533c12487709049c1546962f5cca13ba478785 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Faults.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Faults.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Faults.c  -o ${OBJECTDIR}/_ext/1360937237/Faults.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/Faults.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/PWM.o: ../src/PWM.c  .generated_files/flags/default/4b5d71514dec1baf40d797c532dab34650fd4a03 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PWM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/PWM.c  -o ${OBJECTDIR}/_ext/1360937237/PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PWM.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/System.o: ../src/System.c  .generated_files/flags/default/b5db24d03a826f573d257e022955c9719e6de126 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/System.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/System.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/System.c  -o ${OBJECTDIR}/_ext/1360937237/System.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/System.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto.o: ../src/can_icubProto.c  .generated_files/flags/default/eea8ec7012ad80206d7ebf9b05dccd809e47647f .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o: ../src/can_icubProto_parser.c  .generated_files/flags/default/707c3d4426b194f29b21791e1e50304e582d7d02 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto_parser.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o: ../src/can_icubProto_trasmitter.c  .generated_files/flags/default/eb21d112e7fa3e3d2500ef20f26e21b32c1fe6c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto_trasmitter.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/crc16.o: ../src/crc16.c  .generated_files/flags/default/d17e4b704c6f760133eaa925cb413e8127b25cf3 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/crc16.c  -o ${OBJECTDIR}/_ext/1360937237/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/crc16.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/ecan.o: ../src/ecan.c  .generated_files/flags/default/931286501ddb70ce7e77b818482ab673eb931d7c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ecan.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ecan.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ecan.c  -o ${OBJECTDIR}/_ext/1360937237/ecan.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/ecan.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/i2cTsens.o: ../src/i2cTsens.c  .generated_files/flags/default/4c8a914f349d89246f4f14d24f948b3462119497 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/i2cTsens.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/i2cTsens.c  -o ${OBJECTDIR}/_ext/1360937237/i2cTsens.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/qep.o: ../src/qep.c  .generated_files/flags/default/1a7a8177293b617a365fb0411997d3597b14fe9e .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/qep.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/qep.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/qep.c  -o ${OBJECTDIR}/_ext/1360937237/qep.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/qep.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/traps.o: ../src/traps.c  .generated_files/flags/default/6a5dcdb11f1fe1901f291340fc72934c5b5be220 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/traps.c  -o ${OBJECTDIR}/_ext/1360937237/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/traps.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
else
${OBJECTDIR}/_ext/1360937237/2FOC.o: ../src/2FOC.c  .generated_files/flags/default/d091e4bbbdafaf7de312d0e55560c1e1e7428083 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/2FOC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/2FOC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/2FOC.c  -o ${OBJECTDIR}/_ext/1360937237/2FOC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/2FOC.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/ADC.o: ../src/ADC.c  .generated_files/flags/default/579d3e2051cfc516b61269be750c94233bae4f29 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ADC.c  -o ${OBJECTDIR}/_ext/1360937237/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/ADC.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/DCLink.o: ../src/DCLink.c  .generated_files/flags/default/b88950229be00a5a03e672462c0b55f14840f1b1 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DCLink.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DCLink.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DCLink.c  -o ${OBJECTDIR}/_ext/1360937237/DCLink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/DCLink.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/DHES.o: ../src/DHES.c  .generated_files/flags/default/156fa51ebe6b74a571ad182024c46f7e37e7e9d9 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DHES.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DHES.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DHES.c  -o ${OBJECTDIR}/_ext/1360937237/DHES.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/DHES.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/Faults.o: ../src/Faults.c  .generated_files/flags/default/8d87260331eec1592ff2f923db6ceddd9c682aaf .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Faults.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Faults.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Faults.c  -o ${OBJECTDIR}/_ext/1360937237/Faults.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/Faults.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/PWM.o: ../src/PWM.c  .generated_files/flags/default/3c6c9f96fda9397b66ca1119fac9863427af9bca .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PWM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/PWM.c  -o ${OBJECTDIR}/_ext/1360937237/PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PWM.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/System.o: ../src/System.c  .generated_files/flags/default/808ae670233d2ae97490efe5d8463f023a68892b .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/System.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/System.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/System.c  -o ${OBJECTDIR}/_ext/1360937237/System.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/System.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto.o: ../src/can_icubProto.c  .generated_files/flags/default/9c8636d4c2bc77f94450f7585d87839349578ceb .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o: ../src/can_icubProto_parser.c  .generated_files/flags/default/c025f6f8add47ddc7f2c1a57d60c5b9a8bf80f6d .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto_parser.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o: ../src/can_icubProto_trasmitter.c  .generated_files/flags/default/293e1e3980dab36fb9a7192ff592280e08a2adc7 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto_trasmitter.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/crc16.o: ../src/crc16.c  .generated_files/flags/default/3bebf0e6fc6d59a8a62aa1dd60a3b196112dcac2 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/crc16.c  -o ${OBJECTDIR}/_ext/1360937237/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/crc16.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/ecan.o: ../src/ecan.c  .generated_files/flags/default/91f3c01134c16f631b7b30b47128fc426eaabfe3 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ecan.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ecan.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ecan.c  -o ${OBJECTDIR}/_ext/1360937237/ecan.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/ecan.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/i2cTsens.o: ../src/i2cTsens.c  .generated_files/flags/default/5ccdc3a1dc00faa86e8850399c5a551501c614e8 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/i2cTsens.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/i2cTsens.c  -o ${OBJECTDIR}/_ext/1360937237/i2cTsens.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/qep.o: ../src/qep.c  .generated_files/flags/default/47c4c3550443eb8d1d28b22b299f7480c8661041 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/qep.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/qep.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/qep.c  -o ${OBJECTDIR}/_ext/1360937237/qep.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/qep.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/traps.o: ../src/traps.c  .generated_files/flags/default/f7e1d7801f1a15d3e00e2d7a52965c15d62e09ef .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/traps.c  -o ${OBJECTDIR}/_ext/1360937237/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/traps.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1360919980/MeasCurr.o: ../asm/MeasCurr.s  .generated_files/flags/default/389d0fcad4c0d98318cc6b76a3c9b3fef9e4cc61 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360919980" 
	@${RM} ${OBJECTDIR}/_ext/1360919980/MeasCurr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360919980/MeasCurr.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../asm/MeasCurr.s  -o ${OBJECTDIR}/_ext/1360919980/MeasCurr.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  -I"../asm" -Wa,-MD,"${OBJECTDIR}/_ext/1360919980/MeasCurr.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST) 
	
else
${OBJECTDIR}/_ext/1360919980/MeasCurr.o: ../asm/MeasCurr.s  .generated_files/flags/default/b123eff951699ebc3d5188afb234ae1abd865583 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360919980" 
	@${RM} ${OBJECTDIR}/_ext/1360919980/MeasCurr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360919980/MeasCurr.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../asm/MeasCurr.s  -o ${OBJECTDIR}/_ext/1360919980/MeasCurr.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  -I"../asm" -Wa,-MD,"${OBJECTDIR}/_ext/1360919980/MeasCurr.o.d",--defsym=__MPLAB_BUILD=1,--no-relax$(MP_EXTRA_AS_POST) 
	
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

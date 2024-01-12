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
${OBJECTDIR}/_ext/1360937237/2FOC.o: ../src/2FOC.c  .generated_files/flags/default/4c5f088f82522ed56cfeb40954c44bf70e4399b6 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/2FOC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/2FOC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/2FOC.c  -o ${OBJECTDIR}/_ext/1360937237/2FOC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/2FOC.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/ADC.o: ../src/ADC.c  .generated_files/flags/default/82aedc83ec004211dc4d552d24942f9e256fe199 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ADC.c  -o ${OBJECTDIR}/_ext/1360937237/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/ADC.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/DCLink.o: ../src/DCLink.c  .generated_files/flags/default/a041261bdf2e83d3776c6530f910aec9fc984acd .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DCLink.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DCLink.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DCLink.c  -o ${OBJECTDIR}/_ext/1360937237/DCLink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/DCLink.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/DHES.o: ../src/DHES.c  .generated_files/flags/default/cd6cfa57766bacec6b4d6020ccad056b2a8aca3f .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DHES.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DHES.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DHES.c  -o ${OBJECTDIR}/_ext/1360937237/DHES.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/DHES.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/Faults.o: ../src/Faults.c  .generated_files/flags/default/de6839c0650bdfb9e57924c499a1e0e0b17e8a4e .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Faults.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Faults.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Faults.c  -o ${OBJECTDIR}/_ext/1360937237/Faults.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/Faults.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/PWM.o: ../src/PWM.c  .generated_files/flags/default/475de3d3590cb79014246705f1496275c3c8bbf2 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PWM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/PWM.c  -o ${OBJECTDIR}/_ext/1360937237/PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PWM.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/System.o: ../src/System.c  .generated_files/flags/default/2c875c6ada38cc091c193298ee9f4bf04a3ef816 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/System.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/System.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/System.c  -o ${OBJECTDIR}/_ext/1360937237/System.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/System.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto.o: ../src/can_icubProto.c  .generated_files/flags/default/ad09c3f53056a968968b11f505cced993eb397b7 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o: ../src/can_icubProto_parser.c  .generated_files/flags/default/5ef7c8e8e2ff5c283cc3fdab03ecee6df0695579 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto_parser.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o: ../src/can_icubProto_trasmitter.c  .generated_files/flags/default/458a9e424382fbf0193d712ab14ccc56bb2b42ec .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto_trasmitter.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/crc16.o: ../src/crc16.c  .generated_files/flags/default/d1d5e7ba44e3398febb057281aa1221093fe7c0b .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/crc16.c  -o ${OBJECTDIR}/_ext/1360937237/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/crc16.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/ecan.o: ../src/ecan.c  .generated_files/flags/default/cd31fd10cf2f40ca3453ee71a78dbedfa4fc6227 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ecan.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ecan.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ecan.c  -o ${OBJECTDIR}/_ext/1360937237/ecan.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/ecan.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/i2cTsens.o: ../src/i2cTsens.c  .generated_files/flags/default/5ee4294858cb2365db69c4577222773d6f74031f .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/i2cTsens.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/i2cTsens.c  -o ${OBJECTDIR}/_ext/1360937237/i2cTsens.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/qep.o: ../src/qep.c  .generated_files/flags/default/f519d7e645cdaedfc3871f1ec36f6b463a4358b6 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/qep.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/qep.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/qep.c  -o ${OBJECTDIR}/_ext/1360937237/qep.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/qep.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/traps.o: ../src/traps.c  .generated_files/flags/default/a298e6870e51bf2443633ee060abcfb1048741b6 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/traps.c  -o ${OBJECTDIR}/_ext/1360937237/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/traps.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
else
${OBJECTDIR}/_ext/1360937237/2FOC.o: ../src/2FOC.c  .generated_files/flags/default/6ff66ca5ea4ac33fce28c3b27bdf134827cb4df7 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/2FOC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/2FOC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/2FOC.c  -o ${OBJECTDIR}/_ext/1360937237/2FOC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/2FOC.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/ADC.o: ../src/ADC.c  .generated_files/flags/default/5e725d84836c1394d06f1a38108278209e2b6098 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ADC.c  -o ${OBJECTDIR}/_ext/1360937237/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/ADC.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/DCLink.o: ../src/DCLink.c  .generated_files/flags/default/804a96fe70b54705065305cc5e61d5fc909354f6 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DCLink.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DCLink.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DCLink.c  -o ${OBJECTDIR}/_ext/1360937237/DCLink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/DCLink.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/DHES.o: ../src/DHES.c  .generated_files/flags/default/3f7334474d5df82cb80dc2d3dae0cff2035d3210 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DHES.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DHES.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DHES.c  -o ${OBJECTDIR}/_ext/1360937237/DHES.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/DHES.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/Faults.o: ../src/Faults.c  .generated_files/flags/default/b8052889cd73eec532e1081c8aabee52dee8c546 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Faults.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Faults.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Faults.c  -o ${OBJECTDIR}/_ext/1360937237/Faults.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/Faults.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/PWM.o: ../src/PWM.c  .generated_files/flags/default/681dc4ef6c9bf360420a7763b36ee08e9934901d .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PWM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/PWM.c  -o ${OBJECTDIR}/_ext/1360937237/PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PWM.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/System.o: ../src/System.c  .generated_files/flags/default/756ec946badc80a3eb7cd3f39964d2916e8d6c68 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/System.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/System.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/System.c  -o ${OBJECTDIR}/_ext/1360937237/System.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/System.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto.o: ../src/can_icubProto.c  .generated_files/flags/default/f5f45dd553806fb437b59cbe85c0b73a5e840105 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o: ../src/can_icubProto_parser.c  .generated_files/flags/default/1c642aedac1eb3eae201fa04fb1867c461c9b847 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto_parser.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o: ../src/can_icubProto_trasmitter.c  .generated_files/flags/default/790e52322d2a41949256b344475d5f1e398ba493 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto_trasmitter.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/crc16.o: ../src/crc16.c  .generated_files/flags/default/c3d5147f6640290c345d3b85ab80caf7f905ad59 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/crc16.c  -o ${OBJECTDIR}/_ext/1360937237/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/crc16.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/ecan.o: ../src/ecan.c  .generated_files/flags/default/4017ced2e1031d0384970a2a3b65e232f5bd34c6 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ecan.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ecan.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ecan.c  -o ${OBJECTDIR}/_ext/1360937237/ecan.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/ecan.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/i2cTsens.o: ../src/i2cTsens.c  .generated_files/flags/default/51d65708c71be1046cf3afabf887beddf99309c6 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/i2cTsens.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/i2cTsens.c  -o ${OBJECTDIR}/_ext/1360937237/i2cTsens.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/qep.o: ../src/qep.c  .generated_files/flags/default/e94877f05e665c12811865dcae539cab9840895f .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/qep.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/qep.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/qep.c  -o ${OBJECTDIR}/_ext/1360937237/qep.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/qep.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/_ext/1360937237/traps.o: ../src/traps.c  .generated_files/flags/default/7cd6ad894bc50c42f956dc0f773cb64074d63803 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/traps.c  -o ${OBJECTDIR}/_ext/1360937237/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/traps.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off   
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1360919980/MeasCurr.o: ../asm/MeasCurr.s  .generated_files/flags/default/d44269e96d9fc40ef944c4fa7d6ea5290ae8af71 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360919980" 
	@${RM} ${OBJECTDIR}/_ext/1360919980/MeasCurr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360919980/MeasCurr.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../asm/MeasCurr.s  -o ${OBJECTDIR}/_ext/1360919980/MeasCurr.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  -I"../asm" -Wa,-MD,"${OBJECTDIR}/_ext/1360919980/MeasCurr.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST) 
	
else
${OBJECTDIR}/_ext/1360919980/MeasCurr.o: ../asm/MeasCurr.s  .generated_files/flags/default/17898a9f5806572a5d834c868e007805c82372cf .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
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

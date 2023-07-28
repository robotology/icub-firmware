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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/2FOC-V3.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/2FOC-V3.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS
SUB_IMAGE_ADDRESS_COMMAND=--image-address $(SUB_IMAGE_ADDRESS)
else
SUB_IMAGE_ADDRESS_COMMAND=
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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/2FOC-V3.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ128MC802
MP_LINKER_FILE_OPTION=,--script="..\eElinkerscript_appl_2FOC.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1360937237/2FOC.o: ../src/2FOC.c  .generated_files/34b0d2d477b03ac0e28c13b7d0d8ac894da6a717.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/2FOC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/2FOC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/2FOC.c  -o ${OBJECTDIR}/_ext/1360937237/2FOC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/2FOC.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/ADC.o: ../src/ADC.c  .generated_files/a8aba52b5e26ee81c87531e3a731b1d6f6d72daf.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ADC.c  -o ${OBJECTDIR}/_ext/1360937237/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/ADC.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/DCLink.o: ../src/DCLink.c  .generated_files/62dd267561e63e738481ed2cf92692eba6f01dfb.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DCLink.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DCLink.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DCLink.c  -o ${OBJECTDIR}/_ext/1360937237/DCLink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/DCLink.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/DHES.o: ../src/DHES.c  .generated_files/499690926038325b9c8ce6d9c5c6fcf8f99214ef.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DHES.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DHES.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DHES.c  -o ${OBJECTDIR}/_ext/1360937237/DHES.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/DHES.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/Faults.o: ../src/Faults.c  .generated_files/dc5af4b9c28b135a199f36d127fc6cb3b8978a6.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Faults.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Faults.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Faults.c  -o ${OBJECTDIR}/_ext/1360937237/Faults.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/Faults.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/PWM.o: ../src/PWM.c  .generated_files/6c5cf5b9c97b96eb3522ef89bfc1041b5b2b02b3.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PWM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/PWM.c  -o ${OBJECTDIR}/_ext/1360937237/PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PWM.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/System.o: ../src/System.c  .generated_files/8a72132eda6596b7cd037c1f655af7179dd7903.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/System.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/System.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/System.c  -o ${OBJECTDIR}/_ext/1360937237/System.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/System.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/can_icubProto.o: ../src/can_icubProto.c  .generated_files/31003672734d17b3896bce1952a1a2b161981ceb.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o: ../src/can_icubProto_parser.c  .generated_files/66f437dbb29552991385530c35307bbeefb0edc7.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto_parser.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o: ../src/can_icubProto_trasmitter.c  .generated_files/1f2eec040215198410e98ab2e9052a41eb1914c3.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto_trasmitter.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/crc16.o: ../src/crc16.c  .generated_files/8ff284c917a9344f338e5249b1c9585834d75b7c.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/crc16.c  -o ${OBJECTDIR}/_ext/1360937237/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/crc16.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/ecan.o: ../src/ecan.c  .generated_files/a8bcd401a811e324da2d8ec3d691a57881a14201.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ecan.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ecan.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ecan.c  -o ${OBJECTDIR}/_ext/1360937237/ecan.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/ecan.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/i2cTsens.o: ../src/i2cTsens.c  .generated_files/7df312a55698650609529484828db957d0cdaa64.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/i2cTsens.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/i2cTsens.c  -o ${OBJECTDIR}/_ext/1360937237/i2cTsens.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/qep.o: ../src/qep.c  .generated_files/451711856e0c95c1a84c17c87ac2707a8e0c60f6.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/qep.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/qep.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/qep.c  -o ${OBJECTDIR}/_ext/1360937237/qep.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/qep.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/traps.o: ../src/traps.c  .generated_files/64e12592461576f0464877752c0750fb68c4df2b.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/traps.c  -o ${OBJECTDIR}/_ext/1360937237/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/traps.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
else
${OBJECTDIR}/_ext/1360937237/2FOC.o: ../src/2FOC.c  .generated_files/8add5ff3891baac61b3b094ba9d96f4f0e50b66.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/2FOC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/2FOC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/2FOC.c  -o ${OBJECTDIR}/_ext/1360937237/2FOC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/2FOC.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/ADC.o: ../src/ADC.c  .generated_files/876eb0b2d6a26f67d8eef29947f755de5e3e7b8d.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ADC.c  -o ${OBJECTDIR}/_ext/1360937237/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/ADC.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/DCLink.o: ../src/DCLink.c  .generated_files/37cccc14546caacfa2db0bd13a37bb07e0bde34e.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DCLink.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DCLink.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DCLink.c  -o ${OBJECTDIR}/_ext/1360937237/DCLink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/DCLink.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/DHES.o: ../src/DHES.c  .generated_files/fde2dc44b5957f81c234513e7ab66bf615a0035a.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DHES.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DHES.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DHES.c  -o ${OBJECTDIR}/_ext/1360937237/DHES.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/DHES.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/Faults.o: ../src/Faults.c  .generated_files/bc4f1519abdfbb168225f4ba1efd824c2cce42aa.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Faults.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Faults.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Faults.c  -o ${OBJECTDIR}/_ext/1360937237/Faults.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/Faults.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/PWM.o: ../src/PWM.c  .generated_files/a2528d46d0f9af5ad44b511e0567fe9d3c1c78e.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PWM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/PWM.c  -o ${OBJECTDIR}/_ext/1360937237/PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PWM.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/System.o: ../src/System.c  .generated_files/f0d95bd6f2a35ce0bf16ac2c414f6ea1d66aff59.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/System.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/System.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/System.c  -o ${OBJECTDIR}/_ext/1360937237/System.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/System.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/can_icubProto.o: ../src/can_icubProto.c  .generated_files/4d88dbe8bcfb99c3da9c1bee44a98cf22bb5cb49.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o: ../src/can_icubProto_parser.c  .generated_files/e38dce41241bdc61a2521d49a511fafc3c472696.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto_parser.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto_parser.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o: ../src/can_icubProto_trasmitter.c  .generated_files/bb5a4fb608863bb91c7f1d430ff711573a1ec54f.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/can_icubProto_trasmitter.c  -o ${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/can_icubProto_trasmitter.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/crc16.o: ../src/crc16.c  .generated_files/2bd6eadd396ef48d6040c43b017be0f5ce6d9297.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/crc16.c  -o ${OBJECTDIR}/_ext/1360937237/crc16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/crc16.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/ecan.o: ../src/ecan.c  .generated_files/cdf16b27b370e382432a0fd1c84407b6781cecd0.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ecan.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ecan.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ecan.c  -o ${OBJECTDIR}/_ext/1360937237/ecan.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/ecan.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/i2cTsens.o: ../src/i2cTsens.c  .generated_files/ef9d74c839b10c260bfd0ebc78dec5b34a046614.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/i2cTsens.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/i2cTsens.c  -o ${OBJECTDIR}/_ext/1360937237/i2cTsens.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/i2cTsens.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/qep.o: ../src/qep.c  .generated_files/8345d457e597574839bc5c6e4e3bfae975c23efc.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/qep.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/qep.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/qep.c  -o ${OBJECTDIR}/_ext/1360937237/qep.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/qep.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1360937237/traps.o: ../src/traps.c  .generated_files/8750d9a3aced6c9396f8b439cd8f912e4597ace5.flag .generated_files/6fa25bf04b30d9bb4edd8837a4a601db4f515c90.flag
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/traps.c  -o ${OBJECTDIR}/_ext/1360937237/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/traps.o.d"        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -ffunction-sections -msmall-data -O1 -I"../include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
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
dist/${CND_CONF}/${IMAGE_TYPE}/2FOC-V3.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ../eElinkerscript_appl_2FOC.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/2FOC-V3.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG=__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)      -Wl,,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/2FOC-V3.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ../eElinkerscript_appl_2FOC.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/2FOC-V3.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -Wl,,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/2FOC-V3.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf   -mdfp="${DFP_DIR}/xc16" 
	
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

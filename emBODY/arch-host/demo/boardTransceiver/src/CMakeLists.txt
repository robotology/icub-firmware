# Create a library for all embObj stuff

cmake_minimum_required(VERSION 2.8)

SET(PROJECTNAME boardTransceiver)
PROJECT(${PROJECTNAME})

#############################################################################################################
## the BASE variable must point to the root of the emBODY software, i.e. ${Whatever}/iCub/firmware/embody. ##
## If you followed the installation instruction and therefore the environment variable ICUB_ROOT           ##
## is set on your machine, CMake will use that value, so you don't have nothing to do.                     ## 
#############################################################################################################

set(BASE 				$ENV{ICUB_ROOT}/firmware/emBODY/)
set(MAIN_FOLDER  		${CMAKE_CURRENT_SOURCE_DIR}/)
set(NVS_BOARD_FOLDER  	${MAIN_FOLDER}nvscfg/local_pc104/)

include(../../embObjLib.conf)
find_package(YARP)
include_directories(${YARP_INCLUDE_DIRS})

# Debug messages
# message(INFO " BASE=${BASE}")
# message(INFO " lib_cmake - embObj_includes: ${embObj_includes}")
  	
# get all files needed, collected in different lists for better organization and handling

# Common, architecture independent
set(BODY_COMMON_SOURCE 		${BODY_COMMON_FOLDER}/eEcommon.c)
set(BODY_COMMON_HEADER 		${BODY_COMMON_FOLDER}/eEcommon.h)

# Network Variable configuration
set(NVS_COMMON_SOURCE 		${NVS_COMMON_FOLDER}/eOcfg_nvsEP_base_con.c
							${NVS_COMMON_FOLDER}/eOcfg_nvsEP_mngmnt_con.c
							${NVS_COMMON_FOLDER}/eOcfg_nvsEP_joint_con.c)

set(NVS_COMMON_HEADER 		${NVS_COMMON_FOLDER}/eOcfg_nvsEP_base_con.h
							${NVS_COMMON_FOLDER}/eOcfg_nvsEP_mngmnt_con.h
							${NVS_COMMON_FOLDER}/eOcfg_nvsEP_joint_con.h)

set(NVS_REMOTE_SOURCE		${NVS_REMOTE_FOLDER}/eOcfg_EPs_rem_board.c
							${NVS_REMOTE_FOLDER}/eOcfg_nvsEP_base_usr_rem_anydev.c
							${NVS_REMOTE_FOLDER}/eOcfg_nvsEP_mngmnt_usr_rem_board.c
							${NVS_REMOTE_FOLDER}/eOcfg_nvsEP_joint_usr_rem_board.c)

set(NVS_REMOTE_HEADER		${NVS_REMOTE_FOLDER}/eOcfg_EPs_rem_board.h
							${NVS_REMOTE_FOLDER}/eOcfg_nvsEP_base_usr_rem_anydev.h
							${NVS_REMOTE_FOLDER}/eOcfg_nvsEP_mngmnt_usr_rem_board.h
							${NVS_REMOTE_FOLDER}/eOcfg_nvsEP_jopint_usr_rem_board.h)

set(NVS_LOCAL_SOURCE		${NVS_LOCAL_FOLDER}/eOcfg_EPs_loc_board.c
							${NVS_LOCAL_FOLDER}/eOcfg_nvsEP_base_usr_loc_anydev.c
							${NVS_LOCAL_FOLDER}/eOcfg_nvsEP_mngmnt_usr_loc_board.c
							${NVS_LOCAL_FOLDER}/eOcfg_nvsEP_joint_usr_loc_board.c)

set(NVS_LOCAL_HEADER		${NVS_LOCAL_FOLDER}/eOcfg_EPs_loc_board.h
							${NVS_LOCAL_FOLDER}/eOcfg_nvsEP_base_usr_loc_anydev.h
							${NVS_LOCAL_FOLDER}/eOcfg_nvsEP_mngmnt_usr_loc_board.h
							${NVS_LOCAL_FOLDER}/eOcfg_nvsEP_jopint_usr_loc_board.h)

# Mio tentativo di improvvisare una scheda EMS fake da usare come test per l'applicazione... rimandato a quando avrò fatto un po' di codice in più. 
set(NVS_BOARD_SOURCE  	${NVS_BOARD_FOLDER}/eOcfg_EPs_loc_board_pc104.c
						${NVS_BOARD_FOLDER}/eOcfg_nvsEP_base_usr_loc_pc104.c
						${NVS_BOARD_FOLDER}/eOcfg_nvsEP_mngmnt_usr_loc_pc104.c
						${NVS_BOARD_FOLDER}/eOcfg_nvsEP_joint_usr_loc_pc104.c)

set(NVS_BOARD_HEADER  	${NVS_BOARD_FOLDER}/eOcfg_EPs_loc_board_pc104.h
						${NVS_BOARD_FOLDER}/eOcfg_nvsEP_base_usr_loc_pc104.h
						${NVS_BOARD_FOLDER}/eOcfg_nvsEP_mngmnt_usr_loc_pc104.h
						${NVS_BOARD_FOLDER}/eOcfg_nvsEP_joint_usr_loc_pc104.h)
						
# EmbObj-src
set(CORE_FOLDER_SOURCE  ${CORE_FOLDER}/EOarray.c
						${CORE_FOLDER}/EoCommon.c
						${CORE_FOLDER}/EOconstarray.c
						${CORE_FOLDER}/EOconstvector.c
						${CORE_FOLDER}/EOdeque.c
						${CORE_FOLDER}/EOfifo.c
						${CORE_FOLDER}/EOfifoByte.c
						${CORE_FOLDER}/EOfifoWord.c
						${CORE_FOLDER}/EOlist.c
						${CORE_FOLDER}/EOtheErrorManager.c
						${CORE_FOLDER}/EOtheMemoryPool.c
						${CORE_FOLDER}/EOvector.c)

set(CORE_VEE_SOURCE				${CORE_VEE_FOLDER}/EOVmutex.c
						${CORE_VEE_FOLDER}/EOVtask.c
						${CORE_VEE_FOLDER}/EOVtheSystem.c
#						${CORE_API}/EOVmutex_hid.h
#						${CORE_API}/EOVstorage_hid.h
#						${CORE_API}/EOVtask_hid.h
#						${CORE_API}/EOVtheSystem_hid.h
						)
						
set(CORE_API_HEADER	  	${CORE_API}/EOarray.h
						${CORE_API}/EoCommon.h
						${CORE_API}/EOconstarray.h
						${CORE_API}/EOconstvector.h
						${CORE_API}/EOdeque.h
						${CORE_API}/EOfifo.h
						${CORE_API}/EOfifoByte.h
						${CORE_API}/EOfifoWord.h
						${CORE_API}/EOlist.h
						${CORE_API}/EOtheErrorManager.h
						${CORE_API}/EOtheMemoryPool.h
						${CORE_API}/EOvector.h
#						${CORE_API}/EOVmutex.h
#						${CORE_API}/EOVstorage.h
#						${CORE_API}/EOVtask.h
#						${CORE_API}/EOVtheSystem.h
						)						

						
set(CORE_FOLDER_HEADER	${CORE_API}/EOarray_hid.h
						${CORE_API}/EoCommon_hid.h
						${CORE_API}/EOconstarray_hid.h
						${CORE_API}/EOconstvector_hid.h
						${CORE_API}/EOdeque_hid.h
						${CORE_API}/EOfifo_hid.h
						${CORE_API}/EOfifoByte_hid.h
						${CORE_API}/EOfifoWord_hid.h
						${CORE_API}/EOlist_hid.h
						${CORE_API}/EOtheErrorManager_hid.h
						${CORE_API}/EOtheMemoryPool_hid.h
						${CORE_API}/EOvector_hid.h
						)

# EmbObj-NV & Protocol files
set(PROT_SOURCE  				
						${PROT_FOLDER}/EOnv.c
						${PROT_FOLDER}/EOnvsCfg.c
						${PROT_FOLDER}/EOreceiver.c
						${PROT_FOLDER}/EOrop.c
						${PROT_FOLDER}/EOropframe.c
						${PROT_FOLDER}/EOtheAgent.c
						${PROT_FOLDER}/EOtheFormer.c
						${PROT_FOLDER}/EOtheParser.c
						${PROT_FOLDER}/EOtransceiver.c
						${PROT_FOLDER}/EOtransmitter.c
						${PROT_FOLDER}/EOtreenode.c
						${PROT_FOLDER}/EOtheBOARDtransceiver.c
						${PROT_FOLDER}/EOhostTransceiver.c
						${PROT_FOLDER}/EOVstorage.c
						${PROT_FOLDER}EOmatrix3d.c)

set(PROT_HEADER  		${PROT_FOLDER}/EOhostTransceiver_hid.h
						${PROT_FOLDER}/EOnv_hid.h
						${PROT_FOLDER}/EOnvsCfg_hid.h
						${PROT_FOLDER}/EOreceiver_hid.h
						${PROT_FOLDER}/EOrop_hid.h
						${PROT_FOLDER}/EOropframe_hid.h
						${PROT_FOLDER}/EOtheAgent_hid.h
						${PROT_FOLDER}/EOtheFormer_hid.h
						${PROT_FOLDER}/EOtheParser_hid.h
						${PROT_FOLDER}/EOtransceiver_hid.h
						${PROT_FOLDER}/EOtransmitter_hid.h
						${PROT_FOLDER}/EOtreenode_hid.h)


set(IPNET_SOURCE		${IPNET_FOLDER}/EOpacket.c)

set(IPNET_HEADER		${IPNET_FOLDER}/EOpacket_hid.h)


set(HAL_FAKE_SOURCE		${PORTING_FOLDER}/hal_fake.c)

set(MAIN_SOURCE 		${MAIN_FOLDER}/boardTransceiver.cpp 
						${MAIN_FOLDER}/main_BOARD_transceiver.cpp)

set(embobj_source 		${BODY_COMMON_SOURCE} 
						${CORE_FOLDER_SOURCE} 
						${CORE_VEE_SOURCE} 
						${NVS_COMMON_SOURCE} 
						${PROT_SOURCE} 
						${IPNET_SOURCE} 
						${HAL_FAKE_SOURCE} 
						${MAIN_SOURCE}
						${PROT_FOLDER})

set(embobj_header 		${BODY_COMMON_SOURCE} 
						${CORE_API_HEADER} 
						${PROT_HEADER} 
						${IPNET_HEADER})

set(remote_source		${embobj_source} 
						${NVS_REMOTE_SOURCE})

set(remote_header 		${embobj_header} 
						${NVS_REMOTE_HEADER})


set(board_source		${embobj_source} 
						${NVS_BOARD_SOURCE})

set(board_header 		${embobj_header} 
						${NVS_BOARD_HEADER})
						
include_directories (${NVS_BOARD_FOLDER} ${MAIN_FOLDER} ${BASE}/arch-host/demo/embobj/headers/)
						
SOURCE_GROUP("EmbObj Source Files" FILES ${folder_source})
SOURCE_GROUP("EmbObj Header Files" FILES ${folder_header})
  
add_executable(${PROJECTNAME} ${board_source} ${board_header})
target_link_libraries(${PROJECTNAME} ${YARP_LIBRARIES})

#icub_export_library(${PROJECTNAME})

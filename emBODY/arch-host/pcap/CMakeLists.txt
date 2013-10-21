cmake_minimum_required(VERSION 2.6)

set(projectname  mypcap)

project(${projectname})

find_package(YARP)

#pick all header and code files from current directory
set(${projectname}_source pcap.cpp debugFunctions.cpp)

set(BASE        $ENV{ICUB_ROOT}/firmware/emBODY/)
include(embObjLib.conf)
set(embobj_source 		${BODY_COMMON_SOURCE}
                                                ${CORE_FOLDER_SOURCE}
                                                ${CORE_VEE_SOURCE}
                                                ${PROT_SOURCE}
                                                ${IPNET_SOURCE}
                                                ${HAL_FAKE_SOURCE}
                                                ${PROT_FOLDER}
                                                ${NVS_EB_SOURCE}
                                                ${NVS_MC_SOURCE}
                                                ${NVS_MNGMNT_SOURCE}
                                                ${NVS_SENSORS_SOURCE}
                                                ${NVS_SKIN_SOURCE}
                                                ${NVS_USR_SOURCE}
                                                ${EXTRA_SOURCE}
                                                ${NVS_CBK_SOURCE})

#message(INFO, "YARP_INCLUDE_DIRS = ${YARP_INCLUDE_DIRS}")

include_directories(${embobj_source})


include_directories(${YARP_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${projectname}  ${${projectname}_source})

target_link_libraries(${projectname} ${YARP_LIBRARIES} pcap)


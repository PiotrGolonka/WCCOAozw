cmake_minimum_required(VERSION 2.6.3)

include(/opt/WinCC_OA/3.15/api/CMakeDefines.txt)

## We want to use std::mutex, etc
set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -g -ftest-coverage -fprofile-arcs")
#add_compile_options(-std=c++11)


set(TARGET WCCOAozw)

set (CMAKE_VERBOSE_MAKEFILE on)

set(SOURCES
WCCOAozwResources.cxx
WCCOAozwInternalDp.cxx
WCCOAozwDrv.cxx
WCCOAozwMain.cxx
WCCOAozwAlertService.cxx
WCCOAozwConnection.cxx
WCCOAozwHWMapper.cxx
WCCOAozwHWService.cxx
WCCOAozwTrans.cxx
)

find_package (PkgConfig)
pkg_check_modules (OPENZWAVE REQUIRED libopenzwave>=0.28)
add_driver(${TARGET} ${SOURCES})

include_directories(${OPENZWAVE_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${OPENZWAVE_LIBRARIES})

target_link_libraries(${TARGET} pthread)

#TARGET_LINK_LIBRARIES( ${TARGET} ${OPENZWAVE_LIBRARY} )


cmake_minimum_required(VERSION 2.6.3)

include(/opt/WinCC_OA/3.15/api/CMakeDefines.txt)

set(TARGET WCCOAWCCILozw)

set(SOURCES
WCCILozwAlertService.cxx
WCCILozwConnection.cxx
WCCILozwDrv.cxx
WCCILozwHWMapper.cxx
WCCILozwHWService.cxx
WCCILozwMain.cxx
WCCILozwResources.cxx
WCCILozwTrans.cxx
)

add_driver(${TARGET} ${SOURCES})

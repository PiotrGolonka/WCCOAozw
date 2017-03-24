cmake_minimum_required(VERSION 2.6.3)

include(/opt/WinCC_OA/3.15/api/CMakeDefines.txt)

set(TARGET WCCOAozw)

set(SOURCES
WCCOAozwAlertService.cxx
WCCOAozwConnection.cxx
WCCOAozwDrv.cxx
WCCOAozwHWMapper.cxx
WCCOAozwHWService.cxx
WCCOAozwMain.cxx
WCCOAozwResources.cxx
WCCOAozwTrans.cxx
)

add_driver(${TARGET} ${SOURCES})

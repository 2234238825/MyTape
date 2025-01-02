cmake_minimum_required(VERSION 3.26)
project(untitled13)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-std=c++14 -O0  -pthread -g -Wall ")

include_directories(${CMAKE_SOURCE_DIR}/)
include_directories(${CMAKE_SOURCE_DIR}/utils)
add_executable(untitled13
        main.cpp
        sg.h
        scsiLibrary.cpp
        scsiDrive.cpp
        LibraryInterface.cpp
        Log.cpp
        myFuntion.cpp
        tapeGlobal.cpp
        scsiExcute.cpp
        SCSIBase.cpp
        Catalog.cpp
        Catalog.h
)
#target_link_libraries(untitled13 libevent::core)
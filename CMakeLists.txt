cmake_minimum_required(VERSION 3.26)
project(untitled13)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-std=c++17 -O0  -pthread -g -Wall -ljson-c -ljsoncpp")



include_directories(${PROJECT_SOURCE_DIR}/utils)
include_directories(${PROJECT_SOURCE_DIR}/rpc)
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
        utils/BytePacker.cpp
        utils/OSAPI.h
        utils/netFlux.cpp
        rpc/rpcServer.cpp
        utils/convertJson.h
        utils/CSerialization.h
        utils/CSerialization.cpp
        utils/queueManager.cpp
        utils/queueManager.h
)
#target_link_libraries(untitled13 libevent::core)
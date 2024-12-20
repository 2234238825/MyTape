cmake_minimum_required(VERSION 3.26)
project(untitled13)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-std=c++14 -O0  -pthread -g -Wall ")

include_directories(
        ./libevent-2.1.12-stable/include/
)
add_subdirectory(./libevent-2.1.12-stable)
#find_package(Libevent REQUIRED)
#link_libraries(./libevent-2.1.12-stable/lib)
#add_subdirectory(./libevent-2.1.12-stable)
#include_directories(${LIBEVENT_INCLUDE_DIRS})
add_executable(untitled13
        main.cpp
        sg.h
        scsiLibrary.cpp
        scsiLibrary.h
        scsiDrive.cpp
        scsiDrive.h
        scsiStruct.h
        kfcStruct.h
        myFuntion.h
        LibraryInterface.cpp
        LibraryInterface.h
        Log.h
        Log.cpp
        myFuntion.cpp
        tapeGlobal.h
        tapeGlobal.cpp
        scsiExcute.cpp
        SCSIBase.cpp
        SCSIBase.h
)
#target_link_libraries(untitled13 libevent::core)
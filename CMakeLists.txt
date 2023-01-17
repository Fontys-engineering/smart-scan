cmake_minimum_required(VERSION 3.4)
project(smart-scan)


set(CMAKE_CXX_STANDARD 17)

set (CMAKE_C_FLAGS_RELEASE "-O3")
set (CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(.)
include_directories(SmartScanCli)
include_directories(SmartScanService)
#include_directories(simpleble_install)

add_subdirectory(SmartScanService)
#add_subdirectory(simpleble_install ${CMAKE_BINARY_DIR}/simpleble)

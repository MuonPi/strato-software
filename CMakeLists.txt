cmake_minimum_required (VERSION 3.10)
project (strato-software
        VERSION 0.1)

set(CMAKE_BUILD_TYPE Debug)
message("${CMAKE_HOST_SYSTEM_PROCESSOR}")
add_subdirectory(lmic_library)
add_subdirectory(lorawan)
cmake_minimum_required (VERSION 3.10)

project(lmic LANGUAGES CXX C)

# option(BUILD_TIDY "Use clang tidy to build" OFF)
# option(BUILD_TOOLS "Build additional tools (like readout program) in addition to the library" ON)
# option(BUILD_DOCS "Build doxygen files" OFF)

set(PROJECT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(PROJECT_SRC_DIR_AES "${CMAKE_CURRENT_SOURCE_DIR}/src/aes")
set(PROJECT_SRC_DIR_HAL "${CMAKE_CURRENT_SOURCE_DIR}/src/hal")
set(PROJECT_SRC_DIR_LMIC "${CMAKE_CURRENT_SOURCE_DIR}/src/lmic")
set(PROJECT_SRC_DIR_RASPI "${CMAKE_CURRENT_SOURCE_DIR}/src/raspi")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../bin")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(PROJECT_FILES
    "${PROJECT_SRC_DIR}/lmic.h"
    "${PROJECT_SRC_DIR_HAL}/hal.h"
    "${PROJECT_SRC_DIR_HAL}/hal.cpp"
    "${PROJECT_SRC_DIR_LMIC}/config.h"
    "${PROJECT_SRC_DIR_LMIC}/hal.h"
    "${PROJECT_SRC_DIR_LMIC}/lmic.c"
    "${PROJECT_SRC_DIR_LMIC}/lmic.h"
    "${PROJECT_SRC_DIR_LMIC}/lorabase.h"
    "${PROJECT_SRC_DIR_LMIC}/oslmic.c"
    "${PROJECT_SRC_DIR_LMIC}/oslmic.h"
    "${PROJECT_SRC_DIR_LMIC}/radio.c"
    "${PROJECT_SRC_DIR_AES}/lmic.c"
    "${PROJECT_SRC_DIR_AES}/other.c"
    "${PROJECT_SRC_DIR_AES}/ideetron/AES-128_V10.cpp"
    "${PROJECT_SRC_DIR_RASPI}/raspi.h" 
    "${PROJECT_SRC_DIR_RASPI}/raspi.cpp" 
    # "${PROJECT_SRC_DIR_RASPI}/spidevice.cpp"
    # "${PROJECT_SRC_DIR_RASPI}/spidevice.h"
    # "${PROJECT_SRC_DIR_RASPI}/raspi-spi.h" 
    # "${PROJECT_SRC_DIR_RASPI}/raspi-spi.cpp" 
)

add_compile_options(
    -Wall
    -Wextra
    -Wshadow
    -Wpedantic
    -O3
)

add_definitions(-Dlmic_objlib_LIBRARY_EXPORT)
add_library(lmic_objlib OBJECT ${PROJECT_FILES})

add_library(lmic SHARED $<TARGET_OBJECTS:lmic_objlib>)
add_library(lmic_static STATIC $<TARGET_OBJECTS:lmic_objlib>)

target_include_directories(lmic_objlib PUBLIC
    ${PROJECT_SRC_DIR}
    ${PROJECT_SRC_DIR_AES}
    ${PROJECT_SRC_DIR_HAL}
    ${PROJECT_SRC_DIR_LMIC}
    ${PROJECT_SRC_DIR_RASPI}
)

set_property(TARGET lmic_objlib PROPERTY POSITION_INDEPENDENT_CODE 1)

# set_target_properties(
# lmic
# PROPERTIES
# SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
# )

target_link_libraries(lmic gpiod) # NKRG

#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

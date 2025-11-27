cmake_minimum_required (VERSION 3.10)

project(lmic LANGUAGES CXX C)

# option(BUILD_TIDY "Use clang tidy to build" OFF)
# option(BUILD_TOOLS "Build additional tools (like readout program) in addition to the library" ON)
# option(BUILD_DOCS "Build doxygen files" OFF)

set(PROJECT_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/project_config")
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

set(LMIC_SOURCE_FILES
    "${PROJECT_SRC_DIR_AES}/lmic.c"
    "${PROJECT_SRC_DIR_AES}/other.c"
    "${PROJECT_SRC_DIR_AES}/ideetron/AES-128_V10.cpp"
    "${PROJECT_SRC_DIR_HAL}/hal.cpp"
    "${PROJECT_SRC_DIR_LMIC}/lmic_channelshuffle.c"
    "${PROJECT_SRC_DIR_LMIC}/lmic_compliance.c"
    "${PROJECT_SRC_DIR_LMIC}/lmic_eu_like.c"
    "${PROJECT_SRC_DIR_LMIC}/lmic_eu868.c"
    "${PROJECT_SRC_DIR_LMIC}/lmic_util.c"
    "${PROJECT_SRC_DIR_LMIC}/lmic.c"
    "${PROJECT_SRC_DIR_LMIC}/oslmic.c"
    "${PROJECT_SRC_DIR_LMIC}/radio.c"
    "${PROJECT_SRC_DIR_RASPI}/raspi.cpp"
    "${PROJECT_SRC_DIR_RASPI}/spidevice.cpp"
    "${PROJECT_SRC_DIR_RASPI}/gpiodevice.cpp"
)

set(LMIC_HEADER_FILES
    "${PROJECT_CONFIG_DIR}/lmic_project_config.h"
    "${PROJECT_SRC_DIR_HAL}/hal.h"
    "${PROJECT_SRC_DIR_LMIC}/config.h"
    "${PROJECT_SRC_DIR_LMIC}/hal.h"
    "${PROJECT_SRC_DIR_LMIC}/lmic_bandplan_eu868.h"
    "${PROJECT_SRC_DIR_LMIC}/lmic_bandplan.h"
    "${PROJECT_SRC_DIR_LMIC}/lmic_compliance.h"
    "${PROJECT_SRC_DIR_LMIC}/lmic_config_preconditions.h"
    "${PROJECT_SRC_DIR_LMIC}/lmic_env.h"
    "${PROJECT_SRC_DIR_LMIC}/lmic_compat.h"
    "${PROJECT_SRC_DIR_LMIC}/lmic_eu_like.h"
    "${PROJECT_SRC_DIR_LMIC}/lmic_util.h"
    "${PROJECT_SRC_DIR_LMIC}/lmic.h"
    "${PROJECT_SRC_DIR_LMIC}/lorabase_eu868.h"
    "${PROJECT_SRC_DIR_LMIC}/lorabase.h"
    "${PROJECT_SRC_DIR_LMIC}/lorawan_spec_compliance.h"
    "${PROJECT_SRC_DIR_LMIC}/oslmic_types.h"
    "${PROJECT_SRC_DIR_LMIC}/oslmic.h"
    "${PROJECT_SRC_DIR_RASPI}/raspi.h"
    "${PROJECT_SRC_DIR_RASPI}/spidevice.h"
    "${PROJECT_SRC_DIR_RASPI}/gpiodevice.h"
    "${PROJECT_SRC_DIR}/arduino_lmic_hal_boards.h"
    "${PROJECT_SRC_DIR}/arduino_lmic_hal_configuration.h"
    "${PROJECT_SRC_DIR}/arduino_lmic_lorawan_compliance.h"
    "${PROJECT_SRC_DIR}/arduino_lmic_user_configuration.h"
    "${PROJECT_SRC_DIR}/arduino_lmic.h"
    "${PROJECT_SRC_DIR}/lmic.h"
)

add_compile_options(
    -Wall
    -Wextra
    -Wshadow
    -Wpedantic
    -O3
)

add_library(lmic SHARED ${LMIC_SOURCE_FILES} ${LMIC_HEADER_FILES})
target_compile_definitions(lmic PUBLIC lmic_LIBRARY_EXPORT)
set_target_properties(lmic PROPERTIES POSITION_INDEPENDENT_CODE 1)


target_include_directories(lmic PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SRC_DIR}>
    $<BUILD_INTERFACE:${PROJECT_SRC_DIR_AES}>
    $<BUILD_INTERFACE:${PROJECT_SRC_DIR_HAL}>
    $<BUILD_INTERFACE:${PROJECT_SRC_DIR_LMIC}>
    $<BUILD_INTERFACE:${PROJECT_SRC_DIR_RASPI}>
    $<BUILD_INTERFACE:${PROJECT_CONFIG_DIR}>
)

# add_definitions(
#     -DARDUINO_LMIC_PROJECT_CONFIG_H_SUPPRESS
#     -DCFG_sx1276_radio=1
#     -DCFG_eu868=1
#     -DDISABLE_JOIN
#     -DDISABLE_PING
#     -DDISABLE_BEACONS
#     -DLMIC_DEBUG_LEVEL=0
# )

set_target_properties(
    lmic
    PROPERTIES
    SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
)


target_link_libraries(lmic gpiod) # NKRG

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

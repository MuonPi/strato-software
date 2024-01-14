cmake_minimum_required (VERSION 3.10)

project(STRATO
    VERSION 0.1
    LANGUAGES CXX C)

# option(BUILD_TIDY "Use clang tidy to build" OFF)
# option(BUILD_TOOLS "Build additional tools (like readout program) in addition to the library" ON)
# option(BUILD_DOCS "Build doxygen files" OFF)

set(PROJECT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
# set(PROJECT_SRC_DIR_AES "${CMAKE_CURRENT_SOURCE_DIR}/src/aes")
set(PROJECT_SRC_DIR_HAL "${CMAKE_CURRENT_SOURCE_DIR}/src/hal")
set(PROJECT_SRC_DIR_LMIC "${CMAKE_CURRENT_SOURCE_DIR}/src/lmic")
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(STRATO_SOURCE_FILES
    "${PROJECT_SRC_DIR}/lmic.h"
    "${PROJECT_SRC_DIR_HAL}/hal.h"
    "${PROJECT_SRC_DIR_HAL}/hal.cpp"
    "${PROJECT_SRC_DIR_HAL}/raspi-spi.h"
    "${PROJECT_SRC_DIR_HAL}/raspi-spi.cpp"
    "${PROJECT_SRC_DIR_LMIC}/config.h"
    "${PROJECT_SRC_DIR_LMIC}/hal.h"
    "${PROJECT_SRC_DIR_LMIC}/lmic.c"
    "${PROJECT_SRC_DIR_LMIC}/lmic.h"
    "${PROJECT_SRC_DIR_LMIC}/lorabase.h"
    "${PROJECT_SRC_DIR_LMIC}/oslmic.c"
    "${PROJECT_SRC_DIR_LMIC}/oslmic.h"
    "${PROJECT_SRC_DIR_LMIC}/radio.c"
)

# set(STRATO_SOURCE_FILES
#     "${PROJECT_SRC_DIR2}/lmic.c"
#     "${PROJECT_SRC_DIR2}/oslmic.c"
#     "${PROJECT_SRC_DIR2}/radio.c"
#     "${PROJECT_SRC_DIR3}/hal.cpp"
#     # "${PROJECT_SRC_DIR4}/other.c"
#     # "${PROJECT_SRC_DIR4}/lmic.c"
# )

add_library(_lmic SHARED ${STRATO_SOURCE_FILES})
# target_compile_definitions(arduino PUBLIC RASPBERRY_PI=1 __BASEFILE__="StratoSat" BCM2835_NO_DELAY_COMPATIBILITY=1)

target_include_directories(_lmic PUBLIC
    ${PROJECT_SRC_DIR}
    ${PROJECT_SRC_DIR_AES}
    ${PROJECT_SRC_DIR_HAL}
    ${PROJECT_SRC_DIR_LMIC}
)

# set(THREADS_PREFER_PTHREAD_FLAG ON)
# find_package(Threads REQUIRED)

target_link_libraries(_lmic bcm2835) # NKRG

# target_link_libraries(_lmic
#     Threads::Threads
#     -lbcm2835 # NKRG
# )


#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
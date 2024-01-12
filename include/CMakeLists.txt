cmake_minimum_required (VERSION 3.10)
project(STRATO LANGUAGES CXX)

# option(BUILD_TIDY "Use clang tidy to build" OFF)
# option(BUILD_TOOLS "Build additional tools (like readout program) in addition to the library" ON)
# option(BUILD_DOCS "Build doxygen files" OFF)

set(PROJECT_INCLUDE_DIR1 "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(PROJECT_INCLUDE_DIR2 "${CMAKE_CURRENT_SOURCE_DIR}/src/lmic")
set(PROJECT_SRC_DIR2 "${CMAKE_CURRENT_SOURCE_DIR}/src/lmic")
set(PROJECT_INCLUDE_DIR3 "${CMAKE_CURRENT_SOURCE_DIR}/src/hal")
set(PROJECT_SRC_DIR3 "${CMAKE_CURRENT_SOURCE_DIR}/src/hal")
# set(PROJECT_SRC_DIR4 "${CMAKE_CURRENT_SOURCE_DIR}/src/ideetron")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(STRATO_HEADER_FILES
    "${PROJECT_INCLUDE_DIR1}/lmic.h"
    "${PROJECT_INCLUDE_DIR2}/hal.h"
    "${PROJECT_INCLUDE_DIR2}/config.h"
    "${PROJECT_INCLUDE_DIR2}/lmic.h"
    "${PROJECT_INCLUDE_DIR2}/lorabase.h"
    "${PROJECT_INCLUDE_DIR2}/oslmic.h"
    "${PROJECT_INCLUDE_DIR3}/hal.h"
    "${PROJECT_INCLUDE_DIR3}/SPI.h"
)

set(STRATO_SOURCE_FILES
    "${PROJECT_SRC_DIR2}/lmic.c"
    "${PROJECT_SRC_DIR2}/oslmic.c"
    "${PROJECT_SRC_DIR2}/radio.c"
    "${PROJECT_SRC_DIR3}/hal.cpp"
    # "${PROJECT_SRC_DIR4}/other.c"
    # "${PROJECT_SRC_DIR4}/lmic.c"
)

add_library(ibm_lmic SHARED ${STRATO_SOURCE_FILES})
# target_compile_definitions(arduino PUBLIC RASPBERRY_PI=1 __BASEFILE__="StratoSat" BCM2835_NO_DELAY_COMPATIBILITY=1)

target_include_directories(ibm_lmic PUBLIC
    ${PROJECT_INCLUDE_DIR1}
    ${PROJECT_INCLUDE_DIR2}
    ${PROJECT_INCLUDE_DIR3}
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(ibm_lmic
    Threads::Threads
    -lbcm2835
)


#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
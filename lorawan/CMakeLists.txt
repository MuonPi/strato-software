cmake_minimum_required (VERSION 3.10)

project(STRATO
    VERSION 0.1
    LANGUAGES CXX C)

# option(BUILD_TIDY "Use clang tidy to build" OFF)
# option(BUILD_TOOLS "Build additional tools (like readout program) in addition to the library" ON)
# option(BUILD_DOCS "Build doxygen files" OFF)

set(PROJECT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(PROJECT_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(PROJECT_LMIC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../lmic_library/src")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../bin")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(STRATO_SOURCE_FILES
    "${PROJECT_SRC_DIR}/main.cpp"
    "${PROJECT_SRC_DIR}/strato_lmic.cpp"
)

set(STRATO_HEADER_FILES
    "${PROJECT_HEADER_DIR}/strato_lmic.h")

add_executable(lorawan ${STRATO_SOURCE_FILES} ${STRATO_HEADER_FILES})

include_directories(lorawan PUBLIC
    ${PROJECT_HEADER_DIR}
    ${PROJECT_LMIC_DIR}
)

target_link_directories(lorawan PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/../lib/")
# find_library(LIBGPIOD libgpiod REQUIRED)

target_link_libraries(lorawan
    gpiod
    lmic_static
)  # NKRG

#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

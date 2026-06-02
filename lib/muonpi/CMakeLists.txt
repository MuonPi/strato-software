cmake_minimum_required(VERSION 3.16)
project(muonpi)

find_package(CapnProto REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------
# Qt Settings
# -------------------------------
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)
find_package(Qt6 REQUIRED COMPONENTS Core Network)
find_package(Threads REQUIRED)

# -------------------------------
# Externe Lib protocol
# -------------------------------
set(MUONPI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../muondetector_v2")
set(MUONPI_HEADER_DIR "${MUONPI_DIR}/library/src")
set(MUONPI_LIB_DIR "${MUONPI_DIR}/build/output/lib")
if(NOT EXISTS "${MUONPI_LIB_DIR}/libmuondetector-protocol.a")
    message(FATAL_ERROR "Library not found: ${MUONPI_LIB_DIR}/libmuondetector-protocol.a")
endif()
add_library(protocol STATIC IMPORTED)
set_target_properties(protocol PROPERTIES
    IMPORTED_LOCATION "${MUONPI_LIB_DIR}/libmuondetector-protocol.a"
)

# -------------------------------
# Externe Lib muondetector
# -------------------------------
if(NOT EXISTS "${MUONPI_LIB_DIR}/libmuondetector-static.a")
    message(FATAL_ERROR "Library not found: ${MUONPI_LIB_DIR}/libmuondetector-static.a")
endif()
add_library(muondetector STATIC IMPORTED)
set_target_properties(muondetector PROPERTIES
    IMPORTED_LOCATION "${MUONPI_LIB_DIR}/libmuondetector-static.a"
)

# -------------------------------
# muonpi static library
# -------------------------------
add_library(muonpi STATIC
    muonpi.cpp
    muonpi.h
)

# Header sind im gleichen Ordner
target_include_directories(muonpi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MUONPI_HEADER_DIR}
)

# Link externe Libraries
target_link_libraries(muonpi PUBLIC
    protocol
    muondetector
    Qt6::Core
    Qt6::Network
    Threads::Threads
    ${CAPNP_LIBRARIES}
    ${CAPNP_KJ_LIBRARIES}
)

add_executable(testclient muonpi.cpp muonpi.h main.cpp)

# Header sind im gleichen Ordner
target_include_directories(testclient PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MUONPI_HEADER_DIR}
)
# Link externe Libraries
target_link_libraries(testclient PUBLIC
    protocol
    muondetector
    Qt6::Core
    Qt6::Network
    Threads::Threads
    ${CAPNP_LIBRARIES}
    ${CAPNP_KJ_LIBRARIES}
)

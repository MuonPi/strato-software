cmake_minimum_required(VERSION 3.16)
project(muonpi-connector)

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

add_library(muonpi-connector STATIC
    muonpi-connector.cpp
    muonpi-connector.h
)

target_include_directories(muonpi-connector PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../muondetector/library/src
)

# Link externe Libraries
target_link_libraries(muonpi-connector PUBLIC
    muondetector-protocol
    muondetector-static
    Qt6::Core
    Qt6::Network
    Threads::Threads
)

if (BUILD_TESTCLIENTS)
add_executable(testclient muonpi-connector.cpp muonpi-connector.h main.cpp)

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
endif()

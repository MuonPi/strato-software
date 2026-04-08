cmake_minimum_required(VERSION 3.16)
project(muonpi)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------
# Qt Settings
# -------------------------------
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)
find_package(Qt5 5.15 COMPONENTS Core Network REQUIRED)
find_package(Threads REQUIRED)

# -------------------------------
# Externe Lib muondetector
# -------------------------------
set(MUONPI_HEADER_DIR "/home/user/muondetector/library/include")
set(MUONPI_LIB_DIR "/home/user/muondetector/build/output/lib")
add_library(muondetector STATIC IMPORTED)
set_target_properties(muondetector PROPERTIES
    IMPORTED_LOCATION "${MUONPI_LIB_DIR}/libmuondetector.a"
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
    muondetector
    Qt5::Core
    Qt5::Network
    Threads::Threads
)

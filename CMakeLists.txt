
cmake_minimum_required(VERSION 3.10)

project(strato-software VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------
# Build Type & Standards
# ------------------------------
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ------------------------------
# Folders
# ------------------------------
set(PROJECT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(PROJECT_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(PROJECT_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")

# ------------------------------
# Source & Header Files
# ------------------------------
set(STRATO_SOURCE_FILES
    "${PROJECT_SRC_DIR}/main.cpp"
    "${PROJECT_SRC_DIR}/strato-lorawan.cpp"
    "${PROJECT_SRC_DIR}/strato-sensors.cpp"
    "${PROJECT_SRC_DIR}/logfile.cpp"
)

set(STRATO_HEADER_FILES
    "${PROJECT_HEADER_DIR}/strato-lorawan.h"
    "${PROJECT_HEADER_DIR}/strato-sensors.h"
    "${PROJECT_HEADER_DIR}/strato-config.h"
    "${PROJECT_HEADER_DIR}/logfile.h"
)

# ------------------------------
# Executable
# ------------------------------
add_executable(strato-software
    ${STRATO_SOURCE_FILES}
    ${STRATO_HEADER_FILES}
)

target_include_directories(strato-software PUBLIC
    ${PROJECT_HEADER_DIR}
)

# ------------------------------
# Librarys
# ------------------------------
add_subdirectory(${PROJECT_LIB_DIR}/lmic)

add_subdirectory(${PROJECT_LIB_DIR}/sensors)

add_subdirectory(${PROJECT_LIB_DIR}/cayennelpp)

target_link_libraries(strato-software
    gpiod
    lmic
    sensors
    cayennelpp
    atomic
)

# ------------------------------
# Install Rules
# ------------------------------
set(BIN_INSTALL_DIR "/usr/bin")
set(DATA_INSTALL_DIR "/var/strato-software")
set(SERVICE_NAME "strato-software")
set(SERVICE_INSTALL_DIR "/etc/systemd/system")
set(SERVICE_USER "user")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${SERVICE_NAME}.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/${SERVICE_NAME}.service
    @ONLY
)

install(CODE "
file(MAKE_DIRECTORY \"${DATA_INSTALL_DIR}\")
execute_process(COMMAND chown ${SERVICE_USER}:${SERVICE_USER} ${DATA_INSTALL_DIR})
execute_process(COMMAND chmod 755 ${DATA_INSTALL_DIR})
")

install(TARGETS strato-software
        RUNTIME DESTINATION ${BIN_INSTALL_DIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${SERVICE_NAME}.service
        DESTINATION ${SERVICE_INSTALL_DIR})


cmake_minimum_required (VERSION 3.10)
project (strato-software
        VERSION 0.1)

set(CMAKE_BUILD_TYPE Debug)

set(BIN_INSTALL_DIR "/usr/bin")
set(SERVICE_NAME "strato-lorawan")
set(SERVICE_USER "user")
set(SERVICE_INSTALL_DIR "/etc/systemd/system")
set(CONFIG_INSTALL_DIR "/etc")
set(DATA_INSTALL_DIR "/var/strato-software")
set(LOG_INSTALL_DIR "/var/log/strato-software")

message("${CMAKE_HOST_SYSTEM_PROCESSOR}")

# add_compile_definitions(ARDUINO_LMIC_PROJECT_CONFIG_H_SUPPRESS 1)
# add_compile_definitions(CFG_eu868 1)
# add_compile_definitions(CFG_sx1276_radio 1)
# add_compile_definitions(DISABLE_JOIN 1)
# add_compile_definitions(DISABLE_PING 1)
# add_compile_definitions(DISABLE_BEACON 1)
add_subdirectory(lmic_library)
add_subdirectory(lorawan)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/${SERVICE_NAME}.service.in
  ${CMAKE_CURRENT_BINARY_DIR}/${SERVICE_NAME}.service
  @ONLY
)

install(TARGETS lorawan
        RUNTIME DESTINATION ${BIN_INSTALL_DIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${SERVICE_NAME}.service
        DESTINATION ${SERVICE_INSTALL_DIR})


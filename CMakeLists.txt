cmake_minimum_required (VERSION 3.10)
project (strato-software
        VERSION 0.1)

set(CMAKE_BUILD_TYPE Debug)
message("${CMAKE_HOST_SYSTEM_PROCESSOR}")
# add_compile_definitions(ARDUINO_LMIC_PROJECT_CONFIG_H_SUPPRESS 1)
# add_compile_definitions(CFG_eu868 1)
# add_compile_definitions(CFG_sx1276_radio 1)
# add_compile_definitions(DISABLE_JOIN 1)
# add_compile_definitions(DISABLE_PING 1)
# add_compile_definitions(DISABLE_BEACON 1)
add_subdirectory(lmic_library)
add_subdirectory(lorawan)

add_library(lmic STATIC
    src/aes/lmic.c
    src/aes/other.c
    src/aes/ideetron/AES-128_V10.cpp
    src/hal/hal.cpp
    src/lmic/lmic_channelshuffle.c
    src/lmic/lmic_compliance.c
    src/lmic/lmic_eu_like.c
    src/lmic/lmic_eu868.c
    src/lmic/lmic_util.c
    src/lmic/lmic.c
    src/lmic/oslmic.c
    src/lmic/radio.c
    src/raspi/raspi.cpp
    src/raspi/spidevice.cpp
    src/raspi/gpiodevice.cpp
)

target_include_directories(lmic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/project_config
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/aes
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lmic
    ${CMAKE_CURRENT_SOURCE_DIR}/src/raspi
)

# target_compile_definitions(lmic PUBLIC
#     ARDUINO_LMIC_PROJECT_CONFIG_H_SUPPRESS
#     CFG_sx1276_radio=1
#     CFG_eu868=1
#     DISABLE_JOIN
#     DISABLE_PING
#     DISABLE_BEACONS
#     LMIC_DEBUG_LEVEL=0
# )

target_compile_options(lmic PRIVATE
    -Wall
    -Wextra
    -Wshadow
    -Wpedantic
)

target_link_libraries(lmic PRIVATE gpiod)

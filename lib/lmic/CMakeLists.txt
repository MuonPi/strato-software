

set(LMIC_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/arduino-lmic/src")
set(LMIC_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/arduino-lmic/project_config")
set(LMIC_RASPI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/raspi")

add_library(lmic STATIC
    ${LMIC_SRC_DIR}/aes/lmic.c
    ${LMIC_SRC_DIR}/aes/other.c
    ${LMIC_SRC_DIR}/aes/ideetron/AES-128_V10.cpp
    ${LMIC_SRC_DIR}/hal/hal.cpp
    ${LMIC_SRC_DIR}/lmic/lmic_channelshuffle.c
    ${LMIC_SRC_DIR}/lmic/lmic_compliance.c
    ${LMIC_SRC_DIR}/lmic/lmic_eu_like.c
    ${LMIC_SRC_DIR}/lmic/lmic_eu868.c
    ${LMIC_SRC_DIR}/lmic/lmic_util.c
    ${LMIC_SRC_DIR}/lmic/lmic.c
    ${LMIC_SRC_DIR}/lmic/oslmic.c
    ${LMIC_SRC_DIR}/lmic/radio_sx126x.c
    ${LMIC_SRC_DIR}/lmic/radio_sx127x.c
    ${LMIC_SRC_DIR}/se/drivers/default/lmic_se_default.c
    ${LMIC_RASPI_DIR}/raspi.cpp
    ${LMIC_RASPI_DIR}/spidevice.cpp
    ${LMIC_RASPI_DIR}/gpiodevice.cpp
    ${LMIC_RASPI_DIR}/lmic_lorawan.cpp
)

target_include_directories(lmic PUBLIC
    ${LMIC_CONFIG_DIR}
    ${LMIC_SRC_DIR}
    ${LMIC_SRC_DIR}/aes
    ${LMIC_SRC_DIR}/hal
    ${LMIC_SRC_DIR}/lmic
    ${LMIC_SRC_DIR}/se
    ${LMIC_RASPI_DIR}
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

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBGPIOD libgpiod)
if(LIBGPIOD_VERSION VERSION_LESS "2.0.0")
    add_definitions(-DLIBGPIOD_V1)
else()
    add_definitions(-DLIBGPIOD_V2)
endif()

target_link_libraries(lmic PRIVATE gpiod)

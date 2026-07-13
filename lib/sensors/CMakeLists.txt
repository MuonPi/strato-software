
add_library(sensors STATIC
    src/i2cdevice.cpp
    src/hmc5883.cpp
    src/qmc5883.cpp
    src/sen0321.cpp
    src/veml6075.cpp
    src/ozone3click.cpp
)

target_include_directories(sensors PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../muonpi-connector
    ${CMAKE_CURRENT_SOURCE_DIR}/../muondetector/daemon/src/hardware
    ${CMAKE_CURRENT_SOURCE_DIR}/../muondetector/daemon/src/hardware/i2c
)

# find_library(I2C_LIB i2c REQUIRED)

# target_link_libraries(sensors PRIVATE i2c) #${I2C_LIB})

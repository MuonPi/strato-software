
add_library(sensors STATIC
    src/i2cdevice.cpp
    src/ads1115.cpp
    src/bme280.cpp
    src/hmc5883.cpp
    src/qmc5883.cpp
    src/sen0321.cpp
    src/veml6075.cpp
)

target_include_directories(sensors PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# find_library(I2C_LIB i2c REQUIRED)

target_link_libraries(sensors PRIVATE i2c) #${I2C_LIB})

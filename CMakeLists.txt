cmake_minimum_required(VERSION 3.10)
project(STRATO LANGUAGES CXX C)

set(PROJECT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(PROJECT_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../bin")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(${BUILD_TIDY})
    set(CMAKE_CXX_CLANG_TIDY
      clang-tidy;
      -header-filter=^global;
      -checks=-*,readability-*,bugprone-*,performace-*,clang-analyzer-*,modernize-*,hicpp-*;
    )
endif(${BUILD_TIDY})

add_compile_options(
    -Wall
    -Wextra
    -Wshadow
    -Wpedantic
    -O3
)

# set(spi_test_HEADER_FILES
# )
set(STRATO_SOURCE_FILES
    "${PROJECT_SRC_DIR}/main.cpp"
    "${PROJECT_LIB_DIR}/i2cdevice.cpp"
    "${PROJECT_LIB_DIR}/qmc5883.cpp"
    "${PROJECT_LIB_DIR}/sen0321.cpp"
    "${PROJECT_LIB_DIR}/veml6075.cpp"
)

add_executable(strato-software ${STRATO_SOURCE_FILES})

target_include_directories(strato-software PUBLIC
    ${PROJECT_INCLUDE_DIR}
)

# target_link_directories(spi_test PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/../lib/")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(strato-software
    Threads::Threads
)

#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")


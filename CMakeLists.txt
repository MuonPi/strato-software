cmake_minimum_required (VERSION 3.10)

project(STRATO
    VERSION 0.1
    LANGUAGES CXX C)

# option(BUILD_TIDY "Use clang tidy to build" OFF)
# option(BUILD_TOOLS "Build additional tools (like readout program) in addition to the library" ON)
# option(BUILD_DOCS "Build doxygen files" OFF)

set(PROJECT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(PROJECT_LMIC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/lmic_library/src")
set(PROJECT_LIB_LMIC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/lmic_library/build")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(STRATO_SOURCE_FILES
    "${PROJECT_SRC_DIR}/main.cpp"
    "${PROJECT_SRC_DIR}/strato_lmic.cpp"
)

add_executable(strato-software ${STRATO_SOURCE_FILES})

include_directories(strato-software PUBLIC
    ${PROJECT_INCLUDE_DIR}
    ${PROJECT_LMIC_DIR}
    ${PROJECT_LIB_LMIC_DIR}
)

# set(THREADS_PREFER_PTHREAD_FLAG ON)
# find_package(Threads REQUIRED)

# target_link_libraries(strato-software
#     Threads::Threads
# )

target_link_libraries(strato-software
    ${PROJECT_LIB_LMIC_DIR}/lib_lmic.so
    bcm2835)  # NKRG

#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

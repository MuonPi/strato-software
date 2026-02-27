
add_library(cayennelpp STATIC
    CayenneLPP.cpp
)

target_include_directories(cayennelpp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

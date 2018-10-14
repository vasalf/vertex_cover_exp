add_executable(kernels
    kernels.cpp
)

target_include_directories(kernels PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../ascii_table/include
)
target_link_libraries(kernels LINK_PUBLIC ascii_table)

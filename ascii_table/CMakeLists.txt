add_library (ascii_table STATIC
  table.cpp
  column.cpp
  cell.cpp)
target_include_directories (ascii_table PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable (example
  example.cpp)
target_include_directories (example PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries (example LINK_PUBLIC ascii_table)

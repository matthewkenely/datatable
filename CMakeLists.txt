set(CMAKE_CXX_COMPILER /usr/bin/gcc)

set(dir ${CMAKE_CURRENT_SOURCE_DIR} ./build)

cmake_minimum_required (VERSION 3.15)
project(datatable C)

set(CMAKE_C_STANDARD 11)

add_executable(datatable DataTable.c DataTableFunctions.c DataTableFunctions.h)

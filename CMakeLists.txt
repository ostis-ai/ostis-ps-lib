cmake_minimum_required(VERSION 3.11)
project(ui-translators)
set(SC_BUILD_TESTS ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(SC_EXTENSIONS_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/extensions)

if(${SC_BUILD_TESTS})
    include(CTest)
    include(GoogleTest)
    find_package(GTest REQUIRED)
endif()
find_package(sc-machine REQUIRED)

if(${SC_CLANG_FORMAT_CODE})
    include(cmake/ClangFormat.cmake)
endif()
add_subdirectory(problem-solver)

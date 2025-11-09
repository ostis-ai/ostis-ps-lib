file(GLOB SOURCES CONFIGURE_DEPENDS
    "units/*.cpp"
    "units/*.hpp"
)

add_executable(fixed-search-strategy-template-processing-module-tests ${SOURCES})
target_link_libraries(fixed-search-strategy-template-processing-module-tests
    LINK_PRIVATE GTest::gtest_main
    LINK_PRIVATE fixed-search-strategy-template-processing-module
    LINK_PRIVATE sc-machine::sc-builder-lib
)
target_include_directories(fixed-search-strategy-template-processing-module-tests
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/units
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
)
gtest_discover_tests(fixed-search-strategy-template-processing-module-tests WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/units)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(fixed-search-strategy-template-processing-module-tests)
endif()
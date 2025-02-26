file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
    "units/*.cpp"
)
add_definitions(-DFORMAT_TRANSLATORS_MODULE_TEST_SRC_PATH="${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(format-translators-tests ${TEST_SOURCES})
target_link_libraries(format-translators-tests
    LINK_PRIVATE sc-machine::sc-builder-lib
    LINK_PRIVATE GTest::gtest_main
    LINK_PRIVATE format-translators)
target_include_directories(format-translators-tests
    PRIVATE ${FORMAT_TRANSLATORS_MODULE_SRC_PATH}
)

gtest_discover_tests(format-translators-tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(format-translators-tests)
endif()

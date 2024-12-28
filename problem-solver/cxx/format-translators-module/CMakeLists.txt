file(GLOB SOURCES CONFIGURE_DEPENDS
	"*.cpp" "*.hpp"
	"agent/*/*.cpp" "agent/*/*.hpp"
	"keynodes/*.hpp"
	"model/*.cpp" "model/*.hpp"
	"visitor/*.cpp" "visitor/*.hpp"
)

add_library(format-translators SHARED ${SOURCES})
target_link_libraries(format-translators LINK_PUBLIC
	sc-machine::sc-memory
	sc-machine::sc-core
	sc-machine::sc-agents-common
)
target_include_directories(format-translators PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(SC_EXTENSIONS_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/extensions)
set_target_properties(format-translators PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSIONS_DIRECTORY})

if (${SC_BUILD_TESTS})
	file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
		"test/units/*.cpp"
	)
	add_definitions(-DFORMAT_TRANSLATORS_MODULE_TEST_SRC_PATH="${CMAKE_CURRENT_SOURCE_DIR}/test")
	add_executable(format-translators-tests ${TEST_SOURCES})
	target_link_libraries(format-translators-tests LINK_PRIVATE sc-machine::sc-builder-lib gtest gtest_main format-translators)
	target_include_directories(format-translators-tests
		PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
	)

	gtest_discover_tests(format-translators-tests
		TEST_LIST ${TEST_SOURCES}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
	)
	if (${SC_CLANG_FORMAT_CODE})
		target_clangformat_setup(format-translators-tests)
	endif()
endif ()

if (${SC_CLANG_FORMAT_CODE})
	target_clangformat_setup(format-translators)
endif()

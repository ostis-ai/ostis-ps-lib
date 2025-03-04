file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "agent/*/*.cpp" "agent/*/*.hpp"
    "constants/*.cpp" "constants/*.hpp"
    "keynodes/*.hpp"
    "model/*.cpp" "model/*.hpp"
    "parser/*.cpp" "parser/*.hpp"
    "visitor/*.cpp" "visitor/*.hpp"
)

add_library(format-translators SHARED ${SOURCES})
target_link_libraries(format-translators
        LINK_PUBLIC sc-machine::sc-memory
        LINK_PUBLIC sc-machine::sc-agents-common
)
target_include_directories(format-translators PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(format-translators PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSIONS_DIRECTORY})

if(${SC_BUILD_TESTS})
    set(FORMAT_TRANSLATORS_MODULE_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(test)
endif()

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(format-translators)
endif()

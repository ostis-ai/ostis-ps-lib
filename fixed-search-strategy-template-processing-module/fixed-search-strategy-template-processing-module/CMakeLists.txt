file(GLOB SOURCES CONFIGURE_DEPENDS
    "agent/*"
    "keynodes/*"
    "data/*"
    "*.cpp" "*.hpp"
)

add_library(fixed-search-strategy-template-processing-module SHARED ${SOURCES})
target_link_libraries(fixed-search-strategy-template-processing-module
    LINK_PUBLIC sc-machine::sc-memory
)
target_include_directories(fixed-search-strategy-template-processing-module
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(fixed-search-strategy-template-processing-module
    PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSIONS_DIRECTORY}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(fixed-search-strategy-template-processing-module)
endif()

if(${SC_BUILD_TESTS})
    add_subdirectory(test)
endif()

file(GLOB SOURCES CONFIGURE_DEPENDS
    "lib/include/fixed-search-strategy-template/*.hpp"
    "lib/src/*.hpp" "lib/src/*.cpp"
)

add_library(fixed-search-strategy-template-object OBJECT ${SOURCES})
target_link_libraries(fixed-search-strategy-template-object
    LINK_PUBLIC sc-machine::sc-memory
)
target_include_directories(fixed-search-strategy-template-object
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/src
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/include>
    PUBLIC $<INSTALL_INTERFACE:include>
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(fixed-search-strategy-template-object)
endif()

add_library(fixed-search-strategy-template SHARED)
target_link_libraries(fixed-search-strategy-template 
    LINK_PUBLIC fixed-search-strategy-template-object
)

file(GLOB SOURCES CONFIGURE_DEPENDS
    "module/*.hpp" "module/*.cpp"
    "module/*/*.hpp" "module/*/*.cpp"
)

add_library(fixed-search-strategy-template-processing-module SHARED ${SOURCES})
target_link_libraries(fixed-search-strategy-template-processing-module
    LINK_PUBLIC fixed-search-strategy-template-object
)
target_include_directories(fixed-search-strategy-template-processing-module
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/module
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

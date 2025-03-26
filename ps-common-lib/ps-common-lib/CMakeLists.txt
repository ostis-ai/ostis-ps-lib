set(SOURCES
    "src/utils/logic_utils.cpp"
    "src/utils/relation_utils.cpp"
    "src/utils/template_params_utils.cpp"
    "src/action_cancelled_exception.cpp"
)

set(HEADERS
    "include/ps-common-lib/utils/macros.hpp"
    "include/ps-common-lib/utils/logic_utils.hpp"
    "include/ps-common-lib/utils/relation_utils.hpp"
    "include/ps-common-lib/utils/template_params_utils.hpp"
    "include/ps-common-lib/keynodes.hpp"
    "include/ps-common-lib/action_cancelled_exception.hpp"
)

add_library(common-utils SHARED ${SOURCES} ${HEADERS})
target_link_libraries(common-utils
    LINK_PUBLIC sc-machine::sc-memory
    LINK_PUBLIC sc-machine::sc-agents-common
)
target_include_directories(common-utils
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PUBLIC $<INSTALL_INTERFACE:include>
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(common-utils)
endif()

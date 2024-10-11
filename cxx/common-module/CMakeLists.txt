set(SOURCES
        "utils/LogicUtils.cpp"
        "utils/RelationUtils.cpp"
        "utils/TemplateParamsUtils.cpp"
        "exception/ActionCancelledException.cpp"
)

set(HEADERS
        "utils/Macros.hpp"
        "utils/LogicUtils.hpp"
        "utils/RelationUtils.hpp"
        "utils/TemplateParamsUtils.hpp"
        "keynodes/Keynodes.hpp"
        "exception/ActionCancelledException.hpp"
)
include_directories(
        ${CMAKE_CURRENT_LIST_DIR}
        ${SC_MEMORY_SRC}
        ${SC_KPM_SRC}
        ${SC_TOOLS_SRC})

add_library(common-ps-lib SHARED ${SOURCES} ${HEADERS})

if (${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(common-ps-lib)
endif ()

target_link_libraries(common-ps-lib sc-memory sc-agents-common)

# if (${SC_BUILD_TESTS})
# 	include(${CMAKE_CURRENT_LIST_DIR}/test/tests.cmake)
# endif ()

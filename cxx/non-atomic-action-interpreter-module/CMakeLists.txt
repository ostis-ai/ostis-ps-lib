set(SOURCES
        "NonAtomicActionInterpreterModule.cpp"
        "agent/NonAtomicActionInterpreterAgent.cpp"
        "constants/NonAtomicActionInterpreterConstants.cpp"
        "interpreter/NonAtomicActionInterpreter.cpp"
)

set(HEADERS
        "NonAtomicActionInterpreterModule.hpp"
        "agent/NonAtomicActionInterpreterAgent.hpp"
        "constants/NonAtomicActionInterpreterConstants.hpp"
        "interpreter/NonAtomicActionInterpreter.hpp"
        "keynodes/NonAtomicKeynodes.hpp"
)

add_library(non_atomic_action_interpreter SHARED ${SOURCES} ${HEADERS})

if (${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(non_atomic_action_interpreter)
endif ()

target_link_libraries(non_atomic_action_interpreter common-ps-lib sc-memory sc-agents-common)
target_include_directories(non_atomic_action_interpreter
        PUBLIC ${CMAKE_CURRENT_LIST_DIR}
        PUBLIC ${SC_MEMORY_SRC}
        PUBLIC ${SC_KPM_SRC}
        PUBLIC ${SC_TOOLS_SRC}
        PUBLIC ${COMMON_MODULE_PATH})

if (${SC_BUILD_TESTS})
    include(${CMAKE_CURRENT_LIST_DIR}/test/tests.cmake)
endif ()

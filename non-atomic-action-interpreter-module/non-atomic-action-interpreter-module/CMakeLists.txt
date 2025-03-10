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

add_library(non-atomic-action-interpreter-module SHARED ${SOURCES} ${HEADERS})
target_link_libraries(non-atomic-action-interpreter-module
    LINK_PUBLIC ps-common-lib::common-utils
)
target_include_directories(non-atomic-action-interpreter-module
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(non-atomic-action-interpreter-module
    PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSIONS_DIRECTORY}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(non-atomic-action-interpreter-module)
endif()

if(${SC_BUILD_TESTS})
    set(NON_ATOMIC_ACTION_INTERPRETER_MODULE_SRC ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(test)
endif()

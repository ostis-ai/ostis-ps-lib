file(GLOB NON_ATOMIC_ACTION_INTERPRETER_TEST_AGENTS
    "agent/*.cpp"
    "agent/*.hpp"
    "keynodes/*.hpp"
)

add_library(non-atomic-action-interpreter-module-test-agents SHARED ${NON_ATOMIC_ACTION_INTERPRETER_TEST_AGENTS})
target_link_libraries(non-atomic-action-interpreter-module-test-agents
     LINK_PUBLIC non-atomic-action-interpreter-module
)
target_include_directories(non-atomic-action-interpreter-module-test-agents
     PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
     NAME non-atomic-action-interpreter-module-tests
     DEPENDS non-atomic-action-interpreter-module-test-agents sc-machine::sc-builder-lib
     INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${NON_ATOMIC_ACTION_INTERPRETER_MODULE_SRC}
)

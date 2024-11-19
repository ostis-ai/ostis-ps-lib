file(GLOB NON_ATOMIC_ACTION_INTERPRETER_TEST_AGENTS
        "${CMAKE_CURRENT_LIST_DIR}/agent/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/agent/*.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/keynodes/*.hpp")
add_library(non_atomic_action_interpreter_test_agents STATIC ${NON_ATOMIC_ACTION_INTERPRETER_TEST_AGENTS})
target_link_libraries(
        non_atomic_action_interpreter_test_agents
        non_atomic_action_interpreter
        sc-agents-common)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
        NAME non_atomic_action_interpreter_test_starter
        DEPENDS non_atomic_action_interpreter_test_agents sc-builder-lib sc-core
        INCLUDES ${SC_MEMORY_SRC})

add_definitions(-DNON_ATOMIC_ACTION_INTERPRETER_MODULE_TEST_SRC_PATH="${CMAKE_CURRENT_LIST_DIR}")

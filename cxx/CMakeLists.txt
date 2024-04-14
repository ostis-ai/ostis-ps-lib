set(COMMON_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/common-module CACHE STRING "COMMON_MODULE_PATH")
set(NON_ATOMIC_ACTION_INTERPRETER_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/non-atomic-action-interpreter-module)

add_subdirectory(non-atomic-action-interpreter-module)
add_subdirectory(common-module)

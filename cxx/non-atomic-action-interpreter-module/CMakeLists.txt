set(SOURCES
	"NonAtomicActionInterpreterModule.cpp"
	"agent/NonAtomicActionInterpreterAgent.cpp"
	"constants/NonAtomicActionInterpreterConstants.cpp"
	"exception/ActionCancelledException.cpp"
	"interpreter/NonAtomicActionInterpreter.cpp"
	"keynodes/Keynodes.cpp"
	"manager/LogicFormulaManager.cpp"
	"utils/ActionUtils.cpp"
	"utils/RelationUtils.cpp"
	"utils/TemplateParamsUtils.cpp"
)

set(HEADERS
	"NonAtomicActionInterpreterModule.hpp"
	"agent/NonAtomicActionInterpreterAgent.hpp"
	"constants/NonAtomicActionInterpreterConstants.hpp"
	"exception/ActionCancelledException.hpp"
	"interpreter/NonAtomicActionInterpreter.hpp"
	"keynodes/Keynodes.hpp"
	"manager/LogicFormulaManager.hpp"
	"utils/Macros.hpp"
	"utils/ActionUtils.hpp"
	"utils/RelationUtils.hpp"
	"utils/TemplateParamsUtils.hpp"
)
include_directories(
		${CMAKE_CURRENT_LIST_DIR}
		${SC_MEMORY_SRC}
		${SC_KPM_SRC}
		${SC_TOOLS_SRC})

add_library(non_atomic_action_interpreter SHARED ${SOURCES} ${HEADERS})

if(${SC_CLANG_FORMAT_CODE})
	target_clangformat_setup(non_atomic_action_interpreter)
endif()

target_link_libraries(non_atomic_action_interpreter sc-memory sc-agents-common)

sc_codegen_ex(non_atomic_action_interpreter ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/generated)

if (${SC_BUILD_TESTS})
	include(${CMAKE_CURRENT_LIST_DIR}/test/tests.cmake)
endif ()

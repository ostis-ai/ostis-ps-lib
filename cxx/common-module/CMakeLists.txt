set(SOURCES
	"CommonModule.cpp"
	"utils/ActionUtils.cpp"
	"utils/LogicUtils.cpp"
	"utils/RelationUtils.cpp"
	"utils/TemplateParamsUtils.cpp"
	"keynodes/Keynodes.cpp"
	"exception/ActionCancelledException.cpp"
)

set(HEADERS
	"CommonModule.hpp"
	"utils/Macros.hpp"
	"utils/ActionUtils.hpp"
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

if(${SC_CLANG_FORMAT_CODE})
	target_clangformat_setup(common-ps-lib)
endif()

target_link_libraries(common-ps-lib sc-memory sc-agents-common)

sc_codegen_ex(common-ps-lib ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/generated)

# if (${SC_BUILD_TESTS})
# 	include(${CMAKE_CURRENT_LIST_DIR}/test/tests.cmake)
# endif ()

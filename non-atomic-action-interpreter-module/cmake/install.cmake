install(TARGETS
    non-atomic-action-interpreter-module
    EXPORT non-atomic-action-interpreter-module-export
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/extensions"
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

export(EXPORT non-atomic-action-interpreter-module-export
        NAMESPACE non-atomic-action-interpreter-module::  # to simulate a different name and see it works
        FILE "${CMAKE_CURRENT_BINARY_DIR}/non-atomic-action-interpreter-module-targets.cmake"
)

install(EXPORT non-atomic-action-interpreter-module-export
    FILE non-atomic-action-interpreter-module-targets.cmake
    NAMESPACE non-atomic-action-interpreter-module::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/non-atomic-action-interpreter-module
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/non-atomic-action-interpreter-module-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${NON_ATOMIC_ACTION_INTERPRETER_ROOT}/cmake/non-atomic-action-interpreter-module-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/non-atomic-action-interpreter-module-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/non-atomic-action-interpreter-module
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/non-atomic-action-interpreter-module-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/non-atomic-action-interpreter-module-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/non-atomic-action-interpreter-module
)

set(CPACK_PACKAGE_NAME                  non-atomic-action-interpreter-module)
set(CPACK_PACKAGE_VENDOR                "OSTIS AI")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY   "Non-atomic action interpreter")
set(CPACK_PACKAGE_INSTALL_DIRECTORY     ${CPACK_PACKAGE_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR         ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR         ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH         ${PROJECT_VERSION_PATCH})
set(CPACK_VERBATIM_VARIABLES            TRUE)

if (WIN32)
    set(CPACK_GENERATOR     ZIP)
else()
    set(CPACK_GENERATOR     TGZ)
endif()
include(CPack)

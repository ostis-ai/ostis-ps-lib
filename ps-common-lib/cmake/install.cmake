install(TARGETS 
    common-utils
    EXPORT ps-common-lib-export
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

export(EXPORT ps-common-lib-export
    NAMESPACE ps-common-lib::  # to simulate a different name and see it works
    FILE "${CMAKE_CURRENT_BINARY_DIR}/ps-common-lib-targets.cmake"
)

install(EXPORT ps-common-lib-export 
    FILE ps-common-lib-targets.cmake
    NAMESPACE ps-common-lib::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ps-common-lib
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ps-common-lib-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${COMMON_ROOT}/cmake/ps-common-lib-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/ps-common-lib-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ps-common-lib
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ps-common-lib-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ps-common-lib-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ps-common-lib
)

set(CPACK_PACKAGE_NAME                  ps-common-lib)
set(CPACK_PACKAGE_VENDOR                "OSTIS AI")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY   "Library of common classes and functions")
set(CPACK_PACKAGE_INSTALL_DIRECTORY     ${CPACK_PACKAGE_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR         ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR         ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH         ${PROJECT_VERSION_PATCH})
set(CPACK_VERBATIM_VARIABLES            TRUE)

if (${WIN32})
    set(CPACK_GENERATOR     ZIP)
else()
    set(CPACK_GENERATOR     TGZ)
endif()
include(CPack)

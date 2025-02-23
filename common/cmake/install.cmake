install(TARGETS 
    common-utils
    EXPORT commonExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

export(EXPORT commonExport
    NAMESPACE common::  # to simulate a different name and see it works
    FILE "${CMAKE_CURRENT_BINARY_DIR}/commonTargets.cmake"
)

install(EXPORT commonExport 
    FILE commonTargets.cmake
    NAMESPACE common::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/common
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/common-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${COMMON_ROOT}/cmake/common-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/common-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/common
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/common-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/common-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/common
)

set(CPACK_PACKAGE_NAME                  common)
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

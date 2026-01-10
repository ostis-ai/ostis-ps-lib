install(TARGETS 
    fixed-search-strategy-template-object fixed-search-strategy-template
    EXPORT fixed-search-strategy-template-processing-libExport 
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(TARGETS
    fixed-search-strategy-template-processing-module
    EXPORT privateExport
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/extensions"
)

export(EXPORT fixed-search-strategy-template-processing-libExport 
    NAMESPACE fixed-search-strategy-template-processing-lib::  # to simulate a different name and see it works
    FILE "${CMAKE_CURRENT_BINARY_DIR}/fixed-search-strategy-template-processing-libTargets.cmake"
)

install(EXPORT fixed-search-strategy-template-processing-libExport
    FILE fixed-search-strategy-template-processing-libTargets.cmake
    NAMESPACE fixed-search-strategy-template-processing-lib::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/fixed-search-strategy-template-processing-lib
)

install(EXPORT privateExport
    FILE privateTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/fixed-search-strategy-template-processing-lib
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/fixed-search-strategy-template-processing-lib-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_MODULE_PATH}/fixed-search-strategy-template-processing-lib-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/fixed-search-strategy-template-processing-lib-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/fixed-search-strategy-template-processing-lib
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/fixed-search-strategy-template-processing-lib-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/fixed-search-strategy-template-processing-lib-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/fixed-search-strategy-template-processing-lib
)

set(CPACK_PACKAGE_NAME                  fixed-search-strategy-template-processing-lib)
set(CPACK_PACKAGE_VENDOR                "OSTIS AI")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY   "Fixed search strategy template processing library for ostis-systems")
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

#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "common::common-utils" for configuration "Release"
set_property(TARGET common::common-utils APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(common::common-utils PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libcommon-utils.dylib"
  IMPORTED_SONAME_RELEASE "@rpath/libcommon-utils.dylib"
  )

list(APPEND _cmake_import_check_targets common::common-utils )
list(APPEND _cmake_import_check_files_for_common::common-utils "${_IMPORT_PREFIX}/lib/libcommon-utils.dylib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

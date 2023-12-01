#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Poco::ActiveRecord" for configuration "RelWithDebInfo"
set_property(TARGET Poco::ActiveRecord APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(Poco::ActiveRecord PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libPocoActiveRecord.so.95"
  IMPORTED_SONAME_RELWITHDEBINFO "libPocoActiveRecord.so.95"
  )

list(APPEND _IMPORT_CHECK_TARGETS Poco::ActiveRecord )
list(APPEND _IMPORT_CHECK_FILES_FOR_Poco::ActiveRecord "${_IMPORT_PREFIX}/lib/libPocoActiveRecord.so.95" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

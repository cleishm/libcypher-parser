# - Find check
# Find the native check headers and libraries.
#
# CHECK_INCLUDE_DIRS	- where to find check.h, etc.
# CHECK_LIBRARIES	- List of libraries when using check.
# CHECK_FOUND	- True if check has been found.

# Look for the header file.
FIND_PATH(CHECK_INCLUDE_DIR check.h)

# Look for the library.
FIND_LIBRARY(CHECK_LIBRARY NAMES check)
FIND_LIBRARY(COMPAT_LIBRARY NAMES compat)

# Handle the QUIETLY and REQUIRED arguments and set CHECK_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CHECK DEFAULT_MSG CHECK_LIBRARY CHECK_INCLUDE_DIR)

# Copy the results to the output variables.
IF(CHECK_FOUND)
    SET(CHECK_LIBRARIES ${CHECK_LIBRARY} ${COMPAT_LIBRARY})
    SET(CHECK_INCLUDE_DIRS ${CHECK_INCLUDE_DIR})
ELSE(CHECK_FOUND)
    SET(CHECK_LIBRARIES)
    SET(CHECK_INCLUDE_DIRS)
ENDIF(CHECK_FOUND)

MARK_AS_ADVANCED(CHECK_INCLUDE_DIRS CHECK_LIBRARIES)

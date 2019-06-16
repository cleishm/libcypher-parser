# - Find fmem
# Find the native fmem headers and libraries.
#
# FMEM_INCLUDE_DIRS	- where to find fmem.h, etc.
# FMEM_LIBRARIES	- List of libraries when using fmem.
# FMEM_FOUND	- True if fmem has been found.

# Look for the header file.
FIND_PATH(FMEM_INCLUDE_DIR fmem.h)

# Look for the library.
FIND_LIBRARY(FMEM_LIBRARY NAMES fmem)

# Handle the QUIETLY and REQUIRED arguments and set FMEM_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FMEM DEFAULT_MSG FMEM_LIBRARY FMEM_INCLUDE_DIR)

# Copy the results to the output variables.
IF(FMEM_FOUND)
    SET(FMEM_LIBRARIES ${FMEM_LIBRARY})
    SET(FMEM_INCLUDE_DIRS ${FMEM_INCLUDE_DIR})
ELSE(FMEM_FOUND)
    SET(FMEM_LIBRARIES)
    SET(FMEM_INCLUDE_DIRS)
ENDIF(FMEM_FOUND)

MARK_AS_ADVANCED(FMEM_INCLUDE_DIRS FMEM_LIBRARIES)

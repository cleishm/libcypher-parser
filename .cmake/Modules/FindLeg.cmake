# - FindLeg leg
# Find the native leg program.
#
# LEG_PROGRAMS - List of libraries when using fmem.
# LEG_FOUND	- True if fmem has been found.

# Look for the library.
FIND_PROGRAM(LEG_PROGRAM NAMES leg)

# Handle the QUIETLY and REQUIRED arguments and set LEG_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LEG DEFAULT_MSG LEG_PROGRAM)

# Copy the results to the output variables.
IF(LEG_FOUND)
    SET(LEG_PROGRAMS ${LEG_PROGRAM})
ELSE(LEG_FOUND)
    SET(LEG_PROGRAMS)
ENDIF(LEG_FOUND)

MARK_AS_ADVANCED(LEG_PROGRAMS)

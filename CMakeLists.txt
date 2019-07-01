cmake_minimum_required (VERSION 3.5)

project (libcypher-parser C)

include (CheckCCompilerFlag)
include (CheckSymbolExists)
include (CheckCSourceCompiles)
include (GNUInstallDirs)
include (CMakeParseArguments)
include (CheckFunctionExists)
include (CheckIncludeFile)

set (PACKAGE "libcypher-parser")
set (PACKAGE_MAJOR_VERSION 0)
set (PACKAGE_MINOR_VERSION 6)
set (PACKAGE_PATCH_VERSION 2)
set (VERSION "${PACKAGE_MAJOR_VERSION}.${PACKAGE_MINOR_VERSION}.${PACKAGE_PATCH_VERSION}")
set (PACKAGE_DEVELOPMENT_VERSION "devel")
set (PACKAGE_NAME "${PACKAGE}")
set (PACKAGE_TARNAME "${PACKAGE}")
set (PACKAGE_VERSION "${VERSION}")
set (PACKAGE_STATUS_VERSION 1)
set (PACKAGE_STRING "${PACKAGE} $VERSION}")
set (PACKAGE_URL "https://github.com/cleishm/libcypher-parser")
set (PACKAGE_BUGREPORT "https://github.com/cleishm/libcypher-parser/issues")

set (CYPHER_PARSER_VERSION "${VERSION}")
set (CYPHER_PARSER_MAJOR_VERSION "${PACKAGE_MAJOR_VERSION}")
set (CYPHER_PARSER_MINOR_VERSION "${PACKAGE_MINOR_VERSION}")
set (CYPHER_PARSER_PATCH_VERSION "${PACKAGE_PATCH_VERSION}")
set (CYPHER_PARSER_DEVELOPMENT_VERSION "${PACKAGE_DEVELOPMENT_VERSION}")

check_include_file (dlfcn.h HAVE_DLFCN_H)
check_include_file (endian.h HAVE_ENDIAN_H)
check_include_file (inttypes.h HAVE_INTTYPES_H)
check_include_file (libkern/OSByteOrder.h HAVE_LIBKERN_OSBYTEORDER_H)
check_include_file (memory.h HAVE_MEMORY_H)
check_include_file (stdbool.h HAVE_STDBOOL_H)
check_include_file (stdint.h HAVE_STDINT_H)
check_include_file (stdlib.h HAVE_STDLIB_H)
check_include_file (strings.h HAVE_STRINGS_H)
check_include_file (string.h HAVE_STRING_H)
check_include_file (sys/endian.h HAVE_SYS_ENDIAN_H)
check_include_file (sys/stat.h HAVE_SYS_STAT_H)
check_include_file (sys/types.h HAVE_SYS_TYPES_H)
check_include_file (unistd.h HAVE_UNISTD_H)
check_include_file (stddef.h HAVE_STDDEF_H)

check_symbol_exists (strerror_r string.h HAVE_STRERROR_R)
check_symbol_exists (open_memstream stdio.h HAVE_OPEN_MEMSTREAM)

if (HAVE_STDLIB_H AND HAVE_STDDEF_H)
 set (STDC_HEADERS 1)
endif ()

if (HAVE_STRERROR_R)
  set (HAVE_DECL_STRERROR_R 1)
endif (HAVE_STRERROR_R)

find_package (Threads)
set (HAVE_PTHREADS ${CMAKE_USE_PTHREADS_INIT})

set (LT_OBJDIR ".libs/")
set (NDEBUG 1)

check_c_source_compiles ("void fun(int n) { 
  int arr[n];
}  
int main() { 
   fun(6);
   return 0;
}" HAVE_C_VARARRAYS )

check_c_source_compiles ("#include <string.h>
int main() {
  char * c;
  c = strerror_r(0,c,0);
  return 0;
}" STRERROR_R_CHAR_P )

check_c_source_compiles ("int main() {
  _Bool b = 1;
  return 0;
}" HAVE__BOOL )

check_c_source_compiles ("#include <sys/types.h>
int main() {
  size_t b = 1;
  return 0;
}" HAVE_SIZE_T )

if (NOT HAVE_SIZE_T)
 set (size_t "unsigned int")
endif ()

check_c_source_compiles ("#include <sys/types.h>
int main() {
  ssize_t b = 1;
  return 0;
}" HAVE_SSIZE_T )

if (NOT HAVE_SSIZE_T)
 set (ssize_t "int")
endif ()

check_c_source_compiles ("#include <assert.h>
static_assert(sizeof(int) == 4, \"Code relies on int being exactly 4 bytes\");
int main(void) {
    return 0;
}" HAVE_C_STATIC_ASSERT)

if (NOT HAVE_C_STATIC_ASSERT)
  set ("static_assert" "typedef void _no_static_assert")
endif ()

foreach (KEYWORD "inline" "__inline__" "__inline")
  if (NOT DEFINED C_INLINE)
    set (CMAKE_REQUIRED_DEFINITIONS "-Dinline=${KEYWORD}")

    check_c_source_compiles ("typedef int foo_t;
      static inline foo_t static_foo(){return 0;}
      foo_t foo(){return 0;}
      int main(int argc, char *argv[]){return 0;}" 
      C_HAS_${KEYWORD} )
    set (CMAKE_REQUIRED_DEFINITIONS)

    if (C_HAS_${KEYWORD})
      set (C_INLINE TRUE)
      if (NOT C_HAS_inline)
        set (inline ${KEYWORD})
      endif ()
    endif (C_HAS_${KEYWORD})
  endif (NOT DEFINED C_INLINE)
endforeach (KEYWORD)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/.cmake/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/lib/src/cypher-parser.h.in ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/cypher-parser.h)

set (MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.cmake/Modules")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${MODULE_DIR})

check_c_compiler_flag ("-fPIC" CC_HAVE_FPIC)
check_c_compiler_flag ("-fvisibility=hidden" CC_HAVE_VISIBILITY)

# Linker flags are ignored by check_c_compiler_flag, so we have
# to use this workaround
set (CMAKE_REQUIRED_FLAGS "-Wl,--exclude-libs=ALL")
check_c_compiler_flag ("" LINKER_HAS_EXCLUDE_LIBS)
unset (CMAKE_REQUIRED_FLAGS)

if (LINKER_HAS_EXCLUDE_LIBS)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--exclude-libs=ALL")
endif (LINKER_HAS_EXCLUDE_LIBS)

if (CC_HAVE_FPIC)
  set (PIC_C_FLAGS "-fPIC")
endif (CC_HAVE_FPIC)

if (CC_HAVE_VISIBILITY)
  set (VISI_C_FLAGS "-fvisibility=hidden")
endif (CC_HAVE_VISIBILITY)

function (cr_link_libraries _TARGET)
  set (multiValueArgs IF)
  cmake_parse_arguments (ARGS "" "" "${multiValueArgs}" ${ARGN})
  if (ARGS_IF)
    if (${ARGS_IF})
    else (${ARGS_IF})
      return ()
    endif (${ARGS_IF})
  endif (ARGS_IF)
  target_link_libraries (${_TARGET} ${ARGS_UNPARSED_ARGUMENTS})
endfunction (cr_link_libraries)

find_package (check)
find_package (fmem)
find_package (leg)

list (APPEND CMAKE_REQUIRED_DEFINITIONS
  -D_GNU_SOURCE
  -D_CRT_RAND_S
  -DVC_EXTRALEAN
  -DWIN32_LEAN_AND_MEAN)

function (extract_make_variable filename varname outvar prefix)
  file (STRINGS ${filename} contents)
  foreach (line IN LISTS contents)
    if (line MATCHES "^${varname}*")
      list (REMOVE_AT line 0)
      foreach (item IN LISTS line)
        string (STRIP ${item} item)
        list (APPEND outvar ${prefix}${item})
      endforeach (item)
    endif (line MATCHES "^${varname}*")
  endforeach (line)
  set(${outvar} PARENT_SCOPE)
endfunction (extract_make_variable)

extract_make_variable (lib/src/Makefile.am libcypher_parser_la_SOURCES SOURCES lib/src/)
add_library (libcypher-parser ${SOURCES})
target_compile_options (libcypher-parser PUBLIC /wd4267 /wd4244 /wd4047)

extract_make_variable (lib/test/Makefile.am check_libcypher_parser_CHECKS CHECKS lib/test/)
extract_make_variable (lib/test/Makefile.am check_libcypher_parser_SOURCES TESTS lib/test/)
list (REMOVE_ITEM TESTS "lib/test/\${check_libcypher_parser_CHECKS}")
list (APPEND TESTS ${CHECKS})

foreach (item IN LISTS CHECKS)
  string (REPLACE "check_" "" item ${item})
  get_filename_component(name ${item} NAME_WE)
  list (APPEND cases "TCase* ${name}_tcase(void)")
  list (APPEND add_cases "suite_add_tcase(s, ${name}_tcase())")
endforeach (item)

list (JOIN cases ";\n" cases)
list (JOIN add_cases ";\n    " add_cases)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/.cmake/check_libcypher-parser_suite.c.in 
                ${CMAKE_CURRENT_SOURCE_DIR}/lib/test/check_libcypher-parser_suite.c)

add_executable (tests ${TESTS})
target_link_libraries (tests libcypher-parser)

if (LEG_FOUND)
    add_custom_command (
        TARGET libcypher-parser
        PRE_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/src
        COMMAND ${LEG_PROGRAMS} parser.leg > parser_leg.c)
    add_custom_command (
        TARGET libcypher-parser
        PRE_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/src
        COMMAND ${LEG_PROGRAMS} quick_parser.leg > quick_parser_leg.c)
endif (LEG_FOUND)

if (CHECK_FOUND AND FMEM_FOUND)
  target_include_directories (tests PUBLIC ${FMEM_INCLUDE_DIRS} ${CHECK_INCLUDE_DIRS} ${CHECK_INCLUDE_DIRS}/..)
  target_link_libraries (tests ${CHECK_LIBRARIES} ${FMEM_LIBRARIES})
endif (CHECK_FOUND AND FMEM_FOUND)

if (HAVE_PTHREADS)
  target_link_libraries (libcypher-parser ${CMAKE_THREAD_LIBS_INIT})
endif (HAVE_PTHREADS)

install (TARGETS libcypher-parser tests
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

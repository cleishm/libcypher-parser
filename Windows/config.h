/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to 1 if C supports variable-length arrays. */
#define HAVE_C_VARARRAYS 1

/* Define to 1 if you have the declaration of `strerror_r', and to 0 if you
   don't. */
#define HAVE_DECL_STRERROR_R 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <endian.h> header file. */
/* #undef HAVE_ENDIAN_H */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <libkern/OSByteOrder.h> header file. */
/* #undef HAVE_LIBKERN_OSBYTEORDER_H */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `open_memstream' function. */
/* #undef HAVE_OPEN_MEMSTREAM */

/* Define to 1 if you have pthreads. */
/* #undef HAVE_PTHREADS */

/* Have PTHREAD_PRIO_INHERIT. */
/* #undef HAVE_PTHREAD_PRIO_INHERIT */

/* Define to 1 if stdbool.h conforms to C99. */
#define HAVE_STDBOOL_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strerror_r' function. */
#define HAVE_STRERROR_R 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/endian.h> header file. */
/* #undef HAVE_SYS_ENDIAN_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if the system has the type `_Bool'. */
#define HAVE__BOOL 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Define to 1 to disable assertions. */
/* #undef NDEBUG */

/* Name of package */
#define PACKAGE "libcypher-parser"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the development version of this package */
#define PACKAGE_DEVELOPMENT_VERSION "devel"

/* Define to the major version of this package */
#define PACKAGE_MAJOR_VERSION 0

/* Define to the minor version of this package */
#define PACKAGE_MINOR_VERSION 6

/* Define to the full name of this package. */
#define PACKAGE_NAME "libcypher-parser"

/* Define to the patch version of this package */
#define PACKAGE_PATCH_VERSION 1

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libcypher-parser 1.9.9"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libcypher-parser"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.9.9"

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if strerror_r returns char *. */
/* #undef STRERROR_R_CHAR_P */

/* If the compiler supports a thread local storage class define it to that
   here */
#define THREAD_LOCAL __thread

/* Version number of package */
#define VERSION "1.9.9"

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef ssize_t */

/* Substitute for static_assert */
#define static_assert(c,m) typedef void _no_static_assert

/* Defines for Windows */
#define ssize_t size_t
#define _CRT_SECURE_NO_WARNINGS 1
#define sigjmp_buf jmp_buf
#define sigsetjmp(x, y) setjmp(x)
#define siglongjmp(x, y) longjmp(x, y)
#define restrict

#pragma warning(disable: 4068)
#pragma warning(disable: 4090)
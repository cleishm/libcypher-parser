bin_PROGRAMS = cypher-lint

cypher_lint_SOURCES = cypher-lint.c
cypher_lint_CPPFLAGS = -I$(top_srcdir)/src/lib
cypher_lint_LDADD = $(top_builddir)/src/lib/libcypher-parser.la ${LIBEDIT_LIBS}

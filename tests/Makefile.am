TESTS = check_libcypher-parser
check_PROGRAMS = check_libcypher-parser
check_libcypher_parser_SOURCES = \
	${check_libcypher_parser_CHECKS} \
	check_libcypher-parser.c \
	check_libcypher-parser_suite.c \
	memstream.c \
	memstream.h

check_libcypher_parser_CHECKS = \
	check_command.c \
	check_constraints.c \
	check_error_tracking.c \
	check_indexes.c \
	check_query.c \
	check_statement.c \
	check_util.c

check_libcypher-parser_suite.c: ${check_libcypher_parser_CHECKS}
	@(names=`for check in $^; do \
		basename $$check .c | sed -e 's/^check_//'; \
	done`; \
	echo "#include <check.h>"; \
	echo; \
	for name in $$names; do \
		echo "TCase* $${name}_tcase(void);"; \
	done; \
	echo; \
	echo "Suite *libcypher_parser_suite(void)"; \
	echo "{"; \
	echo "    Suite *s = suite_create(\"libcypher-parser\");"; \
	for name in $$names; do \
		echo "    suite_add_tcase(s, $${name}_tcase());"; \
	done; \
	echo "    return s;"; \
	echo "}") > $@

CLEANFILES = check_libcypher-parser_suite.c

check_libcypher_parser_CFLAGS = @CHECK_CFLAGS@
check_libcypher_parser_LDFLAGS = -static
check_libcypher_parser_LDADD = \
	$(top_builddir)/src/lib/libcypher-parser.la @CHECK_LIBS@

MAINTAINERCLEANFILES = check_libcypher-parser_suite.c

generated_bindings = getters.py node_types.c operators.c props.c

.NOTPARALLEL: $(generated_bindings)
$(generated_bindings): $(top_srcdir)/lib/src/cypher-parser.h.in
	$(PYTHON) $(top_srcdir)/build-aux/pycypher/generate-bindings.py < $<

pkgpyexec_LTLIBRARIES = pycypher.la
pycypher_la_SOURCES = \
	bindings.c \
	extract_props.c \
	extract_props.h \
	node_types.h \
	operators.h \
	parser.c \
	parser.h \
	props.h \
	table_utils.h
nodist_pycypher_la_SOURCES = \
	operators.c \
	node_types.c \
	props.c

EXTRA_DIST = __init__.py ast.py decorators.py setup.py

PYTHON_PREFIX = `$(PYTHON) -c 'import sys ; print sys.prefix'`
PYTHON_LIBS="-lpython$PYTHON_VERSION"

pycypher_la_CPPFLAGS = -I$(PYTHON_PREFIX)/include/python$(PYTHON_VERSION) -I$(top_srcdir)/lib/src
pycypher_la_LDFLAGS = -avoid-version -module -lpython$(PYTHON_VERSION) $(top_builddir)/lib/src/libcypher-parser.la


#all-local: .build/pycypher/bindings.so
#
#.build/pycypher/bindings.so: bindings.c node_types.c operators.c props.c extract_props.c parser.c
#	cd $(srcdir) && $(PYTHON) setup.py build_ext \
#		-I$(top_srcdir)/lib/src \
#		-L$(top_builddir)/lib/src/.libs \
#		-b $(abs_builddir)/.build -t $(abs_builddir)/.build \
#		-v
#	cp $(srcdir)/__init__.py $(builddir)/.build/pycypher
#	cp $(srcdir)/ast.py $(builddir)/.build/pycypher
#	cp $(srcdir)/decorators.py $(builddir)/.build/pycypher
#	cp $(builddir)/getters.py $(builddir)/.build/pycypher
#	cp $(srcdir)/setup.py $(builddir)/.build/pycypher
#	cp $(srcdir)/version.py $(builddir)/.build/pycypher

CLEANFILES = $(generated_bindings)

#clean-local:
#	-rm -rf .build

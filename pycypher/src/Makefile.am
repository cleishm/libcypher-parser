if WANT_PYTHON

pkgpython_PYTHON = __init__.py ast.py decorators.py getters.py version.py

generated_bindings = getters.py node_types.c operators.c props.c

.NOTPARALLEL: $(generated_bindings)
$(generated_bindings): $(top_srcdir)/lib/src/cypher-parser.h.in
	$(PYTHON) $(top_srcdir)/build-aux/pycypher/generate-bindings.py < $<

pkgpyexec_LTLIBRARIES = _pycypher.la
_pycypher_la_SOURCES = \
	bindings.c \
	extract_props.c \
	extract_props.h \
	node_types.h \
	operators.h \
	parser.c \
	parser.h \
	props.h \
	table_utils.h
nodist__pycypher_la_SOURCES = \
	node_types.c \
	operators.c \
	props.c

_pycypher_la_CFLAGS = $(PYTHON_CFLAGS) -I$(top_srcdir)/lib/src
_pycypher_la_LIBADD = $(PYTHON_LIBS) $(top_builddir)/lib/src/libcp.la
_pycypher_la_LDFLAGS = -avoid-version -module -shared -export-dynamic

EXTRA_DIST = setup.py

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
endif

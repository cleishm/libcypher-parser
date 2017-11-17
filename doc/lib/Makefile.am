include $(top_srcdir)/build-aux/automake/doxygen.am

doc:
	$(MAKE) doxygen-doc

libcypher-parser.tag: .deps/cypher-parser.h
.deps/cypher-parser.h: $(top_srcdir)/src/lib/cypher-parser.h
	@mkdir -p .deps
	(cat "$<" && sed -ne "s/^.*@fn \(.*\)$$/\1;/p" < "$<") >> "$@"

EXTRA_DIST = doxygen.cfg

clean-local:
	rm -rf .deps

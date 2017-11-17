include build-aux/aminclude.am

SUBDIRS = src
ACLOCAL_AMFLAGS = -I build-aux/m4

man1_MANS = cypher-lint.1

if HAVE_CHECK
SUBDIRS += tests
endif

doc: doxygen-doc

doc/libcypher-parser.tag: src/.doc/cypher-parser.h
src/.doc/cypher-parser.h: src/lib/cypher-parser.h
	@mkdir -p src/.doc
	(cat "$<" && sed -ne "s/^.*@fn \(.*\)$$/\1;/p" < "$<") >> "$@"

EXTRA_DIST = \
	doxygen.cfg \
	build-aux \
	LICENSE \
	cypher-lint.1.in \
	README.md

docker-check:
	@$(MAKE) dist
	@echo "Building docker image..."
	@docker run -t --rm -v $(abs_builddir):/$(PACKAGE) \
		`docker build . -q -f build-aux/build.dockerfile` \
		sh /$(PACKAGE)/build-aux/check-package.sh /$(PACKAGE)/$(DIST_ARCHIVES) $(distdir)

docker-valgrind:
	@$(MAKE) dist
	@echo "Building docker image..."
	@docker run --rm -t -v $(abs_builddir):/$(PACKAGE) \
		`docker build . -q -f build-aux/build.dockerfile` \
		sh /$(PACKAGE)/build-aux/valgrind-package.sh /$(PACKAGE)/$(DIST_ARCHIVES) $(distdir)

clean-local:
	rm -rf doc src/.doc

MAINTAINERCLEANFILES = \
	build-aux/m4/libtool.m4 \
	build-aux/m4/ltoptions.m4 \
	build-aux/m4/ltsugar.m4 \
	build-aux/m4/ltversion.m4 \
	build-aux/m4/lt~obsolete.m4

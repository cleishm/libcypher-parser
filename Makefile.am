include aminclude.am

SUBDIRS = m4 src .
ACLOCAL_AMFLAGS = -I m4

man1_MANS = cypher-lint.1

if HAVE_CHECK
SUBDIRS += tests
endif

doc: doxygen-doc

doc/libcypher-parser.tag: src/.doc/cypher-parser.h
src/.doc/cypher-parser.h: src/lib/cypher-parser.h
	@mkdir -p src/.doc
	(cat "$<" && sed -ne "s/^.*@fn \(.*\)$$/\1;/p" < "$<") >> "$@"

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = cypher-parser.pc

EXTRA_DIST = \
	doxygen.cfg \
	LICENSE \
	cypher-lint.1.in \
	README.md

docker-check:
	$(MAKE) dist
	id=`docker create -i debian-dev \
		sh -c 'cd $(PACKAGE_TARNAME)-$(PACKAGE_VERSION) && ./configure --enable-silent-rules && make check'` && \
	trap "docker rm $$id" EXIT && \
	docker cp - $$id:/ < $(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz && \
	docker start -i $$id

docker-valgrind:
	$(MAKE) dist
	id=`docker create -i debian-dev \
		sh -c 'cd $(PACKAGE_TARNAME)-$(PACKAGE_VERSION) && ./configure --enable-silent-rules --disable-debug && make check && CK_FORK=no valgrind --leak-check=full ./tests/check_libcypher-parser'` && \
	trap "docker rm $$id" EXIT && \
	docker cp - $$id:/ < $(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz && \
	docker start -i $$id

clean-local:
	rm -rf doc src/.doc

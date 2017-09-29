FROM quay.io/pypa/manylinux1_i686

COPY pycypher/utils/install_deps.sh /install_deps.sh
RUN /install_deps.sh

COPY aminclude.am configure.ac cypher-lint.1.in cypher-parser.pc.in Makefile.am /project/
COPY src/ /project/src/
COPY tests/ /project/tests/
COPY m4/ /project/m4/

COPY pycypher/utils/build_c_lib.sh /build_c_lib.sh
RUN /build_c_lib.sh

COPY pycypher/utils/ownership_fixing_wrapper.sh /ownership_fixing_wrapper.sh
ENTRYPOINT ["bash", "/ownership_fixing_wrapper.sh"]

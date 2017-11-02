FROM quay.io/pypa/manylinux1_i686

COPY libcypher-parser-*.tar.gz /

# take the latest from locally built tarballs (bash globs are sorted lexicographically)
RUN bash -c 'for pkg in libcypher-parser-*.tar.gz; do tar -xzf $pkg; done; \
  rm *.tar.gz; \
  for pkg in libcypher-parser-*; do \
  rm -rf libcypher-parser && mv $pkg /libcypher-parser; done; '

RUN cd libcypher-parser \
  && ./configure \
  && make \
  && make install

COPY ci/add_build_tag_to_wheels.py /

ENTRYPOINT []

FROM quay.io/pypa/manylinux1_x86_64

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

RUN echo 'fix_ownership() { \
  user_and_group=$(stat -c "%u:%g" /project/pycypher) ; \
  chown -R ${user_and_group} /project/pycypher ; \
  } ; trap fix_ownership EXIT ; $@' > /ownership_fixing_wrapper.sh

ENTRYPOINT ["bash", "/ownership_fixing_wrapper.sh"]

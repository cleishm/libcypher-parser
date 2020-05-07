FROM cleishm/libcypher-parser-build-env AS build

COPY --chown=build . /src
WORKDIR /src
RUN ./autogen.sh
RUN env LDFLAGS="-static" ./configure
RUN LDFLAGS="-static" make
USER root
RUN make install


FROM debian:buster-slim AS libcypher-parser
COPY --from=build /usr/local/lib/ /usr/local/lib


FROM debian:buster-slim AS cypher-lint
COPY --from=build /usr/local/bin/cypher-lint /usr/local/bin

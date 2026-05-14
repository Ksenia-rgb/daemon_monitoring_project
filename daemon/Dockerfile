FROM cgr.dev/chainguard/gcc-glibc:latest-dev@sha256:f19a3aa5695c0eac2d74c587e3c952a21605efc5b90e7b36e9d158f017cc55a5 AS builder

WORKDIR /app
COPY daemon/src/ daemon/src/
COPY daemon/include/ daemon/include/
COPY libs/ libs/
COPY Makefile .
RUN make build-daemon
RUN mkdir -p /app/config

FROM cgr.dev/chainguard/glibc-dynamic:latest@sha256:c97b5efe4aeb84e438afa743e69ccf2fc4a23ec847f6c3c68efc3edd9fad683c AS production

WORKDIR /app
COPY --from=builder --chown=65532:65532 /app/out/daemon/component /app/bin/daemon
COPY --from=builder --chown=65532:65532 /app/config /app/daemon/config
USER 65532:65532
ENTRYPOINT ["/app/bin/daemon"]
CMD ["/app/daemon/config/config.json"]

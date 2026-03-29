# Runtime Image (`.docker/runtime`)

Minimal production image for deploying compiled C++ artefacts.
Contains **only the runtime shared libraries** — no compiler, no build
tools, no debug utilities.

## Provides

| Component | Detail |
|-----------|--------|
| Base | Ubuntu 24.04 LTS (pinned digest) |
| Libraries | `libstdc++6`, `libgcc-s1`, `libgomp1`, `ca-certificates` |
| Runtime user | `nonroot` (UID/GID 65532) |

## Usage

This image is designed to be the final stage of your application's
multi-stage Dockerfile:

```dockerfile
FROM ci-build:edge AS builder
WORKDIR /workspace
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release \
 && cmake --build build --parallel

FROM ci-runtime:edge
COPY --from=builder --chown=nonroot:nonroot \
     /workspace/build/my-app /app/my-app
ENTRYPOINT ["/app/my-app"]
```

## Extending

If your binary links against additional shared libraries, add them in the
`apt-get install` block of the Dockerfile.  Use `ldd /app/<binary>` inside
the builder stage to identify required shared objects.

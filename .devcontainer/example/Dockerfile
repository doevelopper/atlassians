# FROM ubuntu:26.04
FROM mcr.microsoft.com/devcontainers/cpp:dev-ubuntu24.04
LABEL maintainer="Doevelopper Atlassians Pipelines <rolland.doe@gmail.com>"

# Args
ARG BUILD_DATE
ARG REVISION
ARG BUILD_VERSION
ARG DEBIAN_FRONTEND=noninteractive

# Labels
LABEL \
    org.opencontainers.image.authors="Doevelopper Atlassians Pipelines <rolland.doe@gmail.com>" \
    org.opencontainers.image.created=$BUILD_DATE \
    org.opencontainers.image.title="doevelopper/atlassians/dev-image" \
    org.opencontainers.image.description="Default image for doevelopper atlassians Pipelines" \
    org.opencontainers.image.url="https://github.com/doevelopper/atlassians.git" \
    org.opencontainers.image.revision=$REVISION \
    org.opencontainers.image.vendor="Doevelopper Atlassians Engineering Team" \
    org.opencontainers.image.version=$BUILD_VERSION
    
# Install bazelisk (latest release)
RUN curl -L https://github.com/bazelbuild/bazelisk/releases/latest/download/bazelisk-linux-amd64 \
      -o /usr/local/bin/bazelisk \
    && chmod +x /usr/local/bin/bazelisk \
    && ln -s /usr/local/bin/bazelisk /usr/local/bin/bazel

# Hook direnv into bash (default Codespaces shell)
RUN echo 'eval "$(direnv hook bash)"' >> /etc/bash.bashrc

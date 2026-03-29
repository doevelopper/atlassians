# syntax=docker/dockerfile:1
# hadolint global ignore=DL3006
# =============================================================================
# CI Build Image
# Full C++ build toolchain for CI pipelines.
# Extends ci-base with: GCC 14, Clang 20, CMake, Ninja, Conan, ccache.
# Tool versions are pinned in .docker/ci/tool-versions.env and kept in sync
# with .devcontainer/cpp/Dockerfile.
#
# Build (from repository root):
#   docker build \
#     -f .docker/ci/Dockerfile \
#     --build-arg BUILD_DATE=$(date -u +%Y-%m-%dT%H:%M:%SZ) \
#     --build-arg REVISION=$(git rev-parse HEAD) \
#     --build-arg BUILD_VERSION=$(cat VERSION) \
#     -t ci-build:edge .
# =============================================================================

ARG CI_BASE_IMAGE=ci-base:edge
ARG CLANG_VERSION=20
ARG GCC_VERSION=14
ARG CCACHE_VERSION=4.13.1
ARG CPM_VERSION=0.42.1
# Public minisign key for verifying ccache releases — https://ccache.dev/minisign.pub
ARG CCACHE_MINISIGN_PUBKEY=RWQX7yXbBedVfI4PNx6FLdFXu9GHUFsr28s4BVGxm4BeybtnX3P06saF

# ---------------------------------------------------------------------------
# Architecture-aware download stages (AMD64 & ARM64)
# Checksums are taken from official GitHub release pages.
# ---------------------------------------------------------------------------
FROM scratch AS downloader-amd64
ARG CCACHE_VERSION
ADD --checksum=sha256:dd9fc188e738add3c12509063bb082b05e77a9a71fa85a20e01230044aa410f1 \
    https://github.com/ccache/ccache/releases/download/v${CCACHE_VERSION}/ccache-${CCACHE_VERSION}-linux-x86_64-glibc.tar.xz \
    /ccache.tar.xz
ADD --checksum=sha256:fdf00b1eadebf437e898ca3c0c94fd3e8d03b9e2bbe4f3d74ac6df2fecbf0a74 \
    https://github.com/ccache/ccache/releases/download/v${CCACHE_VERSION}/ccache-${CCACHE_VERSION}-linux-x86_64-glibc.tar.xz.minisig \
    /ccache.tar.xz.minisig

FROM scratch AS downloader-arm64
ARG CCACHE_VERSION
ADD --checksum=sha256:4cf4b05d9c381b3a60f1f10189f45ad9402bbc58979dbdc4901659c7f5e42dc8 \
    https://github.com/ccache/ccache/releases/download/v${CCACHE_VERSION}/ccache-${CCACHE_VERSION}-linux-aarch64-glibc.tar.xz \
    /ccache.tar.xz
ADD --checksum=sha256:24b50ebf8ce5ec9e5e56af298ddb17699a46f0d9bb035d7c824500270a5cde74 \
    https://github.com/ccache/ccache/releases/download/v${CCACHE_VERSION}/ccache-${CCACHE_VERSION}-linux-aarch64-glibc.tar.xz.minisig \
    /ccache.tar.xz.minisig

# trivy:ignore:AVD-DS-0001  (TARGETARCH is a Docker built-in, not a plain ARG)
FROM downloader-${TARGETARCH} AS downloader
ADD --checksum=sha256:8b2a587ffd672c4687e7581dad4b2f6c1bb2ad6b480cd9771ba2ff48e0b8c75d \
    https://apt.llvm.org/llvm-snapshot.gpg.key /llvm.gpg.key

# ---------------------------------------------------------------------------
# Extractor: verify ccache signature and unpack binaries
# ---------------------------------------------------------------------------
FROM ${CI_BASE_IMAGE} AS extractor
ARG CCACHE_MINISIGN_PUBKEY
ARG CCACHE_VERSION

USER root
SHELL ["/bin/bash", "-Eeuo", "pipefail", "-c"]

# hadolint ignore=DL3008
RUN --mount=from=downloader,target=/dl \
    --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update \
 && apt-get install --no-install-recommends -y minisign \
 && minisign -P "${CCACHE_MINISIGN_PUBKEY}" -Vm /dl/ccache.tar.xz \
 && tar xJf /dl/ccache.tar.xz --strip-components=1 \
              "ccache-${CCACHE_VERSION}-linux-$(uname -m)-glibc/ccache" \
 && cp /dl/llvm.gpg.key /llvm.gpg.key

# ---------------------------------------------------------------------------
# Final CI build image
# ---------------------------------------------------------------------------
FROM ${CI_BASE_IMAGE}

ARG CI_USER=ci
ARG CLANG_VERSION=20
ARG GCC_VERSION=14
ARG CPM_VERSION=0.42.1
ARG DEBIAN_FRONTEND=noninteractive
ARG BUILD_DATE
ARG REVISION
ARG BUILD_VERSION

LABEL \
  org.opencontainers.image.authors="Platform Engineering" \
  org.opencontainers.image.created="${BUILD_DATE}" \
  org.opencontainers.image.description="CI build image — C++ toolchain (GCC ${GCC_VERSION} / Clang ${CLANG_VERSION})" \
  org.opencontainers.image.licenses="Apache-2.0" \
  org.opencontainers.image.revision="${REVISION}" \
  org.opencontainers.image.source="https://github.com/doevelopper/atlassians" \
  org.opencontainers.image.title="ci-build" \
  org.opencontainers.image.url="https://github.com/doevelopper/atlassians" \
  org.opencontainers.image.vendor="Your Organization" \
  org.opencontainers.image.version="${BUILD_VERSION}"

HEALTHCHECK NONE

SHELL ["/bin/bash", "-Eeuo", "pipefail", "-c"]

# Build environment — keep in sync with .devcontainer/cpp/Dockerfile ENV section
ENV CCACHE_DIR=/cache/.ccache \
    CMAKE_EXPORT_COMPILE_COMMANDS=On \
    CMAKE_GENERATOR=Ninja \
    CONAN_HOME=/opt/conan \
    CPM_SOURCE_CACHE=/cache/.cpm \
    PYTHONPYCACHEPREFIX=/cache/.python

USER root

# ---------------------------------------------------------------------------
# Install C++ build toolchain (mirrors .devcontainer/cpp/Dockerfile)
# ---------------------------------------------------------------------------
# hadolint ignore=DL3008
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    --mount=from=extractor,target=/src \
    apt-get update \
 && apt-get install -y --no-install-recommends \
      "gcc-${GCC_VERSION}" \
      "g++-${GCC_VERSION}" \
      autoconf \
      automake \
      bison \
      cmake \
      dos2unix \
      doxygen \
      file \
      flex \
      gcovr \
      gdb \
      graphviz \
      lcov \
      libtool \
      libtool-bin \
      make \
      ninja-build \
      pkg-config \
      python3 \
      python3-pip \
      python3-venv \
      rename \
      unzip \
      xsltproc \
      xz-utils \
      zip \
 && python3 -m pip install --break-system-packages --no-cache-dir conan \
 && cp /src/ccache /usr/local/bin/ccache \
 && cat /src/llvm.gpg.key | gpg --dearmor -o /usr/share/keyrings/llvm-snapshot-keyring.gpg \
 && UBUNTU_CODENAME=$(grep '^UBUNTU_CODENAME=' /etc/os-release | cut -d= -f2) \
 && echo "deb [signed-by=/usr/share/keyrings/llvm-snapshot-keyring.gpg] \
      http://apt.llvm.org/${UBUNTU_CODENAME}/ llvm-toolchain-${UBUNTU_CODENAME}-${CLANG_VERSION} main" \
      | tee /etc/apt/sources.list.d/llvm-snapshot.list > /dev/null \
 && echo -e 'Package: *\nPin: origin "apt.llvm.org"\nPin-Priority: 1000' \
      > /etc/apt/preferences \
 && apt-get update \
 && apt-get install -y --no-install-recommends \
      "clang-${CLANG_VERSION}" \
      "clang-format-${CLANG_VERSION}" \
      "clang-tidy-${CLANG_VERSION}" \
      "clangd-${CLANG_VERSION}" \
      "lld-${CLANG_VERSION}" \
      "llvm-${CLANG_VERSION}" \
 && apt-get clean

# ---------------------------------------------------------------------------
# Set up tool alternatives (gcc, clang, python)
# ---------------------------------------------------------------------------
RUN update-alternatives --install /usr/bin/gcc  gcc  "/usr/bin/gcc-${GCC_VERSION}"  10 \
                        --slave   /usr/bin/g++  g++  "/usr/bin/g++-${GCC_VERSION}" \
                        --slave   /usr/bin/gcov gcov "/usr/bin/gcov-${GCC_VERSION}" \
 && update-alternatives --install /usr/bin/cc   cc   "/usr/bin/gcc-${GCC_VERSION}"  10 \
                        --slave   /usr/bin/c++  c++  "/usr/bin/g++-${GCC_VERSION}" \
 && update-alternatives --install /usr/bin/python python /usr/bin/python3 10 \
 && update-alternatives --install /usr/bin/clang        clang        "/usr/bin/clang-${CLANG_VERSION}"        10 \
                        --slave   /usr/bin/clang++       clang++       "/usr/bin/clang++-${CLANG_VERSION}" \
                        --slave   /usr/bin/clang-format  clang-format  "/usr/bin/clang-format-${CLANG_VERSION}" \
                        --slave   /usr/bin/clang-tidy    clang-tidy    "/usr/bin/clang-tidy-${CLANG_VERSION}" \
                        --slave   /usr/bin/lld           lld           "/usr/bin/lld-${CLANG_VERSION}"

# Add LLVM bin-dir to PATH
ENV PATH="/usr/lib/llvm-${CLANG_VERSION}/bin:${PATH}"

# ---------------------------------------------------------------------------
# Install CPM.cmake and configure Conan
# ---------------------------------------------------------------------------
RUN --mount=type=cache,target=/cache,sharing=locked \
    wget --no-hsts -qP /usr/share/cmake-*/Modules/ \
      "https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_VERSION}/CPM.cmake" \
 && install -d /opt/conan \
 && HOME=/root conan profile detect \
 && echo -e "\n[conf]\ntools.cmake.cmaketoolchain:generator=Ninja" \
      >> "$(HOME=/root conan profile path default)" \
 && chown -R "${CI_USER}:${CI_USER}" /opt/conan

# ---------------------------------------------------------------------------
# Copy and register CI entrypoint
# ---------------------------------------------------------------------------
COPY .docker/ci/entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh

USER ${CI_USER}
WORKDIR /workspace

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["bash"]

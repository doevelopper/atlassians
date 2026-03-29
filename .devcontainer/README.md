# Dev Containers

This directory contains multiple **VS Code Dev Container** configurations for
the project.  When you open the repository in VS Code (or GitHub Codespaces),
you will be prompted to choose which configuration to use.

## Configurations

| Folder | Name | Intended use |
|--------|------|--------------|
| *(root)* | C++ Dev (Docker Compose) | Full environment with named cache volumes and optional sidecar services |
| `cpp/` | C++ Dev (standalone) | Fast single-container iteration, same toolchain as CI |
| `base/` | Base Dev (minimal) | Lightweight shell environment for scripts / BATS testing |
| `example/` | Example | Reference template for adding a new devcontainer flavour |

## Architecture

```
.devcontainer/
├── devcontainer.json          ← compose-backed config (this is the default)
├── docker-compose.yml         ← workspace service + optional sidecars
├── scripts/
│   ├── post-create.sh         ← runs once on first container creation
│   ├── post-start.sh          ← runs on every container start
│   └── post-attach.sh         ← runs on every terminal attach (welcome banner)
├── base/                      ← minimal Ubuntu 24.04 devcontainer
│   ├── devcontainer.json
│   ├── Dockerfile
│   └── apt-requirements.json
├── cpp/                       ← full C++ devcontainer (mirrors CI build image)
│   ├── devcontainer.json
│   ├── Dockerfile
│   ├── devcontainer-lock.json
│   ├── devcontainer-metadata-vscode.json
│   ├── apt-requirements-base.json
│   ├── apt-requirements-clang.json
│   ├── requirements.in        ← pip-tools source
│   └── requirements.txt       ← pip-compiled, hash-locked
├── example/                   ← template for new flavours
├── features/                  ← custom devcontainer Features (placeholder)
└── README.md
```

## Coherence with CI images

The `cpp` devcontainer and the `.docker/ci` build image share the same:

- **Ubuntu 24.04 digest** — guarantees identical OS behaviour
- **Tool versions** — pinned in `.docker/ci/tool-versions.env`
- **Conan profile** — both configure `tools.cmake.cmaketoolchain:generator=Ninja`

When you bump a tool version, update **both** places.

## Cache volumes (compose configuration only)

| Volume | Contents |
|--------|----------|
| `ccache` | Compiler cache (speeds up repeat builds) |
| `conan-home` | Conan package indices and installed packages |
| `cpm-cache` | CPM.cmake downloaded packages |
| `python-cache` | Python bytecode cache |

Volumes persist across container rebuilds. Remove them with:

```bash
docker compose -f .devcontainer/docker-compose.yml down -v
```

## Custom CA certificates

Place `.pem` or `.crt` files in `.docker/common/ca-certificates/` and they
will be installed into all images (devcontainer and CI) at build time via
`.docker/common/ca-certificates/install-ca.sh`.

## GPG commit signing

Set the following environment variables on your **host machine** and they
will be forwarded into the container via `docker-compose.yml`:

```bash
export GIT_AUTHOR_NAME="Your Name"
export GIT_AUTHOR_EMAIL="you@example.com"
export GPG_SIGNING_KEY_ID="<key-fingerprint>"
```

Then re-open the dev container to apply.

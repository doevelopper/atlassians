# Dev Containers

This directory contains multiple **VS Code Dev Container** configurations for
the project.  When you open the repository in VS Code (or GitHub Codespaces),
you will be prompted to choose which configuration to use.

## Local quick start

1. Ensure Docker Engine with Compose v2 is available:
	```bash
	docker --version
	docker compose version
	```
2. In VS Code, run **Dev Containers: Reopen in Container**.
3. Choose one of the available configurations from this folder.
4. After attach, run:
	```bash
	scripts/doctor.sh
	make ci
	```

If you prefer CLI preflight checks before opening in VS Code:

```bash
docker compose -f .devcontainer/docker-compose.yml config
```

Run the repeatable smoke check (compose config, build, and post-create sanity):

```bash
scripts/devcontainer-smoke.sh
```

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

If your Node tooling requires an extra trust bundle, set this on your host
before opening the devcontainer:

```bash
export NODE_EXTRA_CA_CERTS=/usr/local/share/ca-certificates/<your-cert>.crt
```

## GPG commit signing

Set the following environment variables on your **host machine** and they
will be forwarded into the container via `docker-compose.yml`:

```bash
export GIT_AUTHOR_NAME="Your Name"
export GIT_AUTHOR_EMAIL="you@example.com"
export GPG_SIGNING_KEY_ID="<key-fingerprint>"
```

Then re-open the dev container to apply.

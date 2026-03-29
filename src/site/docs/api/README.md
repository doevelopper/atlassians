# API Documentation

Generated API reference documentation is placed in this directory by the build system.

## Generation

### Doxygen (C++)

```sh
make docs           # runs Doxygen and outputs HTML into this directory
```

The output is driven by `buildsys/cmake/Doxyfile.in` or `Doxyfile` at the project root.
HTML output lands in `api/html/`, XML (for Breathe/Sphinx integration) in `api/xml/`.

### Sphinx + Breathe (Python narrative docs over C++ API)

```sh
make docs-sphinx    # runs Sphinx and outputs HTML into api/sphinx/
```

## Conventions

| Path | Content |
|---|---|
| `api/html/` | Doxygen HTML output |
| `api/xml/` | Doxygen XML output (for Breathe) |
| `api/sphinx/` | Sphinx HTML output |

These paths are excluded from VCS via `artifacts/.gitignore`.
Commit the configuration files (`Doxyfile.in`, `conf.py`) but not the generated output.

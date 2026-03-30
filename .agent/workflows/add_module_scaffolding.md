---
description: Scaffolding for a new module version in the BCR
---
1. **Create Directory**:
   ```sh
   mkdir -p modules/<name>/<version>
   ```
2. **Initialize Files**: Copy `MODULE.bazel`, `source.json`, and `presubmit.yml` from a previous version.
3. **Update Metadata**: Add the new version to `modules/<name>/metadata.json`.

---
description: Validate and iterate on a module version
---

1. **Validate**:
   ```sh
   bazel run //tools:bcr_validation -- --check <name>@<version>
   ```
2. **Update Integrity**:
   ```sh
   bazel run //tools:update_integrity -- <name> --version=<version>
   ```
3. **Overlay vs Patches**:
   - `overlay/`: Add or overwrite files (requires `bazel_compatibility` in `MODULE.bazel`).
   - `patches/`: Modify existing files.
4. **Consistency**: `modules/.../MODULE.bazel` must match the source archive's version (if exists) exactly. Use an overlay/patch if they differ.

---
description: Verify module build locally
---
1. **Setup**:
   ```sh
   bazel run //tools:setup_presubmit_repos -- --module <name>@<version>
   ```
2. **Test**: Run the build command provided by the setup tool's output.
3. **Troubleshoot**: Use `bazel clean --expunge` if changes are not reflected.

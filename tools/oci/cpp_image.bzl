"cc_image macro for OCI containers"

load("@rules_oci//oci:defs.bzl", "oci_image", "oci_load")

def cc_image(name, package, entrypoint, base = "@distroless_base", repo_tags = None):
    if repo_tags == None:
        repo_tags = ["atlassians-runtime:latest"]

    oci_image(
        name = name,
        base = base,
        tars = [package],
        entrypoint = entrypoint,
    )

    oci_load(
        name = name + ".load",
        image = name,
        repo_tags = repo_tags,
    )
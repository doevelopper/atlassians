"""Targets in the repository root"""

load("@gazelle//:def.bzl", "gazelle")

exports_files(
    [
        ".clang-tidy",
    ],
    visibility = ["//:__subpackages__"],
)

# We prefer BUILD instead of BUILD.bazel
# gazelle:build_file_name BUILD
# gazelle:exclude githooks/*

gazelle(
    name = "gazelle",
    env = {
        "ENABLE_LANGUAGES": ",".join([
            "starlark",
            "cc",
        ]),
    },
    gazelle = "@multitool//tools/gazelle",
)


# load(
#     "//src/main/resources/off_the_shelf_software:soup.bzl",
#     "soup_dependencies"
# )

# soup_dependencies()

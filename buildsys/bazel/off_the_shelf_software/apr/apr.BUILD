# load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")
# load("@rules_foreign_cc//tools/build_defs:configure.bzl", "configure_make")
load("@rules_foreign_cc//foreign_cc:defs.bzl", "configure_make")
package(default_visibility = ["//visibility:public"])

filegroup(
    name = "all",
    srcs = glob(["**"]),
    visibility = ["//visibility:public"]
)

CONFIGURE_OPTIONS = [
    "--enable-threads",
    "--enable-posix-shm",
    "--enable-allocator-guard-pages",
    "--enable-pool-concurrency-check",
    "--enable-other-child",
    #  "--with-pic",
]

configure_make(
    name = "apr",
    configure_command = "configure",

    # configure_env_vars = select({
    #     # "@bazel_tools//platforms:osx": {"AR": ""},
    #     "//conditions:default": {},
    # }),

    configure_in_place = True,

    configure_options = select({
        "@bazel_tools//platforms:osx": [
            "-fPIC",
        ] + CONFIGURE_OPTIONS,
        "@bazel_tools//platforms:linux": [
            # "--prefix=${INSTALLDIR}",
            "-fPIC",
        ] + CONFIGURE_OPTIONS,
        "//conditions:default": [
            "-fPIC",
            "no-shared",
        ] + CONFIGURE_OPTIONS,
    }),

    lib_source = ":all",

    linkopts = select({
        "@bazel_tools//platforms:osx": [
            "-lpthread",
        ],
        "@bazel_tools//platforms:linux": [
            "-ldl",
        ],
        "//conditions:default": [ ],
    }),

    # out_include_dir = "include/apr",
    # out_lib_dir = "lib",

    out_static_libs = select({
        "@bazel_tools//platforms:osx": [
            "libapr-1.a",
        ],
        # considere using "@platforms//os:windows": or @bazel_tools//platforms:windows or "@bazel_tools//src/conditions:windows":
        "@bazel_tools//platforms:windows": [
            "libapr-1.lib",
        ],
        "//conditions:default": [
            "libapr-1.a",
        ],
    }),

    out_shared_libs = select({
        "@bazel_tools//platforms:osx": [
            "libapr-1.dylib",
        ],
        # considere using "@platforms//os:windows": or @bazel_tools//platforms:windows or "@bazel_tools//src/conditions:windows":
        "@bazel_tools//platforms:windows": [
            "libapr-1.lib",
        ],
        "//conditions:default": [
            "libapr-1.so",
            "libapr-1.so.0",
            "libapr-1.so.0.7.0"
        ],
    }),

    visibility = ["//visibility:public"],
)


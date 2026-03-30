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
        "--disable-shared",
    ]

configure_make(
    name = "apr",
    configure_command = "configure",

    configure_in_place = True,

    configure_options = CONFIGURE_OPTIONS,

    env = select({
        "@platforms//os:windows": {},
        "//conditions:default": {"CFLAGS": "-fPIC"},
    }),

    lib_source = ":all",

    linkopts = select({
        "@platforms//os:osx": [
            "-lpthread",
        ],
        "@platforms//os:linux": [
            "-ldl",
        ],
        "//conditions:default": [],
    }),

    out_static_libs = select({
        "@platforms//os:windows": ["libapr-1.lib"],
        "//conditions:default": ["libapr-1.a"],
    }),

    visibility = ["//visibility:public"],
)


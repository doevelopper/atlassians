load("@rules_foreign_cc//foreign_cc:defs.bzl", "configure_make")
package(default_visibility = ["//visibility:public"])

filegroup(
    name = "all",
    srcs = glob(["**"]),
    visibility = ["//visibility:public"]
)

    # "--with-apr=$$EXT_BUILD_DEPS$$/apr",
    # "--with-expat=$$EXT_BUILD_DEPS$$/expat",
    # "-with-apr=../apr-1.7.0"
    # "--with-expat=../libexpat/expat",
    # $EXT_BUILD_ROOT/external/org_apache_apr
# CONFIGURE_OPTIONS = [
#   # "--with-apr=${EXT_BUILD_DEPS}/org_apache_apr",
#   "--with-apr=${EXT_BUILD_DEPS}/apr/",
#   "--with-expat=${EXT_BUILD_DEPS}/expat/",
#   # "--with-crypto",
#   "--with-openssl=${EXT_BUILD_DEPS}/openssl/",
# #   "$$EXT_BUILD_DEPS$$/apr",
# ]


CONFIGURE_OPTIONS = [
    "--with-apr=$$EXT_BUILD_DEPS/apr/bin/apr-1-config",
    "--with-expat=$$EXT_BUILD_DEPS/expat",
    "--with-openssl=$$EXT_BUILD_DEPS/openssl",
    "--with-crypto",
    "--disable-shared",
]

configure_make(
    name = "apr_util",
    configure_command = "configure",

    # configure_env_vars = select({
    #     # "@bazel_tools//platforms:osx": {"AR": ""},
    #     "//conditions:default": {},
    # }),

    configure_in_place = True,

    configure_options = CONFIGURE_OPTIONS,

    env = select({
        "@platforms//os:windows": {},
        "//conditions:default": {
            "AR": "/usr/bin/ar",
            "CFLAGS": "-fPIC",
        },
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

    # out_include_dir = "include/apr",
    # out_lib_dir = "lib",

    out_static_libs = select({
        "@platforms//os:windows": ["libaprutil-1.lib"],
        "//conditions:default": ["libaprutil-1.a"],
    }),

    deps = [
        "@org_apache_apr//:apr",
        # Expat from BCR (bazel_dep name = "libexpat" in MODULE.bazel)
        "@libexpat//:expat",
        "@openssl//:ssl",
        "@openssl//:crypto",
    ],

    visibility = ["//visibility:public"],
)

"""A module defining the third party dependency apr"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def apr_util_repositories():
    # maybe(
    #     http_archive,
    #     name = "org_apache_apr_util",
    #     build_file = Label("@com.github.doevelopper.rules-sdlc//src/main/resources/off_the_shelf_software/apr_util:BUILD.apr_util.bazel"),
    #     sha256 = "b65e40713da57d004123b6319828be7f1273fbc6490e145874ee1177e112c459",
    #     strip_prefix = "apr-util-1.6.1",
    #     urls = [
    #         "https://mirror.bazel.build/www-us.apache.org/dist//apr/apr-util-1.6.1.tar.gz",
    #         "https://www-us.apache.org/dist//apr/apr-util-1.6.1.tar.gz",
    #     ],
    # )

    maybe(
        http_archive,
        name = "org_apache_apr_util",
        build_file = Label("//buildsys/bazel/off_the_shelf_software/apr_util:apr-utils.BUILD"),
        sha256 = "2b74d8932703826862ca305b094eef2983c27b39d5c9414442e9976a9acf1983",
        strip_prefix = "apr-util-1.6.3",
        urls = [
            "https://dlcdn.apache.org//apr/apr-util-1.6.3.tar.gz",
            "https://github.com/apache/apr-util/archive/refs/tags/1.6.3.tar.gz",
        ],
    )

    # Expat is provided by the BCR module bazel_dep(name = "libexpat") in MODULE.bazel.
    # Use @libexpat//:expat in build files instead of this archive.

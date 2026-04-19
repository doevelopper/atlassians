"""A module defining the third party dependency log4cxx"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def log4cxx_repositories():
    maybe(
        http_archive,
        name = "org_apache_log4cxx",
        build_file = Label("//tools/off_the_shelf_software/log4cxx:BUILD.log4cxx.bazel"),
        #sha256 = "eb425ce35a391cf0927356bebb7da53f96c8a7aaf634aaf740e011203c732bb",
        strip_prefix = "apache-log4cxx-1.1.0",
        urls = [
            "https://archive.apache.org/dist/logging/log4cxx/1.1.0/apache-log4cxx-1.1.0.tar.gz",
            "https://github.com/apache/logging-log4cxx/archive/refs/tags/rel/v1.1.0.tar.gz",
        ],
    )

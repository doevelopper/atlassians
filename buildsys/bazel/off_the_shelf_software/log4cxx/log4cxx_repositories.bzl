"""A module defining the third party dependency log4cxx"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def log4cxx_repositories():
    maybe(
        http_archive,
        name = "org_apache_log4cxx",
        build_file = Label("//buildsys/bazel/off_the_shelf_software/log4cxx:BUILD.log4cxx.bazel"),
        sha256 = "6df9f1f682650de6045309473d5b2fe1f798a03ceb36a74a5b21f5520962d32f",
        strip_prefix = "apache-log4cxx-1.1.0",
        urls = [
            "https://archive.apache.org/dist/logging/log4cxx/1.1.0/apache-log4cxx-1.1.0.tar.gz",
            "https://github.com/apache/logging-log4cxx/archive/refs/tags/rel/v1.1.0.tar.gz",
        ],
    )

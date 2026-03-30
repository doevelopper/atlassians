"""Bzlmod module extension for the log4cxx (Apache Logging) repository."""

load("//buildsys/bazel/off_the_shelf_software/log4cxx:log4cxx_repositories.bzl", "log4cxx_repositories")

def _log4cxx_extension_impl(_ctx):
    log4cxx_repositories()

log4cxx_extension = module_extension(
    implementation = _log4cxx_extension_impl,
)

"""Bzlmod module extension for the APR (Apache Portable Runtime) repository."""

load("//tools/off_the_shelf_software/apr:apr_repositories.bzl", "apr_repositories")

def _apr_extension_impl(_ctx):
    apr_repositories()

apr_extension = module_extension(
    implementation = _apr_extension_impl,
)

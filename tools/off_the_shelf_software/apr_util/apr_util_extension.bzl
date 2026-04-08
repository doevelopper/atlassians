"""Bzlmod module extension for the APR-util repository."""

load("//tools/off_the_shelf_software/apr_util:apr_util_repositories.bzl", "apr_util_repositories")

def _apr_util_extension_impl(_ctx):
    apr_util_repositories()

apr_util_extension = module_extension(
    implementation = _apr_util_extension_impl,
)

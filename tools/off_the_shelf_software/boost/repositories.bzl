load("//tools/off_the_shelf_software/boost:boost.bzl", "boost_deps")

def _non_module_dependencies_impl(_ctx):
    boost_deps()

non_module_dependencies = module_extension(
    implementation = _non_module_dependencies_impl,
)


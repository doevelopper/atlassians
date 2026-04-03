"""A module defining the third party dependency curl"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

cucumber_cpp_ver="1bc2e815b42c113e871e7541391202135b6c3d08"
cucumber_cpp_sha256="bf49d68d98d6c576eaf74fd0f428b073a194b0b19427365a5dd93a5fee6c8072"
# cucumber_cpp_ref = "heads"
# cucumber_cpp_ver = "main"

def cucumber_cpp_repositories():

    maybe(
        http_archive,
        name = "com_github_cucumber_cucumber_cpp",
        sha256 = cucumber_cpp_sha256,
        strip_prefix = "cucumber-cpp-%s" % cucumber_cpp_ver,
        urls = [
            "https://github.com/cucumber/cucumber-cpp/archive/%s.tar.gz" % cucumber_cpp_ver,
            "https://github.com/cucumber/cucumber-cpp/archive/%s.tar.gz" % cucumber_cpp_ver,
            # "https://github.com/cucumber/cucumber-cpp/archive/refs/%s/%s.tar.gz" % (cucumber_cpp_ref, cucumber_cpp_ver),
            # "https://github.com/cucumber/cucumber-cpp/archive/refs/%s/%s.tar.gz" % (cucumber_cpp_ref, cucumber_cpp_ver),
        ],
        # type = "tar.gz",
        build_file = Label("@com.github.doevelopper.rules-sdlc//src/main/resources/off_the_shelf_software/cucumber_cpp:BUILD.cucumber-cpp.bazel"),
    )

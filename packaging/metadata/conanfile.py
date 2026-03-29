from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps, cmake_layout


class ProjectConan(ConanFile):
    """
    Conan 2.x recipe for the project.

    Usage:
        conan install . --output-folder=build --build=missing -pr:a default
        conan build .
    """

    name = "project-name"
    version = "0.1.0"
    license = "Apache-2.0"
    url = "https://github.com/your-org/project-name"
    description = "Replace with your project description."
    topics = ("cpp", "embedded", "systems")

    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_tests": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "with_tests": True,
    }

    # ── Source layout ────────────────────────────────────────────────────────
    exports_sources = (
        "CMakeLists.txt",
        "src/*",
        "include/*",
        "tasks/*",
        "buildsys/*",
    )

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("log4cxx/0.13.0")
        self.requires("boost/1.84.0")
        if self.options.with_tests:
            self.requires("gtest/1.14.0")
            self.requires("benchmark/1.8.3")

    def build_requirements(self):
        self.tool_requires("cmake/3.28.0")
        self.tool_requires("ninja/1.11.1")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_TESTING"] = self.options.with_tests
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

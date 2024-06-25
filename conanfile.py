import os
import re
from conan import ConanFile
from conan.tools.cmake import CMake
from conan.tools.files import load, copy, rmdir


class Recipe(ConanFile):
    name = "metall-ffi"
    version = None

    author = "https://github.com/dice-group/metall-ffi"
    url = "https://github.com/dice-group/metall-ffi"
    description = "FFI for the metall libary"
    topics = "FFI", "persistent memory"

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_test_deps": [True, False],
        "logger_extern_c": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "with_test_deps": False,
        "logger_extern_c": False,
    }
    exports = "LICENSE",
    exports_sources = "src/*", "CMakeLists.txt", "cmake/*"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("metall/0.28", transitive_headers=True)

        if self.options.with_test_deps:
            self.requires("doctest/2.4.11")

    def set_version(self):
        if not hasattr(self, 'version') or self.version is None:
            cmake_file = load(self, os.path.join(self.recipe_folder, "CMakeLists.txt"))
            self.version = re.search(r"project\([^)]*VERSION\s+(\d+\.\d+.\d+)[^)]*\)", cmake_file).group(1)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    _cmake = None

    def _configure_cmake(self):
        if self._cmake:
            return self._cmake
        self._cmake = CMake(self)
        self._cmake.configure(variables={"USE_CONAN": False, "LOGGER_EXTERN_C": self.options.logger_extern_c})

        return self._cmake

    def build(self):
        self._configure_cmake().build()

    def package(self):
        self._configure_cmake().install()
        rmdir(self, os.path.join(self.package_folder, "cmake"))
        rmdir(self, os.path.join(self.package_folder, "share"))
        copy(self, "LICENSE", src=self.recipe_folder, dst="licenses")

    def package_info(self):
        self.cpp_info.libs = ["metall-ffi"]
        self.cpp_info.set_property("cmake_find_mode", "both")
        self.cpp_info.set_property("cmake_file_name", self.name)
        self.cpp_info.set_property("cmake_target_name", f"{self.name}::{self.name}")
        self.cpp_info.requires = [
            "metall::metall",
        ]

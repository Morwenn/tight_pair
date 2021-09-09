# -*- coding: utf-8 -*-

# Copyright (c) 2019-2021 Morwenn
# SPDX-License-Identifier: MIT

from conans import CMake, ConanFile

required_conan_version = ">=1.33.0"


class TightPairConan(ConanFile):
    name = "tight_pair"
    version = "1.1.0"
    description = "Compressed pair for C++17"
    topics = "conan", "tight_pair", "compressed-pair", "data-structures"
    url = "https://github.com/Morwenn/tight_pair"
    homepage = url
    author = "Morwenn <morwenn29@hotmail.fr>"

    # Minimal exports
    exports = "LICENSE"
    exports_sources = [
        "include/*",
        "CMakeLists.txt",
        "cmake/tight_pair-config.cmake.in"
    ]
    no_copy_source = True
    settings = "os", "compiler", "build_type", "arch"

    def validate(self):
        if self.settings.get_safe("compiler.cppstd"):
            tools.check_min_cppstd(self, 17)

    def package(self):
        cmake = CMake(self)
        cmake.definitions["BUILD_TESTING"] = "OFF"
        cmake.configure()
        cmake.install()
        cmake.patch_config_paths()

        self.copy("LICENSE", dst="licenses", ignore_case=True, keep_path=False)

    def package_info(self):
        self.cpp_info.names["cmake_find_package"] = "tight_pair"
        self.cpp_info.names["cmake_find_package_multi"] = "tight_pair"
        if self.settings.compiler == "Visual Studio":
            self.cpp_info.cxxflags = ["/permissive-"]

    def package_id(self):
        self.info.header_only()

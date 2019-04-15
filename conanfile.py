from conans import CMake, ConanFile


class TightPairConan(ConanFile):
    name = "tight_pair"
    version = "1.1.0"
    license = "https://github.com/Morwenn/tight_pair/blob/master/LICENSE"
    url = "https://github.com/Morwenn/tight_pair"
    author = "Morwenn <morwenn29@hotmail.fr>"
    description = "Compressed pair for C++17"
    exports_sources = ("include/*", "CMakeLists.txt", "cmake/*")
    exports = "LICENSE"
    generators = "cmake", "txt"
    no_copy_source = True

    def package(self):
        cmake = CMake(self)
        cmake.definitions["BUILD_TESTING"] = "OFF"
        cmake.configure()
        cmake.install()

        self.copy("LICENSE", dst="licenses", ignore_case=True, keep_path=False)

    def package_id(self):
        self.info.header_only()

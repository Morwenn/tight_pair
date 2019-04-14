# -*- coding: utf-8 -*-

import os.path

from conans import ConanFile, CMake, tools

class TightPairTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        os.chdir('bin')
        self.run(os.path.join('.', 'example'))

# Copyright (c) 2019-2021 Morwenn
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 2.8.12)

project(test_package CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(tight_pair REQUIRED CONFIG)

add_executable(${PROJECT_NAME} tight_pair-integrity.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE tight_pair::tight_pair)

# Copyright (c) 2017-2021 Morwenn
# SPDX-License-Identifier: MIT

include(FetchContent)

########################################
# Test suite options

set(SANITIZE "" CACHE STRING "Comma-separated list of options to pass to -fsanitize")
option(TIGHT_PAIR_ENABLE_COVERAGE "Whether to produce code coverage" OFF)
option(TIGHT_PAIR_USE_VALGRIND "Whether to run the tests with Valgrind" OFF)

# Apparently ENABLE_COVERAGE is needed either way
if (TIGHT_PAIR_ENABLE_COVERAGE)
    set(ENABLE_COVERAGE ON CACHE BOOL "Enable coverage build." FORCE)
endif()

########################################
# Download and configure Catch2

FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG v2.13.6
)

FetchContent_GetProperties(catch2)
if (NOT catch2_POPULATED)
    FetchContent_Populate(catch2)
    add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
    include(Catch)
endif()

########################################
# Create and configure tests

# Make one executable for the whole test suite
add_executable(
    tight_pair-testsuite

    # Custom additional tests
    main.cpp
    alignment.cpp
    cppreference.cpp
    dr-811.cpp
    empty_base_get.cpp
    p1951.cpp
    piecewise_no_copy_move.cpp
    reference_wrapper.cpp
    tricky_comparisons.cpp

    # libc++ tests
    libcxx/assign_const_pair_U_V.cpp
    libcxx/assign_pair.cpp
    libcxx/assign_rv_pair.cpp
    libcxx/assign_rv_pair_U_V.cpp
    libcxx/assign_tuple.cpp
    libcxx/comparison.cpp
    libcxx/const_first_const_second.cpp
    libcxx/const_pair_U_V.cpp
    libcxx/copy_ctor.cpp
    libcxx/default.cpp
    libcxx/default_ctor.cpp
    libcxx/default-sfinae.cpp
    libcxx/dtor.cpp
    libcxx/get_const.cpp
    libcxx/get_const_rv.cpp
    libcxx/get_non_const.cpp
    libcxx/get_rv.cpp
    libcxx/implicit_deduction_guides.cpp
    libcxx/move_ctor.cpp
    libcxx/non_member_swap.cpp
    libcxx/pairs_by_type.cpp
    libcxx/piecewise_construct.cpp
    libcxx/rv_pair_U_V.cpp
    libcxx/special_member_generation.cpp
    libcxx/swap.cpp
    libcxx/trivial_copy_move.cpp
    libcxx/tuple_element.cpp
    libcxx/tuple_size.cpp
    libcxx/types.cpp
    libcxx/U_V.cpp
    libcxx/used_to_be_make_pair.cpp
)

target_link_libraries(tight_pair-testsuite
    PRIVATE
        Catch2::Catch2
        tight_pair::tight_pair
)

# Somewhat speed up Catch2 compile times
target_compile_definitions(tight_pair-testsuite
    PRIVATE
        CATCH_CONFIG_FAST_COMPILE
        CATCH_CONFIG_DISABLE_MATCHERS
)

if (MSVC)
    target_compile_options(tight_pair-testsuite PRIVATE /W2)
else()
    target_compile_options(tight_pair-testsuite
        PRIVATE
            -Wall -Wextra -Wcast-align -Winline -Wmissing-declarations -Wmissing-include-dirs
            -Wnon-virtual-dtor -Wodr -Wpedantic -Wredundant-decls -Wundef -Wunreachable-code
            $<$<CXX_COMPILER_ID:GNU>:-Wlogical-op -Wuseless-cast -Wzero-as-null-pointer-constant>
    )
endif()

# Settings depending on debug/release mode
target_compile_options(tight_pair-testsuite
    PRIVATE
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:Clang>>:-O0>
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU>>:-Og>
)

# Use lld or the gold linker if possible
if (UNIX AND NOT APPLE)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set_property(TARGET tight_pair-testsuite APPEND_STRING PROPERTY LINK_FLAGS " -fuse-ld=lld")
    else()
        set_property(TARGET tight_pair-testsuite APPEND_STRING PROPERTY LINK_FLAGS " -fuse-ld=gold")
    endif()
endif()

# Optionally enable sanitizers
if (UNIX AND SANITIZE)
    target_compile_options(tight_pair-testsuite PRIVATE -fsanitize=${SANITIZE})
    set_property(
        TARGET
            tight_pair-testsuite
        APPEND_STRING PROPERTY LINK_FLAGS
            " -fsanitize=${SANITIZE} -fno-sanitize-recover=all"
    )
endif()

########################################
# Configure coverage

if (TIGHT_PAIR_ENABLE_COVERAGE)
    find_package(codecov)
    add_coverage(tight_pair-testsuite)

    # Set flags specific to coverage builds
    target_compile_options(tight_pair-testsuite PRIVATE --coverage)
    set_property(TARGET tight_pair-testsuite APPEND_STRING PROPERTY LINK_FLAGS " --coverage")

    list(APPEND LCOV_REMOVE_PATTERNS "'/usr/*'")
    coverage_evaluate()
endif()

########################################
# Configure Valgrind

if (TIGHT_PAIR_USE_VALGRIND)
    find_program(MEMORYCHECK_COMMAND valgrind REQUIRED)
    set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --track-origins=yes --error-exitcode=1 --show-reachable=no")
endif()

########################################
# Discover tests

include(CTest)
catch_discover_tests(tight_pair-testsuite)

include(FetchContent)

# Download and configure Catch2 for the tests
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG master
)

FetchContent_GetProperties(catch2)
if (NOT catch2_POPULATED)
    FetchContent_Populate(catch2)
    add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
    include(Catch)
endif()

# Define test files

set(
    LIBCXX_TESTS

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

# Make one executable for the whole testsuite
add_executable(
    tight_pair-testsuite

    main.cpp
    cppreference.cpp
    dr-811.cpp
    empty_base_get.cpp
    piecewise_no_copy_move.cpp
    reference_wrapper.cpp
    tricky_comparisons.cpp
    ${LIBCXX_TESTS}
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

# General settings, mostly warnings
target_compile_options(tight_pair-testsuite
    PRIVATE
        -Wall -Wextra -Wcast-align -Winline -Wmissing-declarations -Wmissing-include-dirs
        -Wnon-virtual-dtor -Wodr -Wpedantic -Wredundant-decls -Wundef -Wunreachable-code
        $<$<CXX_COMPILER_ID:GNU>:-Wlogical-op -Wuseless-cast -Wzero-as-null-pointer-constant>
)

# Settings depending on debug/release mode
target_compile_options(tight_pair-testsuite
    PRIVATE
        $<$<AND:$<CONFIG:DEBUG>,$<CXX_COMPILER_ID:Clang>>:-O0>
        $<$<AND:$<CONFIG:DEBUG>,$<CXX_COMPILER_ID:GNU>>:-Og>
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

include(CTest)
catch_discover_tests(tight_pair-testsuite)

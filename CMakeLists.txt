cmake_minimum_required(VERSION 3.8.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(tight_pair VERSION 1.0.0 LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# Project options
option(BUILD_TESTING "Build the tight_pair test suite" ON)

# Create tight_pair library and configure it
add_library(tight_pair INTERFACE)
target_include_directories(tight_pair INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

target_compile_features(tight_pair INTERFACE cxx_std_17)

add_library(tight_pair::tight_pair ALIAS tight_pair)

# Install targets and files
install(
	TARGETS
		tight_pair
	EXPORT
		tight_pair-targets
	DESTINATION
		${CMAKE_INSTALL_LIBDIR}
)

install(
	EXPORT
		tight_pair-targets
	NAMESPACE
		tight_pair::
	DESTINATION
		"${CMAKE_INSTALL_LIBDIR}/cmake/tight_pair"
)

install(
	DIRECTORY
		"include/"
	DESTINATION
		"${CMAKE_INSTALL_INCLUDEDIR}"
)

configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/tight_pair-config.cmake.in
    ${CMAKE_BINARY_DIR}/cmake/tight_pair-config.cmake
    INSTALL_DESTINATION
		${CMAKE_INSTALL_LIBDIR}/cmake/tight_pair
)

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/cmake/tight_pair-config-version.cmake
    COMPATIBILITY
		SameMajorVersion
)

install(
    FILES
        ${CMAKE_BINARY_DIR}/cmake/tight_pair-config.cmake
        ${CMAKE_BINARY_DIR}/cmake/tight_pair-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tight_pair
)

# Export target so that it can be used in subdirectories
export(EXPORT tight_pair-targets
    FILE
		${CMAKE_BINARY_DIR}/cmake/tight_pair-targets.cmake
    NAMESPACE
		tight_pair::
)

# Build tests if this is the main project
if (BUILD_TESTING AND (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    enable_testing()
    add_subdirectory(tests)
endif()

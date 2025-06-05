cmake_minimum_required(VERSION 3.19.2)

# import vcpkg module
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    endif()
endif()

project(libqt)

# Version Info
set(LIBQT_VERSION_MAJOR 0)
set(LIBQT_VERSION_MINOR 0)
set(LIBQT_VERSION_PATCH 1)
set(LIBQT_VERSION ${LIBQT_VERSION_MAJOR}.${LIBQT_VERSION_MINOR}.${LIBQT_VERSION_PATCH})

# Project Info
set(PACKAGE_NAME      "${PROJECT_NAME}")
set(PACKAGE_VERSION   "${LIBQT_VERSION}")
set(PACKAGE_TARNAME   "${PACKAGE_NAME}-${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "https://github.com/hanjingo/libqt/issues")

add_definitions(-DLIBQT_VERSION_MAJOR="${LIBQT_VERSION_MAJOR}")
add_definitions(-DLIBQT_VERSION_MINOR="${LIBQT_VERSION_MINOR}")
add_definitions(-DLIBQT_VERSION_PATCH="${LIBQT_VERSION_PATCH}")

# build example
if(BUILD_EXAMPLE)
     add_subdirectory(examples)
endif()

# build tests
if(BUILD_TESTS)
     add_subdirectory(tests)
endif()


# ----------------------------- build install -----------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(libqt INTERFACE)
target_include_directories(libqt INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libqt>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmakes/libqtConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/libqtConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}/libqt
    # Passes the includedir install path
    PATH_VARS CMAKE_INSTALL_FULL_INCLUDEDIR
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/libqtConfigVersion.cmake
    VERSION ${LIBQT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/libqtConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/libqtConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_DATADIR}/libqt
)

install(DIRECTORY libqt/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libqt)

install(TARGETS libqt 
        EXPORT libqtTargets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libqt
)

install(EXPORT libqtTargets 
    FILE libqtTargets.cmake
    DESTINATION ${CMAKE_INSTALL_DATADIR}/libqt
)
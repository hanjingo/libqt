cmake_minimum_required(VERSION 3.19.2)

# Version Info
set(LIBQT_VERSION_MAJOR 0)
set(LIBQT_VERSION_MINOR 0)
set(LIBQT_VERSION_PATCH 1)
set(LIBQT_VERSION ${LIBQT_VERSION_MAJOR}.${LIBQT_VERSION_MINOR}.${LIBQT_VERSION_PATCH})

# Project Info
set(PACKAGE_NAME      "${PROJECT_NAME}")
set(PACKAGE_VERSION   "${LIBQT_VERSION}")
set(PACKAGE_TARNAME   "${PACKAGE_NAME}-${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "https://github.com/hanjingo/livermore-qt/issues")

add_definitions(-DLIBQT_VERSION_MAJOR="${LIBQT_VERSION_MAJOR}")
add_definitions(-DLIBQT_VERSION_MINOR="${LIBQT_VERSION_MINOR}")
add_definitions(-DLIBQT_VERSION_PATCH="${LIBQT_VERSION_PATCH}")

# utf-8 enable
add_compile_options(
    /source-charset:utf-8 
    /execution-charset:utf-8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd)

add_subdirectory(examples)


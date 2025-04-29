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

set(LIBQT_SRC
        libqt/db/operator.h
        libqt/db/operator.cpp
        libqt/db/pool.h
        libqt/db/pool.cpp
        libqt/net/proto/message.h
        libqt/net/tcp/ip.h
        libqt/net/tcp/ip.cpp
        libqt/net/tcp/conn.h
        libqt/net/tcp/conn.cpp
        libqt/ui/lineedit.h
        libqt/ui/lineedit.cpp
        libqt/ui/slider.h
        libqt/ui/slider.cpp
        libqt/ui/autogrid.h
        libqt/ui/autogrid.cpp
)

add_subdirectory(examples)

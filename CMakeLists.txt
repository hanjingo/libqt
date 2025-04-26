cmake_minimum_required(VERSION 3.19.2)

# Version Info
set(LIBQT_VERSION_MAJOR 0)
set(LIBQT_VERSION_MINOR 0)
set(LIBQT_VERSION_PATCH 1)
set(LIBQT_VERSION ${LIBQT_VERSION_MAJOR}.${LIBQT_VERSION_MINOR}.${LIBQT_VERSION_PATCH})

# Project Info
set(PACKAGE_NAME      "libqt")
set(PACKAGE_VERSION   "${LIBQT_VERSION}-dev")
set(PACKAGE_TARNAME   "${PACKAGE_NAME}-${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "https://github.com/hanjingo/libqt/issues")

add_definitions(-DVERSION_MAJOR="${LIBQT_VERSION_MAJOR}")
add_definitions(-DVERSION_MINOR="${LIBQT_VERSION_MINOR}")
add_definitions(-DVERSION_PATCH="${LIBQT_VERSION_PATCH}")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
        DB/operator.h
        DB/operator.cpp
        DB/pool.h
        DB/pool.cpp
        Net/Proto/message.h
        Net/Tcp/ip.h
        Net/Tcp/ip.cpp
        Net/Tcp/conn.h
        Net/Tcp/conn.cpp
        UI/lineedit.h
        UI/lineedit.cpp
        UI/slider.h
        UI/slider.cpp
)

add_subdirectory(Example)
project(tcp LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Concurrent)

set(LIBQT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../)

include_directories(${LIBQT_ROOT})

set(PROJECT_SOURCES
        main.cpp
        ${LIBQT_ROOT}/libqt/net/handler.h
        ${LIBQT_ROOT}/libqt/net/handler.cpp
        ${LIBQT_ROOT}/libqt/net/codec.h
        ${LIBQT_ROOT}/libqt/net/codec.cpp
        ${LIBQT_ROOT}/libqt/net/proto/message.h
        ${LIBQT_ROOT}/libqt/net/proto/message_v1.h
        ${LIBQT_ROOT}/libqt/net/tcp/ip.h
        ${LIBQT_ROOT}/libqt/net/tcp/ip.cpp
        ${LIBQT_ROOT}/libqt/net/tcp/tcpclient.h
        ${LIBQT_ROOT}/libqt/net/tcp/tcpclient.cpp
        ${LIBQT_ROOT}/libqt/sync/rwlocker.h
        ${LIBQT_ROOT}/libqt/sync/channel.h
        ${LIBQT_ROOT}/libqt/sync/dispatcher.h
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Concurrent
    Threads::Threads
)

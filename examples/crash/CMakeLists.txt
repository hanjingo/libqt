project(crash LANGUAGES CXX)

set(LIBQT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# enable debug info in release model
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")
# disable optimize in release model
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O0") # prefer to -O2
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O0")
if(MSVC)
    # generate PDB file
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Zi")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
endif()

find_package(Threads REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(unofficial-breakpad REQUIRED)

include_directories(${LIBQT_ROOT})

set(PROJECT_SOURCES
        main.cpp
        ${LIBQT_ROOT}/libqt/core/crash.h
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${BREAKPAD_SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Threads::Threads

    unofficial::breakpad::libbreakpad # for unofficial breakpad cmake config
    unofficial::breakpad::libbreakpad_client # for unofficial breakpad-client cmake config
)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("install to ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

    install(TARGETS ${PROJECT_NAME}
        DESTINATION "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}"
    )

    if(WIN32)
        include(${LIBQT_ROOT}/cmakes/deployqt.cmake)
        windeployqt(${PROJECT_NAME} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
    endif()
endif()
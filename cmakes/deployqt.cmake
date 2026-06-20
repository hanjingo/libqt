# NOTE: reference from: https://github.com/nitroshare/nitroshare-desktop/blob/master/cmake/DeployQt.cmake

# The MIT License (MIT)
#
# Copyright (c) 2018 Nathan Osman
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Find Qt in a version-agnostic way (prefer Qt6, fall back to Qt5).
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

# Retrieve the absolute path to qmake and then use that path to find
# the windeployqt and macdeployqt binaries.
if(TARGET Qt${QT_VERSION_MAJOR}::qmake)
    get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
endif()

if(NOT _qmake_executable)
    # Fallback: infer Qt's bin directory from the Core library location.
    get_target_property(_qt_core_location Qt${QT_VERSION_MAJOR}::Core IMPORTED_LOCATION)
    if(_qt_core_location)
        get_filename_component(_qt_lib_dir "${_qt_core_location}" DIRECTORY)
        get_filename_component(_qt_prefix_dir "${_qt_lib_dir}" DIRECTORY)
        find_program(_qmake_executable NAMES qmake qmake6 HINTS "${_qt_prefix_dir}/bin")
    endif()
endif()

if(_qmake_executable)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
endif()

if(_qt_bin_dir)
    find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt HINTS "${_qt_bin_dir}")
else()
    find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt)
endif()
if(WIN32 AND NOT WINDEPLOYQT_EXECUTABLE)
    message(FATAL_ERROR "windeployqt not found")
endif()

set(_windeployqt_optional_args)
if(WIN32 AND WINDEPLOYQT_EXECUTABLE)
    execute_process(
        COMMAND "${WINDEPLOYQT_EXECUTABLE}" --help
        OUTPUT_VARIABLE _windeployqt_help
        ERROR_VARIABLE _windeployqt_help_err
        RESULT_VARIABLE _windeployqt_help_result
    )

    if(_windeployqt_help_result EQUAL 0)
        if(_windeployqt_help MATCHES "--no-angle")
            list(APPEND _windeployqt_optional_args --no-angle)
        endif()
        if(_windeployqt_help MATCHES "--no-opengl-sw")
            list(APPEND _windeployqt_optional_args --no-opengl-sw)
        endif()
    endif()
endif()

if(_qt_bin_dir)
    find_program(MACDEPLOYQT_EXECUTABLE NAMES macdeployqt HINTS "${_qt_bin_dir}")
else()
    find_program(MACDEPLOYQT_EXECUTABLE NAMES macdeployqt)
endif()
if(APPLE AND NOT MACDEPLOYQT_EXECUTABLE)
    message(FATAL_ERROR "macdeployqt not found")
endif()

# Add commands that copy the required Qt files to the same directory as the
# target after being built as well as including them in final installation
function(windeployqt target)

    # Run windeployqt immediately after build
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E
            env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
                --verbose 0
                --no-compiler-runtime
                ${_windeployqt_optional_args}
                \"$<TARGET_FILE:${target}>\"
        COMMENT "Deploying Qt..."
    )

    # windeployqt doesn't work correctly with the system runtime libraries,
    # so we fall back to one of CMake's own modules for copying them over

    # Doing this with MSVC 2015 requires CMake 3.6+
    if((MSVC_VERSION VERSION_EQUAL 1900 OR MSVC_VERSION VERSION_GREATER 1900)
            AND CMAKE_VERSION VERSION_LESS "3.6")
        message(WARNING "Deploying with MSVC 2015+ requires CMake 3.6+")
    endif()

    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    include(InstallRequiredSystemLibraries)
    foreach(lib ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
        get_filename_component(filename "${lib}" NAME)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E
                copy_if_different "${lib}" \"$<TARGET_FILE_DIR:${target}>\"
            COMMENT "Copying ${filename}..."
        )
    endforeach()
endfunction()

# Add commands that copy the required Qt files to the application bundle
# represented by the target.
function(macdeployqt target)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND "${MACDEPLOYQT_EXECUTABLE}"
            \"$<TARGET_FILE_DIR:${target}>/../..\"
            -always-overwrite
        COMMENT "Deploying Qt..."
    )
endfunction()

mark_as_advanced(WINDEPLOYQT_EXECUTABLE MACDEPLOYQT_EXECUTABLE)
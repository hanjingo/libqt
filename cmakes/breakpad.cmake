cmake_minimum_required(VERSION 3.19.2)

if (NOT BREAKPAD_PATH)
  message(FATAL_ERROR "BREAKPAD_PATH NOT FOUND")
endif()
message(STATUS "BREAKPAD_PATH: ${BREAKPAD_PATH}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(
    -DNOMINMAX
    -DUNICODE
    -DWIN32_LEAN_AND_MEAN
    -D_CRT_SECURE_NO_WARNINGS
    -D_CRT_SECURE_NO_DEPRECATE
    -D_CRT_NONSTDC_NO_DEPRECATE
)

if(WIN32)
  set(SOURCES
          ${BREAKPAD_PATH}/src/client/windows/crash_generation/crash_generation_client.cc
          ${BREAKPAD_PATH}/src/client/windows/handler/exception_handler.cc
          ${BREAKPAD_PATH}/src/common/windows/guid_string.cc
  )
else()
  set(COMM_SOURCES
        ${BREAKPAD_PATH}/src/client/minidump_file_writer.cc
        ${BREAKPAD_PATH}/src/common/convert_UTF.cc
        ${BREAKPAD_PATH}/src/common/md5.cc
        ${BREAKPAD_PATH}/src/common/string_conversion.cc
  )
  if(APPLE)
    set(SOURCES
          ${COMM_SOURCES}
          ${BREAKPAD_PATH}/src/client/mac/crash_generation/crash_generation_client.cc
          ${BREAKPAD_PATH}/src/client/mac/handler/breakpad_nlist_64.cc
          ${BREAKPAD_PATH}/src/client/mac/handler/dynamic_images.cc
          ${BREAKPAD_PATH}/src/client/mac/handler/exception_handler.cc
          ${BREAKPAD_PATH}/src/client/mac/handler/minidump_generator.cc
          ${BREAKPAD_PATH}/src/common/mac/MachIPC.mm
          ${BREAKPAD_PATH}/src/common/mac/bootstrap_compat.cc
          ${BREAKPAD_PATH}/src/common/mac/file_id.cc
          ${BREAKPAD_PATH}/src/common/mac/macho_id.cc
          ${BREAKPAD_PATH}/src/common/mac/macho_utilities.cc
          ${BREAKPAD_PATH}/src/common/mac/macho_walker.cc
          ${BREAKPAD_PATH}/src/common/mac/string_utilities.cc
    )
  else()
    set(SOURCES
          ${COMM_SOURCES}
          ${BREAKPAD_PATH}/src/client/linux/crash_generation/crash_generation_client.cc
          ${BREAKPAD_PATH}/src/client/linux/dump_writer_common/thread_info.cc
          ${BREAKPAD_PATH}/src/client/linux/dump_writer_common/ucontext_reader.cc
          ${BREAKPAD_PATH}/src/client/linux/handler/exception_handler.cc
          ${BREAKPAD_PATH}/src/client/linux/handler/minidump_descriptor.cc
          ${BREAKPAD_PATH}/src/client/linux/log/log.cc
          ${BREAKPAD_PATH}/src/client/linux/microdump_writer/microdump_writer.cc
          ${BREAKPAD_PATH}/src/client/linux/minidump_writer/linux_core_dumper.cc
          ${BREAKPAD_PATH}/src/client/linux/minidump_writer/linux_dumper.cc
          ${BREAKPAD_PATH}/src/client/linux/minidump_writer/linux_ptrace_dumper.cc
          ${BREAKPAD_PATH}/src/client/linux/minidump_writer/minidump_writer.cc
          ${BREAKPAD_PATH}/src/common/linux/breakpad_getcontext.S
          ${BREAKPAD_PATH}/src/common/linux/elfutils.cc
          ${BREAKPAD_PATH}/src/common/linux/file_id.cc
          ${BREAKPAD_PATH}/src/common/linux/guid_creator.cc
          ${BREAKPAD_PATH}/src/common/linux/linux_libc_support.cc
          ${BREAKPAD_PATH}/src/common/linux/memory_mapped_file.cc
          ${BREAKPAD_PATH}/src/common/linux/safe_readlink.cc
    )
  endif()
endif()

include_directories(${BREAKPAD_PATH}/src)

set(BREAKPAD_SOURCES ${SOURCES})

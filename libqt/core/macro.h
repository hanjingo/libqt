#ifndef MACRO_H
#define MACRO_H

#include <qglobal.h>

// --------------------macro functions---------------------
#define CONCATENATE(a, b) a##b

// ----------------------------dll--------------------------
// export/import dll
#if defined(_MSC_VER) //  Microsoft
#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT __declspec(dllimport)
#elif defined(__GNUC__) //  GCC
#define DLL_EXPORT __attribute__((visibility("default")))
#define DLL_IMPORT
#else // Warnning
#define DLL_EXPORT
#define DLL_IMPORT
#pragma WARNING UNKNOWN DYNAMIC LINK IMPORT/EXPORT SEMANTICS.
#endif

// export/import c style dll
#ifdef __cplusplus
#define C_STYLE_EXPORT extern "C" DLL_EXPORT
#define C_STYLE_IMPORT extern "C" DLL_IMPORT
#else
#define C_STYLE_EXPORT DLL_EXPORT
#define C_STYLE_IMPORT DLL_IMPORT
#endif

// dll extension
#if defined(Q_OS_WIN)
#define DLL_EXT ".dll"
#elif defined(Q_OS_MACOS) || defined(Q_OS_IOS)
#define DLL_EXT ".dylib"
#elif defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)
#define DLL_EXT ".so"
#else
#pragma WARNING UNKNOWN DYNAMIC LINK LIBRARY FILE EXTENSION.
#endif


// --------------------------time------------------------------
// __DATE__:[Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sept, Oct, Nov, Dec] 2 2024
// __TIME__:17:37:21
#define COMPILE_YEAR ( \
    (__DATE__ [7] - '0') * 1000 + \
    (__DATE__ [8] - '0') * 100 + \
    (__DATE__ [9] - '0') * 10 + \
    (__DATE__ [10] - '0'))

#define COMPILE_MONTH ( \
    __DATE__[0] == 'J' && __DATE__[1] == 'a' ? 1 : \
    __DATE__[0] == 'F' ? 2 : \
    __DATE__[0] == 'M' && __DATE__[2] == 'r' ? 3 : \
    __DATE__[0] == 'A' && __DATE__[1] == 'p' ? 4 : \
    __DATE__[0] == 'M' && __DATE__[2] == 'y' ? 5 : \
    __DATE__[0] == 'J' && __DATE__[2] == 'n' ? 6 : \
    __DATE__[0] == 'J' && __DATE__[2] == 'l' ? 7 : \
    __DATE__[0] == 'A' && __DATE__[1] == 'u' ? 8 : \
    __DATE__[0] == 'S' ? 9 : \
    __DATE__[0] == 'O' ? 10 : \
    __DATE__[0] == 'N' ? 11 : 12)

#define COMPILE_DAY ( \
    (__DATE__ [4] == ' ' ? 0 : __DATE__ [4] - '0') * 10 +\
    (__DATE__ [5] - '0'))

#define COMPILE_HOUR ((__TIME__[0] - '0') * 10 + (__TIME__[1] - '0'))

#define COMPILE_MINUTE ((__TIME__[3] - '0') * 10 + (__TIME__[4] - '0'))

#define COMPILE_SECOND ((__TIME__[6] - '0') * 10 + (__TIME__[7] - '0'))

#define COMPILE_TIME_FMT_ISO8601 "%04d-%02d-%02d %02d:%02d:%02d"

#ifndef COMPILE_TIME_FMT
#define COMPILE_TIME_FMT COMPILE_TIME_FMT_ISO8601
#endif
#ifndef COMPILE_TIME_LEN
#define COMPILE_TIME_LEN 20
#endif

#if !defined(_MSC_VER)
#define _compile_time()\
({\
    static char _date_time_buf[COMPILE_TIME_LEN] = {0}; \
    memset(_date_time_buf, 0, COMPILE_TIME_LEN); \
    sprintf(_date_time_buf, COMPILE_TIME_FMT, COMPILE_YEAR, COMPILE_MONTH, COMPILE_DAY, COMPILE_HOUR, COMPILE_MINUTE, COMPILE_SECOND); \
    _date_time_buf; \
})
#else
static const char* _compile_time(void) {
    static char _date_time_buf[COMPILE_TIME_LEN] = { 0 };
    snprintf(_date_time_buf, COMPILE_TIME_LEN, COMPILE_TIME_FMT, COMPILE_YEAR, COMPILE_MONTH, COMPILE_DAY, COMPILE_HOUR, COMPILE_MINUTE, COMPILE_SECOND);
    return _date_time_buf;
}
#endif
#define COMPILE_TIME _compile_time()


// ---------------file size-------------------
#ifndef FSIZE
#define FSIZE unsigned long long
#endif

#ifndef BYTE
#define BYTE(n) \
    ((FSIZE)(n))
#endif

#ifndef KB
#define KB(n) \
    ((FSIZE)(n) * 0x400)
#endif

#ifndef MB
#define MB(n) \
    ((FSIZE)(n) * 0x100000)
#endif

#ifndef GB
#define GB(n) \
    ((FSIZE)(n) * 0x40000000)
#endif

#ifndef TB
#define TB(n) \
    ((FSIZE)(n) * 0x10000000000)
#endif

#endif
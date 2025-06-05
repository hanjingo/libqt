#ifndef CRASH_H
#define CRASH_H

#include <QObject>
#include <QDir>
#include <QProcess>
#include <QCoreApplication>

#if defined(Q_OS_MAC)
#include "client/mac/handler/exception_handler.h"
#elif defined(Q_OS_LINUX)
#include "client/linux/handler/exception_handler.h"
#elif defined(Q_OS_WIN32)
#include <windows.h>
#include "client/windows/handler/exception_handler.h"
#endif

#if defined(Q_OS_WIN32)
typedef bool(*dump_callback_t)(const wchar_t*,
                               const wchar_t*,
                               void*,
                               EXCEPTION_POINTERS*,
                               MDRawAssertionInfo*,
                               bool);

static bool default_dump_callback(const wchar_t* dump_dir,
                                  const wchar_t* minidump_id,
                                  void* context,
                                  EXCEPTION_POINTERS* exinfo,
                                  MDRawAssertionInfo* assertion,
                                  bool succeeded)
{
    return true;
}

#elif defined(Q_OS_MAC)
typedef bool (*dump_callback_t)(const char *, const char *, void *, bool);

static bool default_dump_callback(const char *dump_dir, const char *minidump_id, void *context, bool succeeded)
{
    return true;
}

#elif defined(Q_OS_LINUX)
typedef bool (*dump_callback_t)(const google_breakpad::MinidumpDescriptor&, void*, bool );

static bool default_dump_callback(const google_breakpad::MinidumpDescriptor& descriptor, void* context, bool succeeded)
{
    return true;
}
#endif

class CrashHandler: public QObject
{
    Q_OBJECT

public:
    CrashHandler()
        : m_cb{default_dump_callback}
    {
    }
    CrashHandler(const char* abs_path)
        : m_cb{default_dump_callback}
    {
        setLocalPath(abs_path);
    }
    CrashHandler(const char* abs_path, const dump_callback_t cb)
        : m_cb{cb}
    {
        setLocalPath(abs_path);
    }
    virtual ~CrashHandler()
    {
    }

    static CrashHandler* instance()
    {
        static CrashHandler inst;
        return &inst;
    }

    inline void setDumpCallback(const dump_callback_t cb) { m_cb = cb; }

    inline void setLocalPath(const char* abs_path) { setLocalPath(QString(abs_path)); }

    void setLocalPath(const QString& abs_path)
    {  
        if (m_handler != nullptr)  
        {  
            delete m_handler;  
        }  

        std::string std_abs_path = abs_path.toStdString();
    #if defined(_WIN32)  
        std::wstring wabs_path(std_abs_path.begin(), std_abs_path.end());
        m_handler = new google_breakpad::ExceptionHandler(wabs_path,  
                                                         nullptr, // FilterCallback  
                                                         m_cb,  
                                                         nullptr, // context  
                                                         google_breakpad::ExceptionHandler::HANDLER_ALL);  
    #elif __APPLE__  
        m_handler = new google_breakpad::ExceptionHandler(std_abs_path,
                                                         nullptr, // FilterCallback  
                                                         m_cb,  
                                                         nullptr, // context  
                                                         true,  
                                                         nullptr);  
    #else  
        m_handler = new google_breakpad::ExceptionHandler(google_breakpad::MinidumpDescriptor(std_abs_path),
                                                         nullptr, // FilterCallback  
                                                         m_cb,  
                                                         nullptr, // context  
                                                         true,  
                                                         -1);  
    #endif  
    }

#if defined(_WIN32)
    // Reference to: https://www.cnblogs.com/cswuyg/p/3207576.html
    static LPTOP_LEVEL_EXCEPTION_FILTER WINAPI tempSetUnhandledExceptionFilter( 
        LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter )
    {
        return NULL;
    }
#endif

    bool preventSetUnhandledExceptionFilter()  
    {  
    #if defined(_WIN32)  
        HMODULE hKernel32 = LoadLibraryW(L"kernel32.dll"); // Use LoadLibraryW for wide-character strings  
        if (hKernel32 == NULL)  
            return false;  

        void *pOrgEntry = (void*)(::GetProcAddress(hKernel32, "SetUnhandledExceptionFilter"));  
        if (pOrgEntry == NULL)  
            return false;  

        unsigned char newJump[5];  
        DWORD dwOrgEntryAddr = (DWORD)pOrgEntry;  
        dwOrgEntryAddr += 5; // jump instruction has 5 byte space.  

        void *pNewFunc = (void*)(&tempSetUnhandledExceptionFilter);
        DWORD dwNewEntryAddr = (DWORD)pNewFunc;  
        DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;  

        newJump[0] = 0xE9;  // jump  
        memcpy(&newJump[1], &dwRelativeAddr, sizeof(DWORD));  
        SIZE_T bytesWritten;  
        DWORD dwOldFlag, dwTempFlag;  
        ::VirtualProtect(pOrgEntry, 5, PAGE_READWRITE, &dwOldFlag);  
        BOOL bRet = ::WriteProcessMemory(::GetCurrentProcess(), pOrgEntry, newJump, 5, &bytesWritten);  
        ::VirtualProtect(pOrgEntry, 5, dwOldFlag, &dwTempFlag);  
        return bRet;  
    #else  
        return true;  
    #endif  
    }

private:
    google_breakpad::ExceptionHandler* m_handler = nullptr;
    dump_callback_t m_cb;
};

#endif

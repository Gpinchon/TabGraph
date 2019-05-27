#include <StackTracer.hpp>
#include <unistd.h>
#include <inttypes.h>
#include <cstring>

int &_get_handler_fd()
{
    static int fd = STDERR_FILENO;
    return fd;
}

void StackTracer::set_handler_fd(int fd)
{
    _get_handler_fd() = fd;
}

int StackTracer::handler_fd()
{
    return _get_handler_fd();
}

inline char *itoa_safe(intmax_t value, char *result) {
    intmax_t tmp_value;
    char *ptr, *ptr2, tmp_char;
    ptr = result;
    do {
        tmp_value = value;
        value /= 10;
        *ptr++ = "ZYXWVUTSRQPONMLKJIHGFEDCBA9876543210123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[35 + (tmp_value - value * 10)];
    } while (value);
    if (tmp_value < 0)
        *ptr++ = '-';
    ptr2 = result;
    result = ptr;
    *ptr-- = '\0';
    while (ptr2 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr2;
        *ptr2++ = tmp_char;
    }
    return result;
}

inline void putstr(const char *str)
{
    if (str)
        write(StackTracer::handler_fd(), str, strlen(str));
    else
        write(StackTracer::handler_fd(), "???", 3);
}

inline void putnbr(const int nbr)
{
    char buffer[1024];   
    itoa_safe(nbr, buffer);
    write(StackTracer::handler_fd(), buffer, strlen(buffer));
}

#define _DEBUGLOG(file, line, message)  { putstr("file:///"); putstr(file); putstr(": "); putstr(message); putstr("\n"); }//std::cerr << "file:///" << file << ":" << line << ": " << message << std::endl;
#define DEBUGLOG(message)               _DEBUGLOG(__FILE__, __LINE__, message)

#ifdef _WIN32

#include <windows.h>
#include <dbghelp.h>
#include <tlhelp32.h>
#include <iostream>
#include <tr1/memory>

static inline DWORD GetMainThreadId()
{
    static DWORD result = 0;
    if (result != 0)
        return result;
    const std::tr1::shared_ptr<void> hThreadSnapshot(
        CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0), CloseHandle);
    if (hThreadSnapshot.get() == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("GetMainThreadId failed");
    }
    THREADENTRY32 tEntry;
    tEntry.dwSize = sizeof(THREADENTRY32);
    
    DWORD currentPID = GetCurrentProcessId();
    for (BOOL success = Thread32First(hThreadSnapshot.get(), &tEntry);
        !result && success && GetLastError() != ERROR_NO_MORE_FILES;
        success = Thread32Next(hThreadSnapshot.get(), &tEntry))
    {
        if (tEntry.th32OwnerProcessID == currentPID) {
            result = tEntry.th32ThreadID;
        }
    }
    return result;
}

#define IS_MAIN_THREAD (GetCurrentThreadId() == GetMainThreadId())

STACKFRAME64  FillStackFrame(CONTEXT context)
{
    STACKFRAME64  s = {};

#ifdef _M_IX86
    s.AddrPC.Offset = context.Eip;
    s.AddrPC.Mode = AddrModeFlat;
    s.AddrFrame.Offset = context.Ebp;
    s.AddrFrame.Mode = AddrModeFlat;
    s.AddrStack.Offset = context.Esp;
    s.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
    s.AddrPC.Offset     = context.Rip;
    s.AddrPC.Mode       = AddrModeFlat;
    s.AddrStack.Offset  = context.Rsp;
    s.AddrStack.Mode    = AddrModeFlat;
    s.AddrFrame.Offset  = context.Rbp;
    s.AddrFrame.Mode    = AddrModeFlat;
#elif _M_IA64
    s.AddrPC.Offset = context.StIIP;
    s.AddrPC.Mode = AddrModeFlat;
    s.AddrFrame.Offset = context.IntSp;
    s.AddrFrame.Mode = AddrModeFlat;
    s.AddrBStore.Offset = context.RsBSP;
    s.AddrBStore.Mode = AddrModeFlat;
    s.AddrStack.Offset = context.IntSp;
    s.AddrStack.Mode = AddrModeFlat;
#endif
    return s;
}

DWORD   GetMachineType()
{
#ifdef _M_IX86
    return IMAGE_FILE_MACHINE_I386;
#elif _M_X64
    return IMAGE_FILE_MACHINE_AMD64;
#elif _M_IA64
    return IMAGE_FILE_MACHINE_IA64;
#endif
}

void    stackWalkPrint(HANDLE process, HANDLE thread)
{
    DWORD           machineType = GetMachineType();
    CONTEXT         context = {};
    STACKFRAME64    stackframe = {};

    context.ContextFlags = CONTEXT_FULL;
    if (!GetThreadContext(thread, &context)) {
        DEBUGLOG("Couldn't get thread context");
        return;
    }
    stackframe = FillStackFrame(context);
    SymSetOptions(SYMOPT_LOAD_LINES);
    SymInitialize(process, NULL, TRUE);

    while (StackWalk64(
      machineType, process, thread,
      &stackframe, &context, nullptr, 
      SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
    {
        char                moduleBuff[MAX_PATH];
        BYTE                symbolBuffer[sizeof(IMAGEHLP_SYMBOL64) + MAX_SYM_NAME];
        DWORD               offset = 0;
        PIMAGEHLP_SYMBOL64  symbol = (PIMAGEHLP_SYMBOL64)symbolBuffer;
        HINSTANCE           moduleBase = (HINSTANCE)SymGetModuleBase64(process, stackframe.AddrPC.Offset);

        symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64) + MAX_SYM_NAME;
        symbol->MaxNameLength = MAX_SYM_NAME;

        putstr("\t");
        if (moduleBase && GetModuleFileName(moduleBase, moduleBuff, MAX_PATH)) {
            DWORD64 displacement = 0;
            putstr("[Module : ");putstr(moduleBuff);putstr("]");
            if (SymGetSymFromAddr64(process, stackframe.AddrPC.Offset, &displacement, symbol)) {
                putstr("[Symbol : ");putstr(symbol->Name);putstr("]");
            }
            else {
                putstr("[Symbol : ERROR ");putnbr(GetLastError());putstr("]");
            }
            IMAGEHLP_LINE64 line = {};
            line.SizeOfStruct = sizeof(line);
            if (SymGetLineFromAddr64(process, stackframe.AddrPC.Offset, &offset, &line)) {
                putstr("[Location : ");putstr(line.FileName);putstr(" at line ");putnbr(line.LineNumber);putstr("]");
            }
            else {
                putstr("[Location : ERROR ");putnbr(GetLastError());putstr("]");
            }
        }
        else {
            putstr("[Module : ERROR ");putnbr(GetLastError());putstr("]");
        }
        putstr("\n");
    }
}



inline void StackTracer::stack_trace(int /*signum*/)
{
    //std::cout << StackTracer::gettid() << "/" << StackTracer::getpid() << std::endl;
    //std::cout << GetMainThreadId() << "/" << GetCurrentThreadId() << std::endl;
    if (!IS_MAIN_THREAD) {
        return;
    }
    HANDLE process = GetCurrentProcess();
    HANDLE shapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    if (shapshot == INVALID_HANDLE_VALUE)
        return;
    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(threadEntry);
    if (Thread32First(shapshot, &threadEntry)) {
        do {
            if (threadEntry.th32OwnerProcessID != GetCurrentProcessId())
                continue;
            if (threadEntry.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(threadEntry.th32OwnerProcessID))
            {
                //std::cout << GetMainThreadId() << "/" << threadEntry.th32ThreadID << std::endl;
                if (GetMainThreadId() == threadEntry.th32ThreadID)
                    putstr("Main thread stack :\n");
                else {
                    putstr("Thread ");putnbr(threadEntry.th32ThreadID);putstr(" stack :\n");
                }
                auto hThread = OpenThread(THREAD_QUERY_INFORMATION|THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT, false, threadEntry.th32ThreadID);
                if (hThread == nullptr){
                    DEBUGLOG("Couldn't get thread handle");
                }
                else{
                    stackWalkPrint(process, hThread);
                }
            }
            threadEntry.dwSize = sizeof(threadEntry);
        } while (Thread32Next(shapshot, &threadEntry));
        CloseHandle(shapshot);
    }
    SymCleanup(process);
}

long StackTracer::gettid()
{
    return GetCurrentThreadId();
}

long StackTracer::getpid()
{
    return _getpid();
}

void    StackTracer::set_signal_handler(int signum)
{
    signal(signum, StackTracer::stack_trace);
}

#else

#include <iostream>
#include <execinfo.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <csignal>
#include <mutex>
#include <cxxabi.h>
#include <atomic>

#ifndef tkill
# define tkill(tid, sig) syscall(SYS_tgkill, getpid(), tid, sig)
#endif

#define MAX_STACK_FRAMES    64
#define IS_MAIN_THREAD      (StackTracer::getpid() == StackTracer::gettid())

static std::mutex           writeMutex;
static std::atomic<size_t>  threadNbr(0);
static std::atomic<size_t>  threadDone(0);
static std::atomic<bool>    handlingSignal(false);

static inline void  waitForThreads() {
    while (threadNbr > threadDone) {
        usleep(16);
    }
    writeMutex.lock();
    writeMutex.unlock();
    threadDone = 0;
    threadNbr = 0;
}

#include <sys/syscall.h>
#include <sys/types.h>
#include <fcntl.h>

struct linux_dirent {
    unsigned long  d_ino;
    unsigned long  d_off;
    unsigned short d_reclen;
    char           d_name[];
};

#define DIR_BUF_SIZE 1024

static inline void  killAllThreads(int signum)
{
    int dir_fd;
    char dir_buf[DIR_BUF_SIZE];
    struct linux_dirent *dir_entry;
    int r, pos;

    dir_fd = open("/proc/self/task", O_RDONLY | O_DIRECTORY);
    r = syscall(SYS_getdents, dir_fd, dir_buf, DIR_BUF_SIZE);
    for (pos = 0; pos < r; pos += dir_entry->d_reclen)
    {
        dir_entry = (struct linux_dirent *) (dir_buf + pos);
        if(dir_entry->d_name[0] == '.')
            continue;
        int threadId = atoi(dir_entry->d_name);
        if (threadId == StackTracer::gettid())
            continue;
        if (tkill(threadId, signum)) {
            DEBUGLOG("Could not send signal to thread");
            continue;
        }
        threadNbr++;
    }
    close(dir_fd);
}

void  StackTracer::stack_trace(int signum)
{
    StackTracer::getpid();
    if (signum > 0 && !handlingSignal) {
        kill(0, signum);
        return;
    }
    writeMutex.lock();
    putstr("--------------------------------------------------------------------------------\n");
    if (IS_MAIN_THREAD) putstr("Main Thread");
    else { putstr("Thread "); putnbr(StackTracer::gettid() - StackTracer::getpid()); }
    putstr(" received signal("); putnbr(signum); putstr(")\n");
    void*   stackFrames[MAX_STACK_FRAMES];
    size_t  size = backtrace(stackFrames, MAX_STACK_FRAMES);
    backtrace_symbols_fd(stackFrames, size, StackTracer::handler_fd());   
    putstr("--------------------------------------------------------------------------------\n");
    writeMutex.unlock();
    if (signum == 0)
        return;
    if (!IS_MAIN_THREAD) {
        if (threadNbr > 0)
            threadDone++;
        while (handlingSignal){ usleep(16); }
        return;
    }
    killAllThreads(signum);
    waitForThreads();
}

void    StackTracer::sig_handler(int signum) {
    if (IS_MAIN_THREAD)
        handlingSignal = true;
    StackTracer::stack_trace(signum);
    if (IS_MAIN_THREAD) {
        handlingSignal = false;
    }
}

void    StackTracer::sigaction_handler(int signum, siginfo_t *siginfo, void *)
{
    if (IS_MAIN_THREAD) {
        handlingSignal = true;
        psiginfo(siginfo, nullptr);
    }
    StackTracer::stack_trace(signum);
    if (IS_MAIN_THREAD) {
        handlingSignal = false;
    }
}

void    StackTracer::set_signal_handler(int signum)
{
    struct sigaction sa = {};
    sa.sa_handler = StackTracer::sig_handler;
    sa.sa_restorer = nullptr;
    sa.sa_sigaction = StackTracer::sigaction_handler;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigaction(signum, &sa, nullptr);
}

#ifdef gettid
long StackTracer::gettid(void) {
    return ::gettid();
}
#else
long StackTracer::gettid(void) {
    return syscall(SYS_gettid);
}
#endif //gettid

long StackTracer::getpid(void) {
    return ::getpid();
}

#endif //_WIN32



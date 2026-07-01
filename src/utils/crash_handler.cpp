#include "crash_handler.h"
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cstdint>
#include <ctime>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

static void writeStackTrace(EXCEPTION_POINTERS *ep, FILE *log) {
  HANDLE process = GetCurrentProcess();
  HANDLE thread = GetCurrentThread();
  SymInitialize(process, NULL, TRUE);

  STACKFRAME64 stack = {};
  DWORD machineType;
#ifdef _M_X64
  machineType = IMAGE_FILE_MACHINE_AMD64;
  stack.AddrPC.Offset = ep->ContextRecord->Rip;
  stack.AddrFrame.Offset = ep->ContextRecord->Rbp;
  stack.AddrStack.Offset = ep->ContextRecord->Rsp;
#else
  machineType = IMAGE_FILE_MACHINE_I386;
  stack.AddrPC.Offset = ep->ContextRecord->Eip;
  stack.AddrFrame.Offset = ep->ContextRecord->Ebp;
  stack.AddrStack.Offset = ep->ContextRecord->Esp;
#endif
  stack.AddrPC.Mode = AddrModeFlat;
  stack.AddrFrame.Mode = AddrModeFlat;
  stack.AddrStack.Mode = AddrModeFlat;

  fprintf(log, "Stack trace:\n");
  for (int i = 0; i < 32; i++) {
    if (!StackWalk64(machineType, process, thread, &stack, ep->ContextRecord,
                     NULL, &SymFunctionTableAccess64, &SymGetModuleBase64, NULL))
      break;

    DWORD64 addr = stack.AddrPC.Offset;
    if (addr == 0)
      break;

    char buffer[sizeof(SYMBOL_INFO) + 256];
    PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = 255;

    DWORD64 displacement = 0;
    if (SymFromAddr(process, addr, &displacement, symbol)) {
      fprintf(log, "  [%d] %s+0x%llx (0x%llx)\n", i, symbol->Name,
              (unsigned long long)displacement, (unsigned long long)addr);
    } else {
      fprintf(log, "  [%d] 0x%llx\n", i, (unsigned long long)addr);
    }
  }
  SymCleanup(process);
}

static LONG WINAPI exceptionHandler(EXCEPTION_POINTERS *ep) {
  const char *reason = "Unknown exception";
  switch (ep->ExceptionRecord->ExceptionCode) {
  case EXCEPTION_ACCESS_VIOLATION:
    reason = "Access Violation (nullptr or invalid memory)";
    break;
  case EXCEPTION_STACK_OVERFLOW:
    reason = "Stack Overflow";
    break;
  case EXCEPTION_INT_DIVIDE_BY_ZERO:
    reason = "Integer Division by Zero";
    break;
  case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    reason = "Float Division by Zero";
    break;
  case EXCEPTION_FLT_OVERFLOW:
    reason = "Float Overflow";
    break;
  case EXCEPTION_ILLEGAL_INSTRUCTION:
    reason = "Illegal Instruction";
    break;
  }

  FILE *log = fopen("crash.log", "w");
  if (log) {
    time_t t = time(NULL);
    fprintf(log, "=== CRASH REPORT ===\n");
    fprintf(log, "Time: %s", ctime(&t));
    fprintf(log, "Reason: %s\n", reason);
    fprintf(log, "Exception code: 0x%lX\n",
            (unsigned long)ep->ExceptionRecord->ExceptionCode);
    fprintf(log, "Address: 0x%llx\n",
            (unsigned long long)ep->ExceptionRecord->ExceptionAddress);

    if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
      fprintf(log, "Access type: %s address 0x%llx\n",
              ep->ExceptionRecord->ExceptionInformation[0] == 0 ? "READ" : "WRITE",
              (unsigned long long)ep->ExceptionRecord->ExceptionInformation[1]);
    }

    fprintf(log, "\n");
    writeStackTrace(ep, log);
    fclose(log);
  }

  fprintf(stderr, "\n=== CRASH ===\nReason: %s\nAddress: 0x%llx\n",
          reason, (unsigned long long)ep->ExceptionRecord->ExceptionAddress);
  fprintf(stderr, "Full stack trace saved to crash.log\n");

  return EXCEPTION_EXECUTE_HANDLER;
}

#else

#include <execinfo.h>

static void signalHandler(int sig) {
  const char *name = "Unknown signal";
  switch (sig) {
  case SIGSEGV:
    name = "Segmentation Fault (nullptr or invalid memory)";
    break;
  case SIGABRT:
    name = "Abort (assertion failed or abort() called)";
    break;
  case SIGFPE:
    name = "Floating Point Exception (division by zero)";
    break;
  case SIGILL:
    name = "Illegal Instruction";
    break;
  }

  FILE *log = fopen("crash.log", "w");
  if (log) {
    time_t t = time(NULL);
    fprintf(log, "=== CRASH REPORT ===\n");
    fprintf(log, "Time: %s", ctime(&t));
    fprintf(log, "Signal: %d (%s)\n", sig, name);
    fprintf(log, "\nStack trace:\n");

    void *callstack[32];
    int frames = backtrace(callstack, 32);
    char **symbols = backtrace_symbols(callstack, frames);
    for (int i = 0; i < frames; i++)
      fprintf(log, "  [%d] %s\n", i, symbols[i]);
    free(symbols);
    fclose(log);
  }

  fprintf(stderr, "\n=== CRASH ===\nSignal: %d (%s)\n", sig, name);
  fprintf(stderr, "Full stack trace saved to crash.log\n");

  _exit(1);
}

#endif

namespace Engine {

void installCrashHandler() {
#ifdef _WIN32
  SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
  SetUnhandledExceptionFilter(exceptionHandler);
#else
  signal(SIGSEGV, signalHandler);
  signal(SIGABRT, signalHandler);
  signal(SIGFPE, signalHandler);
  signal(SIGILL, signalHandler);
#endif
}

} // namespace Engine

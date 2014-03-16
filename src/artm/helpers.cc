// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/helpers.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO {
  DWORD dwType;      // Must be 0x1000.
  LPCSTR szName;     // Pointer to name (in user addr space).
  DWORD dwThreadID;  // Thread ID (-1=caller thread).
  DWORD dwFlags;     // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

#endif

namespace artm {
namespace core {

#if defined(_WIN32) || defined(_WIN64)

// How to: Set a Thread Name in Native Code:
// http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
void Helpers::SetThreadName(int thread_id, const char* thread_name) {
  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = thread_name;
  info.dwThreadID = static_cast<DWORD>(thread_id);
  info.dwFlags = 0;

  __try {
    RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );  // NOLINT
  }
  __except(EXCEPTION_EXECUTE_HANDLER) {
  }
}

#else

void Helpers::SetThreadName(int thread_id, const char* thread_name) {
  // Currently not implemented for Linux.
}

#endif

}  // namespace core
}  // namespace artm

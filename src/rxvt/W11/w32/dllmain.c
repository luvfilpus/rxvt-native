#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#include "dllmain.h"

BOOL APIENTRY
DllMain (
         HINSTANCE hInst /* Library instance handle. */ ,
         DWORD reason /* Reason this function is being called. */ ,
         LPVOID reserved /* Not used. */)
{
  switch (reason)
    {
    case DLL_PROCESS_ATTACH:
#ifdef __CYGWIN__
      /* we can only do this because libW11.dll is never
       * linked directly to an app; it is always loaded
       * dynamically -- which means cygwin is already in
       * residence. Otherwise, calling this function from
       * DLL initialization code is a really bad idea.
       */
      openlog ("libW11", LOG_PID, LOG_USER);
#endif
      break;

    case DLL_PROCESS_DETACH:
#ifdef __CYGWIN__
      closelog ();
#endif
      break;

    case DLL_THREAD_ATTACH:
      break;

    case DLL_THREAD_DETACH:
      break;
    }
  return TRUE;
}

#ifdef __CYGWIN__
void w11_syslog(int priority, const char *format, ...)
{
    va_list args;
    va_start (args, format);
    vsyslog (priority, format, args);
    va_end (args);
}
#endif


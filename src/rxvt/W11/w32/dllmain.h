#ifndef W11_DLLMAIN_H
#define W11_DLLMAIN_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <stdio.h>

#ifdef __CYGWIN__
#include <stdarg.h>
#include <syslog.h>
# define SYSLOG(args) w11_syslog args
#else
# define SYSLOG(args)
#endif

#ifdef __cplusplus
extern "C" {
#endif

BOOL APIENTRY DllMain (HINSTANCE hInst, DWORD reason,
                       LPVOID reserved /* Not used. */ );
void w11_syslog(int priority, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* W11_DLLMAIN_H */


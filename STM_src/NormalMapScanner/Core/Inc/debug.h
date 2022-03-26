
#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "LCD_GUI.h"

extern GUI_Window g_window;
/// Macros on the library methods to spare some code
#define printInfo(info) printOnConsole(g_window.consoles, info)
#define printfInfo(format, ...)                                                \
    printfOnConsole(g_window.consoles, format, __VA_ARGS__)

#endif //__DEBUG_H

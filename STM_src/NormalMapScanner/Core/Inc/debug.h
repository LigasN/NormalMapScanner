
#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "LCD_GUI.h"
#include "usart.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

extern GUI_Window g_window;

#define UART
#ifndef UART
/// Macros on the library methods to spare some code
#define printInfo(info) printOnConsole(g_window.consoles, info)
#define printfInfo(format, ...)                                                \
    printfOnConsole(g_window.consoles, format, __VA_ARGS__)
#else
static void vprint(const char* fmt, va_list argp)
{
    char string[200];
    int size = vsprintf(string, fmt, argp);
    if (0 < size) // build string
    {
        HAL_UART_Transmit(&huart3, (uint8_t*)(string), strlen(string),
                          0xffffff); // send message via UART
    }
}

static void printInfo(const char* fmt, ...) // custom printf() function
{
    va_list argp;
    va_start(argp, fmt);
    vprint(fmt, argp);
    va_end(argp);
}
static void printfInfo(const char* fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    vprint(fmt, argp);
    va_end(argp);
}
#endif // UART
#endif //__DEBUG_H

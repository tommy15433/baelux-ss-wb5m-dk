#include <stdio.h>
#include <stdarg.h>
#include "utils_config.h"
#include "utils_print.h"

void utils_print_log(const char* msg, ...)
{
    va_list args;
    char tmp[1024];

    va_start(args, msg);
    vsprintf(tmp, msg, args);
    va_end(args);

    printf(ANSI_COLOR_RESET "%s\r\n" ANSI_COLOR_RESET, tmp);
}
void utils_print_info(const char* msg, ...)
{
    va_list args;
    char tmp[1024];

    va_start(args, msg);
    vsprintf(tmp, msg, args);
    va_end(args);

    printf(ANSI_COLOR_GREEN "%s\r\n" ANSI_COLOR_RESET, tmp);
}
void utils_print_err(const char* msg, ...)
{
    va_list args;
    char tmp[1024];

    va_start(args, msg);
    vsprintf(tmp, msg, args);
    va_end(args);

    printf(ANSI_COLOR_RED "%s\r\n" ANSI_COLOR_RESET, tmp);
}
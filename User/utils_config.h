#pragma once

#include <stdint.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/** maximum ringbuffer arraysize */
#define UTIL_CONF_RINGBUFFER_MAX_ALLOC      1
/** maximum ringbuffer's buffer size */
#define UTIL_CONF_RINGBUFFER_MAX_BUFFERSIZE 32
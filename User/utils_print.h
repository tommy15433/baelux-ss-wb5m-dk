#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void utils_print_log(const char* msg, ...);
void utils_print_info(const char* msg, ...);
void utils_print_err(const char* msg, ...);

#ifdef __cplusplus
}
#endif
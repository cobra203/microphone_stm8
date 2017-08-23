#ifndef STM8S_DEBUG_H
#define STM8S_DEBUG_H

#define DEBUG_ENABLE2
#define DUMP_ENABLE2

#if defined(DEBUG_ENABLE)
#include <stdio.h>
#include <stm8s.h>
#define DEBUG(fmt, ...)     printf(fmt, ##__VA_ARGS__)
#else
#define DEBUG(fmt, ...)
#endif

#if defined(DUMP_ENABLE)
#include <stdio.h>
#include <stm8s.h>
void datadump(char *title, void *data, uint32_t len);
#else
#define datadump(fmt, ...)
#endif

#endif /* STM8S_DEBUG_H */

#ifndef LOG_H
#define LOG_H

#include "config.h"

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR 3

#if !defined(LOG_LEVEL)
#define LOG_LEVEL (LOG_LEVEL_ERROR+1)
#else
#include <stdio.h>
#include <string.h>
static MQTT_INLINE const char *log_basename(const char *s)
{
#if !defined(_WIN32)
    const char *p = strrchr(s, '/');
#else
    const char *p = strrchr(s, '\\');
#endif

    if (p)
        return p+1;

    return s;
}
#endif

#define LOG_DOLOG(level, fmt, ...) \
    fprintf(stderr, "%s %s %s:%d " fmt "\n", \
        #level, __FUNCTION__, log_basename(__FILE__), __LINE__, ##__VA_ARGS__)

#if (LOG_LEVEL <= LOG_LEVEL_DEBUG)
#define LOG_DEBUG(FMT, ...) LOG_DOLOG(DEBUG, FMT, ##__VA_ARGS__)
#else
#define LOG_DEBUG(FMT, ...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_INFO)
#define LOG_INFO(FMT, ...) LOG_DOLOG(INFO, FMT, ##__VA_ARGS__)
#else
#define LOG_INFO(FMT, ...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_WARNING)
#define LOG_WARNING(FMT, ...) LOG_DOLOG(WARNING, FMT, ##__VA_ARGS__)
#else
#define LOG_WARNING(FMT, ...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_ERROR)
#define LOG_ERROR(FMT, ...) LOG_DOLOG(ERROR, FMT, ##__VA_ARGS__)
#else
#define LOG_ERROR(FMT, ...)
#endif

#endif

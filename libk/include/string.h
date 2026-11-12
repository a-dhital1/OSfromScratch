/* library's public interface */

#ifndef STRING_H
#define STRING_H

#include <stddef.h>

void *memcpy(void *restrict dest,
             const void *restrict src,
             size_t n);

void *memset(void *dest, int value, size_t n);

void *memmove(void *dest,
              const void *src,
              size_t n);

int memcmp(const void *a,
           const void *b,
           size_t n);

size_t strlen(const char *str);

#endif


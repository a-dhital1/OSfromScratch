
#include <stdint.h>
#include <stddef.h>
#include <string.h>

void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
        unsigned char *d = dest;
        const unsigned char *s = src;

        for (size_t i = 0; i < n; i++) {
                d[i] = s[i];
        }

        return dest;
}

void *memset(void *dest, int value, size_t n) {
        unsigned char *d = dest;

        for (size_t i = 0; i < n; i++) {
                d[i] = (unsigned char)value;
        }

        return dest;
}

void *memmove(void *dest, const void *src, size_t n) {
        unsigned char *d = dest;
        const unsigned char *s = src;

        if ((uintptr_t)d < (uintptr_t)s) {
                for (size_t i = 0; i < n; i++) {
                        d[i] = s[i];
                }
        } else if ((uintptr_t)d > (uintptr_t)s) {
                for (size_t i = n; i > 0; i--) {
                        d[i - 1] = s[i - 1];
                }
        }

        return dest;
}

int memcmp(const void *a, const void *b, size_t n) {
        const unsigned char *p = a;
        const unsigned char *q = b;

        for (size_t i = 0; i < n; i++) {
                if (p[i] != q[i]) {
                        return (int)p[i] - (int)q[i];
                }
        }

        return 0;
}

size_t strlen(const char *str) {
        size_t length = 0;

        while (str[length] != '\0') {
                length++;
        }

        return length;
}


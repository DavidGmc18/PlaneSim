#pragma once

#include <cstdio>
#include <cstdlib>

inline bool chk(bool res, const char* expr, const char* file, int line) {
    if (!res) {
        fprintf(stderr, "ERROR (%s) at %s:%d\n", expr, file, line);
        exit(-1);
    }
    return res;
}

template<typename T>
inline T* chk(T* res, const char* expr, const char* file, int line) {
    if (!res) {
        fprintf(stderr, "NULL pointer (%s) at %s:%d\n", expr, file, line);
        exit(-1);
    }
    return res;
}

#define CHK(x) chk((x), #x, __FILE__, __LINE__)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*(arr)))
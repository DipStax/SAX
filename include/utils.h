#ifndef UTILS_H__
#define UTILS_H__

#include <stdbool.h>

#ifdef WIN32
    #include <io.h>
    #define F_OK 0
    #define access _access
#elif
    #include <unistd.h>
#endif

#include <stdio.h>

typedef struct return_s
{
    int code;
    char *msg;
} return_t;

return_t check_open_file(char *_path, FILE **_file);
void *alloc_zero(size_t _size);
void *realloc_zero(void *_ptr, size_t _actsize, size_t _size);

char *substr(char *_dst, const char *_src, size_t _offset, size_t _size);

bool is_digit(char _c);
bool is_alpha(char _c);
bool is_alphanumeric(char _c);

#endif
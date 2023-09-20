#include <string.h>
#include <stdlib.h>

#include "utils.h"

return_t check_open_file(char *_path, FILE **_file)
{
    return_t final = { 0, NULL };

    fprintf(stderr, "[check_open_file] using file: \"%s\"\n", _path);
    if (access(_path, R_OK)) {
        final.code = 1;
        final.msg = alloc_zero(sizeof(char) * (26 + strlen(_path)));
        sprintf(final.msg, "the file '%s' doesn't exist", _path);
    } else {
        fprintf(stderr, "[check_open_file] got access right\n");
        (*_file) = fopen(_path, "r");
        fprintf(stderr, "[check_open_file] is file open: %d\n", (*_file) != NULL);
        if ((*_file) == NULL) {
            fprintf(stderr, "file note open\n");
            final.code = 1;
            final.msg = alloc_zero(sizeof(char) * (23 + strlen(_path)));
            sprintf(final.msg, "can't read the file '%s'", _path);
        }
    }
    fprintf(stderr, "[check_open_file] file open sucefully\n");
    return final;
}

void *alloc_zero(size_t _size)
{
    void *alloc = malloc(_size);

    memset(alloc, 0, _size);
    return alloc;
}

void *realloc_zero(void *_ptr, size_t _actsize, size_t _size)
{
    _ptr = realloc(_ptr, _size);
    if (_actsize < _size)
        _ptr = memset(_ptr + _actsize, 0, _actsize - _size);
    return _ptr;
}

char *substr(char *_dst, const char *_src, size_t _offset, size_t _size)
{
    memcpy(_dst, _src + _offset, _size);
    return _dst;
}

bool is_digit(char _c)
{
    return '0' <= _c && _c <= '9';
}

bool is_alpha(char _c)
{
    return ('a' <= _c && _c <= 'z') || ('A' <= _c && _c <= 'Z');
}

bool is_alphanumeric(char _c)
{
    return is_alpha(_c) || is_digit(_c);
}
#ifndef LEXER_H__
#define LEXER_H__

#ifndef TOKEN_SOURCE_SIZE
    #define TOKEN_SOURCE_SIZE 256
#endif

#include <stdbool.h>

#include "token.h"
#include "map.h"
#include "list.h"

typedef struct lexer_s
{
    size_t start;
    size_t current;
    size_t offset;

    FILE *file;
    char *source;
    size_t source_size;

    list_t *list;
    map_t *build_in;
} lexer_t;

typedef void (*fn_lexer)(lexer_t *);

lexer_t *lexer_create(FILE *_file);
lexer_t *lexer_create_wlist(FILE *_file, list_t *_list);
void lexer_destroy(lexer_t *_lexer);
void lexer_destroy_void(void *_lexer);

void lexer_source_append(lexer_t *_lexer);
void lexer_source_fetch(lexer_t *_lexer);
bool lexer_is_end(lexer_t *_lexer, fn_lexer _callback);

bool lexer_match(lexer_t *_lexer, char _c);
char lexer_peek(lexer_t *_lexer);
char lexer_peek_next(lexer_t *_lexer);
char lexer_next(lexer_t *_lexer);

void lexer_get_string(lexer_t *_lexer, token_t *_token);
void lexer_get_number(lexer_t *_lexer, token_t *_token);
void lexer_get_identifier(lexer_t *_lexer, token_t *_token);

void fast_push_strint(map_t *_map, char *_key, int _value);
bool str_map_fn_cmp(void *_left, void *_right);
void str_map_fn_del(void *_data, bool _side);

void lexer_scan(lexer_t *_lexer);

return_t lexer_start(FILE *_file, list_t *_list);

#endif
#ifndef TOKENIZER_H__
#define TOKENIZER_H__

#ifndef TOKEN_SOURCE_SIZE
    #define TOKEN_SOURCE_SIZE 256
#endif

#include <stdbool.h>

#include "token.h"
#include "map.h"

typedef struct tokenizer_s
{
    size_t start;
    size_t current;
    size_t offset;

    FILE *file;
    char *source;
    size_t source_size;

    token_list_t *list;
    map_t *build_in;
} tokenizer_t;

typedef void (*fn_tokenizer)(tokenizer_t *);

tokenizer_t *tokenizer_create(FILE *_file);
tokenizer_t *tokenizer_create_wlist(FILE *_file, token_list_t *_list);

void tokenizer_source_append(tokenizer_t *_tokenizer);
void tokenizer_source_fetch(tokenizer_t *_tokenizer);
bool tokenizer_is_end(tokenizer_t *_tokenizer, fn_tokenizer _callback);

bool tokenizer_match(tokenizer_t *_tokenizer, char _c);
char tokenizer_peek(tokenizer_t *_tokenizer);
char tokenizer_peek_next(tokenizer_t *_tokenizer);
char tokenizer_next(tokenizer_t *_tokenizer);

void tokenizer_get_string(tokenizer_t *_tokenizer, token_t *_token);
void tokenizer_get_number(tokenizer_t *_tokenizer, token_t *_token);
void tokenizer_get_identifier(tokenizer_t *_tokenizer, token_t *_token);

void fast_push_strint(map_t *_map, char *_key, int _value);
bool str_map_fn_cmp(void *_left, void *_right);
void str_map_fn_del(void *_data, bool _side);

void tokenizer_scan(tokenizer_t *_tokenizer);

return_t tokenize(FILE *_file, token_list_t *_list);

#endif
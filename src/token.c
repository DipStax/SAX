#include <string.h>
#include <stdlib.h>

#include "token.h"

token_t *token_create(token_e _type, char *_lexem, void *_literal)
{
    char *lexem = _lexem != NULL ? strdup(_lexem) : NULL;
    char *literal = _literal != NULL ? strdup(_literal) : NULL;

    return token_create_noalloc(_type, lexem, literal);
}

token_t *token_create_noalloc(token_e _type, char *_lexem, void *_literal)
{
    token_t *token = alloc_zero(sizeof(token_t));

    token->type = _type;
    token->lexeme = _lexem;
    token->literal = _literal;
    return token;
}

void token_destroy(token_t *_token)
{
    if (_token->lexeme)
        free(_token->lexeme);
    if (_token->literal)
        free(_token->literal);
    free(_token);
}

void token_destroy_void(void *_token)
{
    token_destroy((token_t *)_token);
}
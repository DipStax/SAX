#include <stdlib.h>
#include <string.h>
#define _GNU_SOURCE
#include <stdio.h>

#include "tokenizer.h"

tokenizer_t *tokenizer_create(FILE *_file)
{
    tokenizer_t *tokenizer = alloc_zero(sizeof(tokenizer_t));

    tokenizer_create_wlist(_file, list_create(0, token_destroy_void));
    return tokenizer;
}

tokenizer_t *tokenizer_create_wlist(FILE *_file, list_t *_list)
{
    tokenizer_t *tokenizer = alloc_zero(sizeof(tokenizer_t));

    tokenizer->list = _list;
    tokenizer->file = _file;
    tokenizer->build_in = map_create(str_map_fn_cmp, str_map_fn_del);
    fast_push_strint(tokenizer->build_in, "class", Class);
    fast_push_strint(tokenizer->build_in, "while", While);
    fast_push_strint(tokenizer->build_in, "for", For);
    fast_push_strint(tokenizer->build_in, "if", If);
    fast_push_strint(tokenizer->build_in, "else", Else);
    fast_push_strint(tokenizer->build_in, "return", Return);
    fast_push_strint(tokenizer->build_in, "null", Null);
    fast_push_strint(tokenizer->build_in, "true", True);
    fast_push_strint(tokenizer->build_in, "false", False);
    return tokenizer;
}

void tokenizer_destroy(tokenizer_t *_tokenizer)
{
    if (_tokenizer->source)
        free(_tokenizer->source);
    map_destroy(_tokenizer->build_in);
    if (_tokenizer->file)
        fclose(_tokenizer->file);
    free(_tokenizer);
}

void tokenizer_destroy_void(void *_tokenizer)
{
    tokenizer_destroy((tokenizer_t *)_tokenizer);
}

void tokenizer_source_append(tokenizer_t *_tokenizer)
{
    size_t read = TOKEN_SOURCE_SIZE;
    char *append = NULL;
    size_t old_source_size = _tokenizer->source_size;
    size_t block_size = 0;

    if (_tokenizer->current == _tokenizer->source_size) {
        block_size = getline(&append, &read, _tokenizer->file);
        if (block_size == -1)
            return;
        _tokenizer->source_size += block_size;
        _tokenizer->source = realloc_zero(_tokenizer->source, old_source_size, _tokenizer->source_size + 1);
        _tokenizer->source = strcat(_tokenizer->source, append);
    }
}

void tokenizer_source_fetch(tokenizer_t *_tokenizer)
{
    size_t read = TOKEN_SOURCE_SIZE;
    char *append = NULL;
    size_t block_size = 0;

    if (_tokenizer->source_size == 0 || _tokenizer->current == _tokenizer->source_size) {
        block_size = getline(&append, &read, _tokenizer->file);
        if (block_size != -1) {
            _tokenizer->source = append;
            _tokenizer->offset += _tokenizer->source_size;
            _tokenizer->source_size = block_size;
            _tokenizer->current = 0;
        } else {
            free(append);
        }
    }
}

bool tokenizer_is_end(tokenizer_t *_tokenizer, fn_tokenizer _callback)
{
    _callback(_tokenizer);
    return _tokenizer->current == _tokenizer->source_size;
}

bool tokenizer_match(tokenizer_t *_tokenizer, char _c)
{
    if (tokenizer_is_end(_tokenizer, tokenizer_source_append))
        return false;
    if (_tokenizer->source[_tokenizer->current] != _c)
        return false;
    _tokenizer->current++;
    return true;
}

char tokenizer_peek(tokenizer_t *_tokenizer)
{
    if (tokenizer_is_end(_tokenizer, tokenizer_source_append))
        return 0;
    return _tokenizer->source[_tokenizer->current];
}

char tokenizer_peek_next(tokenizer_t *_tokenizer)
{
    if (tokenizer_is_end(_tokenizer, tokenizer_source_append))
        return 0;
    return _tokenizer->source[_tokenizer->current + 1];
}

char tokenizer_next(tokenizer_t *_tokenizer)
{
    if (tokenizer_is_end(_tokenizer, tokenizer_source_append))
        return 0;
    return _tokenizer->source[_tokenizer->current++];
}

void tokenizer_get_string(tokenizer_t *_tokenizer, token_t *_token)
{
    while (!tokenizer_is_end(_tokenizer, tokenizer_source_append) && tokenizer_peek(_tokenizer) != '"')
        _tokenizer->current++;
    if (tokenizer_is_end(_tokenizer, tokenizer_source_append))
        return;
    _token->lexeme = alloc_zero(sizeof(char) * (_tokenizer->current - _tokenizer->start + 1));
    _token->lexeme = substr(_token->lexeme, _tokenizer->source, _tokenizer->start, _tokenizer->current);
    _token->literal = alloc_zero(sizeof(char) * (_tokenizer->current - (_tokenizer->start + 2)));
    _token->literal = substr(_token->literal, _tokenizer->source, _tokenizer->start + 1, _tokenizer->current - 1);
    _tokenizer->current++;
    _token->type = String;
}

void tokenizer_get_number(tokenizer_t *_tokenizer, token_t *_token)
{
    char *ptr = NULL;
    bool is_float = false;

    while (!tokenizer_is_end(_tokenizer, tokenizer_source_append) && is_digit(tokenizer_peek(_tokenizer)))
        _tokenizer->current++;
    if (tokenizer_is_end(_tokenizer, tokenizer_source_append))
        return;
    is_float = tokenizer_peek(_tokenizer) == '.';
    if (is_float && is_digit(tokenizer_peek_next(_tokenizer))) {
        _tokenizer->current++;
        while (!tokenizer_is_end(_tokenizer, tokenizer_source_append) && is_digit(tokenizer_peek(_tokenizer)))
            _tokenizer->current++;
        if (tokenizer_is_end(_tokenizer, tokenizer_source_append))
            return;
        _token->type = NumFloat;
    } else {
        _token->type = NumInt;
    }
    _token->lexeme = alloc_zero(sizeof(char) * (_tokenizer->current - _tokenizer->start + 1));
    _token->lexeme = substr(_token->lexeme, _tokenizer->source, _tokenizer->start, _tokenizer->current);
    if (is_float) {
        double double_value = strtod(_token->lexeme, &ptr);

        _token->literal = alloc_zero(sizeof(double));
        memcpy(_token->literal, &double_value, sizeof(double));
    } else {
        int int_value = atoi(_token->lexeme);

        _token->literal = alloc_zero(sizeof(int));
        memcpy(_token->literal, &int_value, sizeof(int));
    }
}

void tokenizer_get_identifier(tokenizer_t *_tokenizer, token_t *_token)
{
    char *key = NULL;
    node_pair_t *it = NULL;

    while (!tokenizer_is_end(_tokenizer, tokenizer_source_append) && is_alphanumeric(tokenizer_peek(_tokenizer)))
        _tokenizer->current++;
    key = alloc_zero(sizeof(char) * (_tokenizer->current - _tokenizer->start + 1));
    key = substr(key, _tokenizer->source, _tokenizer->start, _tokenizer->current);
    it = map_find(_tokenizer->build_in, key);

    if (it) {
        _token->type = *((int *)(it->value));
        _token->lexeme = NULL;
        free(key);
    } else {
        _token->type = Identifier;
        _token->lexeme = key;
        _token->literal = malloc(sizeof(char) * (_tokenizer->current - _tokenizer->start + 1));
        memcpy(_token->literal, _token->lexeme, _tokenizer->current - _tokenizer->start);
    }
}

void fast_push_strint(map_t *_map, char *_key, int _value)
{
    MAP_PARAM(int, value, _value);

    map_push(_map, _key, value);
}

bool str_map_fn_cmp(void *_left, void *_right)
{
    return strcmp((const char *)_left, (const char *)_right) == 0;
}

void str_map_fn_del(void *_data, bool _size)
{
    free(_data);
}

void tokenizer_scan(tokenizer_t *_tokenizer)
{
    char c = tokenizer_next(_tokenizer);
    token_t *token = token_create(Eof, NULL, NULL);

    switch (c) {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            token_destroy(token);
            return;
        case '(': token->type = ClosePar;
            break;
        case ')': token->type = ClosePar;
            break;
        case '{': token->type = OpenBracket;
            break;
        case '}': token->type = CloseBracket;
            break;
        case ',': token->type = Comma;
            break;
        case ';': token->type = SemiColon;
            break;
        case '.': token->type = Point;
            break;
        case '+': token->type = tokenizer_match(_tokenizer, '=') ? PlusEq : Plus;
            break;
        case '-': token->type = tokenizer_match(_tokenizer, '=') ? MinusEq : Minus;
            break;
        case '*':  token->type = tokenizer_match(_tokenizer, '=') ? MulEq : Mul;
            break;
        case '/':
            if (tokenizer_match(_tokenizer, '/')) {
                while (tokenizer_peek(_tokenizer) && tokenizer_is_end(_tokenizer, tokenizer_source_append))
                    _tokenizer->current++;
                token_destroy(token);
                return;
            } else {
                token->type = tokenizer_match(_tokenizer, '=') ? DivEq : Div;
            }
            break;
        case '%':  token->type = tokenizer_match(_tokenizer, '=') ? ModEq : Mod;
            break;
        case '^':  token->type = tokenizer_match(_tokenizer, '=') ? XorEq : Xor;
            break;
        case '&':  token->type = tokenizer_match(_tokenizer, '=') ? AndEq : And;
            break;
        case '|': token->type = tokenizer_match(_tokenizer, '=') ? OrEq : Or;
            break;
        case '<': token->type = tokenizer_match(_tokenizer, '=') ? LsEq : Ls;
            break;
        case '>': token->type = tokenizer_match(_tokenizer, '=') ? GtEq : Gt;
            break;
        case '=': token->type = tokenizer_match(_tokenizer, '=') ? EqEq : Eq;
            break;
        case '"': tokenizer_get_string(_tokenizer, token);
            break;
        default:
            if (is_digit(c))
                tokenizer_get_number(_tokenizer, token);
            else if (is_alpha(c))
                tokenizer_get_identifier(_tokenizer, token);
    }
    list_push(_tokenizer->list, (void *)token);
}

return_t tokenize(FILE *_file, list_t *_list)
{
    return_t final = { 0, NULL };
    tokenizer_t *tokenizer = NULL;

    if (_list == NULL) {
        final.code = 1;
        final.msg = alloc_zero(sizeof(char) * 21);
        sprintf(final.msg, "List not initialized");
        return final;
    }
    tokenizer = tokenizer_create_wlist(_file, _list);

    tokenizer_source_fetch(tokenizer);
    while (!tokenizer_is_end(tokenizer, tokenizer_source_fetch)) {
        tokenizer->start = tokenizer->current;
        tokenizer_scan(tokenizer);
        if (((token_t *)(tokenizer->list->__last->value))->type == Eof) {
            final.code = 1;
            final.msg = alloc_zero(sizeof(char) * 16);
            sprintf(final.msg, "Token eof reach");
            break;
        }
    }
    list_push(tokenizer->list, token_create(Eof, "", NULL));
    tokenizer_destroy(tokenizer);
    return final;
}
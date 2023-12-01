#include <stdlib.h>
#include <string.h>
#define _GNU_SOURCE
#include <stdio.h>

#include "lexer.h"

lexer_t *lexer_create(FILE *_file)
{
    lexer_t *lexer = alloc_zero(sizeof(lexer_t));

    lexer_create_wlist(_file, list_create(0, token_destroy_void));
    return lexer;
}

lexer_t *lexer_create_wlist(FILE *_file, list_t *_list)
{
    lexer_t *lexer = alloc_zero(sizeof(lexer_t));

    lexer->list = _list;
    lexer->file = _file;
    lexer->build_in = map_create(str_map_fn_cmp, str_map_fn_del);
    fast_push_strint(lexer->build_in, "class", Class);
    fast_push_strint(lexer->build_in, "while", While);
    fast_push_strint(lexer->build_in, "for", For);
    fast_push_strint(lexer->build_in, "if", If);
    fast_push_strint(lexer->build_in, "else", Else);
    fast_push_strint(lexer->build_in, "return", Return);
    fast_push_strint(lexer->build_in, "null", Null);
    fast_push_strint(lexer->build_in, "true", True);
    fast_push_strint(lexer->build_in, "false", False);
    return lexer;
}

void lexer_destroy(lexer_t *_lexer)
{
    if (_lexer->source)
        free(_lexer->source);
    map_destroy(_lexer->build_in);
    if (_lexer->file)
        fclose(_lexer->file);
    free(_lexer);
}

void lexer_destroy_void(void *_lexer)
{
    lexer_destroy((lexer_t *)_lexer);
}

void lexer_source_append(lexer_t *_lexer)
{
    size_t read = TOKEN_SOURCE_SIZE;
    char *append = NULL;
    size_t old_source_size = _lexer->source_size;
    size_t block_size = 0;

    if (_lexer->current == _lexer->source_size) {
        block_size = getline(&append, &read, _lexer->file);
        if (block_size == -1)
            return;
        _lexer->source_size += block_size;
        _lexer->source = realloc_zero(_lexer->source, old_source_size, _lexer->source_size + 1);
        _lexer->source = strcat(_lexer->source, append);
    }
}

void lexer_source_fetch(lexer_t *_lexer)
{
    size_t read = TOKEN_SOURCE_SIZE;
    char *append = NULL;
    size_t block_size = 0;

    if (_lexer->source_size == 0 || _lexer->current == _lexer->source_size) {
        block_size = getline(&append, &read, _lexer->file);
        if (block_size != -1) {
            _lexer->source = append;
            _lexer->offset += _lexer->source_size;
            _lexer->source_size = block_size;
            _lexer->current = 0;
        } else {
            free(append);
        }
    }
}

bool lexer_is_end(lexer_t *_lexer, fn_lexer _callback)
{
    _callback(_lexer);
    return _lexer->current == _lexer->source_size;
}

bool lexer_match(lexer_t *_lexer, char _c)
{
    if (lexer_is_end(_lexer, lexer_source_append))
        return false;
    if (_lexer->source[_lexer->current] != _c)
        return false;
    _lexer->current++;
    return true;
}

char lexer_peek(lexer_t *_lexer)
{
    if (lexer_is_end(_lexer, lexer_source_append))
        return 0;
    return _lexer->source[_lexer->current];
}

char lexer_peek_next(lexer_t *_lexer)
{
    if (lexer_is_end(_lexer, lexer_source_append))
        return 0;
    return _lexer->source[_lexer->current + 1];
}

char lexer_next(lexer_t *_lexer)
{
    if (lexer_is_end(_lexer, lexer_source_append))
        return 0;
    return _lexer->source[_lexer->current++];
}

void lexer_get_string(lexer_t *_lexer, token_t *_token)
{
    while (!lexer_is_end(_lexer, lexer_source_append) && lexer_peek(_lexer) != '"')
        _lexer->current++;
    if (lexer_is_end(_lexer, lexer_source_append))
        return;
    _token->lexeme = alloc_zero(sizeof(char) * (_lexer->current - _lexer->start + 1));
    _token->lexeme = substr(_token->lexeme, _lexer->source, _lexer->start, _lexer->current);
    _token->literal = alloc_zero(sizeof(char) * (_lexer->current - (_lexer->start + 2)));
    _token->literal = substr(_token->literal, _lexer->source, _lexer->start + 1, _lexer->current - 1);
    _lexer->current++;
    _token->type = String;
}

void lexer_get_number(lexer_t *_lexer, token_t *_token)
{
    char *ptr = NULL;
    bool is_float = false;

    while (!lexer_is_end(_lexer, lexer_source_append) && is_digit(lexer_peek(_lexer)))
        _lexer->current++;
    if (lexer_is_end(_lexer, lexer_source_append))
        return;
    is_float = lexer_peek(_lexer) == '.';
    if (is_float && is_digit(lexer_peek_next(_lexer))) {
        _lexer->current++;
        while (!lexer_is_end(_lexer, lexer_source_append) && is_digit(lexer_peek(_lexer)))
            _lexer->current++;
        if (lexer_is_end(_lexer, lexer_source_append))
            return;
        _token->type = NumFloat;
    } else {
        _token->type = NumInt;
    }
    _token->lexeme = alloc_zero(sizeof(char) * (_lexer->current - _lexer->start + 1));
    _token->lexeme = substr(_token->lexeme, _lexer->source, _lexer->start, _lexer->current - _lexer->start);
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

void lexer_get_identifier(lexer_t *_lexer, token_t *_token)
{
    char *key = NULL;
    node_pair_t *it = NULL;

    while (!lexer_is_end(_lexer, lexer_source_append) && is_alphanumeric(lexer_peek(_lexer)))
        _lexer->current++;
    key = alloc_zero(sizeof(char) * (_lexer->current - _lexer->start + 1));
    key = substr(key, _lexer->source, _lexer->start, _lexer->current - _lexer->start);
    it = map_find(_lexer->build_in, key);

    if (it) {
        _token->type = *((int *)(it->value));
        _token->lexeme = NULL;
        free(key);
    } else {
        _token->type = Identifier;
        _token->lexeme = key;
        _token->literal = strdup(key);
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

void lexer_scan(lexer_t *_lexer)
{
    char c = lexer_next(_lexer);
    token_t *token = token_create(Eof, NULL, NULL);

    switch (c) {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            token_destroy(token);
            return;
        case '(': token->type = OpenPar;
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
        case '+': token->type = lexer_match(_lexer, '=') ? PlusEq : Plus;
            break;
        case '-': token->type = lexer_match(_lexer, '=') ? MinusEq : Minus;
            break;
        case '*':  token->type = lexer_match(_lexer, '=') ? MulEq : Mul;
            break;
        case '/':
            if (lexer_match(_lexer, '/')) {
                while (lexer_peek(_lexer) && lexer_is_end(_lexer, lexer_source_append))
                    _lexer->current++;
                token_destroy(token);
                return;
            } else {
                token->type = lexer_match(_lexer, '=') ? DivEq : Div;
            }
            break;
        case '%':  token->type = lexer_match(_lexer, '=') ? ModEq : Mod;
            break;
        case '^':  token->type = lexer_match(_lexer, '=') ? XorEq : Xor;
            break;
        case '&':  token->type = lexer_match(_lexer, '=') ? AndEq : And;
            break;
        case '|': token->type = lexer_match(_lexer, '=') ? OrEq : Or;
            break;
        case '<': token->type = lexer_match(_lexer, '=') ? LsEq : Ls;
            break;
        case '>': token->type = lexer_match(_lexer, '=') ? GtEq : Gt;
            break;
        case '=': token->type = lexer_match(_lexer, '=') ? EqEq : Eq;
            break;
        case '!': token->type = lexer_match(_lexer, '=') ? NotEq : Not;
            break;
        case '"': lexer_get_string(_lexer, token);
            break;
        default:
            if (is_digit(c))
                lexer_get_number(_lexer, token);
            else if (is_alpha(c))
                lexer_get_identifier(_lexer, token);
    }
    list_push(_lexer->list, (void *)token);
}

return_t lexer_run(FILE *_file, list_t *_list)
{
    return_t final = { 0, NULL };
    lexer_t *lexer = NULL;

    if (_list == NULL) {
        final.code = 1;
        final.msg = alloc_zero(sizeof(char) * 21);
        sprintf(final.msg, "List not initialized");
        return final;
    }
    lexer = lexer_create_wlist(_file, _list);

    lexer_source_fetch(lexer);
    while (!lexer_is_end(lexer, lexer_source_fetch)) {
        lexer->start = lexer->current;
        lexer_scan(lexer);
        if (((token_t *)(lexer->list->__last->value))->type == Eof) {
            final.code = 1;
            final.msg = alloc_zero(sizeof(char) * 16);
            sprintf(final.msg, "Token eof reach");
            break;
        }
    }
    list_push(lexer->list, token_create(Eof, "", NULL));
    lexer_destroy(lexer);
    return final;
}
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"

tokenizer_t *tokenizer_create(FILE *_file)
{
    tokenizer_t *tokenizer = alloc_zero(sizeof(tokenizer_t));

    tokenizer->list = token_list_create();
    tokenizer->file = _file;
    return tokenizer;
}

tokenizer_t *tokenizer_create_wlist(FILE *_file, token_list_t *_list)
{
    tokenizer_t *tokenizer = alloc_zero(sizeof(tokenizer_t));
    // fprintf(stderr, "[tokenizer_create_wlist] allocated pointer: %p\n", tokenizer);

    tokenizer->list = _list;
    tokenizer->file = _file;
    // fprintf(stderr, "[tokenizer_create_wlist] result: %p, %p, %d, %d, %d\n", tokenizer->list, tokenizer->file, tokenizer->offset, tokenizer->start, tokenizer->current);
    return tokenizer;
}

void tokenizer_source_append(tokenizer_t *_tokenizer)
{
    size_t read = TOKEN_SOURCE_SIZE;
    char *append = NULL;
    size_t old_source_size = _tokenizer->source_size;
    size_t block_size = 0;

    // fprintf(stderr, "[tokenizer_source_append]: start\n");
    if (_tokenizer->current == _tokenizer->source_size) {
        // fprintf(stderr, "[tokenizer_source_append] source_size: %d, current: %d\n", _tokenizer->source_size, _tokenizer->current);
        block_size += getline(&append, &read, _tokenizer->file);
        if (block_size == -1)
            return;
        _tokenizer->source_size += block_size;
        // fprintf(stderr, "[tokenizer_source_append] appending with: \"%s\", source_size: %d\n", append, _tokenizer->source_size);
        _tokenizer->source = realloc_zero(_tokenizer->source, old_source_size, _tokenizer->source_size + 1);
        // fprintf(stderr, "[tokenizer_source_append] appending done: %d, source_size: \"%s\", append: \"%s\"\n", _tokenizer->source_size + 1, _tokenizer->source, append);
        _tokenizer->source = strcat(_tokenizer->source, append);
        // fprintf(stderr, "[tokenizer_source_append] final source: \"%s\", source_size: %d\n", _tokenizer->source, _tokenizer->source_size);
    }
    // fprintf(stderr, "[tokenizer_source_append] end\n");
}

void tokenizer_source_fetch(tokenizer_t *_tokenizer)
{
    size_t read = TOKEN_SOURCE_SIZE;
    char *append = NULL;
    size_t block_size = 0;

    // fprintf(stderr, "[tokenizer_source_fetch]: start >\n");
    // fprintf(stderr, "[tokenizer_source_fetch] source_size: %d, current: %d\n", _tokenizer->source_size, _tokenizer->current);
    if (_tokenizer->source_size == 0 || _tokenizer->current == _tokenizer->source_size) {
        // fprintf(stderr, "[tokenizer_source_fetch] offset: %d > ", _tokenizer->offset);
        // fprintf(stderr, "new offset: %d\n", _tokenizer->offset);
        block_size = getline(&append, &read, _tokenizer->file);
        if (block_size != -1) {
            _tokenizer->source = append;
            _tokenizer->offset += _tokenizer->source_size;
            _tokenizer->source_size = block_size;
            // fprintf(stderr, "[tokenizer_source_fetch] number of element read: %d\n", read);
            // fprintf(stderr, "[tokenizer_source_fetch] new source: \"%s\", size: %d\n", _tokenizer->source, _tokenizer->source_size);
            _tokenizer->current = 0;
        } else {
            free(append);
        }
    }
    // fprintf(stderr, "[tokenizer_source_fetch]: end >\n");
}

bool tokenizer_is_end(tokenizer_t *_tokenizer, fn_tokenizer _callback)
{
    // fprintf(stderr, "[tokenizer_is_end] start\n");
    _callback(_tokenizer);
    // fprintf(stderr, "[tokenizer_is_end] end > %d == %d = %d---\n", _tokenizer->current, _tokenizer->source_size - 1, _tokenizer->current == _tokenizer->source_size - 1);
    return _tokenizer->current == _tokenizer->source_size;
}

bool tokenizer_match(tokenizer_t *_tokenizer, char _c)
{
    if (tokenizer_is_end(_tokenizer, tokenizer_source_append))
        return false;
    // fprintf(stderr, "[tokenizer_match] start\n");
    // fprintf(stderr, "[tokenizer_match] current: %d, expected: %c, char %c\n", _tokenizer->current, _c, _tokenizer->source[_tokenizer->current]);
    if (_tokenizer->source[_tokenizer->current] != _c)
        return false;
    _tokenizer->current++;
    // fprintf(stderr, "[tokenizer_match] new current: %d\n", _tokenizer->current);
    // fprintf(stderr, "[tokenizer_match] end\n");
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
    // fprintf(stderr, "[tokenizer_get_identifier] current cursor: %d\n", _tokenizer->current);
    while (!tokenizer_is_end(_tokenizer, tokenizer_source_append) && is_alphanumeric(tokenizer_peek(_tokenizer)))
        _tokenizer->current++;
    if (tokenizer_is_end(_tokenizer, tokenizer_source_append))
        return;
    // fprintf(stderr, "[tokenizer_get_identifier] new current cursor: %d\n", _tokenizer->current);
    _token->type = Identifier;
    _token->lexeme = malloc(sizeof(char) * (_tokenizer->current - _tokenizer->start + 1));
    _token->lexeme = substr(_token->lexeme, _tokenizer->source, _tokenizer->start, _tokenizer->current);
    _token->literal = malloc(sizeof(char) * (_tokenizer->current - _tokenizer->start + 1));
    memcpy(_token->literal, _token->lexeme, _tokenizer->current - _tokenizer->start);
}

void tokenizer_scan(tokenizer_t *_tokenizer)
{
    // fprintf(stderr, "[tokenizer_scan] start\n");
    char c = tokenizer_next(_tokenizer);
    // fprintf(stderr, "[tokenizer_scan] lexing char: %c\n", c);
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
    token_list_append(_tokenizer->list, token);
}

return_t tokenize(FILE *_file, token_list_t *_list)
{
    return_t final = { 0, NULL };
    tokenizer_t *tokenizer = NULL;

    if (_list == NULL) {
        // fprintf(stderr, "tokenize: token_list not init");
        final.code = 1;
        final.msg = alloc_zero(sizeof(char) * 40);
        // sprintf(final.msg, "tokenize: list of token not initialized");
        return final;
    }
    // fprintf(stderr, "[tokenize] call tokenizer_create_wlist\n");
    tokenizer = tokenizer_create_wlist(_file, _list);
    // fprintf(stderr, "[tokenize] tokenizer: pointer: %p\n", tokenizer);
    // fprintf(stderr, "[tokenize] tokenizer: offset: %d\n", tokenizer->offset);
    // fprintf(stderr, "[tokenize] tokenizer: start: %d\n", tokenizer->start);
    // fprintf(stderr, "[tokenize] tokenizer: current: %d\n", tokenizer->current);
    // fprintf(stderr, "--------------------------------------------------\n");

    tokenizer_source_fetch(tokenizer);
    // fprintf(stderr, "[tokenize] entering tokenizing loop: %d\n", tokenizer->source_size);
    // fprintf(stderr, "[tokenize] source: \"%s\"\n", tokenizer->source);
    while (!tokenizer_is_end(tokenizer, tokenizer_source_fetch)) {
        // fprintf(stderr, ">>>>>\n");
        // fprintf(stderr, "[tokenize] iteration { offset: %d, start: %d, current: %d }, source_size: %d\n", tokenizer->offset, tokenizer->start, tokenizer->current, tokenizer->source_size);
        tokenizer->start = tokenizer->current;
        tokenizer_scan(tokenizer);
        if (tokenizer->list->__last->token->type == Eof) {
            final.code = 1;
            final.msg = alloc_zero(sizeof(char) * 34);
            // sprintf(final.msg, "tokenize: EOF reached before end?");
            break;
        }
        // fprintf(stderr, "[tokenize] done with this new value: { offset: %d, start: %d, current: %d }\n", tokenizer->offset, tokenizer->start, tokenizer->current);
        // fprintf(stderr, "<<<<<\n");
    }
    token_list_append(_list, token_create(Eof, "", NULL));
    return final;
}
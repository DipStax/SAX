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

node_token_t *node_token_create(token_t *_token)
{
    node_token_t *node = alloc_zero(sizeof(node_token_t));

    node->token = _token;
    return node;
}

void node_token_destroy(node_token_t *_node)
{
    if (_node->prev)
        _node->prev->next = _node->next;
    if (_node->next)
        _node->next->prev = _node->prev;
    token_destroy(_node->token);
    free(_node);
}

node_token_t *priv_node_token_at(node_token_t *_head, size_t _pos)
{
    node_token_t *node = _head;

    for (size_t it = 0; it < _pos; it++)
        node = node->next;
    return node;
}

token_list_t *token_list_create()
{
    return (token_list_t *)alloc_zero(sizeof(token_list_t));
}

size_t token_list_append(token_list_t *_list, token_t *_token)
{
    node_token_t *node = node_token_create(_token);

    // fprintf(stderr, "[node_token_t] append { %d, \"%s\", %p }\n", _token->type, _token->lexeme, _token->literal);
    if (_list->__last) {
        _list->__last->next = node;
        _list->__last->next->prev = _list->__last;
        _list->__last = _list->__last->next;
    } else {
        _list->list = node;
        _list->__last = node;
    }
    _list->size++;
    return _list->size;
}

size_t token_list_erase(token_list_t *_list, size_t _pos)
{
    node_token_t *node = priv_node_token_at(_list->list, _pos);

    if (_pos == 0)
        _list->list = node->next;
    if (_pos == _list->size)
        _list->__last = node->prev;
    node_token_destroy(node);
    _list->size--;
    return _list->size;
}

token_t *token_list_at(token_list_t *_list, size_t _pos)
{
    return priv_node_token_at(_list->list, _pos)->token;
}

void token_list_destroy(token_list_t *_list)
{
    while (_list->size != 0)
        token_list_erase(_list, 0);
    free(_list);
}
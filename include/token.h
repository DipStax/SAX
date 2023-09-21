#ifndef TOKEN_H__
#define TOKEN_H__

#include "utils.h"

typedef enum token_e {
    Class,
    Func,
    Identifier,
    Type,
    // 3
    String, NumFloat, NumInt,
    // 6
    Eq, Ls, Gt,
    EqEq, LsEq, GtEq,
    // 12
    Plus, Minus, Mul, Div, Mod,
    PlusEq, MinusEq, MulEq, DivEq, ModEq,
    // 22
    Xor, And, Or,
    XorEq, AndEq, OrEq,
    // 28
    OpenBracket, CloseBracket, OpenPar, ClosePar,
    // 32
    Comma, SemiColon, TwoPoint, Point,
    Slash,
    Eof
} token_e;

typedef struct token_s
{
    token_e type;
    char *lexeme;
    void *literal;
} token_t;

token_t *token_create(token_e _type, char *_lexem, void *_literal);
token_t *token_create_noalloc(token_e _type, char *_lexem, void *_literal);
void token_destroy(token_t *_token);

typedef struct node_token_s
{
    token_t *token;
    struct node_token_s *next;
    struct node_token_s *prev;
} node_token_t;

node_token_t *node_token_create(token_t *_token);
void node_token_destroy(node_token_t *_node);

node_token_t *priv_node_token_at(node_token_t *_head, size_t _pos);

typedef struct token_list_s
{
    size_t size;
    node_token_t *list;
    node_token_t *__last;
} token_list_t;

token_list_t *token_list_create();
void token_list_destroy(token_list_t *_list);

size_t token_list_append(token_list_t *_list, token_t *_token);
token_t *token_list_at(token_list_t *_list, size_t _pos);
size_t token_list_erase(token_list_t *_list, size_t _pos);

#endif
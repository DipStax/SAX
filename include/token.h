#ifndef TOKEN_H__
#define TOKEN_H__

#include "utils.h"

typedef enum token_e {
    Class,
    While, For,
    If, Else,
    Identifier,
    Type, Return,
    String, NumFloat, NumInt,
    Eq, Ls, Gt,
    EqEq, LsEq, GtEq,
    Plus, Minus, Mul, Div, Mod,
    PlusEq, MinusEq, MulEq, DivEq, ModEq,
    Xor, And, Or,
    XorEq, AndEq, OrEq,
    OpenBracket, CloseBracket, OpenPar, ClosePar,
    Comma, SemiColon, TwoPoint, Point,
    Null, True, False,
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
void token_destroy_void(void *_token);

#endif
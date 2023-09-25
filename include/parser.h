#ifndef PARSER_H__
#define PARSER_H__

#include "expression.h"
#include "token.h"
#include "list.h"

typedef struct parser_s
{
    list_t *token;
    size_t cursor;
} parser_t;

parser_t *parser_create(list_t *_token);
void parser_destroy(parser_t *_parser);

expression_t *parser_run(parser_t *_parser);

token_t *parser_peek(parser_t *_parser);
token_t *parser_previous(parser_t *_parser);

bool parser_match(parser_t *_parser, size_t _count, ...);

expression_t *parser_equality(parser_t *_parser);
expression_t *parser_comparison(parser_t *_parser);
expression_t *parser_term(parser_t *_parser);
expression_t *parser_factor(parser_t *_parser);
expression_t *parser_unary(parser_t *_parser);
expression_t *parser_primary(parser_t *_parser);


#endif
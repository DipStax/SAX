#include <stdlib.h>
#include <stdarg.h>

#include "parser.h"
#include "utils.h"

parser_t *parser_create(list_t *_token)
{
    parser_t *parser = alloc_zero(sizeof(parser_t));

    parser->token = _token;
    return parser;
}

void parser_destroy(parser_t *_parser)
{
    free(_parser);
}

expression_t *parser_run(parser_t *_parser)
{
    return parser_equality(_parser);
}

token_t *parser_peek(parser_t *_parser)
{
    return (token_t *)(list_at(_parser->token, _parser->cursor));
}

token_t *parser_previous(parser_t *_parser)
{
    return (token_t *)(list_at(_parser->token, _parser->cursor - 1));
}

bool parser_match(parser_t *_parser, size_t _count, ...)
{
    token_t *token = parser_peek(_parser);
    va_list param;
    token_e type;

    va_start(param, _count);
    while (_count) {
        type = (token_e)va_arg(param, token_e);
        if (type == token->type) {
            if (type != Eof)
                (_parser->cursor)++;
            return true;
        }
        _count--;
    }
    return false;
}

expression_t *parser_equality(parser_t *_parser)
{
    expression_t *expr = parser_comparison(_parser);
    bin_expr_t *bin_expr = NULL;

    while (parser_match(_parser, 2, EqEq, NotEq)) {
        // memory leak?
        bin_expr = bin_expr_create(expr, parser_previous(_parser), parser_comparison(_parser));
        expr = expression_create(Binary, (void *)bin_expr, bin_expr_destroy_void);
    }
    return expr;
}

expression_t *parser_comparison(parser_t *_parser)
{
    expression_t *expr = parser_term(_parser);
    bin_expr_t *bin_expr = NULL;

    while (parser_match(_parser, 4, Gt, Ls, GtEq, LsEq)) {
        bin_expr = bin_expr_create(expr, parser_previous(_parser), parser_term(_parser));
        expr = expression_create(Binary, (void *)bin_expr, bin_expr_destroy_void);
    }
    return expr;
}

expression_t *parser_term(parser_t *_parser)
{
    expression_t *expr = parser_factor(_parser);
    bin_expr_t *bin_expr = NULL;

    while (parser_match(_parser, 2, Plus, Minus)) {
        bin_expr = bin_expr_create(expr, parser_previous(_parser), parser_factor(_parser));
        expr = expression_create(Binary, (void *)bin_expr, bin_expr_destroy_void);
    }
    return expr;
}

expression_t *parser_factor(parser_t *_parser)
{
    expression_t *expr = parser_unary(_parser);
    bin_expr_t *bin_expr = NULL;

    while (parser_match(_parser, 2, Div, Mul)) {
        bin_expr = bin_expr_create(expr, parser_previous(_parser), parser_unary(_parser));
        expr = expression_create(Binary, (void *)bin_expr, bin_expr_destroy_void);
    }
    return expr;
}

expression_t *parser_unary(parser_t *_parser)
{
    expression_t *expr = NULL;
    unary_expr_t *unary_expr = NULL;

    if (parser_match(_parser, 2, Not, Minus)) {
        unary_expr = unary_expr_create(parser_previous(_parser), parser_unary(_parser));
        expr = expression_create(Unary, (void *)unary_expr, unary_expr_destroy_void);
        return expr;
    }
    return parser_primary(_parser);
}

expression_t *parser_primary(parser_t *_parser)
{
    token_t *token = parser_peek(_parser);
    expression_t *expr = NULL;
    literal_expr_t *literal_expr = NULL;

    if (parser_match(_parser, 3, False, True, Null)) {
        literal_expr = literal_expr_create(token->literal);
    } else if (parser_match(_parser, 2, NumFloat, NumInt, String)) {
        literal_expr = literal_expr_create(token->literal);
    } else if (token->type == OpenPar) {
        expr = parser_run(_parser);
        if (parser_peek(_parser)->type != ClosePar)
            return NULL; // memory leak?
        (_parser->cursor)++;
        return expression_create(Gourping, (void *)expr, groupe_expr_destroy_void);
    }
    return expression_create(Literal, (void *)literal_expr, literal_expr_destroy_void);
}
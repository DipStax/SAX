#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "sax.h"

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

parser_return_t parser_run(parser_t *_parser)
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
    display_token(token);
    while (_count) {
        type = (token_e)va_arg(param, token_e);
        printf("compare to: %d\n", type);
        if (type == token->type) {
            if (type != Eof)
                (_parser->cursor)++;
            return true;
        }
        _count--;
    }
    return false;
}

parser_return_t parser_equality(parser_t *_parser)
{
    printf("equality >\n");
    parser_return_t final = parser_comparison(_parser);
    parser_return_t ret_cmp = { { 0, NULL }, NULL };
    bin_expr_t *bin_expr = NULL;
    token_t *op = NULL;

    if (final.ret.code == 0) {
        while (parser_match(_parser, 2, EqEq, NotEq)) {
            op = parser_previous(_parser);
            ret_cmp = parser_comparison(_parser);
            if (ret_cmp.ret.code) {
                printf("equality < error\n");
                return ret_cmp;
            }
            bin_expr = bin_expr_create(final.value, op, ret_cmp.value);
            final.value = expression_create(Binary, (void *)bin_expr, bin_expr_destroy_void);
        }
    }
    printf("equality <\n");
    return final;
}

parser_return_t parser_comparison(parser_t *_parser)
{
    printf("comparison >\n");
    parser_return_t final = parser_term(_parser);
    parser_return_t ret_term = { { 0, NULL }, NULL };
    bin_expr_t *bin_expr = NULL;
    token_t *op = NULL;

    if (final.ret.code == 0) {
        while (parser_match(_parser, 4, Gt, Ls, GtEq, LsEq)) {
            op = parser_previous(_parser);
            ret_term = parser_term(_parser);
            if (ret_term.ret.code) {
                printf("comparison < error\n");
                return ret_term;
            }
            bin_expr = bin_expr_create(final.value, op, ret_term.value);
            final.value = expression_create(Binary, (void *)bin_expr, bin_expr_destroy_void);
        }
    }
    printf("comparison <\n");
    return final;
}

parser_return_t parser_term(parser_t *_parser)
{
    printf("term >\n");
    parser_return_t final = parser_factor(_parser);
    parser_return_t ret_fac = { { 0, NULL }, NULL };
    bin_expr_t *bin_expr = NULL;
    token_t *op = NULL;

    if (final.ret.code == 0) {
        while (parser_match(_parser, 2, Plus, Minus)) {
            op = parser_previous(_parser);
            ret_fac = parser_factor(_parser);
            if (ret_fac.ret.code) {
                printf("term < error\n");
                return ret_fac;
            }
            bin_expr = bin_expr_create(final.value, op, ret_fac.value);
            final.value = expression_create(Binary, (void *)bin_expr, bin_expr_destroy_void);
        }
    }
    printf("term <\n");
    return final;
}

parser_return_t parser_factor(parser_t *_parser)
{
    printf("factor >\n");
    parser_return_t final = parser_unary(_parser);
    parser_return_t ret_unary = { { 0, NULL }, NULL };
    bin_expr_t *bin_expr = NULL;
    token_t *op = NULL;

    if (final.ret.code == 0) {
        while (parser_match(_parser, 2, Div, Mul)) {
            op = parser_previous(_parser);
            ret_unary = parser_unary(_parser);
            if (ret_unary.ret.code) {
                printf("factor < error\n");
                return ret_unary;
            }
            bin_expr = bin_expr_create(final.value, op, ret_unary.value);
            final.value = expression_create(Binary, (void *)bin_expr, bin_expr_destroy_void);
        }
    }
    printf("factor <\n");
    return final;
}

parser_return_t parser_unary(parser_t *_parser)
{
    printf("unary >\n");
    parser_return_t final = { { 0, NULL }, NULL };
    parser_return_t ret_unary = { { 0, NULL }, NULL };
    unary_expr_t *unary_expr = NULL;
    token_t *op = NULL;

    if (parser_match(_parser, 2, Not, Minus)) {
        op = parser_previous(_parser);
        ret_unary = parser_unary(_parser);
        if (ret_unary.ret.code) {
            printf("unary < error\n");
            return ret_unary;
        }
        unary_expr = unary_expr_create(op, ret_unary.value);
        final.value = expression_create(Unary, (void *)unary_expr, unary_expr_destroy_void);
    } else {
        final = parser_primary(_parser);
    }
    printf("unary <\n");
    return final;
}

parser_return_t parser_primary(parser_t *_parser)
{
    printf("primary >\n");
    token_t *token = parser_peek(_parser);
    parser_return_t final = { { 0, NULL }, NULL };
    literal_expr_t *literal_expr = NULL;

    if (parser_match(_parser, 3, False, True, Null)) {
        literal_expr = literal_expr_create(token->literal);
    } else if (parser_match(_parser, 2, NumFloat, NumInt, String)) {
        literal_expr = literal_expr_create(token->literal);
    } else if (parser_match(_parser, 1, OpenPar)) {
        printf("!! Parser grouping >\n");
        final = parser_run(_parser);
        printf("!! Parser grouping <\n");
        if (final.ret.code)
            return final;
        if (parser_peek(_parser)->type != ClosePar) {
            final.ret.code = 1;
            final.ret.msg = strdup("expected ')' after expression");
            printf("primary < error 1\n");
            return final;
        }
        (_parser->cursor)++;
        final.value = expression_create(Gourping, (void *)final.value, groupe_expr_destroy_void);
        printf("primary <\n");
        return final;
    } else {
        final.ret.code = 1;
        final.ret.msg = strdup("expected expression");
        printf("primary < error 2\n");
        return final;
    }
    final.value = expression_create(Literal, (void *)literal_expr, literal_expr_destroy_void);
    printf("primary <\n");
    return final;
}
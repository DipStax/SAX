#ifndef DEBUG_H__
#define DEBUG_H__

#ifndef MARGIN
    #define MARGIN 4
#endif

#ifdef DEBUG

#include "token.h"
#include "list.h"
#include "expression.h"

void display_token(token_t *_token);
void display_token_list(list_t *_list);

void display_expression(expression_t *_expr);
void display_expression_rec(expression_t *_expr, size_t _depth);

void display_binary_expr(bin_expr_t *_expr, size_t _depth);
void display_groupe_expr(groupe_expr_t *_expr, size_t _depth);
void display_literal_expr(literal_expr_t *_expr);
void display_unary_expr(unary_expr_t *_expr, size_t _depth);

void display_margin(size_t _rep);

#endif

#endif
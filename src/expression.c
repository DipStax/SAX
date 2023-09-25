#include <stdlib.h>

#include "expression.h"

bin_expr_t *bin_expr_create(expression_t *_left, token_t *_op, expression_t *_right)
{
    bin_expr_t *expr = alloc_zero(sizeof(bin_expr_t));

    expr->left = _left;
    expr->operator = _op;
    expr->right = _right;
    return expr;
}

void bin_expr_destroy(bin_expr_t *_expr)
{
    expression_destroy(_expr->left);
    expression_destroy(_expr->right);
    free(_expr);
}

void bin_expr_destroy_void(void *_expr)
{
    bin_expr_destroy((bin_expr_t *)_expr);
}

groupe_expr_t *groupe_expr_create(expression_t *_expr)
{
    groupe_expr_t *expr = alloc_zero(sizeof(groupe_expr_t));

    expr->expr = _expr;
    return expr;
}

void groupe_expr_destroy(groupe_expr_t *_expr)
{
    expression_destroy(_expr->expr);
    free(_expr);
}

void groupe_expr_destroy_void(void *_expr)
{
    groupe_expr_destroy((groupe_expr_t *)_expr);
}

literal_expr_t *literal_expr_create(void *_value)
{
    literal_expr_t *expr = alloc_zero(sizeof(literal_expr_t));

    expr->value = _value;
    return expr;
}

void literal_expr_destroy(literal_expr_t *_expr)
{
    free(_expr);
}

void literal_expr_destroy_void(void *_expr)
{
    literal_expr_destroy((literal_expr_t *)_expr);
}

unary_expr_t *unary_expr_create(token_t *_op, expression_t *_right)
{
    unary_expr_t *expr = alloc_zero(sizeof(unary_expr_t));

    expr->operator = _op;
    expr->right = _right;
    return expr;
}

void unary_expr_destroy(unary_expr_t *_expr)
{
    expression_destroy(_expr->right);
    free(_expr);
}

void unary_expr_destroy_void(void *_expr)
{
    unary_expr_destroy((unary_expr_t *)_expr);
}

expression_t *expression_create(expression_e type, void *_expr, fn_expr_del _del)
{
    expression_t *expr = alloc_zero(sizeof(expression_t));

    expression_set_del(expr, _del);
    expr->expr = _expr;
    return expr;
}

void expression_destroy(expression_t *_expr)
{
    expression_destroy(_expr->expr);
    _expr->__del(_expr->expr);
    free(_expr);
}

void expression_set_del(expression_t *_expr, fn_expr_del _del)
{
    _expr->__del = _del;
}
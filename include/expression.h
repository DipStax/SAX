#ifndef EXPRESSION_H__
#define EXPRESSION_H__

#include "token.h"

typedef struct expression_s expression_t;

typedef enum expression_e {
    Binary,
    Gourping,
    Literal,
    Unary
} expression_e;

typedef struct bin_expr_s
{
    expression_t *left;
    token_t *operator;
    expression_t *right;
} bin_expr_t;

bin_expr_t *bin_expr_create(expression_t *_left, token_t *_operator, expression_t *_right);
void bin_expr_destroy(bin_expr_t *_expr);
void bin_expr_destroy_void(void *_expr);

typedef struct groupe_expr_s
{
    expression_t *expr;
} groupe_expr_t;

groupe_expr_t *groupe_expr_create(expression_t *_expr);
void groupe_expr_destroy(groupe_expr_t *_expr);
void groupe_expr_destroy_void(void *_expr);

typedef struct literal_expr_s
{
    void *value;
} literal_expr_t;

literal_expr_t *literal_expr_create(void *_value);
void literal_expr_destroy(literal_expr_t *_expr);
void literal_expr_destroy_void(void *_expr);

typedef struct unary_expr_s
{
    token_t *operator;
    expression_t *right;
} unary_expr_t;

unary_expr_t *unary_expr_create(token_t *_op, expression_t *_expr);
void unary_expr_destroy(unary_expr_t *_expr);
void unary_expr_destroy_void(void *_expr);

typedef void (*fn_expr_del)(void *);

typedef struct expression_s
{
    expression_e type;
    void *expr;
    fn_expr_del __del;
} expression_t;

expression_t *expression_create(expression_e type, void *expr, fn_expr_del _del);
void expression_destroy(expression_t *_expr);

void expression_set_del(expression_t *_expr, fn_expr_del _del);

#endif
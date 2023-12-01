#ifdef DEBUG

#include "debug.h"

void display_token(token_t *_token)
{
    printf("Token: { %d, \"%s\", %p }\n", _token->type, _token->lexeme, _token->literal);
}

void display_token_list(list_t *_list)
{
    for (list_it_t *it = _list->__list; it != NULL; it = it->next)
        display_token((token_t *)(it->value));
}

void display_expression(expression_t *_expr)
{
    display_expression_rec(_expr, 0);
}

void display_expression_rec(expression_t *_expr, size_t _depth)
{
    display_margin(_depth);
    switch (_expr->type) {
        case Binary:
            display_binary_expr((bin_expr_t *)(_expr->expr), _depth);
            break;
        case Gourping:
            display_groupe_expr((groupe_expr_t *)(_expr->expr), _depth);
            break;
        case Literal:
            display_margin(_depth);
            display_literal_expr((literal_expr_t *)(_expr->expr));
            break;
        case Unary:
            display_unary_expr((unary_expr_t *)(_expr->expr), _depth);
            break;
    }
}

void display_binary_expr(bin_expr_t *_expr, size_t _depth)
{
    display_margin(_depth);
    printf("Binary: {\n");
    display_expression_rec(_expr->left, _depth + 1);
    display_margin(_depth + 1);
    display_token(_expr->operator);
    display_expression_rec(_expr->right, _depth + 1);
    display_margin(_depth);
    printf("}\n");
}

void display_groupe_expr(groupe_expr_t *_expr, size_t _depth)
{
    display_margin(_depth);
    printf("Groupe: {\n");
    display_expression_rec(_expr->expr, _depth + 1);
    display_margin(_depth);
    printf("}\n");
}

void display_literal_expr(literal_expr_t *_expr)
{
    printf("Literal: { %p }\n", _expr->value);
}

void display_unary_expr(unary_expr_t *_expr, size_t _depth)
{
    display_margin(_depth);
    printf("Unary: {\n");
    display_margin(_depth + 1);
    display_token(_expr->operator);
    display_expression_rec(_expr->right, _depth + 1);
    display_margin(_depth);
    printf("}\n");
}


void  display_margin(size_t _rep)
{
    for (size_t it = 0; it < _rep; it++)
        printf("%*c", MARGIN, ' ');
}

#endif
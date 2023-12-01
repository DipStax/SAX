#include <stdlib.h>

#include "core.h"

return_t run(int _ac, char **_av)
{
    list_t *token = NULL;
    expression_t *expr = NULL;
    return_t ret = { 0, NULL };

    ret = run_lexer(_av[1], &token);
    if (ret.code)
        return ret;
    printf("token ok\n");
#ifdef DEBUG
    display_token_list(token);
#endif
    ret = run_parser(token, &expr);
    if (ret.code)
        return ret;
    printf("parser ok %p\n", expr);
#ifdef DEBUG
    display_expression(expr);
#endif
    return ret;
}

return_t run_lexer(char *_path, list_t **_token)
{
    FILE *file = NULL;
    return_t ret = check_open_file(_path, &file);

    if (ret.code)
        return ret;
    printf("file ok\n");
    (*_token) = list_create(0, token_destroy_void);
    printf("token_list ok\n");
    ret = lexer_run(file, (*_token));
    if (ret.code)
        list_destroy(*_token);
    fclose(file);
    return ret;
}

return_t run_parser(list_t *_token, expression_t **_expr)
{
    parser_t *parser = parser_create(_token);
    parser_return_t ret = parser_run(parser);

    if (ret.ret.code)
        parser_destroy(parser);
    (*_expr) = ret.value;
    return ret.ret;
}

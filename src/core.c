#include <stdlib.h>

#include "tokenizer.h"
#include "core.h"

void display_token(token_t *_token)
{
    printf("TOKEN { %d, \"%s\", %p }\n", _token->type, _token->lexeme, _token->literal);
}

void display_token_list(token_list_t *_list)
{
    for (node_token_t *it = _list->list; it != NULL; it = it->next)
        display_token(it->token);
}

return_t run(int _ac, char **_av)
{
    FILE *file = NULL;
    token_list_t *token = token_list_create();
    return_t final = check_open_file(_av[1], &file);

    if (final.code != 0)
        return final;
    final = tokenize(file, token);
    if (final.code != 0)
        return final;
    display_token_list(token);
    token_list_destroy(token);
    fclose(file);
    return final;
}
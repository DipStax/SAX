#include <stdlib.h>

#include "tokenizer.h"
#include "core.h"

return_t run(int _ac, char **_av)
{
    FILE *file = NULL;
    token_list_t *token = token_list_create();
    fprintf(stderr, "--------------------------------------------------\n");
    return_t final = check_open_file(_av[1], &file);

    fprintf(stderr, "--------------------------------------------------\n");
    fprintf(stderr, "[Core::Run]: token_list is null: %d\n", token == NULL);
    fprintf(stderr, "[Core::Run]: file is null: %d\n", file == NULL);
    if (final.code != 0)
        return final;
    fprintf(stderr, "--------------------------------------------------\n");
    final = tokenize(file, token);
    fprintf(stderr, "--------------------------------------------------\n");
    if (final.code != 0)
        return final;
    fprintf(stderr, "--------------------------------------------------\n");
    token_list_destroy(token);
    fclose(file);
    fprintf(stderr, "--------------------------------------------------\n");
    return final;
}
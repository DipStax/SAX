#include <stdlib.h>

#include "core.h"

int main(int _ac, char **_av)
{
    return_t final;

    if (_ac < 2) {
        printf("Need a file to interpret");
        return 1;
    }
    final = run(_ac, _av);
    if (final.code != 0 && final.msg != NULL) {
        printf("Error: %s\n", final.msg);
        free(final.msg);
    }
    return final.code;
}
#ifndef CORE_H__
#define CORE_H__

#include "sax.h"

return_t run(int _ac, char **_av);

return_t run_lexer(char *_path, list_t **_token);
return_t run_parser(list_t *_token, expression_t **_expr);

#endif
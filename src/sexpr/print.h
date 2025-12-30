#ifndef PRINT_H
#define PRINT_H

#include "types.h"

void lval_print(lval *v);
void lval_expr_print(lval *v, char open, char close);
void lval_println(lval *v);

#endif

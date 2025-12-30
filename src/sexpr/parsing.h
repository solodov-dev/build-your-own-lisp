#ifndef PARSING_H
#define PARSING_H

#include "types.h"

lval *lval_num(long x);
lval *lval_err(char *m);
lval *lval_sym(char *s);
lval *lval_sexpr(void);
lval *lval_qexpr(void);
void lval_del(lval *v);

#endif

#ifndef EVAL_H
#define EVAL_H

#include "types.h"

lval *lval_eval_sexpr(lval *v);
lval *lval_eval(lval *v);
lval *builtin_op(lval *a, char *op);
lval *lval_pop(lval *v, int i);
lval *lval_take(lval *v, int i);

#endif // EVAL_H

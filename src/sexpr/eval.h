#ifndef EVAL_H
#define EVAL_H

#include "types.h"

lval *lval_eval_sexpr(lval *v);
lval *lval_eval(lval *v);
lval *builtin(lval *a, char *func);

#endif // EVAL_H

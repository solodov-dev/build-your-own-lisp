#include "../mpc/mpc.h"
#include "types.h"

#ifndef READ_H
#define READ_H

lval *lval_read_num(mpc_ast_t *t);
lval *lval_add(lval *v, lval *x);
lval *lval_read(mpc_ast_t *t);
lval *lval_join(lval *x, lval *y);
lval *lval_pop(lval *v, int i);
lval *lval_take(lval *v, int i);

#endif

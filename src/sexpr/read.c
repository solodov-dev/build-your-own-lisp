#include "read.h"
#include "parsing.h"

// Convert the AST number value to lval Number
lval *lval_read_num(mpc_ast_t *t) {
  errno = 0;
  long x = strtol(t->contents, NULL, 10);
  return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

lval *lval_add(lval *v, lval *x) {
  v->count++;
  v->cell = realloc(v->cell, sizeof(lval *) * v->count);
  v->cell[v->count - 1] = x;
  return v;
}

lval *lval_read(mpc_ast_t *t) {
  // If Symbol or Number return onversion to that type
  if (strstr(t->tag, "number")) {
    return lval_read_num(t);
  }

  if (strstr(t->tag, "symbol")) {
    return lval_sym(t->contents);
  }

  // If root (>) or Sexpr then create empty list
  lval *x = NULL;

  if (strcmp(t->tag, ">") == 0) {
    x = lval_sexpr();
  }

  if (strstr(t->tag, "sexpr")) {
    x = lval_sexpr();
  }

  if (strstr(t->tag, "qexpr")) {
    x = lval_qexpr();
  }

  // Fill this list with any valid expression contained within
  for (int i = 0; i < t->children_num; i++) {
    if (strcmp(t->children[i]->contents, "(") == 0) {
      continue;
    }
    if (strcmp(t->children[i]->contents, ")") == 0) {
      continue;
    }
    if (strcmp(t->children[i]->contents, "{") == 0) {
      continue;
    }
    if (strcmp(t->children[i]->contents, "}") == 0) {
      continue;
    }
    if (strcmp(t->children[i]->tag, "regex") == 0) {
      continue;
    }
    x = lval_add(x, lval_read(t->children[i]));
  }

  return x;
}

lval *lval_join(lval *x, lval *y) {
  // For each cell in 'y' add it to 'x'
  while (y->count) {
    x = lval_add(x, lval_pop(y, 0));
  }

  lval_del(y);
  return x;
}

lval *lval_pop(lval *v, int i) {
  lval *x = v->cell[i];

  memmove(&v->cell[i], &v->cell[i + 1], sizeof(lval *) * (v->count - i - 1));

  v->count--;

  v->cell = realloc(v->cell, sizeof(lval *) * v->count);
  return x;
}

lval *lval_take(lval *v, int i) {
  lval *x = lval_pop(v, i);
  lval_del(v);
  return x;
}

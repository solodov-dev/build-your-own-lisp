#include "types.h"
#include <stdlib.h>
#include <string.h>

// Create a pointer to a new Number lval
lval *lval_num(long x) {
  lval *v = malloc(sizeof(lval));
  v->type = LVAL_NUM;
  v->num = x;
  return v;
}

// Create a pointer to a new Error lval
lval *lval_err(char *m) {
  lval *v = malloc(sizeof(lval));
  v->type = LVAL_ERR;
  v->err = malloc(strlen(m) + 1);
  strcpy(v->err, m);
  return v;
}

// Create a pointer to a new Symbol lval
lval *lval_sym(char *s) {
  lval *v = malloc(sizeof(lval));
  v->type = LVAL_SYM;
  v->sym = malloc(strlen(s) + 1);
  strcpy(v->sym, s);
  return v;
}

// Create a pointer to a new Sexpr lval
lval *lval_sexpr(void) {
  lval *v = malloc(sizeof(lval));
  v->type = LVAL_SEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

// Free memory reserverd for lval
void lval_del(lval *v) {
  switch (v->type) {
    // Do nothing special for number type
  case LVAL_NUM:
    break;
    // For Error and Symbol free string data
  case LVAL_ERR:
    free(v->err);
    break;
  case LVAL_SYM:
    free(v->sym);
    break;
    // For Sexpr delete all elements inside
  case LVAL_SEXPR:
    for (int i = 0; i < v->count; i++) {
      lval_del(v->cell[i]);
    }
    // Also free the memory allocated to contain the pointers
    free(v->cell);
    break;
  }

  // Free the memory allocated for the lval struct itself
  free(v);
}

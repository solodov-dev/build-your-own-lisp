#ifndef TYPES_H
#define TYPES_H

// Main lval struct to contain our lisp expression value
typedef struct lval {
  int type;
  long num;
  // Error and Symbol types have some string data
  char *err;
  char *sym;
  // Count and pointer to a list of lval*
  int count;
  struct lval **cell;
} lval;

// Enum of possible lval types
enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR };

// Enum of possible error types
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM, LERR_NOT_ENOUGH_OPERANDS };

#endif

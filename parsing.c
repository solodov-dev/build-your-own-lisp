#include "mpc.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

// Fake readline definition
char *readline(char *prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char *cpy = malloc(strlen(buffer) + 1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy) - 1] = '\0';
  return cpy;
}

// Fake add_history function
void add_history(char *unused) {}

// Otherwise include the editline headers
#else
#include <editline/readline.h>
#endif

typedef struct {
  int type;
  long num;
  int err;
} lval;

lval eval(mpc_ast_t *t);

// Enum of possible lval types
enum { LVAL_NUM, LVAL_ERR };

// Enum of possible error types
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM, LERR_NOT_ENOUGH_OPERANDS };

// Create a new number type lval
lval lval_num(long x) {
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

lval lval_err(int x) {
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}

void lval_print(lval v) {
  switch (v.type) {
  case LVAL_NUM:
    printf("%li", v.num);
    break;
  case LVAL_ERR:
    if (v.err == LERR_DIV_ZERO) {
      printf("Error: Division By Zero");
    }
    if (v.err == LERR_BAD_OP) {
      printf("Error: Invalid Operator!");
    }
    if (v.err == LERR_BAD_NUM) {
      printf("Error: Invalid Number!");
    }
    if (v.err == LERR_NOT_ENOUGH_OPERANDS) {
      printf("Error: Invalid Number Of Opertands!");
    }
    break;
  }
}

void lval_println(lval v) {
  lval_print(v);
  putchar('\n');
}

int main(int argc, char **argv) {
  // Create Some Parsers
  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Operator = mpc_new("operator");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Lispy = mpc_new("lispy");

  // Define them with the following language
  mpca_lang(MPCA_LANG_DEFAULT, "                                        \
      number : /-?[0-9]+/ ;                                             \
      operator : '+' | '-' | '*' | '/' | '%' | '^' | \"min\" | \"max\"; \
      expr : <number> | '(' <operator> <expr>+ ')' ;                    \
      lispy : /^/ <operator> <expr>+ /$/ ;                              \
    ",
            Number, Operator, Expr, Lispy);

  // Print Version and Exit Information
  puts("Lispy Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");

  while (1) {
    // Output prompt and get input
    char *input = readline("lispy> ");

    // Add input to history
    add_history(input);

    // Attempt to parse the user input
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      lval result = eval(r.output);
      lval_println(result);
      mpc_ast_delete(r.output);
    } else {
      // Print error
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    // Free retrieved input
    free(input);
  }

  mpc_cleanup(4, Number, Operator, Expr, Lispy);

  return 0;
}

lval eval_op(lval x, char *op, lval y) {
  if (x.type == LVAL_ERR) {
    return x;
  }
  if (y.type == LVAL_ERR) {
    return y;
  }

  if (strcmp(op, "+") == 0) {
    return lval_num(x.num + y.num);
  }
  if (strcmp(op, "-") == 0) {
    return lval_num(x.num - y.num);
  }
  if (strcmp(op, "*") == 0) {
    return lval_num(x.num * y.num);
  }
  if (strcmp(op, "/") == 0) {
    return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
  }
  if (strcmp(op, "%") == 0) {
    return lval_num(x.num % y.num);
  }
  if (strcmp(op, "^") == 0) {
    return lval_num(x.num ^ y.num);
  }
  if (strcmp(op, "max") == 0) {
    return x.num > y.num ? x : y;
  }
  if (strcmp(op, "min") == 0) {
    return x.num < y.num ? x : y;
  }
  return lval_err(LERR_BAD_OP);
}

lval eval_unary_op(lval x, char *op) {
  if (strcmp(op, "-") == 0) {
    return lval_num(-x.num);
  }

  return lval_err(LERR_NOT_ENOUGH_OPERANDS);
}

lval eval(mpc_ast_t *t) {
  if (strstr(t->tag, "number")) {
    // Check if there is some error in conversion
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  // The operator is always second child
  char *op = t->children[1]->contents;

  // We store the third child in `x`
  lval x = eval(t->children[2]);

  // Iterate the remaining children and combining
  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  // No second child (unary operator)
  if (i == 3) {
    x = eval_unary_op(x, op);
  }

  return x;
}

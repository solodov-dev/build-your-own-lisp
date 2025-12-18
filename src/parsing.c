#include "mpc.h"
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

long eval(mpc_ast_t *t);

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
      long result = eval(r.output);
      printf("%li\n", result);
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

long eval_op(long x, char *op, long y) {
  if (strcmp(op, "+") == 0) {
    return x + y;
  }
  if (strcmp(op, "-") == 0) {
    return x - y;
  }
  if (strcmp(op, "*") == 0) {
    return x * y;
  }
  if (strcmp(op, "/") == 0) {
    return x / y;
  }
  if (strcmp(op, "%") == 0) {
    return x % y;
  }
  if (strcmp(op, "^") == 0) {
    return x ^ y;
  }
  if (strcmp(op, "max") == 0) {
    return x > y ? x : y;
  }
  if (strcmp(op, "min") == 0) {
    return x < y ? x : y;
  }
  return 0;
}

long eval_unary_op(long x, char *op) {
  if (strcmp(op, "-") == 0) {
    return -x;
  }

  return x;
}

long eval(mpc_ast_t *t) {
  // If tagged as number return it directly
  if (strstr(t->tag, "number")) {
    return atoi(t->contents);
  }

  // The operator is always second child
  char *op = t->children[1]->contents;

  // We store the third child in `x`
  long x = eval(t->children[2]);

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

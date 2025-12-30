#include "src/mpc/mpc.h"
#include "src/sexpr/eval.h"
#include "src/sexpr/parsing.h"
#include "src/sexpr/print.h"
#include "src/sexpr/read.h"
#include "src/sexpr/types.h"
#include <editline/readline.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  // Create Some Parsers
  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Symbol = mpc_new("symbol");
  mpc_parser_t *Sexpr = mpc_new("sexpr");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Lispy = mpc_new("lispy");

  // Define them with the following language
  mpca_lang(MPCA_LANG_DEFAULT, "                                        \
      number : /-?[0-9]+/ ;                                             \
      symbol : '+' | '-' | '*' | '/' | '%' | '^' | \"min\" | \"max\";   \
      sexpr : '(' <expr>* ')' ;                                         \
      expr : <number> | <symbol> | <sexpr> ;                            \
      lispy : /^/ <expr>* /$/ ;                                         \
    ",
            Number, Symbol, Sexpr, Expr, Lispy);

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
      lval *x = lval_eval(lval_read(r.output));
      lval_println(x);
      lval_del(x);
      mpc_ast_delete(r.output);
    } else {
      // Print error
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    // Free retrieved input
    free(input);
  }

  mpc_cleanup(5, Number, Symbol, Expr, Sexpr, Lispy);

  return 0;
}

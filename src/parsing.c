#include "mpc.h"
#include <math.h>

#if defined _WIN32 || defined _WIN64
const char* platform = "Windows";

#include <string.h>
static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

void add_history(char* unused) {}

#elif defined __APPLE__
const char* platform = "Mac";

#include <editline/readline.h>

#else
const char* platform = "Linux";

#include <editline/readline.h>
#include <editline/history.h>

#endif


long eval_op(long x, char* op, long y) {

  if (strcmp(op, "+") == 0) { return x + y; }
  if (strcmp(op, "add") == 0) { return x + y; }

  if (strcmp(op, "-") == 0) { return x - y; }
  if (strcmp(op, "minus") == 0) { return x - y; }

  if (strcmp(op, "*") == 0) { return x * y; }
  if (strcmp(op, "times") == 0) { return x * y; }

  if (strcmp(op, "/") == 0) { return x / y; }
  if (strcmp(op, "divide") == 0) { return x / y; }

  if (strcmp(op, "%") == 0) { return x % y; }
  if (strcmp(op, "mod") == 0) { return x % y; }

  if (strcmp(op, "^") == 0) { return pow(x, y); }

  if (strcmp(op, "min") == 0) { return x < y ? x : y; }
  if (strcmp(op, "max") == 0) { return x > y ? x : y; }

  return 0;

}


long eval(mpc_ast_t* t) {

  if (strstr(t->tag, "number")) {
    return atoi(t->contents);
  }

  char* op = t->children[1]->contents;
  long x = eval(t->children[2]);

  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;

}


void print_welcome() {

  printf("\n\n\
##       ######## ####  ######  ########  \n\
##       ##        ##  ##    ## ##     ## \n\
##       ##        ##  ##       ##     ## \n\
##       ######    ##   ######  ########  \n\
##       ##        ##        ## ##        \n\
##       ##        ##  ##    ## ##        \n\
######## ######## ####  ######  ##        \n\
  \n");
  printf("Welcome to leisp 0.0.1 (%s) <http://ucdok.com/>\n", platform);
  printf("\n\
Copyright (c) 2016 Zongmin Lei <leizongmin@gmail.com> \n\
                                                      \n\
Type :h and hit Enter for context help.               \n\
Press Ctrl+C to Exit.                                 \n\
  \n");

}


int main(int argc, char* argv[]) {

  mpc_parser_t* Number    = mpc_new("number");
  mpc_parser_t* Operator  = mpc_new("operator");
  mpc_parser_t* Expr      = mpc_new("expr");
  mpc_parser_t* Lispy     = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                                                         \
      number    : /-?[0-9]+/ ;                                                                \
      operator  : /[a-zA-Z\\+\\-\\*\\/\\-\\?\\^\:_%][0-9a-zA-Z\\+\\-\\*\\/\\-\\?\\^\:_%]*/ ;  \
      expr      : <number> | '(' <operator> <expr>+ ')' ;                                     \
      lispy     : /^/ <operator> <expr>+ /$/ ;                                                \
    ",
    Number, Operator, Expr, Lispy);

  print_welcome();

  while (1) {

    char* input = readline("leisp> ");
    add_history(input);

    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {

      // mpc_ast_print(r.output);
      long result = eval(r.output);
      printf("%li\n", result);
      mpc_ast_delete(r.output);

    } else {

      mpc_err_print(r.error);
      mpc_err_delete(r.error);

    }

    free(input);

  }

  mpc_cleanup(4, Number, Operator, Expr, Lispy);

  return 0;

}

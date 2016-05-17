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


enum { LVAL_NUM, LVAL_ERR };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

typedef struct {
  int type;
  long num;
  int err;
} lval;

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
    case LVAL_NUM: printf("%li", v.num); break;
    case LVAL_ERR:
      if (v.err == LERR_DIV_ZERO) { printf("Error: Division By Zero!"); }
      if (v.err == LERR_BAD_OP)   { printf("Error: Invalid Symbol!"); }
      if (v.err == LERR_BAD_NUM)  { printf("Error: Invalid Number!"); }
      break;
  }
}

void lval_println(lval v) {
  lval_print(v);
  putchar('\n');
}


lval eval_op(lval x, char* op, lval y) {

  if (x.type == LVAL_ERR) { return x; }
  if (y.type == LVAL_ERR) { return y; }

  if (strcmp(op, "+") == 0 || strcmp(op, "add") == 0) { return lval_num(x.num + y.num); }
  if (strcmp(op, "-") == 0 || strcmp(op, "minus") == 0) { return lval_num(x.num - y.num); }
  if (strcmp(op, "*") == 0 || strcmp(op, "times") == 0) { return lval_num(x.num * y.num); }
  if (strcmp(op, "/") == 0 || strcmp(op, "divide") == 0) {
    return y.num == 0
      ? lval_err(LERR_DIV_ZERO)
      : lval_num(x.num / y.num);
  }
  if (strcmp(op, "%") == 0 || strcmp(op, "mod") == 0) { return lval_num(x.num % y.num); }
  if (strcmp(op, "^") == 0) { return lval_num(pow(x.num, y.num)); }
  if (strcmp(op, "min") == 0) { return x.num < y.num ? x : y; }
  if (strcmp(op, "max") == 0) { return x.num > y.num ? x : y; }

  return lval_err(LERR_BAD_OP);

}


lval eval(mpc_ast_t* t) {

  if (strstr(t->tag, "number")) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  char* op = t->children[1]->contents;
  lval x = eval(t->children[2]);

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


void print_help() {
  printf("\n\
==================================\n\
Help                              \n\
                                  \n\
+ 1 2                             \n\
  => 3                            \n\
                                  \n\
Press Ctrl+C to Exit.             \n\
==================================\n\
  \n");
}


int main(int argc, char* argv[]) {

  mpc_parser_t* Number  = mpc_new("number");
  mpc_parser_t* Symbol  = mpc_new("symbol");
  mpc_parser_t* Sexpr   = mpc_new("sexpr");
  mpc_parser_t* Expr    = mpc_new("expr");
  mpc_parser_t* Leisp   = mpc_new("leisp");

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                                                           \
      number    : /-?[0-9]+/ ;                                                                  \
      symbol    : /[a-zA-Z\\+\\-\\*\\/\\-\\?\\^\\:_%][0-9a-zA-Z\\+\\-\\*\\/\\-\\?\\^\\:_%]*/ ;  \
      sexpr     : '(' <expr>* ')' ;                                                             \
      expr      : <number> | <symbol> | <sexpr> ;                                               \
      leisp     : /^/ <symbol> <expr>+ /$/ ;                                                    \
    ",
    Number, Symbol, Sexpr, Expr, Leisp);

  print_welcome();

  while (1) {

    char* input = readline("leisp> ");
    add_history(input);

    if (strcmp(input, ":h") == 0) {

      print_help();

    } else {

      mpc_result_t r;
      if (mpc_parse("<stdin>", input, Leisp, &r)) {

        lval result = eval(r.output);
        lval_println(result);
        mpc_ast_delete(r.output);

      } else {

        mpc_err_print(r.error);
        mpc_err_delete(r.error);

      }

    }

    free(input);

  }

  mpc_cleanup(5, Number, Symbol, Sexpr, Expr, Leisp);

  return 0;

}

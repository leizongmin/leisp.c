#include "parsing.h"


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


void run_loop(mpc_parser_t* Leisp, char* input) {

  mpc_result_t r;
  if (mpc_parse("<stdin>", input, Leisp, &r)) {

    lval* x = lval_eval(lval_read(r.output));
    lval_println(x);
    lval_del(x);
    mpc_ast_delete(r.output);

  } else {

    mpc_err_print(r.error);
    mpc_err_delete(r.error);

  }

}


int main(int argc, char* argv[]) {

  mpc_parser_t* ANumber = mpc_new("number");
  mpc_parser_t* ASymbol = mpc_new("symbol");
  mpc_parser_t* ASexpr  = mpc_new("sexpr");
  mpc_parser_t* AQexpr  = mpc_new("qexpr");
  mpc_parser_t* AExpr   = mpc_new("expr");
  mpc_parser_t* ALeisp  = mpc_new("leisp");

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                                                           \
      number    : /-?[0-9]+/ ;                                                                  \
      symbol    : /[a-zA-Z\\+\\-\\*\\/\\-\\?\\^\\:_%][0-9a-zA-Z\\+\\-\\*\\/\\-\\?\\^\\:_%]*/ ;  \
      sexpr     : '(' <expr>* ')' ;                                                             \
      qexpr     : '{' <expr>* '}' ;                                                             \
      expr      : <number> | <symbol> | <sexpr> | <qexpr> ;                                     \
      leisp     : /^/ <expr>* /$/ ;                                                             \
    ",
    ANumber, ASymbol, ASexpr, AQexpr, AExpr, ALeisp);

  print_welcome();

  while (1) {

    char* input = readline("leisp> ");
    add_history(input);

    if (strcmp(input, ":h") == 0) {
      print_help();
    } else {
      run_loop(ALeisp, input);
    }

    free(input);

  }

  mpc_cleanup(6, ANumber, ASymbol, ASexpr, AQexpr, AExpr, ALeisp);

  return 0;

}

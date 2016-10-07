#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../sperl_parser.h"

int main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(stderr, "Not script\n");
    exit(1);
  }
  
  /* Source file */
  SPerl_char* file = argv[1];
  
  /* initialize parser */
  SPerl_PARSER* parser = SPerl_PARSER_new();
  parser->cur_file = file;
  
  /* call SPerl_yyparse */
  SPerl_yydebug = 0;
  SPerl_int parse_success = SPerl_yyparse(parser);
  
  /* dump parser information */
  if (parse_success == 0) {
    SPerl_PARSER_dump_parser_info(parser);
  }
  
  SPerl_PARSER_free(parser);
  
  return parse_success;
}
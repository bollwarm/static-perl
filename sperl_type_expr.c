#include <stdlib.h>

#include "sperl_type_expr.h"
#include "sperl_memory_pool.h"
#include "sperl_parser.h"

SPerl_char* const SPerl_TYPE_EXPR_C_CODE_NAMES[] = {
  "word",
  "array",
  "sub",
};

SPerl_TYPE_EXPR* SPerl_TYPE_EXPR_new(SPerl_PARSER* parser) {
  return SPerl_MEMORY_POOL_alloc(parser->memory_pool, sizeof(SPerl_TYPE_EXPR));
}
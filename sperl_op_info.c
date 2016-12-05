#include "sperl_op_info.h"
#include "sperl_parser.h"






SPerl_char* const SPerl_OP_INFO_C_CODE_NAMES[] = {
  "NULL",
  "CONVERT_DOUBLE_TO_FLOAT",
  "CONVERT_DOUBLE_TO_INT",
  "CONVERT_DDOUBLE_TO_LONG",
  "CONVERT_FLOAT_TO_DOUBLE",
  "CONVERT_FLOAT_TO_INT",
  "CONVERT_FLOAT_TO_LONG",
  "CONVERT_INT_TO_BYTE",
  "CONVERT_INT_TO_CHAR",
  "CONVERT_INT_TO_DOUBLE",
  "CONVERT_INT_TO_FLOAT",
  "CONVERT_INT_TO_LONG",
  "CONVERT_INT_TO_SHORT",
  "CONVERT_LONG_TO_DOUBLE",
  "CONVERT_LONG_TO_FLOAT",
  "CONVERT_LONG_TO_INT",
  "POP",
  "POP2",
};


SPerl_OP_INFO* SPerl_OP_INFO_new(SPerl_PARSER* parser) {
  return SPerl_PARSER_alloc_memory_pool(parser, sizeof(SPerl_OP_INFO));
}

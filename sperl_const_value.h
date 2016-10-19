#ifndef SPerl_CONST_VALUE_H
#define SPerl_CONST_VALUE_H

#include "sperl_type.h"

enum SPerl_CONST_VALUE_TYPE {
  SPerl_CONST_VALUE_TYPE_BOOLEAN,
  SPerl_CONST_VALUE_TYPE_CHAR,
  SPerl_CONST_VALUE_TYPE_BYTE,
  SPerl_CONST_VALUE_TYPE_SHORT,
  SPerl_CONST_VALUE_TYPE_INT,
  SPerl_CONST_VALUE_TYPE_LONG,
  SPerl_CONST_VALUE_TYPE_FLOAT,
  SPerl_CONST_VALUE_TYPE_DOUBLE,
  SPerl_CONST_VALUE_TYPE_STRING
};

struct SPerl_const_value {
  SPerl_char type;
  union {
    SPerl_int int_value;
    SPerl_int long_value;
    float float_value;
    SPerl_double double_value;
    SPerl_char* string_value;
  } uv;
  SPerl_int pool_pos;
  SPerl_OP* op;
};

SPerl_CONST_VALUE* SPerl_CONST_VALUE_new(SPerl_PARSER* parser);

#endif
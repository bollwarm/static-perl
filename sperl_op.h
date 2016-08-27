#ifndef SPERL_OP_H
#define SPERL_OP_H

#include "sperl.h"

/* Operation code */
enum SPerl_OP_CODE {
  SPerl_OP_NULL,
  SPerl_OP_CONST_INT,
  SPerl_OP_CONST_FLOAT,
  SPerl_OP_CONST_STRING,
  SPerl_OP_LT,
  SPerl_OP_LE,
  SPerl_OP_GT,
  SPerl_OP_GE,
  SPerl_OP_ADD,
  SPerl_OP_SUBTRACT,
  SPerl_OP_MULTIPLY,
  SPerl_OP_DIVIDE,
  SPerl_OP_AND,
  SPerl_OP_OR,
  SPerl_OP_BIT_AND,
  SPerl_OP_BIT_OR,
  SPerl_OP_MODULO,
  SPerl_OP_BIT_XOR,
  SPerl_OP_NOT,
  SPerl_OP_LEFT_SHIFT,
  SPerl_OP_RIGHT_SHIFT,
  SPerl_OP_INC,
  SPerl_OP_DEC,
  SPerl_OP_BIT_NOT,
  SPerl_OP_CONST_BOOL,
};

/* Token(OP) type */
#define SPerl_BASEOP I32 type;

/* Token(OP) expect ID */
enum SPerl_OP_EXPECT {
  SPerl_OP_EXPECT_NORMAL,
  SPerl_OP_EXPECT_WORD
};

struct SPerl_op;
typedef struct SPerl_op SPerl_OP;
struct SPerl_op {
  SPerl_BASEOP
};

struct SPerl_svop;
typedef struct SPerl_svop SPerl_SVOP;
struct SPerl_svop {
  SPerl_BASEOP
  union {
    char* pv;
    int iv;
    double nv;
  } uv;
};

struct SPerl_unop;
typedef struct SPerl_unop SPerl_UNOP;
struct SPerl_unop {
  SPerl_BASEOP
  SPerl_OP*	op_first;
};

struct SPerl_binop;
typedef struct SPerl_binop SPerl_BINOP;
struct SPerl_binop {
  SPerl_BASEOP
  SPerl_OP *	op_first;
  SPerl_OP *	op_last;
};

struct SPerl_listop;
typedef struct SPerl_listop SPerl_LISTOP;
struct SPerl_listop {
  SPerl_BASEOP
  SPerl_OP*	op_first;
  SPerl_OP*	op_last;
};

#endif

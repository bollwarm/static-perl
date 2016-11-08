#ifndef SPERL_VMCODE_H
#define SPERL_VMCODE_H

#include "sperl_base.h"

enum {
  SPerl_VMCODE_C_CODE_AALOAD,
  SPerl_VMCODE_C_CODE_AASTORE,
  SPerl_VMCODE_C_CODE_ACONST_NULL,
  SPerl_VMCODE_C_CODE_ALOAD,
  SPerl_VMCODE_C_CODE_ALOAD_0,
  SPerl_VMCODE_C_CODE_ALOAD_1,
  SPerl_VMCODE_C_CODE_ALOAD_2,
  SPerl_VMCODE_C_CODE_ALOAD_3,
  SPerl_VMCODE_C_CODE_ANEWARRAY,
  SPerl_VMCODE_C_CODE_ARETURN,
  SPerl_VMCODE_C_CODE_ARRAYLENGTH,
  SPerl_VMCODE_C_CODE_ASTORE,
  SPerl_VMCODE_C_CODE_ASTORE_0,
  SPerl_VMCODE_C_CODE_ASTORE_1,
  SPerl_VMCODE_C_CODE_ASTORE_2,
  SPerl_VMCODE_C_CODE_ASTORE_3,
  SPerl_VMCODE_C_CODE_ATHROW,
  SPerl_VMCODE_C_CODE_BALOAD,
  SPerl_VMCODE_C_CODE_BASTORE,
  SPerl_VMCODE_C_CODE_BIPUSH,
  SPerl_VMCODE_C_CODE_CALOAD,
  SPerl_VMCODE_C_CODE_CASTORE,
  SPerl_VMCODE_C_CODE_CHECKCAST,
  SPerl_VMCODE_C_CODE_D2F,
  SPerl_VMCODE_C_CODE_D2I,
  SPerl_VMCODE_C_CODE_D2L,
  SPerl_VMCODE_C_CODE_DADD,
  SPerl_VMCODE_C_CODE_DALOAD,
  SPerl_VMCODE_C_CODE_DASTORE,
  SPerl_VMCODE_C_CODE_DCMPG,
  SPerl_VMCODE_C_CODE_DCMPL,
  SPerl_VMCODE_C_CODE_DCONST_0,
  SPerl_VMCODE_C_CODE_DCONST_1,
  SPerl_VMCODE_C_CODE_DDIV,
  SPerl_VMCODE_C_CODE_DLOAD,
  SPerl_VMCODE_C_CODE_DLOAD_0,
  SPerl_VMCODE_C_CODE_DLOAD_1,
  SPerl_VMCODE_C_CODE_DLOAD_2,
  SPerl_VMCODE_C_CODE_DLOAD_3,
  SPerl_VMCODE_C_CODE_DMUL,
  SPerl_VMCODE_C_CODE_DNEG,
  SPerl_VMCODE_C_CODE_DREM,
  SPerl_VMCODE_C_CODE_DRETURN,
  SPerl_VMCODE_C_CODE_DSTORE,
  SPerl_VMCODE_C_CODE_DSTORE_0,
  SPerl_VMCODE_C_CODE_DSTORE_1,
  SPerl_VMCODE_C_CODE_DSTORE_2,
  SPerl_VMCODE_C_CODE_DSTORE_3,
  SPerl_VMCODE_C_CODE_DSUB,
  SPerl_VMCODE_C_CODE_DUP,
  SPerl_VMCODE_C_CODE_DUP_X1,
  SPerl_VMCODE_C_CODE_DUP_X2,
  SPerl_VMCODE_C_CODE_DUP2,
  SPerl_VMCODE_C_CODE_DUP2_X1,
  SPerl_VMCODE_C_CODE_DUP2_X2,
  SPerl_VMCODE_C_CODE_F2D,
  SPerl_VMCODE_C_CODE_F2I,
  SPerl_VMCODE_C_CODE_F2L,
  SPerl_VMCODE_C_CODE_FADD,
  SPerl_VMCODE_C_CODE_FALOAD,
  SPerl_VMCODE_C_CODE_FASTORE,
  SPerl_VMCODE_C_CODE_FCMPG,
  SPerl_VMCODE_C_CODE_FCMPL,
  SPerl_VMCODE_C_CODE_FCONST_0,
  SPerl_VMCODE_C_CODE_FCONST_1,
  SPerl_VMCODE_C_CODE_FCONST_2,
  SPerl_VMCODE_C_CODE_FDIV,
  SPerl_VMCODE_C_CODE_FLOAD,
  SPerl_VMCODE_C_CODE_FLOAD_0,
  SPerl_VMCODE_C_CODE_FLOAD_1,
  SPerl_VMCODE_C_CODE_FLOAD_2,
  SPerl_VMCODE_C_CODE_FLOAD_3,
  SPerl_VMCODE_C_CODE_FMUL,
  SPerl_VMCODE_C_CODE_FNEG,
  SPerl_VMCODE_C_CODE_FREM,
  SPerl_VMCODE_C_CODE_FRETURN,
  SPerl_VMCODE_C_CODE_FSTORE,
  SPerl_VMCODE_C_CODE_FSTORE_0,
  SPerl_VMCODE_C_CODE_FSTORE_1,
  SPerl_VMCODE_C_CODE_FSTORE_2,
  SPerl_VMCODE_C_CODE_FSTORE_3,
  SPerl_VMCODE_C_CODE_FSUB,
  SPerl_VMCODE_C_CODE_GETFIELD,
  SPerl_VMCODE_C_CODE_GETSTATIC,
  SPerl_VMCODE_C_CODE_GOTO,
  SPerl_VMCODE_C_CODE_GOTO_W,
  SPerl_VMCODE_C_CODE_I2B,
  SPerl_VMCODE_C_CODE_I2C,
  SPerl_VMCODE_C_CODE_I2D,
  SPerl_VMCODE_C_CODE_I2F,
  SPerl_VMCODE_C_CODE_I2L,
  SPerl_VMCODE_C_CODE_I2S,
  SPerl_VMCODE_C_CODE_IADD,
  SPerl_VMCODE_C_CODE_IALOAD,
  SPerl_VMCODE_C_CODE_IAND,
  SPerl_VMCODE_C_CODE_IASTORE,
  SPerl_VMCODE_C_CODE_ICONST_M1,
  SPerl_VMCODE_C_CODE_ICONST_0,
  SPerl_VMCODE_C_CODE_ICONST_1,
  SPerl_VMCODE_C_CODE_ICONST_2,
  SPerl_VMCODE_C_CODE_ICONST_3,
  SPerl_VMCODE_C_CODE_ICONST_4,
  SPerl_VMCODE_C_CODE_ICONST_5,
  SPerl_VMCODE_C_CODE_IDIV,
  SPerl_VMCODE_C_CODE_IF_ACMPEQ,
  SPerl_VMCODE_C_CODE_IF_ACMPNE,
  SPerl_VMCODE_C_CODE_IF_ICMPEQ,
  SPerl_VMCODE_C_CODE_IF_ICMPNE,
  SPerl_VMCODE_C_CODE_IF_ICMPLT,
  SPerl_VMCODE_C_CODE_IF_ICMPGE,
  SPerl_VMCODE_C_CODE_IF_ICMPGT,
  SPerl_VMCODE_C_CODE_IF_ICMPLE,
  SPerl_VMCODE_C_CODE_IFEQ,
  SPerl_VMCODE_C_CODE_IFNE,
  SPerl_VMCODE_C_CODE_IFLT,
  SPerl_VMCODE_C_CODE_IFGE,
  SPerl_VMCODE_C_CODE_IFGT,
  SPerl_VMCODE_C_CODE_IFLE,
  SPerl_VMCODE_C_CODE_IFNONNULL,
  SPerl_VMCODE_C_CODE_IFNULL,
  SPerl_VMCODE_C_CODE_IINC,
  SPerl_VMCODE_C_CODE_ILOAD,
  SPerl_VMCODE_C_CODE_ILOAD_0,
  SPerl_VMCODE_C_CODE_ILOAD_1,
  SPerl_VMCODE_C_CODE_ILOAD_2,
  SPerl_VMCODE_C_CODE_ILOAD_3,
  SPerl_VMCODE_C_CODE_IMUL,
  SPerl_VMCODE_C_CODE_INEG,
  SPerl_VMCODE_C_CODE_INSTANCEOF,
  SPerl_VMCODE_C_CODE_INVOKEDYNAMIC,
  SPerl_VMCODE_C_CODE_INVOKEINTERFACE,
  SPerl_VMCODE_C_CODE_INVOKESPECIAL,
  SPerl_VMCODE_C_CODE_INVOKESTATIC,
  SPerl_VMCODE_C_CODE_INVOKEVIRTUAL,
  SPerl_VMCODE_C_CODE_IOR,
  SPerl_VMCODE_C_CODE_IREM,
  SPerl_VMCODE_C_CODE_IRETURN,
  SPerl_VMCODE_C_CODE_ISHL,
  SPerl_VMCODE_C_CODE_ISHR,
  SPerl_VMCODE_C_CODE_ISTORE,
  SPerl_VMCODE_C_CODE_ISTORE_0,
  SPerl_VMCODE_C_CODE_ISTORE_1,
  SPerl_VMCODE_C_CODE_ISTORE_2,
  SPerl_VMCODE_C_CODE_ISTORE_3,
  SPerl_VMCODE_C_CODE_ISUB,
  SPerl_VMCODE_C_CODE_IUSHR,
  SPerl_VMCODE_C_CODE_IXOR,
  SPerl_VMCODE_C_CODE_JSR,
  SPerl_VMCODE_C_CODE_JSR_W,
  SPerl_VMCODE_C_CODE_L2D,
  SPerl_VMCODE_C_CODE_L2F,
  SPerl_VMCODE_C_CODE_L2I,
  SPerl_VMCODE_C_CODE_LADD,
  SPerl_VMCODE_C_CODE_LALOAD,
  SPerl_VMCODE_C_CODE_LAND,
  SPerl_VMCODE_C_CODE_LASTORE,
  SPerl_VMCODE_C_CODE_LCMP,
  SPerl_VMCODE_C_CODE_LCONST_0,
  SPerl_VMCODE_C_CODE_LCONST_1,
  SPerl_VMCODE_C_CODE_LDC,
  SPerl_VMCODE_C_CODE_LDC_W,
  SPerl_VMCODE_C_CODE_LDC2_W,
  SPerl_VMCODE_C_CODE_LDIV,
  SPerl_VMCODE_C_CODE_LLOAD,
  SPerl_VMCODE_C_CODE_LLOAD_0,
  SPerl_VMCODE_C_CODE_LLOAD_1,
  SPerl_VMCODE_C_CODE_LLOAD_2,
  SPerl_VMCODE_C_CODE_LLOAD_3,
  SPerl_VMCODE_C_CODE_LMUL,
  SPerl_VMCODE_C_CODE_LNEG,
  SPerl_VMCODE_C_CODE_LOOKUPSWITCH,
  SPerl_VMCODE_C_CODE_LOR,
  SPerl_VMCODE_C_CODE_LREM,
  SPerl_VMCODE_C_CODE_LRETURN,
  SPerl_VMCODE_C_CODE_LSHL,
  SPerl_VMCODE_C_CODE_LSHR,
  SPerl_VMCODE_C_CODE_LSTORE,
  SPerl_VMCODE_C_CODE_LSTORE_0,
  SPerl_VMCODE_C_CODE_LSTORE_1,
  SPerl_VMCODE_C_CODE_LSTORE_2,
  SPerl_VMCODE_C_CODE_LSTORE_3,
  SPerl_VMCODE_C_CODE_LSUB,
  SPerl_VMCODE_C_CODE_LUSHR,
  SPerl_VMCODE_C_CODE_LXOR,
  SPerl_VMCODE_C_CODE_MONITORENTER,
  SPerl_VMCODE_C_CODE_MONITOREXIT,
  SPerl_VMCODE_C_CODE_MULTIANEWARRAY,
  SPerl_VMCODE_C_CODE_NEW,
  SPerl_VMCODE_C_CODE_NEWARRAY,
  SPerl_VMCODE_C_CODE_NOP,
  SPerl_VMCODE_C_CODE_POP,
  SPerl_VMCODE_C_CODE_POP2,
  SPerl_VMCODE_C_CODE_PUTFIELD,
  SPerl_VMCODE_C_CODE_PUTSTATIC,
  SPerl_VMCODE_C_CODE_RET,
  SPerl_VMCODE_C_CODE_RETURN,
  SPerl_VMCODE_C_CODE_SALOAD,
  SPerl_VMCODE_C_CODE_SASTORE,
  SPerl_VMCODE_C_CODE_SIPUSH,
  SPerl_VMCODE_C_CODE_SWAP,
  SPerl_VMCODE_C_CODE_TABLESWITCH,
  SPerl_VMCODE_C_CODE_WIDE
};

#endif
#include <stdlib.h>

#include "sperl_sub.h"
#include "sperl_array.h"
#include "sperl_hash.h"
#include "sperl.h"
#include "sperl_allocator.h"
#include "sperl_bytecodes.h"

SPerl_SUB* SPerl_SUB_new(SPerl* sperl) {
  SPerl_SUB* sub = (SPerl_SUB*)calloc(1, sizeof(SPerl_SUB));
  
  sub->op_my_vars = SPerl_ALLOCATOR_new_array(sperl, 0);
  sub->bytecodes = SPerl_BYTECODES_new();
  
  return sub;
}

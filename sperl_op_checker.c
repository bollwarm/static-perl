#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "sperl_array.h"
#include "sperl_hash.h"
#include "sperl_parser.h"
#include "sperl_allocator.h"
#include "sperl_yacc.h"
#include "sperl_op.h"
#include "sperl_sub.h"
#include "sperl_constant.h"
#include "sperl_field.h"
#include "sperl_my_var.h"
#include "sperl_var.h"
#include "sperl_memory_pool.h"
#include "sperl_use.h"
#include "sperl_word.h"
#include "sperl_enumeration_value.h"
#include "sperl_type.h"
#include "sperl_type_component_word.h"
#include "sperl_type_component_array.h"
#include "sperl_type_component_sub.h"
#include "sperl_type_part.h"
#include "sperl_enumeration.h"
#include "sperl_package.h"
#include "sperl_name.h"
#include "sperl_resolved_type.h"

void SPerl_OP_CHECKER_check(SPerl_PARSER* parser) {
  for (int32_t i = 0; i < parser->op_packages->length; i++) {
    SPerl_OP* op_package = SPerl_ARRAY_fetch(parser->op_packages, i);
    SPerl_PACKAGE* package = op_package->uv.package;
    
    for (int32_t k = 0; k < package->op_subs->length; k++) {
      
      SPerl_OP* op_sub = SPerl_ARRAY_fetch(package->op_subs, k);
      SPerl_SUB* sub = op_sub->uv.sub;
      
      // my var informations
      int32_t next_my_var_pos = 0;
      SPerl_ARRAY* op_my_vars = SPerl_ALLOCATOR_new_array(parser, 0);
      SPerl_HASH* my_var_symtable = SPerl_ALLOCATOR_new_hash(parser, 0);
      
      // my variable stack
      SPerl_ARRAY* op_my_var_stack = SPerl_ALLOCATOR_new_array(parser, 0);
      
      // block base position stack
      SPerl_ARRAY* block_base_stack = SPerl_ALLOCATOR_new_array(parser, 0);
      int32_t block_base = 0;
      _Bool block_start = 0;
      
      // In switch statement
      _Bool in_switch = 0;
      
      // Current case statements
      SPerl_ARRAY* cur_case_ops = NULL;
      
      // Current default statement
      SPerl_OP* cur_default_op = NULL;
      
      // Run OPs
      SPerl_OP* op_base = op_sub;
      SPerl_OP* op_cur = op_base;
      _Bool finish = 0;
      while (op_cur) {
        
        // Check file and line
        // warn("AAAAAAAAAA %s", SPerl_OP_C_CODE_NAMES[op_cur->code]);
        // assert(op_cur->file);
        // assert(op_cur->line);
        
        // [START]Preorder traversal position
        
        switch (op_cur->code) {
          case SPerl_OP_C_CODE_SWITCH: {
            if (in_switch) {
              SPerl_yyerror_format(parser, "duplicate switch is forbidden at %s line %d\n", op_cur->file, op_cur->line);
              parser->fatal_error = 1;
              return;
            }
            else {
              in_switch = 1;
            }
            break;
          }
          // Start scope
          case SPerl_OP_C_CODE_BLOCK: {
            if (block_start) {
              int32_t* block_base_ptr = SPerl_MEMORY_POOL_alloc(parser->memory_pool, sizeof(int32_t));
              *block_base_ptr = op_my_var_stack->length;
              SPerl_ARRAY_push(block_base_stack, block_base_ptr);
              block_base = *block_base_ptr;
            }
            else {
              block_start = 1;
            }
            break;
          }
          case SPerl_OP_C_CODE_ASSIGN: {
            op_cur->first->lvalue = 1;
            break;
          }
        }
        
        // [END]Preorder traversal position
        if (op_cur->first) {
          op_cur = op_cur->first;
        }
        else {
          while (1) {
           // [START]Postorder traversal position
            switch (op_cur->code) {
              case SPerl_OP_C_CODE_DEFAULT: {
                if (cur_default_op) {
                  SPerl_yyerror_format(parser, "multiple default is forbidden at %s line %d\n", op_cur->file, op_cur->line);
                  parser->fatal_error = 1;
                  return;
                }
                else {
                  cur_default_op = op_cur;
                }
                break;
              }
              case SPerl_OP_C_CODE_CASE: {
                
                if (op_cur->first->code != SPerl_OP_C_CODE_CONSTANT) {
                  SPerl_yyerror_format(parser, "case need constant at %s line %d\n", op_cur->file, op_cur->line);
                }
                
                if (!cur_case_ops) {
                  cur_case_ops = SPerl_ALLOCATOR_new_array(parser, 0);
                }
                SPerl_ARRAY_push(cur_case_ops, op_cur);
                
                break;
              }
              case SPerl_OP_C_CODE_SWITCH: {
                
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                
                if (first_resolved_type->id > SPerl_RESOLVED_TYPE_C_ID_INT) {
                  SPerl_yyerror_format(parser, "switch need int at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                in_switch = 0;
                cur_default_op = NULL;
                cur_case_ops = NULL;
                break;
              }
              
              case SPerl_OP_C_CODE_CONDITION: {
                if ((op_cur->first && !op_cur->last) || op_cur->first == op_cur->last) {
                  SPerl_RESOLVED_TYPE* resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                  if (!resolved_type) {
                    SPerl_OP_convert_to_op_constant_false(parser, op_cur->first);
                  }
                }
                
                break;
              }
              case SPerl_OP_C_CODE_AND: {
                if (!op_cur->condition) {
                  SPerl_yyerror_format(parser, "&& operator can use only condition context at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                // Convert && to if statement
                SPerl_OP_convert_and_to_if(parser, op_cur);
                
                break;
              }
              case SPerl_OP_C_CODE_OR: {
                if (!op_cur->condition) {
                  SPerl_yyerror_format(parser, "|| operator can use only condition context at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }

                // Convert || to if statement
                SPerl_OP_convert_or_to_if(parser, op_cur);
                
                break;
              }
              case SPerl_OP_C_CODE_NOT: {
                if (!op_cur->condition) {
                  SPerl_yyerror_format(parser, "! operator can use only condition context at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                // Convert ! to if statement
                SPerl_OP_convert_not_to_if(parser, op_cur);
                
                break;
              }
              case SPerl_OP_C_CODE_EQ: {
                if (!op_cur->condition) {
                  SPerl_yyerror_format(parser, "== operator can use only condition context at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                if (!first_resolved_type && !last_resolved_type) {
                  // Convert to constant true
                  SPerl_OP_convert_to_op_constant_true(parser, op_cur);
                  op_cur->first = NULL;
                  op_cur->last = NULL;
                }
                else {
                  // undef
                  if (!first_resolved_type) {
                    if (SPerl_RESOLVED_TYPE_is_core_type(parser, last_resolved_type)) {
                      SPerl_yyerror_format(parser, "== right value must be reference at %s line %d\n", op_cur->file, op_cur->line);
                      break;
                    }
                  }
                  else if (!last_resolved_type) {
                    if (SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type)) {
                      SPerl_yyerror_format(parser, "== left value must be reference at %s line %d\n", op_cur->file, op_cur->line);
                      break;
                    }
                  }
                  else {
                    if (SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type) && SPerl_RESOLVED_TYPE_is_core_type(parser, last_resolved_type)) {
                      // Insert type converting op
                      SPerl_OP_insert_op_convert_type(parser, op_cur);
                    }
                    else if (!SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type)) {
                      SPerl_yyerror_format(parser, "== right value must be reference at %s line %d\n", op_cur->file, op_cur->line);
                      break;
                    }
                    else if (!SPerl_RESOLVED_TYPE_is_core_type(parser, last_resolved_type)) {
                      SPerl_yyerror_format(parser, "== left value must be reference at %s line %d\n", op_cur->file, op_cur->line);
                      break;
                    }
                  }
                  
                  op_cur->uv.resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "int", strlen("int"));
                }
                
                break;
              }
              case SPerl_OP_C_CODE_NE: {
                if (!op_cur->condition) {
                  SPerl_yyerror_format(parser, "== operator can use only condition context at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }

                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);

                if (!first_resolved_type && !last_resolved_type) {
                  // Convert to constant true
                  SPerl_OP_convert_to_op_constant_false(parser, op_cur);
                  op_cur->first = NULL;
                  op_cur->last = NULL;
                }
                else {
                  // undef
                  if (!first_resolved_type && last_resolved_type) {
                    if (SPerl_RESOLVED_TYPE_is_core_type(parser, last_resolved_type)) {
                      SPerl_yyerror_format(parser, "!= right value must be reference at %s line %d\n", op_cur->file, op_cur->line);
                      break;
                    }                
                  }
                  else if (first_resolved_type && !last_resolved_type) {
                    if (SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type)) {
                      SPerl_yyerror_format(parser, "!= left value must be reference at %s line %d\n", op_cur->file, op_cur->line);
                      break;
                    }
                  }
                  else {
                    if (SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type) && SPerl_RESOLVED_TYPE_is_core_type(parser, last_resolved_type)) {
                      // Insert type converting op
                      SPerl_OP_insert_op_convert_type(parser, op_cur);
                    }
                    else if (!SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type)) {
                      SPerl_yyerror_format(parser, "!= right value must be reference at %s line %d\n", op_cur->file, op_cur->line);
                      break;
                    }
                    else if (!SPerl_RESOLVED_TYPE_is_core_type(parser, last_resolved_type)) {
                      SPerl_yyerror_format(parser, "!= left value must be reference at %s line %d\n", op_cur->file, op_cur->line);
                      break;
                    }
                  }
                  
                  op_cur->uv.resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "int", strlen("int"));
                }
                
                break;
              }
              case SPerl_OP_C_CODE_LT: {
                if (!op_cur->condition) {
                  SPerl_yyerror_format(parser, "< operator can use only condition context at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }

                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                // Can receive only core type
                if (!SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type)) {
                  SPerl_yyerror_format(parser, "< left value must be core type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                if (!SPerl_RESOLVED_TYPE_is_core_type(parser, last_resolved_type)) {
                  SPerl_yyerror_format(parser, "< right value must be core type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                // Insert type converting op
                SPerl_OP_insert_op_convert_type(parser, op_cur);
                
                op_cur->uv.resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "int", strlen("int"));
                
                break;
              }
              case SPerl_OP_C_CODE_LE: {
                if (!op_cur->condition) {
                  SPerl_yyerror_format(parser, "<= operator can use only condition context at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }

                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                // Can receive only core type
                if (!SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type)) {
                  SPerl_yyerror_format(parser, "<= left value must be core type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                if (!SPerl_RESOLVED_TYPE_is_core_type(parser, last_resolved_type)) {
                  SPerl_yyerror_format(parser, "<= right value must be core type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                // Insert type converting op
                SPerl_OP_insert_op_convert_type(parser, op_cur);
                
                op_cur->uv.resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "int", strlen("int"));
                
                break;
              }
              case SPerl_OP_C_CODE_GT: {
                if (!op_cur->condition) {
                  SPerl_yyerror_format(parser, "> operator can use only condition context at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }

                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                // Can receive only core type
                if (!SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type)) {
                  SPerl_yyerror_format(parser, "> left value must be core type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                if (!SPerl_RESOLVED_TYPE_is_core_type(parser, last_resolved_type)) {
                  SPerl_yyerror_format(parser, "> right value must be core type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                // Insert type converting op
                SPerl_OP_insert_op_convert_type(parser, op_cur);
                
                op_cur->uv.resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "int", strlen("int"));
                
                break;
              }
              case SPerl_OP_C_CODE_GE: {
                if (!op_cur->condition) {
                  SPerl_yyerror_format(parser, ">= operator can use only condition context at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }

                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                // Can receive only core type
                if (SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type) && !SPerl_RESOLVED_TYPE_is_core_type(parser, last_resolved_type)) {
                  SPerl_yyerror_format(parser, ">= left value must be core type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                if (!SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type) && SPerl_RESOLVED_TYPE_is_core_type(parser, last_resolved_type)) {
                  SPerl_yyerror_format(parser, ">= right value must be core type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                // Insert type converting op
                SPerl_OP_insert_op_convert_type(parser, op_cur);
                
                op_cur->uv.resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "int", strlen("int"));
                
                break;
              }
              case SPerl_OP_C_CODE_LEFT_SHIFT: {
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                // Can receive only core type
                if (!SPerl_RESOLVED_TYPE_is_integral(parser, first_resolved_type)) {
                  SPerl_yyerror_format(parser, "<< operator left value must be integral at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                if (last_resolved_type->id > SPerl_RESOLVED_TYPE_C_ID_INT) {
                  SPerl_yyerror_format(parser, "<< operator right value must be int at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                SPerl_RESOLVED_TYPE* resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                op_cur->uv.resolved_type = resolved_type;
                
                break;
              }
              case SPerl_OP_C_CODE_RIGHT_SHIFT: {
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                // Can receive only core type
                if (!SPerl_RESOLVED_TYPE_is_integral(parser, first_resolved_type)) {
                  SPerl_yyerror_format(parser, ">> operator left value must be integral at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                if (last_resolved_type->id > SPerl_RESOLVED_TYPE_C_ID_INT) {
                  SPerl_yyerror_format(parser, ">> operator right value must be int at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                SPerl_RESOLVED_TYPE* resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                op_cur->uv.resolved_type = resolved_type;
                
                break;
              }
              case SPerl_OP_C_CODE_RIGHT_SHIFT_UNSIGNED: {
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                // Can receive only core type
                if (!SPerl_RESOLVED_TYPE_is_integral(parser, first_resolved_type)) {
                  SPerl_yyerror_format(parser, ">>> operator left value must be integral at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                if (last_resolved_type->id > SPerl_RESOLVED_TYPE_C_ID_INT) {
                  SPerl_yyerror_format(parser, ">>> operator right value must be int at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                SPerl_RESOLVED_TYPE* resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                op_cur->uv.resolved_type = resolved_type;
                
                break;
              }
              case SPerl_OP_C_CODE_NEW_TYPE: {
                SPerl_OP* op_type = op_cur->first;
                SPerl_RESOLVED_TYPE* resolved_type = op_type->uv.type->resolved_type;
                
                if (SPerl_RESOLVED_TYPE_contain_sub(parser, resolved_type) && !SPerl_RESOLVED_TYPE_is_array(parser, resolved_type)) {
                  SPerl_yyerror_format(parser,
                    "new operator can't receive sub type %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                if (resolved_type->id <= SPerl_RESOLVED_TYPE_C_ID_DOUBLE) {
                  SPerl_yyerror_format(parser,
                    "new operator can't receive core type %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                op_cur->uv.resolved_type = resolved_type;
                
                break;
              }
              case SPerl_OP_C_CODE_BIT_XOR: {
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                // Can receive only core type
                if (first_resolved_type->id >= SPerl_RESOLVED_TYPE_C_ID_FLOAT || last_resolved_type->id >= SPerl_RESOLVED_TYPE_C_ID_FLOAT) {
                  SPerl_yyerror_format(parser,
                    "& operator can receive only boolean, char, char, short, int, long type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                // Insert type converting op
                SPerl_OP_insert_op_convert_type(parser, op_cur);
                
                SPerl_RESOLVED_TYPE* resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                op_cur->uv.resolved_type = resolved_type;
                
                break;
              }
              case SPerl_OP_C_CODE_BIT_OR: {
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                // Can receive only core type
                if (first_resolved_type->id >= SPerl_RESOLVED_TYPE_C_ID_FLOAT || last_resolved_type->id >= SPerl_RESOLVED_TYPE_C_ID_FLOAT) {
                  SPerl_yyerror_format(parser,
                    "& operator can receive only boolean, char, char, short, int, long type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                // Insert type converting op
                SPerl_OP_insert_op_convert_type(parser, op_cur);
                
                SPerl_RESOLVED_TYPE* resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                op_cur->uv.resolved_type = resolved_type;
                
                break;
              }
              case SPerl_OP_C_CODE_BIT_AND: {
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                // Can receive only core type
                if (first_resolved_type->id >= SPerl_RESOLVED_TYPE_C_ID_FLOAT || last_resolved_type->id >= SPerl_RESOLVED_TYPE_C_ID_FLOAT) {
                  SPerl_yyerror_format(parser,
                    "& operator can receive only boolean, char, char, short, int, long type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                // Insert type converting op
                SPerl_OP_insert_op_convert_type(parser, op_cur);
                
                SPerl_RESOLVED_TYPE* resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                op_cur->uv.resolved_type = resolved_type;
                
                break;
              }
              case SPerl_OP_C_CODE_ARRAY_LENGTH: {
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                
                // First value must be array
                _Bool first_resolved_type_is_array = SPerl_RESOLVED_TYPE_is_array(parser, first_resolved_type);
                if (!first_resolved_type_is_array) {
                  SPerl_yyerror_format(parser, "right of @ must be array at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                // Resolved type
                SPerl_RESOLVED_TYPE* resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "int", strlen("int"));
                op_cur->uv.resolved_type = resolved_type;
                
                break;
              }
              case SPerl_OP_C_CODE_ARRAY_ELEM: {
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                // First value must be array
                _Bool first_resolved_type_is_array = SPerl_RESOLVED_TYPE_is_array(parser, first_resolved_type);
                if (!first_resolved_type_is_array) {
                  SPerl_yyerror_format(parser, "left value must be array at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                // Last value must be integer
                if (last_resolved_type->id != SPerl_RESOLVED_TYPE_C_ID_INT) {
                  SPerl_yyerror_format(parser, "array index must be integer at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                SPerl_RESOLVED_TYPE* resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, first_resolved_type->name, strlen(first_resolved_type->name) - 2);
                op_cur->uv.resolved_type = resolved_type;
                
                break;
              }
              case SPerl_OP_C_CODE_ASSIGN: {
                // Type assumption
                if (op_cur->first->first && op_cur->first->first->code == SPerl_OP_C_CODE_DECL_MY_VAR) {
                  SPerl_OP* op_my_var = op_cur->first->first;
                  if (op_my_var->uv.my_var->op_type->code == SPerl_OP_C_CODE_NULL) {
                    SPerl_OP* op_type = SPerl_OP_newOP(parser, SPerl_OP_C_CODE_TYPE);
                    SPerl_TYPE* type = SPerl_TYPE_new(parser);
                    type->resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                    
                    op_type->uv.type = type;
                    
                    op_my_var->uv.my_var->op_type = op_type;
                  }
                }
                
                op_cur->lvalue = 1;
                
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                if (!first_resolved_type) {
                  SPerl_yyerror_format(parser, "Type can't be detected at %s line %d\n", op_cur->first->file, op_cur->first->line);
                  parser->fatal_error = 1;
                  return;
                }
                
                // Convert type
                if (SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type) && SPerl_RESOLVED_TYPE_is_core_type(parser, last_resolved_type)) {
                  SPerl_OP_insert_op_convert_type(parser, op_cur);
                }
                
                first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                if (first_resolved_type->id != last_resolved_type->id) {
                  SPerl_yyerror_format(parser, "Invalid type at %s line %d\n", op_cur->file, op_cur->line);
                  parser->fatal_error = 1;
                  return;
                }
                
                SPerl_RESOLVED_TYPE* resolved_type = first_resolved_type;
                op_cur->uv.resolved_type = resolved_type;
                
                // Insert var op
                if (op_cur->last->code == SPerl_OP_C_CODE_ASSIGN) {
                  SPerl_OP* op_var = SPerl_OP_newOP(parser, SPerl_OP_C_CODE_VAR);
                  op_var->uv.var = op_cur->last->first->uv.var;
                  
                  SPerl_OP* op_last_old = op_cur->last;
                  
                  op_last_old->sibparent = op_var;
                  
                  op_var->first = op_last_old;
                  op_var->sibparent = op_cur;
                  
                  op_cur->last = op_var;
                  
                  op_cur->first->sibparent = op_var;
                }
                
                break;
              }
              case SPerl_OP_C_CODE_RETURN: {
                if (op_cur->first) {
                  SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                  
                  // Can receive only core type
                  if (first_resolved_type->id != sub->op_return_type->uv.type->resolved_type->id) {
                    SPerl_yyerror_format(parser, "Invalid return type at %s line %d\n", op_cur->file, op_cur->line);
                    break;
                  }
                  
                  SPerl_RESOLVED_TYPE* resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                  op_cur->uv.resolved_type = resolved_type;
                }
                
                break;
              }
              case SPerl_OP_C_CODE_NEGATE: {
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                
                // Can receive only core type
                if (!SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type)) {
                  SPerl_yyerror_format(parser, "- operator can receive only core type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                SPerl_RESOLVED_TYPE* resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                op_cur->uv.resolved_type = resolved_type;
                
                break;              
              }
              case SPerl_OP_C_CODE_PLUS: {
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                
                // Can receive only core type
                if (!SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type)) {
                  SPerl_yyerror_format(parser, "+ operator can receive only core type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                
                SPerl_RESOLVED_TYPE* resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                op_cur->uv.resolved_type = resolved_type;
                
                break;
              }
              case SPerl_OP_C_CODE_ADD: {
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                // Can receive only core type
                if (!SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type) || !SPerl_RESOLVED_TYPE_is_core_type(parser, last_resolved_type)) {
                  SPerl_yyerror_format(parser, "+ operator can receive only core type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                // Insert type converting op
                SPerl_OP_insert_op_convert_type(parser, op_cur);
                
                SPerl_RESOLVED_TYPE* resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                op_cur->uv.resolved_type = resolved_type;
                
                break;
              }
              case SPerl_OP_C_CODE_SUBTRACT: {
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                // Can receive only core type
                if (!SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type) || !SPerl_RESOLVED_TYPE_is_core_type(parser, last_resolved_type)) {
                  SPerl_yyerror_format(parser, "- operator can receive only core type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                // Insert type converting op
                SPerl_OP_insert_op_convert_type(parser, op_cur);
                
                SPerl_RESOLVED_TYPE* resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                op_cur->uv.resolved_type = resolved_type;
                
                break;
              }
              case SPerl_OP_C_CODE_MULTIPLY: {
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                // Can receive only core type
                if (!SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type) || !SPerl_RESOLVED_TYPE_is_core_type(parser, last_resolved_type)) {
                  SPerl_yyerror_format(parser, "* operator can receive only core type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                // Insert type converting op
                SPerl_OP_insert_op_convert_type(parser, op_cur);
                
                SPerl_RESOLVED_TYPE* resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                op_cur->uv.resolved_type = resolved_type;
                
                break;
              }
              case SPerl_OP_C_CODE_DIVIDE: {
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                // Can receive only core type
                if (!SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type) || !SPerl_RESOLVED_TYPE_is_core_type(parser, last_resolved_type)) {
                  SPerl_yyerror_format(parser, "/ operator can receive only core type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                // Insert type converting op
                SPerl_OP_insert_op_convert_type(parser, op_cur);
                
                SPerl_RESOLVED_TYPE* resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                op_cur->uv.resolved_type = resolved_type;
                
                break;
              }
              case SPerl_OP_C_CODE_REMAINDER: {
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->last);
                
                // Can receive only core type
                if (!SPerl_RESOLVED_TYPE_is_core_type(parser, first_resolved_type) || !SPerl_RESOLVED_TYPE_is_core_type(parser, last_resolved_type)) {
                  SPerl_yyerror_format(parser, "% operator can receive only core type at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                // Insert type converting op
                SPerl_OP_insert_op_convert_type(parser, op_cur);
                
                SPerl_RESOLVED_TYPE* resolved_type = SPerl_OP_get_resolved_type(parser, op_cur->first);
                op_cur->uv.resolved_type = resolved_type;
                
                break;
              }
              case SPerl_OP_C_CODE_PRE_INC:
              case SPerl_OP_C_CODE_POST_INC:
              case SPerl_OP_C_CODE_PRE_DEC:
              case SPerl_OP_C_CODE_POST_DEC: {
                SPerl_OP* first = op_cur->first;
                if (first->code != SPerl_OP_C_CODE_VAR) {
                  SPerl_yyerror_format(parser, "invalid lvalue in increment at %s line %d\n", op_cur->file, op_cur->line);
                  break;
                }
                SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(parser, first);
                
                // Only int or long
                if (first_resolved_type->id != SPerl_RESOLVED_TYPE_C_ID_INT) {
                  SPerl_yyerror_format(parser, "must be int in increment at %s line %d\n", op_cur->file, op_cur->line);
                }
                op_cur->uv.resolved_type = first_resolved_type;
                break;
              }
              case SPerl_OP_C_CODE_CONSTANT: {

                SPerl_ARRAY_push(package->op_constants, op_cur);
                break;
              }
              // End of scope
              case SPerl_OP_C_CODE_BLOCK: {
                int32_t* block_base_ptr = SPerl_ARRAY_pop(block_base_stack);
                if (block_base_ptr) {
                  int32_t block_base = *block_base_ptr;
                  for (int32_t j = 0; j < op_my_var_stack->length - block_base; j++) {
                    SPerl_ARRAY_pop(op_my_var_stack);
                  }
                }
                int32_t* before_block_base_ptr = SPerl_ARRAY_fetch(block_base_stack, block_base_stack->length - 1);
                if (before_block_base_ptr) {
                  block_base = *before_block_base_ptr;
                }
                else {
                  block_base = 0;
                }
                
                break;
              }
              // Add my var
              case SPerl_OP_C_CODE_VAR: {
                SPerl_VAR* var = op_cur->uv.var;
                
                if (op_cur->first && op_cur->first->code == SPerl_OP_C_CODE_DECL_MY_VAR) {
                  op_cur->lvalue = 1;
                }
                
                // First child is my_var, but my_var don't have type and don't sibling to detect type
                if (op_cur->first && op_cur->first->code == SPerl_OP_C_CODE_DECL_MY_VAR) {
                  SPerl_OP* op_my_var = op_cur->first;
                  if (op_my_var->uv.my_var->op_type->code == SPerl_OP_C_CODE_NULL && !op_cur->moresib) {
                    // Error
                    SPerl_yyerror_format(parser, "\"my %s\" can't detect type at %s line %d\n", var->op_name->uv.word->value, op_cur->file, op_cur->line);
                    parser->fatal_error = 1;
                    return;
                  }
                }
                
                // Serach same name variable
                SPerl_OP* op_my_var = NULL;
                for (int32_t i = op_my_var_stack->length - 1 ; i >= 0; i--) {
                  SPerl_OP* op_my_var_tmp = SPerl_ARRAY_fetch(op_my_var_stack, i);
                  SPerl_MY_VAR* my_var_tmp = op_my_var_tmp->uv.my_var;
                  if (strcmp(var->op_name->uv.word->value, my_var_tmp->op_name->uv.word->value) == 0) {
                    op_my_var = op_my_var_tmp;
                    break;
                  }
                }
                
                if (op_my_var) {
                  // Add my var information to var
                  var->op_my_var = op_my_var;
                }
                else {
                  // Error
                  SPerl_yyerror_format(parser, "\"my %s\" undeclared at %s line %d\n", var->op_name->uv.word->value, op_cur->file, op_cur->line);
                  parser->fatal_error = 1;
                  return;
                }
                break;
              }
              case SPerl_OP_C_CODE_DECL_MY_VAR: {
                SPerl_MY_VAR* my_var = op_cur->uv.my_var;
                
                // Serach same name variable
                int32_t found = 0;
                
                for (int32_t i = op_my_var_stack->length - 1 ; i >= block_base; i--) {
                  SPerl_OP* op_bef_my_var = SPerl_ARRAY_fetch(op_my_var_stack, i);
                  SPerl_MY_VAR* bef_my_var = op_bef_my_var->uv.my_var;
                  if (strcmp(my_var->op_name->uv.word->value, bef_my_var->op_name->uv.word->value) == 0) {
                    found = 1;
                    break;
                  }
                }
                
                if (found) {
                  SPerl_yyerror_format(parser, "redeclaration of my \"%s\" at %s line %d\n", my_var->op_name->uv.word->value, op_cur->file, op_cur->line);
                }
                else {
                  // Add my var information
                  my_var->pos = next_my_var_pos++;
                  SPerl_ARRAY_push(op_my_vars, op_cur);
                  my_var->op_sub = op_sub;
                  
                  SPerl_ARRAY_push(op_my_var_stack, op_cur);
                }
                break;
              }
              case SPerl_OP_C_CODE_CALL_SUB: {
                // Check sub name
                SPerl_NAME* name = op_cur->uv.name;
                if (!name->anon) {
                  SPerl_OP_check_sub_name(parser, op_cur);
                  if (parser->fatal_error) {
                    return;
                  }
                }
                break;
              }
              case SPerl_OP_C_CODE_FIELD: {
                // Check field name
                SPerl_OP_check_field_name(parser, op_cur);
                if (parser->fatal_error) {
                  return;
                }
                break;
              }
              case SPerl_OP_C_CODE_CONVERT: {
                SPerl_RESOLVED_TYPE* resolved_type_src = SPerl_OP_get_resolved_type(parser, op_cur->first);
                
                SPerl_RESOLVED_TYPE* resolved_type_dist = SPerl_OP_get_resolved_type(parser, op_cur->last);;
                
                // Can receive only core type
                if (!SPerl_RESOLVED_TYPE_is_core_type(parser, resolved_type_src) || !SPerl_RESOLVED_TYPE_is_core_type(parser, resolved_type_dist)) {
                  SPerl_yyerror_format(parser, "can't convert type %s to %s at %s line %d\n",
                    resolved_type_src->name, resolved_type_dist->name, op_cur->file, op_cur->line);
                }
                
                // Resolve convert_type op
                if (resolved_type_dist->id <= SPerl_RESOLVED_TYPE_C_ID_INT) {
                  op_cur->uv.resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "int", strlen("int"));
                }
                else if (resolved_type_dist->id <= SPerl_RESOLVED_TYPE_C_ID_LONG) {
                  op_cur->uv.resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "long", strlen("long"));
                }
                else if (resolved_type_dist->id == SPerl_RESOLVED_TYPE_C_ID_FLOAT) {
                  op_cur->uv.resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "float", strlen("float"));
                }
                else if (resolved_type_dist->id == SPerl_RESOLVED_TYPE_C_ID_DOUBLE) {
                  op_cur->uv.resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "double", strlen("double"));
                }
              }
              break;
            }
            
            // [END]Postorder traversal position
            
            if (op_cur == op_base) {
              finish = 1;
              break;
            }
            
            // Next sibling
            if (op_cur->moresib) {
              op_cur = SPerl_OP_sibling(parser, op_cur);
              break;
            }
            // Next is parent
            else {
              op_cur = op_cur->sibparent;
            }
          }
          if (finish) {
            break;
          }
        }
      }
      // Set my var information
      sub->op_my_vars = op_my_vars;
    }
  }
}
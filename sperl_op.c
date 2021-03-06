#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "sperl.h"
#include "sperl_parser.h"
#include "sperl_array.h"
#include "sperl_hash.h"
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
#include "sperl_enumeration_value.h"
#include "sperl_type.h"
#include "sperl_type_component_name.h"
#include "sperl_type_component_array.h"
#include "sperl_type_component_sub.h"
#include "sperl_enumeration.h"
#include "sperl_package.h"
#include "sperl_name_info.h"
#include "sperl_resolved_type.h"
#include "sperl_constant_pool.h"
#include "sperl_bytecode_builder.h"
#include "sperl_op_checker.h"
#include "sperl_switch_info.h"

const char* const SPerl_OP_C_CODE_NAMES[] = {
  "IF",
  "ELSIF",
  "ELSE",
  "FOR",
  "WHILE",
  "NULL",
  "LIST",
  "PUSHMARK",
  "GRAMMAR",
  "NAME",
  "DECL_PACKAGE",
  "DECL_MY_VAR",
  "DECL_FIELD",
  "DECL_SUB",
  "DECL_ENUM",
  "DECL_DESCRIPTER",
  "DECL_ANON_SUB",
  "DECL_ENUMERATION_VALUE",
  "BLOCK",
  "ENUM_BLOCK",
  "CLASS_BLOCK",
  "TYPE",
  "DESCRIPTER_ENUM",
  "CONSTANT",
  "PRE_INC",
  "POST_INC",
  "PRE_DEC",
  "POST_DEC",
  "BIT_NOT",
  "COMPLEMENT",
  "NEGATE",
  "PLUS",
  "EQ",
  "NE",
  "LT",
  "LE",
  "GT",
  "GE",
  "ADD",
  "SUBTRACT",
  "MULTIPLY",
  "DIVIDE",
  "BIT_AND",
  "BIT_OR",
  "REMAINDER",
  "BIT_XOR",
  "LEFT_SHIFT",
  "RIGHT_SHIFT",
  "RIGHT_SHIFT_UNSIGNED",
  "AND",
  "OR",
  "NOT",
  "ARRAY_ELEM",
  "FIELD",
  "ASSIGN",
  "CALL_SUB",
  "USE",
  "RETURN",
  "LAST",
  "NEXT",
  "LOOP",
  "VAR",
  "CONVERT",
  "POP",
  "NEW_ARRAY",
  "UNDEF",
  "NEW",
  "NEW_TYPE",
  "NEW_ARRAY_CONSTANT",
  "ARRAY_LENGTH",
  "CONDITION",
  "DIE",
  "SWITCH",
  "CASE",
  "DEFAULT",
  "SWITCH_CONDITION",
};

void SPerl_OP_insert_op_convert(SPerl* sperl, SPerl_OP* op) {
  
  SPerl_PARSER* parser = sperl->parser;

  SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(sperl, op->first);
  SPerl_RESOLVED_TYPE* last_resolved_type = SPerl_OP_get_resolved_type(sperl, op->last);
  
  int32_t first_type_id = first_resolved_type->id;
  int32_t last_type_id = last_resolved_type->id;
  
  // last int
  if (first_type_id <= SPerl_RESOLVED_TYPE_C_ID_INT) {
    first_type_id = SPerl_RESOLVED_TYPE_C_ID_INT;
  }
  
  // last int
  if (last_type_id <= SPerl_RESOLVED_TYPE_C_ID_INT) {
    last_type_id = SPerl_RESOLVED_TYPE_C_ID_INT;
  }
  
  if (first_type_id != last_type_id) {
    _Bool replace_first = 0;
    
    SPerl_OP* op_convert = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_CONVERT, op->file, op->line);
    SPerl_OP* op_type = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_TYPE, op->file, op->line);
    SPerl_RESOLVED_TYPE* op_type_resolved_type = SPerl_RESOLVED_TYPE_new(sperl);
    SPerl_TYPE* type = SPerl_TYPE_new(sperl);
    type->resolved_type = op_type_resolved_type;
    op_type->uv.type = type;
    
    if (first_type_id == SPerl_RESOLVED_TYPE_C_ID_INT) {
      if (last_type_id == SPerl_RESOLVED_TYPE_C_ID_LONG) {
        op_type->uv.type->resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "long", strlen("long"));
        replace_first = 1;
      }
      else if (last_type_id == SPerl_RESOLVED_TYPE_C_ID_FLOAT) {
        op_type->uv.type->resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "float", strlen("float"));
        replace_first = 1;
      }
      else if (last_type_id == SPerl_RESOLVED_TYPE_C_ID_DOUBLE) {
        op_type->uv.type->resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "double", strlen("double"));
        replace_first = 1;
      }
    }
    else if (first_type_id == SPerl_RESOLVED_TYPE_C_ID_LONG) {
      if (last_type_id == SPerl_RESOLVED_TYPE_C_ID_INT) {
        op_type->uv.type->resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "long", strlen("long"));
      }
      else if (last_type_id == SPerl_RESOLVED_TYPE_C_ID_FLOAT) {
        op_type->uv.type->resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "float", strlen("float"));
        replace_first = 1;
      }
      else if (last_type_id == SPerl_RESOLVED_TYPE_C_ID_DOUBLE) {
        op_type->uv.type->resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "double", strlen("double"));
        replace_first = 1;
      }
    }
    else if (first_type_id == SPerl_RESOLVED_TYPE_C_ID_FLOAT) {
      if (last_type_id == SPerl_RESOLVED_TYPE_C_ID_INT) {
        op_type->uv.type->resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "float", strlen("float"));
      }
      else if (last_type_id == SPerl_RESOLVED_TYPE_C_ID_LONG) {
        op_type->uv.type->resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "long", strlen("long"));
      }
      else if (last_type_id == SPerl_RESOLVED_TYPE_C_ID_DOUBLE) {
        op_type->uv.type->resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "double", strlen("double"));
        replace_first = 1;
      }
    }
    else if (first_type_id == SPerl_RESOLVED_TYPE_C_ID_DOUBLE) {
      if (last_type_id == SPerl_RESOLVED_TYPE_C_ID_INT) {
        op_type->uv.type->resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "double", strlen("double"));
      }
      else if (last_type_id == SPerl_RESOLVED_TYPE_C_ID_LONG) {
        op_type->uv.type->resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "long", strlen("long"));
      }
      else if (last_type_id == SPerl_RESOLVED_TYPE_C_ID_FLOAT) {
        op_type->uv.type->resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "float", strlen("float"));
      }
    }
    
    if (replace_first) {
      op_convert->sibparent = op->first->sibparent;
      op_convert->moresib = op->first->moresib;
      op_convert->first = op->first;
      op->first->sibparent = op_convert;
      op->first->moresib = 0;
      op->first = op_convert;
    }
    else {
      op_convert->sibparent = op->last->sibparent;
      op_convert->moresib = op->last->moresib;
      op_convert->first = op->last;
      op->last->sibparent = op_convert;
      op->last->moresib = 0;
      op->last = op_convert;
      
      op->first->sibparent = op_convert;
    }
    
    SPerl_OP_sibling_splice(sperl, op_convert, op_convert->first, 0, op_type);
  }
}

SPerl_OP* SPerl_OP_build_for_statement(SPerl* sperl, SPerl_OP* op_for, SPerl_OP* op_term_loop_var, SPerl_OP* op_term_condition, SPerl_OP* op_term_next_value, SPerl_OP* op_block) {

  SPerl_OP* op_condition = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_CONDITION, op_term_condition->file, op_term_condition->line);
  SPerl_OP_sibling_splice(sperl, op_condition, NULL, 0, op_term_condition);

  SPerl_OP* op_loop = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_LOOP, op_for->file, op_for->line);
  
  SPerl_OP* op_statements = op_block->first;
  
  op_block->flag |= SPerl_OP_C_FLAG_BLOCK_LOOP;
  op_condition->flag |= SPerl_OP_C_FLAG_CONDITION_LOOP;
  
  // Convert to while loop
  if (op_term_next_value->code != SPerl_OP_C_CODE_NULL) {
    if (op_statements->last) {
      SPerl_OP_sibling_splice(sperl, op_statements, op_statements->last, 0, op_term_next_value);
    }
    else {
      SPerl_OP_sibling_splice(sperl, op_statements, op_statements->first, 0, op_term_next_value);
    }
  }
  
  SPerl_OP_sibling_splice(sperl, op_loop, NULL, 0, op_term_loop_var);
  SPerl_OP_sibling_splice(sperl, op_loop, op_term_loop_var, 0, op_condition);
  SPerl_OP_sibling_splice(sperl, op_loop, op_condition, 0, op_block);
  
  op_term_condition->condition = 1;
  
  return op_loop;
}

SPerl_OP* SPerl_OP_build_switch_statement(SPerl* sperl, SPerl_OP* op_switch, SPerl_OP* op_term, SPerl_OP* op_block) {
  
  SPerl_PARSER* parser = sperl->parser;
  
  SPerl_OP* op_switch_condition = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_SWITCH_CONDITION, op_term->file, op_term->line);
  SPerl_OP_sibling_splice(sperl, op_switch_condition, NULL, 0, op_term);
  
  SPerl_OP_sibling_splice(sperl, op_switch, NULL, 0, op_switch_condition);
  SPerl_OP_sibling_splice(sperl, op_switch, op_switch_condition, 0, op_block);
  
  op_block->flag |= SPerl_OP_C_FLAG_BLOCK_SWITCH;
  
  op_switch->uv.switch_info = SPerl_SWITCH_INFO_new(sperl);
  op_switch->uv.switch_info->op_cases = parser->cur_op_cases;
  
  parser->cur_op_cases = SPerl_ALLOCATOR_new_array(sperl, 0);
  
  return op_switch;
}

SPerl_OP* SPerl_OP_build_case_statement(SPerl* sperl, SPerl_OP* op_case, SPerl_OP* op_term) {
  
  SPerl_PARSER* parser = sperl->parser;
  
  SPerl_OP_sibling_splice(sperl, op_case, NULL, 0, op_term);
  
  op_term->flag = SPerl_OP_C_FLAG_CONSTANT_CASE;
  
  SPerl_ARRAY_push(parser->cur_op_cases, op_case);
  
  return op_case;
}

SPerl_OP* SPerl_OP_build_while_statement(SPerl* sperl, SPerl_OP* op_while, SPerl_OP* op_term, SPerl_OP* op_block) {

  SPerl_OP* op_condition = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_CONDITION, op_term->file, op_term->line);
  SPerl_OP_sibling_splice(sperl, op_condition, NULL, 0, op_term);
  
  op_block->flag |= SPerl_OP_C_FLAG_BLOCK_LOOP;
  op_condition->flag |= SPerl_OP_C_FLAG_CONDITION_LOOP;

  SPerl_OP* op_loop = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_LOOP, op_while->file, op_while->line);
  
  SPerl_OP* op_null = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_NULL, op_while->file, op_while->line);
  
  SPerl_OP_sibling_splice(sperl, op_loop, NULL, 0, op_null);
  SPerl_OP_sibling_splice(sperl, op_loop, op_null, 0, op_condition);
  SPerl_OP_sibling_splice(sperl, op_loop, op_condition, 0, op_block);
  
  op_term->condition = 1;
  
  return op_loop;
}

SPerl_OP* SPerl_OP_build_if_statement(SPerl* sperl, SPerl_OP* op_if, SPerl_OP* op_term, SPerl_OP* op_block, SPerl_OP* op_else_statement) {
  
  SPerl_OP* op_condition = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_CONDITION, op_term->file, op_term->line);
  SPerl_OP_sibling_splice(sperl, op_condition, NULL, 0, op_term);
  
  op_block->flag |= SPerl_OP_C_FLAG_BLOCK_IF;
  op_condition->flag |= SPerl_OP_C_FLAG_CONDITION_IF;
  
  SPerl_OP_sibling_splice(sperl, op_if, NULL, 0, op_condition);
  SPerl_OP_sibling_splice(sperl, op_if, op_condition, 0, op_block);
  SPerl_OP_sibling_splice(sperl, op_if, op_block, 0, op_else_statement);
  
  op_term->condition = 1;
  
  return op_if;
}


SPerl_OP* SPerl_OP_build_array_length(SPerl* sperl, SPerl_OP* op_array_length, SPerl_OP* op_term) {
  SPerl_OP_sibling_splice(sperl, op_array_length, NULL, 0, op_term);
  
  return op_array_length;
}

SPerl_OP* SPerl_OP_build_new_object(SPerl* sperl, SPerl_OP* op_new, SPerl_OP* op_type) {
  
  SPerl_PARSER* parser = sperl->parser;
  
  SPerl_OP* op_new_object = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_NEW_TYPE, op_new->file, op_new->line);
  SPerl_OP_sibling_splice(sperl, op_new_object, NULL, 0, op_type);
  
  SPerl_ARRAY_push(parser->op_types, op_type);
  
  return op_new_object;
}

SPerl_RESOLVED_TYPE* SPerl_OP_get_resolved_type(SPerl* sperl, SPerl_OP* op) {
  
  SPerl_PARSER* parser = sperl->parser;
  
  SPerl_RESOLVED_TYPE*  resolved_type = NULL;
  
  switch (op->code) {
    case SPerl_OP_C_CODE_ARRAY_LENGTH:
      resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "int", strlen("int"));
    case SPerl_OP_C_CODE_ARRAY_ELEM: {
      SPerl_RESOLVED_TYPE* first_resolved_type = SPerl_OP_get_resolved_type(sperl, op->first);
     resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, first_resolved_type->name, strlen(first_resolved_type->name) - 2);
      break;
    }
    case SPerl_OP_C_CODE_RETURN: {
      if (op->first) {
        resolved_type = SPerl_OP_get_resolved_type(sperl, op->first);
      }
      break;
    }
    case SPerl_OP_C_CODE_ADD:
    case SPerl_OP_C_CODE_SUBTRACT:
    case SPerl_OP_C_CODE_MULTIPLY:
    case SPerl_OP_C_CODE_DIVIDE:
    case SPerl_OP_C_CODE_REMAINDER:
    case SPerl_OP_C_CODE_PRE_INC:
    case SPerl_OP_C_CODE_POST_INC:
    case SPerl_OP_C_CODE_PRE_DEC:
    case SPerl_OP_C_CODE_POST_DEC:
    case SPerl_OP_C_CODE_LEFT_SHIFT:
    case SPerl_OP_C_CODE_RIGHT_SHIFT:
    case SPerl_OP_C_CODE_RIGHT_SHIFT_UNSIGNED:
    case SPerl_OP_C_CODE_NEW_TYPE:
    case SPerl_OP_C_CODE_BIT_XOR:
    case SPerl_OP_C_CODE_BIT_OR:
    case SPerl_OP_C_CODE_BIT_AND:
    case SPerl_OP_C_CODE_PLUS:
    case SPerl_OP_C_CODE_NEGATE:
    case SPerl_OP_C_CODE_ASSIGN:
    {
      resolved_type = SPerl_OP_get_resolved_type(sperl, op->first);
      break;
    }
    case SPerl_OP_C_CODE_CONVERT: {
      SPerl_OP* op_type = op->last;
      resolved_type = SPerl_OP_get_resolved_type(sperl, op_type);
      break;
    }
    case SPerl_OP_C_CODE_TYPE: {
      resolved_type = op->uv.type->resolved_type;
      break;
    }
    case SPerl_OP_C_CODE_SWITCH_CONDITION : {
      resolved_type = SPerl_OP_get_resolved_type(sperl, op->first);
      break;
    }
    case SPerl_OP_C_CODE_UNDEF : {
      resolved_type = NULL;
      break;
    }
    case SPerl_OP_C_CODE_DECL_ANON_SUB: {
      SPerl_SUB* sub = op->uv.sub;
      resolved_type = sub->op_return_type->uv.type->resolved_type;
      break;
    }
    case SPerl_OP_C_CODE_CONSTANT: {
      SPerl_CONSTANT* constant = op->uv.constant;
      resolved_type = constant->resolved_type;
      break;
    }
    case SPerl_OP_C_CODE_VAR: {
      SPerl_VAR* var = op->uv.var;
      resolved_type = var->op_my_var->uv.my_var->op_type->uv.type->resolved_type;
      break;
    }
    case SPerl_OP_C_CODE_CALL_SUB: {
      SPerl_NAME_INFO* name_info = op->uv.name_info;
      const char* abs_name = name_info->abs_name;
      SPerl_SUB* sub = SPerl_HASH_search(parser->sub_abs_name_symtable, abs_name, strlen(abs_name));
      resolved_type = sub->op_return_type->uv.type->resolved_type;
      break;
    }
    case SPerl_OP_C_CODE_FIELD: {
      SPerl_NAME_INFO* name_info = op->uv.name_info;
      const char* abs_name = name_info->abs_name;
      SPerl_FIELD* field = SPerl_HASH_search(parser->field_abs_name_symtable, abs_name, strlen(abs_name));
      resolved_type = field->op_type->uv.type->resolved_type;
      break;
    }
  }
  
  return resolved_type;
}

void SPerl_OP_convert_to_op_constant_true(SPerl* sperl, SPerl_OP* op) {
  
  SPerl_PARSER* parser = sperl->parser;
  
  op->code = SPerl_OP_C_CODE_CONSTANT;
  SPerl_CONSTANT* constant_true = SPerl_CONSTANT_new(sperl);
  constant_true->code = SPerl_CONSTANT_C_CODE_INT;
  constant_true->resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "int", strlen("int"));
  constant_true->uv.int_value = 1;
  op->uv.constant = constant_true;
}

void SPerl_OP_convert_to_op_constant_false(SPerl* sperl, SPerl_OP* op) {

  SPerl_PARSER* parser = sperl->parser;

  op->code = SPerl_OP_C_CODE_CONSTANT;
  SPerl_CONSTANT* constant_false = SPerl_CONSTANT_new(sperl);
  constant_false->code = SPerl_CONSTANT_C_CODE_INT;
  constant_false->uv.int_value = 0;
  constant_false->resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "int", strlen("int"));
  op->uv.constant = constant_false;
}

void SPerl_OP_convert_and_to_if(SPerl* sperl, SPerl_OP* op) {
  
  /* before
    AND
      x
      y
  */
  
  /* after 
    IF
      x
      IF
        y
        1
        0
      0
  */
  
  SPerl_OP* op_first = op->first;
  SPerl_OP* op_last = op->last;
  
  // Constant false 1
  SPerl_OP* op_constant_false1 = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_NULL, op->file, op->line);
  SPerl_OP_convert_to_op_constant_false(sperl, op_constant_false1);
  
  // Constant false 2
  SPerl_OP* op_constant_false2 = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_NULL, op->file, op->line);
  SPerl_OP_convert_to_op_constant_false(sperl, op_constant_false2);
  
  // Constant true
  SPerl_OP* op_constant_true = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_NULL, op->file, op->line);
  SPerl_OP_convert_to_op_constant_true(sperl, op_constant_true);
  
  // if
  SPerl_OP* op_if = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_IF, op->file, op->line);
  
  // and to if
  op->code = SPerl_OP_C_CODE_IF;
  op->first = NULL;
  
  op_if = SPerl_OP_build_if_statement(sperl, op_if, op_last, op_constant_true, op_constant_false2);
  
  op_first->sibparent = op_if;
  
  op_if->sibparent = op_if;
  
  SPerl_OP_sibling_splice(sperl, op, op_if, 0, op_constant_false1);
}

void SPerl_OP_convert_or_to_if(SPerl* sperl, SPerl_OP* op) {
  
  // before
  //  OR
  //    x
  //    y
  
  // after 
  //  IF
  //    x
  //    1
  //    IF
  //      y
  //      1
  //      0
  
  SPerl_OP* op_first = op->first;
  SPerl_OP* op_last = op->last;
  
  // Constant true 1
  SPerl_OP* op_constant_true1 = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_NULL, op->file, op->line);
  SPerl_OP_convert_to_op_constant_true(sperl, op_constant_true1);
  
  // Constant true 2
  SPerl_OP* op_constant_true2 = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_NULL, op->file, op->line);
  SPerl_OP_convert_to_op_constant_true(sperl, op_constant_true2);
  
  // Constant false
  SPerl_OP* op_constant_false = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_NULL, op->file, op->line);
  SPerl_OP_convert_to_op_constant_false(sperl, op_constant_false);
  
  // if
  SPerl_OP* op_if = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_IF, op->file, op->line);
  
  // or to if
  op->code = SPerl_OP_C_CODE_IF;
  op->first = NULL;
  
  op_if = SPerl_OP_build_if_statement(sperl, op_if, op_last, op_constant_true2, op_constant_false);
  
  op_first->sibparent = op_constant_true1;
  
  SPerl_OP_sibling_splice(sperl, op, op_constant_true1, 0, op_if);
}

void SPerl_OP_convert_not_to_if(SPerl* sperl, SPerl_OP* op) {
  
  // before
  //  NOT
  //    x
  
  // after 
  //  IF
  //    x
  //    0
  //    1
  
  SPerl_OP* op_first = op->first;
  
  // Constant true 1
  SPerl_OP* op_constant_true = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_NULL, op->file, op->line);
  SPerl_OP_convert_to_op_constant_true(sperl, op_constant_true);
  
  // Constant false
  SPerl_OP* op_constant_false = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_NULL, op->file, op->line);
  SPerl_OP_convert_to_op_constant_false(sperl, op_constant_false);
  
  // If
  op->code = SPerl_OP_C_CODE_IF;
  op->first = NULL;
  op = SPerl_OP_build_if_statement(sperl, op, op_first, op_constant_false, op_constant_true);
}

void SPerl_OP_resolve_types(SPerl* sperl) {
  
  SPerl_PARSER* parser = sperl->parser;
  
  SPerl_ARRAY* op_types = parser->op_types;
  
  for (int32_t i = 0; i < op_types->length; i++) {
    SPerl_OP* op_type = SPerl_ARRAY_fetch(op_types, i);
    _Bool success = SPerl_TYPE_resolve_type(sperl, op_type, 0);
    if (!success) {
      parser->fatal_error = 1;
      return;
    }
  }
}

void SPerl_OP_check(SPerl* sperl) {
  
  SPerl_PARSER* parser = sperl->parser;
  
  // Resolve types
  SPerl_OP_resolve_types(sperl);
  if (parser->fatal_error) {
    return;
  }
  
  // Check types
  SPerl_OP_CHECKER_check(sperl);
  if (parser->fatal_error) {
    return;
  }
}

void SPerl_OP_check_sub_name(SPerl* sperl, SPerl_OP* op_name) {
  
  SPerl_PARSER* parser = sperl->parser;
  
  SPerl_NAME_INFO* name_info = op_name->uv.name_info;
  
  const char* sub_abs_name = NULL;
  if (name_info->op_var) {
    const char* package_name = name_info->op_var->uv.var->op_my_var->uv.my_var->op_type->uv.type->resolved_type->name;
    const char* base_name = name_info->op_name->uv.name;
    sub_abs_name = SPerl_OP_create_abs_name(sperl, package_name, base_name);
  }
  else if (name_info->op_name) {
    sub_abs_name = name_info->op_name->uv.name;
  }
  
  name_info->abs_name = sub_abs_name;
  
  SPerl_SUB* found_sub= SPerl_HASH_search(
    parser->sub_abs_name_symtable,
    sub_abs_name,
    strlen(sub_abs_name)
  );
  if (!found_sub) {
    SPerl_yyerror_format(sperl, "unknown sub \"%s\" at %s line %d\n",
      sub_abs_name, op_name->file, op_name->line);
  }
}

void SPerl_OP_check_field_name(SPerl* sperl, SPerl_OP* op_name_info) {
  
  SPerl_PARSER* parser = sperl->parser;
  
  SPerl_NAME_INFO* name_info = op_name_info->uv.name_info;
  
  const char* field_abs_name;
  const char* package_name = name_info->op_var->uv.var->op_my_var->uv.my_var->op_type->uv.type->resolved_type->name;
  const char* base_name = name_info->op_name->uv.name;
  field_abs_name = SPerl_OP_create_abs_name(sperl, package_name, base_name);
  
  name_info->abs_name = field_abs_name;
  
  SPerl_FIELD* found_field= SPerl_HASH_search(
    parser->field_abs_name_symtable,
    field_abs_name,
    strlen(field_abs_name)
  );
  if (!found_field) {
    SPerl_yyerror_format(sperl, "unknown field \"%s\" at %s line %d\n",
      field_abs_name, op_name_info->file, op_name_info->line);
    parser->fatal_error = 1;
  }
}

SPerl_OP* SPerl_OP_build_field(SPerl* sperl, SPerl_OP* op_var, SPerl_OP* op_field_name) {
  SPerl_OP* op_field = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_FIELD, op_var->file, op_var->line);
  SPerl_OP_sibling_splice(sperl, op_field, NULL, 0, op_var);
  SPerl_OP_sibling_splice(sperl, op_field, op_var, 0, op_field_name);
  
  SPerl_NAME_INFO* name_info = SPerl_NAME_INFO_new(sperl);
  name_info->op_var = op_var;
  name_info->op_name = op_field_name;
  
  op_field->uv.name_info = name_info;
  
  return op_field;
}

SPerl_OP* SPerl_OP_build_array_init(SPerl* sperl, SPerl_OP* op_opt_terms) {
  SPerl_OP* op_new_array = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_NEW_ARRAY_CONSTANT, op_opt_terms->file, op_opt_terms->line);
  SPerl_OP_sibling_splice(sperl, op_new_array, NULL, 0, op_opt_terms);
  
  return op_new_array;
}

SPerl_OP* SPerl_OP_build_convert_type(SPerl* sperl, SPerl_OP* op_type, SPerl_OP* op_term) {
  
  SPerl_OP* op_convert_type = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_CONVERT, op_type->file, op_type->line);
  SPerl_OP_sibling_splice(sperl, op_convert_type, NULL, 0, op_term);
  SPerl_OP_sibling_splice(sperl, op_convert_type, op_term, 0, op_type);
  
  op_convert_type->file = op_type->file;
  op_convert_type->line = op_type->line;
  
  return op_convert_type;
}

SPerl_OP* SPerl_OP_build_grammar(SPerl* sperl, SPerl_OP* op_packages) {
  
  SPerl_PARSER* parser = sperl->parser;
  
  SPerl_OP* op_grammar = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_GRAMMAR, op_packages->file, op_packages->line);
  SPerl_OP_sibling_splice(sperl, op_grammar, NULL, 0, op_packages);
  
  parser->op_grammar = op_grammar;

  // Resovle types, check types, and names.
  SPerl_OP_check(sperl);
  if (parser->fatal_error) {
    return NULL;
  }
  
  // Build constant pool
  SPerl_OP_build_const_pool(sperl);
  
  // Create bytecodes
  SPerl_BYTECODE_BUILDER_build_bytecodes(sperl);
  
  return op_grammar;
}

void SPerl_OP_build_const_pool(SPerl* sperl) {
  
  SPerl_PARSER* parser = sperl->parser;
  
  for (int32_t i = 0; i < parser->op_packages->length; i++) {
    SPerl_OP* op_package = SPerl_ARRAY_fetch(parser->op_packages, i);
    SPerl_PACKAGE* package = op_package->uv.package;
    
    // Set constant informations
    SPerl_ARRAY* op_constants = package->op_constants;
    
    // Create constant pool
    for (int32_t j = 0; j < op_constants->length; j++) {
      SPerl_OP* op_constant = SPerl_ARRAY_fetch(op_constants, j);
      SPerl_CONSTANT* constant = op_constant->uv.constant;
      
      int32_t value1;
      int32_t value2;

      constant->pool_pos = package->constant_pool->length;
      switch (constant->code) {
        case SPerl_CONSTANT_C_CODE_BOOLEAN:
        case SPerl_CONSTANT_C_CODE_BYTE:
        case SPerl_CONSTANT_C_CODE_INT:
          SPerl_CONSTANT_POOL_push(package->constant_pool, constant->uv.int_value);
          break;
        case SPerl_CONSTANT_C_CODE_LONG:
          memcpy(&value1, &constant->uv.long_value, 4);
          memcpy(&value2, ((int32_t*)&constant->uv.long_value) + 1, 4);
          
          SPerl_CONSTANT_POOL_push(package->constant_pool, value1);
          SPerl_CONSTANT_POOL_push(package->constant_pool, value2);
          break;
        case SPerl_CONSTANT_C_CODE_FLOAT:
          memcpy(&value1, &constant->uv.float_value, 4);
          SPerl_CONSTANT_POOL_push(package->constant_pool, value1);
          break;
        case SPerl_CONSTANT_C_CODE_DOUBLE:
          memcpy(&value1, &constant->uv.double_value, 4);
          memcpy(&value2, ((int32_t*)&constant->uv.double_value) + 1, 4);
          
          SPerl_CONSTANT_POOL_push(package->constant_pool, value1);
          SPerl_CONSTANT_POOL_push(package->constant_pool, value2);
          break;
      }
    }
  }
}

const char* SPerl_OP_create_abs_name(SPerl* sperl, const char* package_name, const char* base_name) {
  int32_t length = strlen(package_name) + 2 + strlen(base_name);
  
  char* abs_name = SPerl_ALLOCATOR_new_string(sperl, length);
  
  sprintf(abs_name, "%s::%s", package_name, base_name);
  
  return abs_name;
}

SPerl_OP* SPerl_OP_build_decl_package(SPerl* sperl, SPerl_OP* op_package, SPerl_OP* op_package_name, SPerl_OP* op_block) {
  
  SPerl_PARSER* parser = sperl->parser;

  SPerl_OP_sibling_splice(sperl, op_package, NULL, 0, op_package_name);
  SPerl_OP_sibling_splice(sperl, op_package, op_package_name, 0, op_block);
  
  const char* package_name = op_package_name->uv.name;
  SPerl_HASH* package_symtable = parser->package_symtable;
  
  // Redeclaration package error
  SPerl_TYPE* found_package = SPerl_HASH_search(package_symtable, package_name, strlen(package_name));
  if (found_package) {
    SPerl_yyerror_format(sperl, "redeclaration of package \"%s\" at %s line %d\n", package_name, op_package->file, op_package->line);
  }
  else {
    // Package
    SPerl_PACKAGE* package = SPerl_PACKAGE_new(sperl);
    package->op_name = op_package_name;
    
    // Type(type is same as package name)
    SPerl_TYPE* type = SPerl_TYPE_new(sperl);
    type->code = SPerl_TYPE_C_CODE_NAME;
    SPerl_TYPE_COMPONENT_NAME* type_component_name = SPerl_TYPE_COMPONENT_NAME_new(sperl);
    type_component_name->op_name = op_package_name;
    type->uv.type_component_name = type_component_name;
    
    // Type OP
    SPerl_OP* op_type = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_TYPE, op_package_name->file, op_package_name->line);
    op_type->uv.type = type;
    
    // Add type
    package->op_type = op_type;
    SPerl_ARRAY_push(parser->op_types, op_type);
    
    SPerl_ARRAY* op_fields = SPerl_ALLOCATOR_new_array(sperl, 0);
    SPerl_HASH* field_symtable = SPerl_ALLOCATOR_new_hash(sperl, 0);
    
    // Collect field and use information
    SPerl_OP* op_decls = op_block->first;
    SPerl_OP* op_decl = op_decls->first;
    while ((op_decl = SPerl_OP_sibling(sperl, op_decl))) {
      if (op_decl->code == SPerl_OP_C_CODE_DECL_FIELD) {
        SPerl_OP* op_field = op_decl;
        SPerl_FIELD* field = op_field->uv.field;
        const char* field_name = field->op_name->uv.name;
        SPerl_FIELD* found_field
          = SPerl_HASH_search(field_symtable, field_name, strlen(field_name));
        if (found_field) {
          SPerl_yyerror_format(sperl, "redeclaration of has \"%s\" at %s line %d\n", field_name, op_field->file, op_field->line);
        }
        else {
          // Set ID
          field->id = op_fields->length;
          SPerl_ARRAY_push(op_fields, op_field);
          SPerl_HASH_insert(field_symtable, field_name, strlen(field_name), field);
          
          // Field absolute name
          const char* field_abs_name = SPerl_OP_create_abs_name(sperl, package_name, field_name);
          SPerl_HASH_insert(parser->field_abs_name_symtable, field_abs_name, strlen(field_abs_name), field);
        }
      }
    }
    
    // Method information
    SPerl_HASH* sub_abs_name_symtable = parser->sub_abs_name_symtable;
    int32_t i = parser->cur_op_subs->length - 1;
    while (1) {
      if (i < 0) {
        break;
      }
      SPerl_OP* op_sub = SPerl_ARRAY_fetch(parser->cur_op_subs, i);
      SPerl_SUB* sub = op_sub->uv.sub;
      if (sub->op_package) {
        break;
      }
      
      if (!sub->anon) {
        SPerl_OP* op_sub_name = sub->op_name;
        const char* sub_name = op_sub_name->uv.name;
        const char* sub_abs_name = SPerl_OP_create_abs_name(sperl, package_name, sub_name);
        
        SPerl_SUB* found_sub = NULL;
        found_sub = SPerl_HASH_search(sub_abs_name_symtable, sub_abs_name, strlen(sub_abs_name));
        
        if (found_sub) {
          SPerl_yyerror_format(sperl, "redeclaration of sub \"%s\" at %s line %d\n", sub_abs_name, op_sub->file, op_sub->line);
        }
        // Unknown sub
        else {
          SPerl_HASH_insert(sub_abs_name_symtable, sub_abs_name, strlen(sub_abs_name), sub);
        }
        i--;
      }
      sub->op_package = op_package;
    }
    
    // Set package
    package->op_fields = op_fields;
    package->op_subs = parser->cur_op_subs;
    parser->cur_op_subs = SPerl_ALLOCATOR_new_array(sperl, 0);
    
    // Add package
    op_package->uv.package = package;
    SPerl_ARRAY_push(parser->op_packages, op_package);
    SPerl_HASH_insert(parser->package_symtable, package_name, strlen(package_name), type);
  }
  
  return op_package;
}

SPerl_OP* SPerl_OP_build_decl_use(SPerl* sperl, SPerl_OP* op_use, SPerl_OP* op_package_name) {

  SPerl_PARSER* parser = sperl->parser;

  SPerl_OP_sibling_splice(sperl, op_use, NULL, 0, op_package_name);
  
  SPerl_USE* use = SPerl_USE_new(sperl);
  use->op_package_name = op_package_name;
  op_use->uv.use = use;

  const char* package_name = op_package_name->uv.name;
  
  SPerl_USE* found_use = SPerl_HASH_search(parser->use_package_symtable, package_name, strlen(package_name));
  
  if (!found_use) {
    SPerl_ARRAY_push(parser->op_use_stack, op_use);
    SPerl_HASH_insert(parser->use_package_symtable, package_name, strlen(package_name), op_use);
  }
  
  return op_use;
}

SPerl_OP* SPerl_OP_build_decl_my(SPerl* sperl, SPerl_OP* op_my, SPerl_OP* op_var, SPerl_OP* op_type) {
  
  SPerl_OP_sibling_splice(sperl, op_my, NULL, 0, op_type);
  
  SPerl_OP_sibling_splice(sperl, op_var, NULL, 0, op_my);
  
  // Create my var information
  SPerl_MY_VAR* my_var = SPerl_MY_VAR_new(sperl);
  SPerl_VAR* var = op_var->uv.var;
  
  // Name OP
  SPerl_OP* op_name = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_NAME, op_var->file, op_var->line);
  op_name->uv.name = var->op_name->uv.name;
  my_var->op_name = op_name;
  
  // type
  my_var->op_type = op_type;
  
  // Add my_var information to op
  op_my->uv.my_var = my_var;
  
  return op_var;
}

SPerl_OP* SPerl_OP_build_decl_field(SPerl* sperl, SPerl_OP* op_field, SPerl_OP* op_field_name, SPerl_OP* op_type) {
  
  // Build OP
  SPerl_OP_sibling_splice(sperl, op_field, NULL, 0, op_field_name);
  SPerl_OP_sibling_splice(sperl, op_field, op_field_name, 0, op_type);
  
  // Create field information
  SPerl_FIELD* field = SPerl_FIELD_new(sperl);
  
  // Name
  field->op_name = op_field_name;
  
  // Type
  field->op_type = op_type;
  
  // Set field informaiton
  op_field->uv.field = field;
  
  return op_field;
}

SPerl_OP* SPerl_OP_build_decl_sub(SPerl* sperl, SPerl_OP* op_sub, SPerl_OP* op_sub_name, SPerl_OP* op_subargs, SPerl_OP* op_type, SPerl_OP* op_block) {
  
  SPerl_PARSER* parser = sperl->parser;
  
  // Build OP_SUB
  SPerl_OP_sibling_splice(sperl, op_sub, NULL, 0, op_sub_name);
  SPerl_OP_sibling_splice(sperl, op_sub, op_sub_name, 0, op_subargs);
  SPerl_OP_sibling_splice(sperl, op_sub, op_subargs, 0, op_type);
  SPerl_OP_sibling_splice(sperl, op_sub, op_type, 0, op_block);
  
  // Create sub information
  SPerl_SUB* sub = SPerl_SUB_new(sperl);
  if (op_sub_name->code == SPerl_OP_C_CODE_NULL) {
    sub->anon = 1;
  }
  else {
    sub->op_name = op_sub_name;
  }
  
  // subargs
  int32_t argument_count = 0;
  SPerl_OP* op_subarg = op_subargs->first;
  while ((op_subarg = SPerl_OP_sibling(sperl, op_subarg))) {
    // Increment argument count
    argument_count++;
  }
  sub->argument_count = argument_count;
  
  // return type
  sub->op_return_type = op_type;
  
  // Save block
  sub->op_block = op_block;
  
  // ID
  sub->id = parser->cur_op_subs->length;
  
  // Add sub information
  SPerl_ARRAY_push(parser->cur_op_subs, op_sub);
  
  op_sub->uv.sub = sub;
  
  return op_sub;
}

SPerl_OP* SPerl_OP_build_decl_enum(SPerl* sperl, SPerl_OP* op_enum, SPerl_OP* op_enum_block) {
  
  SPerl_PARSER* parser = sperl->parser;
  
  // Build OP_SUB
  SPerl_OP_sibling_splice(sperl, op_enum, NULL, 0, op_enum_block);
  
  // Starting value
  int64_t start_value = 0;
  SPerl_OP* op_enumeration_values = op_enum_block->first;
  SPerl_OP* op_enumeration_value = op_enumeration_values->first;
  while ((op_enumeration_value = SPerl_OP_sibling(sperl, op_enumeration_value))) {
    SPerl_ENUMERATION_VALUE* enumeration_value = SPerl_ENUMERATION_VALUE_new(sperl);
    enumeration_value->op_name = op_enumeration_value->first;
    if (op_enumeration_value->first != op_enumeration_value->last) {
      enumeration_value->op_constant = op_enumeration_value->last;
    }
    
    SPerl_CONSTANT* constant;
    if (enumeration_value->op_constant) {
      SPerl_OP* op_constant = enumeration_value->op_constant;
      start_value = op_constant->uv.constant->uv.int_value + 1;
      constant = op_constant->uv.constant;
    }
    else {
      constant = SPerl_CONSTANT_new(sperl);
      constant->code = SPerl_CONSTANT_C_CODE_INT;
      constant->uv.int_value = start_value;
      constant->resolved_type = SPerl_HASH_search(parser->resolved_type_symtable, "int", strlen("int"));
      SPerl_OP* op_constant = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_CONSTANT, op_enumeration_value->file, op_enumeration_value->line);
      op_constant->uv.constant = constant;
      
      enumeration_value->op_constant = op_constant;
      start_value++;
    }

    // sub
    SPerl_OP* op_sub = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_DECL_SUB, op_enumeration_value->file, op_enumeration_value->line);
    op_sub->file = op_enumeration_value->file;
    op_sub->line = op_enumeration_value->line;
    
    // sub name
    SPerl_OP* op_sub_name = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_NAME, op_enumeration_value->file, op_enumeration_value->line);
    op_sub_name->uv.name = enumeration_value->op_name->uv.name;
    
    // sub args
    SPerl_OP* op_subargs = SPerl_OP_newOP_LIST(sperl, op_enumeration_value->file, op_enumeration_value->line);
    
    // Type
    SPerl_OP* op_type = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_TYPE, op_enumeration_value->file, op_enumeration_value->line);
    SPerl_TYPE* type = SPerl_TYPE_new(sperl);
    type->resolved_type = constant->resolved_type;
    op_type->uv.type = type;

    // Constant
    SPerl_OP* op_constant = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_CONSTANT, op_enumeration_value->file, op_enumeration_value->line);
    op_constant->uv.constant = constant;
    
    // Return
    SPerl_OP* op_return = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_RETURN, op_enumeration_value->file, op_enumeration_value->line);
    SPerl_OP_sibling_splice(sperl, op_return, NULL, 0, op_constant);
    
    // Statement
    SPerl_OP* op_statements = SPerl_OP_newOP_LIST(sperl, op_enumeration_value->file, op_enumeration_value->line);
    SPerl_OP_sibling_splice(sperl, op_statements, op_statements->first, 0, op_return);
    
    // Block
    SPerl_OP* op_block = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_BLOCK, op_enumeration_value->file, op_enumeration_value->line);
    SPerl_OP_sibling_splice(sperl, op_block, NULL, 0, op_statements);
    
    SPerl_OP_build_decl_sub(sperl, op_sub, op_sub_name, op_subargs, op_type, op_block);
  }
  
  return op_enum;
}

SPerl_OP* SPerl_OP_build_call_sub(SPerl* sperl, SPerl_OP* op_invocant, SPerl_OP* op_sub_name, SPerl_OP* op_terms, _Bool anon) {
  
  // Build OP_SUB
  SPerl_OP* op_call_sub = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_CALL_SUB, op_sub_name->file, op_sub_name->line);
  SPerl_OP_sibling_splice(sperl, op_call_sub, NULL, 0, op_sub_name);
  SPerl_OP_sibling_splice(sperl, op_call_sub, op_sub_name, 0, op_terms);
  
  SPerl_NAME_INFO* name_info = SPerl_NAME_INFO_new(sperl);
  
  if (!anon) {
    const char* sub_name = op_sub_name->uv.name;
    SPerl_OP* op_name = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_NAME, op_invocant->file, op_invocant->line);
    
    if (strstr(sub_name, ":")) {
      op_name->uv.name = sub_name;
      name_info->op_name = op_name;
    }
    else {
      if (op_invocant->code == SPerl_OP_C_CODE_VAR) {
        name_info->op_var = op_invocant;
        name_info->op_name = op_sub_name;
      }
      else {
        const char* sub_abs_name = SPerl_OP_create_abs_name(sperl, "CORE", sub_name);
        op_name->uv.name = sub_abs_name;
        name_info->op_name = op_name;
      }
    }
  }
  
  // Add invocant to arguments
  if (op_invocant->code == SPerl_OP_C_CODE_VAR) {
    SPerl_OP_sibling_splice(sperl, op_terms, op_terms->first, 0, op_invocant);
  }
  
  name_info->anon = anon;
  
  op_call_sub->uv.name_info = name_info;
  
  return op_call_sub;
}

SPerl_OP* SPerl_OP_build_call_op(SPerl* sperl, SPerl_OP* op_call_op, SPerl_OP* op_first, SPerl_OP* op_last) {
  
  // Build OP_SUB
  SPerl_OP_sibling_splice(sperl, op_call_op, NULL, 0, op_first);
  if (op_last) {
    SPerl_OP_sibling_splice(sperl, op_call_op, op_first, 0, op_last);
  }
  
  return op_call_op;
}

SPerl_OP* SPerl_OP_build_logical_op(SPerl* sperl, SPerl_OP* op_logical_op, SPerl_OP* op_first, SPerl_OP* op_last) {
  
  // Build OP_SUB
  SPerl_OP_sibling_splice(sperl, op_logical_op, NULL, 0, op_first);
  if (op_last) {
    SPerl_OP_sibling_splice(sperl, op_logical_op, op_first, 0, op_last);
  }
  
  op_first->condition = 1;
  if (op_last) {
    op_last->condition = 1;
  }
  
  return op_logical_op;
}

SPerl_OP* SPerl_OP_build_type_name(SPerl* sperl, SPerl_OP* op_name) {
  
  SPerl_PARSER* parser = sperl->parser;
  
  // Type component name
  SPerl_TYPE_COMPONENT_NAME* type_component_name = SPerl_TYPE_COMPONENT_NAME_new(sperl);
  type_component_name->op_name = op_name;
  
  // 
  SPerl_TYPE* type = SPerl_TYPE_new(sperl);
  type->code = SPerl_TYPE_C_CODE_NAME;
  type->uv.type_component_name = type_component_name;

  SPerl_OP* op_type_name = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_TYPE, op_name->file, op_name->line);
  SPerl_OP_sibling_splice(sperl, op_type_name, NULL, 0, op_name);
  
  op_type_name->uv.type = type;
  op_type_name->file = op_name->file;
  op_type_name->line = op_name->line;

  SPerl_ARRAY_push(parser->op_types, op_type_name);
  
  return op_type_name;
}

SPerl_OP* SPerl_OP_build_type_array(SPerl* sperl, SPerl_OP* op_type, SPerl_OP* op_term) {
  
  SPerl_PARSER* parser = sperl->parser;
  
  // Type array
  SPerl_TYPE_COMPONENT_ARRAY* type_component_array = SPerl_TYPE_COMPONENT_ARRAY_new(sperl);
  type_component_array->type = op_type->uv.type;
  type_component_array->op_term = op_term;
  
  // Type
  SPerl_TYPE* type = SPerl_TYPE_new(sperl);
  type->code = SPerl_TYPE_C_CODE_ARRAY;
  type->uv.type_component_array = type_component_array;
  
  // Type OP
  SPerl_OP* op_type_array = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_TYPE, op_type->file, op_type->line);
  SPerl_OP_sibling_splice(sperl, op_type_array, NULL, 0, op_type);
  
  op_type_array->uv.type = type;
  op_type_array->file = op_type->file;
  op_type_array->line = op_type->line;
  
  SPerl_ARRAY_push(parser->op_types, op_type_array);
  
  return op_type_array;
}

SPerl_OP* SPerl_OP_build_type_sub(SPerl* sperl, SPerl_OP* op_argument_types, SPerl_OP* op_return_type) {

  SPerl_PARSER* parser = sperl->parser;
  
  SPerl_TYPE* type = SPerl_TYPE_new(sperl);
  type->code = SPerl_TYPE_C_CODE_SUB;
  
  const char* file = NULL;
  int32_t line = -1;
  
  // sub type
  SPerl_TYPE_COMPONENT_SUB* type_component_sub = SPerl_TYPE_COMPONENT_SUB_new(sperl);
  SPerl_ARRAY* argument_types = SPerl_ALLOCATOR_new_array(sperl, 0);
  {
    SPerl_OP* op_argument_type = op_argument_types->first;
    while ((op_argument_type = SPerl_OP_sibling(sperl, op_argument_type))) {
      if (file == NULL) {
        file = op_argument_type->file;
        line = op_argument_type->line;
      }
      SPerl_ARRAY_push(argument_types, op_argument_type->uv.type);
    }
  }
  type_component_sub->return_type = op_return_type->uv.type;
  if (file == NULL) {
    file = op_return_type->file;
    line = op_return_type->line;
  }
  type_component_sub->argument_types = argument_types;
  
  type->uv.type_component_sub = type_component_sub;

  SPerl_OP* op_type_sub = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_TYPE, op_argument_types->file, op_argument_types->line);
  SPerl_OP_sibling_splice(sperl, op_type_sub, NULL, 0, op_argument_types);
  SPerl_OP_sibling_splice(sperl, op_type_sub, op_argument_types, 0, op_return_type);
  
  op_type_sub->uv.type = type;
  op_type_sub->file = file;
  op_type_sub->line = line;

  SPerl_ARRAY_push(parser->op_types, op_type_sub);
  
  return op_type_sub;
}

SPerl_OP* SPerl_OP_append_elem(SPerl* sperl, SPerl_OP *first, SPerl_OP *last, const char* file, uint32_t line) {
  if (!first) {
    return last;
  }
  
  if (!last) {
    return first;
  }
  
  if (first->code == SPerl_OP_C_CODE_LIST) {
    SPerl_OP_sibling_splice(sperl, first, first->last, 0, last);
    return first;
  }
  else {
    SPerl_OP* op_list = SPerl_OP_newOP_LIST(sperl, file, line);
    SPerl_OP_sibling_splice(sperl, op_list, op_list->first, 0, first);
    SPerl_OP_sibling_splice(sperl, op_list, first, 0, last);
    
    return op_list;
  }
}

SPerl_OP* SPerl_OP_newOP_LIST(SPerl* sperl, const char* file, uint32_t line) {
  
  SPerl_OP* op_pushmark = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_PUSHMARK, file, line);
  
  SPerl_OP* op_list = SPerl_OP_newOP(sperl, SPerl_OP_C_CODE_LIST, file, line);
  SPerl_OP_sibling_splice(sperl, op_list, NULL, 0, op_pushmark);
  
  return op_list;
}


SPerl_OP* SPerl_OP_newOP(SPerl* sperl, int32_t code, const char* file, uint32_t line) {

  SPerl_OP *op = SPerl_MEMORY_POOL_alloc(sperl->memory_pool, sizeof(SPerl_OP));
  
  memset(op, 0, sizeof(SPerl_OP));
  
  op->code = code;
  
  assert(!op->file);
  assert(!op->line);
  
  op->file = file;
  op->line = line;
  
  return op;
}

SPerl_OP* SPerl_OP_sibling_splice(SPerl* sperl, SPerl_OP* parent, SPerl_OP* start, int32_t del_count, SPerl_OP* insert) {
  SPerl_OP *first;
  SPerl_OP *rest;
  SPerl_OP *last_del = NULL;
  SPerl_OP *last_ins = NULL;

  if (start) {
    first = SPerl_OP_sibling(sperl, start);
  }
  else if (!parent) {
    goto no_parent;
  }
  else {
    first = parent->first;
  }
  
  if (del_count && first) {
    last_del = first;
    while (--del_count && last_del->moresib)
      last_del = SPerl_OP_sibling(sperl, last_del);
    rest = SPerl_OP_sibling(sperl, last_del);
    SPerl_OP_lastsib_set(sperl, last_del, NULL);
  }
  else {
    rest = first;
  }
  
  if (insert) {
    last_ins = insert;
    while (last_ins->moresib) {
      last_ins = SPerl_OP_sibling(sperl, last_ins);
    }
    SPerl_OP_maybesib_set(sperl, last_ins, rest, NULL);
  }
  else {
    insert = rest;
  }

  if (start) {
    SPerl_OP_maybesib_set(sperl, start, insert, NULL);
  }
  else {
    if (!parent) {
      goto no_parent;
    }
    parent->first = insert;
  }
  
  if (!rest) {
    /* update last etc */
    SPerl_OP *lastop;

    if (!parent) {
      goto no_parent;
    }
    lastop = last_ins ? last_ins : start ? start : NULL;
    parent->last = lastop;

    if (lastop) {
      SPerl_OP_lastsib_set(sperl, lastop, parent);
    }
  }
  return last_del ? first : NULL;

  no_parent:
    fprintf(stderr, "panic: op_sibling_splice(): NULL parent");
    exit(1);
}

SPerl_OP* SPerl_OP_sibling(SPerl* sperl, SPerl_OP* op) {
  (void)sperl;
  
  return op->moresib ? op->sibparent : NULL;
}

void SPerl_OP_moresib_set(SPerl* sperl, SPerl_OP* op, SPerl_OP* sib) {
  (void)sperl;
  
  op->moresib = 1;
  op->sibparent = sib;
}

void SPerl_OP_lastsib_set(SPerl* sperl, SPerl_OP* op, SPerl_OP* parent) {
  (void)sperl;
  op->moresib = 0;
  op->sibparent = parent;
}

void SPerl_OP_maybesib_set(SPerl* sperl, SPerl_OP* op, SPerl_OP* sib, SPerl_OP* parent) {
  (void)sperl;
  op->moresib = sib ? 1 : 0;
  op->sibparent = op->moresib ? sib : parent;
}

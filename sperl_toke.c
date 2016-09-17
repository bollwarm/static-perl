#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sperl_op.h"
#include "sperl_parser.h"
#include "sperly.tab.h"

/* Get token */
int SPerl_yylex(YYSTYPE* SPerl_yylvalp, SPerl_yy_parser* parser) {
  uint8_t* bufptr = parser->bufptr;
  enum SPerl_OP_EXPECT expect = parser->expect;
  parser->expect = SPerl_OP_EXPECT_NORMAL;
  
  while(1) {
    /* Get current character */
    uint8_t c = *bufptr;
    
    /* line end */
    switch(c) {
      /* End */
      case '\0':
        return 0;
      
      /* Skip space character */
      case ' ':
      case '\t':
      case '\n':
      case '\r':
      case (uint8_t)EOF :
        bufptr++;
        parser->bufptr = bufptr;
        continue;
      
      /* Addition */
      case '+':
        bufptr++;
        
        if (*bufptr == '+') {
          bufptr++;
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_INC;
          return INCOP;
        }
        else if (*bufptr == '=') {
          bufptr++;
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_ADD;
          return ASSIGNOP;
        }
        else {
          parser->bufptr = bufptr;
          
          return '+';
        }
      
      /* Subtract */
      case '-':
        bufptr++;
        
        if (*bufptr == '>') {
          bufptr++;
          parser->bufptr = bufptr;
          parser->expect = SPerl_OP_EXPECT_WORD;
          return ARROW;
        }
        else if (*bufptr == '-') {
          bufptr++;
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_DEC;
          return DECOP;
        }
        else if (*bufptr == '=') {
          bufptr++;
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_SUBTRACT;
          return ASSIGNOP;
        }
        else {
          parser->bufptr = bufptr;

          return '-';
        }
      /* Multiply */
      case '*':
        bufptr++;
        if (*bufptr == '=') {
          bufptr++;
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_MULTIPLY;
          return ASSIGNOP;
        }
        else {
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_MULTIPLY;
          return MULOP;
        }
      
      /* Divide */
      case '/':
        bufptr++;
        if (*bufptr == '=') {
          bufptr++;
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_DIVIDE;
          return ASSIGNOP;
        }
        else {
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_DIVIDE;
          return MULOP;
        }

      case '%':
        bufptr++;
        if (*bufptr == '=') {
          bufptr++;
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_MODULO;
          return ASSIGNOP;
        }
        else {
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_MODULO;
          return MULOP;
        }

      case '^':
        bufptr++;
        if (*bufptr == '=') {
          bufptr++;
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_BIT_XOR;
          return ASSIGNOP;
        }
        else {
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_BIT_XOR;
          return MULOP;
        }
            
      case '|':
        bufptr++;
        /* Or */
        if (*bufptr == '|') {
          bufptr++;
          if (*bufptr == '=') {
            bufptr++;
            parser->bufptr = bufptr;
            SPerl_yylvalp->ival = SPerl_OP_OR;
            return ASSIGNOP;
          }
          else {
            parser->bufptr = bufptr;
            SPerl_yylvalp->ival = SPerl_OP_OR;
            return OROP;
          }
        }
        /* Bit or */
        else {
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_BIT_OR;
          return BITOROP;
        }

      case '&':
        bufptr++;
        /* Or */
        if (*bufptr == '&') {
          bufptr++;
          if (*bufptr == '=') {
            bufptr++;
            parser->bufptr = bufptr;
            SPerl_yylvalp->ival = SPerl_OP_AND;
            return ASSIGNOP;
          }
          else {
            parser->bufptr = bufptr;
            SPerl_yylvalp->ival = SPerl_OP_AND;
            return ANDOP;
          }
        }
        /* Bit and */
        else {
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_BIT_AND;
          return BITANDOP;
        }
      
      /* Comment */
      case '#':
        bufptr++;
        while(1) {
          if (*bufptr == '\r' || *bufptr == '\n' || *bufptr == (uint8_t)EOF || *bufptr == '\0') {
            break;
          }
          bufptr++;
        }
        parser->bufptr = bufptr;
        continue;
      
      case '=':
        bufptr++;
        
        /* == */
        if (*bufptr == '=') {
          bufptr++;
          
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_EQ;
          return RELOP;
        }
        /* = */
        else {
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = 0;
          return ASSIGNOP;
        }
        
      case '<':
        bufptr++;
        
        if (*bufptr == '<') {
          bufptr++;
          
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_LEFT_SHIFT;
          return SHIFTOP;
        }
        /* <= */
        else if (*bufptr == '=') {
          bufptr++;
          
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_LE;
          return RELOP;
        }
        /* < */
        else {
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_LT;
          return RELOP;
        }
      
      case '>':
        bufptr++;
        
        if (*bufptr == '>') {
          bufptr++;
          
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_RIGHT_SHIFT;
          return SHIFTOP;
        }
        /* >= */
        else if (*bufptr == '=') {
          bufptr++;
          
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_GE;
          return RELOP;
        }
        /* < */
        else {
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_GT;
          return RELOP;
        }
      case '!':
        bufptr++;
        
        if (*bufptr == '=') {
          bufptr++;
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_NE;
          return RELOP;
        }
        else {
          parser->bufptr = bufptr;
          SPerl_yylvalp->ival = SPerl_OP_NOT;
          return NOTOP;
        }
        
      case '~':
        bufptr++;
        
        parser->bufptr = bufptr;
        SPerl_yylvalp->ival = SPerl_OP_COMPLEMENT;
        return '~';
      
      case '\'': {
        bufptr++;
        
        /* Save current position */
        uint8_t* cur_token_ptr = bufptr;
        
        uint8_t ch;
        if (*bufptr == '\'') {
          ch = '\0';
          bufptr++;
        }
        else {
          ch = *bufptr;
          bufptr++;
          if (*bufptr != '\'') {
            fprintf(stderr, "syntax error: string don't finish\n");
            exit(1);
          }
          bufptr++;
        }
        
        SPerl_OP* op = SPerl_newOP_flag(SPerl_OP_CONST, NULL, NULL, 0, SPerl_OPp_CONST_CHAR);
        op->uv.char_value = ch;
        
        parser->bufptr = bufptr;
        SPerl_yylvalp->opval = (SPerl_OP*)op;
        return CONST;
      }
      case '"': {
        bufptr++;
        
        /* Save current position */
        uint8_t* cur_token_ptr = bufptr;
        
        uint8_t* str;
        if (*(bufptr + 1) == '"') {
          str = malloc(1);
          str[0] = '\0';
          bufptr++;
          bufptr++;
        }
        else {
          while(*bufptr != '"' && *bufptr != '\0') {
            bufptr++;
          }
          if (*bufptr == '\0') {
            fprintf(stderr, "syntax error: string don't finish\n");
            exit(1);
          }
          
          size_t str_len = bufptr - cur_token_ptr;
          str = malloc(str_len + 1);
          memcpy(str, cur_token_ptr, str_len);
          str[str_len] = '\0';

          bufptr++;
        }
        
        SPerl_OP* op = SPerl_newOP_flag(SPerl_OP_CONST, NULL, NULL, 0, SPerl_OPp_CONST_STRING);
        op->uv.string_value = str;
        
        parser->bufptr = bufptr;
        SPerl_yylvalp->opval = (SPerl_OP*)op;
        return CONST;
      }
      default:
        /* Variable */
        if (c == '$') {
          /* Save current position */
          uint8_t* cur_token_ptr = bufptr;
          
          bufptr++;

          /* Next is graph */
          while(isalnum(*bufptr) || (*bufptr) == '_' || (*bufptr) == ':') {
            bufptr++;
          }

          size_t str_len = bufptr - cur_token_ptr;
          uint8_t* var = malloc(str_len + 1);
          memcpy(var, cur_token_ptr, str_len);
          var[str_len] = '\0';
          
          SPerl_OP* op = SPerl_newOP(SPerl_OP_VAR, NULL, NULL);
          op->uv.string_value = var;
          
          parser->bufptr = bufptr;
          SPerl_yylvalp->opval = (SPerl_OP*)op;
          return VAR;
        }
        /* Number literal */
        else if (isdigit(c)) {
          uint8_t* cur_token_ptr = bufptr;
          
          int32_t point_count = 0;
          
          bufptr++;
          
          // Scan number
          while(isdigit(*bufptr) || *bufptr == '.') {
            if (*bufptr == '.') {
              point_count++;
            }
            bufptr++;
          }
          
          if (point_count >= 2) {
            fprintf(stderr, "Invalid number literal\n");
            exit(1);
          }
          
          // Number literal
          size_t str_len = bufptr - cur_token_ptr;
          uint8_t* num_str = malloc(str_len + 1);
          memcpy(num_str, cur_token_ptr, str_len);
          num_str[str_len] = '\0';
          
          // Convert to double
          if (point_count) {
            char* ends;
            double num = strtod(num_str, &ends);
            SPerl_OP* op = SPerl_newOP_flag(SPerl_OP_CONST, NULL, NULL, 0, SPerl_OPp_CONST_DOUBLE);
            op->uv.double_value = num;
            
            free(num_str);
            
            parser->bufptr = bufptr;
            SPerl_yylvalp->opval = (SPerl_OP*)op;
            
            return CONST;
          }
          // Convert to integer
          else {
            int32_t num = atoi(num_str);
            SPerl_OP* op = SPerl_newOP_flag(SPerl_OP_CONST, NULL, NULL, 0, SPerl_OPp_CONST_INT);
            op->uv.int_value = num;

            free(num_str);
            
            parser->bufptr = bufptr;
            SPerl_yylvalp->opval = (SPerl_OP*)op;
            
            return CONST;
          }
        }
        /* Keyword or word */
        else if (isalpha(c) || c == '_') {
          /* Save current position */
          uint8_t* cur_token_ptr = bufptr;
          
          bufptr++;
          
          while(isalnum(*bufptr) || (*bufptr) == '_' || (*bufptr) == ':') {
            bufptr++;
          }
          
          size_t str_len = bufptr - cur_token_ptr;
          uint8_t* keyword = malloc(str_len + 1);
          memcpy(keyword, cur_token_ptr, str_len);
          keyword[str_len] = '\0';
          
          if (expect != SPerl_OP_EXPECT_WORD) {
            if (memcmp(keyword, "my", str_len) == 0) {
              parser->bufptr = bufptr;
              return MY;
            }
            else if (memcmp(keyword, "has", str_len) == 0) {
              parser->bufptr = bufptr;
              parser->expect = SPerl_OP_EXPECT_WORD;
              return HAS;
            }
            else if (memcmp(keyword, "sub", str_len) == 0) {
              parser->bufptr = bufptr;
              parser->expect = SPerl_OP_EXPECT_WORD;
              return SUB;
            }
            else if (memcmp(keyword, "package", str_len) == 0) {
              parser->bufptr = bufptr;
              return PACKAGE;
            }
            else if (memcmp(keyword, "if", str_len) == 0) {
              parser->bufptr = bufptr;
              return IF;
            }
            else if (memcmp(keyword, "elsif", str_len) == 0) {
              parser->bufptr = bufptr;
              return ELSIF;
            }
            else if (memcmp(keyword, "else", str_len) == 0) {
              parser->bufptr = bufptr;
              return ELSE;
            }
            else if (memcmp(keyword, "return", str_len) == 0) {
              parser->bufptr = bufptr;
              return RETURN;
            }
            else if (memcmp(keyword, "for", str_len) == 0) {
              parser->bufptr = bufptr;
              return FOR;
            }
            else if (memcmp(keyword, "last", str_len) == 0) {
              parser->bufptr = bufptr;
              return LAST;
            }
            else if (memcmp(keyword, "next", str_len) == 0) {
              parser->bufptr = bufptr;
              return NEXT;
            }
            else if (memcmp(keyword, "use", str_len) == 0) {
              parser->bufptr = bufptr;
              return USE;
            }
            else if (memcmp(keyword, "while", str_len) == 0) {
              parser->bufptr = bufptr;
              return WHILE;
            }
            else if (memcmp(keyword, "true", str_len) == 0) {
              SPerl_OP* op = SPerl_newOP_flag(SPerl_OP_CONST, NULL, NULL, 0, SPerl_OPp_CONST_BOOLEAN);
              op->uv.int_value = 1;

              parser->bufptr = bufptr;
              SPerl_yylvalp->opval = (SPerl_OP*)op;

              return CONST;
            }
            else if (memcmp(keyword, "false", str_len) == 0) {
              SPerl_OP* op = SPerl_newOP_flag(SPerl_OP_CONST, NULL, NULL, 0, SPerl_OPp_CONST_BOOLEAN);
              op->uv.int_value = 0;

              parser->bufptr = bufptr;
              SPerl_yylvalp->opval = (SPerl_OP*)op;

              return CONST;
            }
            else if (memcmp(keyword, "as", str_len) == 0) {
              parser->bufptr = bufptr;
              return AS;
            }
          }
          
          SPerl_OP* op = SPerl_newOP_flag(SPerl_OP_CONST, NULL, NULL, 0, SPerl_OPp_CONST_STRING);
          op->uv.string_value = keyword;
          
          parser->bufptr = bufptr;
          SPerl_yylvalp->opval = (SPerl_OP*)op;
          return WORD;
        }
        
        /* Return character */
        bufptr++;
        parser->bufptr = bufptr;
        return c;
    }
  }
}

/* Function for error */
void SPerl_yyerror(SPerl_yy_parser* parser, const uint8_t* s)
{
  fprintf(stderr, "error: %s\n", s);
}


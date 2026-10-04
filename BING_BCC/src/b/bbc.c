/*
 * Copyright (c) 2026, GP Interactive (GamePlanet Interactive)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h> 
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "configuration.h"

#ifdef _WIN32
    #define PATH_SEP "\\" 
#else
   #define PATH_SEP "/"
#endif
// --- DATA STRUCTURES ---

typedef enum {
    TOKEN_EOF,
    TOKEN_RETURN,
    TOKEN_NUMBER,
    TOKEN_INCLUDE,
    TOKEN_IDENTIFIER,
    TOKEN_OR,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_SEMI,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_EQUAL,
    TOKEN_COMP_EQUAL,
    TOKEN_IF,
    TOKEN_GOTO,
    TOKEN_COLON,
    TOKEN_BREAK,
    TOKEN_SWITCH,
    TOKEN_CASE,
    TOKEN_DEFAULT,
    TOKEN_ADDRESS,
    TOKEN_NOT_EQUAL,
    TOKEN_NEGATE,
    TOKEN_GREATER,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER_EQUAL,
    TOKEN_BIT_AND,
    TOKEN_BIT_OR,
    TOKEN_BIT_NOT,
    TOKEN_LSHIFT,
    TOKEN_RSHIFT,
    TOKEN_AND,
    TOKEN_ASM,
    TOKEN_COMMA
    
} TokenType;

typedef struct {
	char name[64];
	int is_defined;
} FunctionTable;

FunctionTable function_table[128];
int function_count = 0;

void add_function_to_table(const char* name, int defined) {
	for (int i = 0; i < function_count; i++) {
		if (strcmp(function_table[i].name, name) == 0) {
			if (defined && function_table[i].is_defined){
				printf("[Compile Error] THe function '%s' is not a repeatable function\n", name);
				exit(1);
				}
				if (defined) function_table[i].is_defined = 1;
				return;
			}
		}
		if (function_count >= 128){
			printf("[Compile Error] Overflow of table functions\n");
			exit(1);
			}
			strcpy(function_table[function_count].name, name);
			function_table[function_count].is_defined = defined;
			function_count++;
	}
typedef struct {
    TokenType type;
    char text[64];
    int value;	
} Token;

typedef struct {
    char name[64];
    int offset;
    
} Symbol;

Symbol symbol_table[256];
int symbol_count = 0;
int current_stack_offset = 0;
int if_label_count = 0;
int const_max_break = 64;
char break_labels_stack[64][64];
int break_labels_top = -1;


void push_break_label(const char* label) {
	if(break_labels_top < 63) {
		break_labels_top++;
		strcpy(break_labels_stack[break_labels_top], label);
		}
	}

void pop_break_label() {
	if(break_labels_top >= 0) {
		break_labels_top--;
		}
	}	
#define INCLUDE_MAX_DEPTH 100
FILE* file_stack[INCLUDE_MAX_DEPTH];
int file_stack_top = -1;

FILE* file_in;
FILE* fasm_out;
Token current_token;
int next_char = ' ';

#if defined(__APPLE__) || defined(__MACH__)
int is_bsd = 0; 
int is_win = 0; 
int is_mac = 1; 
int is_bin = 0;
int is_linux = 0;

#elif defined(__WIN32__) || defined(__WIN64__)
int is_bsd = 0; 
int is_win = 1; 
int is_mac = 0; 
int is_bin = 0;
int is_linux = 0;

#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
int is_bsd = 1;
int is_win = 0; 
int is_mac = 0; 
int is_bin = 0;
int is_linux = 0;

#elif defined(__linux__)
int is_bsd = 0;
int is_win = 0;
int is_mac = 0;
int is_bin = 0;
int is_linux = 1;

#else
int is_bsd = 0; 
int is_win = 0; 
int is_mac = 0;
int is_bin = 0; 
int is_linux = 0;
#endif

int read_char() {
    return fgetc(file_in);	
}
// --- LEXER ---
Token get_next_token() {
      Token token;
      memset(&token, 0, sizeof(Token));
      while (next_char != EOF && isspace(next_char)) {
	      next_char = read_char(); 	  
	  }	
      
      if(next_char == EOF) {
		  if(file_stack_top >= 0) {
			  fclose(file_in);
			  file_in = file_stack[file_stack_top];
			  file_stack_top--;
			  next_char = read_char();
			  return get_next_token();
			  }
		else {
		  token.type = TOKEN_EOF;
		  return token;
	  }
		  }
		  
		  if (isdigit(next_char)) {
			  int i = 0;
			  while (next_char != EOF && isdigit(next_char) && i < 63) {
			       token.text[i++] = next_char;
			       next_char = read_char();
			}
			token.type = TOKEN_NUMBER;
			token.value = atoi(token.text);
			return token;
		 } 
		 if (isalpha(next_char) || next_char == '_') {
		      int i = 0;
		      while (next_char != EOF && (isalnum(next_char) || next_char == '_') && i < 63) {
			      token.text[i++] = next_char;
			      next_char = read_char();	  
			  }
		 if(strcmp(token.text, "return") == 0) {
			 token.type = TOKEN_RETURN;
			   
			 } 
			 else if (strcmp(token.text, "if") == 0) {
			 token.type = TOKEN_IF;
				 }
			 else if (strcmp(token.text, "break") == 0) {
			 token.type = TOKEN_BREAK;
			 }
			 else if (strcmp(token.text, "switch") == 0) {
			 token.type = TOKEN_SWITCH;	 
				 }
		     else if (strcmp(token.text, "case") == 0) {
			 token.type = TOKEN_CASE;
				 }
			 else if(strcmp(token.text, "include") == 0) {
				 token.type = TOKEN_INCLUDE;
				 return token;
				 }
			 else if (strcmp(token.text, "default") == 0) {
			 token.type = TOKEN_DEFAULT;
				 }
			 else if (strcmp(token.text, "__asm__") == 0) {
				 token.type = TOKEN_ASM;
				 return token;
				 }
			 else if (strcmp(token.text, "putchar") == 0) {
			 token.type = TOKEN_IDENTIFIER;
			 }
			 else if (strcmp(token.text, "goto") == 0) {
		     token.type = TOKEN_GOTO;		 
				 }
			 
			 else {
			     token.type = TOKEN_IDENTIFIER;	 
			 }
		 return token;
}
		  
	token.text[0] = next_char;
	token.text[1] = '\0';
	int saved_char = next_char;
	next_char = read_char();

	switch (saved_char) {
	   	case '{': token.type = TOKEN_LBRACE; return token;
	   	case '}': token.type = TOKEN_RBRACE; return token;
	   	case '(': token.type = TOKEN_LPAREN; return token;
	   	case ')': token.type = TOKEN_RPAREN; return token;
	   	case ';': token.type = TOKEN_SEMI;   return token;
	   	case ':': token.type = TOKEN_COLON;  return token;
	   	case '-': token.type = TOKEN_MINUS;  return token;
	   	case '+': token.type = TOKEN_PLUS;   return token;
	   	case '*': token.type = TOKEN_MUL;    return token;
	   	case '/': 
	   	   if (next_char == '*') {
			   next_char = read_char();
			   
			   while (next_char != EOF) {
				   if (next_char == '*'){
					   next_char = read_char();
					   
					   if (next_char == '/') {
						   next_char = read_char();
						   break;
						   }
					   } else {
						   next_char = read_char();
						   }
				   }
				   
				   return get_next_token();
			   } else {
				   token.type = TOKEN_DIV;
				   return token;
				   }
	   	case ',': token.type = TOKEN_COMMA;  return token;
	   	case '"':
	   	    token.type = TOKEN_SEMI;
	   	    strcpy(token.text, "\"");
	   	    return token;
	   	case '&': 
	   	    if (next_char == '&') {
			      token.type = TOKEN_AND;
			      strcpy(token.text, "&&");
			      next_char = read_char();
			       } else {
				    
				    token.type = TOKEN_BIT_AND;
				    strcpy(token.text, "&");
				    	   
				}
			return token;
			case '|':
			    if (next_char == '|') {
					token.type = TOKEN_OR;
					strcpy(token.text, "||");
					next_char = read_char();
					} else {
						token.type = TOKEN_BIT_OR;
						strcpy(token.text, "|");
						
						}
						return token;
		case '~':
		        token.type = TOKEN_BIT_NOT;
		        strcpy(token.text, "~");
		        return token;
	   	case '>': 
	   	    if (next_char == '=') {
				  token.type = TOKEN_GREATER_EQUAL;
				  strcpy(token.text, ">=");
				  next_char = read_char();
				} else if (next_char == '>' ) {
					token.type = TOKEN_RSHIFT;
					strcpy(token.text, ">>");
					next_char = read_char();
					} 
				else {
					    token.type = TOKEN_GREATER;
					    strcpy(token.text, ">");
					    
					}
			    return token;
	   	case '<': 
	   	    if (next_char == '=') {
				token.type = TOKEN_LESS_EQUAL;
				strcpy(token.text, "<=");
				next_char = read_char();
				} else if (next_char == '<') { 
					token.type = TOKEN_LSHIFT;
					strcpy(token.text, "<<");
					next_char = read_char();
					}else {
					    token.type = TOKEN_LESS;
					    strcpy(token.text, "<");
					    
					}
				return token;
	   	
	   	case '!':
	   	    if (next_char == '=') {
				token.type = TOKEN_NOT_EQUAL;
				token.text[0] = '!';
				token.text[1] = '=';
				token.text[2] = '\0';
				next_char = read_char();
				} else {
					token.type = TOKEN_NEGATE;
					token.text[0] = '!';
					token.text[1] = '\0';
					}
				return token;
	   	case '=': 
	   	    if (next_char == '=') {
				token.type = TOKEN_COMP_EQUAL;
				token.text[0] = '=';
				token.text[1] = '=';
				token.text[2] = '\0';
				next_char = read_char();
				
				} else {
					token.type = TOKEN_EQUAL;
					token.text[0] = '=';
					token.text[1] = '\0';
					}
				return token;
			     
	   	
	   	            
	}
	
	printf("[Lexer Error] Unknown symbol: %c\n", saved_char);
	exit(1); 
}

// --- SYMBOL TABLE FUNCTIONS ---

void add_symbol(char* name) {
	if(symbol_count >= 256){
		printf("[Compile Error] s~ (Symbol table overflow\n");
		exit(1);
		}
		
		for (int i = 0; i <= symbol_count; i++) {
			if (strcmp(symbol_table[i].name, name) == 0) {
				return;
				}
			}
		
		strcpy(symbol_table[symbol_count].name, name);
		
		current_stack_offset -= 8;
		symbol_table[symbol_count].offset = current_stack_offset;
		
		symbol_count++;
	}

int find_symbol(char* name) {
	for (int i = 0; i < symbol_count; i++) {
		if(strcmp(symbol_table[i]. name, name) == 0) {
			return symbol_table[i].offset;
			}
		}
		
		printf("[Semantic Error] Variable '%s' is undeclared\n", name);
		exit(1);
		
	}

// --- PARSER AND CODEGEN ---


void parse_expression();
void parse_term();
void parse_factor();
void parse_statement();


void match(TokenType type) {
	if(current_token.type == type) {
	   current_token = get_next_token();
	   	} else {
			printf("[Syntax Error] Expected token type %d, but got %d\n", type, current_token.type);
			exit(1);
		}
	}


void parse_factor(){
    if (current_token.type == TOKEN_NUMBER) {
		if(is_bin) {
		   fprintf(fasm_out, "     mov ax, %d\n", current_token.value);	
	  } else {
		   fprintf(fasm_out, "    mov rax, %d\n", current_token.value);
		  }
		match(TOKEN_NUMBER);
		
		
		}
		else if (current_token.type == TOKEN_IDENTIFIER) {
		   char id_name[64];
		   strcpy(id_name, current_token.text);
		   match(TOKEN_IDENTIFIER);
		   
		 
		
	    if (current_token.type == TOKEN_LPAREN) {
			
	       match(TOKEN_LPAREN);
	       
	       int arg_count = 0;
	       
	       if (current_token.type != TOKEN_RPAREN) {
			   while (1){
				   parse_expression();
				   
				   if(is_bin) {
					   fprintf(fasm_out, "      push ax\n");
					   } else {
					   fprintf(fasm_out, "      push rax\n");	   
						   }
						 arg_count++;
						 
						 if (current_token.type == TOKEN_COMMA) {
							 match(TOKEN_COMMA);
							 } else {
								 break;
						}  
				   }
				   
				}
	       match(TOKEN_RPAREN);
	       
	       if (is_bin) {
			   fprintf(fasm_out, "   call %s\n", id_name);
			   if(arg_count > 0) {
				   fprintf(fasm_out, "  add sp, %d\n", arg_count * 2);
				   }
			   } else if (is_mac) {
				   fprintf(fasm_out, "   call _%s\n", id_name);
				   
				   if (arg_count > 0) {
					   fprintf(fasm_out, "  add rsp, %d\n", arg_count * 8);
					   }
			   } else {
				   fprintf(fasm_out, "   call %s\n", id_name);
				   if (arg_count > 0) {
					   fprintf(fasm_out, "  add rsp, %d\n", arg_count * 8);
					   }
				   }
		  }
		  else {
			  int offset = find_symbol(id_name);
			  if (is_bin) {
				  int bin_offset = (offset / 8) * 2;
				  if (bin_offset >= 0) {
				  fprintf(fasm_out, "    mov ax, word [bp + %d]\n", bin_offset);
				  } else {
				  fprintf(fasm_out, "    mov ax, word [bp %d]\n", bin_offset);
					  }
			  }
			  else {
				   if (offset >= 0) {
					   fprintf(fasm_out, "  mov rax, [rbp + %d]\n", offset);
					   } else {
					   fprintf(fasm_out, "  mov rax, [rbp %d]\n", offset);	   
						   }
				  }
	  }	
  }
		else if (current_token.type == TOKEN_MUL) {
			match(TOKEN_MUL);
			if(current_token.type != TOKEN_IDENTIFIER) {
				printf("[Syntax Error] Expected variable name after '*'\n");
				exit(1);
				}
			int offset = find_symbol(current_token.text);
			match(TOKEN_IDENTIFIER);
			
			if (is_bin) {
				int bin_offset = (offset / 8) * 2;
				fprintf(fasm_out, "    mov bx, word [bp %d]\n", bin_offset);
				fprintf(fasm_out, "    mov ax, word [bx]\n");
				} else {
					fprintf(fasm_out, " mov rbx, [rbp %d]\n", offset);
					fprintf(fasm_out, " mov rax, [rbx]\n");
					}
				
			}
		else if (current_token.type == TOKEN_BIT_AND) {
			match(TOKEN_BIT_AND);
			if(current_token.type != TOKEN_IDENTIFIER) {
				printf("[Syntax Error] Expected variable name after '&'\n");
				exit(1);
				}
			int offset = find_symbol(current_token.text);
			match(TOKEN_IDENTIFIER);
			
			if (is_bin) {
				int bin_offset = (offset / 8) * 2;
				fprintf(fasm_out, "     lea ax, [bp %d]\n", bin_offset);
				} else {
				fprintf(fasm_out, "     lea rax, [rbp %d]\n", offset);
					}
			}
		else if (current_token.type == TOKEN_NEGATE) {
            match(TOKEN_NEGATE);
            parse_factor();
            
            static int negate_label_count = 0;
            int local_negate = negate_label_count++;
            char negate_true_lbl[64];
            char negate_end_lbl[64];
            sprintf(negate_true_lbl, ".negate_true_%d", local_negate);
            sprintf(negate_end_lbl,  ".negate_end_%d", local_negate);
            
            if (is_bin) {
			    fprintf(fasm_out, "   cmp ax, 0\n");
			    fprintf(fasm_out, "   je %s\n", negate_true_lbl);
			    fprintf(fasm_out, "   mov ax, 0\n");
			    fprintf(fasm_out, "   jmp %s\n", negate_end_lbl);
			    fprintf(fasm_out, "%s:\n", negate_true_lbl);
			    fprintf(fasm_out, "   mov ax, 1\n");
			    fprintf(fasm_out, "%s:\n", negate_end_lbl);
			    	
			}
			
			else {
				fprintf(fasm_out, "    cmp rax, 0\n");
				fprintf(fasm_out, "    je %s\n", negate_true_lbl);
				fprintf(fasm_out, "    mov rax, 0\n");
				fprintf(fasm_out, "    jmp %s\n", negate_end_lbl);
				fprintf(fasm_out, "%s:\n", negate_true_lbl);
				fprintf(fasm_out, "    mov rax, 1\n");
				fprintf(fasm_out, "%s:\n", negate_end_lbl);
				}
			
}
        else if (current_token.type == TOKEN_BIT_NOT) {
			match(TOKEN_BIT_NOT);
			parse_factor();
			
			if(is_bin) {
				fprintf(fasm_out, "   not ax\n");
				}
			else {
				fprintf(fasm_out, "   not rax\n");
				}
			}
		else {
			printf("[Syntax Error]Expected number or '(' in expression, but got token %d\n", current_token.type);
			exit(1);
			}
}
void parse_term() {
	parse_factor();
	while (current_token.type == TOKEN_MUL || current_token.type == TOKEN_DIV) {
		TokenType op = current_token.type;
		match(op);
		
		if(is_bin) {
			fprintf(fasm_out, "      push ax\n");
			parse_factor();
			fprintf(fasm_out, "      mov bx, ax\n");
			fprintf(fasm_out, "      pop ax\n");
			
			if(op == TOKEN_MUL) {
			   fprintf(fasm_out, "       imul ax, bx\n");	
			} else {
			   fprintf(fasm_out, "       cwd\n");
			   fprintf(fasm_out, "       idiv bx\n");
			}
			
			
			
			} else {
				fprintf(fasm_out, " push rax\n");
				parse_factor();
				fprintf(fasm_out, "  mov rbx, rax\n");
				fprintf(fasm_out, "  pop rax\n");
				
				if (op == TOKEN_MUL) {
					fprintf(fasm_out, "    imul rax, rbx\n");
					} else {
					      fprintf(fasm_out, "   cqo\n");
					      fprintf(fasm_out, "   idiv rbx\n");	
					}
				
				}
		
		}
	}
void parse_expression() {
     parse_term();
     static int comp_label_count = 0;
     
     while (current_token.type == TOKEN_PLUS || current_token.type == TOKEN_MINUS) {
		 TokenType op = current_token.type;
		 match(op);
		 
		 if(is_bin) {
		 fprintf(fasm_out, "     push ax\n");
		 parse_term();
		 fprintf(fasm_out, "     mov bx, ax\n");
		 fprintf(fasm_out, "     pop ax\n");	 
		 
		 if ( op == TOKEN_PLUS) fprintf(fasm_out, "   add ax, bx\n");
		 else                   fprintf(fasm_out, "   sub ax, bx\n");
	 }   else {
	     fprintf(fasm_out, "     push rax\n");
	     parse_term();
	     fprintf(fasm_out, "     mov rbx, rax\n");
	     fprintf(fasm_out, "     pop rax\n");
	     
	     if(op == TOKEN_PLUS)   fprintf(fasm_out, "   add rax, rbx\n");
	     else                   fprintf(fasm_out, "   sub rax, rbx\n");
	} 
		 
		}
	   while (current_token.type == TOKEN_COMP_EQUAL || current_token.type == TOKEN_NOT_EQUAL ||
	          current_token.type == TOKEN_GREATER    || current_token.type == TOKEN_LESS ||
	          current_token.type == TOKEN_GREATER_EQUAL || current_token.type == TOKEN_LESS_EQUAL ||
	          current_token.type == TOKEN_BIT_AND    || current_token.type == TOKEN_BIT_OR ||
	          current_token.type == TOKEN_LSHIFT     || current_token.type == TOKEN_RSHIFT) {
		   TokenType op = current_token.type;
		   match(op);
		   
		   int local_comp = comp_label_count++;
		   char jmp_true_lbl[64];
		   char jmp_end_lbl[64];
		   sprintf(jmp_true_lbl, ".comp_true_%d", local_comp);
		   sprintf(jmp_end_lbl,  ".comp_end_%d", local_comp);
		   
		   if (is_bin) {
			   fprintf(fasm_out, "   push ax\n");
			   parse_term();
			   fprintf(fasm_out, "   mov bx, ax\n");
			   fprintf(fasm_out, "   pop ax\n");
			  
			   if (op == TOKEN_BIT_AND) fprintf(fasm_out, "    and ax, bx\n");
			   else if (op == TOKEN_BIT_OR)  fprintf(fasm_out, " or ax, bx\n");
			   else if (op == TOKEN_LSHIFT)  fprintf(fasm_out, "    mov cx, bx\n  shl ax, cl\n");
			   else if (op == TOKEN_RSHIFT)  fprintf(fasm_out, "    mov cx, bx\n  shr ax, cl\n");
			   else {
			   fprintf(fasm_out, "   cmp ax, bx\n");
			   
			   if (op == TOKEN_COMP_EQUAL)     fprintf(fasm_out, "    je %s\n", jmp_true_lbl);
			   else if (op == TOKEN_NOT_EQUAL) fprintf(fasm_out, "   jne %s\n", jmp_true_lbl);
			   else if (op == TOKEN_GREATER)   fprintf(fasm_out, "    jg %s\n", jmp_true_lbl);
			   else if (op == TOKEN_LESS)      fprintf(fasm_out, "    jl %s\n", jmp_true_lbl);
			   else if (op == TOKEN_GREATER_EQUAL) fprintf(fasm_out, "  jge %s\n", jmp_true_lbl);
			   else if (op == TOKEN_LESS_EQUAL) fprintf (fasm_out, "    jle %s\n", jmp_true_lbl);
			   
			   fprintf(fasm_out, "    mov ax, 0\n");
			   fprintf(fasm_out, "    jmp %s\n", jmp_end_lbl);
			   fprintf(fasm_out, "%s:\n", jmp_true_lbl);
			   fprintf(fasm_out, "    mov ax, 1\n");
			   fprintf(fasm_out, "%s:\n", jmp_end_lbl);
			  }
			   
			   } else {
				   fprintf(fasm_out, "   push rax\n");
				   parse_term();
				   fprintf(fasm_out, "   mov rbx, rax\n");
				   fprintf(fasm_out, "   pop rax\n");
				   
				   if (op == TOKEN_BIT_AND) fprintf (fasm_out, "   and rax, rbx\n");
				   else if (op == TOKEN_BIT_OR)  fprintf (fasm_out, "   or  rax, rbx\n");
				   else if (op == TOKEN_LSHIFT)  fprintf (fasm_out, "   mov rcx, rbx\n   shl rax, cl\n");
				   else if (op == TOKEN_RSHIFT)  fprintf (fasm_out, "   mov rcx, rbx\n   shr rax, cl\n");
				   else {
				   fprintf(fasm_out, "   cmp rax, rbx\n");
				   
				   if (op == TOKEN_COMP_EQUAL) fprintf(fasm_out, "    je %s\n", jmp_true_lbl);
				   else if (op == TOKEN_NOT_EQUAL) fprintf(fasm_out, "   jne %s\n", jmp_true_lbl);
				   else if  (op == TOKEN_GREATER)  fprintf(fasm_out, "   jg %s\n", jmp_true_lbl);
				   else if (op == TOKEN_LESS)      fprintf(fasm_out, "   jl %s\n", jmp_true_lbl);
				   else if (op == TOKEN_GREATER_EQUAL) fprintf(fasm_out, "  jge %s\n", jmp_true_lbl);
			       else if (op == TOKEN_LESS_EQUAL) fprintf (fasm_out, "    jle %s\n", jmp_true_lbl);
				   
				   fprintf(fasm_out, "    mov rax, 0\n");
				   fprintf(fasm_out, "    jmp %s\n", jmp_end_lbl);
				   fprintf(fasm_out, "%s:\n", jmp_true_lbl);
				   fprintf(fasm_out, "    mov rax, 1\n");
				   fprintf(fasm_out, "%s:\n", jmp_end_lbl);
			         }
				   }
		   
				  }	
		 }

void parse_inline_asm() {
	while(next_char != EOF && next_char != '{') {
		next_char = read_char();
		}
		
	if(next_char == EOF) {
		printf("[Syntax Error] Expected '{' after '__asm__'n");
		exit(1);
		}
		
	next_char = read_char();
	
	fprintf(fasm_out, "\n");
	int brace_count = 1;
	while(brace_count > 0 && next_char != EOF) {
		if(next_char == '{') brace_count++;
		if(next_char == '}') brace_count--;
		
		if(brace_count == 0) {
			next_char = read_char();
			break;
			}
		fputc(next_char, fasm_out);
		next_char = read_char();
		}
		current_token = get_next_token();
	}
 
void parse_statement() {
	
	static int switch_label_count = 0;
	static int local_switch = 0;
	static int case_count = 0;
    static char end_switch_lbl[64] = {0};
	if (current_token.type == TOKEN_ASM) {
		parse_inline_asm();
		return;
		}
	else if (current_token.type == TOKEN_RETURN) {
		match(TOKEN_RETURN);
		parse_expression();
		match(TOKEN_SEMI);
		
	   if (is_bin) {
		  fprintf(fasm_out, "      mov sp, bp\n");
		  fprintf(fasm_out, "      pop bp\n");
		  fprintf(fasm_out, "      ret\n");
		  
	   
	 } else {
		  fprintf(fasm_out, "      mov rsp, rbp\n");
		  fprintf(fasm_out, "      pop rbp\n");
		  fprintf(fasm_out, "      ret\n");
		 }
		 return;
		}
		else if (current_token.type == TOKEN_MUL) {
			match(TOKEN_MUL);
			if(current_token.type != TOKEN_IDENTIFIER) {
				printf("[Syntax Error] Expected variable name after '*'\n");
				exit(1);
				}
				
			char var_name[64];
			strcpy(var_name, current_token.text);
			match(TOKEN_IDENTIFIER);
			
			match(TOKEN_EQUAL);
			parse_expression();
			match(TOKEN_SEMI);
			
			int offset = find_symbol(var_name);
			
			if (is_bin) {
				int bin_offset = (offset / 8) * 2;
				fprintf(fasm_out, "    mov bx, word [bp %d]\n", bin_offset);
				fprintf(fasm_out, "    mov word [bx], ax\n");
				} else {
					fprintf(fasm_out, " mov rbx, [rbp %d]\n", offset);
					fprintf(fasm_out, " mov [rbx], rax\n");
					}
				return;
			}
		else if (current_token.type == TOKEN_SWITCH) {
			match(TOKEN_SWITCH);
			match(TOKEN_LPAREN);
			parse_expression();
			match(TOKEN_RPAREN);
			match(TOKEN_LBRACE);
			
			local_switch = switch_label_count++;
			sprintf(end_switch_lbl, ".switch_end_%d", local_switch);
			push_break_label(end_switch_lbl);
			
			case_count = 0;
			char next_case_lbl[64];
			char code_case_lbl[64];
			sprintf(next_case_lbl, ".switch_%d_case_%d", local_switch, case_count);
			
			while(current_token.type != TOKEN_RBRACE && current_token.type != TOKEN_EOF) {
				if(current_token.type == TOKEN_CASE) {
					if (case_count > 0) {
						fprintf(fasm_out, "%s:\n", next_case_lbl);
						}
					match(TOKEN_CASE);
					int val = current_token.value;
					match(TOKEN_NUMBER);
					match(TOKEN_COLON);
					
					
					sprintf(code_case_lbl, ".switch_%d_code_%d", local_switch, case_count);
					case_count++;
					sprintf(next_case_lbl, ".switch_%d_case_%d", local_switch, case_count);
					if (is_bin) fprintf(fasm_out, "  cmp ax, %d\n", val);
					else        fprintf(fasm_out, "   cmp rax, %d\n", val);
					
					fprintf (fasm_out, "     jne %s\n", next_case_lbl);
					
					fprintf (fasm_out, "%s:\n", code_case_lbl);
			}  else if (current_token.type == TOKEN_DEFAULT) {
				if(case_count > 0) {
					fprintf(fasm_out, "%s:\n", next_case_lbl);
					case_count = 0;
					}
				match(TOKEN_DEFAULT);
				match(TOKEN_COLON);
			      }
			   else {
				   parse_statement();
				   }
				}
				if (case_count > 0) {
					fprintf(fasm_out, "%s:\n", next_case_lbl);
					}
				match(TOKEN_RBRACE);
				pop_break_label();
				fprintf(fasm_out, "%s:\n", end_switch_lbl);
				return;
			  }
		else if (current_token.type == TOKEN_IDENTIFIER &&  strcmp(current_token.text, "putchar") == 0 ){
			match (TOKEN_IDENTIFIER);
			match (TOKEN_LPAREN);
			parse_expression();
			match (TOKEN_RPAREN);
			match(TOKEN_SEMI);
			
			if(is_bin) {
				
			   fprintf(fasm_out, "     and ax, 0x00FF\n");
			   fprintf(fasm_out, "     mov ah, 0x0E\n");
			   fprintf(fasm_out, "     int 0x10\n");
			}
			
			else if (is_win) {
			     fprintf(fasm_out, "   mov rcx, rax\n");
			     fprintf(fasm_out, "   sub rsp, 40\n");
			     fprintf(fasm_out, "   call [putchar]\n");
			     fprintf(fasm_out, "   add rsp, 40\n");	
			}
			else if (is_mac) {
			     fprintf(fasm_out, "   mov rdi, rax\n");
			     fprintf(fasm_out, "   call [putchar]\n");
			}
			else if (is_bsd) {
			     fprintf(fasm_out, "   push rax\n");
			     fprintf(fasm_out, "   mov rdi, 1\n");
			     fprintf(fasm_out, "   mov rsi, rsp\n");
			     fprintf(fasm_out, "   mov rdx, 1\n");
		         fprintf(fasm_out, "   mov rax, 4\n");
			     fprintf(fasm_out, "   syscall\n");
			     fprintf(fasm_out, "   mov rax, 1\n");
			     fprintf(fasm_out, "   pop rax\n");
			}
			else  {
				fprintf(fasm_out, "     push rdx\n push rsi\n push rdi\n push rcx\n push r11\n");
				fprintf(fasm_out, "     push rax\n");
				fprintf(fasm_out, "     mov rdi, 1\n");
				fprintf(fasm_out, "     mov rsi, rsp\n");
				fprintf(fasm_out, "     mov rdx, 1\n");
				fprintf(fasm_out, "     mov rax, 1\n");
				fprintf(fasm_out, "     syscall\n");
				fprintf(fasm_out, "     pop rax\n");
				fprintf(fasm_out, "     pop r11\n pop rcx\n pop rdi\n pop rsi\n pop rdx\n");
				
				
				
				
				
			     	
			}
	}   else if (current_token.type == TOKEN_GOTO) {
		    match(TOKEN_GOTO);
		    
		    if (current_token.type != TOKEN_IDENTIFIER) {
				printf("[Syntax Error] Expected label name after 'goto'\n");
				exit(1);
				}
				
			fprintf(fasm_out, "        jmp .user_lbl_%s\n", current_token.text);
			match(TOKEN_IDENTIFIER);
			match(TOKEN_SEMI);
		}
		else if (current_token.type == TOKEN_BREAK) {
			match(TOKEN_BREAK);
			match(TOKEN_SEMI);
			
			if (break_labels_top >= 0) {
				
				fprintf(fasm_out, "     jmp %s\n", break_labels_stack[break_labels_top]);
				
				}
			else {
				
				printf("[Semantic Error] 'break' statement not within loop context\n");
				exit (1);
				}
			return;
			
			}
		
	    else if (current_token.type == TOKEN_IF) {
			match(TOKEN_IF);
			match(TOKEN_LPAREN);
			
			parse_expression();
			
			
				
			
			match(TOKEN_RPAREN);
			match(TOKEN_LBRACE);
			if (is_bin) {
				    fprintf(fasm_out, "    cmp ax, 0\n");
				} else {
					fprintf(fasm_out, "    cmp rax, 0\n");
					}
			int local_label = if_label_count++;
			fprintf(fasm_out, "    je .if_false_%d\n", local_label);
			
			while (current_token.type != TOKEN_RBRACE && current_token.type != TOKEN_EOF) {
				parse_statement();
				}
				
			match(TOKEN_RBRACE);
			
			if(current_token.type == TOKEN_IDENTIFIER && strcmp(current_token.text, "else") == 0) {
				match(TOKEN_IDENTIFIER);
				
				fprintf(fasm_out, "  jmp .if_end_%d\n", local_label);
				
				fprintf(fasm_out, ".if_false_%d:\n", local_label);
				
				match(TOKEN_LBRACE);
				
				while (current_token.type != TOKEN_RBRACE && current_token.type != TOKEN_EOF) {
					parse_statement();
					}
					
					match(TOKEN_RBRACE);
					fprintf(fasm_out, ".if_end_%d:\n", local_label);
					
                    					
				} else {
					fprintf(fasm_out, ".if_false_%d:\n", local_label);
					}
			
			return;
			
			}
			
		else if (current_token.type == TOKEN_IDENTIFIER && strcmp(current_token.text, "while") == 0) {
		    match(TOKEN_IDENTIFIER);
		    
		    static int while_label_count = 0;
		    int local_while_label = while_label_count++;
		    
		    char end_while_label[64];
		    sprintf(end_while_label, ".while_end_%d", local_while_label);
		    
		    fprintf(fasm_out, ".while_start_%d: \n", local_while_label);
		    
		    match(TOKEN_LPAREN);
		    
		    parse_expression();
		    
		    match(TOKEN_RPAREN);
		    match(TOKEN_LBRACE);
		    
		    push_break_label(end_while_label);
		    
		    if(is_bin) {
				fprintf(fasm_out, "     cmp ax, 0\n");
			} else {
				fprintf(fasm_out, "      cmp rax, 0\n");
				}
			fprintf(fasm_out, "         je %s\n", end_while_label);
			
			while (current_token.type != TOKEN_RBRACE && current_token.type != TOKEN_EOF) {
			parse_statement();
				}
			match(TOKEN_RBRACE);
			
			pop_break_label();
			
			fprintf(fasm_out, "        jmp .while_start_%d\n", local_while_label);
			fprintf(fasm_out, "%s:\n", end_while_label);
			return;
		    
			}
		else if (current_token.type == TOKEN_IDENTIFIER) {
		  char var_name[64];
		  strcpy(var_name, current_token.text);
		  match(TOKEN_IDENTIFIER);
		  
		  if (current_token.type == TOKEN_COLON) {
			  match(TOKEN_COLON);
			  
			  fprintf(fasm_out, ".user_lbl_%s:\n", var_name);
			  
			  return;
		  }
		  
		  else if (current_token.type == TOKEN_LPAREN) {
			  match(TOKEN_LPAREN);
			  int arg_count = 0;
			  
			  if(current_token.type != TOKEN_RPAREN) {
				  while(1) {
					  parse_expression();
					  if (is_bin) {
						  fprintf(fasm_out, "   push ax\n");
						  }
					  else {
						  fprintf(fasm_out, "   push rax\n");
						  }
					
					  arg_count++;
					  
					  if(current_token.type == TOKEN_COMMA) {
						  match(TOKEN_COMMA);
						  } else {
							  break;
							  }
					  }
					  
				  }
			  match(TOKEN_RPAREN);
			  match(TOKEN_SEMI);
			  
			  if (is_bin) {
				  fprintf(fasm_out, "   call %s\n", var_name);
				  if (arg_count > 0) fprintf(fasm_out, "     add sp, %d\n", arg_count * 2);
				  }
		else  if (is_mac) {
			      fprintf(fasm_out, "   call _%s\n",var_name);
			      if (arg_count > 0) fprintf(fasm_out, "     add rsp, %d\n", arg_count * 8);
			
			}
		      else {
				  fprintf(fasm_out, "   call %s\n", var_name);
				  if (arg_count > 0) fprintf(fasm_out, "     add rsp, %d\n", arg_count * 8);
				  }
				  
				 return;
			  
			  }
		  
		  else {
		  
		  match(TOKEN_EQUAL);
		  
		  parse_expression();
		  
		  match(TOKEN_SEMI);
		  
		  add_symbol(var_name);
		  int offset = find_symbol(var_name);
		  
		  if(is_bin) {
			 int bin_offset = (offset / 8) * 2;
			 if(bin_offset >= 0) {
			     fprintf(fasm_out, "   mov word [bp + %d], ax\n", bin_offset);
		     } else {
				 fprintf(fasm_out, "   mov word [bp %d], ax\n",   bin_offset);
				 }
			   
			} else {
				if (offset >= 0) {
			    fprintf(fasm_out, " mov [rbp + %d], rax\n", offset);	
			    } else {
					fprintf(fasm_out, " mov [rbp %d], rax\n", offset);
					}
			}	
		} 
	}
		
		else {
			fprintf(stderr,"[Error] Unknown Instruction: '%s'\n", current_token.text);
		    exit(1);
		}
}

void parse_function() {
    char func_name[64];
    strcpy(func_name, current_token.text);
    match(TOKEN_IDENTIFIER);
    match(TOKEN_LPAREN);
    
    char args[32][64];
    int arg_count = 0;
    
    if (current_token.type != TOKEN_RPAREN) {
		while (1) {
			if (current_token.type == TOKEN_IDENTIFIER) {
				strcpy(args[arg_count++], current_token.text);
				match(TOKEN_IDENTIFIER);
				}
			if (current_token.type == TOKEN_COMMA) {
				match(TOKEN_COMMA);
				} else {
					break;
				}
			}
		}
    match(TOKEN_RPAREN);
   
   if (current_token.type == TOKEN_SEMI) {
	   match(TOKEN_SEMI);
	   add_function_to_table(func_name, 0);
	   return;
	   }
	   
   
    match(TOKEN_LBRACE);
    add_function_to_table(func_name, 1);
    
   
      if(is_mac) {
		fprintf(fasm_out, "extern _putchar\n\n");
		if(strcmp(func_name, "main") == 0) {
	    fprintf(fasm_out, "entry _%s\n\n", func_name);
	    }
	    fprintf(fasm_out, "_%s:\n", func_name);
	    fprintf(fasm_out, "    push rbp\n");
	    fprintf(fasm_out, "    mov rbp, rsp\n");
	    fprintf(fasm_out, "    sub rsp, 256\n");
    } else if (is_bin) {
		fprintf(fasm_out, "%s:\n", func_name);
		if(strcmp(func_name, "main") == 0) {
      fprintf(fasm_out,  "   cli\n");
      fprintf(fasm_out, "    xor ax, ax\n");         
      fprintf(fasm_out, "    mov ds, ax\n");         
      fprintf(fasm_out, "    mov es, ax\n");         
      fprintf(fasm_out, "    mov ss, ax\n");
		fprintf(fasm_out, "    mov sp, 0x7C00\n");
		fprintf(fasm_out, "    mov bp, sp\n"); 
      fprintf(fasm_out, "    sti\n");
	  } else {
		  fprintf(fasm_out,"   push bp\n");
		  fprintf(fasm_out, "  mov bp, sp\n");
		  }
	} else{
    fprintf(fasm_out, "%s:\n", func_name);
    fprintf(fasm_out, "    push rbp\n");
    fprintf(fasm_out, "    mov rbp, rsp\n");
    fprintf(fasm_out, "    sub rsp, 256\n");
    }
    
    symbol_count = 0;
    current_stack_offset = 0;
    
    for (int i = 0; i < arg_count; i++) {
		strcpy(symbol_table[symbol_count].name, args[i]);
		if (is_bin) {
			symbol_table[symbol_count].offset = (4 + (i * 2)) * 4;
			
			} else {
				symbol_table[symbol_count].offset = 16 + (i * 8);
				}
			    symbol_count++;
		}
    
    while(current_token.type != TOKEN_RBRACE && current_token.type != TOKEN_EOF) {
	    parse_statement();
		}
	match(TOKEN_RBRACE);
	
	if(is_bin) {
		if(strcmp(func_name, "main") != 0) {
		fprintf(fasm_out, "     pop bp\n");
		fprintf(fasm_out, "     ret\n");
			}
		else {
			fprintf(fasm_out, "   cli\n");
			fprintf(fasm_out, ".halt_loop:\n");
			fprintf(fasm_out, "    hlt\n");
			fprintf(fasm_out, "    jmp .halt_loop\n");
			}
	
	} else {
		fprintf(fasm_out, "    mov rsp, rbp\n");
		fprintf(fasm_out, "    pop rbp\n");
		fprintf(fasm_out, "    ret\n");
		}
	
}

// --- COMPILER_MAIN --- 

int main (int argc, char* argv[]){
	char resolved_path[1024] = {0};
	char compiler_dir[512] = {0};
	if (realpath(argv[0], resolved_path) != NULL) {
	strcpy(compiler_dir, argv[0]);
	char* last_sep = strrchr(compiler_dir, '/');
	if (last_sep != NULL) *last_sep = '\0';
	} else {

        strcpy(compiler_dir, ".");
		
	}
	
	if(argc < 2) {
		printf("Usage: %s <input_file.b> [-bsd] [-mac], [-win], [-bin], [-linux]\n", argv[0]);
		return 1;
	}
	
	
	
	char* input_filename = NULL;
	srand(time(NULL));
	
	for(int i = 1; i < argc; i++) {
	 if(strcmp(argv[i], "-V") == 0) {
		   printf("%s (BCC) version %s\n", BCC_COLLECTION_NAME, VERSION);
		   printf("Compiler Identity: %s\n", BCC_COMPILER_IDENTITY);
		   printf("Target Frontend: %s\n", BCC_TARGET_LANG);
		   return 0;
	   } else if(strcmp(argv[i], "-h") == 0) {
		   printf("[Usage]: %s [options] file...\n", argv[0]);
		   printf("Options:\n");
		   printf("    -V     Display compiler version\n");
		   printf("    -h     Display helper menu\n");
		   printf("    -win   Target 64-bit Windows OS\n");
		   printf("    -mac   Target 64-bit MacOS (MachO64 format)\n");
		   printf("    -bsd   Target 64-bit FreeBSD/OpenBSD/NetBSD (ELF64 format)\n");
		   printf("    -bin   Target freestanding 16-bit Master Boot Record (MBR) binary\n");
		   printf("    -linux Target 64-bit Linux (ELF64 format)\n");
		   return 0;
		   }
	   
	 
	 else if (strcmp(argv[i], "-bsd") == 0) {
			
		    	is_bsd = 1; is_mac = 0; is_win = 0;
		    	is_bin = 0; is_linux = 0;
			
			} else if (strcmp(argv[i], "-win") == 0)  {
			     is_win = 1; is_mac = 0; is_bsd = 0;  is_bin = 0; is_linux = 0;
			} else if (strcmp(argv[i], "-mac") == 0)   {
			     is_mac = 1; is_bsd = 0; is_win = 0; is_bin = 0; is_linux = 0;
			} else if(strcmp(argv[i], "-bin") == 0) {
				 is_bin = 1; is_mac = 0; is_win = 0; is_bsd = 0; is_linux = 0;
			} else if(strcmp(argv[i], "-linux") == 0) {
				 is_linux = 1; is_mac = 0; is_win = 0; is_bsd = 0; is_bin = 0;
			} else {
			    input_filename = argv[i];	
			}
		}
	if (input_filename == NULL) {
	    printf("Error: No input file specified.\n");
	    return 1;	
	}
	
	char asm_filename[256];
	char out_filename[256];
	
	strcpy(asm_filename, input_filename);
	strcpy(out_filename, input_filename);
	
	char* dot = strrchr(asm_filename,'.');
	if (dot != NULL){
	    *dot = '\0';
	}
	
	strcat(asm_filename, ".asm");
	
	dot = strrchr(out_filename, '.');
	if (dot != NULL) {
	    *dot = '\0';
	}
	
	if(is_win) {
	   strcat(out_filename, ".exe");	
	} else if(is_bin) {
	   strcat(out_filename, ".bin"); 	
	} else {
	
	}
	
	file_in = fopen(input_filename, "r");
	if (!file_in) {
	    perror("Failed to open the input file\n");
	    return 1;	
	}
	
	fasm_out = fopen(asm_filename, "w");
	if (!fasm_out) {
	   perror("Failed to create file output.asm");
	   fclose(file_in);
	   return 1;	
	}
	if(is_win) {
	   // Format for 64-bit Windows (Console)
	   fprintf(fasm_out, "WINDOWS = 1\n");
	   fprintf(fasm_out, "format PE64 console\n");
	} else if(is_mac) {
	   // Format for MAC
	   fprintf(fasm_out, "MACH = 1\n");
	   fprintf(fasm_out, "format MachO64 executable\n");
	   fprintf(fasm_out, "interpreter '/usr/lib/dyld'\n");
	   fprintf(fasm_out, "uses '/usr/lib/libSystem.B.dylib'\n\n");
	} else if(is_bin) {
		// Format for 16-bit MBR
		fprintf(fasm_out, "MBR = 1\n");
		fprintf(fasm_out, "format binary\n");
		fprintf(fasm_out, "use16\n");
		fprintf(fasm_out, "org 0x7C00\n\n");
	} else {
	   // Format for LINUX/BSD
		if (is_bsd) {
            fprintf(fasm_out, "BSD = 1\n");
		}
		else {

            fprintf(fasm_out, "LINUX = 1\n");
			
		}
	   fprintf(fasm_out, "format ELF64 executable at 0x400000\n");
	   fprintf(fasm_out, "segment readable executable\n\n");
	   
	   fprintf(fasm_out, "entry _start\n\n");
	   fprintf(fasm_out, "_start:\n");
	   fprintf(fasm_out, "     call main\n");
	   fprintf(fasm_out, "     mov rdi, rax\n");
	   
	   if (is_bsd) {
		   fprintf(fasm_out, "     mov rax, 1\n");
		   fprintf(fasm_out, "     syscall\n");

		   }
	  else {
		   fprintf(fasm_out, "     mov rax, 60\n");
		   fprintf(fasm_out, "     syscall\n");
		  }
    }
	next_char = read_char();
	current_token = get_next_token();
	
	while (current_token.type != TOKEN_EOF) {
		if (current_token.type == TOKEN_IDENTIFIER) {
			parse_function();
			} else if (current_token.type == TOKEN_INCLUDE){
				match(TOKEN_INCLUDE);
				
				char include_filename[256];
				int idx = 0;
				
				while (next_char == ' ' || next_char == '"' || next_char =='<'){
					   next_char = read_char();
					  }
				
				while (next_char != EOF && next_char != '"' && next_char != '>') {
					    include_filename[idx++] = next_char;
					    next_char = read_char();
					}
				include_filename[idx] = '\0';
				
				next_char = read_char();
				while (next_char != EOF && next_char != ';') {
					   next_char = read_char();
					
					}
				if (next_char == ';') next_char = read_char();
				
				char* ext = strrchr(include_filename, '.');
				if (ext == NULL || strcmp(ext, ".b") != 0) {
					    printf("[Compile Error] include only supports .b files! Attemp: %s\n", include_filename);
					    exit(1);
					}
				
				if (file_stack_top >= INCLUDE_MAX_DEPTH -1) {
					printf("[Compile Error] Include nesting depth exceeded standard limit of %d\n", INCLUDE_MAX_DEPTH);
					exit(1);
					}
				file_stack_top++;
				file_stack[file_stack_top] = file_in;
				
				file_in = fopen(include_filename, "r");
				if (!file_in) {
					char system_include_path[512];
					sprintf(system_include_path, "include%s%s", PATH_SEP, include_filename);
					
					file_in = fopen(system_include_path, "r");
					if(!file_in) {
					printf("[Compile Error] Could not find the file '%s' in the project's directory, nor in the 'include/'\n", include_filename);
					exit(1);
				}
					}
					
					next_char = read_char();
					current_token = get_next_token();
				}else {
				printf("[Compile Error] Undefined token %d in global region\n", current_token.type);
				exit(1);
				}
		}
	    if (is_bin) {
			fprintf(fasm_out,"\n");
			fprintf(fasm_out, "times 510 - ($ - $$) db 0\n");
			fprintf(fasm_out, "dw 0xAA55\n");
			}
       else	if (is_win) {
	    fprintf(fasm_out, "\nsection '.idata' import data readable writeable\n");
	    fprintf(fasm_out, " include '%s/include/macro/import64.inc'\n\n", compiler_dir);
	    fprintf(fasm_out, " library kernel32, 'KERNEL32.DLL', msvcrt, 'MSVCRT.DLL'\n\n");
	    fprintf(fasm_out, " import kernel32, ExitProcess, 'ExitProcess'\n\n");
	    fprintf(fasm_out, " import msvcrt, putchar,'putchar'\n");
	    
	      	
	}
	
	fclose(file_in);
	fclose(fasm_out);
	
	
	char fasm_command[1024];

	    sprintf(fasm_command, "fasm \"%s\" \"%s\"", asm_filename, out_filename);
	
	

	
	int status = system(fasm_command);
	
	if (status == 0) {
	    const char* jokes[] = {
		    "The code is so clean that your processor just had an orgasm.",
		    "Python looks at this build's speed and fals into a depression.",
		    "FASM completed two passes; in that time, GCC would have barely finished reading stdio.h.",
		    "This binary will run even on your neighbor's iron.",
		    "Binary Is Not GNU. Thank God! Build succesful.",
		    "Congratulations, your code is officialy freestanding.The hardware says 'Thank you'."
		};
		int total_jokes = sizeof(jokes) / sizeof(jokes[0]);
		
		int random_index = rand() % total_jokes;
	    printf("Joke of the day: %s\n\n", jokes[random_index]);
	} else {
	   printf("[Epic Shitshow] Backend compilation via FASM failed. Go fix your code!\n\n");
	}
	
	return 0;
}

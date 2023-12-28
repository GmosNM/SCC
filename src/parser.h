#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lexer.h"
#include "ast.h"
#include "symbol_table.h"

typedef struct parser {
    token* tokens;
    int num_tokens;
    int current_token_index;
    symbol_table* global_symbol_table;
    scope* current_scope;
} parser;

parser init_parser(lexer* l, token* tokens);
void consume(parser* p, tag);
void consume_simicolon(parser* p);
token get_current_token(parser* p);
token get_next_token(parser* p);
token get_next_next_token(parser* p);
bool is_unary_operator(tag kind);
bool is_binary_operator(tag kind);
token get_prev_token(parser* p);
ast_program_node* parse_program(parser* p);
ast_variable_decl_node* parse_variable_declaration(parser* p, scope* s);
builtin_types parse_type(parser* p);
ast_node* parse_identifier(parser* p);
ast_node* parse_literal(parser* p);
ast_node* parse_declaration(parser* p, scope* scope);
ast_assignment_node* parse_assignment(parser* p);
ast_block_node* parse_block(parser* p);
ast_function_decl_node* parse_function_declaration(parser* p);
ast_return_node* parse_return_stmt(parser* p);
ast_binary_expr_node* parse_binary_expr(parser* p);

#endif // PARSER_H


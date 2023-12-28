#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum {
    INT,
    CHAR,
    DOUBLE,
    VOID,
} builtin_types;

typedef enum {
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_MODULO,
    OP_ASSIGN,
    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_LOGICAL_NOT,
    OP_INCREMENT,
    OP_DECREMENT,
} operator_type;

typedef enum {
    AST_FUNCTION_DECL,
    AST_VARIABLE_DECL,
    AST_IDENTIFIER,
    AST_RETURN_STMT,
    AST_PARAMETER,
    AST_BINARY_EXPR,
    AST_UNARY_EXPR,
    AST_ASSIGNMENT,
    AST_PROGRAM,
    AST_BLOCK,
    AST_LITERAL
} ast_node_type;

typedef struct ast_node {
    ast_node_type type;
    char* value;
    char* type_str;
    struct ast_node** children;
    size_t num_children;
} ast_node;

typedef struct ast_program_node {
    ast_node_type type;
    struct ast_node** declarations;
    size_t num_declarations;
} ast_program_node;

typedef struct ast_literal_node {
    ast_node_type type;
    char* value;
} ast_literal_node;

typedef struct ast_identifier_node {
    ast_node_type type;
    char* value;
} ast_identifier_node;

typedef struct ast_unary_expr_node {
    ast_node_type type;
    struct ast_node* operand;
    operator_type op;
} ast_unary_expr_node;

typedef struct ast_binary_expr_node {
    ast_node_type type;
    struct ast_node* left;
    struct ast_node* right;
    operator_type op;
} ast_binary_expr_node;

typedef struct ast_variable_decl_node {
    ast_node_type type;
    builtin_types type_node;
    ast_node* identifier_node;
    ast_node* value;
    bool is_constant;
} ast_variable_decl_node;

typedef struct ast_assignment_node {
    ast_node_type type;
    ast_node* identifier_node;
    ast_node* value;
} ast_assignment_node;

typedef struct ast_block_node {
    ast_node_type type;
    struct ast_node** declarations;
    size_t num_declarations;
} ast_block_node;

typedef struct ast_function_decl_node {
    ast_node_type type;
    builtin_types return_type;
    char* function_name;
    ast_node** parameters;
    size_t num_parameters;
    ast_block_node* body;
} ast_function_decl_node;

typedef struct ast_return_node {
    ast_node_type type;
    ast_node* expr;
} ast_return_node;

void print_ast_node(ast_node* node, int level);
void print_ast(ast_program_node* root);
const char* type_tostring(builtin_types type);
void add_child(ast_node* parent, ast_node* child);
const char* op_ToString(operator_type op);
ast_block_node* create_block_node();
ast_assignment_node* create_assignment_node(ast_node* identifier_node, ast_node* value);
ast_variable_decl_node* create_variable_decl_node(builtin_types type_node, ast_node* identifier_node,
                                                  ast_node* value, bool constant);
ast_program_node* create_program_node();
ast_node* create_ast_node(ast_node_type type, const char* value, const char* type_str);
ast_function_decl_node* create_function_decl_node(builtin_types return_type, const char* function_name,
                                                  ast_node** parameters, size_t num_parameters, ast_block_node* body);
ast_literal_node* create_literal_node(const char* value);
ast_binary_expr_node* create_binary_expr_node(ast_node* left, ast_node* right, operator_type op);
ast_unary_expr_node* create_unary_expr_node(ast_node* operand, operator_type op);
ast_return_node* create_return_node(ast_node* value);

#endif // AST_H


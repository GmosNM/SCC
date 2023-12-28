#include "ast.h"


const char* type_tostring(builtin_types type){
    switch (type) {
        case INT:
            return "int";
            break;
        case VOID:
            return "void";
            break;
        case DOUBLE:
            return "double";
            break;
        case CHAR:
            return "char";
            break;
        default:
            return "ERROR: type to string!\n";
    }
}

void print_ast_node(ast_node* node, int level) {
    if (node == NULL) {
        printf("ROOT!\n");
        return;
    }
    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    switch (node->type) {
        case AST_FUNCTION_DECL:
            printf("Function Declaration\n");
            ast_function_decl_node * function_node_decl = (ast_function_decl_node*)node;
            printf("return type: %s\n", type_tostring(function_node_decl->return_type));
            printf("Parameters: \n");
            for (size_t i = 0; i < function_node_decl->num_parameters; ++i) {
                print_ast_node(function_node_decl->parameters[i], 1);
            }
            printf("Body: \n");
            for (size_t i = 0; i < function_node_decl->body->num_declarations; ++i) {
                print_ast_node(function_node_decl->body->declarations[i], 1);
            }
            break;
        case AST_VARIABLE_DECL: {
            ast_variable_decl_node* var_decl = (ast_variable_decl_node*)node;
            printf("Variable Declaration\n");
            for (int i = 0; i < level; i++) {
                printf("  ");
            }
            char* constant = "false";
            if (var_decl->is_constant){
                constant = "true";
            }
            printf("  constant = %s\n", constant);
            printf("    Type: %s\n",type_tostring(var_decl->type_node));
            print_ast_node(var_decl->identifier_node, level + 1);
            if (var_decl->value != NULL) {
                print_ast_node(var_decl->value, level + 1);
            }
            break;
        }
        case AST_IDENTIFIER:
            printf("Identifier: %s\n", node->value);
            break;
        case AST_RETURN_STMT:
            ast_return_node* return_stmt = (ast_return_node*)node;
            printf("Return Statement\n");
            print_ast_node(return_stmt->expr, level + 1);
            break;
        case AST_PARAMETER:
            printf("Parameter\n");
            break;
        case AST_BINARY_EXPR:
            ast_binary_expr_node* binary_epxr = (ast_binary_expr_node*)node;
            printf("Binary Expression: %s\n", node->value);
            print_ast_node(binary_epxr->left, level + 1);
            for (int i = 0; i < level; i++) {
                printf("  ");
            }
            printf("  oprator:  %s\n", op_ToString(binary_epxr->op));
            print_ast_node(binary_epxr->right, level + 1);
            break;
        case AST_UNARY_EXPR:
            printf("Unary Expression: %s\n", node->value);
            break;
        case AST_ASSIGNMENT:
            ast_assignment_node* var_decl = (ast_assignment_node*)node;
            printf("Variable Assignment: %s\n", node->value);
            printf("  Identifier: %s\n", var_decl->identifier_node->value);
            // Print value
            if (var_decl->value != NULL) {
                printf("  Value: ");
                print_ast_node(var_decl->value, level + 1);
            }
            break;
        case AST_PROGRAM:
            printf("Program\n");
            break;
        case AST_BLOCK:
            printf("Block: \n");
            ast_block_node * block_node_decl = (ast_block_node*)node;
            for (size_t i = 0; i < block_node_decl->num_declarations; ++i) {
                print_ast_node(block_node_decl->declarations[i], 0);
            }
            break;
        case AST_LITERAL:
            printf("Literal: %s\n", node->value);
            break;
        default:
            printf("Unknown Node Type\n");
    }

}

const char* op_ToString(operator_type op){
    switch(op){
        case OP_ADD:
            return "+";
        case OP_SUBTRACT:
            return "-";
        case OP_MULTIPLY:
            return "*";
        case OP_DIVIDE:
            return "/";
        case OP_MODULO:
            return "%";
        case OP_ASSIGN:
            return "=";
        case OP_EQUAL:
            return "==";
        case OP_NOT_EQUAL:
            return "!=";
        case OP_LOGICAL_NOT:
            return "!";
        case OP_INCREMENT:
            return "+=";
        case OP_DECREMENT:
            return "-=";
    }
}

void print_ast(ast_program_node* root) {
    if (root == NULL || root->num_declarations <= 0) {
        printf("AST is empty.\n");
    } else {
        printf("SIZE OF AST: %zu\n", root->num_declarations);
        for (size_t i = 0; i < root->num_declarations; ++i) {
            print_ast_node(root->declarations[i], 0);
        }
    }
}

void add_child(ast_node* parent, ast_node* child) {
    ast_program_node* program_node = (ast_program_node*)parent;

    // Allocate memory for the new child
    ast_node** new_declarations = realloc(program_node->declarations, (program_node->num_declarations + 1) * sizeof(ast_node*));

    if (new_declarations == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in add_child.\n");
        exit(1);
    }

    program_node->declarations = new_declarations;
    program_node->declarations[program_node->num_declarations++] = child;
}


ast_assignment_node* create_assignment_node(ast_node* identifier_node, ast_node* value) {
    ast_assignment_node* assignment_node = malloc(sizeof(ast_assignment_node));
    assignment_node->type = AST_ASSIGNMENT;
    assignment_node->identifier_node = identifier_node;
    assignment_node->value = value;
    return assignment_node;
}

ast_variable_decl_node* create_variable_decl_node(builtin_types type_node, ast_node* identifier_node, ast_node* value,
                                                    bool constant) {
    ast_variable_decl_node* decl_node = malloc(sizeof(ast_variable_decl_node));
    decl_node->type = AST_VARIABLE_DECL;
    decl_node->type_node = type_node;
    decl_node->identifier_node = identifier_node;
    decl_node->value = value;
    decl_node->is_constant = constant;
    return decl_node;
}

ast_program_node* create_program_node() {
    ast_program_node* program_node = malloc(sizeof(ast_program_node));
    program_node->type = AST_PROGRAM;
    program_node->declarations = NULL;
    program_node->num_declarations = 0;
    return program_node;
}

ast_block_node* create_block_node() {
    ast_block_node* block_node = malloc(sizeof(ast_block_node));
    block_node->type = AST_BLOCK;
    block_node->declarations = NULL;
    block_node->num_declarations = 0;
    return block_node;
}

ast_node* create_ast_node(ast_node_type type, const char* value, const char* type_str) {
    ast_node* node = malloc(sizeof(ast_node));
    node->type = type;
    node->value = (value != NULL) ? _strdup(value) : NULL;
    node->type_str = (type_str != NULL) ? _strdup(type_str) : NULL;
    node->children = NULL;
    node->num_children = 0;
    return node;
}


ast_function_decl_node* create_function_decl_node(builtin_types return_type, const char* function_name, ast_node** parameters, size_t num_parameters, ast_block_node* body) {
    ast_function_decl_node* node = (ast_function_decl_node*)malloc(sizeof(ast_function_decl_node));
    if (!node) {
        fprintf(stderr, "Memory allocation failed for function declaration node.\n");
        exit(EXIT_FAILURE);
    }

    node->type = AST_FUNCTION_DECL;
    node->return_type = return_type;
    node->function_name = _strdup(function_name);

    // Copy parameters
    node->parameters = (ast_node**)malloc(num_parameters * sizeof(ast_node*));
    if (!node->parameters) {
        fprintf(stderr, "Memory allocation failed for function parameters.\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < num_parameters; ++i) {
        node->parameters[i] = parameters[i];
    }
    node->num_parameters = num_parameters;

    node->body = body;

    return node;
}


// Function to create a return statement node
ast_return_node* create_return_node(ast_node* expr) {
    ast_return_node* node = (ast_return_node*)malloc(sizeof(ast_return_node));
    if (node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for return statement node.\n");
        exit(EXIT_FAILURE);
    }

    node->type = AST_RETURN_STMT;
    node->expr = expr;

    return node;
}


ast_binary_expr_node* create_binary_expr_node(ast_node* left, ast_node* right, operator_type op) {
    ast_binary_expr_node* node = (ast_binary_expr_node*)malloc(sizeof(ast_binary_expr_node));
    if (node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for binary expression node.\n");
        exit(EXIT_FAILURE);
    }

    node->type = AST_BINARY_EXPR;
    node->left = left;
    node->right = right;
    node->op = op;

    return node;
}

ast_unary_expr_node* create_unary_expr_node(ast_node* operand, operator_type op) {
    ast_unary_expr_node* node = (ast_unary_expr_node*)malloc(sizeof(ast_unary_expr_node));
    if (node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for unary expression node.\n");
        exit(EXIT_FAILURE);
    }

    node->type = AST_UNARY_EXPR;
    node->operand = operand;
    node->op = op;

    return node;
}

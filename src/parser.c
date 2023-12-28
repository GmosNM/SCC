#include "parser.h"


void consume(parser* p, tag expected) {
    if (get_current_token(p).kind != expected){
        fprintf(stderr, "Error: Expected '%s' but found '%s'.\n", tag_tostring(expected), get_current_token(p).lexme);
        exit(1);
    }
    if (p->current_token_index < p->num_tokens - 1) {
        p->current_token_index++;
    } else {
        fprintf(stderr, "Error: Attempting to consume beyond the end of tokens.\n");
        exit(1);
    }
}

void consume_simicolon(parser* p) {
    if (get_current_token(p).kind == SIMICOLON) {
        consume(p, SIMICOLON);
    } else {
        fprintf(stderr, "Error: Expected ';', got %s\n", get_current_token(p).lexme);
        exit(1);
    }
}

token get_current_token(parser* p) {
    return p->tokens[p->current_token_index];
}

token get_next_token(parser* p) {
    if (p->current_token_index < p->num_tokens - 1) {
        return p->tokens[p->current_token_index + 1];
    } else {
        fprintf(stderr, "Error: Attempting to access beyond the end of tokens.\n");
        exit(1);
    }
}

token get_next_next_token(parser* p) {
    if (p->current_token_index < p->num_tokens - 2) {
        return p->tokens[p->current_token_index + 2];
    } else {
        fprintf(stderr, "Error: Attempting to access beyond the end of tokens.\n");
        exit(1);
    }
}

token get_prev_token(parser* p) {
    if (p->current_token_index > 0) {
        return p->tokens[p->current_token_index - 1];
    } else {
        fprintf(stderr, "Error: Attempting to access before the start of tokens.\n");
        exit(1);
    }
}

bool is_type(token token) {
    switch (token.kind) {
        case KW_INT:
        case KW_CHAR:
        case KW_VOID:
        case KW_DOUBLE:
            return true;
        default:
            return false;
    }
}

bool is_unary_operator(tag kind) {
    return kind == PLUS || kind == MINUS || kind == LOGICAL_NOT || kind == INCREMENT || kind == DECREMENT;
}

bool is_binary_operator(tag kind) {
    return kind == PLUS || kind == MINUS || kind == MULTIPLY || kind == DIVIDE || kind == MODULO ||
           kind == ASSIGN || kind == EQUAL || kind == NOT_EQUAL;
}

parser init_parser(lexer* l, token* tokens) {
    parser p = { tokens, l->tokens_count, 0 };

    p.global_symbol_table = create_symbol_table();

    return p;
}


ast_node* parse_expression(parser* p) {
    if (is_binary_operator(get_next_token(p).kind)) {
        return (ast_node*)parse_binary_expr(p);
    } else {
        return parse_literal(p);
    }
}

ast_binary_expr_node* parse_binary_expr(parser* p) {
    ast_node* left = parse_literal(p);

    tag operator_kind = get_current_token(p).kind;
    if (!is_binary_operator(operator_kind)) {
        fprintf(stderr, "Error: Expected binary operator, got %s\n", get_current_token(p).lexme);
        exit(1);
    }
    consume(p, operator_kind);
    ast_node* right = parse_literal(p);

    operator_type op;
    switch (operator_kind) {
        case PLUS:
            op = OP_ADD;
            break;
        case MINUS:
            op = OP_SUBTRACT;
            break;
        case MULTIPLY:
            op = OP_MULTIPLY;
            break;
        default:
            fprintf(stderr, "Error: Unsupported binary operator\n");
            exit(1);
    }

    return create_binary_expr_node(left, right, op);
}

ast_assignment_node* parse_assignment(parser* p) {
    ast_node* identifier_node = parse_identifier(p);
    symbol* variable_symbol = find_symbol(p->global_symbol_table, identifier_node->value);

    if (variable_symbol == NULL) {
        fprintf(stderr, "ERROR: Variable '%s' not found in the current scope.\n", identifier_node->value);
        exit(EXIT_FAILURE);
    }

    if (variable_symbol->is_const){
        fprintf(stderr, "ERROR: Variable '%s' is a constant, cannot be assigned a value.\n", identifier_node->value);
        exit(EXIT_FAILURE);
    }

    if (variable_symbol->type != VARIABLE) {
        fprintf(stderr, "ERROR: '%s' is not a variable; cannot be assigned a value.\n", identifier_node->value);
        exit(EXIT_FAILURE);
    }

    consume(p, ASSIGN);
    ast_node* value = parse_literal(p);
    consume_simicolon(p);

    return create_assignment_node(identifier_node, value);
}

ast_variable_decl_node* parse_variable_declaration(parser* p, scope* s) {
    bool constant = false;
    if (get_current_token(p).kind == KW_CONST){
        consume(p, KW_CONST);
        constant = true;
    }
    builtin_types type_node = parse_type(p);
    ast_node* identifier_node = parse_identifier(p);

    ast_node* value;
    if (get_current_token(p).kind == ASSIGN){
        consume(p, ASSIGN);
        value = parse_literal(p);
    }
    consume_simicolon(p);

    symbol* var = create_symbol(identifier_node->value, VARIABLE, constant);
    add_symbol_to_scope(s, var);

    return create_variable_decl_node(type_node, identifier_node, value, constant);
}


ast_block_node* parse_block(parser* p) {
    ast_block_node* block_node = create_block_node();
    consume(p, LBRACE);
    scope* block_scope = create_scope();
    add_scope_to_table(p->global_symbol_table, block_scope);

    while (get_current_token(p).kind != RBRACE) {
        ast_node* declaration = parse_declaration(p, block_scope);
        add_child((ast_node*)block_node, declaration);
    }

    consume(p, RBRACE);
    return block_node;
}



ast_function_decl_node* parse_function_declaration(parser* p) {
    builtin_types return_type = parse_type(p);

    ast_node* identifier_node = parse_identifier(p);
    char* function_name = identifier_node->value;

    consume(p, LPAREN);
    ast_node** parameters = NULL;
    size_t num_parameters = 0;
    bool constant = false;

    while (get_current_token(p).kind != RPAREN) {
        if (get_current_token(p).kind == KW_CONST){
            consume(p, KW_CONST);
            constant = true;
        }
        builtin_types type = parse_type(p);

        ast_node* id = parse_identifier(p);

        ast_variable_decl_node* param_decl = create_variable_decl_node(type, id, NULL, constant);

        parameters = (ast_node**)realloc(parameters, (num_parameters + 1) * sizeof(ast_node*));
        parameters[num_parameters++] = (ast_node*)param_decl;

        // Check for a comma between parameters
        if (get_current_token(p).kind == COMMA) {
            consume(p, COMMA);
        } else {
            break;
        }
    }

    consume(p, RPAREN);

    ast_block_node* body = parse_block(p);

    ast_function_decl_node* function_decl = create_function_decl_node(return_type, function_name, parameters, num_parameters, body);

    symbol* function_symbol = create_symbol(function_name, FUNCTION, false);
    add_symbol_to_scope(p->global_symbol_table->scopes[0], function_symbol);

    return function_decl;
}

ast_node* parse_declaration(parser* p, scope* scope) {
    token current_token = get_current_token(p);

    if (is_type(current_token) || current_token.kind == KW_CONST) {
        if (get_next_next_token(p).kind == LPAREN){
            ast_function_decl_node * function_decl = parse_function_declaration(p);
            return (ast_node*)function_decl;
        }
        ast_variable_decl_node* variable_decl = parse_variable_declaration(p, scope);
        return (ast_node*)variable_decl;
    } else if (current_token.kind == IDENTIFIER) {
        ast_assignment_node* variable_asg = parse_assignment(p);
        return (ast_node*)variable_asg;
    } else if (current_token.kind == KW_RETURN) {
        ast_return_node* return_stmt = parse_return_stmt(p);
        return (ast_node*)return_stmt;
    } else if (current_token.kind == LBRACE) {
        return (ast_node*)parse_block(p);
    } else if (current_token.kind != ENDOF) {
        fprintf(stderr, "Error: Unexpected token in declaration, got %s\n", current_token.lexme);
        exit(1);
    }

    return NULL;
}

builtin_types parse_type(parser* p) {
    token current_token = get_current_token(p);
    ast_node_type type;
    char* type_str;
    switch (current_token.kind) {
        case KW_INT:
            consume(p, KW_INT);
            return INT;
            break;
        case KW_CHAR:
            consume(p, KW_CHAR);
            return CHAR;
            break;
        case KW_VOID:
            consume(p, KW_VOID);
            return VOID;
            break;
        case KW_DOUBLE:
            consume(p, KW_DOUBLE);
            return DOUBLE;
            break;
        default:
            fprintf(stderr, "Error: Expected type, got %s\n", current_token.lexme);
            exit(1);
    }
    return VOID;

}

ast_node* parse_identifier(parser* p) {
    token current_token = get_current_token(p);
    if (current_token.kind != IDENTIFIER) {
        fprintf(stderr, "Error: Expected identifier, got %s\n", current_token.lexme);
        exit(1);
    }

    char* identifier_value = current_token.lexme;
    consume(p, IDENTIFIER);
    return create_ast_node(AST_IDENTIFIER, identifier_value, NULL);
}

ast_node* parse_literal(parser* p) {
    token current_token = get_current_token(p);
    if (current_token.kind == NUMBER || current_token.kind == CHARACTER) {
        char* literal_value = _strdup(current_token.lexme);
        consume(p, current_token.kind);
        return create_ast_node(AST_LITERAL, literal_value, NULL);
    }else if (current_token.kind == IDENTIFIER) {
        ast_node* id = parse_identifier(p);
        return create_ast_node(AST_IDENTIFIER, id->value, NULL);
    } else {
        fprintf(stderr, "Error: Expected literal, got %s\n", current_token.lexme);
        exit(1);
    }
}

ast_return_node* parse_return_stmt(parser* p) {
    consume(p, KW_RETURN);

    ast_node* expr = parse_expression(p);

    consume_simicolon(p);

    return create_return_node(expr);
}

ast_program_node* parse_program(parser* p) {
    ast_program_node* program_node = create_program_node();
    while (get_current_token(p).kind != ENDOF) {
        ast_node* declaration = parse_declaration(p, p->global_symbol_table->scopes[0]);
        add_child((ast_node*)program_node, declaration);
    }

    return program_node;
}


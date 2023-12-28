#include "lexer.h"

lexer init_lexer(char* file_name) {
    FILE* fp;
    char* buffer;
    long file_size;

    if (fopen_s(&fp, file_name, "rb") != 0) {
        printf("Error: File %s not found!\n", file_name);
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buffer = (char*)malloc((file_size + 1) * sizeof(char));
    if (buffer == NULL) {
        printf("Error: Failed to allocate memory for file %s!\n", file_name);
        exit(1);
    }

    fread(buffer, sizeof(char), file_size, fp);
    buffer[file_size] = '\0';

    fclose(fp);

    lexer l = {
        file_name,
        0,
        buffer,
        1,
        1,
        0,
    };
    return l;
}

int is_digit(char c) {
    return isdigit(c);
}

int is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}


int is_alnum(char c) {
    return isalnum(c);
}


void resize_tokens(token** tokens, int* max_tokens) {
    *max_tokens *= 2;
    *tokens = realloc(*tokens, *max_tokens * sizeof(token));

    if (*tokens == NULL) {
        printf("Error: Failed to reallocate memory for tokens!\n");
        exit(1);
    }
}

void push_token(token** tokens, int* num_tokens, int* max_tokens, const char* lexme, tag kind) {
    if (*num_tokens == *max_tokens) {
        *max_tokens *= 2;
        *tokens = realloc(*tokens, *max_tokens * sizeof(token));

        if (*tokens == NULL) {
            printf("Error: Failed to reallocate memory for tokens!\n");
            exit(1);
        }
    }

    char* lexme_copy = NULL;
    if (lexme != NULL) {
        lexme_copy = _strdup(lexme);
        if (lexme_copy == NULL) {
            printf("Error: Failed to allocate memory for lexme!\n");
            exit(1);
        }
    }

    (*tokens)[*num_tokens].lexme = lexme_copy;
    (*tokens)[*num_tokens].kind = kind;
    *num_tokens += 1;
}

tag get_keyword(const char* value) {
    if (strcmp(value, "int") == 0) {
        return KW_INT;
    } else if (strcmp(value, "char") == 0) {
        return KW_CHAR;
    } else if (strcmp(value, "float") == 0) {
        return KW_FLOAT;
    } else if (strcmp(value, "double") == 0) {
        return KW_DOUBLE;
    } else if (strcmp(value, "void") == 0) {
        return KW_VOID;
    } else if (strcmp(value, "short") == 0) {
        return KW_SHORT;
    } else if (strcmp(value, "long") == 0) {
        return KW_LONG;
    } else if (strcmp(value, "signed") == 0) {
        return KW_SIGNED;
    } else if (strcmp(value, "unsigned") == 0) {
        return KW_UNSIGNED;
    } else if (strcmp(value, "if") == 0) {
        return KW_IF;
    } else if (strcmp(value, "else") == 0) {
        return KW_ELSE;
    } else if (strcmp(value, "switch") == 0) {
        return KW_SWITCH;
    } else if (strcmp(value, "case") == 0) {
        return KW_CASE;
    } else if (strcmp(value, "default") == 0) {
        return KW_DEFAULT;
    } else if (strcmp(value, "while") == 0) {
        return KW_WHILE;
    } else if (strcmp(value, "do") == 0) {
        return KW_DO;
    } else if (strcmp(value, "for") == 0) {
        return KW_FOR;
    } else if (strcmp(value, "continue") == 0) {
        return KW_CONTINUE;
    } else if (strcmp(value, "break") == 0) {
        return KW_BREAK;
    } else if (strcmp(value, "return") == 0) {
        return KW_RETURN;
    } else if (strcmp(value, "const") == 0) {
        return KW_CONST;
    } else {
        return IDENTIFIER;
    }
}


tag get_directive(const char* value) {
    if (strcmp(value, "include") == 0) {
        return PP_INCLUDE;
    } else if (strcmp(value, "define") == 0) {
        return PP_DEFINE;
    } else if (strcmp(value, "ifdef") == 0) {
        return PP_IFDEF;
    } else if (strcmp(value, "ifndef") == 0) {
        return PP_IFNDEF;
    } else if (strcmp(value, "endif") == 0) {
        return PP_ENDIF;
    } else if (strcmp(value, "if") == 0) {
        return PP_IF;
    } else if (strcmp(value, "else") == 0) {
        return PP_ELSE;
    } else if (strcmp(value, "elif") == 0) {
        return PP_ELIF;
    } else if (strcmp(value, "pragma") == 0) {
        return PP_PRAGMA;
    } else if (strcmp(value, "undef") == 0) {
        return PP_UNDEF;
    } else {
        return IDENTIFIER;
    }
}


token* tokenizer(lexer* lex) {
    int max_tokens = 20;
    int num_tokens = 0;
    token* tokens = malloc(max_tokens * sizeof(token));

    if (!tokens) {
        printf("Error: Failed to allocate memory for tokens!\n");
        exit(1);
    }

    int content_length = strlen(lex->content);

    while (lex->index < content_length) {
        char current_char = lex->content[lex->index];
        switch (current_char) {
            case '\n': {
                lex->index += 1;
                lex->current_line += 1;
                break;
            }
            case '(': {
                lex->index += 1;
                lex->current_col += 1;
                push_token(&tokens, &num_tokens, &max_tokens, "(", LPAREN);
                break;
            }
            case ')': {
                lex->index += 1;
                lex->current_col += 1;
                push_token(&tokens, &num_tokens, &max_tokens, ")", RPAREN);
                break;
            }
            case '{': {
                lex->index += 1;
                lex->current_col += 1;
                push_token(&tokens, &num_tokens, &max_tokens, "{", LBRACE);
                break;
            }
            case '}': {
                lex->index += 1;
                lex->current_col += 1;
                push_token(&tokens, &num_tokens, &max_tokens, "}", RBRACE);
                break;
            }
            case ';': {
                lex->index += 1;
                lex->current_col += 1;
                push_token(&tokens, &num_tokens, &max_tokens, ";", SIMICOLON);
                break;
            }
            case ':': {
                lex->index += 1;
                lex->current_col += 1;
                push_token(&tokens, &num_tokens, &max_tokens, ":", COLON);
                break;
            }
            case ',': {
                lex->index += 1;
                lex->current_col += 1;
                push_token(&tokens, &num_tokens, &max_tokens, ",", COMMA);
                break;
            }
            case '+': {
                lex->index += 1;
                lex->current_col += 1;
                if (lex->content[lex->index] == '=') {
                    lex->index += 1;
                    lex->current_col += 1;
                    push_token(&tokens, &num_tokens, &max_tokens, "+=", PLUS_ASSIGN);
                } else {
                    push_token(&tokens, &num_tokens, &max_tokens, "+", PLUS);
                }
                break;
            }
            case '-': {
                lex->index += 1;
                lex->current_col += 1;
                push_token(&tokens, &num_tokens, &max_tokens, "-", MINUS);
                break;
            }
            case '*': {
                lex->index += 1;
                lex->current_col += 1;
                push_token(&tokens, &num_tokens, &max_tokens, "*", MULTIPLY);
                break;
            }
            case '.': {
                lex->index += 1;
                lex->current_col += 1;
                push_token(&tokens, &num_tokens, &max_tokens, ".", DOT);
                break;
            }
            case '=': {
                lex->index += 1;
                lex->current_col += 1;
                if (lex->content[lex->index] == '=') {
                    lex->index += 1;
                    lex->current_col += 1;
                    push_token(&tokens, &num_tokens, &max_tokens, "==", EQUAL);
                } else {
                    push_token(&tokens, &num_tokens, &max_tokens, "=", ASSIGN);
                }
                break;
            }
            case '!': {
                lex->index += 1;
                lex->current_col += 1;
                if (lex->content[lex->index] == '=') {
                    lex->index += 1;
                    lex->current_col += 1;
                    push_token(&tokens, &num_tokens, &max_tokens, "!=", NOT_EQUAL);
                } else {
                    lex->index += 1;
                }
                break;
            }
            case '<': {
                lex->index += 1;
                lex->current_col += 1;
                if (lex->content[lex->index] == '=') {
                    lex->index += 1;
                    lex->current_col += 1;
                    push_token(&tokens, &num_tokens, &max_tokens, "<=", LESS_EQUAL);
                } else {
                    push_token(&tokens, &num_tokens, &max_tokens, "<", LESS);
                }
                break;
            }
            case '>': {
                lex->index += 1;
                lex->current_col += 1;
                if (lex->content[lex->index] == '=') {
                    lex->index += 1;
                    lex->current_col += 1;
                    push_token(&tokens, &num_tokens, &max_tokens, ">=", GREATER_EQUAL);
                } else {
                    push_token(&tokens, &num_tokens, &max_tokens, ">", GREATER);
                }
                break;
            }
            case '#': {
                lex->index += 1;
                lex->current_col += 1;

                while (lex->content[lex->index] == ' ' || lex->content[lex->index] == '\t') {
                    lex->index += 1;
                    lex->current_col += 1;
                }

                int start_index = lex->index;
                while (is_alpha(lex->content[lex->index]) || lex->content[lex->index] == '_') {
                    lex->index += 1;
                    lex->current_col += 1;
                }
                int end_index = lex->index;
                char* directive = malloc((end_index - start_index + 1) * sizeof(char));
                strncpy_s(directive, (end_index - start_index + 1), lex->content + start_index, (end_index - start_index));
                directive[end_index - start_index] = '\0';

                tag pp_tag = get_directive(directive);

                push_token(&tokens, &num_tokens, &max_tokens, directive, pp_tag);

                if (pp_tag == PP_INCLUDE) {
                    while (lex->content[lex->index] == ' ' || lex->content[lex->index] == '\t') {
                        lex->index += 1;
                        lex->current_col += 1;
                    }

                    if (lex->content[lex->index] == '"' || lex->content[lex->index] == '<') {
                        lex->index += 1;
                        start_index = lex->index;
                        char end_char = (lex->content[start_index - 1] == '<') ? '>' : '"';
                        while (lex->content[lex->index] != end_char && lex->content[lex->index] != '\n') {
                            lex->index += 1;
                            lex->current_col += 1;
                        }
                        int end_index = lex->index;
                        size_t path_size = end_index - start_index + 1;
                        char* path = malloc(path_size * sizeof(char));

                        if (path != NULL) {
                            if (strncpy_s(path, path_size, lex->content + start_index, end_index - start_index) == 0) {
                                path[path_size - 1] = '\0';

                                push_token(&tokens, &num_tokens, &max_tokens, path, IDENTIFIER);

                                if (lex->content[lex->index] == end_char) {
                                    lex->index += 1;
                                    lex->current_col += 1;
                                }
                            } else {
                                fprintf(stderr, "Error: strncpy_s failed for path\n");
                                free(path);
                            }
                        } else {
                            fprintf(stderr, "Error: Failed to allocate memory for path\n");
                        }
                    }
                }

                free(directive);
                break;
            }
            case '\'': {
                lex->index += 1;
                lex->current_col += 1;
                char* char_value = malloc(2);
                char_value[0] = '\0';
                int index = 0;

                while (lex->content[lex->index] != '\'' && lex->content[lex->index] != '\0' && lex->content[lex->index] != '\n') {
                    current_char = lex->content[lex->index];
                    if (current_char == '\\') {
                        switch (lex->content[lex->index]) {
                            case 'n':
                                current_char = '\n';
                                break;
                            case 'r':
                                current_char = '\r';
                                break;
                            case 't':
                                current_char = '\t';
                                break;
                            case '0':
                                current_char = '\0';
                                break;
                            case 'f':
                                current_char = '\f';
                                break;
                            case 'a':
                                current_char = '\a';
                                break;
                            default:
                                break;
                        }
                    }

                    char_value = realloc(char_value, index + 2);
                    char_value[index] = current_char;
                    index += 1;
                    lex->index += 1;
                }

                lex->index += 1;
                char_value[index] = '\0';

                push_token(&tokens, &num_tokens, &max_tokens, char_value, CHARACTER);
                free(char_value);
                break;
            }
            case '\"': {
                lex->current_col += 1;
                lex->index += 1;
                char* string_value = malloc(1);
                string_value[0] = '\0';
                int index = 0;

                while (lex->content[lex->index] != '\"') {
                    current_char = lex->content[lex->index];
                    string_value = realloc(string_value, index + 2);
                    string_value[index] = current_char;
                    index += 1;
                    lex->index += 1;
                }
                lex->index += 1;
                string_value[index] = '\0';

                push_token(&tokens, &num_tokens, &max_tokens, string_value, STRING);
                free(string_value);
                break;
            }
            default: {
                if (is_digit(current_char)) {
                    int start_index = lex->index;
                    while (is_digit(current_char) || current_char == '.') {
                        lex->index += 1;
                        current_char = lex->content[lex->index];
                    }
                    int end_index = lex->index;
                    size_t value_size = end_index - start_index + 1;
                    char* value = malloc(value_size * sizeof(char));

                    if (value != NULL) {
                        if (strncpy_s(value, value_size, lex->content + start_index, end_index - start_index) == 0) {
                            value[value_size - 1] = '\0';

                            push_token(&tokens, &num_tokens, &max_tokens, value, NUMBER);
                        } else {
                            free(value);
                        }
                    } else {
                        fprintf(stderr, "ERROR: memory in toknizer (numbers)\n");
                    }
                } else if (is_alpha(current_char)) {
                    int start_index = lex->index;
                    while (is_alnum(current_char) || current_char == '_') {
                        lex->index += 1;
                        current_char = lex->content[lex->index];
                    }
                    int end_index = lex->index;
                    size_t value_size = end_index - start_index + 1;
                    char* value = malloc(value_size * sizeof(char));

                    if (value != NULL) {
                        if (strncpy_s(value, value_size, lex->content + start_index, end_index - start_index) == 0) {
                            value[value_size - 1] = '\0';

                            tag keyword_kind = get_keyword(value);

                            push_token(&tokens, &num_tokens, &max_tokens, value, keyword_kind);
                        } else {
                            fprintf(stderr, "Error: strncpy_s failed for path\n");
                            free(value);
                        }
                    } else {
                        fprintf(stderr, "ERROR: memory in toknizer (identifiers)\n");
                    }
                } else {
                    lex->index += 1;
                }
                break;
            }
        }
    }

    if (num_tokens == max_tokens) {
        resize_tokens(&tokens, &max_tokens);
    }

    push_token(&tokens, &num_tokens, &max_tokens, NULL, ENDOF);
    lex->tokens_count = num_tokens;

    return tokens;
}


char* token_to_string(token* token) {
    switch (token->kind) {
        case IDENTIFIER: return "identifier";
        case NUMBER: return "number";
        case STRING: return "string";
        case CHARACTER: return "character";

        case RPAREN: return "right_paren";
        case LPAREN: return "left_paren";
        case RBRACE: return "right_brace";
        case LBRACE: return "left_brace";
        case SIMICOLON: return "semicolon";
        case COLON: return "colon";
        case DOT: return "dot";
        case COMMA: return "comma";

        // Operators
        case EQUAL: return "equal";
        case NOT_EQUAL: return "not_equal";
        case LESS: return "less";
        case LESS_EQUAL: return "less_equal";
        case GREATER: return "greater";
        case GREATER_EQUAL: return "greater_equal";
        case PLUS: return "plus";
        case MINUS: return "minus";
        case MULTIPLY: return "multiply";
        case DIVIDE: return "divide";
        case ASSIGN: return "assign";

        // keywords
        case KW_INT: return "keyword_int";
        case KW_CHAR: return "keyword_char";
        case KW_FLOAT: return "keyword_float";
        case KW_DOUBLE: return "keyword_double";
        case KW_VOID: return "keyword_void";
        case KW_SHORT: return "keyword_short";
        case KW_LONG: return "keyword_long";
        case KW_SIGNED: return "keyword_signed";
        case KW_UNSIGNED: return "keyword_unsigned";
        case KW_IF: return "keyword_if";
        case KW_ELSE: return "keyword_else";
        case KW_SWITCH: return "keyword_switch";
        case KW_CASE: return "keyword_case";
        case KW_DEFAULT: return "keyword_default";
        case KW_WHILE: return "keyword_while";
        case KW_DO: return "keyword_do";
        case KW_FOR: return "keyword_for";
        case KW_CONTINUE: return "keyword_continue";
        case KW_BREAK: return "keyword_break";
        case KW_RETURN: return "keyword_return";
        case KW_CONST: return "keyword_const";
        case ENDOF: return "end_of_file";

        // Preprocessor Directives
        case PP_INCLUDE: return "preprocessor_include";
        case PP_DEFINE: return "preprocessor_define";
        case PP_UNDEF: return "preprocessor_undef";
        case PP_IF: return "preprocessor_if";
        case PP_ELIF: return "preprocessor_elif";
        case PP_ELSE: return "preprocessor_else";
        case PP_ENDIF: return "preprocessor_endif";
        case PP_IFDEF: return "preprocessor_ifdef";
        case PP_IFNDEF: return "preprocessor_ifndef";
        case PP_ERROR: return "preprocessor_error";
        case PP_PRAGMA: return "preprocessor_pragma";
        default: return "UNDEF";
    }
}

const char* tag_tostring(tag t) {
    switch (t) {
        case IDENTIFIER: return "identifier";
        case NUMBER: return "number";
        case STRING: return "string";
        case CHARACTER: return "character";

        case RPAREN: return "right_paren";
        case LPAREN: return "left_paren";
        case RBRACE: return "right_brace";
        case LBRACE: return "left_brace";
        case SIMICOLON: return "semicolon";
        case COLON: return "colon";
        case DOT: return "dot";
        case COMMA: return "comma";

        // Operators
        case EQUAL: return "equal";
        case NOT_EQUAL: return "not_equal";
        case LESS: return "less";
        case LESS_EQUAL: return "less_equal";
        case GREATER: return "greater";
        case GREATER_EQUAL: return "greater_equal";
        case PLUS: return "plus";
        case MINUS: return "minus";
        case MULTIPLY: return "multiply";
        case DIVIDE: return "divide";
        case ASSIGN: return "assign";

        // keywords
        case KW_INT: return "keyword_int";
        case KW_CHAR: return "keyword_char";
        case KW_FLOAT: return "keyword_float";
        case KW_DOUBLE: return "keyword_double";
        case KW_VOID: return "keyword_void";
        case KW_SHORT: return "keyword_short";
        case KW_LONG: return "keyword_long";
        case KW_SIGNED: return "keyword_signed";
        case KW_UNSIGNED: return "keyword_unsigned";
        case KW_IF: return "keyword_if";
        case KW_ELSE: return "keyword_else";
        case KW_SWITCH: return "keyword_switch";
        case KW_CASE: return "keyword_case";
        case KW_DEFAULT: return "keyword_default";
        case KW_WHILE: return "keyword_while";
        case KW_DO: return "keyword_do";
        case KW_FOR: return "keyword_for";
        case KW_CONTINUE: return "keyword_continue";
        case KW_BREAK: return "keyword_break";
        case KW_RETURN: return "keyword_return";
        case KW_CONST: return "keyword_const";
        case ENDOF: return "end_of_file";

        // Preprocessor Directives
        case PP_INCLUDE: return "preprocessor_include";
        case PP_DEFINE: return "preprocessor_define";
        case PP_UNDEF: return "preprocessor_undef";
        case PP_IF: return "preprocessor_if";
        case PP_ELIF: return "preprocessor_elif";
        case PP_ELSE: return "preprocessor_else";
        case PP_ENDIF: return "preprocessor_endif";
        case PP_IFDEF: return "preprocessor_ifdef";
        case PP_IFNDEF: return "preprocessor_ifndef";
        case PP_ERROR: return "preprocessor_error";
        case PP_PRAGMA: return "preprocessor_pragma";
        default: return "UNDEF";
    }
}


void print_tokens(token* tokens, int num_tokens) {
    for (int i = 0; i < num_tokens; i++) {
        printf("Token: '%s' Kind: %s\n", tokens[i].lexme, token_to_string(&tokens[i]));
        free(tokens[i].lexme);
    }
}

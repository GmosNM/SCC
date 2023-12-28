#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


symbol* create_symbol(const char* name, symbol_type type, bool is_const) {
    symbol* sym = malloc(sizeof(symbol));
    sym->name = _strdup(name);
    sym->type = type;
    sym->is_const = is_const;
    return sym;
}

scope* create_scope() {
    scope* s = malloc(sizeof(scope));
    s->symbols = NULL;
    s->num_symbols = 0;
    return s;
}

symbol_table* create_symbol_table() {
    symbol_table* st = malloc(sizeof(symbol_table));
    st->scopes = NULL;
    st->num_scopes = 0;

    scope* scope = create_scope();
    add_scope_to_table(st, scope);

    return st;
}

void add_symbol_to_scope(scope* s, symbol* sym) {
    s->symbols = realloc(s->symbols, (s->num_symbols + 1) * sizeof(symbol*));
    s->symbols[s->num_symbols++] = sym;
}

void add_scope_to_table(symbol_table* st, scope* s) {
    st->scopes = realloc(st->scopes, (st->num_scopes + 1) * sizeof(scope*));
    st->scopes[st->num_scopes++] = s;
}

symbol* find_symbol(symbol_table* st, const char* name) {
    for (size_t i = st->num_scopes; i > 0; --i) {
        scope* current_scope = st->scopes[i - 1];
        for (size_t j = 0; j < current_scope->num_symbols; ++j) {
            if (strcmp(current_scope->symbols[j]->name, name) == 0) {
                return current_scope->symbols[j];
            }
        }
    }
    return NULL;
}

symbol* find_global_symbol(symbol_table* st, const char* name) {
    scope* global_scope = st->scopes[0];
    for (size_t i = 0; i < global_scope->num_symbols; ++i) {
        if (strcmp(global_scope->symbols[i]->name, name) == 0) {
            return global_scope->symbols[i];
        }
    }
    return NULL;
}

void print_symbol_table(symbol_table* st) {
    printf("---------------------------------\n");
    for (size_t i = 0; i < st->num_scopes; ++i) {
        scope* current_scope = st->scopes[i];
        printf("Scope %zu:\n", i + 1);

        for (size_t j = 0; j < current_scope->num_symbols; ++j) {
            symbol* sym = current_scope->symbols[j];
            printf("  Name: %s, Type: ", sym->name);
            switch (sym->type) {
                case VARIABLE:
                    printf("Variable");
                    break;
                case FUNCTION:
                    printf("Function");
                    break;
                case TYPE:
                    printf("Type");
                    break;
                default:
                    printf("Unknown");
                    break;
            }

            printf("\n");
        }

        printf("---------------------------------\n");
    }
}

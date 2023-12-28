#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>

typedef enum {
    VARIABLE,
    FUNCTION,
    TYPE,
} symbol_type;

typedef struct {
    const char* name;
    symbol_type type;
    bool is_const;
} symbol;

typedef struct {
    symbol** symbols;
    size_t num_symbols;
} scope;

typedef struct {
    scope** scopes;
    size_t num_scopes;
    scope* current_scope;
} symbol_table;

symbol* create_symbol(const char* name, symbol_type type, bool is_const);
scope* create_scope();
symbol_table* create_symbol_table();
void add_symbol_to_scope(scope* s, symbol* sym);
void add_scope_to_table(symbol_table* st, scope* s);
symbol* find_symbol(symbol_table* st, const char* name);
symbol* find_global_symbol(symbol_table* st, const char* name);
void print_symbol_table(symbol_table* st);

#endif // SYMBOL_TABLE_H


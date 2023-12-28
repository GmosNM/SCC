#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum tag {
    IDENTIFIER,
    STRING,
    CHARACTER,
    NUMBER,

    //  Operators
    PLUS,               // +
    MINUS,              // -
    MULTIPLY,           // *
    DIVIDE,             // /
    MODULO,             // %
    ASSIGN,             // =
    EQUAL,              // ==
    NOT_EQUAL,          // !=
    LESS,               // <
    LESS_EQUAL,         // <=
    GREATER,            // >
    GREATER_EQUAL,      // >=
    LOGICAL_AND,        // &&
    LOGICAL_OR,         // ||
    LOGICAL_NOT,        // !
    BITWISE_AND,        // &
    BITWISE_OR,         // |
    BITWISE_XOR,        // ^
    LEFT_SHIFT,         // <<
    RIGHT_SHIFT,        // >>
    INCREMENT,          // ++
    DECREMENT,          // --
    PLUS_ASSIGN,        // +=
    MINUS_ASSIGN,       // -=
    MULTIPLY_ASSIGN,    // *=
    DIVIDE_ASSIGN,      // /=
    MODULO_ASSIGN,      // %=
    AND_ASSIGN,         // &=
    OR_ASSIGN,          // |=
    XOR_ASSIGN,         // ^=
    LEFT_SHIFT_ASSIGN,  // <<=
    RIGHT_SHIFT_ASSIGN, // >>=

    //  Delimiters
    RPAREN,             // )
    LPAREN,             // (
    RBRACE,             // }
    LBRACE,             // {
    SIMICOLON,          // ;
    COLON,              // :
    DOT,                // .
    COMMA,              // ,

    //  Keywords
    KW_INT,
    KW_CHAR,
    KW_FLOAT,
    KW_DOUBLE,
    KW_VOID,
    KW_SHORT,
    KW_LONG,
    KW_SIGNED,
    KW_UNSIGNED,
    KW_IF,
    KW_ELSE,
    KW_SWITCH,
    KW_CASE,
    KW_DEFAULT,
    KW_WHILE,
    KW_DO,
    KW_FOR,
    KW_CONTINUE,
    KW_BREAK,
    KW_RETURN,
    KW_CONST,

    // Preprocessor Directives
    PP_INCLUDE,        // #include
    PP_DEFINE,         // #define
    PP_UNDEF,          // #undef
    PP_IF,             // #if
    PP_ELIF,           // #elif
    PP_ELSE,           // #else
    PP_ENDIF,          // #endif
    PP_IFDEF,          // #ifdef
    PP_IFNDEF,         // #ifndef
    PP_ERROR,          // #error
    PP_PRAGMA,         // #pragma

    ENDOF,
} tag;


typedef struct token {
    char* lexme;
    tag kind;
} token;

typedef struct lexer {
    char* file_name;
    int index;
    char* content;
    int current_line;
    int current_col;
    int tokens_count;
} lexer;


lexer init_lexer(char* file_name);

void resize_tokens(token** tokens, int* max_tokens);
void push_token(token** tokens, int* num_tokens, int* max_tokens, const char* lexme, tag kind);
void print_tokens(token* tokens, int num_tokens);

tag get_keyword(const char* value);
tag get_directive(const char* value);

token* tokenizer(lexer* lex);
char* token_to_string(token* token);


int is_digit(char c);
int is_alpha(char c);
int is_alnum(char c);


const char* tag_tostring(tag t);

#include "parser.h"


int main(int argc, char** argv) {
    if (argc < 2) {
        printf("ERROR: no input file\n");
        exit(1);
    }
    lexer l = init_lexer(argv[1]);
    token* tokens = tokenizer(&l);
    parser p = init_parser(&l, tokens);

    ast_program_node* program = parse_program(&p);
    print_ast(program);
    print_symbol_table(p.global_symbol_table);

    // TOOD: free all the ast nodes

    free(l.content);
    free(tokens);

    return 0;
}



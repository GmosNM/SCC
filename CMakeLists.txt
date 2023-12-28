cmake_minimum_required(VERSION 3.26)
project(scc C)


set(SRC
    src/main.c
    src/lexer.h
    src/lexer.c
    src/parser.h
    src/parser.c
    src/ast.h
    src/ast.c
    src/symbol_table.h
    src/symbol_table.c
    src/sir.c
    src/sir.h
)



add_executable(scc ${SRC})

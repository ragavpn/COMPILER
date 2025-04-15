#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <input_file>"
    exit 1
fi

input_file=$1

# Delete any existing generated files
rm -f parser.tab.c parser.tab.h lex.yy.c parser

# Generate the parser
bison -d parser.y

# Generate the lexer
flex lexer.l

# Compile everything
gcc lex.yy.c parser.tab.c -o parser

# Run the parser with the specified input file
./parser < "$input_file"

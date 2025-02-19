#!/bin/bash

# Generate the lexer
flex lexer.l
gcc lex.yy.c -o lexer

# Run lexer on each .txt file
for file in *.txt; do
    if [ -f "$file" ]; then
        echo -e "\n========================================="
        echo "Analyzing file: $file"
        echo -e "========================================="
        ./lexer "$file"
    fi
done

# Clean up
rm lexer lex.yy.c 
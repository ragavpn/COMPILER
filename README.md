# Compiler Construction Lab Implementations

This repository contains the implementation of a compiler for a custom programming language developed across three lab assignments. Each lab builds upon the previous one, progressively constructing a complete compiler front-end with lexical analysis, syntax parsing, and semantic analysis capabilities.

## Table of Contents

- [Overview](#overview)
- [LAB1: Lexical Analysis](#lab1-lexical-analysis)
- [LAB2: Syntax Analysis](#lab2-syntax-analysis)
- [LAB3: Semantic Analysis](#lab3-semantic-analysis)
- [Design Decisions](#design-decisions)
- [Running the Code](#running-the-code)
- [Implementation Details](#implementation-details)
- [Attribute Grammar](#attribute-grammar)
- [Challenges and Solutions](#challenges-and-solutions)

## Overview

This project implements a compiler front-end in three stages:

1. **Lexical Analysis (LAB1)**: Tokenization of the input source code using Flex
2. **Syntax Analysis (LAB2)**: Parsing and syntax tree construction using Bison
3. **Semantic Analysis (LAB3)**: Type checking, scope handling, and function overloading

The compiler supports a C-like language with custom features including variable declarations, function declarations with overloading, expressions, control structures, and arrays.

## LAB1: Lexical Analysis

The lexical analyzer implemented in LAB1 recognizes the following token types:

- **Keywords**: `int_RP`, `float_RP`, `return_RP`, `if_RP`, `else_RP`, `while_RP`
- **Identifiers**: Names with pattern `095*` (variables, functions)
- **Operators**: 
  - Arithmetic: `+`, `-`, `*`, `/`, `%`
  - Relational: `<`, `>`, `<=`, `>=`, `==`, `!=`
  - Logical: `&&`, `||`, `!`
  - Assignment: `=`
- **Delimiters**: `{`, `}`, `[`, `]`, `(`, `)`, `;`, `,`
- **Constants**: Integer and floating-point numbers
- **Comments**: Line comments starting with `//`

### Regular Expressions

The lexical analyzer uses the following regular expressions to recognize tokens:

```
DIGIT       [0-9]
LETTER      [a-zA-Z]
IDENTIFIER  095[a-zA-Z0-9_]*
INTEGER     {DIGIT}+
FLOAT       {DIGIT}+\.{DIGIT}+
COMMENT     "//".*
WHITESPACE  [ \t\n\r]+
```

### Token Classification

Each token is classified and output in a tabular format showing:
- Token type (keyword, identifier, operator, etc.)
- Token value (lexeme)
- Line and column number (for error reporting)

## LAB2: Syntax Analysis

The parser in LAB2 builds upon the lexical analyzer to create a syntax tree according to a context-free grammar for the custom language.

### Complete Grammar

```
program → block
block → { statement_list }
statement_list → statement_list statement | ε
statement → declaration | assignment | if_statement | while_statement | return_statement
          | expression_statement | block
declaration → type identifier [ = expression ] ;
            | type identifier [ dimension ] ;
            | function_declaration
dimension → [ INTEGER ] | [ INTEGER ] [ INTEGER ]
function_declaration → type identifier ( parameters ) block
parameters → parameter_list | ε
parameter_list → parameter_list , parameter | parameter
parameter → type identifier
type → int_RP | float_RP
assignment → identifier [ indices ] = expression ;
indices → [ expression ] | [ expression ] [ expression ]
if_statement → if_RP ( expression ) statement [ else_RP statement ]
while_statement → while_RP ( expression ) statement
return_statement → return_RP [ expression ] ;
expression_statement → expression ;
expression → expression + term | expression - term | term
term → term * factor | term / factor | term % factor | factor
factor → ( expression ) | identifier [ indices ] | function_call | INTEGER | FLOAT | - factor
function_call → identifier ( arguments )
arguments → argument_list | ε
argument_list → argument_list , expression | expression
```

### Parse Tree Construction

The parser constructs a parse tree for the input program where:
- Internal nodes represent grammar productions
- Leaf nodes represent tokens
- Each node contains type information (synthesized attribute)
- The tree is traversed for semantic analysis

## LAB3: Semantic Analysis

The semantic analyzer validates the program according to the language's semantic rules and builds a decorated parse tree with type and value information.

### Key Features

- **Symbol Table Management**: Tracks variables across different scopes
- **Type Checking**: Ensures type compatibility in expressions and assignments
- **Function Overloading**: Allows multiple function definitions with different parameter types
- **Array Bounds Checking**: Validates array dimensions and access
- **Value Propagation**: Calculates and displays values in the parse tree

### Symbol and Function Tables

The implementation maintains comprehensive symbol and function tables that store:

- Variable/function names
- Types (int, float, array, function)
- Scope information (global vs. local)
- Array dimensions
- Function signatures for overload resolution
- Variable values

### Type System

The language supports the following types:
- `int` - Integer values
- `float` - Floating-point values
- `array` - One or two-dimensional arrays of int or float
- `function` - Function declarations with return types

### Type Conversion Rules

1. Implicit conversion from `int` to `float` is allowed
2. No implicit conversion from `float` to `int`
3. Array elements must match the array's declared type
4. Function arguments must match parameter types (or be implicitly convertible)

## Design Decisions

### Identifier Naming Convention

All identifiers must begin with the prefix "095" to satisfy the language requirements. This distinctive pattern helps in easily identifying user-defined names.

### Scope Management

The compiler implements static scoping with the following rules:
- Variables are visible within their block and nested blocks
- Inner blocks can shadow variables from outer blocks
- Function parameters are treated as local variables in the function body
- Functions have global scope (can be called from anywhere)

### Function Overloading

The language supports function overloading based on:
- Number of parameters
- Parameter types

Functions with the same name but different parameter lists are considered distinct. The compiler uses a signature-based approach to resolve function calls to the appropriate overloaded version.

## Running the Code

Each lab includes a run script that compiles and executes the code:

```bash
# For LAB1 (Lexical Analysis)
cd LAB1
./run.sh input.txt

# For LAB2 (Syntax Analysis)
cd LAB2
./run.sh input.txt

# For LAB3 (Semantic Analysis)
cd LAB3
./run.sh input.txt
```

## Implementation Details

### Lexical Analyzer (Flex)

The lexical analyzer is implemented using Flex, which generates a C function `yylex()` that returns token types one at a time. The analyzer performs:
- Skipping of whitespace and comments
- Recognition of tokens based on regular expressions
- Error handling for invalid tokens

### Parser (Bison)

The parser uses Bison, which generates a LALR(1) parser in C. The parser:
- Defines all grammar productions
- Associates semantic actions with productions
- Builds the abstract syntax tree
- Handles syntactic error recovery

### Semantic Analysis

The semantic analyzer performs:

- **Type Inference**: Determines expression types based on operands
- **Type Coercion**: Handles implicit conversions (int to float)
- **Scope Management**: Variables are bound to their proper scope
- **Function Resolution**: Resolves function calls to the correct overload
- **Value Calculation**: For constant expressions and variables

## Attribute Grammar

The parser implements an **L-attributed grammar** with both synthesized and inherited attributes:

### Synthesized Attributes
- **Node values**: Computed from children and passed upward (e.g., expression evaluation)
- **Type information**: Determined from operands and propagated upward

### Inherited Attributes
- **Scope information**: Passed from parent nodes to children
- **Function context**: Parameter information inherited from function declarations

### Semantic Rules

1. **Type Checking and Coercion**:
   ```
   Rule: expression → expression + expression
   Action: {
       if (either_is_float(e1, e2))
           e.type = "float";
           e.value = float_value(e1) + float_value(e2);
       else
           e.type = "int";
           e.value = e1.value + e2.value;
   }
   ```

2. **Scope Management**:
   ```
   Rule: block → { statements }
   Action: {
       before statements: currentScope++;
       after statements: currentScope--;
       block.node = statements.node;
   }
   ```

3. **Variable Declaration**:
   ```
   Rule: declaration → type identifier = expression;
   Action: {
       check_compatible_types(type, expression.type);
       addSymbol(identifier, type, currentScope, 0);
       declaration.node = new AssignmentNode(identifier, expression);
       update_symbol_value(identifier, expression.value);
   }
   ```

4. **Function Overloading**:
   ```
   Rule: function_declaration → type identifier ( parameter_list ) { statements }
   Action: {
       check_unique_function_signature(identifier, type, parameter_types);
       add_function_to_table(identifier, type, parameters);
       enter_function_scope();
       add_parameters_to_symbol_table();
       process_function_body();
       exit_function_scope();
   }
   ```

## Challenges and Solutions

### Parser Conflicts

The grammar initially produced shift/reduce and reduce/reduce conflicts due to ambiguities in the if-else construct and expression precedence. Solutions:
- Added appropriate precedence declarations for operators
- Refined grammar rules for if-else statements to resolve the dangling else problem
- Used Bison's %prec directive to specify precedence in specific productions

### Function Overloading Resolution

Implementing function overload resolution was challenging due to the need to match argument types with parameter types. The solution involved:
- Creating a function signature system that combines name and parameter types
- Implementing a type compatibility checker that accounts for implicit conversions
- Using a best-match algorithm when multiple overloaded functions might apply

### Value Propagation

Tracking and displaying values for variables and expressions throughout the parse tree required:
- Adding a value field to all nodes in the parse tree
- Implementing an evaluation function for expressions
- Adding logic to determine when values should be displayed in the parse tree

The combination of these components creates a robust compiler front-end that performs comprehensive analysis of the input program, catching errors at the lexical, syntactic, and semantic levels.

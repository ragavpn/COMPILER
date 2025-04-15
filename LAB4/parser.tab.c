/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// DO NOT DECLARE rootNode HERE - it needs to be after the Node struct definition

int yylex();
void yyerror(const char *msg);

typedef struct Node {
    char *name;
    char *type; // Variable type (e.g., "int", "array", "function")
    int value;  // For numeric evaluation
    struct Node *left;
    struct Node *right;
} Node;

// Moved rootNode declaration here - after Node is defined
Node* rootNode = NULL;

typedef struct Symbol {
    char *name;
    char *type; // e.g., "int", "float", "array", "function"
    int scope;
    int array_dimensions; // Number of dimensions for arrays
    int function_id;     // For function overload management
    int value;      // Add this to track the current value
} Symbol;

Symbol symbolTable[100];
int symbolCount = 0;
int currentScope = 0;

// Function parameter storage structure
typedef struct {
    char* name;
    char* type;
} Parameter;

// Function information structure
typedef struct {
    char* name;
    char* returnType;
    Parameter params[10];  // Allow up to 10 parameters
    int paramCount;
    int id;               // Unique identifier for this function
} Function;

Function functionTable[50];  // Store up to 50 functions
int functionCount = 0;
int nextFunctionId = 1;     // For generating unique function IDs

// Add these declarations before the main parser code:
char* paramNames[10];
char* paramTypes[10];
int paramCount = 0;

// Add prototype for findMatchingFunction
int findMatchingFunction(char* name, Node* argList);
int countArguments(Node* argList);
int lookupFunctionOverload(char* name, char* returnType, char** paramTypes, int paramCount);

// Add this prototype near the other function prototypes (around line 60)
int evaluateExpr(const char* op, int leftVal, int rightVal);

// Add this prototype before any use of generateTACForNode
void generateTACForNode(Node *node);

Node *createNode(char *name, char *type, Node *left, Node *right) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->name = strdup(name);
    node->type = strdup(type);
    node->value = 0; // Default value
    node->left = left;
    node->right = right;
    return node;
}

// Update this helper function to be more selective about showing values
int shouldShowValue(Node *node) {
    // Only show values for variables and number literals
    if (node == NULL) return 0;
    
    // Show values for number literals
    if ((strcmp(node->type, "int") == 0 || strcmp(node->type, "float") == 0) && 
        (isdigit(node->name[0]) || node->name[0] == '.')) {
        return 1; // It's a number literal
    }
    
    // Show values for variable identifiers (but not arrays, functions, etc.)
    if ((strcmp(node->type, "int") == 0 || strcmp(node->type, "float") == 0) && 
        strncmp(node->name, "095", 3) == 0) {
        return 1; // It's a variable
    }
    
    // Don't show values for anything else
    return 0;
}

// Modify the printTree function to selectively show values
void printTree(Node *root, int level) {
    if (root == NULL) return;
    
    for (int i = 0; i < level; i++) printf("  ");
    
    if (shouldShowValue(root)) {
        printf("└── %s (%s, %d)\n", root->name, root->type, root->value);
    } else {
        printf("└── %s (%s)\n", root->name, root->type);
    }
    
    printTree(root->left, level + 1);
    printTree(root->right, level + 1);
}

int lookupSymbol(char *name, int scope) {
    for (int i = symbolCount - 1; i >= 0; i--) {
        if (strcmp(symbolTable[i].name, name) == 0 && symbolTable[i].scope <= scope) {
            return i;
        }
    }
    return -1;
}

// Function to add a function to the function table allowing overloading
int addFunction(char* name, char* returnType) {
    // Check if there's enough space in the function table
    if (functionCount >= 50) {
        printf("Semantic Error: Too many functions defined\n");
        return -1;
    }
    
    // We allow multiple functions with the same name (overloading)
    // but need to collect parameter types first
    char* paramTypesCopy[10];
    for (int i = 0; i < paramCount; i++) {
        paramTypesCopy[i] = strdup(paramTypes[i]);
    }
    
    // Check if this exact function signature already exists
    int existingFunc = lookupFunctionOverload(name, returnType, paramTypesCopy, paramCount);
    if (existingFunc >= 0) {
        printf("Semantic Error: Function '%s' with identical signature already exists\n", name);
        
        // Clean up temporary parameter types
        for (int i = 0; i < paramCount; i++) {
            free(paramTypesCopy[i]);
        }
        
        return -1;
    }
    
    // Clean up temporary parameter types
    for (int i = 0; i < paramCount; i++) {
        free(paramTypesCopy[i]);
    }
    
    // This is a new function signature - add it
    functionTable[functionCount].name = strdup(name);
    functionTable[functionCount].returnType = strdup(returnType);
    functionTable[functionCount].paramCount = 0;
    functionTable[functionCount].id = nextFunctionId++;
    return functionCount++;
}

// Look up a function with exact signature
int lookupFunctionOverload(char* name, char* returnType, char** paramTypes, int paramCount) {
    for (int i = 0; i < functionCount; i++) {
        if (strcmp(functionTable[i].name, name) == 0 && 
            strcmp(functionTable[i].returnType, returnType) == 0 &&
            functionTable[i].paramCount == paramCount) {
            
            // Check each parameter type
            int match = 1;
            for (int j = 0; j < paramCount; j++) {
                if (strcmp(functionTable[i].params[j].type, paramTypes[j]) != 0) {
                    match = 0;
                    break;
                }
            }
            
            if (match) {
                return i; // Found matching function
            }
        }
    }
    return -1; // No match found
}

// Modified to handle function overloads
void addSymbol(char *name, char *type, int scope, int array_dimensions) {
    // Special handling for function names - they can be overloaded
    if (strcmp(type, "function") == 0) {
        symbolTable[symbolCount].name = strdup(name);
        symbolTable[symbolCount].type = strdup(type);
        symbolTable[symbolCount].scope = scope;
        symbolTable[symbolCount].array_dimensions = array_dimensions;
        symbolTable[symbolCount].function_id = nextFunctionId - 1; // Associate with latest function
        symbolTable[symbolCount].value = 0; // Initialize value
        symbolCount++;
        return;
    }
    
    // For parameters, add scope+function_id to make them unique
    if (scope > 1) { // Parameters are typically at scope 2+
        // Don't check for duplicates - parameters can have same name in different functions
        symbolTable[symbolCount].name = strdup(name);
        symbolTable[symbolCount].type = strdup(type);
        symbolTable[symbolCount].scope = scope;
        symbolTable[symbolCount].array_dimensions = array_dimensions;
        symbolTable[symbolCount].function_id = nextFunctionId - 1; // Associate with current function
        symbolTable[symbolCount].value = 0; // Initialize value
        symbolCount++;
        return;
    }
    
    // Regular variables - check for duplicates at the same scope
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0 && symbolTable[i].scope == scope) {
            yyerror("Duplicate declaration");
            return;
        }
    }
    
    symbolTable[symbolCount].name = strdup(name);
    symbolTable[symbolCount].type = strdup(type);
    symbolTable[symbolCount].scope = scope;
    symbolTable[symbolCount].array_dimensions = array_dimensions;
    symbolTable[symbolCount].function_id = 0; // Not a function
    symbolTable[symbolCount].value = 0; // Initialize value
    symbolCount++;
}

// Modified to include function_id in the output
void printSymbolTable() {
    printf("\nSymbol Table:\n");
    printf("+----------------+----------------+--------+----------------+--------+\n");
    printf("| Name           | Type           | Scope  | Array Dim      | Func ID|\n");
    printf("+----------------+----------------+--------+----------------+--------+\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("| %-14s | %-14s | %-6d | %-14d | %-6d |\n",
               symbolTable[i].name,
               symbolTable[i].type,
               symbolTable[i].scope,
               symbolTable[i].array_dimensions,
               symbolTable[i].function_id);
    }
    printf("+----------------+----------------+--------+----------------+--------+\n");
}

// Function to get type from keyword by removing _RP suffix
char* getTypeFromKeyword(char* keyword) {
    char* type = strdup(keyword);
    int len = strlen(type);
    if (len > 3) {
        type[len-3] = '\0'; // Remove _RP suffix
    }
    return type;
}

// Function to check type compatibility and return the resulting type
char* checkTypeCompatibility(const char* type1, const char* type2, const char* operation) {
    // If either is error, propagate error
    if (strcmp(type1, "error") == 0 || strcmp(type2, "error") == 0) {
        return strdup("error");
    }
    
    // Check for array types in operations - disallow
    if (strcmp(type1, "array") == 0 || strcmp(type2, "array") == 0) {
        printf("Semantic Error: Cannot perform operation '%s' on array type\n", operation);
        return strdup("error");
    }
    
    // Both int? Result is int
    if (strcmp(type1, "int") == 0 && strcmp(type2, "int") == 0) {
        return strdup("int");
    }
    
    // If either is float, result is float
    if (strcmp(type1, "float") == 0 || strcmp(type2, "float") == 0) {
        return strdup("float");
    }
    
    // Special check for modulo - both operands must be int
    if (strcmp(operation, "%") == 0) {
        if (strcmp(type1, "int") != 0 || strcmp(type2, "int") != 0) {
            printf("Semantic Error: Modulo operation requires integer operands, got '%s' and '%s'\n",
                   type1, type2);
            return strdup("error");
        }
        return strdup("int");
    }
    
    // Default case - incompatible types
    printf("Semantic Error: Incompatible types '%s' and '%s' for operation '%s'\n", 
           type1, type2, operation);
    return strdup("error");
}

// Function to check assignment compatibility
int isAssignmentCompatible(const char* targetType, const char* valueType) {
    // Cannot assign to array
    if (strcmp(targetType, "array") == 0) {
        printf("Semantic Error: Cannot assign to array type\n");
        return 0;
    }
    
    // Same types are always compatible
    if (strcmp(targetType, valueType) == 0) {
        return 1;
    }
    
    // Int can be assigned to float (implicit conversion)
    if (strcmp(targetType, "float") == 0 && strcmp(valueType, "int") == 0) {
        return 1;
    }
    
    // All other combinations are incompatible
    printf("Semantic Error: Cannot assign '%s' to '%s'\n", valueType, targetType);
    return 0;
}

// Function to add a parameter to a function
void addParameter(int funcIndex, char* name, char* type) {
    int paramIdx = functionTable[funcIndex].paramCount;
    if (paramIdx < 10) {
        functionTable[funcIndex].params[paramIdx].name = strdup(name);
        functionTable[funcIndex].params[paramIdx].type = strdup(type);
        functionTable[funcIndex].paramCount++;
    } else {
        printf("Semantic Error: Too many parameters for function '%s'\n", 
               functionTable[funcIndex].name);
    }
}

// Function to lookup a function by name
int lookupFunction(char* name) {
    for (int i = 0; i < functionCount; i++) {
        if (strcmp(functionTable[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// Function to print the function table - simpler approach with proper border
void printFunctionTable() {
    printf("\nFunction Table (with Overloading):\n");
    printf("+------+----------------+----------------+-------------------------+\n");
    printf("| ID   | Name           | Return Type    | Parameters              |\n");
    printf("+------+----------------+----------------+-------------------------+\n");
    
    for (int i = 0; i < functionCount; i++) {
        printf("| %-4d | %-14s | %-14s | ", 
               functionTable[i].id,
               functionTable[i].name, 
               functionTable[i].returnType);
        
        // Print parameters
        if (functionTable[i].paramCount == 0) {
            printf("%-23s|\n", "");
        } else {
            for (int j = 0; j < functionTable[i].paramCount; j++) {
                if (j > 0) printf(", ");
                printf("%s %s", 
                       functionTable[i].params[j].type,
                       functionTable[i].params[j].name);
            }
            
            // Calculate padding space to align the right border
            int len = 0;
            for (int j = 0; j < functionTable[i].paramCount; j++) {
                len += strlen(functionTable[i].params[j].type) + 
                       strlen(functionTable[i].params[j].name) + 1; // +1 for space
                if (j > 0) len += 2; // for ", "
            }
            
            int padding = 23 - len;
            if (padding < 0) padding = 0;
            
            printf("%*s|\n", padding, "");
        }
    }
    
    printf("+------+----------------+----------------+-------------------------+\n");
}

// Add to your printTree function to include function table and DAG
void printAST() {
    // Print parse tree
    printf("Parse Tree:\n");
    printTree(rootNode, 0);
    printf("\n");

    // Print symbol table
    printSymbolTable();
    
    // Print function table
    printFunctionTable();
}

// Three-address code generation
typedef struct TAC {
    char *result;
    char *arg1;
    char *op;
    char *arg2;
} TAC;

TAC tacTable[1000];
int tacCount = 0;
int tempVarCount = 0;

// Function to generate a new temporary variable
char* newTemp() {
    char *temp = (char *)malloc(10);
    sprintf(temp, "t%d", tempVarCount++);
    return temp;
}

// Function to add a TAC instruction
void addTAC(char *result, char *arg1, char *op, char *arg2) {
    tacTable[tacCount].result = result ? strdup(result) : NULL;
    tacTable[tacCount].arg1 = arg1 ? strdup(arg1) : NULL;
    tacTable[tacCount].op = op ? strdup(op) : NULL;
    tacTable[tacCount].arg2 = arg2 ? strdup(arg2) : NULL;
    tacCount++;
}

// Function to print the TAC in proper three-address code format with labels and control flow
void printTAC() {
    printf("\nThree-Address Code:\nL0:\n");
    for (int i = 0; i < tacCount; i++) {
        int isLabel = (tacTable[i].result && tacTable[i].op == NULL && tacTable[i].arg1 == NULL && tacTable[i].arg2 == NULL);
        if (isLabel) {
            printf("%s:\n", tacTable[i].result);
        } else if (tacTable[i].op == NULL) {
            // Assignment (remove double equals, print as "a = b")
            if (tacTable[i].result && tacTable[i].arg1) {
                printf("    %s = %s\n", tacTable[i].result, tacTable[i].arg1);
            }
        } else if (strcmp(tacTable[i].op, "if") == 0) {
            printf("    if %s goto %s\n", tacTable[i].arg1, tacTable[i].arg2);
        } else if (strcmp(tacTable[i].op, "goto") == 0) {
            printf("    goto %s\n", tacTable[i].arg1);
        } else {
            if (tacTable[i].result && tacTable[i].arg1 && tacTable[i].arg2)
                printf("    %s = %s %s %s\n", tacTable[i].result, tacTable[i].arg1, tacTable[i].op, tacTable[i].arg2);
            else if (tacTable[i].result && tacTable[i].arg1)
                printf("    %s = %s %s\n", tacTable[i].result, tacTable[i].op, tacTable[i].arg1);
        }
    }
}

// Function to add a TAC instruction with labels for control flow
void addTACWithLabel(char *label, char *result, char *arg1, char *op, char *arg2) {
    if (label != NULL) {
        tacTable[tacCount].result = strdup(label);
        tacTable[tacCount].arg1 = NULL;
        tacTable[tacCount].op = NULL;
        tacTable[tacCount].arg2 = NULL;
        tacCount++;
    }
    addTAC(result, arg1, op, arg2);
}

// Function to generate a new label
char* newLabel() {
    char *label = (char *)malloc(10);
    sprintf(label, "L%d", tempVarCount++);
    return label;
}

// Modify TAC generation for control flow constructs
void generateSwitchTAC(char *switchVar, Node *cases, Node *defaultCase) {
    char *testLabel = newLabel();
    char *endLabel = newLabel();

    // Generate test conditions for each case
    Node *currentCase = cases;
    while (currentCase != NULL) {
        char *caseLabel = newLabel();
        char *caseValue = currentCase->left->name; // Case value
        addTAC(NULL, switchVar, "==", caseValue);
        addTAC(NULL, NULL, "if", caseLabel);

        // Generate case body
        addTACWithLabel(caseLabel, NULL, NULL, NULL, NULL);
        generateTACForNode(currentCase->right); // Case body
        addTAC(NULL, NULL, "goto", endLabel);

        currentCase = currentCase->right; // Move to next case
    }

    // Generate default case
    if (defaultCase != NULL) {
        char *defaultLabel = newLabel();
        addTACWithLabel(defaultLabel, NULL, NULL, NULL, NULL);
        generateTACForNode(defaultCase);
    }

    // End label
    addTACWithLabel(endLabel, NULL, NULL, NULL, NULL);
}

// Modify TAC generation for while loops
void generateWhileTAC(Node *condition, Node *body) {
    char *startLabel = newLabel();
    char *testLabel = newLabel();
    char *endLabel = newLabel();

    addTACWithLabel(startLabel, NULL, NULL, NULL, NULL);
    generateTACForNode(condition);
    addTAC(NULL, condition->name, "if", testLabel);
    addTAC(NULL, NULL, "goto", endLabel);

    addTACWithLabel(testLabel, NULL, NULL, NULL, NULL);
    generateTACForNode(body);
    addTAC(NULL, NULL, "goto", startLabel);

    addTACWithLabel(endLabel, NULL, NULL, NULL, NULL);
}

// Modify TAC generation for for loops
void generateForTAC(Node *init, Node *condition, Node *increment, Node *body) {
    char *startLabel = newLabel();
    char *testLabel = newLabel();
    char *endLabel = newLabel();

    generateTACForNode(init);
    addTACWithLabel(startLabel, NULL, NULL, NULL, NULL);
    generateTACForNode(condition);
    addTAC(NULL, condition->name, "if", testLabel);
    addTAC(NULL, NULL, "goto", endLabel);

    addTACWithLabel(testLabel, NULL, NULL, NULL, NULL);
    generateTACForNode(body);
    generateTACForNode(increment);
    addTAC(NULL, NULL, "goto", startLabel);

    addTACWithLabel(endLabel, NULL, NULL, NULL, NULL);
}

// Modify TAC generation for if-else constructs
void generateIfElseTAC(Node *condition, Node *ifBody, Node *elseBody) {
    char *ifLabel = newLabel();
    char *elseLabel = newLabel();
    char *endLabel = newLabel();

    generateTACForNode(condition);
    addTAC(NULL, condition->name, "if", ifLabel);
    addTAC(NULL, NULL, "goto", elseLabel);

    addTACWithLabel(ifLabel, NULL, NULL, NULL, NULL);
    generateTACForNode(ifBody);
    addTAC(NULL, NULL, "goto", endLabel);

    addTACWithLabel(elseLabel, NULL, NULL, NULL, NULL);
    if (elseBody != NULL) {
        generateTACForNode(elseBody);
    }

    addTACWithLabel(endLabel, NULL, NULL, NULL, NULL);
}

// Helper to generate TAC for a statement node (minimal, for control flow)
void generateTACForNode(Node *node) {
    if (!node) return;
    // Only generate for assignment and expression nodes for now
    if (strcmp(node->name, "Assignment") == 0) {
        addTAC(node->left->name, node->right->name, "=", NULL);
    } else if (strcmp(node->name, "++") == 0 || strcmp(node->name, "--") == 0) {
        char *temp = newTemp();
        addTAC(temp, node->left->name, strcmp(node->name, "++") == 0 ? "+" : "-", "1");
        addTAC(node->left->name, temp, "=", NULL);
    } else if (strcmp(node->name, "IfElse") == 0) {
        // handled in parser rule
    } else if (strcmp(node->name, "If") == 0) {
        // handled in parser rule
    } else if (strcmp(node->name, "While") == 0) {
        // handled in parser rule
    } else if (strcmp(node->name, "For") == 0) {
        // handled in parser rule
    } else if (strcmp(node->name, "Switch") == 0) {
        // handled in parser rule
    } else {
        // Recursively generate for children
        generateTACForNode(node->left);
        generateTACForNode(node->right);
    }
}


#line 667 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_UMINUS = 3,                     /* UMINUS  */
  YYSYMBOL_IDENTIFIER = 4,                 /* IDENTIFIER  */
  YYSYMBOL_NUMBER = 5,                     /* NUMBER  */
  YYSYMBOL_KEYWORD = 6,                    /* KEYWORD  */
  YYSYMBOL_IF = 7,                         /* IF  */
  YYSYMBOL_ELSE = 8,                       /* ELSE  */
  YYSYMBOL_WHILE = 9,                      /* WHILE  */
  YYSYMBOL_FOR = 10,                       /* FOR  */
  YYSYMBOL_SWITCH = 11,                    /* SWITCH  */
  YYSYMBOL_CASE = 12,                      /* CASE  */
  YYSYMBOL_DEFAULT = 13,                   /* DEFAULT  */
  YYSYMBOL_RETURN = 14,                    /* RETURN  */
  YYSYMBOL_ASSIGN = 15,                    /* ASSIGN  */
  YYSYMBOL_EQ = 16,                        /* EQ  */
  YYSYMBOL_PLUS = 17,                      /* PLUS  */
  YYSYMBOL_MINUS = 18,                     /* MINUS  */
  YYSYMBOL_MULT = 19,                      /* MULT  */
  YYSYMBOL_DIV = 20,                       /* DIV  */
  YYSYMBOL_MOD = 21,                       /* MOD  */
  YYSYMBOL_AND = 22,                       /* AND  */
  YYSYMBOL_OR = 23,                        /* OR  */
  YYSYMBOL_NOT = 24,                       /* NOT  */
  YYSYMBOL_LT = 25,                        /* LT  */
  YYSYMBOL_GT = 26,                        /* GT  */
  YYSYMBOL_LE = 27,                        /* LE  */
  YYSYMBOL_GE = 28,                        /* GE  */
  YYSYMBOL_INCREMENT = 29,                 /* INCREMENT  */
  YYSYMBOL_DECREMENT = 30,                 /* DECREMENT  */
  YYSYMBOL_LPAREN = 31,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 32,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 33,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 34,                    /* RBRACE  */
  YYSYMBOL_SEMICOLON = 35,                 /* SEMICOLON  */
  YYSYMBOL_COMMA = 36,                     /* COMMA  */
  YYSYMBOL_COLON = 37,                     /* COLON  */
  YYSYMBOL_LBRACKET = 38,                  /* LBRACKET  */
  YYSYMBOL_RBRACKET = 39,                  /* RBRACKET  */
  YYSYMBOL_BREAK = 40,                     /* BREAK  */
  YYSYMBOL_FUNCTION = 41,                  /* FUNCTION  */
  YYSYMBOL_YYACCEPT = 42,                  /* $accept  */
  YYSYMBOL_program = 43,                   /* program  */
  YYSYMBOL_block = 44,                     /* block  */
  YYSYMBOL_45_1 = 45,                      /* $@1  */
  YYSYMBOL_46_2 = 46,                      /* $@2  */
  YYSYMBOL_statements = 47,                /* statements  */
  YYSYMBOL_function_decl = 48,             /* function_decl  */
  YYSYMBOL_param_list = 49,                /* param_list  */
  YYSYMBOL_param = 50,                     /* param  */
  YYSYMBOL_statement = 51,                 /* statement  */
  YYSYMBOL_matched_stmt = 52,              /* matched_stmt  */
  YYSYMBOL_unmatched_stmt = 53,            /* unmatched_stmt  */
  YYSYMBOL_for_statement = 54,             /* for_statement  */
  YYSYMBOL_declaration = 55,               /* declaration  */
  YYSYMBOL_condition = 56,                 /* condition  */
  YYSYMBOL_increment_expr = 57,            /* increment_expr  */
  YYSYMBOL_switch_statement = 58,          /* switch_statement  */
  YYSYMBOL_cases = 59,                     /* cases  */
  YYSYMBOL_case_statement = 60,            /* case_statement  */
  YYSYMBOL_case_statements = 61,           /* case_statements  */
  YYSYMBOL_case_statement_item = 62,       /* case_statement_item  */
  YYSYMBOL_break_statement = 63,           /* break_statement  */
  YYSYMBOL_expression = 64,                /* expression  */
  YYSYMBOL_elif_chain = 65,                /* elif_chain  */
  YYSYMBOL_elif_clause = 66,               /* elif_clause  */
  YYSYMBOL_function_call = 67,             /* function_call  */
  YYSYMBOL_arg_list = 68                   /* arg_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   589

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  42
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  27
/* YYNRULES -- Number of rules.  */
#define YYNRULES  79
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  182

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   296


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   622,   622,   632,   632,   633,   633,   637,   638,   639,
     640,   644,   672,   675,   678,   684,   703,   716,   724,   733,
     748,   758,   769,   770,   771,   772,   773,   774,   789,   793,
     818,   838,   842,   873,   883,   893,   900,   927,   936,   940,
     950,   960,   964,  1014,  1015,  1019,  1028,  1040,  1043,  1049,
    1053,  1064,  1070,  1076,  1082,  1088,  1099,  1103,  1107,  1111,
    1115,  1119,  1123,  1127,  1131,  1135,  1145,  1155,  1158,  1168,
    1178,  1192,  1207,  1212,  1213,  1217,  1228,  1260,  1263,  1266
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "UMINUS", "IDENTIFIER",
  "NUMBER", "KEYWORD", "IF", "ELSE", "WHILE", "FOR", "SWITCH", "CASE",
  "DEFAULT", "RETURN", "ASSIGN", "EQ", "PLUS", "MINUS", "MULT", "DIV",
  "MOD", "AND", "OR", "NOT", "LT", "GT", "LE", "GE", "INCREMENT",
  "DECREMENT", "LPAREN", "RPAREN", "LBRACE", "RBRACE", "SEMICOLON",
  "COMMA", "COLON", "LBRACKET", "RBRACKET", "BREAK", "FUNCTION", "$accept",
  "program", "block", "$@1", "$@2", "statements", "function_decl",
  "param_list", "param", "statement", "matched_stmt", "unmatched_stmt",
  "for_statement", "declaration", "condition", "increment_expr",
  "switch_statement", "cases", "case_statement", "case_statements",
  "case_statement_item", "break_statement", "expression", "elif_chain",
  "elif_clause", "function_call", "arg_list", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-137)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-41)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      -7,   -18,    64,  -137,   136,    34,  -137,   298,  -137,     2,
     184,   184,   184,  -137,    88,  -137,  -137,  -137,  -137,  -137,
    -137,   321,    39,  -137,   184,  -137,  -137,   184,   184,    -6,
     133,    85,  -137,  -137,  -137,   421,  -137,  -137,  -137,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,  -137,  -137,   341,   523,   -17,   200,   184,    22,  -137,
     184,    37,    76,    46,   438,   184,  -137,   561,    57,    57,
    -137,  -137,  -137,   549,   536,   214,   214,   214,   214,  -137,
    -137,   184,    33,   361,    84,    29,  -137,   224,   173,    82,
     184,   176,   239,   523,   184,   184,  -137,  -137,    -7,    22,
      28,    13,  -137,   184,    68,   523,    -3,  -137,   101,    70,
     381,   254,  -137,  -137,  -137,   184,    -8,   523,   206,    -1,
       7,  -137,    12,   104,  -137,   184,  -137,    96,   269,   286,
      86,   523,    80,   173,  -137,  -137,    89,   184,  -137,  -137,
      41,  -137,   284,   184,    91,    92,    95,   173,   173,  -137,
     181,  -137,    78,   455,    99,  -137,  -137,   401,  -137,  -137,
     181,     4,  -137,  -137,   472,    40,    78,  -137,  -137,  -137,
     173,   100,  -137,   489,   184,   173,   506,   126,    40,    41,
     127,    40
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     3,     0,     2,     0,     0,     1,    65,    66,     0,
       0,     0,     0,    31,     0,    10,     9,    22,    23,    24,
      25,     0,    72,     6,     0,    68,    69,    79,     0,     0,
       0,    65,    64,    72,    63,     0,     4,     8,     7,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    26,    28,     0,    78,     0,     0,     0,    14,    27,
       0,     0,     0,     0,     0,     0,    67,    60,    51,    52,
      53,    54,    55,    61,    62,    57,    56,    59,    58,    19,
      76,     0,    70,     0,     0,     0,    13,     0,     0,     0,
       0,     0,     0,    77,     0,     0,    16,    15,     0,     0,
       0,     0,    35,     0,     0,    38,     3,    32,    22,    70,
       0,     0,    11,    12,    17,     0,     0,    37,     0,     0,
       0,    44,     0,     0,    74,     0,    20,    71,     0,    65,
       0,    41,     0,     0,    42,    43,     0,     0,    29,    33,
       0,    73,     0,     0,     0,    68,    69,     0,     0,    49,
       0,    48,     0,     0,     0,    34,    71,     0,    18,    36,
       0,     0,    47,    46,     0,     0,     0,    21,    45,    50,
       0,     0,    75,     0,     0,     0,     0,    22,     0,     0,
       0,     0
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -137,  -137,     1,  -137,  -137,  -137,   122,  -137,    49,    -2,
     -80,  -117,  -137,  -137,  -137,  -137,  -137,  -137,    30,    11,
    -136,   -11,    10,  -137,    32,    -4,  -137
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     2,    13,     4,     5,    14,    15,    85,    86,   149,
      17,    18,    19,    63,   104,   130,    20,   120,   121,   150,
     151,   163,    21,   123,   124,    33,    55
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      22,     3,    16,   119,   132,   139,    29,    57,   116,    57,
      22,   108,    38,   119,   162,    80,    -5,   116,   136,    81,
      32,    34,    35,   155,   162,    58,     1,    59,    84,    59,
      60,    -5,    60,    30,    53,    30,   133,    54,    56,   169,
      64,   134,   138,   137,    30,     1,   171,   154,    94,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    98,   139,   114,     6,    99,   115,    83,    23,    88,
      87,    95,   137,     1,    52,    92,    42,    43,    44,    61,
      89,    90,    31,     8,    22,   172,   102,    22,    97,   107,
     108,    93,     7,     8,     9,   177,    10,   103,   180,   112,
     105,   138,    11,   118,   110,   111,    10,   122,   125,    12,
     140,   143,    11,   117,    25,    26,    27,   148,   147,    12,
     152,     1,    36,    65,   -39,   128,   158,   -40,   131,    22,
     166,   174,   179,   181,    61,   142,    37,    31,     8,    62,
       7,     8,     9,    22,    22,   159,    22,   153,   113,   168,
     135,    10,     0,   157,    10,   141,    22,    11,     0,   160,
      11,     0,   164,     0,    12,     0,    22,    12,   107,     1,
       0,    22,     0,   107,     0,     0,   173,     7,     8,   101,
       7,     8,   101,     0,   176,     7,     8,   161,    31,     8,
       0,    10,     0,     0,    10,     0,     0,    11,     0,    10,
      11,     0,    10,     0,    12,    11,     1,    12,    11,   106,
     129,     8,    12,     0,     1,    12,    39,    40,    41,    42,
      43,    44,    45,    46,    10,    47,    48,    49,    50,     0,
      11,    40,    41,    42,    43,    44,     0,    12,     0,    82,
      39,    40,    41,    42,    43,    44,    45,    46,     0,    47,
      48,    49,    50,     0,     0,    39,    40,    41,    42,    43,
      44,    45,    46,   100,    47,    48,    49,    50,     0,     0,
      39,    40,    41,    42,    43,    44,    45,    46,   109,    47,
      48,    49,    50,     0,     0,    39,    40,    41,    42,    43,
      44,    45,    46,   127,    47,    48,    49,    50,     0,     0,
      39,    40,    41,    42,    43,    44,    45,    46,   144,    47,
      48,    49,    50,    24,     0,   145,   146,    27,     0,     0,
       0,     0,     0,   156,    65,     0,     0,    25,    26,    27,
       0,     0,     0,     0,     0,     0,    28,    39,    40,    41,
      42,    43,    44,    45,    46,     0,    47,    48,    49,    50,
       0,     0,     0,     0,     0,     0,    51,    39,    40,    41,
      42,    43,    44,    45,    46,     0,    47,    48,    49,    50,
       0,     0,     0,     0,     0,     0,    79,    39,    40,    41,
      42,    43,    44,    45,    46,     0,    47,    48,    49,    50,
       0,     0,     0,     0,     0,     0,    96,    39,    40,    41,
      42,    43,    44,    45,    46,     0,    47,    48,    49,    50,
       0,     0,     0,     0,     0,     0,   126,    39,    40,    41,
      42,    43,    44,    45,    46,     0,    47,    48,    49,    50,
       0,     0,     0,     0,     0,     0,   167,    39,    40,    41,
      42,    43,    44,    45,    46,     0,    47,    48,    49,    50,
       0,     0,     0,    66,    39,    40,    41,    42,    43,    44,
      45,    46,     0,    47,    48,    49,    50,     0,     0,     0,
      91,    39,    40,    41,    42,    43,    44,    45,    46,     0,
      47,    48,    49,    50,     0,     0,     0,   165,    39,    40,
      41,    42,    43,    44,    45,    46,     0,    47,    48,    49,
      50,     0,     0,     0,   170,    39,    40,    41,    42,    43,
      44,    45,    46,     0,    47,    48,    49,    50,     0,     0,
       0,   175,    39,    40,    41,    42,    43,    44,    45,    46,
       0,    47,    48,    49,    50,     0,     0,     0,   178,    39,
      40,    41,    42,    43,    44,    45,    46,     0,    47,    48,
      49,    50,    39,    40,    41,    42,    43,    44,    45,     0,
       0,    47,    48,    49,    50,    39,    40,    41,    42,    43,
      44,     0,     0,     0,    47,    48,    49,    50,    40,    41,
      42,    43,    44,     0,     0,     0,    47,    48,    49,    50
};

static const yytype_int16 yycheck[] =
{
       4,     0,     4,     6,     5,   122,     4,    15,     4,    15,
      14,    91,    14,     6,   150,    32,    34,     4,     6,    36,
      10,    11,    12,   140,   160,    31,    33,    35,     6,    35,
      38,    34,    38,    31,    24,    31,    37,    27,    28,    35,
      30,    34,   122,    31,    31,    33,     6,     6,    15,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    32,   179,    35,     0,    36,    38,    57,    34,    32,
      60,    38,    31,    33,    35,    65,    19,    20,    21,     1,
       4,    35,     4,     5,    88,   165,    88,    91,     4,    91,
     170,    81,     4,     5,     6,   175,    18,    15,   178,    98,
      90,   181,    24,    35,    94,    95,    18,     6,    38,    31,
       6,    15,    24,   103,    29,    30,    31,    37,    32,    31,
      31,    33,    34,    38,    32,   115,    35,    32,   118,   133,
      31,    31,     6,     6,     1,   125,    14,     4,     5,     6,
       4,     5,     6,   147,   148,   147,   150,   137,    99,   160,
     120,    18,    -1,   143,    18,   123,   160,    24,    -1,   148,
      24,    -1,   152,    -1,    31,    -1,   170,    31,   170,    33,
      -1,   175,    -1,   175,    -1,    -1,   166,     4,     5,     6,
       4,     5,     6,    -1,   174,     4,     5,     6,     4,     5,
      -1,    18,    -1,    -1,    18,    -1,    -1,    24,    -1,    18,
      24,    -1,    18,    -1,    31,    24,    33,    31,    24,    33,
       4,     5,    31,    -1,    33,    31,    16,    17,    18,    19,
      20,    21,    22,    23,    18,    25,    26,    27,    28,    -1,
      24,    17,    18,    19,    20,    21,    -1,    31,    -1,    39,
      16,    17,    18,    19,    20,    21,    22,    23,    -1,    25,
      26,    27,    28,    -1,    -1,    16,    17,    18,    19,    20,
      21,    22,    23,    39,    25,    26,    27,    28,    -1,    -1,
      16,    17,    18,    19,    20,    21,    22,    23,    39,    25,
      26,    27,    28,    -1,    -1,    16,    17,    18,    19,    20,
      21,    22,    23,    39,    25,    26,    27,    28,    -1,    -1,
      16,    17,    18,    19,    20,    21,    22,    23,    39,    25,
      26,    27,    28,    15,    -1,    29,    30,    31,    -1,    -1,
      -1,    -1,    -1,    39,    38,    -1,    -1,    29,    30,    31,
      -1,    -1,    -1,    -1,    -1,    -1,    38,    16,    17,    18,
      19,    20,    21,    22,    23,    -1,    25,    26,    27,    28,
      -1,    -1,    -1,    -1,    -1,    -1,    35,    16,    17,    18,
      19,    20,    21,    22,    23,    -1,    25,    26,    27,    28,
      -1,    -1,    -1,    -1,    -1,    -1,    35,    16,    17,    18,
      19,    20,    21,    22,    23,    -1,    25,    26,    27,    28,
      -1,    -1,    -1,    -1,    -1,    -1,    35,    16,    17,    18,
      19,    20,    21,    22,    23,    -1,    25,    26,    27,    28,
      -1,    -1,    -1,    -1,    -1,    -1,    35,    16,    17,    18,
      19,    20,    21,    22,    23,    -1,    25,    26,    27,    28,
      -1,    -1,    -1,    -1,    -1,    -1,    35,    16,    17,    18,
      19,    20,    21,    22,    23,    -1,    25,    26,    27,    28,
      -1,    -1,    -1,    32,    16,    17,    18,    19,    20,    21,
      22,    23,    -1,    25,    26,    27,    28,    -1,    -1,    -1,
      32,    16,    17,    18,    19,    20,    21,    22,    23,    -1,
      25,    26,    27,    28,    -1,    -1,    -1,    32,    16,    17,
      18,    19,    20,    21,    22,    23,    -1,    25,    26,    27,
      28,    -1,    -1,    -1,    32,    16,    17,    18,    19,    20,
      21,    22,    23,    -1,    25,    26,    27,    28,    -1,    -1,
      -1,    32,    16,    17,    18,    19,    20,    21,    22,    23,
      -1,    25,    26,    27,    28,    -1,    -1,    -1,    32,    16,
      17,    18,    19,    20,    21,    22,    23,    -1,    25,    26,
      27,    28,    16,    17,    18,    19,    20,    21,    22,    -1,
      -1,    25,    26,    27,    28,    16,    17,    18,    19,    20,
      21,    -1,    -1,    -1,    25,    26,    27,    28,    17,    18,
      19,    20,    21,    -1,    -1,    -1,    25,    26,    27,    28
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    33,    43,    44,    45,    46,     0,     4,     5,     6,
      18,    24,    31,    44,    47,    48,    51,    52,    53,    54,
      58,    64,    67,    34,    15,    29,    30,    31,    38,     4,
      31,     4,    64,    67,    64,    64,    34,    48,    51,    16,
      17,    18,    19,    20,    21,    22,    23,    25,    26,    27,
      28,    35,    35,    64,    64,    68,    64,    15,    31,    35,
      38,     1,     6,    55,    64,    38,    32,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    35,
      32,    36,    39,    64,     6,    49,    50,    64,    32,     4,
      35,    32,    64,    64,    15,    38,    35,     4,    32,    36,
      39,     6,    51,    15,    56,    64,    33,    51,    52,    39,
      64,    64,    44,    50,    35,    38,     4,    64,    35,     6,
      59,    60,     6,    65,    66,    38,    35,    39,    64,     4,
      57,    64,     5,    37,    34,    60,     6,    31,    52,    53,
       6,    66,    64,    15,    39,    29,    30,    32,    37,    51,
      61,    62,    31,    64,     6,    53,    39,    64,    35,    51,
      61,     6,    62,    63,    64,    32,    31,    35,    63,    35,
      32,     6,    52,    64,    31,    32,    64,    52,    32,     6,
      52,     6
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    42,    43,    45,    44,    46,    44,    47,    47,    47,
      47,    48,    49,    49,    49,    50,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    52,
      52,    52,    53,    53,    53,    53,    54,    55,    56,    57,
      57,    57,    58,    59,    59,    60,    60,    61,    61,    62,
      63,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    65,    65,    66,    67,    68,    68,    68
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     4,     0,     3,     2,     2,     1,
       1,     6,     3,     1,     0,     2,     5,     6,     9,     4,
       7,    10,     1,     1,     1,     1,     2,     3,     2,     7,
       5,     1,     5,     7,     8,     5,     9,     4,     1,     2,
       2,     1,     7,     2,     1,     5,     4,     2,     1,     1,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     1,     1,     3,     2,     2,
       4,     7,     1,     2,     1,     5,     4,     3,     1,     0
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: block  */
#line 622 "parser.y"
          {
        rootNode = (yyvsp[0].node);
        // Print lexer table footer
        fprintf(stderr, "+-----------------+------------------+\n\n");
        printAST();
        printTAC(); // Print TAC after AST, symbol table, and function table
    }
#line 1923 "parser.tab.c"
    break;

  case 3: /* $@1: %empty  */
#line 632 "parser.y"
           { currentScope++; }
#line 1929 "parser.tab.c"
    break;

  case 4: /* block: LBRACE $@1 statements RBRACE  */
#line 632 "parser.y"
                                                 { currentScope--; (yyval.node) = (yyvsp[-1].node); }
#line 1935 "parser.tab.c"
    break;

  case 5: /* $@2: %empty  */
#line 633 "parser.y"
             { currentScope++; }
#line 1941 "parser.tab.c"
    break;

  case 6: /* block: LBRACE $@2 RBRACE  */
#line 633 "parser.y"
                                        { currentScope--; (yyval.node) = createNode("EmptyBlock", "block", NULL, NULL); }
#line 1947 "parser.tab.c"
    break;

  case 7: /* statements: statements statement  */
#line 637 "parser.y"
                         { (yyval.node) = createNode("Statements", "block", (yyvsp[-1].node), (yyvsp[0].node)); }
#line 1953 "parser.tab.c"
    break;

  case 8: /* statements: statements function_decl  */
#line 638 "parser.y"
                               { (yyval.node) = createNode("Statements", "block", (yyvsp[-1].node), (yyvsp[0].node)); }
#line 1959 "parser.tab.c"
    break;

  case 9: /* statements: statement  */
#line 639 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 1965 "parser.tab.c"
    break;

  case 10: /* statements: function_decl  */
#line 640 "parser.y"
                    { (yyval.node) = (yyvsp[0].node); }
#line 1971 "parser.tab.c"
    break;

  case 11: /* function_decl: KEYWORD IDENTIFIER LPAREN param_list RPAREN block  */
#line 644 "parser.y"
                                                      {
        char* returnType = getTypeFromKeyword((yyvsp[-5].str));
        
        // Very important: Add function to symbol table FIRST in global scope
        addSymbol((yyvsp[-4].str), "function", 0, 0);
        
        // Then add to function table
        int funcIndex = addFunction((yyvsp[-4].str), returnType);
        
        if (funcIndex >= 0) {
            // Process parameters from stored list
            for (int i = 0; i < paramCount; i++) {
                addParameter(funcIndex, paramNames[i], paramTypes[i]);
                free(paramNames[i]);
                free(paramTypes[i]);
            }
            paramCount = 0;
            
            (yyval.node) = createNode("Function", returnType, 
                           createNode((yyvsp[-4].str), "function", (yyvsp[-2].node), (yyvsp[0].node)), NULL);
        } else {
            (yyval.node) = createNode("Error", "error", NULL, NULL);
        }
        free(returnType);
    }
#line 2001 "parser.tab.c"
    break;

  case 12: /* param_list: param_list COMMA param  */
#line 672 "parser.y"
                           { 
        (yyval.node) = createNode("Parameters", "params", (yyvsp[-2].node), (yyvsp[0].node)); 
    }
#line 2009 "parser.tab.c"
    break;

  case 13: /* param_list: param  */
#line 675 "parser.y"
            { 
        (yyval.node) = (yyvsp[0].node); 
    }
#line 2017 "parser.tab.c"
    break;

  case 14: /* param_list: %empty  */
#line 678 "parser.y"
                  { 
        (yyval.node) = NULL; 
    }
#line 2025 "parser.tab.c"
    break;

  case 15: /* param: KEYWORD IDENTIFIER  */
#line 684 "parser.y"
                       {
        char* type = getTypeFromKeyword((yyvsp[-1].str));
        
        // Store parameter for later addition to function
        if (paramCount < 10) {
            paramNames[paramCount] = strdup((yyvsp[0].str));
            paramTypes[paramCount] = strdup(type);
            paramCount++;
        }
        
        // Add to symbol table with current scope (for body access)
        addSymbol((yyvsp[0].str), type, currentScope+1, 0);
        
        (yyval.node) = createNode("Parameter", type, createNode((yyvsp[0].str), type, NULL, NULL), NULL);
        free(type);
    }
#line 2046 "parser.tab.c"
    break;

  case 16: /* statement: KEYWORD IDENTIFIER ASSIGN expression SEMICOLON  */
#line 703 "parser.y"
                                                   {
        char* type = getTypeFromKeyword((yyvsp[-4].str));
        if (isAssignmentCompatible(type, (yyvsp[-1].node)->type)) {
            addSymbol((yyvsp[-3].str), type, currentScope, 0);
            addTAC((yyvsp[-3].str), (yyvsp[-1].node)->name, "=", NULL);
            (yyval.node) = createNode("Assignment", type, createNode((yyvsp[-3].str), type, NULL, NULL), (yyvsp[-1].node));
            (yyval.node)->value = (yyvsp[-1].node)->value;
            symbolTable[lookupSymbol((yyvsp[-3].str), currentScope)].value = (yyvsp[-1].node)->value;
        } else {
            (yyval.node) = createNode("Error", "error", NULL, NULL);
        }
        free(type);
    }
#line 2064 "parser.tab.c"
    break;

  case 17: /* statement: KEYWORD IDENTIFIER LBRACKET expression RBRACKET SEMICOLON  */
#line 716 "parser.y"
                                                                {
        char* type = getTypeFromKeyword((yyvsp[-5].str));
        // Store as array type and track dimensions
        addSymbol((yyvsp[-4].str), "array", currentScope, 1);
        (yyval.node) = createNode("ArrayDeclaration", type, 
                       createNode((yyvsp[-4].str), "array", NULL, NULL), (yyvsp[-2].node));
        free(type);
    }
#line 2077 "parser.tab.c"
    break;

  case 18: /* statement: KEYWORD IDENTIFIER LBRACKET expression RBRACKET LBRACKET expression RBRACKET SEMICOLON  */
#line 724 "parser.y"
                                                                                             {
        char* type = getTypeFromKeyword((yyvsp[-8].str));
        // For 2D arrays, store dimensions count as 2
        addSymbol((yyvsp[-7].str), "array", currentScope, 2);
        Node* dimensions = createNode("Dimensions", "int", (yyvsp[-5].node), (yyvsp[-2].node));
        (yyval.node) = createNode("ArrayDeclaration2D", type,
                       createNode((yyvsp[-7].str), "array", NULL, NULL), dimensions);
        free(type);
    }
#line 2091 "parser.tab.c"
    break;

  case 19: /* statement: IDENTIFIER ASSIGN expression SEMICOLON  */
#line 733 "parser.y"
                                             {
        int index = lookupSymbol((yyvsp[-3].str), currentScope);
        if (index == -1) {
            yyerror("Undeclared variable used");
            (yyval.node) = createNode("Error", "error", NULL, NULL);
        } else if (isAssignmentCompatible(symbolTable[index].type, (yyvsp[-1].node)->type)) {
            addTAC((yyvsp[-3].str), (yyvsp[-1].node)->name, "=", NULL);
            (yyval.node) = createNode("Assignment", symbolTable[index].type, 
                           createNode((yyvsp[-3].str), symbolTable[index].type, NULL, NULL), (yyvsp[-1].node));
            (yyval.node)->value = (yyvsp[-1].node)->value;
            symbolTable[index].value = (yyvsp[-1].node)->value;
        } else {
            (yyval.node) = createNode("Error", "error", NULL, NULL);
        }
    }
#line 2111 "parser.tab.c"
    break;

  case 20: /* statement: IDENTIFIER LBRACKET expression RBRACKET ASSIGN expression SEMICOLON  */
#line 748 "parser.y"
                                                                          {
        int index = lookupSymbol((yyvsp[-6].str), currentScope);
        if (index == -1 || strcmp(symbolTable[index].type, "array") != 0) {
            yyerror("Invalid array access");
            (yyval.node) = createNode("Error", "error", NULL, NULL);
        } else {
            (yyval.node) = createNode("ArrayAssignment", "int", 
                           createNode((yyvsp[-6].str), "array", (yyvsp[-4].node), NULL), (yyvsp[-1].node));
        }
    }
#line 2126 "parser.tab.c"
    break;

  case 21: /* statement: IDENTIFIER LBRACKET expression RBRACKET LBRACKET expression RBRACKET ASSIGN expression SEMICOLON  */
#line 758 "parser.y"
                                                                                                       {
        int index = lookupSymbol((yyvsp[-9].str), currentScope);
        if (index == -1 || strcmp(symbolTable[index].type, "array") != 0) {
            yyerror("Invalid array access");
            (yyval.node) = createNode("Error", "error", NULL, NULL);
        } else {
            Node* indices = createNode("Indices", "int", (yyvsp[-7].node), (yyvsp[-4].node));
            (yyval.node) = createNode("Array2DAssignment", "int",
                           createNode((yyvsp[-9].str), "array", indices, NULL), (yyvsp[-1].node));
        }
    }
#line 2142 "parser.tab.c"
    break;

  case 22: /* statement: matched_stmt  */
#line 769 "parser.y"
                   { (yyval.node) = (yyvsp[0].node); }
#line 2148 "parser.tab.c"
    break;

  case 23: /* statement: unmatched_stmt  */
#line 770 "parser.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 2154 "parser.tab.c"
    break;

  case 24: /* statement: for_statement  */
#line 771 "parser.y"
                    { (yyval.node) = (yyvsp[0].node); }
#line 2160 "parser.tab.c"
    break;

  case 25: /* statement: switch_statement  */
#line 772 "parser.y"
                       { (yyval.node) = (yyvsp[0].node); }
#line 2166 "parser.tab.c"
    break;

  case 26: /* statement: expression SEMICOLON  */
#line 773 "parser.y"
                           { (yyval.node) = (yyvsp[-1].node); }
#line 2172 "parser.tab.c"
    break;

  case 27: /* statement: KEYWORD IDENTIFIER SEMICOLON  */
#line 774 "parser.y"
                                   {
        if (strcmp((yyvsp[-2].str), "return_RP") == 0) {
            int index = lookupSymbol((yyvsp[-1].str), currentScope);
            if (index == -1) {
                printf("Semantic Error: Returning undeclared variable '%s'\n", (yyvsp[-1].str));
                (yyval.node) = createNode("Return", "error", NULL, NULL);
            } else {
                (yyval.node) = createNode("Return", symbolTable[index].type, 
                               createNode((yyvsp[-1].str), symbolTable[index].type, NULL, NULL), NULL);
            }
        } else {
            yyerror("Unexpected keyword in statement");
            (yyval.node) = createNode("Error", "error", NULL, NULL);
        }
    }
#line 2192 "parser.tab.c"
    break;

  case 28: /* statement: function_call SEMICOLON  */
#line 789 "parser.y"
                              { (yyval.node) = (yyvsp[-1].node); }
#line 2198 "parser.tab.c"
    break;

  case 29: /* matched_stmt: KEYWORD LPAREN expression RPAREN matched_stmt KEYWORD matched_stmt  */
#line 793 "parser.y"
                                                                       {
        if (strcmp((yyvsp[-6].str), "if_RP") == 0 && strcmp((yyvsp[-1].str), "else_RP") == 0) {
            Node *if_node = createNode("If", "control", (yyvsp[-4].node), (yyvsp[-2].node));
            Node *else_node = createNode("Else", "control", NULL, (yyvsp[0].node));
            (yyval.node) = createNode("IfElse", "control", if_node, else_node);

            // TAC for if-else
            char *label_true = newLabel();
            char *label_false = newLabel();
            char *label_end = newLabel();
            addTAC(NULL, NULL, NULL, NULL); // for alignment
            addTAC(NULL, NULL, NULL, NULL);
            addTAC(NULL, (yyvsp[-4].node)->name, "if", label_true);
            addTAC(NULL, NULL, "goto", label_false);
            addTAC(label_true, NULL, NULL, NULL);
            generateTACForNode((yyvsp[-2].node));
            addTAC(NULL, NULL, "goto", label_end);
            addTAC(label_false, NULL, NULL, NULL);
            generateTACForNode((yyvsp[0].node));
            addTAC(label_end, NULL, NULL, NULL);
        } else {
            yyerror("Expected 'if_RP' and 'else_RP' keywords");
            (yyval.node) = NULL;
        }
    }
#line 2228 "parser.tab.c"
    break;

  case 30: /* matched_stmt: KEYWORD LPAREN expression RPAREN matched_stmt  */
#line 818 "parser.y"
                                                    {
        if (strcmp((yyvsp[-4].str), "while_RP") == 0) {
            (yyval.node) = createNode("While", "control", (yyvsp[-2].node), (yyvsp[0].node));

            // TAC for while
            char *label_start = newLabel();
            char *label_body = newLabel();
            char *label_end = newLabel();
            addTAC(label_start, NULL, NULL, NULL);
            addTAC(NULL, (yyvsp[-2].node)->name, "if", label_body);
            addTAC(NULL, NULL, "goto", label_end);
            addTAC(label_body, NULL, NULL, NULL);
            generateTACForNode((yyvsp[0].node));
            addTAC(NULL, NULL, "goto", label_start);
            addTAC(label_end, NULL, NULL, NULL);
        } else {
            yyerror("Expected 'while_RP' keyword");
            (yyval.node) = NULL;
        }
    }
#line 2253 "parser.tab.c"
    break;

  case 31: /* matched_stmt: block  */
#line 838 "parser.y"
            { (yyval.node) = (yyvsp[0].node); }
#line 2259 "parser.tab.c"
    break;

  case 32: /* unmatched_stmt: KEYWORD LPAREN expression RPAREN statement  */
#line 842 "parser.y"
                                               {
        if (strcmp((yyvsp[-4].str), "if_RP") == 0) {
            (yyval.node) = createNode("If", "control", (yyvsp[-2].node), (yyvsp[0].node));

            // TAC for if (no else)
            char *label_true = newLabel();
            char *label_end = newLabel();
            addTAC(NULL, (yyvsp[-2].node)->name, "if", label_true);
            addTAC(NULL, NULL, "goto", label_end);
            addTAC(label_true, NULL, NULL, NULL);
            generateTACForNode((yyvsp[0].node));
            addTAC(label_end, NULL, NULL, NULL);
        } else if (strcmp((yyvsp[-4].str), "while_RP") == 0) {
            (yyval.node) = createNode("While", "control", (yyvsp[-2].node), (yyvsp[0].node));

            // TAC for while
            char *label_start = newLabel();
            char *label_body = newLabel();
            char *label_end = newLabel();
            addTAC(label_start, NULL, NULL, NULL);
            addTAC(NULL, (yyvsp[-2].node)->name, "if", label_body);
            addTAC(NULL, NULL, "goto", label_end);
            addTAC(label_body, NULL, NULL, NULL);
            generateTACForNode((yyvsp[0].node));
            addTAC(NULL, NULL, "goto", label_start);
            addTAC(label_end, NULL, NULL, NULL);
        } else {
            yyerror("Expected 'if_RP' or 'while_RP' keyword");
            (yyval.node) = NULL;
        }
    }
#line 2295 "parser.tab.c"
    break;

  case 33: /* unmatched_stmt: KEYWORD LPAREN expression RPAREN matched_stmt KEYWORD unmatched_stmt  */
#line 873 "parser.y"
                                                                           {
        if (strcmp((yyvsp[-6].str), "if_RP") == 0 && strcmp((yyvsp[-1].str), "else_RP") == 0) {
            Node *if_node = createNode("If", "control", (yyvsp[-4].node), (yyvsp[-2].node));
            Node *else_node = createNode("Else", "control", NULL, (yyvsp[0].node));
            (yyval.node) = createNode("IfElse", "control", if_node, else_node);
        } else {
            yyerror("Expected 'if_RP' and 'else_RP' keywords");
            (yyval.node) = NULL;
        }
    }
#line 2310 "parser.tab.c"
    break;

  case 34: /* unmatched_stmt: KEYWORD LPAREN expression RPAREN matched_stmt elif_chain KEYWORD unmatched_stmt  */
#line 883 "parser.y"
                                                                                      {
        if (strcmp((yyvsp[-7].str), "if_RP") == 0 && strcmp((yyvsp[-1].str), "else_RP") == 0) {
            Node *if_node = createNode("If", "control", (yyvsp[-5].node), (yyvsp[-3].node));
            Node *elif_else = createNode("ElifElse", "control", (yyvsp[-2].node), (yyvsp[0].node));
            (yyval.node) = createNode("IfElifElse", "control", if_node, elif_else);
        } else {
            yyerror("Expected 'if_RP' and 'else_RP' keywords");
            (yyval.node) = NULL;
        }
    }
#line 2325 "parser.tab.c"
    break;

  case 35: /* unmatched_stmt: KEYWORD LPAREN error RPAREN statement  */
#line 893 "parser.y"
                                            {
        yyerror("Expected expression in if statement");
        (yyval.node) = NULL;
    }
#line 2334 "parser.tab.c"
    break;

  case 36: /* for_statement: KEYWORD LPAREN declaration SEMICOLON condition SEMICOLON increment_expr RPAREN statement  */
#line 900 "parser.y"
                                                                                             {
        if (strcmp((yyvsp[-8].str), "for_RP") == 0) {
            (yyval.node) = createNode("For", "control", (yyvsp[-6].node),
                createNode("ForBody", "control", (yyvsp[-4].node),
                    createNode("ForIncr", "control", (yyvsp[-2].node), (yyvsp[0].node))));

            // TAC for for loop
            char *label_start = newLabel();
            char *label_body = newLabel();
            char *label_end = newLabel();
            generateTACForNode((yyvsp[-6].node)); // init
            addTAC(label_start, NULL, NULL, NULL);
            addTAC(NULL, (yyvsp[-4].node)->name, "if", label_body);
            addTAC(NULL, NULL, "goto", label_end);
            addTAC(label_body, NULL, NULL, NULL);
            generateTACForNode((yyvsp[0].node)); // body
            generateTACForNode((yyvsp[-2].node)); // increment
            addTAC(NULL, NULL, "goto", label_start);
            addTAC(label_end, NULL, NULL, NULL);
        } else {
            yyerror("Expected 'for_RP' keyword");
            (yyval.node) = NULL;
        }
    }
#line 2363 "parser.tab.c"
    break;

  case 37: /* declaration: KEYWORD IDENTIFIER ASSIGN expression  */
#line 927 "parser.y"
                                         {
        char* type = getTypeFromKeyword((yyvsp[-3].str));
        addSymbol((yyvsp[-2].str), type, currentScope, 0);
        (yyval.node) = createNode("Initialize", type, createNode((yyvsp[-2].str), type, NULL, NULL), (yyvsp[0].node));
        free(type);
    }
#line 2374 "parser.tab.c"
    break;

  case 38: /* condition: expression  */
#line 936 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 2380 "parser.tab.c"
    break;

  case 39: /* increment_expr: IDENTIFIER INCREMENT  */
#line 940 "parser.y"
                         {
        int index = lookupSymbol((yyvsp[-1].str), currentScope);
        if (index != -1) {
            (yyval.node) = createNode("++", symbolTable[index].type, 
                createNode((yyvsp[-1].str), symbolTable[index].type, NULL, NULL), NULL);
        } else {
            yyerror("Undeclared variable in increment");
            (yyval.node) = NULL;
        }
    }
#line 2395 "parser.tab.c"
    break;

  case 40: /* increment_expr: IDENTIFIER DECREMENT  */
#line 950 "parser.y"
                           {
        int index = lookupSymbol((yyvsp[-1].str), currentScope);
        if (index != -1) {
            (yyval.node) = createNode("--", symbolTable[index].type,
                createNode((yyvsp[-1].str), symbolTable[index].type, NULL, NULL), NULL);
        } else {
            yyerror("Undeclared variable in decrement");
            (yyval.node) = NULL;
        }
    }
#line 2410 "parser.tab.c"
    break;

  case 41: /* increment_expr: expression  */
#line 960 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 2416 "parser.tab.c"
    break;

  case 42: /* switch_statement: KEYWORD LPAREN expression RPAREN LBRACE cases RBRACE  */
#line 964 "parser.y"
                                                         {
        if (strcmp((yyvsp[-6].str), "switch_RP") == 0) {
            (yyval.node) = createNode("Switch", "control", (yyvsp[-4].node), (yyvsp[-1].node));

            // TAC for switch
            // For simplicity, generate a sequence of if-goto for each case, then goto default if present
            Node *cases = (yyvsp[-1].node);
            char *endLabel = newLabel();
            Node *current = cases;
            while (current) {
                if (strcmp(current->name, "Cases") == 0) {
                    Node *caseNode = current->right;
                    if (caseNode && strcmp(caseNode->name, "Case") == 0) {
                        char *caseLabel = newLabel();
                        addTAC(NULL, (yyvsp[-4].node)->name, "==", caseNode->left->name);
                        addTAC(NULL, NULL, "if", caseLabel);
                        // ... skip to next case if not matched
                        // Save label for case body
                        addTAC(caseLabel, NULL, NULL, NULL);
                        generateTACForNode(caseNode->right); // Case body
                        addTAC(NULL, NULL, "goto", endLabel);
                    }
                    current = current->left;
                } else if (strcmp(current->name, "Case") == 0) {
                    char *caseLabel = newLabel();
                    addTAC(NULL, (yyvsp[-4].node)->name, "==", current->left->name);
                    addTAC(NULL, NULL, "if", caseLabel);
                    addTAC(caseLabel, NULL, NULL, NULL);
                    generateTACForNode(current->right);
                    addTAC(NULL, NULL, "goto", endLabel);
                    break;
                } else if (strcmp(current->name, "Default") == 0) {
                    char *defaultLabel = newLabel();
                    addTAC(defaultLabel, NULL, NULL, NULL);
                    generateTACForNode(current->right);
                    addTAC(NULL, NULL, "goto", endLabel);
                    break;
                } else {
                    break;
                }
            }
            addTAC(endLabel, NULL, NULL, NULL);
        } else {
            yyerror("Expected 'switch_RP' keyword");
            (yyval.node) = NULL;
        }
    }
#line 2468 "parser.tab.c"
    break;

  case 43: /* cases: cases case_statement  */
#line 1014 "parser.y"
                         { (yyval.node) = createNode("Cases", "block", (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2474 "parser.tab.c"
    break;

  case 44: /* cases: case_statement  */
#line 1015 "parser.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 2480 "parser.tab.c"
    break;

  case 45: /* case_statement: KEYWORD NUMBER COLON case_statements break_statement  */
#line 1019 "parser.y"
                                                         {
        if (strcmp((yyvsp[-4].str), "case_RP") == 0) {
            Node* caseBody = createNode("CaseBody", "block", (yyvsp[-1].node), (yyvsp[0].node));
            (yyval.node) = createNode("Case", "int", createNode((yyvsp[-3].str), "int", NULL, NULL), caseBody);
        } else {
            yyerror("Expected 'case_RP' keyword");
            (yyval.node) = NULL;
        }
    }
#line 2494 "parser.tab.c"
    break;

  case 46: /* case_statement: KEYWORD COLON case_statements break_statement  */
#line 1028 "parser.y"
                                                    {
        if (strcmp((yyvsp[-3].str), "default_RP") == 0) {
            Node* defaultBody = createNode("DefaultBody", "block", (yyvsp[-1].node), (yyvsp[0].node));
            (yyval.node) = createNode("Default", "int", NULL, defaultBody);
        } else {
            yyerror("Expected 'default_RP' keyword");
            (yyval.node) = NULL;
        }
    }
#line 2508 "parser.tab.c"
    break;

  case 47: /* case_statements: case_statements case_statement_item  */
#line 1040 "parser.y"
                                        { 
        (yyval.node) = createNode("CaseStatements", "block", (yyvsp[-1].node), (yyvsp[0].node)); 
    }
#line 2516 "parser.tab.c"
    break;

  case 48: /* case_statements: case_statement_item  */
#line 1043 "parser.y"
                          { 
        (yyval.node) = (yyvsp[0].node); 
    }
#line 2524 "parser.tab.c"
    break;

  case 49: /* case_statement_item: statement  */
#line 1049 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 2530 "parser.tab.c"
    break;

  case 50: /* break_statement: KEYWORD SEMICOLON  */
#line 1053 "parser.y"
                      {
        if (strcmp((yyvsp[-1].str), "break_RP") == 0) {
            (yyval.node) = createNode("Break", "control", NULL, NULL);
        } else {
            yyerror("Expected 'break_RP' keyword");
            (yyval.node) = NULL;
        }
    }
#line 2543 "parser.tab.c"
    break;

  case 51: /* expression: expression PLUS expression  */
#line 1064 "parser.y"
                               {
        char *temp = newTemp();
        addTAC(temp, (yyvsp[-2].node)->name, "+", (yyvsp[0].node)->name);
        (yyval.node) = createNode(temp, "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = (yyvsp[-2].node)->value + (yyvsp[0].node)->value;
    }
#line 2554 "parser.tab.c"
    break;

  case 52: /* expression: expression MINUS expression  */
#line 1070 "parser.y"
                                  {
        char *temp = newTemp();
        addTAC(temp, (yyvsp[-2].node)->name, "-", (yyvsp[0].node)->name);
        (yyval.node) = createNode(temp, "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = (yyvsp[-2].node)->value - (yyvsp[0].node)->value;
    }
#line 2565 "parser.tab.c"
    break;

  case 53: /* expression: expression MULT expression  */
#line 1076 "parser.y"
                                 {
        char *temp = newTemp();
        addTAC(temp, (yyvsp[-2].node)->name, "*", (yyvsp[0].node)->name);
        (yyval.node) = createNode(temp, "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = (yyvsp[-2].node)->value * (yyvsp[0].node)->value;
    }
#line 2576 "parser.tab.c"
    break;

  case 54: /* expression: expression DIV expression  */
#line 1082 "parser.y"
                                {
        char *temp = newTemp();
        addTAC(temp, (yyvsp[-2].node)->name, "/", (yyvsp[0].node)->name);
        (yyval.node) = createNode(temp, "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = (yyvsp[-2].node)->value / (yyvsp[0].node)->value;
    }
#line 2587 "parser.tab.c"
    break;

  case 55: /* expression: expression MOD expression  */
#line 1088 "parser.y"
                                {
        // If either operand is float, result must be float
        if (strcmp((yyvsp[-2].node)->type, "float") == 0 || strcmp((yyvsp[0].node)->type, "float") == 0) {
            (yyval.node) = createNode("%", "float", (yyvsp[-2].node), (yyvsp[0].node));
            // For float operations, we need to convert to int first for modulo
            (yyval.node)->value = (int)((yyvsp[-2].node)->value) % (int)((yyvsp[0].node)->value);
        } else {
            (yyval.node) = createNode("%", "int", (yyvsp[-2].node), (yyvsp[0].node));
            (yyval.node)->value = evaluateExpr("%", (yyvsp[-2].node)->value, (yyvsp[0].node)->value);
        }
    }
#line 2603 "parser.tab.c"
    break;

  case 56: /* expression: expression GT expression  */
#line 1099 "parser.y"
                               {
        (yyval.node) = createNode(">", "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = ((yyvsp[-2].node)->value > (yyvsp[0].node)->value);
    }
#line 2612 "parser.tab.c"
    break;

  case 57: /* expression: expression LT expression  */
#line 1103 "parser.y"
                               {
        (yyval.node) = createNode("<", "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = ((yyvsp[-2].node)->value < (yyvsp[0].node)->value);
    }
#line 2621 "parser.tab.c"
    break;

  case 58: /* expression: expression GE expression  */
#line 1107 "parser.y"
                               {
        (yyval.node) = createNode(">=", "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = ((yyvsp[-2].node)->value >= (yyvsp[0].node)->value);
    }
#line 2630 "parser.tab.c"
    break;

  case 59: /* expression: expression LE expression  */
#line 1111 "parser.y"
                               {
        (yyval.node) = createNode("<=", "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = ((yyvsp[-2].node)->value <= (yyvsp[0].node)->value);
    }
#line 2639 "parser.tab.c"
    break;

  case 60: /* expression: expression EQ expression  */
#line 1115 "parser.y"
                               {
        (yyval.node) = createNode("==", "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = ((yyvsp[-2].node)->value == (yyvsp[0].node)->value);
    }
#line 2648 "parser.tab.c"
    break;

  case 61: /* expression: expression AND expression  */
#line 1119 "parser.y"
                                {
        (yyval.node) = createNode("&&", "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = ((yyvsp[-2].node)->value && (yyvsp[0].node)->value);
    }
#line 2657 "parser.tab.c"
    break;

  case 62: /* expression: expression OR expression  */
#line 1123 "parser.y"
                               {
        (yyval.node) = createNode("||", "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = ((yyvsp[-2].node)->value || (yyvsp[0].node)->value);
    }
#line 2666 "parser.tab.c"
    break;

  case 63: /* expression: NOT expression  */
#line 1127 "parser.y"
                     {
        (yyval.node) = createNode("!", "int", (yyvsp[0].node), NULL);
        (yyval.node)->value = (!(yyvsp[0].node)->value);
    }
#line 2675 "parser.tab.c"
    break;

  case 64: /* expression: MINUS expression  */
#line 1131 "parser.y"
                                    {
        (yyval.node) = createNode("unary-", "int", (yyvsp[0].node), NULL);
        (yyval.node)->value = (-(yyvsp[0].node)->value);
    }
#line 2684 "parser.tab.c"
    break;

  case 65: /* expression: IDENTIFIER  */
#line 1135 "parser.y"
                 {
        int index = lookupSymbol((yyvsp[0].str), currentScope);
        if (index == -1) {
            printf("Semantic Error: Undeclared variable '%s' used in expression\n", (yyvsp[0].str));
            (yyval.node) = createNode((yyvsp[0].str), "error", NULL, NULL);
        } else {
            (yyval.node) = createNode((yyvsp[0].str), symbolTable[index].type, NULL, NULL);
            (yyval.node)->value = symbolTable[index].value;
        }
    }
#line 2699 "parser.tab.c"
    break;

  case 66: /* expression: NUMBER  */
#line 1145 "parser.y"
             {
        // Check if number contains a decimal point
        if (strchr((yyvsp[0].str), '.') != NULL) {
            (yyval.node) = createNode((yyvsp[0].str), "float", NULL, NULL);
            (yyval.node)->value = (int)atof((yyvsp[0].str)); // Store approximate int value
        } else {
            (yyval.node) = createNode((yyvsp[0].str), "int", NULL, NULL);
            (yyval.node)->value = atoi((yyvsp[0].str));
        }
    }
#line 2714 "parser.tab.c"
    break;

  case 67: /* expression: LPAREN expression RPAREN  */
#line 1155 "parser.y"
                               { 
        (yyval.node) = (yyvsp[-1].node); 
    }
#line 2722 "parser.tab.c"
    break;

  case 68: /* expression: IDENTIFIER INCREMENT  */
#line 1158 "parser.y"
                           {
        int index = lookupSymbol((yyvsp[-1].str), currentScope);
        if (index != -1) {
            (yyval.node) = createNode("++", symbolTable[index].type,
                createNode((yyvsp[-1].str), symbolTable[index].type, NULL, NULL), NULL);
        } else {
            addSymbol((yyvsp[-1].str), "int", currentScope, 0);
            (yyval.node) = createNode("++", "int", createNode((yyvsp[-1].str), "int", NULL, NULL), NULL);
        }
    }
#line 2737 "parser.tab.c"
    break;

  case 69: /* expression: IDENTIFIER DECREMENT  */
#line 1168 "parser.y"
                           {
        int index = lookupSymbol((yyvsp[-1].str), currentScope);
        if (index != -1) {
            (yyval.node) = createNode("--", symbolTable[index].type,
                createNode((yyvsp[-1].str), symbolTable[index].type, NULL, NULL), NULL);
        } else {
            addSymbol((yyvsp[-1].str), "int", currentScope, 0);
            (yyval.node) = createNode("--", "int", createNode((yyvsp[-1].str), "int", NULL, NULL), NULL);
        }
    }
#line 2752 "parser.tab.c"
    break;

  case 70: /* expression: IDENTIFIER LBRACKET expression RBRACKET  */
#line 1178 "parser.y"
                                              {
        int index = lookupSymbol((yyvsp[-3].str), currentScope);
        if (index == -1 || strcmp(symbolTable[index].type, "array") != 0) {
            yyerror("Invalid array access in expression");
            (yyval.node) = createNode("error", "error", NULL, NULL);
        } else if (strcmp((yyvsp[-1].node)->type, "float") == 0) {
            printf("Semantic Error: Array index must be integer, not float\n");
            (yyval.node) = createNode("ArrayAccessError", "error", 
                           createNode((yyvsp[-3].str), "array", (yyvsp[-1].node), NULL), NULL);
        } else {
            (yyval.node) = createNode("ArrayAccess", "int", 
                           createNode((yyvsp[-3].str), "array", (yyvsp[-1].node), NULL), NULL);
        }
    }
#line 2771 "parser.tab.c"
    break;

  case 71: /* expression: IDENTIFIER LBRACKET expression RBRACKET LBRACKET expression RBRACKET  */
#line 1192 "parser.y"
                                                                           {
        int index = lookupSymbol((yyvsp[-6].str), currentScope);
        if (index == -1 || strcmp(symbolTable[index].type, "array") != 0) {
            yyerror("Invalid 2D array access in expression");
            (yyval.node) = createNode("error", "error", NULL, NULL);
        } else if (strcmp((yyvsp[-4].node)->type, "float") == 0 || strcmp((yyvsp[-1].node)->type, "float") == 0) {
            printf("Semantic Error: Array indices must be integer, not '%s' and '%s'\n", 
                   (yyvsp[-4].node)->type, (yyvsp[-1].node)->type);
            (yyval.node) = createNode("Array2DAccessError", "error", NULL, NULL);
        } else {
            Node* indices = createNode("Indices", "int", (yyvsp[-4].node), (yyvsp[-1].node));
            (yyval.node) = createNode("Array2DAccess", "int",
                           createNode((yyvsp[-6].str), "array", indices, NULL), NULL);
        }
    }
#line 2791 "parser.tab.c"
    break;

  case 72: /* expression: function_call  */
#line 1207 "parser.y"
                    { (yyval.node) = (yyvsp[0].node); }
#line 2797 "parser.tab.c"
    break;

  case 73: /* elif_chain: elif_chain elif_clause  */
#line 1212 "parser.y"
                           { (yyval.node) = createNode("ElifChain", "control", (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2803 "parser.tab.c"
    break;

  case 74: /* elif_chain: elif_clause  */
#line 1213 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 2809 "parser.tab.c"
    break;

  case 75: /* elif_clause: KEYWORD LPAREN expression RPAREN matched_stmt  */
#line 1217 "parser.y"
                                                  {
        if (strcmp((yyvsp[-4].str), "elif_RP") == 0) {
            (yyval.node) = createNode("Elif", "control", (yyvsp[-2].node), (yyvsp[0].node));
        } else {
            yyerror("Expected 'elif_RP' keyword");
            (yyval.node) = NULL;
        }
    }
#line 2822 "parser.tab.c"
    break;

  case 76: /* function_call: IDENTIFIER LPAREN arg_list RPAREN  */
#line 1228 "parser.y"
                                      {
        int index = lookupSymbol((yyvsp[-3].str), currentScope);
        
        if (index == -1) {
            printf("Semantic Error: Undefined function '%s'\n", (yyvsp[-3].str));
            (yyval.node) = createNode("Error", "error", NULL, NULL);
        } else if (strcmp(symbolTable[index].type, "function") != 0) {
            printf("Semantic Error: '%s' is not a function\n", (yyvsp[-3].str));
            (yyval.node) = createNode("Error", "error", NULL, NULL);
        } else {
            // Find best matching function based on argument types
            int funcIndex = findMatchingFunction((yyvsp[-3].str), (yyvsp[-1].node));
            
            if (funcIndex == -1) {
                printf("Semantic Error: No matching overloaded function '%s' found for given arguments\n", (yyvsp[-3].str));
                (yyval.node) = createNode("Error", "error", NULL, NULL);
            } else {
                // Create a node that includes the function ID to disambiguate which overloaded version
                char functionIdStr[20];
                sprintf(functionIdStr, "%s#%d", (yyvsp[-3].str), functionTable[funcIndex].id);
                
                (yyval.node) = createNode("Call", functionTable[funcIndex].returnType,
                               createNode(functionIdStr, "function", (yyvsp[-1].node), NULL), NULL);
                
                printf("Function call resolved to: %s (ID: %d)\n", 
                       functionTable[funcIndex].name, functionTable[funcIndex].id);
            }
        }
    }
#line 2856 "parser.tab.c"
    break;

  case 77: /* arg_list: arg_list COMMA expression  */
#line 1260 "parser.y"
                              { 
        (yyval.node) = createNode("Arguments", "args", (yyvsp[-2].node), (yyvsp[0].node)); 
    }
#line 2864 "parser.tab.c"
    break;

  case 78: /* arg_list: expression  */
#line 1263 "parser.y"
                 { 
        (yyval.node) = createNode("Argument", "arg", (yyvsp[0].node), NULL); 
    }
#line 2872 "parser.tab.c"
    break;

  case 79: /* arg_list: %empty  */
#line 1266 "parser.y"
                  { 
        (yyval.node) = NULL; 
    }
#line 2880 "parser.tab.c"
    break;


#line 2884 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1271 "parser.y"


// Move the countArguments function to the C section after the grammar
int countArguments(Node* argList) {
    if (argList == NULL) return 0;
    
    // If it's a single argument
    if (strcmp(argList->name, "Argument") == 0) return 1;
    
    // If it's an Arguments node with right child
    if (strcmp(argList->name, "Arguments") == 0) {
        int count = 1; // Count the right child
        
        // Count all nodes in the left branch
        Node* current = argList->left;
        while (current != NULL) {
            count++;
            if (strcmp(current->name, "Argument") == 0) break;
            current = current->left;
        }
        
        return count;
    }
    
    return 0;
}

// Add a function to find the best matching function based on argument types
int findMatchingFunction(char* name, Node* argList) {
    int bestMatch = -1;
    
    // Count arguments
    int argCount = 0;
    if (argList != NULL) {
        Node* argNode = argList;
        if (strcmp(argNode->name, "Argument") == 0) {
            argCount = 1;
        } else if (strcmp(argNode->name, "Arguments") == 0) {
            argCount = 2; // Start with 2 for the first pair
            
            // Traverse the nested Arguments nodes
            Node* current = argNode->left;
            while (current != NULL && strcmp(current->name, "Arguments") == 0) {
                argCount++;
                current = current->left;
            }
        }
    }
    
    // Collect argument types
    char* argTypes[10]; // Max 10 arguments
    int argTypesCount = 0;
    
    if (argList != NULL) {
        // Extract types from the argument nodes
        if (strcmp(argList->name, "Argument") == 0) {
            argTypes[0] = strdup(argList->left->type);
            argTypesCount = 1;
        } else if (strcmp(argList->name, "Arguments") == 0) {
            // The rightmost argument is in argList->right
            argTypes[0] = strdup(argList->right->type);
            argTypesCount = 1;
            
            // Traverse left branch to gather other arguments
            Node* current = argList->left;
            while (current != NULL) {
                if (strcmp(current->name, "Argument") == 0) {
                    argTypes[argTypesCount++] = strdup(current->left->type);
                    break;
                } else if (strcmp(current->name, "Arguments") == 0) {
                    argTypes[argTypesCount++] = strdup(current->right->type);
                    current = current->left;
                } else {
                    break;
                }
            }
        }
    }
    
    // Find functions that match the name and parameter count
    for (int i = 0; i < functionCount; i++) {
        if (strcmp(functionTable[i].name, name) == 0 && 
            functionTable[i].paramCount == argCount) {
            
            // Check parameter type compatibility
            int typeMatch = 1;
            for (int j = 0; j < argCount; j++) {
                char* paramType = functionTable[i].params[j].type;
                char* argType = argTypes[argTypesCount - j - 1]; // Arguments are in reverse order
                
                if (strcmp(paramType, argType) != 0 && 
                    !(strcmp(paramType, "float") == 0 && strcmp(argType, "int") == 0)) {
                    // Parameter type doesn't match and no implicit conversion possible
                    typeMatch = 0;
                    break;
                }
            }
            
            if (typeMatch) {
                bestMatch = i;
                break;
            }
        }
    }
    
    // Clean up
    for (int i = 0; i < argTypesCount; i++) {
        free(argTypes[i]);
    }
    
    return bestMatch;
}

// Add this function to evaluate binary expressions
int evaluateExpr(const char* op, int leftVal, int rightVal) {
    if (strcmp(op, "+") == 0) return leftVal + rightVal;
    if (strcmp(op, "-") == 0) return leftVal - rightVal;
    if (strcmp(op, "*") == 0) return leftVal * rightVal;
    if (strcmp(op, "/") == 0) return rightVal != 0 ? leftVal / rightVal : 0;
    if (strcmp(op, "%") == 0) return rightVal != 0 ? leftVal % rightVal : 0;
    if (strcmp(op, ">") == 0) return leftVal > rightVal;
    if (strcmp(op, "<") == 0) return leftVal < rightVal;
    if (strcmp(op, ">=") == 0) return leftVal >= rightVal;
    if (strcmp(op, "<=") == 0) return leftVal <= rightVal;
    if (strcmp(op, "==") == 0) return leftVal == rightVal;
    if (strcmp(op, "&&") == 0) return leftVal && rightVal;
    if (strcmp(op, "||") == 0) return leftVal || rightVal;
    return 0;
}

int main() {
    yyparse();
    return 0;
}

void yyerror(const char *msg) {
    printf("Semantic Error: %s\n", msg);
}

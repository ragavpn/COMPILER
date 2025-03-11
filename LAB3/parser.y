%{
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

// Add to your printTree function to include function table
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
%}

%left OR
%left AND
%left EQ
%left LT GT LE GE
%left PLUS MINUS
%left MULT DIV MOD
%right NOT
%right INCREMENT DECREMENT
%nonassoc UMINUS
%union {
    char *str;
    struct Node *node;
}

%token <str> IDENTIFIER NUMBER KEYWORD
%token IF ELSE WHILE FOR SWITCH CASE DEFAULT RETURN ASSIGN EQ PLUS MINUS MULT DIV MOD AND OR NOT LT GT LE GE INCREMENT DECREMENT
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA COLON LBRACKET RBRACKET
%token BREAK FUNCTION
%type <node> expression statement program block statements matched_stmt unmatched_stmt for_statement switch_statement case_statement 
%type <node> cases declaration condition increment_expr elif_chain elif_clause case_statements case_statement_item break_statement
%type <node> function_decl param_list param function_call arg_list

%%

program:
    block {
        rootNode = $1;
        // Print lexer table footer
        fprintf(stderr, "+-----------------+------------------+\n\n");
        printAST();
    }
    ;

block:
    LBRACE { currentScope++; } statements RBRACE { currentScope--; $$ = $3; }
    | LBRACE { currentScope++; } RBRACE { currentScope--; $$ = createNode("EmptyBlock", "block", NULL, NULL); }
    ;

statements:
    statements statement { $$ = createNode("Statements", "block", $1, $2); }
    | statements function_decl { $$ = createNode("Statements", "block", $1, $2); }
    | statement { $$ = $1; }
    | function_decl { $$ = $1; }
    ;

function_decl:
    KEYWORD IDENTIFIER LPAREN param_list RPAREN block {
        char* returnType = getTypeFromKeyword($1);
        
        // Very important: Add function to symbol table FIRST in global scope
        addSymbol($2, "function", 0, 0);
        
        // Then add to function table
        int funcIndex = addFunction($2, returnType);
        
        if (funcIndex >= 0) {
            // Process parameters from stored list
            for (int i = 0; i < paramCount; i++) {
                addParameter(funcIndex, paramNames[i], paramTypes[i]);
                free(paramNames[i]);
                free(paramTypes[i]);
            }
            paramCount = 0;
            
            $$ = createNode("Function", returnType, 
                           createNode($2, "function", $4, $6), NULL);
        } else {
            $$ = createNode("Error", "error", NULL, NULL);
        }
        free(returnType);
    }
    ;

param_list:
    param_list COMMA param { 
        $$ = createNode("Parameters", "params", $1, $3); 
    }
    | param { 
        $$ = $1; 
    }
    | /* empty */ { 
        $$ = NULL; 
    }
    ;

param:
    KEYWORD IDENTIFIER {
        char* type = getTypeFromKeyword($1);
        
        // Store parameter for later addition to function
        if (paramCount < 10) {
            paramNames[paramCount] = strdup($2);
            paramTypes[paramCount] = strdup(type);
            paramCount++;
        }
        
        // Add to symbol table with current scope (for body access)
        addSymbol($2, type, currentScope+1, 0);
        
        $$ = createNode("Parameter", type, createNode($2, type, NULL, NULL), NULL);
        free(type);
    }
    ;

statement:
    KEYWORD IDENTIFIER ASSIGN expression SEMICOLON {
        char* type = getTypeFromKeyword($1);
        if (isAssignmentCompatible(type, $4->type)) {
            addSymbol($2, type, currentScope, 0);
            $$ = createNode("Assignment", type, createNode($2, type, NULL, NULL), $4);
            $$->value = $4->value;
            symbolTable[lookupSymbol($2, currentScope)].value = $4->value;
        } else {
            $$ = createNode("Error", "error", NULL, NULL);
        }
        free(type);
    }
    | KEYWORD IDENTIFIER LBRACKET expression RBRACKET SEMICOLON {
        char* type = getTypeFromKeyword($1);
        // Store as array type and track dimensions
        addSymbol($2, "array", currentScope, 1);
        $$ = createNode("ArrayDeclaration", type, 
                       createNode($2, "array", NULL, NULL), $4);
        free(type);
    }
    | KEYWORD IDENTIFIER LBRACKET expression RBRACKET LBRACKET expression RBRACKET SEMICOLON {
        char* type = getTypeFromKeyword($1);
        // For 2D arrays, store dimensions count as 2
        addSymbol($2, "array", currentScope, 2);
        Node* dimensions = createNode("Dimensions", "int", $4, $7);
        $$ = createNode("ArrayDeclaration2D", type,
                       createNode($2, "array", NULL, NULL), dimensions);
        free(type);
    }
    | IDENTIFIER ASSIGN expression SEMICOLON {
        int index = lookupSymbol($1, currentScope);
        if (index == -1) {
            yyerror("Undeclared variable used");
            $$ = createNode("Error", "error", NULL, NULL);
        } else if (isAssignmentCompatible(symbolTable[index].type, $3->type)) {
            $$ = createNode("Assignment", symbolTable[index].type, 
                           createNode($1, symbolTable[index].type, NULL, NULL), $3);
            $$->value = $3->value;
            symbolTable[index].value = $3->value;
        } else {
            $$ = createNode("Error", "error", NULL, NULL);
        }
    }
    | IDENTIFIER LBRACKET expression RBRACKET ASSIGN expression SEMICOLON {
        int index = lookupSymbol($1, currentScope);
        if (index == -1 || strcmp(symbolTable[index].type, "array") != 0) {
            yyerror("Invalid array access");
            $$ = createNode("Error", "error", NULL, NULL);
        } else {
            $$ = createNode("ArrayAssignment", "int", 
                           createNode($1, "array", $3, NULL), $6);
        }
    }
    | IDENTIFIER LBRACKET expression RBRACKET LBRACKET expression RBRACKET ASSIGN expression SEMICOLON {
        int index = lookupSymbol($1, currentScope);
        if (index == -1 || strcmp(symbolTable[index].type, "array") != 0) {
            yyerror("Invalid array access");
            $$ = createNode("Error", "error", NULL, NULL);
        } else {
            Node* indices = createNode("Indices", "int", $3, $6);
            $$ = createNode("Array2DAssignment", "int",
                           createNode($1, "array", indices, NULL), $9);
        }
    }
    | matched_stmt { $$ = $1; }
    | unmatched_stmt { $$ = $1; }
    | for_statement { $$ = $1; }
    | switch_statement { $$ = $1; }
    | expression SEMICOLON { $$ = $1; }
    | KEYWORD IDENTIFIER SEMICOLON {
        if (strcmp($1, "return_RP") == 0) {
            int index = lookupSymbol($2, currentScope);
            if (index == -1) {
                printf("Semantic Error: Returning undeclared variable '%s'\n", $2);
                $$ = createNode("Return", "error", NULL, NULL);
            } else {
                $$ = createNode("Return", symbolTable[index].type, 
                               createNode($2, symbolTable[index].type, NULL, NULL), NULL);
            }
        } else {
            yyerror("Unexpected keyword in statement");
            $$ = createNode("Error", "error", NULL, NULL);
        }
    }
    | function_call SEMICOLON { $$ = $1; }
    ;

matched_stmt:
    KEYWORD LPAREN expression RPAREN matched_stmt KEYWORD matched_stmt {
        if (strcmp($1, "if_RP") == 0 && strcmp($6, "else_RP") == 0) {
            Node *if_node = createNode("If", "control", $3, $5);
            Node *else_node = createNode("Else", "control", NULL, $7);
            $$ = createNode("IfElse", "control", if_node, else_node);
        } else {
            yyerror("Expected 'if_RP' and 'else_RP' keywords");
            $$ = NULL;
        }
    }
    | KEYWORD LPAREN expression RPAREN matched_stmt elif_chain KEYWORD matched_stmt {
        if (strcmp($1, "if_RP") == 0 && strcmp($7, "else_RP") == 0) {
            Node *if_node = createNode("If", "control", $3, $5);
            // Add the elif chain in between
            Node *elif_else = createNode("ElifElse", "control", $6, $8);
            $$ = createNode("IfElifElse", "control", if_node, elif_else);
        } else {
            yyerror("Expected 'if_RP' and 'else_RP' keywords");
            $$ = NULL;
        }
    }
    | KEYWORD LPAREN expression RPAREN matched_stmt elif_chain {
        if (strcmp($1, "if_RP") == 0) {
            Node *if_node = createNode("If", "control", $3, $5);
            $$ = createNode("IfElif", "control", if_node, $6);
        } else {
            yyerror("Expected 'if_RP' keyword");
            $$ = NULL;
        }
    }
    | KEYWORD LPAREN expression RPAREN matched_stmt {
        if (strcmp($1, "while_RP") == 0) {
            $$ = createNode("While", "control", $3, $5);
        } else {
            yyerror("Expected 'while_RP' keyword");
            $$ = NULL;
        }
    }
    | block { $$ = $1; }
    ;

unmatched_stmt:
    KEYWORD LPAREN expression RPAREN statement {
        if (strcmp($1, "if_RP") == 0) {
            $$ = createNode("If", "control", $3, $5);
        } else if (strcmp($1, "while_RP") == 0) {
            $$ = createNode("While", "control", $3, $5);
        } else {
            yyerror("Expected 'if_RP' or 'while_RP' keyword");
            $$ = NULL;
        }
    }
    | KEYWORD LPAREN expression RPAREN matched_stmt KEYWORD unmatched_stmt {
        if (strcmp($1, "if_RP") == 0 && strcmp($6, "else_RP") == 0) {
            Node *if_node = createNode("If", "control", $3, $5);
            Node *else_node = createNode("Else", "control", NULL, $7);
            $$ = createNode("IfElse", "control", if_node, else_node);
        } else {
            yyerror("Expected 'if_RP' and 'else_RP' keywords");
            $$ = NULL;
        }
    }
    | KEYWORD LPAREN expression RPAREN matched_stmt elif_chain KEYWORD unmatched_stmt {
        if (strcmp($1, "if_RP") == 0 && strcmp($7, "else_RP") == 0) {
            Node *if_node = createNode("If", "control", $3, $5);
            Node *elif_else = createNode("ElifElse", "control", $6, $8);
            $$ = createNode("IfElifElse", "control", if_node, elif_else);
        } else {
            yyerror("Expected 'if_RP' and 'else_RP' keywords");
            $$ = NULL;
        }
    }
    | KEYWORD LPAREN error RPAREN statement {
        yyerror("Expected expression in if statement");
        $$ = NULL;
    }
    ;

for_statement:
    KEYWORD LPAREN declaration SEMICOLON condition SEMICOLON increment_expr RPAREN statement {
        if (strcmp($1, "for_RP") == 0) {
            $$ = createNode("For", "control", $3,
                createNode("ForBody", "control", $5,
                    createNode("ForIncr", "control", $7, $9)));
        } else {
            yyerror("Expected 'for_RP' keyword");
            $$ = NULL;
        }
    }
    ;

declaration:
    KEYWORD IDENTIFIER ASSIGN expression {
        char* type = getTypeFromKeyword($1);
        addSymbol($2, type, currentScope, 0);
        $$ = createNode("Initialize", type, createNode($2, type, NULL, NULL), $4);
        free(type);
    }
    ;

condition:
    expression { $$ = $1; }
    ;

increment_expr:
    IDENTIFIER INCREMENT {
        int index = lookupSymbol($1, currentScope);
        if (index != -1) {
            $$ = createNode("++", symbolTable[index].type, 
                createNode($1, symbolTable[index].type, NULL, NULL), NULL);
        } else {
            yyerror("Undeclared variable in increment");
            $$ = NULL;
        }
    }
    | IDENTIFIER DECREMENT {
        int index = lookupSymbol($1, currentScope);
        if (index != -1) {
            $$ = createNode("--", symbolTable[index].type,
                createNode($1, symbolTable[index].type, NULL, NULL), NULL);
        } else {
            yyerror("Undeclared variable in decrement");
            $$ = NULL;
        }
    }
    | expression { $$ = $1; }
    ;

switch_statement:
    KEYWORD LPAREN expression RPAREN LBRACE cases RBRACE {
        if (strcmp($1, "switch_RP") == 0) {
            $$ = createNode("Switch", "control", $3, $6);
        } else {
            yyerror("Expected 'switch_RP' keyword");
            $$ = NULL;
        }
    }
    ;

cases:
    cases case_statement { $$ = createNode("Cases", "block", $1, $2); }
    | case_statement { $$ = $1; }
    ;

case_statement:
    KEYWORD NUMBER COLON case_statements break_statement {
        if (strcmp($1, "case_RP") == 0) {
            Node* caseBody = createNode("CaseBody", "block", $4, $5);
            $$ = createNode("Case", "int", createNode($2, "int", NULL, NULL), caseBody);
        } else {
            yyerror("Expected 'case_RP' keyword");
            $$ = NULL;
        }
    }
    | KEYWORD COLON case_statements break_statement {
        if (strcmp($1, "default_RP") == 0) {
            Node* defaultBody = createNode("DefaultBody", "block", $3, $4);
            $$ = createNode("Default", "int", NULL, defaultBody);
        } else {
            yyerror("Expected 'default_RP' keyword");
            $$ = NULL;
        }
    }
    ;

case_statements:
    case_statements case_statement_item { 
        $$ = createNode("CaseStatements", "block", $1, $2); 
    }
    | case_statement_item { 
        $$ = $1; 
    }
    ;

case_statement_item:
    statement { $$ = $1; }
    ;

break_statement:
    KEYWORD SEMICOLON {
        if (strcmp($1, "break_RP") == 0) {
            $$ = createNode("Break", "control", NULL, NULL);
        } else {
            yyerror("Expected 'break_RP' keyword");
            $$ = NULL;
        }
    }
    ;

expression:
    expression PLUS expression {
        // If either operand is float, result must be float
        if (strcmp($1->type, "float") == 0 || strcmp($3->type, "float") == 0) {
            $$ = createNode("+", "float", $1, $3);
            // For float operations, we can approximate
            $$->value = (int)($1->value + $3->value);
        } else {
            $$ = createNode("+", "int", $1, $3);
            $$->value = evaluateExpr("+", $1->value, $3->value);
        }
    }
    | expression MINUS expression {
        // If either operand is float, result must be float
        if (strcmp($1->type, "float") == 0 || strcmp($3->type, "float") == 0) {
            $$ = createNode("-", "float", $1, $3);
            // For float operations, we can approximate
            $$->value = (int)($1->value - $3->value);
        } else {
            $$ = createNode("-", "int", $1, $3);
            $$->value = evaluateExpr("-", $1->value, $3->value);
        }
    }
    | expression MULT expression {
        // If either operand is float, result must be float
        if (strcmp($1->type, "float") == 0 || strcmp($3->type, "float") == 0) {
            $$ = createNode("*", "float", $1, $3);
            // For float operations, we can approximate
            $$->value = (int)(atof($1->name) * atof($3->name));
        } else {
            $$ = createNode("*", "int", $1, $3);
            $$->value = evaluateExpr("*", $1->value, $3->value);
        }
    }
    | expression DIV expression {
        // If either operand is float, result must be float
        if (strcmp($1->type, "float") == 0 || strcmp($3->type, "float") == 0) {
            $$ = createNode("/", "float", $1, $3);
            // For float operations, we can approximate
            $$->value = (int)(atof($1->name) / atof($3->name));
        } else {
            $$ = createNode("/", "int", $1, $3);
            $$->value = evaluateExpr("/", $1->value, $3->value);
        }
    }
    | expression MOD expression {
        // If either operand is float, result must be float
        if (strcmp($1->type, "float") == 0 || strcmp($3->type, "float") == 0) {
            $$ = createNode("%", "float", $1, $3);
            // For float operations, we need to convert to int first for modulo
            $$->value = (int)($1->value) % (int)($3->value);
        } else {
            $$ = createNode("%", "int", $1, $3);
            $$->value = evaluateExpr("%", $1->value, $3->value);
        }
    }
    | expression GT expression {
        $$ = createNode(">", "int", $1, $3);
        $$->value = ($1->value > $3->value);
    }
    | expression LT expression {
        $$ = createNode("<", "int", $1, $3);
        $$->value = ($1->value < $3->value);
    }
    | expression GE expression {
        $$ = createNode(">=", "int", $1, $3);
        $$->value = ($1->value >= $3->value);
    }
    | expression LE expression {
        $$ = createNode("<=", "int", $1, $3);
        $$->value = ($1->value <= $3->value);
    }
    | expression EQ expression {
        $$ = createNode("==", "int", $1, $3);
        $$->value = ($1->value == $3->value);
    }
    | expression AND expression {
        $$ = createNode("&&", "int", $1, $3);
        $$->value = ($1->value && $3->value);
    }
    | expression OR expression {
        $$ = createNode("||", "int", $1, $3);
        $$->value = ($1->value || $3->value);
    }
    | NOT expression {
        $$ = createNode("!", "int", $2, NULL);
        $$->value = (!$2->value);
    }
    | MINUS expression %prec UMINUS {
        $$ = createNode("unary-", "int", $2, NULL);
        $$->value = (-$2->value);
    }
    | IDENTIFIER {
        int index = lookupSymbol($1, currentScope);
        if (index == -1) {
            printf("Semantic Error: Undeclared variable '%s' used in expression\n", $1);
            $$ = createNode($1, "error", NULL, NULL);
        } else {
            $$ = createNode($1, symbolTable[index].type, NULL, NULL);
            $$->value = symbolTable[index].value;
        }
    }
    | NUMBER {
        // Check if number contains a decimal point
        if (strchr($1, '.') != NULL) {
            $$ = createNode($1, "float", NULL, NULL);
            $$->value = (int)atof($1); // Store approximate int value
        } else {
            $$ = createNode($1, "int", NULL, NULL);
            $$->value = atoi($1);
        }
    }
    | LPAREN expression RPAREN { 
        $$ = $2; 
    }
    | IDENTIFIER INCREMENT {
        int index = lookupSymbol($1, currentScope);
        if (index != -1) {
            $$ = createNode("++", symbolTable[index].type,
                createNode($1, symbolTable[index].type, NULL, NULL), NULL);
        } else {
            addSymbol($1, "int", currentScope, 0);
            $$ = createNode("++", "int", createNode($1, "int", NULL, NULL), NULL);
        }
    }
    | IDENTIFIER DECREMENT {
        int index = lookupSymbol($1, currentScope);
        if (index != -1) {
            $$ = createNode("--", symbolTable[index].type,
                createNode($1, symbolTable[index].type, NULL, NULL), NULL);
        } else {
            addSymbol($1, "int", currentScope, 0);
            $$ = createNode("--", "int", createNode($1, "int", NULL, NULL), NULL);
        }
    }
    | IDENTIFIER LBRACKET expression RBRACKET {
        int index = lookupSymbol($1, currentScope);
        if (index == -1 || strcmp(symbolTable[index].type, "array") != 0) {
            yyerror("Invalid array access in expression");
            $$ = createNode("error", "error", NULL, NULL);
        } else if (strcmp($3->type, "float") == 0) {
            printf("Semantic Error: Array index must be integer, not float\n");
            $$ = createNode("ArrayAccessError", "error", 
                           createNode($1, "array", $3, NULL), NULL);
        } else {
            $$ = createNode("ArrayAccess", "int", 
                           createNode($1, "array", $3, NULL), NULL);
        }
    }
    | IDENTIFIER LBRACKET expression RBRACKET LBRACKET expression RBRACKET {
        int index = lookupSymbol($1, currentScope);
        if (index == -1 || strcmp(symbolTable[index].type, "array") != 0) {
            yyerror("Invalid 2D array access in expression");
            $$ = createNode("error", "error", NULL, NULL);
        } else if (strcmp($3->type, "float") == 0 || strcmp($6->type, "float") == 0) {
            printf("Semantic Error: Array indices must be integer, not '%s' and '%s'\n", 
                   $3->type, $6->type);
            $$ = createNode("Array2DAccessError", "error", NULL, NULL);
        } else {
            Node* indices = createNode("Indices", "int", $3, $6);
            $$ = createNode("Array2DAccess", "int",
                           createNode($1, "array", indices, NULL), NULL);
        }
    }
    | function_call { $$ = $1; }
    ;

// New rules for elif chains and clauses
elif_chain:
    elif_chain elif_clause { $$ = createNode("ElifChain", "control", $1, $2); }
    | elif_clause { $$ = $1; }
    ;

elif_clause:
    KEYWORD LPAREN expression RPAREN matched_stmt {
        if (strcmp($1, "elif_RP") == 0) {
            $$ = createNode("Elif", "control", $3, $5);
        } else {
            yyerror("Expected 'elif_RP' keyword");
            $$ = NULL;
        }
    }
    ;

function_call:
    IDENTIFIER LPAREN arg_list RPAREN {
        int index = lookupSymbol($1, currentScope);
        
        if (index == -1) {
            printf("Semantic Error: Undefined function '%s'\n", $1);
            $$ = createNode("Error", "error", NULL, NULL);
        } else if (strcmp(symbolTable[index].type, "function") != 0) {
            printf("Semantic Error: '%s' is not a function\n", $1);
            $$ = createNode("Error", "error", NULL, NULL);
        } else {
            // Find best matching function based on argument types
            int funcIndex = findMatchingFunction($1, $3);
            
            if (funcIndex == -1) {
                printf("Semantic Error: No matching overloaded function '%s' found for given arguments\n", $1);
                $$ = createNode("Error", "error", NULL, NULL);
            } else {
                // Create a node that includes the function ID to disambiguate which overloaded version
                char functionIdStr[20];
                sprintf(functionIdStr, "%s#%d", $1, functionTable[funcIndex].id);
                
                $$ = createNode("Call", functionTable[funcIndex].returnType,
                               createNode(functionIdStr, "function", $3, NULL), NULL);
                
                printf("Function call resolved to: %s (ID: %d)\n", 
                       functionTable[funcIndex].name, functionTable[funcIndex].id);
            }
        }
    }
    ;

arg_list:
    arg_list COMMA expression { 
        $$ = createNode("Arguments", "args", $1, $3); 
    }
    | expression { 
        $$ = createNode("Argument", "arg", $1, NULL); 
    }
    | /* empty */ { 
        $$ = NULL; 
    }
    ;

%%

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
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int yylex();
void yyerror(const char *msg);

typedef struct Node {
    char *name;
    char *type; // Variable type (e.g., "int", "array", "function")
    int value;  // For numeric evaluation
    struct Node *left;
    struct Node *right;
} Node;

typedef struct Symbol {
    char *name;
    char *type; // e.g., "int", "float", "array", "function"
    int scope;
    int array_dimensions; // Number of dimensions for arrays
} Symbol;

Symbol symbolTable[100];
int symbolCount = 0;
int currentScope = 0;

Node *createNode(char *name, char *type, Node *left, Node *right) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->name = strdup(name);
    node->type = strdup(type);
    node->value = 0; // Default value
    node->left = left;
    node->right = right;
    return node;
}

void printTree(Node *root, int level) {
    if (root == NULL) return;
    for (int i = 0; i < level; i++) printf("  ");
    printf("└── %s (%s, %d)\n", root->name, root->type, root->value);
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

void addSymbol(char *name, char *type, int scope, int array_dimensions) {
    if (lookupSymbol(name, scope) != -1) {
        yyerror("Duplicate declaration");
    } else {
        symbolTable[symbolCount].name = strdup(name);
        symbolTable[symbolCount].type = strdup(type);
        symbolTable[symbolCount].scope = scope;
        symbolTable[symbolCount].array_dimensions = array_dimensions;
        symbolCount++;
    }
}

void printSymbolTable() {
    printf("\nSymbol Table:\n");
    printf("+----------------+----------------+--------+----------------+\n");
    printf("| Name           | Type           | Scope  | Array Dim      |\n");
    printf("+----------------+----------------+--------+----------------+\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("| %-14s | %-14s | %-6d | %-14d |\n",
               symbolTable[i].name,
               symbolTable[i].type,
               symbolTable[i].scope,
               symbolTable[i].array_dimensions);
    }
    printf("+----------------+----------------+--------+----------------+\n");
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
%token BREAK
%type <node> expression statement program block statements matched_stmt unmatched_stmt for_statement switch_statement case_statement 
%type <node> cases declaration condition increment_expr elif_chain elif_clause case_statements case_statement_item break_statement

%%

program:
    block {
        // Print lexer table footer
        fprintf(stderr, "+-----------------+------------------+\n\n");

        // Print parse tree
        printf("Parse Tree:\n");
        printTree($1, 0);
        printf("\n");

        // Print symbol table
        printSymbolTable();
    }
    ;

block:
    LBRACE { currentScope++; } statements RBRACE { currentScope--; $$ = $3; }
    ;

statements:
    statements statement { $$ = createNode("Statements", "block", $1, $2); }
    | statement { $$ = $1; }
    ;

statement:
    KEYWORD IDENTIFIER ASSIGN expression SEMICOLON {
        char* type = getTypeFromKeyword($1);
        addSymbol($2, type, currentScope, 0);
        $$ = createNode("Assignment", type, createNode($2, type, NULL, NULL), $4);
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
        } else {
            $$ = createNode("Assignment", symbolTable[index].type, 
                           createNode($1, symbolTable[index].type, NULL, NULL), $3);
        }
    }
    | IDENTIFIER LBRACKET expression RBRACKET ASSIGN expression SEMICOLON {
        int index = lookupSymbol($1, currentScope);
        if (index == -1 || strcmp(symbolTable[index].type, "array") != 0) {
            yyerror("Invalid array access");
        } else {
            $$ = createNode("ArrayAssignment", "int", 
                           createNode($1, "array", $3, NULL), $6);
        }
    }
    | IDENTIFIER LBRACKET expression RBRACKET LBRACKET expression RBRACKET ASSIGN expression SEMICOLON {
        int index = lookupSymbol($1, currentScope);
        if (index == -1 || strcmp(symbolTable[index].type, "array") != 0) {
            yyerror("Invalid array access");
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
        $$ = createNode("+", "int", $1, $3);
        $$->value = $1->value + $3->value;
    }
    | expression MINUS expression {
        $$ = createNode("-", "int", $1, $3);
        $$->value = $1->value - $3->value;
    }
    | expression MULT expression {
        $$ = createNode("*", "int", $1, $3);
        $$->value = $1->value * $3->value;
    }
    | expression DIV expression {
        $$ = createNode("/", "int", $1, $3);
        if ($3->value != 0) {
            $$->value = $1->value / $3->value;
        } else {
            yyerror("Division by zero");
            $$->value = 0;
        }
    }
    | expression MOD expression {
        $$ = createNode("%", "int", $1, $3);
        if ($3->value != 0) {
            $$->value = $1->value % $3->value;
        } else {
            yyerror("Modulo by zero");
            $$->value = 0;
        }
    }
    | expression GT expression {
        $$ = createNode(">", "int", $1, $3);
        $$->value = ($1->value > $3->value) ? 1 : 0;
    }
    | expression LT expression {
        $$ = createNode("<", "int", $1, $3);
        $$->value = ($1->value < $3->value) ? 1 : 0;
    }
    | expression GE expression {
        $$ = createNode(">=", "int", $1, $3);
        $$->value = ($1->value >= $3->value) ? 1 : 0;
    }
    | expression LE expression {
        $$ = createNode("<=", "int", $1, $3);
        $$->value = ($1->value <= $3->value) ? 1 : 0;
    }
    | expression EQ expression {
        $$ = createNode("==", "int", $1, $3);
        $$->value = ($1->value == $3->value) ? 1 : 0;
    }
    | expression AND expression {
        $$ = createNode("&&", "int", $1, $3);
        $$->value = ($1->value && $3->value) ? 1 : 0;
    }
    | expression OR expression {
        $$ = createNode("||", "int", $1, $3);
        $$->value = ($1->value || $3->value) ? 1 : 0;
    }
    | NOT expression {
        $$ = createNode("!", "int", $2, NULL);
        $$->value = !($2->value) ? 1 : 0;
    }
    | MINUS expression %prec UMINUS {
        $$ = createNode("unary-", "int", $2, NULL);
        $$->value = -($2->value);
    }
    | IDENTIFIER {
        int index = lookupSymbol($1, currentScope);
        if (index == -1) {
            // Instead of error, create a forward declaration
            addSymbol($1, "int", currentScope, 0);  // Default to int
            $$ = createNode($1, "int", NULL, NULL);
        } else {
            $$ = createNode($1, symbolTable[index].type, NULL, NULL);
        }
    }
    | NUMBER {
        $$ = createNode($1, "int", NULL, NULL);
        $$->value = atoi($1);
    }
    | LPAREN expression RPAREN { 
        $$ = $2; 
    }
    | IDENTIFIER ASSIGN expression {
        int index = lookupSymbol($1, currentScope);
        if (index == -1) {
            // Instead of error, create a forward declaration
            addSymbol($1, "int", currentScope, 0);  // Default to int
            index = lookupSymbol($1, currentScope);
        }
        $$ = createNode("Assignment", symbolTable[index].type, 
                       createNode($1, symbolTable[index].type, NULL, NULL), $3);
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
        } else {
            Node* indices = createNode("Indices", "int", $3, $6);
            $$ = createNode("Array2DAccess", "int",
                           createNode($1, "array", indices, NULL), NULL);
        }
    }
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

%%

int main() {
    yyparse();
    return 0;
}

void yyerror(const char *msg) {
    printf("Semantic Error: %s\n", msg);
}
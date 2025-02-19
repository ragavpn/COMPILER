%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex();
void yyerror(const char *msg);

typedef struct Node {
    char *name;
    struct Node *left;
    struct Node *right;
} Node;

typedef struct Symbol {
    char *name;
    char *type;
    int scope;
} Symbol;

Symbol symbolTable[100];
int symbolCount = 0;
int currentScope = 0;

Node *createNode(char *name, Node *left, Node *right) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->name = strdup(name);
    node->left = left;
    node->right = right;
    return node;
}

void printTree(Node *root, int level) {
    if (root == NULL) return;
    for (int i = 0; i < level; i++) printf("  ");
    printf("└── %s\n", root->name);
    printTree(root->left, level + 1);
    printTree(root->right, level + 1);
}

void addSymbol(char *name, char *type, int scope) {
    symbolTable[symbolCount].name = strdup(name);
    symbolTable[symbolCount].type = strdup(type);
    symbolTable[symbolCount].scope = scope;
    symbolCount++;
}

void printSymbolTable() {
    printf("\nSymbol Table:\n");
    printf("+----------------+----------------+--------+\n");
    printf("| Name           | Type           | Scope  |\n");
    printf("+----------------+----------------+--------+\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("| %-14s | %-14s | %-6d |\n", 
               symbolTable[i].name, 
               symbolTable[i].type, 
               symbolTable[i].scope);
    }
    printf("+----------------+----------------+--------+\n");
}

%}

%left OR
%left AND
%left EQ LT GT LE GE
%left PLUS MINUS
%left MULT DIV MOD
%right NOT
%nonassoc UMINUS

%union {
    char *str;
    struct Node *node;
}

%token <str> IDENTIFIER NUMBER KEYWORD
%token IF ELSE WHILE RETURN ASSIGN EQ PLUS MINUS MULT DIV MOD AND OR NOT LT GT LE GE
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA

%type <node> expression statement program block statements if_statement

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
    statements statement { $$ = createNode("Statements", $1, $2); }
    | statement { $$ = $1; }
    ;

statement:
    KEYWORD IDENTIFIER ASSIGN expression SEMICOLON {
        addSymbol($2, "variable", currentScope);
        $$ = createNode("Assignment", createNode($2, NULL, NULL), $4);
    }
    | IDENTIFIER ASSIGN expression SEMICOLON {
        $$ = createNode("Assignment", createNode($1, NULL, NULL), $3);
    }
    | if_statement { $$ = $1; }
    | expression SEMICOLON { $$ = $1; }
    ;

if_statement:
    KEYWORD LPAREN expression RPAREN block {
        if (strcmp($1, "if_RP") == 0) {
            $$ = createNode("If", $3, $5);
        } else {
            yyerror("Expected if_RP");
        }
    }
    ;

expression:
    expression PLUS expression { $$ = createNode("+", $1, $3); }
    | expression MINUS expression { $$ = createNode("-", $1, $3); }
    | expression MULT expression { $$ = createNode("*", $1, $3); }
    | expression DIV expression { $$ = createNode("/", $1, $3); }
    | expression MOD expression { $$ = createNode("%", $1, $3); }
    | expression EQ expression { $$ = createNode("==", $1, $3); }
    | expression LT expression { $$ = createNode("<", $1, $3); }
    | expression GT expression { $$ = createNode(">", $1, $3); }
    | expression LE expression { $$ = createNode("<=", $1, $3); }
    | expression GE expression { $$ = createNode(">=", $1, $3); }
    | expression AND expression { $$ = createNode("&&", $1, $3); }
    | expression OR expression { $$ = createNode("||", $1, $3); }
    | NOT expression { $$ = createNode("!", $2, NULL); }
    | MINUS expression %prec UMINUS { $$ = createNode("-", $2, NULL); }
    | IDENTIFIER { $$ = createNode($1, NULL, NULL); }
    | NUMBER { $$ = createNode($1, NULL, NULL); }
    | LPAREN expression RPAREN { $$ = $2; }
    ;

%%

int main() {
    yyparse();
    return 0;
}

void yyerror(const char *msg) {
    printf("Syntax Error: %s\n", msg);
}
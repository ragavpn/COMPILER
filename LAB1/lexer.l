%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ROLL_NUMBER "095"

// Initialize the table header flag
int header_printed = 0;
int error_count = 0;  // Add error counter

// Enhanced error reporting function
void report_error(const char* error_type, const char* lexeme, int line_no) {
    fprintf(stderr, "\nError at line %d: %s\n", line_no, error_type);
    fprintf(stderr, "Problematic token: '%s'\n", lexeme);
    error_count++;
}

// Add this debug function with improved formatting
void print_token(const char* lexeme, const char* category) {
    if (!header_printed) {
        printf("\nLexical Analysis Results:\n");
        printf("+-----------------+------------------+\n");
        printf("| Lexeme         | Category         |\n");
        printf("+-----------------+------------------+\n");
        header_printed = 1;
    }
    printf("| %-15s | %-16s |\n", lexeme, category);
}
%}

/* Add line counting capability */
%option yylineno

%%

"//"[^\n]*      { /* Ignore single line comments */ }
"/*"            { 
                    /* Handle unterminated multi-line comments */
                    int c;
                    while((c = input()) != EOF) {
                        if(c == '*') {
                            if((c = input()) == '/')
                                break;
                            unput(c);
                        }
                    }
                    if(c == EOF)
                        report_error("Unterminated multi-line comment", yytext, yylineno);
                }

"auto_RP"       { print_token(yytext, "AUTO"); }
"break_RP"      { print_token(yytext, "BREAK"); }
"case_RP"       { print_token(yytext, "CASE"); }
"char_RP"       { print_token(yytext, "CHAR"); }
"const_RP"      { print_token(yytext, "CONST"); }
"continue_RP"   { print_token(yytext, "CONTINUE"); }
"default_RP"    { print_token(yytext, "DEFAULT"); }
"do_RP"         { print_token(yytext, "DO"); }
"double_RP"     { print_token(yytext, "DOUBLE"); }
"else_RP"       { print_token(yytext, "ELSE"); }
"enum_RP"       { print_token(yytext, "ENUM"); }
"extern_RP"     { print_token(yytext, "EXTERN"); }
"float_RP"      { print_token(yytext, "FLOAT"); }
"for_RP"        { print_token(yytext, "FOR"); }
"goto_RP"       { print_token(yytext, "GOTO"); }
"if_RP"         { print_token(yytext, "IF"); }
"int_RP"        { print_token(yytext, "INT"); }
"long_RP"       { print_token(yytext, "LONG"); }
"register_RP"   { print_token(yytext, "REGISTER"); }
"return_RP"     { print_token(yytext, "RETURN"); }
"short_RP"      { print_token(yytext, "SHORT"); }
"signed_RP"     { print_token(yytext, "SIGNED"); }
"sizeof_RP"     { print_token(yytext, "SIZEOF"); }
"static_RP"     { print_token(yytext, "STATIC"); }
"struct_RP"     { print_token(yytext, "STRUCT"); }
"switch_RP"     { print_token(yytext, "SWITCH"); }
"typedef_RP"    { print_token(yytext, "TYPEDEF"); }
"union_RP"      { print_token(yytext, "UNION"); }
"unsigned_RP"   { print_token(yytext, "UNSIGNED"); }
"void_RP"       { print_token(yytext, "VOID"); }
"volatile_RP"   { print_token(yytext, "VOLATILE"); }
"while_RP"      { print_token(yytext, "WHILE"); }
"main_RP"       { print_token(yytext, "MAIN"); }

"true_RP"       { print_token(yytext, "TRUE"); }
"false_RP"      { print_token(yytext, "FALSE"); }

"095"[a-zA-Z][a-zA-Z0-9]*  { print_token(yytext, "IDENTIFIER"); }

[0-9]+          { print_token(yytext, "NUMBER"); }

[0-9]+\.[0-9]+  { print_token(yytext, "FLOAT_VALUE"); }

\"              {   /* Handle string literals with error checking */
                    char string_buf[1000];
                    int i = 0;
                    int c;
                    
                    while((c = input()) != EOF && c != '"') {
                        if(c == '\n') {
                            report_error("Unterminated string literal", string_buf, yylineno);
                            break;
                        }
                        if(i < sizeof(string_buf)-1)
                            string_buf[i++] = c;
                        else {
                            report_error("String literal too long", string_buf, yylineno);
                            break;
                        }
                    }
                    if(c == EOF)
                        report_error("Unterminated string literal", string_buf, yylineno);
                    else {
                        string_buf[i] = '\0';
                        print_token(string_buf, "STRING");
                    }
                }

\+              { print_token(yytext, "PLUS"); }
\-              { print_token(yytext, "MINUS"); }
\*              { print_token(yytext, "MULT"); }
\/              { print_token(yytext, "DIV"); }
\%              { print_token(yytext, "MOD"); }
\=              { print_token(yytext, "ASSIGN"); }
\==             { print_token(yytext, "EQ"); }
\!=             { print_token(yytext, "NEQ"); }
\>              { print_token(yytext, "GT"); }
\<              { print_token(yytext, "LT"); }
\>=             { print_token(yytext, "GE"); }
\<=             { print_token(yytext, "LE"); }
\&&             { print_token(yytext, "AND"); }
\|\|            { print_token(yytext, "OR"); }
\!              { print_token(yytext, "NOT"); }
\++             { print_token(yytext, "INCREMENT"); }
\--             { print_token(yytext, "DECREMENT"); }

\(              { print_token(yytext, "LPAREN"); }
\)              { print_token(yytext, "RPAREN"); }
\{              { print_token(yytext, "LBRACE"); }
\}              { print_token(yytext, "RBRACE"); }
\[              { print_token(yytext, "LBRACK"); }
\]              { print_token(yytext, "RBRACK"); }
\;              { print_token(yytext, "SEMICOLON"); }
\:              { print_token(yytext, "COLON"); }
\,              { print_token(yytext, "COMMA"); }
\.              { print_token(yytext, "DOT"); }

[ \t\n]+        { /* Ignore spaces, tabs, and newlines */ }

095[0-9][a-zA-Z0-9_]*  { 
    report_error("Invalid identifier - digit after 095", yytext, yylineno);
}

[a-zA-Z0-9_][a-zA-Z0-9_]* { 
    report_error("Invalid identifier - missing '095' prefix", yytext, yylineno);
}

.               { 
    report_error("Invalid character", yytext, yylineno);
}

%%

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    yyin = file;
    yylex();

    // Print table footer
    if (header_printed) {
        printf("+-----------------+------------------+\n");
    }

    // Print error summary
    if (error_count > 0) {
        fprintf(stderr, "\nLexical Analysis completed with %d error(s)\n", error_count);
    } else {
        printf("\nLexical Analysis completed successfully with no errors\n");
    }

    fclose(file);
    return error_count > 0 ? 1 : 0;  // Return error status
}

int yywrap() {
    return 1;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
    exit(1);
}
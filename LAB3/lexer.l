%{
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initialize the table header flag
int header_printed = 0;

// Debug function with improved formatting
void print_token(const char* token_type, const char* value) {
    if (!header_printed) {
        fprintf(stderr, "\nLexical Analysis Results:\n");
        fprintf(stderr, "+-----------------+------------------+\n");
        fprintf(stderr, "| Token Type      | Value           |\n");
        fprintf(stderr, "+-----------------+------------------+\n");
        header_printed = 1;
    }
    fprintf(stderr, "| %-15s | %-16s |\n", token_type, value);
}
%}

%%
auto_RP|break_RP|case_RP|char_RP|const_RP|continue_RP|default_RP|do_RP|double_RP|else_RP|elif_RP|enum_RP|extern_RP|float_RP|for_RP|goto_RP|if_RP|int_RP|long_RP|register_RP|return_RP|short_RP|signed_RP|sizeof_RP|static_RP|struct_RP|switch_RP|typedef_RP|union_RP|unsigned_RP|void_RP|volatile_RP|while_RP {
    print_token("KEYWORD", yytext);
    yylval.str = strdup(yytext);
    return KEYWORD;
}
"095"[a-zA-Z_][a-zA-Z0-9_]* {
    print_token("IDENTIFIER", yytext);
    yylval.str = strdup(yytext);
    return IDENTIFIER;
}
[0-9]+(\.[0-9]+)? {
    print_token("NUMBER", yytext);
    yylval.str = strdup(yytext);
    return NUMBER;
}
"="     { print_token("ASSIGN", "="); return ASSIGN; }
"=="    { print_token("EQ", "=="); return EQ; }
"+"     { print_token("PLUS", "+"); return PLUS; }
"-"     { print_token("MINUS", "-"); return MINUS; }
"*"     { print_token("MULT", "*"); return MULT; }
"/"     { print_token("DIV", "/"); return DIV; }
"%"     { print_token("MOD", "%"); return MOD; }
"&&"    { print_token("AND", "&&"); return AND; }
"||"    { print_token("OR", "||"); return OR; }
"!"     { print_token("NOT", "!"); return NOT; }
"<"     { print_token("LT", "<"); return LT; }
">"     { print_token("GT", ">"); return GT; }
"<="    { print_token("LE", "<="); return LE; }
">="    { print_token("GE", ">="); return GE; }
"("     { print_token("LPAREN", "("); return LPAREN; }
")"     { print_token("RPAREN", ")"); return RPAREN; }
"{"     { print_token("LBRACE", "{"); return LBRACE; }
"}"     { print_token("RBRACE", "}"); return RBRACE; }
";"     { print_token("SEMICOLON", ";"); return SEMICOLON; }
","     { print_token("COMMA", ","); return COMMA; }
":"     { print_token("COLON", ":"); return COLON; }
"["     { print_token("LBRACKET", "["); return LBRACKET; }
"]"     { print_token("RBRACKET", "]"); return RBRACKET; }
"++"    { print_token("INCREMENT", "++"); return INCREMENT; }
"--"    { print_token("DECREMENT", "--"); return DECREMENT; }
[ \t\r\n]+    { /* Ignore whitespace */ }
.             { printf("Unexpected character: %s\n", yytext); }
%%

int yywrap() {
    return 1;
}


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

#line 164 "parser.tab.c"

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
  YYSYMBOL_YYACCEPT = 41,                  /* $accept  */
  YYSYMBOL_program = 42,                   /* program  */
  YYSYMBOL_block = 43,                     /* block  */
  YYSYMBOL_44_1 = 44,                      /* $@1  */
  YYSYMBOL_statements = 45,                /* statements  */
  YYSYMBOL_statement = 46,                 /* statement  */
  YYSYMBOL_matched_stmt = 47,              /* matched_stmt  */
  YYSYMBOL_unmatched_stmt = 48,            /* unmatched_stmt  */
  YYSYMBOL_for_statement = 49,             /* for_statement  */
  YYSYMBOL_declaration = 50,               /* declaration  */
  YYSYMBOL_condition = 51,                 /* condition  */
  YYSYMBOL_increment_expr = 52,            /* increment_expr  */
  YYSYMBOL_switch_statement = 53,          /* switch_statement  */
  YYSYMBOL_cases = 54,                     /* cases  */
  YYSYMBOL_case_statement = 55,            /* case_statement  */
  YYSYMBOL_case_statements = 56,           /* case_statements  */
  YYSYMBOL_case_statement_item = 57,       /* case_statement_item  */
  YYSYMBOL_break_statement = 58,           /* break_statement  */
  YYSYMBOL_expression = 59,                /* expression  */
  YYSYMBOL_elif_chain = 60,                /* elif_chain  */
  YYSYMBOL_elif_clause = 61                /* elif_clause  */
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
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   514

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  41
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  21
/* YYNRULES -- Number of rules.  */
#define YYNRULES  66
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  156

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   295


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
      35,    36,    37,    38,    39,    40
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   118,   118,   133,   133,   137,   138,   142,   148,   156,
     165,   174,   183,   193,   194,   195,   196,   197,   201,   211,
     222,   231,   239,   243,   253,   263,   273,   280,   293,   302,
     306,   316,   326,   330,   341,   342,   346,   355,   367,   370,
     376,   380,   391,   395,   399,   403,   412,   421,   425,   429,
     433,   437,   441,   445,   449,   453,   457,   467,   471,   474,
     484,   494,   504,   514,   529,   530,   534
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
  "COMMA", "COLON", "LBRACKET", "RBRACKET", "BREAK", "$accept", "program",
  "block", "$@1", "statements", "statement", "matched_stmt",
  "unmatched_stmt", "for_statement", "declaration", "condition",
  "increment_expr", "switch_statement", "cases", "case_statement",
  "case_statements", "case_statement_item", "break_statement",
  "expression", "elif_chain", "elif_clause", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-116)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-32)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -23,  -116,    24,  -116,    95,  -116,   124,  -116,    22,   128,
     128,   128,  -116,    46,  -116,  -116,  -116,  -116,  -116,   263,
     128,  -116,  -116,   128,   -13,    80,   238,  -116,  -116,   363,
    -116,  -116,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   128,   128,   128,  -116,   283,   149,   128,   128,   -19,
      50,    21,   380,   128,   128,  -116,   486,    86,    86,  -116,
    -116,  -116,   474,   461,   239,   239,   239,   239,  -116,     8,
     303,   164,    95,    42,   128,   112,   448,   179,   128,   128,
    -116,   -27,  -116,   128,    25,   448,    55,  -116,    59,    29,
     323,   194,  -116,   128,   448,   133,     2,    13,  -116,    60,
      66,  -116,   128,  -116,    88,   209,   240,    43,   448,    71,
      95,  -116,  -116,    64,   128,  -116,  -116,    60,  -116,   224,
     128,    75,    82,    93,    95,    95,  -116,   117,  -116,    58,
     397,  -116,  -116,  -116,   343,  -116,  -116,   117,    10,  -116,
    -116,   414,    11,  -116,  -116,  -116,    95,    81,  -116,   128,
     431,    11,   125,    96,   134,    96
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     3,     0,     2,     0,     1,    56,    57,     0,     0,
       0,     0,    22,     0,     6,    13,    14,    15,    16,     0,
       0,    60,    61,     0,     0,     0,    56,    55,    54,     0,
       4,     5,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    17,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    58,    51,    42,    43,    44,
      45,    46,    52,    53,    48,    47,    50,    49,    10,    62,
       0,     0,     0,     0,     0,     0,    59,     0,     0,     0,
       7,     0,    26,     0,     0,    29,     3,    23,    13,    62,
       0,     0,     8,     0,    28,     0,     0,     0,    35,     0,
      20,    65,     0,    11,    63,     0,    56,     0,    32,     0,
       0,    33,    34,     0,     0,    18,    24,     0,    64,     0,
       0,     0,    60,    61,     0,     0,    40,     0,    39,     0,
       0,    19,    25,    63,     0,     9,    27,     0,     0,    38,
      37,     0,     0,    12,    36,    41,     0,     0,    66,     0,
       0,     0,    21,     0,    20,     0
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -116,  -116,   147,  -116,  -116,    -4,   -59,   -96,  -116,  -116,
    -116,  -116,  -116,  -116,    37,    30,  -115,    12,    -5,     4,
     -99
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     2,    12,     4,    13,   126,    15,    16,    17,    51,
      84,   107,    18,    97,    98,   127,   128,   140,    19,   100,
     101
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      14,   118,    47,   116,    27,    28,    29,   109,    92,    31,
       1,    93,   139,    72,    24,    45,    88,   147,    46,    96,
      52,   132,   139,    78,     5,    48,    24,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,   110,
     115,    25,    70,    71,     1,   145,    79,   111,    76,    77,
       6,     7,     8,    25,    73,   118,    74,    83,   131,    49,
      95,    96,    26,     7,     9,    99,   113,   102,    82,    85,
      10,    87,   117,    90,    91,   124,     9,    11,    94,     1,
      30,    49,    10,   148,    26,     7,    50,    88,   105,    11,
     108,   114,   152,     1,   115,   129,   131,   119,     9,     6,
       7,     8,   147,   120,    10,    35,    36,    37,   125,   130,
     135,    11,   149,     9,   -30,   134,     6,     7,     8,    10,
     136,     6,     7,   138,   141,   -31,    11,   114,     1,     1,
       9,   153,    26,     7,   112,     9,    10,   106,     7,    20,
     155,    10,    87,    11,   150,    86,     9,     3,    11,   144,
       1,     9,    10,    21,    22,   137,   154,    10,     0,    11,
       0,     0,    23,     0,    11,    32,    33,    34,    35,    36,
      37,    38,    39,     0,    40,    41,    42,    43,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    69,    40,
      41,    42,    43,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    81,    40,    41,    42,    43,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    89,    40,
      41,    42,    43,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,   104,    40,    41,    42,    43,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,   121,    40,
      41,    42,    43,    53,     0,    53,    33,    34,    35,    36,
      37,     0,     0,   133,     0,     0,     0,    21,    22,   122,
     123,     0,     0,     0,     0,     0,    54,     0,    54,    32,
      33,    34,    35,    36,    37,    38,    39,     0,    40,    41,
      42,    43,     0,     0,     0,     0,     0,     0,    44,    32,
      33,    34,    35,    36,    37,    38,    39,     0,    40,    41,
      42,    43,     0,     0,     0,     0,     0,     0,    68,    32,
      33,    34,    35,    36,    37,    38,    39,     0,    40,    41,
      42,    43,     0,     0,     0,     0,     0,     0,    80,    32,
      33,    34,    35,    36,    37,    38,    39,     0,    40,    41,
      42,    43,     0,     0,     0,     0,     0,     0,   103,    32,
      33,    34,    35,    36,    37,    38,    39,     0,    40,    41,
      42,    43,     0,     0,     0,     0,     0,     0,   143,    32,
      33,    34,    35,    36,    37,    38,    39,     0,    40,    41,
      42,    43,     0,     0,     0,    55,    32,    33,    34,    35,
      36,    37,    38,    39,     0,    40,    41,    42,    43,     0,
       0,     0,    75,    32,    33,    34,    35,    36,    37,    38,
      39,     0,    40,    41,    42,    43,     0,     0,     0,   142,
      32,    33,    34,    35,    36,    37,    38,    39,     0,    40,
      41,    42,    43,     0,     0,     0,   146,    32,    33,    34,
      35,    36,    37,    38,    39,     0,    40,    41,    42,    43,
       0,     0,     0,   151,    32,    33,    34,    35,    36,    37,
      38,    39,     0,    40,    41,    42,    43,    32,    33,    34,
      35,    36,    37,    38,     0,     0,    40,    41,    42,    43,
      32,    33,    34,    35,    36,    37,     0,     0,     0,    40,
      41,    42,    43,    33,    34,    35,    36,    37,     0,     0,
       0,    40,    41,    42,    43
};

static const yytype_int16 yycheck[] =
{
       4,   100,    15,    99,     9,    10,    11,     5,    35,    13,
      33,    38,   127,    32,     4,    20,    75,     6,    23,     6,
      25,   117,   137,    15,     0,    38,     4,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    37,
      99,    31,    47,    48,    33,    35,    38,    34,    53,    54,
       4,     5,     6,    31,     4,   154,    35,    15,   117,     1,
      35,     6,     4,     5,    18,     6,     6,    38,    72,    74,
      24,    75,     6,    78,    79,    32,    18,    31,    83,    33,
      34,     1,    24,   142,     4,     5,     6,   146,    93,    31,
      95,    31,   151,    33,   153,    31,   155,   102,    18,     4,
       5,     6,     6,    15,    24,    19,    20,    21,    37,   114,
      35,    31,    31,    18,    32,   120,     4,     5,     6,    24,
     124,     4,     5,     6,   129,    32,    31,    31,    33,    33,
      18,     6,     4,     5,    97,    18,    24,     4,     5,    15,
       6,    24,   146,    31,   149,    33,    18,     0,    31,   137,
      33,    18,    24,    29,    30,   125,   152,    24,    -1,    31,
      -1,    -1,    38,    -1,    31,    16,    17,    18,    19,    20,
      21,    22,    23,    -1,    25,    26,    27,    28,    -1,    -1,
      16,    17,    18,    19,    20,    21,    22,    23,    39,    25,
      26,    27,    28,    -1,    -1,    16,    17,    18,    19,    20,
      21,    22,    23,    39,    25,    26,    27,    28,    -1,    -1,
      16,    17,    18,    19,    20,    21,    22,    23,    39,    25,
      26,    27,    28,    -1,    -1,    16,    17,    18,    19,    20,
      21,    22,    23,    39,    25,    26,    27,    28,    -1,    -1,
      16,    17,    18,    19,    20,    21,    22,    23,    39,    25,
      26,    27,    28,    15,    -1,    15,    17,    18,    19,    20,
      21,    -1,    -1,    39,    -1,    -1,    -1,    29,    30,    29,
      30,    -1,    -1,    -1,    -1,    -1,    38,    -1,    38,    16,
      17,    18,    19,    20,    21,    22,    23,    -1,    25,    26,
      27,    28,    -1,    -1,    -1,    -1,    -1,    -1,    35,    16,
      17,    18,    19,    20,    21,    22,    23,    -1,    25,    26,
      27,    28,    -1,    -1,    -1,    -1,    -1,    -1,    35,    16,
      17,    18,    19,    20,    21,    22,    23,    -1,    25,    26,
      27,    28,    -1,    -1,    -1,    -1,    -1,    -1,    35,    16,
      17,    18,    19,    20,    21,    22,    23,    -1,    25,    26,
      27,    28,    -1,    -1,    -1,    -1,    -1,    -1,    35,    16,
      17,    18,    19,    20,    21,    22,    23,    -1,    25,    26,
      27,    28,    -1,    -1,    -1,    -1,    -1,    -1,    35,    16,
      17,    18,    19,    20,    21,    22,    23,    -1,    25,    26,
      27,    28,    -1,    -1,    -1,    32,    16,    17,    18,    19,
      20,    21,    22,    23,    -1,    25,    26,    27,    28,    -1,
      -1,    -1,    32,    16,    17,    18,    19,    20,    21,    22,
      23,    -1,    25,    26,    27,    28,    -1,    -1,    -1,    32,
      16,    17,    18,    19,    20,    21,    22,    23,    -1,    25,
      26,    27,    28,    -1,    -1,    -1,    32,    16,    17,    18,
      19,    20,    21,    22,    23,    -1,    25,    26,    27,    28,
      -1,    -1,    -1,    32,    16,    17,    18,    19,    20,    21,
      22,    23,    -1,    25,    26,    27,    28,    16,    17,    18,
      19,    20,    21,    22,    -1,    -1,    25,    26,    27,    28,
      16,    17,    18,    19,    20,    21,    -1,    -1,    -1,    25,
      26,    27,    28,    17,    18,    19,    20,    21,    -1,    -1,
      -1,    25,    26,    27,    28
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    33,    42,    43,    44,     0,     4,     5,     6,    18,
      24,    31,    43,    45,    46,    47,    48,    49,    53,    59,
      15,    29,    30,    38,     4,    31,     4,    59,    59,    59,
      34,    46,    16,    17,    18,    19,    20,    21,    22,    23,
      25,    26,    27,    28,    35,    59,    59,    15,    38,     1,
       6,    50,    59,    15,    38,    32,    59,    59,    59,    59,
      59,    59,    59,    59,    59,    59,    59,    59,    35,    39,
      59,    59,    32,     4,    35,    32,    59,    59,    15,    38,
      35,    39,    46,    15,    51,    59,    33,    46,    47,    39,
      59,    59,    35,    38,    59,    35,     6,    54,    55,     6,
      60,    61,    38,    35,    39,    59,     4,    52,    59,     5,
      37,    34,    55,     6,    31,    47,    48,     6,    61,    59,
      15,    39,    29,    30,    32,    37,    46,    56,    57,    31,
      59,    47,    48,    39,    59,    35,    46,    56,     6,    57,
      58,    59,    32,    35,    58,    35,    32,     6,    47,    31,
      59,    32,    47,     6,    60,     6
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    41,    42,    44,    43,    45,    45,    46,    46,    46,
      46,    46,    46,    46,    46,    46,    46,    46,    47,    47,
      47,    47,    47,    48,    48,    48,    48,    49,    50,    51,
      52,    52,    52,    53,    54,    54,    55,    55,    56,    56,
      57,    58,    59,    59,    59,    59,    59,    59,    59,    59,
      59,    59,    59,    59,    59,    59,    59,    59,    59,    59,
      59,    59,    59,    59,    60,    60,    61
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     4,     2,     1,     5,     6,     9,
       4,     7,    10,     1,     1,     1,     1,     2,     7,     8,
       6,     5,     1,     5,     7,     8,     5,     9,     4,     1,
       2,     2,     1,     7,     2,     1,     5,     4,     2,     1,
       1,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     2,     1,     1,     3,     3,
       2,     2,     4,     7,     2,     1,     5
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
#line 118 "parser.y"
          {
        // Print lexer table footer
        fprintf(stderr, "+-----------------+------------------+\n\n");

        // Print parse tree
        printf("Parse Tree:\n");
        printTree((yyvsp[0].node), 0);
        printf("\n");

        // Print symbol table
        printSymbolTable();
    }
#line 1391 "parser.tab.c"
    break;

  case 3: /* $@1: %empty  */
#line 133 "parser.y"
           { currentScope++; }
#line 1397 "parser.tab.c"
    break;

  case 4: /* block: LBRACE $@1 statements RBRACE  */
#line 133 "parser.y"
                                                 { currentScope--; (yyval.node) = (yyvsp[-1].node); }
#line 1403 "parser.tab.c"
    break;

  case 5: /* statements: statements statement  */
#line 137 "parser.y"
                         { (yyval.node) = createNode("Statements", "block", (yyvsp[-1].node), (yyvsp[0].node)); }
#line 1409 "parser.tab.c"
    break;

  case 6: /* statements: statement  */
#line 138 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 1415 "parser.tab.c"
    break;

  case 7: /* statement: KEYWORD IDENTIFIER ASSIGN expression SEMICOLON  */
#line 142 "parser.y"
                                                   {
        char* type = getTypeFromKeyword((yyvsp[-4].str));
        addSymbol((yyvsp[-3].str), type, currentScope, 0);
        (yyval.node) = createNode("Assignment", type, createNode((yyvsp[-3].str), type, NULL, NULL), (yyvsp[-1].node));
        free(type);
    }
#line 1426 "parser.tab.c"
    break;

  case 8: /* statement: KEYWORD IDENTIFIER LBRACKET expression RBRACKET SEMICOLON  */
#line 148 "parser.y"
                                                                {
        char* type = getTypeFromKeyword((yyvsp[-5].str));
        // Store as array type and track dimensions
        addSymbol((yyvsp[-4].str), "array", currentScope, 1);
        (yyval.node) = createNode("ArrayDeclaration", type, 
                       createNode((yyvsp[-4].str), "array", NULL, NULL), (yyvsp[-2].node));
        free(type);
    }
#line 1439 "parser.tab.c"
    break;

  case 9: /* statement: KEYWORD IDENTIFIER LBRACKET expression RBRACKET LBRACKET expression RBRACKET SEMICOLON  */
#line 156 "parser.y"
                                                                                             {
        char* type = getTypeFromKeyword((yyvsp[-8].str));
        // For 2D arrays, store dimensions count as 2
        addSymbol((yyvsp[-7].str), "array", currentScope, 2);
        Node* dimensions = createNode("Dimensions", "int", (yyvsp[-5].node), (yyvsp[-2].node));
        (yyval.node) = createNode("ArrayDeclaration2D", type,
                       createNode((yyvsp[-7].str), "array", NULL, NULL), dimensions);
        free(type);
    }
#line 1453 "parser.tab.c"
    break;

  case 10: /* statement: IDENTIFIER ASSIGN expression SEMICOLON  */
#line 165 "parser.y"
                                             {
        int index = lookupSymbol((yyvsp[-3].str), currentScope);
        if (index == -1) {
            yyerror("Undeclared variable used");
        } else {
            (yyval.node) = createNode("Assignment", symbolTable[index].type, 
                           createNode((yyvsp[-3].str), symbolTable[index].type, NULL, NULL), (yyvsp[-1].node));
        }
    }
#line 1467 "parser.tab.c"
    break;

  case 11: /* statement: IDENTIFIER LBRACKET expression RBRACKET ASSIGN expression SEMICOLON  */
#line 174 "parser.y"
                                                                          {
        int index = lookupSymbol((yyvsp[-6].str), currentScope);
        if (index == -1 || strcmp(symbolTable[index].type, "array") != 0) {
            yyerror("Invalid array access");
        } else {
            (yyval.node) = createNode("ArrayAssignment", "int", 
                           createNode((yyvsp[-6].str), "array", (yyvsp[-4].node), NULL), (yyvsp[-1].node));
        }
    }
#line 1481 "parser.tab.c"
    break;

  case 12: /* statement: IDENTIFIER LBRACKET expression RBRACKET LBRACKET expression RBRACKET ASSIGN expression SEMICOLON  */
#line 183 "parser.y"
                                                                                                       {
        int index = lookupSymbol((yyvsp[-9].str), currentScope);
        if (index == -1 || strcmp(symbolTable[index].type, "array") != 0) {
            yyerror("Invalid array access");
        } else {
            Node* indices = createNode("Indices", "int", (yyvsp[-7].node), (yyvsp[-4].node));
            (yyval.node) = createNode("Array2DAssignment", "int",
                           createNode((yyvsp[-9].str), "array", indices, NULL), (yyvsp[-1].node));
        }
    }
#line 1496 "parser.tab.c"
    break;

  case 13: /* statement: matched_stmt  */
#line 193 "parser.y"
                   { (yyval.node) = (yyvsp[0].node); }
#line 1502 "parser.tab.c"
    break;

  case 14: /* statement: unmatched_stmt  */
#line 194 "parser.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 1508 "parser.tab.c"
    break;

  case 15: /* statement: for_statement  */
#line 195 "parser.y"
                    { (yyval.node) = (yyvsp[0].node); }
#line 1514 "parser.tab.c"
    break;

  case 16: /* statement: switch_statement  */
#line 196 "parser.y"
                       { (yyval.node) = (yyvsp[0].node); }
#line 1520 "parser.tab.c"
    break;

  case 17: /* statement: expression SEMICOLON  */
#line 197 "parser.y"
                           { (yyval.node) = (yyvsp[-1].node); }
#line 1526 "parser.tab.c"
    break;

  case 18: /* matched_stmt: KEYWORD LPAREN expression RPAREN matched_stmt KEYWORD matched_stmt  */
#line 201 "parser.y"
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
#line 1541 "parser.tab.c"
    break;

  case 19: /* matched_stmt: KEYWORD LPAREN expression RPAREN matched_stmt elif_chain KEYWORD matched_stmt  */
#line 211 "parser.y"
                                                                                    {
        if (strcmp((yyvsp[-7].str), "if_RP") == 0 && strcmp((yyvsp[-1].str), "else_RP") == 0) {
            Node *if_node = createNode("If", "control", (yyvsp[-5].node), (yyvsp[-3].node));
            // Add the elif chain in between
            Node *elif_else = createNode("ElifElse", "control", (yyvsp[-2].node), (yyvsp[0].node));
            (yyval.node) = createNode("IfElifElse", "control", if_node, elif_else);
        } else {
            yyerror("Expected 'if_RP' and 'else_RP' keywords");
            (yyval.node) = NULL;
        }
    }
#line 1557 "parser.tab.c"
    break;

  case 20: /* matched_stmt: KEYWORD LPAREN expression RPAREN matched_stmt elif_chain  */
#line 222 "parser.y"
                                                               {
        if (strcmp((yyvsp[-5].str), "if_RP") == 0) {
            Node *if_node = createNode("If", "control", (yyvsp[-3].node), (yyvsp[-1].node));
            (yyval.node) = createNode("IfElif", "control", if_node, (yyvsp[0].node));
        } else {
            yyerror("Expected 'if_RP' keyword");
            (yyval.node) = NULL;
        }
    }
#line 1571 "parser.tab.c"
    break;

  case 21: /* matched_stmt: KEYWORD LPAREN expression RPAREN matched_stmt  */
#line 231 "parser.y"
                                                    {
        if (strcmp((yyvsp[-4].str), "while_RP") == 0) {
            (yyval.node) = createNode("While", "control", (yyvsp[-2].node), (yyvsp[0].node));
        } else {
            yyerror("Expected 'while_RP' keyword");
            (yyval.node) = NULL;
        }
    }
#line 1584 "parser.tab.c"
    break;

  case 22: /* matched_stmt: block  */
#line 239 "parser.y"
            { (yyval.node) = (yyvsp[0].node); }
#line 1590 "parser.tab.c"
    break;

  case 23: /* unmatched_stmt: KEYWORD LPAREN expression RPAREN statement  */
#line 243 "parser.y"
                                               {
        if (strcmp((yyvsp[-4].str), "if_RP") == 0) {
            (yyval.node) = createNode("If", "control", (yyvsp[-2].node), (yyvsp[0].node));
        } else if (strcmp((yyvsp[-4].str), "while_RP") == 0) {
            (yyval.node) = createNode("While", "control", (yyvsp[-2].node), (yyvsp[0].node));
        } else {
            yyerror("Expected 'if_RP' or 'while_RP' keyword");
            (yyval.node) = NULL;
        }
    }
#line 1605 "parser.tab.c"
    break;

  case 24: /* unmatched_stmt: KEYWORD LPAREN expression RPAREN matched_stmt KEYWORD unmatched_stmt  */
#line 253 "parser.y"
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
#line 1620 "parser.tab.c"
    break;

  case 25: /* unmatched_stmt: KEYWORD LPAREN expression RPAREN matched_stmt elif_chain KEYWORD unmatched_stmt  */
#line 263 "parser.y"
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
#line 1635 "parser.tab.c"
    break;

  case 26: /* unmatched_stmt: KEYWORD LPAREN error RPAREN statement  */
#line 273 "parser.y"
                                            {
        yyerror("Expected expression in if statement");
        (yyval.node) = NULL;
    }
#line 1644 "parser.tab.c"
    break;

  case 27: /* for_statement: KEYWORD LPAREN declaration SEMICOLON condition SEMICOLON increment_expr RPAREN statement  */
#line 280 "parser.y"
                                                                                             {
        if (strcmp((yyvsp[-8].str), "for_RP") == 0) {
            (yyval.node) = createNode("For", "control", (yyvsp[-6].node),
                createNode("ForBody", "control", (yyvsp[-4].node),
                    createNode("ForIncr", "control", (yyvsp[-2].node), (yyvsp[0].node))));
        } else {
            yyerror("Expected 'for_RP' keyword");
            (yyval.node) = NULL;
        }
    }
#line 1659 "parser.tab.c"
    break;

  case 28: /* declaration: KEYWORD IDENTIFIER ASSIGN expression  */
#line 293 "parser.y"
                                         {
        char* type = getTypeFromKeyword((yyvsp[-3].str));
        addSymbol((yyvsp[-2].str), type, currentScope, 0);
        (yyval.node) = createNode("Initialize", type, createNode((yyvsp[-2].str), type, NULL, NULL), (yyvsp[0].node));
        free(type);
    }
#line 1670 "parser.tab.c"
    break;

  case 29: /* condition: expression  */
#line 302 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 1676 "parser.tab.c"
    break;

  case 30: /* increment_expr: IDENTIFIER INCREMENT  */
#line 306 "parser.y"
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
#line 1691 "parser.tab.c"
    break;

  case 31: /* increment_expr: IDENTIFIER DECREMENT  */
#line 316 "parser.y"
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
#line 1706 "parser.tab.c"
    break;

  case 32: /* increment_expr: expression  */
#line 326 "parser.y"
                 { (yyval.node) = (yyvsp[0].node); }
#line 1712 "parser.tab.c"
    break;

  case 33: /* switch_statement: KEYWORD LPAREN expression RPAREN LBRACE cases RBRACE  */
#line 330 "parser.y"
                                                         {
        if (strcmp((yyvsp[-6].str), "switch_RP") == 0) {
            (yyval.node) = createNode("Switch", "control", (yyvsp[-4].node), (yyvsp[-1].node));
        } else {
            yyerror("Expected 'switch_RP' keyword");
            (yyval.node) = NULL;
        }
    }
#line 1725 "parser.tab.c"
    break;

  case 34: /* cases: cases case_statement  */
#line 341 "parser.y"
                         { (yyval.node) = createNode("Cases", "block", (yyvsp[-1].node), (yyvsp[0].node)); }
#line 1731 "parser.tab.c"
    break;

  case 35: /* cases: case_statement  */
#line 342 "parser.y"
                     { (yyval.node) = (yyvsp[0].node); }
#line 1737 "parser.tab.c"
    break;

  case 36: /* case_statement: KEYWORD NUMBER COLON case_statements break_statement  */
#line 346 "parser.y"
                                                         {
        if (strcmp((yyvsp[-4].str), "case_RP") == 0) {
            Node* caseBody = createNode("CaseBody", "block", (yyvsp[-1].node), (yyvsp[0].node));
            (yyval.node) = createNode("Case", "int", createNode((yyvsp[-3].str), "int", NULL, NULL), caseBody);
        } else {
            yyerror("Expected 'case_RP' keyword");
            (yyval.node) = NULL;
        }
    }
#line 1751 "parser.tab.c"
    break;

  case 37: /* case_statement: KEYWORD COLON case_statements break_statement  */
#line 355 "parser.y"
                                                    {
        if (strcmp((yyvsp[-3].str), "default_RP") == 0) {
            Node* defaultBody = createNode("DefaultBody", "block", (yyvsp[-1].node), (yyvsp[0].node));
            (yyval.node) = createNode("Default", "int", NULL, defaultBody);
        } else {
            yyerror("Expected 'default_RP' keyword");
            (yyval.node) = NULL;
        }
    }
#line 1765 "parser.tab.c"
    break;

  case 38: /* case_statements: case_statements case_statement_item  */
#line 367 "parser.y"
                                        { 
        (yyval.node) = createNode("CaseStatements", "block", (yyvsp[-1].node), (yyvsp[0].node)); 
    }
#line 1773 "parser.tab.c"
    break;

  case 39: /* case_statements: case_statement_item  */
#line 370 "parser.y"
                          { 
        (yyval.node) = (yyvsp[0].node); 
    }
#line 1781 "parser.tab.c"
    break;

  case 40: /* case_statement_item: statement  */
#line 376 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 1787 "parser.tab.c"
    break;

  case 41: /* break_statement: KEYWORD SEMICOLON  */
#line 380 "parser.y"
                      {
        if (strcmp((yyvsp[-1].str), "break_RP") == 0) {
            (yyval.node) = createNode("Break", "control", NULL, NULL);
        } else {
            yyerror("Expected 'break_RP' keyword");
            (yyval.node) = NULL;
        }
    }
#line 1800 "parser.tab.c"
    break;

  case 42: /* expression: expression PLUS expression  */
#line 391 "parser.y"
                               {
        (yyval.node) = createNode("+", "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = (yyvsp[-2].node)->value + (yyvsp[0].node)->value;
    }
#line 1809 "parser.tab.c"
    break;

  case 43: /* expression: expression MINUS expression  */
#line 395 "parser.y"
                                  {
        (yyval.node) = createNode("-", "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = (yyvsp[-2].node)->value - (yyvsp[0].node)->value;
    }
#line 1818 "parser.tab.c"
    break;

  case 44: /* expression: expression MULT expression  */
#line 399 "parser.y"
                                 {
        (yyval.node) = createNode("*", "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = (yyvsp[-2].node)->value * (yyvsp[0].node)->value;
    }
#line 1827 "parser.tab.c"
    break;

  case 45: /* expression: expression DIV expression  */
#line 403 "parser.y"
                                {
        (yyval.node) = createNode("/", "int", (yyvsp[-2].node), (yyvsp[0].node));
        if ((yyvsp[0].node)->value != 0) {
            (yyval.node)->value = (yyvsp[-2].node)->value / (yyvsp[0].node)->value;
        } else {
            yyerror("Division by zero");
            (yyval.node)->value = 0;
        }
    }
#line 1841 "parser.tab.c"
    break;

  case 46: /* expression: expression MOD expression  */
#line 412 "parser.y"
                                {
        (yyval.node) = createNode("%", "int", (yyvsp[-2].node), (yyvsp[0].node));
        if ((yyvsp[0].node)->value != 0) {
            (yyval.node)->value = (yyvsp[-2].node)->value % (yyvsp[0].node)->value;
        } else {
            yyerror("Modulo by zero");
            (yyval.node)->value = 0;
        }
    }
#line 1855 "parser.tab.c"
    break;

  case 47: /* expression: expression GT expression  */
#line 421 "parser.y"
                               {
        (yyval.node) = createNode(">", "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = ((yyvsp[-2].node)->value > (yyvsp[0].node)->value) ? 1 : 0;
    }
#line 1864 "parser.tab.c"
    break;

  case 48: /* expression: expression LT expression  */
#line 425 "parser.y"
                               {
        (yyval.node) = createNode("<", "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = ((yyvsp[-2].node)->value < (yyvsp[0].node)->value) ? 1 : 0;
    }
#line 1873 "parser.tab.c"
    break;

  case 49: /* expression: expression GE expression  */
#line 429 "parser.y"
                               {
        (yyval.node) = createNode(">=", "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = ((yyvsp[-2].node)->value >= (yyvsp[0].node)->value) ? 1 : 0;
    }
#line 1882 "parser.tab.c"
    break;

  case 50: /* expression: expression LE expression  */
#line 433 "parser.y"
                               {
        (yyval.node) = createNode("<=", "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = ((yyvsp[-2].node)->value <= (yyvsp[0].node)->value) ? 1 : 0;
    }
#line 1891 "parser.tab.c"
    break;

  case 51: /* expression: expression EQ expression  */
#line 437 "parser.y"
                               {
        (yyval.node) = createNode("==", "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = ((yyvsp[-2].node)->value == (yyvsp[0].node)->value) ? 1 : 0;
    }
#line 1900 "parser.tab.c"
    break;

  case 52: /* expression: expression AND expression  */
#line 441 "parser.y"
                                {
        (yyval.node) = createNode("&&", "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = ((yyvsp[-2].node)->value && (yyvsp[0].node)->value) ? 1 : 0;
    }
#line 1909 "parser.tab.c"
    break;

  case 53: /* expression: expression OR expression  */
#line 445 "parser.y"
                               {
        (yyval.node) = createNode("||", "int", (yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node)->value = ((yyvsp[-2].node)->value || (yyvsp[0].node)->value) ? 1 : 0;
    }
#line 1918 "parser.tab.c"
    break;

  case 54: /* expression: NOT expression  */
#line 449 "parser.y"
                     {
        (yyval.node) = createNode("!", "int", (yyvsp[0].node), NULL);
        (yyval.node)->value = !((yyvsp[0].node)->value) ? 1 : 0;
    }
#line 1927 "parser.tab.c"
    break;

  case 55: /* expression: MINUS expression  */
#line 453 "parser.y"
                                    {
        (yyval.node) = createNode("unary-", "int", (yyvsp[0].node), NULL);
        (yyval.node)->value = -((yyvsp[0].node)->value);
    }
#line 1936 "parser.tab.c"
    break;

  case 56: /* expression: IDENTIFIER  */
#line 457 "parser.y"
                 {
        int index = lookupSymbol((yyvsp[0].str), currentScope);
        if (index == -1) {
            // Instead of error, create a forward declaration
            addSymbol((yyvsp[0].str), "int", currentScope, 0);  // Default to int
            (yyval.node) = createNode((yyvsp[0].str), "int", NULL, NULL);
        } else {
            (yyval.node) = createNode((yyvsp[0].str), symbolTable[index].type, NULL, NULL);
        }
    }
#line 1951 "parser.tab.c"
    break;

  case 57: /* expression: NUMBER  */
#line 467 "parser.y"
             {
        (yyval.node) = createNode((yyvsp[0].str), "int", NULL, NULL);
        (yyval.node)->value = atoi((yyvsp[0].str));
    }
#line 1960 "parser.tab.c"
    break;

  case 58: /* expression: LPAREN expression RPAREN  */
#line 471 "parser.y"
                               { 
        (yyval.node) = (yyvsp[-1].node); 
    }
#line 1968 "parser.tab.c"
    break;

  case 59: /* expression: IDENTIFIER ASSIGN expression  */
#line 474 "parser.y"
                                   {
        int index = lookupSymbol((yyvsp[-2].str), currentScope);
        if (index == -1) {
            // Instead of error, create a forward declaration
            addSymbol((yyvsp[-2].str), "int", currentScope, 0);  // Default to int
            index = lookupSymbol((yyvsp[-2].str), currentScope);
        }
        (yyval.node) = createNode("Assignment", symbolTable[index].type, 
                       createNode((yyvsp[-2].str), symbolTable[index].type, NULL, NULL), (yyvsp[0].node));
    }
#line 1983 "parser.tab.c"
    break;

  case 60: /* expression: IDENTIFIER INCREMENT  */
#line 484 "parser.y"
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
#line 1998 "parser.tab.c"
    break;

  case 61: /* expression: IDENTIFIER DECREMENT  */
#line 494 "parser.y"
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
#line 2013 "parser.tab.c"
    break;

  case 62: /* expression: IDENTIFIER LBRACKET expression RBRACKET  */
#line 504 "parser.y"
                                              {
        int index = lookupSymbol((yyvsp[-3].str), currentScope);
        if (index == -1 || strcmp(symbolTable[index].type, "array") != 0) {
            yyerror("Invalid array access in expression");
            (yyval.node) = createNode("error", "error", NULL, NULL);
        } else {
            (yyval.node) = createNode("ArrayAccess", "int", 
                           createNode((yyvsp[-3].str), "array", (yyvsp[-1].node), NULL), NULL);
        }
    }
#line 2028 "parser.tab.c"
    break;

  case 63: /* expression: IDENTIFIER LBRACKET expression RBRACKET LBRACKET expression RBRACKET  */
#line 514 "parser.y"
                                                                           {
        int index = lookupSymbol((yyvsp[-6].str), currentScope);
        if (index == -1 || strcmp(symbolTable[index].type, "array") != 0) {
            yyerror("Invalid 2D array access in expression");
            (yyval.node) = createNode("error", "error", NULL, NULL);
        } else {
            Node* indices = createNode("Indices", "int", (yyvsp[-4].node), (yyvsp[-1].node));
            (yyval.node) = createNode("Array2DAccess", "int",
                           createNode((yyvsp[-6].str), "array", indices, NULL), NULL);
        }
    }
#line 2044 "parser.tab.c"
    break;

  case 64: /* elif_chain: elif_chain elif_clause  */
#line 529 "parser.y"
                           { (yyval.node) = createNode("ElifChain", "control", (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2050 "parser.tab.c"
    break;

  case 65: /* elif_chain: elif_clause  */
#line 530 "parser.y"
                  { (yyval.node) = (yyvsp[0].node); }
#line 2056 "parser.tab.c"
    break;

  case 66: /* elif_clause: KEYWORD LPAREN expression RPAREN matched_stmt  */
#line 534 "parser.y"
                                                  {
        if (strcmp((yyvsp[-4].str), "elif_RP") == 0) {
            (yyval.node) = createNode("Elif", "control", (yyvsp[-2].node), (yyvsp[0].node));
        } else {
            yyerror("Expected 'elif_RP' keyword");
            (yyval.node) = NULL;
        }
    }
#line 2069 "parser.tab.c"
    break;


#line 2073 "parser.tab.c"

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

#line 544 "parser.y"


int main() {
    yyparse();
    return 0;
}

void yyerror(const char *msg) {
    printf("Semantic Error: %s\n", msg);
}

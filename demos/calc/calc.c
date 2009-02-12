#ifndef lint
static const char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif

#include <stdlib.h>

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20070509

#define YYEMPTY (-1)
#define yyclearin    (yychar = YYEMPTY)
#define yyerrok      (yyerrflag = 0)
#define YYRECOVERING (yyerrflag != 0)

extern int yyparse(void);

static int yygrowstack(void);
#define YYPREFIX "yy"
#line 2 "calc.y"
/* A simple integer desk calculator using yacc and gmp.

Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
Street, Fifth Floor, Boston, MA 02110-1301, USA. */


/* This is a simple program, meant only to show one way to use GMP for this
   sort of thing.  There's few features, and error checking is minimal.
   Standard input is read, calc_help() below shows the inputs accepted.

   Expressions are evaluated as they're read.  If user defined functions
   were wanted it'd be necessary to build a parse tree like pexpr.c does, or
   a list of operations for a stack based evaluator.  That would also make
   it possible to detect and optimize evaluations "mod m" like pexpr.c does.

   A stack is used for intermediate values in the expression evaluation,
   separate from the yacc parser stack.  This is simple, makes error
   recovery easy, minimizes the junk around mpz calls in the rules, and
   saves initializing or clearing "mpz_t"s during a calculation.  A
   disadvantage though is that variables must be copied to the stack to be
   worked on.  A more sophisticated calculator or language system might be
   able to avoid that when executing a compiled or semi-compiled form.

   Avoiding repeated initializing and clearing of "mpz_t"s is important.  In
   this program the time spent parsing is obviously much greater than any
   possible saving from this, but a proper calculator or language should
   take some trouble over it.  Don't be surprised if an init/clear takes 3
   or more times as long as a 10 limb addition, depending on the system (see
   the mpz_init_realloc_clear example in tune/README).  */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpir.h"
#define NO_CALC_H /* because it conflicts with normal calc.c stuff */
#include "calc-common.h"


#define numberof(x)  (sizeof (x) / sizeof ((x)[0]))


void
calc_help (void)
{
  printf ("Examples:\n");
  printf ("    2+3*4        expressions are evaluated\n");
  printf ("    x=5^6        variables a to z can be set and used\n");
  printf ("Operators:\n");
  printf ("    + - *        arithmetic\n");
  printf ("    / %%          division and remainder (rounding towards negative infinity)\n");
  printf ("    ^            exponentiation\n");
  printf ("    !            factorial\n");
  printf ("    << >>        left and right shifts\n");
  printf ("    <= >= >      \\ comparisons, giving 1 if true, 0 if false\n");
  printf ("    == != <      /\n");
  printf ("    && ||        logical and/or, giving 1 if true, 0 if false\n");
  printf ("Functions:\n");
  printf ("    abs(n)       absolute value\n");
  printf ("    bin(n,m)     binomial coefficient\n");
  printf ("    fib(n)       fibonacci number\n");
  printf ("    gcd(a,b,..)  greatest common divisor\n");
  printf ("    kron(a,b)    kronecker symbol\n");
  printf ("    lcm(a,b,..)  least common multiple\n");
  printf ("    lucnum(n)    lucas number\n");
  printf ("    nextprime(n) next prime after n\n");
  printf ("    powm(b,e,m)  modulo powering, b^e%%m\n");
  printf ("    root(n,r)    r-th root\n");
  printf ("    sqrt(n)      square root\n");
  printf ("Other:\n");
  printf ("    hex          \\ set hex or decimal for input and output\n");
  printf ("    decimal      /   (\"0x\" can be used for hex too)\n");
  printf ("    quit         exit program (EOF works too)\n");
  printf ("    ;            statements are separated with a ; or newline\n");
  printf ("    \\            continue expressions with \\ before newline\n");
  printf ("    # xxx        comments are # though to newline\n");
  printf ("Hex numbers must be entered in upper case, to distinguish them from the\n");
  printf ("variables a to f (like in bc).\n");
}


int  ibase = 0;
int  obase = 10;


/* The stack is a fixed size, which means there's a limit on the nesting
   allowed in expressions.  A more sophisticated program could let it grow
   dynamically.  */

mpz_t    stack[100];
mpz_ptr  sp = stack[0];

#define CHECK_OVERFLOW()                                                  \
  if (sp >= stack[numberof(stack)])                                       \
    {                                                                     \
      fprintf (stderr,                                                    \
               "Value stack overflow, too much nesting in expression\n"); \
      YYERROR;                                                            \
    }

#define CHECK_EMPTY()                                                   \
  if (sp != stack[0])                                                   \
    {                                                                   \
      fprintf (stderr, "Oops, expected the value stack to be empty\n"); \
      sp = stack[0];                                                    \
    }


mpz_t  variable[26];

#define CHECK_VARIABLE(var)                                             \
  if ((var) < 0 || (var) >= numberof (variable))                        \
    {                                                                   \
      fprintf (stderr, "Oops, bad variable somehow: %d\n", var);        \
      YYERROR;                                                          \
    }


#define CHECK_UI(name,z)                        \
  if (! mpz_fits_ulong_p (z))                   \
    {                                           \
      fprintf (stderr, "%s too big\n", name);   \
      YYERROR;                                  \
    }

#line 143 "calc.y"
typedef union {
  char  *str;
  int   var;
} YYSTYPE;
#line 167 "calc.c"
#define EOS 257
#define BAD 258
#define HELP 259
#define HEX 260
#define DECIMAL 261
#define QUIT 262
#define ABS 263
#define BIN 264
#define FIB 265
#define GCD 266
#define KRON 267
#define LCM 268
#define LUCNUM 269
#define NEXTPRIME 270
#define POWM 271
#define ROOT 272
#define SQRT 273
#define NUMBER 274
#define VARIABLE 275
#define LOR 276
#define LAND 277
#define EQ 278
#define NE 279
#define LE 280
#define GE 281
#define LSHIFT 282
#define RSHIFT 283
#define UMINUS 284
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,    2,    2,    2,    1,    1,    1,    1,    1,
    1,    1,    3,    3,    3,    3,    3,    3,    3,    3,
    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
    3,    3,    3,    3,    4,    4,    5,    5,
};
short yylen[] = {                                         2,
    1,    2,    2,    3,    2,    0,    1,    3,    1,    1,
    1,    1,    3,    3,    3,    3,    3,    3,    3,    3,
    3,    2,    2,    3,    3,    3,    3,    3,    3,    3,
    3,    4,    6,    4,    4,    6,    4,    4,    4,    8,
    6,    4,    1,    1,    1,    3,    1,    3,
};
short yydefred[] = {                                      0,
    0,    9,   10,   11,   12,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   44,    0,    0,    0,
    0,    0,    0,    0,    5,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   43,    0,    0,
    3,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   22,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   13,    4,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   32,    0,   34,   35,    0,    0,   37,    0,   38,
   39,    0,    0,   42,    0,    0,    0,    0,    0,    0,
   33,   36,    0,   41,    0,   40,
};
short yydgoto[] = {                                      21,
   22,   23,   24,   64,   67,
};
short yysindex[] = {                                    742,
 -257,    0,    0,    0,    0,  -22,  -20,  -17,   -5,    5,
   18,   20,   22,   25,   28,   29,    0,  -54,  808,  808,
    0, -244,  786,  667,    0,  808,  808,  808,  808,  808,
  808,  808,  808,  808,  808,  808,  808,    0,  -27,  203,
    0, -217,  808,  808,  808,  808,  808,  808,  808,  808,
  808,  808,  808,  808,  808,  808,  808,  808,    0,  454,
  465,  487,  667,  -33,  498,  667,  -16,  520,  531,  542,
  564,  586,  667,    0,    0,  678,  929,  -28,  -28,  -28,
  -28,  -28,  -28,  -21,  -21,   -6,   -6,  -27,  -27,  -27,
  -27,    0,  808,    0,    0,  808,  808,    0,  808,    0,
    0,  808,  808,    0,  597,  667,  608,  667,  619,  645,
    0,    0,  808,    0,  656,    0,
};
short yyrindex[] = {                                      2,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    1,    0,    0,
    0,   50,    2,    3,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   10,    0,
    0,   71,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  -12,    0,    0,  -11,    0,    0,    0,    0,
    0,    0,    4,    0,    0,  193,   64,  166,  178,  182,
  187,  189,  191,  139,  151,  112,  124,   37,   49,   76,
   85,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   -2,    0,   15,    0,    0,
    0,    0,    0,    0,    0,    0,
};
short yygindex[] = {                                      0,
   52,    0, 1065,    0,    0,
};
#define YYTABLESIZE 1212
short yytable[] = {                                      25,
   43,    6,    7,    8,   59,   59,   37,   95,   57,   23,
   96,   59,   41,   55,   53,   57,   54,   26,   56,   27,
   55,   53,   28,   54,   98,   56,   59,   99,   45,   47,
   57,   45,   47,   43,   29,   55,   16,   43,   46,   75,
   56,   46,   43,   43,   30,   43,   23,   43,   17,    1,
   23,   23,   23,   23,   23,   48,   23,   31,   48,   32,
   43,   33,   43,   30,   34,   58,   58,   35,   36,   23,
    2,   23,   58,   16,   42,   18,    0,   16,   16,   16,
   16,   16,    0,   16,   19,   17,    0,   58,    0,   17,
   17,   17,   17,   17,   43,   17,   16,    0,   16,    0,
    0,    0,    0,    0,   30,    0,    0,   30,   17,    0,
   17,   14,   18,    0,    0,    0,   18,   18,   18,   18,
   18,   19,   18,   15,    0,   19,   19,   19,   19,   19,
    0,   19,    0,    0,    0,   18,    0,   18,   20,    0,
    0,    0,    0,    0,   19,    0,   19,    0,    0,    0,
   21,    0,   14,    0,   14,   14,   14,    0,    0,    0,
    0,    0,    0,    0,   15,   24,   15,   15,   15,    0,
    0,   14,    0,   14,    0,    0,    0,   29,    0,   20,
    0,   26,   20,   15,    0,   15,   27,    0,   25,    0,
   28,   21,   31,    0,   21,    0,    0,    0,   20,    0,
   20,    0,    0,    0,    0,    0,   24,    0,    0,   24,
   21,    0,   21,    0,    0,    0,    0,    0,   29,    0,
    0,   29,   26,    0,    0,   26,    0,   27,    0,   25,
   27,   28,   25,   31,   28,   59,   31,    0,    0,   57,
    0,    0,    0,   74,   55,   53,    0,   54,    0,   56,
    0,    0,    0,   51,   52,    0,    0,   43,    6,    7,
    8,    0,   45,    0,   46,    0,   23,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   43,   43,   43,   43,
   43,   43,   43,   43,    0,   23,   23,   23,   23,   23,
   23,   23,   23,   16,    0,    0,   58,    0,    0,    0,
    0,    0,    0,    0,    0,   17,    0,    0,    0,    0,
    0,    0,   16,   16,   16,   16,   16,   16,   16,   16,
   30,    0,    0,    0,   17,   17,   17,   17,   17,   17,
   17,   17,   18,    0,    0,    0,    0,    0,    0,   30,
   30,   19,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   18,   18,   18,   18,   18,   18,   18,   18,    0,
   19,   19,   19,   19,   19,   19,   19,   19,   14,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   15,    0,    0,    0,    0,    0,    0,   14,   14,   14,
   14,   14,   14,   14,   14,   20,    0,    0,    0,   15,
   15,   15,   15,   15,   15,   15,   15,   21,    0,    0,
    0,    0,    0,    0,   20,   20,   20,   20,   20,   20,
   20,   20,   24,    0,    0,    0,   21,   21,   21,   21,
   21,   21,   21,   21,   29,    0,    0,    0,   26,    0,
    0,   24,   24,   27,    0,   25,    0,   28,    0,   31,
    0,    0,    0,   29,   29,    0,    0,   26,   26,    0,
    0,    0,   27,   27,   25,   25,   28,   28,   31,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   43,   44,
   47,   48,   49,   50,   51,   52,   59,    0,    0,    0,
   57,    0,    0,    0,   92,   55,   53,   59,   54,    0,
   56,   57,    0,    0,    0,    0,   55,   53,   93,   54,
    0,   56,    0,   45,    0,   46,    0,    0,    0,   59,
    0,    0,    0,   57,   45,    0,   46,   94,   55,   53,
   59,   54,    0,   56,   57,    0,    0,    0,    0,   55,
   53,   97,   54,    0,   56,    0,   45,   58,   46,    0,
    0,    0,   59,    0,    0,    0,   57,   45,   58,   46,
  100,   55,   53,   59,   54,    0,   56,   57,    0,    0,
    0,  101,   55,   53,   59,   54,    0,   56,   57,   45,
   58,   46,    0,   55,   53,  102,   54,    0,   56,    0,
   45,   58,   46,    0,    0,    0,   59,    0,    0,    0,
   57,   45,    0,   46,    0,   55,   53,  103,   54,    0,
   56,    0,    0,   58,    0,    0,    0,    0,   59,    0,
    0,    0,   57,   45,   58,   46,  104,   55,   53,   59,
   54,    0,   56,   57,    0,   58,    0,  111,   55,   53,
   59,   54,    0,   56,   57,   45,    0,   46,  112,   55,
   53,   59,   54,    0,   56,   57,   45,   58,   46,    0,
   55,   53,  113,   54,    0,   56,    0,   45,    0,   46,
    0,    0,    0,    0,    0,    0,    0,   59,   45,   58,
   46,   57,    0,    0,    0,  114,   55,   53,   59,   54,
   58,   56,   57,    0,    0,    0,  116,   55,   53,   59,
   54,   58,   56,   57,   45,    0,   46,    0,   55,   53,
   59,   54,   58,   56,   57,   45,    0,   46,    0,   55,
   53,    0,   54,    0,   56,    0,   45,    0,   46,   43,
   44,   47,   48,   49,   50,   51,   52,   45,   58,   46,
   43,   44,   47,   48,   49,   50,   51,   52,    0,   58,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   58,    0,   43,   44,   47,   48,   49,   50,   51,   52,
    0,   58,    0,   43,   44,   47,   48,   49,   50,   51,
   52,   20,    0,    0,    0,    0,   19,    0,    0,    0,
    0,    0,    0,    0,    0,   43,   44,   47,   48,   49,
   50,   51,   52,    0,    0,    0,   43,   44,   47,   48,
   49,   50,   51,   52,    0,    0,    0,   43,   44,   47,
   48,   49,   50,   51,   52,   20,    0,    0,    0,    0,
   19,    0,    0,    0,    0,    0,    0,    0,    0,   43,
   44,   47,   48,   49,   50,   51,   52,   20,    0,    0,
    0,    0,   19,    0,    0,    0,    0,    0,    0,    0,
    0,   43,   44,   47,   48,   49,   50,   51,   52,    0,
    0,    0,   43,   44,   47,   48,   49,   50,   51,   52,
    0,    0,    0,   43,   44,   47,   48,   49,   50,   51,
   52,    0,    0,    0,   43,   44,   47,   48,   49,   50,
   51,   52,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   43,   44,   47,   48,   49,   50,   51,   52,    0,    0,
    0,   43,   44,   47,   48,   49,   50,   51,   52,    0,
    0,    0,   43,   44,   47,   48,   49,   50,   51,   52,
    0,    0,    0,    0,   44,   47,   48,   49,   50,   51,
   52,   59,    0,    0,    0,   57,    0,    0,    0,    0,
   55,   53,    0,   54,    0,   56,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   45,    0,
   46,    0,    0,    0,    0,    0,    0,    1,    0,    0,
    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
   12,   13,   14,   15,   16,   17,   18,    0,    0,    0,
    0,    0,   58,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    2,    3,    4,    5,    6,    7,
    8,    9,   10,   11,   12,   13,   14,   15,   16,   17,
   18,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
   16,   17,   38,   39,   40,    0,    0,    0,    0,    0,
   60,   61,   62,   63,   65,   66,   68,   69,   70,   71,
   72,   73,    0,    0,    0,    0,    0,   76,   77,   78,
   79,   80,   81,   82,   83,   84,   85,   86,   87,   88,
   89,   90,   91,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  105,    0,    0,
  106,  107,    0,  108,    0,    0,  109,  110,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  115,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   47,   48,   49,   50,
   51,   52,
};
short yycheck[] = {                                     257,
    0,    0,    0,    0,   33,   33,   61,   41,   37,    0,
   44,   33,  257,   42,   43,   37,   45,   40,   47,   40,
   42,   43,   40,   45,   41,   47,   33,   44,   41,   41,
   37,   44,   44,   33,   40,   42,    0,   37,   41,  257,
   47,   44,   42,   43,   40,   45,   37,   47,    0,    0,
   41,   42,   43,   44,   45,   41,   47,   40,   44,   40,
   60,   40,   62,    0,   40,   94,   94,   40,   40,   60,
    0,   62,   94,   37,   23,    0,   -1,   41,   42,   43,
   44,   45,   -1,   47,    0,   37,   -1,   94,   -1,   41,
   42,   43,   44,   45,   94,   47,   60,   -1,   62,   -1,
   -1,   -1,   -1,   -1,   41,   -1,   -1,   44,   60,   -1,
   62,    0,   37,   -1,   -1,   -1,   41,   42,   43,   44,
   45,   37,   47,    0,   -1,   41,   42,   43,   44,   45,
   -1,   47,   -1,   -1,   -1,   60,   -1,   62,    0,   -1,
   -1,   -1,   -1,   -1,   60,   -1,   62,   -1,   -1,   -1,
    0,   -1,   41,   -1,   43,   44,   45,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   41,    0,   43,   44,   45,   -1,
   -1,   60,   -1,   62,   -1,   -1,   -1,    0,   -1,   41,
   -1,    0,   44,   60,   -1,   62,    0,   -1,    0,   -1,
    0,   41,    0,   -1,   44,   -1,   -1,   -1,   60,   -1,
   62,   -1,   -1,   -1,   -1,   -1,   41,   -1,   -1,   44,
   60,   -1,   62,   -1,   -1,   -1,   -1,   -1,   41,   -1,
   -1,   44,   41,   -1,   -1,   44,   -1,   41,   -1,   41,
   44,   41,   44,   41,   44,   33,   44,   -1,   -1,   37,
   -1,   -1,   -1,   41,   42,   43,   -1,   45,   -1,   47,
   -1,   -1,   -1,  282,  283,   -1,   -1,  257,  257,  257,
  257,   -1,   60,   -1,   62,   -1,  257,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  276,  277,  278,  279,
  280,  281,  282,  283,   -1,  276,  277,  278,  279,  280,
  281,  282,  283,  257,   -1,   -1,   94,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  257,   -1,   -1,   -1,   -1,
   -1,   -1,  276,  277,  278,  279,  280,  281,  282,  283,
  257,   -1,   -1,   -1,  276,  277,  278,  279,  280,  281,
  282,  283,  257,   -1,   -1,   -1,   -1,   -1,   -1,  276,
  277,  257,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  276,  277,  278,  279,  280,  281,  282,  283,   -1,
  276,  277,  278,  279,  280,  281,  282,  283,  257,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  257,   -1,   -1,   -1,   -1,   -1,   -1,  276,  277,  278,
  279,  280,  281,  282,  283,  257,   -1,   -1,   -1,  276,
  277,  278,  279,  280,  281,  282,  283,  257,   -1,   -1,
   -1,   -1,   -1,   -1,  276,  277,  278,  279,  280,  281,
  282,  283,  257,   -1,   -1,   -1,  276,  277,  278,  279,
  280,  281,  282,  283,  257,   -1,   -1,   -1,  257,   -1,
   -1,  276,  277,  257,   -1,  257,   -1,  257,   -1,  257,
   -1,   -1,   -1,  276,  277,   -1,   -1,  276,  277,   -1,
   -1,   -1,  276,  277,  276,  277,  276,  277,  276,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  276,  277,
  278,  279,  280,  281,  282,  283,   33,   -1,   -1,   -1,
   37,   -1,   -1,   -1,   41,   42,   43,   33,   45,   -1,
   47,   37,   -1,   -1,   -1,   -1,   42,   43,   44,   45,
   -1,   47,   -1,   60,   -1,   62,   -1,   -1,   -1,   33,
   -1,   -1,   -1,   37,   60,   -1,   62,   41,   42,   43,
   33,   45,   -1,   47,   37,   -1,   -1,   -1,   -1,   42,
   43,   44,   45,   -1,   47,   -1,   60,   94,   62,   -1,
   -1,   -1,   33,   -1,   -1,   -1,   37,   60,   94,   62,
   41,   42,   43,   33,   45,   -1,   47,   37,   -1,   -1,
   -1,   41,   42,   43,   33,   45,   -1,   47,   37,   60,
   94,   62,   -1,   42,   43,   44,   45,   -1,   47,   -1,
   60,   94,   62,   -1,   -1,   -1,   33,   -1,   -1,   -1,
   37,   60,   -1,   62,   -1,   42,   43,   44,   45,   -1,
   47,   -1,   -1,   94,   -1,   -1,   -1,   -1,   33,   -1,
   -1,   -1,   37,   60,   94,   62,   41,   42,   43,   33,
   45,   -1,   47,   37,   -1,   94,   -1,   41,   42,   43,
   33,   45,   -1,   47,   37,   60,   -1,   62,   41,   42,
   43,   33,   45,   -1,   47,   37,   60,   94,   62,   -1,
   42,   43,   44,   45,   -1,   47,   -1,   60,   -1,   62,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   33,   60,   94,
   62,   37,   -1,   -1,   -1,   41,   42,   43,   33,   45,
   94,   47,   37,   -1,   -1,   -1,   41,   42,   43,   33,
   45,   94,   47,   37,   60,   -1,   62,   -1,   42,   43,
   33,   45,   94,   47,   37,   60,   -1,   62,   -1,   42,
   43,   -1,   45,   -1,   47,   -1,   60,   -1,   62,  276,
  277,  278,  279,  280,  281,  282,  283,   60,   94,   62,
  276,  277,  278,  279,  280,  281,  282,  283,   -1,   94,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   94,   -1,  276,  277,  278,  279,  280,  281,  282,  283,
   -1,   94,   -1,  276,  277,  278,  279,  280,  281,  282,
  283,   40,   -1,   -1,   -1,   -1,   45,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  276,  277,  278,  279,  280,
  281,  282,  283,   -1,   -1,   -1,  276,  277,  278,  279,
  280,  281,  282,  283,   -1,   -1,   -1,  276,  277,  278,
  279,  280,  281,  282,  283,   40,   -1,   -1,   -1,   -1,
   45,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  276,
  277,  278,  279,  280,  281,  282,  283,   40,   -1,   -1,
   -1,   -1,   45,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  276,  277,  278,  279,  280,  281,  282,  283,   -1,
   -1,   -1,  276,  277,  278,  279,  280,  281,  282,  283,
   -1,   -1,   -1,  276,  277,  278,  279,  280,  281,  282,
  283,   -1,   -1,   -1,  276,  277,  278,  279,  280,  281,
  282,  283,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  276,  277,  278,  279,  280,  281,  282,  283,   -1,   -1,
   -1,  276,  277,  278,  279,  280,  281,  282,  283,   -1,
   -1,   -1,  276,  277,  278,  279,  280,  281,  282,  283,
   -1,   -1,   -1,   -1,  277,  278,  279,  280,  281,  282,
  283,   33,   -1,   -1,   -1,   37,   -1,   -1,   -1,   -1,
   42,   43,   -1,   45,   -1,   47,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   60,   -1,
   62,   -1,   -1,   -1,   -1,   -1,   -1,  256,   -1,   -1,
  259,  260,  261,  262,  263,  264,  265,  266,  267,  268,
  269,  270,  271,  272,  273,  274,  275,   -1,   -1,   -1,
   -1,   -1,   94,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  259,  260,  261,  262,  263,  264,
  265,  266,  267,  268,  269,  270,  271,  272,  273,  274,
  275,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  263,  264,  265,  266,  267,  268,  269,  270,  271,  272,
  273,  274,  275,   19,   20,   -1,   -1,   -1,   -1,   -1,
   26,   27,   28,   29,   30,   31,   32,   33,   34,   35,
   36,   37,   -1,   -1,   -1,   -1,   -1,   43,   44,   45,
   46,   47,   48,   49,   50,   51,   52,   53,   54,   55,
   56,   57,   58,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   93,   -1,   -1,
   96,   97,   -1,   99,   -1,   -1,  102,  103,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  113,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  278,  279,  280,  281,
  282,  283,
};
#define YYFINAL 21
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 284
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'!'",0,0,0,"'%'",0,0,"'('","')'","'*'","'+'","','","'-'",0,"'/'",0,0,0,0,0,0,0,
0,0,0,0,0,"'<'","'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,"'^'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,"EOS","BAD","HELP","HEX","DECIMAL","QUIT","ABS","BIN","FIB",
"GCD","KRON","LCM","LUCNUM","NEXTPRIME","POWM","ROOT","SQRT","NUMBER",
"VARIABLE","LOR","LAND","EQ","NE","LE","GE","LSHIFT","RSHIFT","UMINUS",
};
char *yyrule[] = {
"$accept : top",
"top : statement",
"top : statements statement",
"statements : statement EOS",
"statements : statements statement EOS",
"statements : error EOS",
"statement :",
"statement : e",
"statement : VARIABLE '=' e",
"statement : HELP",
"statement : HEX",
"statement : DECIMAL",
"statement : QUIT",
"e : '(' e ')'",
"e : e '+' e",
"e : e '-' e",
"e : e '*' e",
"e : e '/' e",
"e : e '%' e",
"e : e '^' e",
"e : e LSHIFT e",
"e : e RSHIFT e",
"e : e '!'",
"e : '-' e",
"e : e '<' e",
"e : e LE e",
"e : e EQ e",
"e : e NE e",
"e : e GE e",
"e : e '>' e",
"e : e LAND e",
"e : e LOR e",
"e : ABS '(' e ')'",
"e : BIN '(' e ',' e ')'",
"e : FIB '(' e ')'",
"e : GCD '(' gcdlist ')'",
"e : KRON '(' e ',' e ')'",
"e : LCM '(' lcmlist ')'",
"e : LUCNUM '(' e ')'",
"e : NEXTPRIME '(' e ')'",
"e : POWM '(' e ',' e ',' e ')'",
"e : ROOT '(' e ',' e ')'",
"e : SQRT '(' e ')'",
"e : VARIABLE",
"e : NUMBER",
"gcdlist : e",
"gcdlist : gcdlist ',' e",
"lcmlist : e",
"lcmlist : lcmlist ',' e",
};
#endif
#if YYDEBUG
#include <stdio.h>
#endif

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH  500
#endif
#endif

#define YYINITSTACKSIZE 500

int      yydebug;
int      yynerrs;
int      yyerrflag;
int      yychar;
short   *yyssp;
YYSTYPE *yyvsp;
YYSTYPE  yyval;
YYSTYPE  yylval;

/* variables for the parser stack */
static short   *yyss;
static short   *yysslim;
static YYSTYPE *yyvs;
static int      yystacksize;
#line 266 "calc.y"

yyerror (char *s)
{
  fprintf (stderr, "%s\n", s);
}

int calc_option_readline = -1;

int
main (int argc, char *argv[])
{
  int  i;

  for (i = 1; i < argc; i++)
    {
      if (strcmp (argv[i], "--readline") == 0)
        calc_option_readline = 1;
      else if (strcmp (argv[i], "--noreadline") == 0)
        calc_option_readline = 0;
      else if (strcmp (argv[i], "--help") == 0)
        {
          printf ("Usage: calc [--option]...\n");
          printf ("  --readline    use readline\n");
          printf ("  --noreadline  don't use readline\n");
          printf ("  --help        this message\n");
          printf ("Readline is only available when compiled in,\n");
          printf ("and in that case it's the default on a tty.\n");
          exit (0);
        }
      else
        {
          fprintf (stderr, "Unrecognised option: %s\n", argv[i]);
          exit (1);
        }
    }

#if WITH_READLINE
  calc_init_readline ();
#else
  if (calc_option_readline == 1)
    {
      fprintf (stderr, "Readline support not available\n");
      exit (1);
    }
#endif

  for (i = 0; i < numberof (variable); i++)
    mpz_init (variable[i]);

  for (i = 0; i < numberof (stack); i++)
    mpz_init (stack[i]);

  return yyparse ();
}
#line 666 "calc.c"
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(void)
{
    int newsize, i;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = yyssp - yyss;
    newss = (yyss != 0)
          ? (short *)realloc(yyss, newsize * sizeof(*newss))
          : (short *)malloc(newsize * sizeof(*newss));
    if (newss == 0)
        return -1;

    yyss  = newss;
    yyssp = newss + i;
    newvs = (yyvs != 0)
          ? (YYSTYPE *)realloc(yyvs, newsize * sizeof(*newvs))
          : (YYSTYPE *)malloc(newsize * sizeof(*newvs));
    if (newvs == 0)
        return -1;

    yyvs = newvs;
    yyvsp = newvs + i;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse(void)
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;

    yyerror("syntax error");

#ifdef lint
    goto yyerrlab;
#endif

yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yysslim && yygrowstack())
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 5:
#line 174 "calc.y"
{ sp = stack[0]; yyerrok; }
break;
case 7:
#line 178 "calc.y"
{
      mpz_out_str (stdout, obase, sp); putchar ('\n');
      sp--;
      CHECK_EMPTY ();
    }
break;
case 8:
#line 183 "calc.y"
{
      CHECK_VARIABLE (yyvsp[-2].var);
      mpz_swap (variable[yyvsp[-2].var], sp);
      sp--;
      CHECK_EMPTY ();
    }
break;
case 9:
#line 189 "calc.y"
{ calc_help (); }
break;
case 10:
#line 190 "calc.y"
{ ibase = 16; obase = -16; }
break;
case 11:
#line 191 "calc.y"
{ ibase = 0;  obase = 10; }
break;
case 12:
#line 192 "calc.y"
{ exit (0); }
break;
case 14:
#line 199 "calc.y"
{ sp--; mpz_add    (sp, sp, sp+1); }
break;
case 15:
#line 200 "calc.y"
{ sp--; mpz_sub    (sp, sp, sp+1); }
break;
case 16:
#line 201 "calc.y"
{ sp--; mpz_mul    (sp, sp, sp+1); }
break;
case 17:
#line 202 "calc.y"
{ sp--; mpz_fdiv_q (sp, sp, sp+1); }
break;
case 18:
#line 203 "calc.y"
{ sp--; mpz_fdiv_r (sp, sp, sp+1); }
break;
case 19:
#line 204 "calc.y"
{ CHECK_UI ("Exponent", sp);
                    sp--; mpz_pow_ui (sp, sp, mpz_get_ui (sp+1)); }
break;
case 20:
#line 206 "calc.y"
{ CHECK_UI ("Shift count", sp);
                    sp--; mpz_mul_2exp (sp, sp, mpz_get_ui (sp+1)); }
break;
case 21:
#line 208 "calc.y"
{ CHECK_UI ("Shift count", sp);
                    sp--; mpz_fdiv_q_2exp (sp, sp, mpz_get_ui (sp+1)); }
break;
case 22:
#line 210 "calc.y"
{ CHECK_UI ("Factorial", sp);
                    mpz_fac_ui (sp, mpz_get_ui (sp)); }
break;
case 23:
#line 212 "calc.y"
{ mpz_neg (sp, sp); }
break;
case 24:
#line 214 "calc.y"
{ sp--; mpz_set_ui (sp, mpz_cmp (sp, sp+1) <  0); }
break;
case 25:
#line 215 "calc.y"
{ sp--; mpz_set_ui (sp, mpz_cmp (sp, sp+1) <= 0); }
break;
case 26:
#line 216 "calc.y"
{ sp--; mpz_set_ui (sp, mpz_cmp (sp, sp+1) == 0); }
break;
case 27:
#line 217 "calc.y"
{ sp--; mpz_set_ui (sp, mpz_cmp (sp, sp+1) != 0); }
break;
case 28:
#line 218 "calc.y"
{ sp--; mpz_set_ui (sp, mpz_cmp (sp, sp+1) >= 0); }
break;
case 29:
#line 219 "calc.y"
{ sp--; mpz_set_ui (sp, mpz_cmp (sp, sp+1) >  0); }
break;
case 30:
#line 221 "calc.y"
{ sp--; mpz_set_ui (sp, mpz_sgn (sp) && mpz_sgn (sp+1)); }
break;
case 31:
#line 222 "calc.y"
{ sp--; mpz_set_ui (sp, mpz_sgn (sp) || mpz_sgn (sp+1)); }
break;
case 32:
#line 224 "calc.y"
{ mpz_abs (sp, sp); }
break;
case 33:
#line 225 "calc.y"
{ sp--; CHECK_UI ("Binomial base", sp+1);
                                   mpz_bin_ui (sp, sp, mpz_get_ui (sp+1)); }
break;
case 34:
#line 227 "calc.y"
{ CHECK_UI ("Fibonacci", sp);
                                   mpz_fib_ui (sp, mpz_get_ui (sp)); }
break;
case 36:
#line 230 "calc.y"
{ sp--; mpz_set_si (sp,
                                         mpz_kronecker (sp, sp+1)); }
break;
case 38:
#line 233 "calc.y"
{ CHECK_UI ("Lucas number", sp);
                                   mpz_lucnum_ui (sp, mpz_get_ui (sp)); }
break;
case 39:
#line 235 "calc.y"
{ mpz_nextprime (sp, sp); }
break;
case 40:
#line 236 "calc.y"
{ sp -= 2; mpz_powm (sp, sp, sp+1, sp+2); }
break;
case 41:
#line 237 "calc.y"
{ sp--; CHECK_UI ("Nth-root", sp+1);
                                   mpz_root (sp, sp, mpz_get_ui (sp+1)); }
break;
case 42:
#line 239 "calc.y"
{ mpz_sqrt (sp, sp); }
break;
case 43:
#line 241 "calc.y"
{
        sp++;
        CHECK_OVERFLOW ();
        CHECK_VARIABLE (yyvsp[0].var);
        mpz_set (sp, variable[yyvsp[0].var]);
      }
break;
case 44:
#line 247 "calc.y"
{
        sp++;
        CHECK_OVERFLOW ();
        if (mpz_set_str (sp, yyvsp[0].str, ibase) != 0)
          {
            fprintf (stderr, "Invalid number: %s\n", yyvsp[0].str);
            YYERROR;
          }
      }
break;
case 46:
#line 259 "calc.y"
{ sp--; mpz_gcd (sp, sp, sp+1); }
break;
case 48:
#line 263 "calc.y"
{ sp--; mpz_lcm (sp, sp, sp+1); }
break;
#line 1027 "calc.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yysslim && yygrowstack())
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;

yyoverflow:
    yyerror("yacc stack overflow");

yyabort:
    return (1);

yyaccept:
    return (0);
}

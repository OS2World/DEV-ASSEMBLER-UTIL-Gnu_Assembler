/* expr.h -> header file for expr.c
   Copyright (C) 1987, 1992, 1993 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*
 * By popular demand, we define a struct to represent an expression.
 * This will no doubt mutate as expressions become baroque.
 *
 * Currently, we support expressions like "foo OP bar + 42".  In other
 * words we permit a (possibly undefined) symbol, a (possibly
 * undefined) symbol and the operation used to combine the symbols,
 * and an (absolute) augend.  RMS says this is so we can have 1-pass
 * assembly for any compiler emissions, and a 'case' statement might
 * emit 'undefined1 - undefined2'.
 *
 * The type of an expression used to be stored as a segment.  That got
 * confusing because it overloaded the concept of a segment.  I added
 * an operator field, instead.
 */

/* This is the type of an expression.  The operator types are also
   used while parsing an expression.

   NOTE: This enumeration must match the op_rank array in expr.c.  */

typedef enum
{
  /* An illegal expression.  */
  O_illegal,
  /* A nonexistent expression.  */
  O_absent,
  /* X_add_number (a constant expression).  */
  O_constant,
  /* X_add_symbol + X_add_number.  */
  O_symbol,
  /* A register (X_add_number is register number).  */
  O_register,
  /* A big value.  If X_add_number is negative or 0, the value is in
     generic_floating_point_number.  Otherwise the value is in
     generic_bignum, and X_add_number is the number of LITTLENUMs in
     the value.  */
  O_big,
  /* (- X_add_symbol) + X_add_number.  */
  O_uminus,
  /* (~ X_add_symbol) + X_add_number.  */
  O_bit_not,
  /* (X_add_symbol * X_op_symbol) + X_add_number.  */
  O_multiply,
  /* (X_add_symbol / X_op_symbol) + X_add_number.  */
  O_divide,
  /* X_add_symbol % X_op_symbol) + X_add_number.  */
  O_modulus,
  /* X_add_symbol << X_op_symbol) + X_add_number.  */
  O_left_shift,
  /* X_add_symbol >> X_op_symbol) + X_add_number.  */
  O_right_shift,
  /* X_add_symbol | X_op_symbol) + X_add_number.  */
  O_bit_inclusive_or,
  /* X_add_symbol |~ X_op_symbol) + X_add_number.  */
  O_bit_or_not,
  /* X_add_symbol ^ X_op_symbol) + X_add_number.  */
  O_bit_exclusive_or,
  /* X_add_symbol & X_op_symbol) + X_add_number.  */
  O_bit_and,
  /* X_add_symbol + X_op_symbol) + X_add_number.  */
  O_add,
  /* X_add_symbol - X_op_symbol) + X_add_number.  */
  O_subtract
} operatorT;

typedef struct
{
  /* The type of the expression.  */
  operatorT X_op;
  /* The main symbol.  */
  struct symbol *X_add_symbol;
  /* The second symbol, if needed.  */
  struct symbol *X_op_symbol;
  /* A number to add.  */
  offsetT X_add_number;
  /* Non-zero if X_add_number should be regarded as unsigned.  This is
     only valid for O_constant expressions.  It is only used when an
     O_constant must be extended into a bignum (i.e., it is not used
     when performing arithmetic on these values).
     FIXME: This field is not set very reliably.
     If we ever need more flags here, we can make them bitfields.  */
  int X_unsigned;
} expressionS;

/* "result" should be type (expressionS *). */
#define expression(result) expr (0, result)

/* If an expression is O_big, look here for its value. These common
   data may be clobbered whenever expr() is called. */
/* Flonums returned here.  Big enough to hold most precise flonum. */
extern FLONUM_TYPE generic_floating_point_number;
/* Bignums returned here. */
extern LITTLENUM_TYPE generic_bignum[];
/* Number of littlenums in above. */
#define SIZE_OF_LARGE_NUMBER (20)

typedef char operator_rankT;

char get_symbol_end PARAMS ((void));
segT expr PARAMS ((int rank, expressionS * resultP));
unsigned int get_single_number PARAMS ((void));

/* end of expr.h */

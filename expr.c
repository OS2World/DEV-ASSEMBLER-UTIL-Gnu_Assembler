/* expr.c -operands, expressions-
   Copyright (C) 1987, 1990, 1991, 1992, 1993 Free Software Foundation, Inc.

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
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. */

/*
 * This is really a branch office of as-read.c. I split it out to clearly
 * distinguish the world of expressions from the world of statements.
 * (It also gives smaller files to re-compile.)
 * Here, "operand"s are of expressions, not instructions.
 */

#include <ctype.h>
#include <string.h>

#include "as.h"

#include "obstack.h"

static void floating_constant PARAMS ((expressionS * expressionP));
static void integer_constant PARAMS ((int radix, expressionS * expressionP));
static void clean_up_expression PARAMS ((expressionS * expressionP));
static symbolS *make_expr_symbol PARAMS ((expressionS * expressionP));

extern const char EXP_CHARS[], FLT_CHARS[];

/* Build a dummy symbol to hold a complex expression.  This is how we
   build expressions up out of other expressions.  The symbol is put
   into the fake section expr_section.  */

static symbolS *
make_expr_symbol (expressionP)
     expressionS *expressionP;
{
  const char *fake;
  symbolS *symbolP;

  /* FIXME: This should be something which decode_local_label_name
     will handle.  */
  fake = FAKE_LABEL_NAME;

  /* Putting constant symbols in absolute_section rather than
     expr_section is convenient for the old a.out code, for which
     S_GET_SEGMENT does not always retrieve the value put in by
     S_SET_SEGMENT.  */
  symbolP = symbol_new (fake,
			(expressionP->X_op == O_constant
			 ? absolute_section
			 : expr_section),
			0, &zero_address_frag);
  symbolP->sy_value = *expressionP;
  return symbolP;
}

/*
 * Build any floating-point literal here.
 * Also build any bignum literal here.
 */

/* Seems atof_machine can backscan through generic_bignum and hit whatever
   happens to be loaded before it in memory.  And its way too complicated
   for me to fix right.  Thus a hack.  JF:  Just make generic_bignum bigger,
   and never write into the early words, thus they'll always be zero.
   I hate Dean's floating-point code.  Bleh.  */
LITTLENUM_TYPE generic_bignum[SIZE_OF_LARGE_NUMBER + 6];
FLONUM_TYPE generic_floating_point_number =
{
  &generic_bignum[6],		/* low (JF: Was 0) */
  &generic_bignum[SIZE_OF_LARGE_NUMBER + 6 - 1],	/* high JF: (added +6) */
  0,				/* leader */
  0,				/* exponent */
  0				/* sign */
};
/* If nonzero, we've been asked to assemble nan, +inf or -inf */
int generic_floating_point_magic;

static void
floating_constant (expressionP)
     expressionS *expressionP;
{
  /* input_line_pointer->*/
  /* floating-point constant. */
  int error_code;

  error_code = atof_generic
    (&input_line_pointer, ".", EXP_CHARS,
     &generic_floating_point_number);

  if (error_code)
    {
      if (error_code == ERROR_EXPONENT_OVERFLOW)
	{
	  as_bad ("bad floating-point constant: exponent overflow, probably assembling junk");
	}
      else
	{
	  as_bad ("bad floating-point constant: unknown error code=%d.", error_code);
	}
    }
  expressionP->X_op = O_big;
  /* input_line_pointer->just after constant, */
  /* which may point to whitespace. */
  expressionP->X_add_number = -1;
}

static void
integer_constant (radix, expressionP)
     int radix;
     expressionS *expressionP;
{
  char *digit_2;	/*->2nd digit of number. */
  char c;

  valueT number;	/* offset or (absolute) value */
  short int digit;	/* value of next digit in current radix */
  short int maxdig = 0;/* highest permitted digit value. */
  int too_many_digits = 0;	/* if we see >= this number of */
  char *name;		/* points to name of symbol */
  symbolS *symbolP;	/* points to symbol */

  int small;			/* true if fits in 32 bits. */
  extern const char hex_value[]; /* in hex_value.c */

  /* May be bignum, or may fit in 32 bits. */
  /* Most numbers fit into 32 bits, and we want this case to be fast.
     so we pretend it will fit into 32 bits.  If, after making up a 32
     bit number, we realise that we have scanned more digits than
     comfortably fit into 32 bits, we re-scan the digits coding them
     into a bignum.  For decimal and octal numbers we are
     conservative: Some numbers may be assumed bignums when in fact
     they do fit into 32 bits.  Numbers of any radix can have excess
     leading zeros: We strive to recognise this and cast them back
     into 32 bits.  We must check that the bignum really is more than
     32 bits, and change it back to a 32-bit number if it fits.  The
     number we are looking for is expected to be positive, but if it
     fits into 32 bits as an unsigned number, we let it be a 32-bit
     number.  The cavalier approach is for speed in ordinary cases. */
  /* This has been extended for 64 bits.  We blindly assume that if
     you're compiling in 64-bit mode, the target is a 64-bit machine.
     This should be cleaned up.  */

#ifdef BFD64
#define valuesize 64
#else /* includes non-bfd case, mostly */
#define valuesize 32
#endif

  switch (radix)
    {
    case 2:
      maxdig = 2;
      too_many_digits = valuesize + 1;
      break;
    case 8:
      maxdig = radix = 8;
      too_many_digits = (valuesize + 2) / 3;
      break;
    case 16:
      maxdig = radix = 16;
      too_many_digits = (valuesize + 3) / 4;
      break;
    case 10:
      maxdig = radix = 10;
      too_many_digits = (valuesize + 12) / 4; /* very rough */
    }
#undef valuesize
  c = *input_line_pointer;
  input_line_pointer++;
  digit_2 = input_line_pointer;
  for (number = 0;
       (digit = hex_value[(unsigned char) c]) < maxdig;
       c = *input_line_pointer++)
    {
      number = number * radix + digit;
    }
  /* c contains character after number. */
  /* input_line_pointer->char after c. */
  small = input_line_pointer - digit_2 < too_many_digits;
  if (!small)
    {
      /*
       * we saw a lot of digits. manufacture a bignum the hard way.
       */
      LITTLENUM_TYPE *leader;	/*->high order littlenum of the bignum. */
      LITTLENUM_TYPE *pointer;	/*->littlenum we are frobbing now. */
      long carry;

      leader = generic_bignum;
      generic_bignum[0] = 0;
      generic_bignum[1] = 0;
      /* we could just use digit_2, but lets be mnemonic. */
      input_line_pointer = --digit_2;	/*->1st digit. */
      c = *input_line_pointer++;
      for (;
	   (carry = hex_value[(unsigned char) c]) < maxdig;
	   c = *input_line_pointer++)
	{
	  for (pointer = generic_bignum;
	       pointer <= leader;
	       pointer++)
	    {
	      long work;

	      work = carry + radix * *pointer;
	      *pointer = work & LITTLENUM_MASK;
	      carry = work >> LITTLENUM_NUMBER_OF_BITS;
	    }
	  if (carry)
	    {
	      if (leader < generic_bignum + SIZE_OF_LARGE_NUMBER - 1)
		{		/* room to grow a longer bignum. */
		  *++leader = carry;
		}
	    }
	}
      /* again, c is char after number, */
      /* input_line_pointer->after c. */
      know (LITTLENUM_NUMBER_OF_BITS == 16);
      if (leader < generic_bignum + sizeof (valueT) / 2)
	{			/* will fit into 32 bits. */
	  number =
	    ((generic_bignum[1] & LITTLENUM_MASK) << LITTLENUM_NUMBER_OF_BITS)
	    | (generic_bignum[0] & LITTLENUM_MASK);
	  small = 1;
	}
      else
	{
	  number = leader - generic_bignum + 1;	/* number of littlenums in the bignum. */
	}
    }
  if (small)
    {
      /*
       * here with number, in correct radix. c is the next char.
       * note that unlike un*x, we allow "011f" "0x9f" to
       * both mean the same as the (conventional) "9f". this is simply easier
       * than checking for strict canonical form. syntax sux!
       */

      switch (c)
	{

#ifdef LOCAL_LABELS_FB
	case 'b':
	  {
	    /*
	     * backward ref to local label.
	     * because it is backward, expect it to be defined.
	     */
	    /* Construct a local label.  */
	    name = fb_label_name ((int) number, 0);

	    /* seen before, or symbol is defined: ok */
	    symbolP = symbol_find (name);
	    if ((symbolP != NULL) && (S_IS_DEFINED (symbolP)))
	      {

		/* local labels are never absolute. don't waste time
		   checking absoluteness. */
		know (SEG_NORMAL (S_GET_SEGMENT (symbolP)));

		expressionP->X_op = O_symbol;
		expressionP->X_add_symbol = symbolP;

	      }
	    else
	      {
		/* either not seen or not defined. */
		/* @@ Should print out the original string instead of
		   the parsed number.  */
		as_bad ("backw. ref to unknown label \"%d:\", 0 assumed.",
			(int) number);
		expressionP->X_op = O_constant;
	      }

	    expressionP->X_add_number = 0;
	    break;
	  }			/* case 'b' */

	case 'f':
	  {
	    /*
	     * forward reference. expect symbol to be undefined or
	     * unknown. undefined: seen it before. unknown: never seen
	     * it before.
	     * construct a local label name, then an undefined symbol.
	     * don't create a xseg frag for it: caller may do that.
	     * just return it as never seen before.
	     */
	    name = fb_label_name ((int) number, 1);
	    symbolP = symbol_find_or_make (name);
	    /* we have no need to check symbol properties. */
#ifndef many_segments
	    /* since "know" puts its arg into a "string", we
	       can't have newlines in the argument.  */
	    know (S_GET_SEGMENT (symbolP) == undefined_section || S_GET_SEGMENT (symbolP) == text_section || S_GET_SEGMENT (symbolP) == data_section);
#endif
	    expressionP->X_op = O_symbol;
	    expressionP->X_add_symbol = symbolP;
	    expressionP->X_add_number = 0;

	    break;
	  }			/* case 'f' */

#endif /* LOCAL_LABELS_FB */

#ifdef LOCAL_LABELS_DOLLAR

	case '$':
	  {

	    /* If the dollar label is *currently* defined, then this is just
	       another reference to it.  If it is not *currently* defined,
	       then this is a fresh instantiation of that number, so create
	       it.  */

	    if (dollar_label_defined ((long) number))
	      {
		name = dollar_label_name ((long) number, 0);
		symbolP = symbol_find (name);
		know (symbolP != NULL);
	      }
	    else
	      {
		name = dollar_label_name ((long) number, 1);
		symbolP = symbol_find_or_make (name);
	      }

	    expressionP->X_op = O_symbol;
	    expressionP->X_add_symbol = symbolP;
	    expressionP->X_add_number = 0;

	    break;
	  }			/* case '$' */

#endif /* LOCAL_LABELS_DOLLAR */

	default:
	  {
	    expressionP->X_op = O_constant;
	    expressionP->X_add_number = number;
	    input_line_pointer--;	/* restore following character. */
	    break;
	  }			/* really just a number */

	}			/* switch on char following the number */


    }
  else
    {
      /* not a small number */
      expressionP->X_op = O_big;
      expressionP->X_add_number = number;
      input_line_pointer--;	/*->char following number. */
    }
}


/*
 * Summary of operand().
 *
 * in:	Input_line_pointer points to 1st char of operand, which may
 *	be a space.
 *
 * out:	A expressionS.
 *	The operand may have been empty: in this case X_op == O_absent.
 *	Input_line_pointer->(next non-blank) char after operand.
 */

static segT
operand (expressionP)
     expressionS *expressionP;
{
  char c;
  symbolS *symbolP;	/* points to symbol */
  char *name;		/* points to name of symbol */
  segT segment;

  /* All integers are regarded as unsigned unless they are negated.
     This is because the only thing which cares whether a number is
     unsigned is the code in emit_expr which extends constants into
     bignums.  It should only sign extend negative numbers, so that
     something like ``.quad 0x80000000'' is not sign extended even
     though it appears negative if valueT is 32 bits.  */
  expressionP->X_unsigned = 1;

  /* digits, assume it is a bignum. */

  SKIP_WHITESPACE ();		/* leading whitespace is part of operand. */
  c = *input_line_pointer++;	/* input_line_pointer->past char in c. */

  switch (c)
    {
#ifdef MRI
    case '%':
      integer_constant (2, expressionP);
      break;
    case '@':
      integer_constant (8, expressionP);
      break;
    case '$':
      integer_constant (16, expressionP);
      break;
#endif
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      input_line_pointer--;

      integer_constant (10, expressionP);
      break;

    case '0':
      /* non-decimal radix */

      c = *input_line_pointer;
      switch (c)
	{

	default:
	  if (c && strchr (FLT_CHARS, c))
	    {
	      input_line_pointer++;
	      floating_constant (expressionP);
	    }
	  else
	    {
	      /* The string was only zero */
	      expressionP->X_op = O_constant;
	      expressionP->X_add_number = 0;
	    }

	  break;

	case 'x':
	case 'X':
	  input_line_pointer++;
	  integer_constant (16, expressionP);
	  break;

	case 'b':
#ifdef LOCAL_LABELS_FB
	  if (!input_line_pointer[1]
	      /* Strictly speaking, we should only need to check for
		 "+-01", since that's all you'd normally have in a
		 binary constant.  But some of our code does permit
		 digits greater than the base we're expecting.  */
	      || !strchr ("+-0123456789", input_line_pointer[1]))
	    {
	      input_line_pointer--;
	      integer_constant (10, expressionP);
	      break;
	    }
#endif
	case 'B':
	  input_line_pointer++;
	  integer_constant (2, expressionP);
	  break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	  integer_constant (8, expressionP);
	  break;

	case 'f':
#ifdef LOCAL_LABELS_FB
	  /* if it says '0f' and the line ends or it doesn't look like
	     a floating point #, its a local label ref.  dtrt */
	  /* likewise for the b's.  xoxorich. */
	  if (c == 'f'
	      && (!input_line_pointer[1]
		  || (!strchr ("+-.0123456789", input_line_pointer[1])
		      && !strchr (EXP_CHARS, input_line_pointer[1]))))
	    {
	      input_line_pointer -= 1;
	      integer_constant (10, expressionP);
	      break;
	    }
#endif

	case 'd':
	case 'D':
	case 'F':
	case 'r':
	case 'e':
	case 'E':
	case 'g':
	case 'G':

	  input_line_pointer++;
	  floating_constant (expressionP);
	  expressionP->X_add_number = -(isupper (c) ? tolower (c) : c);
	  break;

#ifdef LOCAL_LABELS_DOLLAR
	case '$':
	  integer_constant (10, expressionP);
	  break;
#endif
	}

      break;

    case '(':
      /* didn't begin with digit & not a name */
      segment = expression (expressionP);
      /* Expression() will pass trailing whitespace */
      if (*input_line_pointer++ != ')')
	{
	  as_bad ("Missing ')' assumed");
	  input_line_pointer--;
	}
      /* here with input_line_pointer->char after "(...)" */
      return segment;

    case '\'':
      /* Warning: to conform to other people's assemblers NO ESCAPEMENT is
	 permitted for a single quote. The next character, parity errors and
	 all, is taken as the value of the operand. VERY KINKY.  */
      expressionP->X_op = O_constant;
      expressionP->X_add_number = *input_line_pointer++;
      break;

    case '+':
      (void) operand (expressionP);
      break;

    case '~':
    case '-':
      {
	operand (expressionP);
	if (expressionP->X_op == O_constant)
	  {
	    /* input_line_pointer -> char after operand */
	    if (c == '-')
	      {
		expressionP->X_add_number = - expressionP->X_add_number;
		/* Notice: '-' may overflow: no warning is given. This is
		   compatible with other people's assemblers. Sigh.  */
		expressionP->X_unsigned = 0;
	      }
	    else
	      expressionP->X_add_number = ~ expressionP->X_add_number;
	  }
	else if (expressionP->X_op != O_illegal
		 && expressionP->X_op != O_absent)
	  {
	    expressionP->X_add_symbol = make_expr_symbol (expressionP);
	    if (c == '-')
	      expressionP->X_op = O_uminus;
	    else
	      expressionP->X_op = O_bit_not;
	    expressionP->X_add_number = 0;
	  }
	else
	  as_warn ("Unary operator %c ignored because bad operand follows",
		   c);
      }
      break;

    case '.':
      if (!is_part_of_name (*input_line_pointer))
	{
	  const char *fake;

	  /* JF: '.' is pseudo symbol with value of current location
	     in current segment.  */
	  fake = FAKE_LABEL_NAME;
	  symbolP = symbol_new (fake,
				now_seg,
				(valueT) frag_now_fix (),
				frag_now);

	  expressionP->X_op = O_symbol;
	  expressionP->X_add_symbol = symbolP;
	  expressionP->X_add_number = 0;
	  break;
	}
      else
	{
	  goto isname;
	}
    case ',':
    case '\n':
    case '\0':
    eol:
      /* can't imagine any other kind of operand */
      expressionP->X_op = O_absent;
      input_line_pointer--;
      md_operand (expressionP);
      break;

    default:
      if (is_end_of_line[(unsigned char) c])
	goto eol;
      if (is_name_beginner (c))	/* here if did not begin with a digit */
	{
	  /*
	   * Identifier begins here.
	   * This is kludged for speed, so code is repeated.
	   */
	isname:
	  name = --input_line_pointer;
	  c = get_symbol_end ();
	  symbolP = symbol_find_or_make (name);

	  /* If we have an absolute symbol or a reg, then we know its
	     value now.  */
	  segment = S_GET_SEGMENT (symbolP);
	  if (segment == absolute_section)
	    {
	      expressionP->X_op = O_constant;
	      expressionP->X_add_number = S_GET_VALUE (symbolP);
	    }
	  else if (segment == reg_section)
	    {
	      expressionP->X_op = O_register;
	      expressionP->X_add_number = S_GET_VALUE (symbolP);
	    }
	  else
	    {
	      expressionP->X_op = O_symbol;
	      expressionP->X_add_symbol = symbolP;
	      expressionP->X_add_number = 0;
	    }
	  *input_line_pointer = c;
	}
      else
	{
	  as_bad ("Bad expression");
	  expressionP->X_op = O_constant;
	  expressionP->X_add_number = 0;
	}
    }

  /*
   * It is more 'efficient' to clean up the expressionS when they are created.
   * Doing it here saves lines of code.
   */
  clean_up_expression (expressionP);
  SKIP_WHITESPACE ();		/*->1st char after operand. */
  know (*input_line_pointer != ' ');

  /* The PA port needs this information.  */
  if (expressionP->X_add_symbol)
    expressionP->X_add_symbol->sy_used = 1;

  switch (expressionP->X_op)
    {
    default:
      return absolute_section;
    case O_symbol:
      return S_GET_SEGMENT (expressionP->X_add_symbol);
    case O_register:
      return reg_section;
    }
}				/* operand() */

/* Internal. Simplify a struct expression for use by expr() */

/*
 * In:	address of a expressionS.
 *	The X_op field of the expressionS may only take certain values.
 *	Elsewise we waste time special-case testing. Sigh. Ditto SEG_ABSENT.
 * Out:	expressionS may have been modified:
 *	'foo-foo' symbol references cancelled to 0,
 *		which changes X_op from O_subtract to O_constant.
 *	Unused fields zeroed to help expr().
 */

static void
clean_up_expression (expressionP)
     expressionS *expressionP;
{
  switch (expressionP->X_op)
    {
    case O_illegal:
    case O_absent:
      expressionP->X_add_number = 0;
      /* Fall through.  */
    case O_big:
    case O_constant:
    case O_register:
      expressionP->X_add_symbol = NULL;
      /* Fall through.  */
    case O_symbol:
    case O_uminus:
    case O_bit_not:
      expressionP->X_op_symbol = NULL;
      break;
    case O_subtract:
      if (expressionP->X_op_symbol == expressionP->X_add_symbol
	  || ((expressionP->X_op_symbol->sy_frag
	       == expressionP->X_add_symbol->sy_frag)
	      && SEG_NORMAL (S_GET_SEGMENT (expressionP->X_add_symbol))
	      && (S_GET_VALUE (expressionP->X_op_symbol)
		  == S_GET_VALUE (expressionP->X_add_symbol))))
	{
	  expressionP->X_op = O_constant;
	  expressionP->X_add_symbol = NULL;
	  expressionP->X_op_symbol = NULL;
	}
      break;
    default:
      break;
    }
}

/* Expression parser. */

/*
 * We allow an empty expression, and just assume (absolute,0) silently.
 * Unary operators and parenthetical expressions are treated as operands.
 * As usual, Q==quantity==operand, O==operator, X==expression mnemonics.
 *
 * We used to do a aho/ullman shift-reduce parser, but the logic got so
 * warped that I flushed it and wrote a recursive-descent parser instead.
 * Now things are stable, would anybody like to write a fast parser?
 * Most expressions are either register (which does not even reach here)
 * or 1 symbol. Then "symbol+constant" and "symbol-symbol" are common.
 * So I guess it doesn't really matter how inefficient more complex expressions
 * are parsed.
 *
 * After expr(RANK,resultP) input_line_pointer->operator of rank <= RANK.
 * Also, we have consumed any leading or trailing spaces (operand does that)
 * and done all intervening operators.
 *
 * This returns the segment of the result, which will be
 * absolute_section or the segment of a symbol.
 */

#undef __
#define __ O_illegal

static const operatorT op_encoding[256] =
{				/* maps ASCII->operators */

  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,

  __, O_bit_or_not, __, __, __, O_modulus, O_bit_and, __,
  __, __, O_multiply, O_add, __, O_subtract, __, O_divide,
  __, __, __, __, __, __, __, __,
  __, __, __, __, O_left_shift, __, O_right_shift, __,
  __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, O_bit_exclusive_or, __,
  __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __,
  __, __, __, __, O_bit_inclusive_or, __, __, __,

  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __
};


/*
 *	Rank	Examples
 *	0	operand, (expression)
 *	1	+ -
 *	2	& ^ ! |
 *	3	* / % << >>
 *	4	unary - unary ~
 */
static const operator_rankT op_rank[] =
{
  0,	/* O_illegal */
  0,	/* O_absent */
  0,	/* O_constant */
  0,	/* O_symbol */
  0,	/* O_register */
  0,	/* O_bit */
  4,	/* O_uminus */
  4,	/* O_bit_now */
  3,	/* O_multiply */
  3,	/* O_divide */
  3,	/* O_modulus */
  3,	/* O_left_shift */
  3,	/* O_right_shift */
  2,	/* O_bit_inclusive_or */
  2,	/* O_bit_or_not */
  2,	/* O_bit_exclusive_or */
  2,	/* O_bit_and */
  1,	/* O_add */
  1,	/* O_subtract */
};

segT
expr (rank, resultP)
     operator_rankT rank;	/* Larger # is higher rank. */
     expressionS *resultP;	/* Deliver result here. */
{
  segT retval;
  expressionS right;
  operatorT op_left;
  char c_left;		/* 1st operator character. */
  operatorT op_right;
  char c_right;

  know (rank >= 0);

  retval = operand (resultP);

  know (*input_line_pointer != ' ');	/* Operand() gobbles spaces. */

  c_left = *input_line_pointer;	/* Potential operator character. */
  op_left = op_encoding[(unsigned char) c_left];
  while (op_left != O_illegal && op_rank[(int) op_left] > rank)
    {
      segT rightseg;

      input_line_pointer++;	/*->after 1st character of operator. */
      /* Operators "<<" and ">>" have 2 characters. */
      if (*input_line_pointer == c_left && (c_left == '<' || c_left == '>'))
	++input_line_pointer;

      rightseg = expr (op_rank[(int) op_left], &right);
      if (right.X_op == O_absent)
	{
	  as_warn ("missing operand; zero assumed");
	  right.X_op = O_constant;
	  right.X_add_number = 0;
	  right.X_add_symbol = NULL;
	  right.X_op_symbol = NULL;
	}

      know (*input_line_pointer != ' ');

      if (retval == undefined_section)
	{
	  if (SEG_NORMAL (rightseg))
	    retval = rightseg;
	}
      else if (! SEG_NORMAL (retval))
	retval = rightseg;
      else if (SEG_NORMAL (rightseg)
	       && retval != rightseg
#ifdef DIFF_EXPR_OK
	       && op_left != O_subtract
#endif
	       )
	as_bad ("operation combines symbols in different segments");

      c_right = *input_line_pointer;
      op_right = op_encoding[(unsigned char) c_right];
      if (*input_line_pointer == c_right && (c_right == '<' || c_right == '>'))
	++input_line_pointer;

      know (op_right == O_illegal || op_rank[(int) op_right] <= op_rank[(int) op_left]);
      know ((int) op_left >= (int) O_multiply && (int) op_left <= (int) O_subtract);

      /* input_line_pointer->after right-hand quantity. */
      /* left-hand quantity in resultP */
      /* right-hand quantity in right. */
      /* operator in op_left. */

      if (resultP->X_op == O_big)
	{
	  as_warn ("left operand of %c is a %s; integer 0 assumed",
		   c_left, resultP->X_add_number > 0 ? "bignum" : "float");
	  resultP->X_op = O_constant;
	  resultP->X_add_number = 0;
	  resultP->X_add_symbol = NULL;
	  resultP->X_op_symbol = NULL;
	}
      if (right.X_op == O_big)
	{
	  as_warn ("right operand of %c is a %s; integer 0 assumed",
		   c_left, right.X_add_number > 0 ? "bignum" : "float");
	  right.X_op = O_constant;
	  right.X_add_number = 0;
	  right.X_add_symbol = NULL;
	  right.X_op_symbol = NULL;
	}

      /* Optimize common cases.  */
      if (op_left == O_add && right.X_op == O_constant)
	{
	  /* X + constant.  */
	  resultP->X_add_number += right.X_add_number;
	}
      else if (op_left == O_subtract && right.X_op == O_constant)
	{
	  /* X - constant.  */
	  resultP->X_add_number -= right.X_add_number;
	}
      else if (op_left == O_add && resultP->X_op == O_constant)
	{
	  /* Constant + X.  */
	  resultP->X_op = right.X_op;
	  resultP->X_add_symbol = right.X_add_symbol;
	  resultP->X_op_symbol = right.X_op_symbol;
	  resultP->X_add_number += right.X_add_number;
	  retval = rightseg;
	}
      else if (resultP->X_op == O_constant && right.X_op == O_constant)
	{
	  /* Constant OP constant.  */
	  offsetT v = right.X_add_number;
	  if (v == 0 && (op_left == O_divide || op_left == O_modulus))
	    {
	      as_warn ("division by zero");
	      v = 1;
	    }
	  switch (op_left)
	    {
	    case O_multiply:		resultP->X_add_number *= v; break;
	    case O_divide:		resultP->X_add_number /= v; break;
	    case O_modulus:		resultP->X_add_number %= v; break;
	    case O_left_shift:		resultP->X_add_number <<= v; break;
	    case O_right_shift:		resultP->X_add_number >>= v; break;
	    case O_bit_inclusive_or:	resultP->X_add_number |= v; break;
	    case O_bit_or_not:		resultP->X_add_number |= ~v; break;
	    case O_bit_exclusive_or:	resultP->X_add_number ^= v; break;
	    case O_bit_and:		resultP->X_add_number &= v; break;
	    case O_add:			resultP->X_add_number += v; break;
	    case O_subtract:		resultP->X_add_number -= v; break;
	    default:			abort ();
	    }
	}
      else if (resultP->X_op == O_symbol
	       && right.X_op == O_symbol
	       && (op_left == O_add
		   || op_left == O_subtract
		   || (resultP->X_add_number == 0
		       && right.X_add_number == 0)))
	{
	  /* Symbol OP symbol.  */
	  resultP->X_op = op_left;
	  resultP->X_op_symbol = right.X_add_symbol;
	  if (op_left == O_add)
	    resultP->X_add_number += right.X_add_number;
	  else if (op_left == O_subtract)
	    resultP->X_add_number -= right.X_add_number;
	}
      else
	{
	  /* The general case.  */
	  resultP->X_add_symbol = make_expr_symbol (resultP);
	  resultP->X_op_symbol = make_expr_symbol (&right);
	  resultP->X_op = op_left;
	  resultP->X_add_number = 0;
	  resultP->X_unsigned = 1;
	}
	  
      op_left = op_right;
    }				/* While next operator is >= this rank. */

  /* The PA port needs this information.  */
  if (resultP->X_add_symbol)
    resultP->X_add_symbol->sy_used = 1;

  return resultP->X_op == O_constant ? absolute_section : retval;
}

/*
 *			get_symbol_end()
 *
 * This lives here because it belongs equally in expr.c & read.c.
 * Expr.c is just a branch office read.c anyway, and putting it
 * here lessens the crowd at read.c.
 *
 * Assume input_line_pointer is at start of symbol name.
 * Advance input_line_pointer past symbol name.
 * Turn that character into a '\0', returning its former value.
 * This allows a string compare (RMS wants symbol names to be strings)
 * of the symbol name.
 * There will always be a char following symbol name, because all good
 * lines end in end-of-line.
 */
char
get_symbol_end ()
{
  char c;

  while (is_part_of_name (c = *input_line_pointer++))
    ;
  *--input_line_pointer = 0;
  return (c);
}


unsigned int 
get_single_number ()
{
  expressionS exp;
  operand (&exp);
  return exp.X_add_number;

}

/* end of expr.c */

/* symbols.h -

   Copyright (C) 1987, 1990, 1992 Free Software Foundation, Inc.

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

extern struct obstack notes;	/* eg FixS live here. */

extern struct obstack cond_obstack;	/* this is where we track .ifdef/.endif
				       (if we do that at all).  */

extern symbolS *symbol_rootP;	/* all the symbol nodes */
extern symbolS *symbol_lastP;	/* last struct symbol we made, or NULL */

extern symbolS abs_symbol;

extern symbolS *dot_text_symbol;
extern symbolS *dot_data_symbol;
extern symbolS *dot_bss_symbol;

char *decode_local_label_name PARAMS ((char *s));
symbolS *symbol_find PARAMS ((CONST char *name));
symbolS *symbol_find_base PARAMS ((CONST char *name, int strip_underscore));
symbolS *symbol_find_or_make PARAMS ((char *name));
symbolS *symbol_make PARAMS ((CONST char *name));
symbolS *symbol_new PARAMS ((CONST char *name, segT segment, valueT value,
			     fragS * frag));
void colon PARAMS ((char *sym_name));
void local_colon PARAMS ((int n));
void symbol_begin PARAMS ((void));
void symbol_table_insert PARAMS ((symbolS * symbolP));
void resolve_symbol_value PARAMS ((symbolS *));

#ifdef LOCAL_LABELS_DOLLAR
int dollar_label_defined PARAMS ((long l));
void dollar_label_clear PARAMS ((void));
void define_dollar_label PARAMS ((long l));
char *dollar_label_name PARAMS ((long l, int augend));
#endif /* LOCAL_LABELS_DOLLAR */

#ifdef LOCAL_LABELS_FB
void fb_label_instance_inc PARAMS ((long label));
char *fb_label_name PARAMS ((long n, long augend));
#endif /* LOCAL_LABELS_FB */

/* Get and set the values of symbols.  These used to be macros.  */
extern valueT S_GET_VALUE PARAMS ((symbolS *));
extern void S_SET_VALUE PARAMS ((symbolS *, valueT));

#ifdef BFD_ASSEMBLER
extern int S_IS_EXTERNAL PARAMS ((symbolS *));
extern int S_IS_COMMON PARAMS ((symbolS *));
extern int S_IS_DEFINED PARAMS ((symbolS *));
extern int S_IS_DEBUG PARAMS ((symbolS *));
extern int S_IS_LOCAL PARAMS ((symbolS *));
extern int S_IS_EXTERN PARAMS ((symbolS *));
extern int S_IS_STABD PARAMS ((symbolS *));
extern CONST char *S_GET_NAME PARAMS ((symbolS *));
extern segT S_GET_SEGMENT PARAMS ((symbolS *));
extern void S_SET_SEGMENT PARAMS ((symbolS *, segT));
extern void S_SET_EXTERNAL PARAMS ((symbolS *));
extern void S_SET_NAME PARAMS ((symbolS *, char *));
extern void S_CLEAR_EXTERNAL PARAMS ((symbolS *));
extern void S_SET_WEAK PARAMS ((symbolS *));
#endif

/* end of symbols.h */

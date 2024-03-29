/* subsegs.h -> subsegs.c

   Copyright (C) 1987, 1992 Free Software Foundation, Inc.

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
 * For every sub-segment the user mentions in the ASsembler program,
 * we make one struct frchain. Each sub-segment has exactly one struct frchain
 * and vice versa.
 *
 * Struct frchain's are forward chained (in ascending order of sub-segment
 * code number). The chain runs through frch_next of each subsegment.
 * This makes it hard to find a subsegment's frags
 * if programmer uses a lot of them. Most programs only use text0 and
 * data0, so they don't suffer. At least this way:
 * (1)	There are no "arbitrary" restrictions on how many subsegments
 *	can be programmed;
 * (2)	Subsegments' frchain-s are (later) chained together in the order in
 *	which they are emitted for object file viz text then data.
 *
 * From each struct frchain dangles a chain of struct frags. The frags
 * represent code fragments, for that sub-segment, forward chained.
 */

struct frchain			/* control building of a frag chain */
{				/* FRCH = FRagment CHain control */
  struct frag *frch_root;	/* 1st struct frag in chain, or NULL */
  struct frag *frch_last;	/* last struct frag in chain, or NULL */
  struct frchain *frch_next;	/* next in chain of struct frchain-s */
  segT frch_seg;		/* SEG_TEXT or SEG_DATA. */
  subsegT frch_subseg;		/* subsegment number of this chain */
};

typedef struct frchain frchainS;

/* All subsegments' chains hang off here.  NULL means no frchains yet.  */
extern frchainS *frchain_root;

/* Frchain we are assembling into now That is, the current segment's
   frag chain, even if it contains no (complete) frags. */
extern frchainS *frchain_now;


typedef struct
{
  frchainS *frchainP;
  int hadone;
  int user_stuff;
  fixS *fix_root;
  fixS *fix_tail;
#if defined (MANY_SEGMENTS) && !defined (BFD_ASSEMBLER)
  struct internal_scnhdr scnhdr;
#endif
  symbolS *dot;

  struct lineno_list *lineno_list_head;
  struct lineno_list *lineno_list_tail;

#ifdef BFD_ASSEMBLER
  /* Which BFD section does this gas segment correspond to?  */
  asection *bfd_section;

  /* NULL, or pointer to the gas symbol that is the section symbol for
     this section.  sym->bsym and bfd_section->symbol should be the same.  */
  symbolS *sym;
#endif

  union
    {
      /* Current size of section holding stabs strings.  */
      unsigned long stab_string_size;
      /* Initial frag for ELF.  */
      char *p;
    }
  stabu;
} segment_info_type;

#ifdef BFD_ASSEMBLER

#define seg_info(SEC)	\
	((segment_info_type *) bfd_get_section_userdata (stdoutput, (SEC)))

#else /* ! BFD_ASSEMBLER */

#ifdef MANY_SEGMENTS

extern segment_info_type segment_info[];

#define seg_info(SEC)	(&segment_info[SEC])

#else

/* Sentinel for frchain crawling.  Points to the 1st data-segment
   frchain.  (Which is pointed to by the last text-segment frchain.) */
extern frchainS *data0_frchainP;
extern frchainS *bss0_frchainP;

#endif

#endif /* ! BFD_ASSEMBLER */

/* end of subsegs.h */

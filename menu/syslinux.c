/* -*- c -*- ------------------------------------------------------------- *
 *
 *   Copyright 2004 Murali Krishnan Ganapathy - All Rights Reserved
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 53 Temple Place Ste 330,
 *   Bostom MA 02111-1307, USA; either version 2 of the License, or
 *   (at your option) any later version; incorporated herein by reference.
 *
 * ----------------------------------------------------------------------- */

#include "syslinux.h"
#include "biosio.h"

static inline int asm_issyslinux(void)
{
  unsigned long eax, ebx, ecx, edx;

  asm("movb $0x30,%%ah ; int $0x21"
      : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx));

  return (eax == 0x53590000) && (ebx == 0x534c0000) &&
    (ecx == 0x494e0000) && (edx == 0x55580000);
}

char issyslinux()
{
   return asm_issyslinux();
}

static inline void asm_runcommand(char *cmd)
{
  asm volatile("int $0x22" : : "a" (0x0003), "b" (cmd));
}

void runcommand(char *cmd)
{
   asm_runcommand(cmd); 
}

static inline void asm_gototxtmode()
{
  asm volatile("int $0x22" : : "a" (0x0005));
}
   
void gototxtmode()
{
   asm_gototxtmode();
}

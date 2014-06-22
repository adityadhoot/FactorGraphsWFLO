#ifndef _memalloc_h_
#define _memalloc_h_

#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>

// inline functions

inline void *Malloc(long x)
{
   void *p;

   p=malloc(x);

   if (p==NULL)
   {
      printf("ERROR: Not enough memory (for a block of size %lu)\n",x);
      exit(-1);
   }

   return p;
}

inline void *Calloc(long x, int s)
{
   void *p;

   ///   printf("Allocating %u blocks of length %u\n",x,s);

   p=calloc((long) x, (int) s);

   if (p==NULL)
   {
      printf("ERROR: Not enough memory. (for a block of size %lu)\n",x*s);
      exit(-1);
   }

   return p;
}

inline void Free(void *x)
{
  free(x);
}

#endif

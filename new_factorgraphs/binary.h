#ifndef _binary_h_
#define _binary_h_

inline int indexedBinaryToInt(char *x, int *index, int n)
{
  int val;
  int *idx;
  int *indexN;

  val=0;
  indexN = index+n;

  for (idx=index; idx<indexN; idx++)
    {
      val <<= 1;
      val +=  x[*idx];
    }
 
  return val;
};

#endif
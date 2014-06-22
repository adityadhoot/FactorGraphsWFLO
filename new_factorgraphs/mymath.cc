// ################################################################################
//
// name:          mymath.cc
//
// author:        Martin Pelikan
//
// purpose:       commonly used mathematical functions
//
// last modified: August 2000
//
// ################################################################################

#include "mymath.h"
#include "memalloc.h"

#define invLog2 1.44269504088896338700465094007086008787155151367188

// ---------------------------------------------------------
// an array consisting of precomputed cummulative logarithms
// ---------------------------------------------------------

double *precomputedCummulativeLogarithm=NULL;

// ================================================================================
//
// name:          getPrecomputedCummulativeLog
//
// function:      returns a precomputed cummulative logarithm (log(i)+...+log(j))
//
// parameters:    i............starting number
//                j............ending number
//  
// returns:       (float) sum_{k=i}^{k=j}{log(k)}
//
// ================================================================================

float getPrecomputedCummulativeLog(long i, long j)
{
  return (float) ((double) (precomputedCummulativeLogarithm[j]-precomputedCummulativeLogarithm[i-1]));
}

// ================================================================================
//
// name:          precomputeCummulativeLogarithms
//
// function:      allocates memory for and precomputes cummulative logarithm 
//                (log(1)+...+log(j)) for all j up to a specified number
//
// parameters:    n............maximal number of cummulative logarithm to 
//                             precompute
//  
// returns:       (int) 0
//
// ================================================================================

int precomputeCummulativeLogarithms(long n)
{
  long i;
  double sum;
  
  precomputedCummulativeLogarithm = (double*) Calloc(n+1,sizeof(double));
  sum = 0;
  precomputedCummulativeLogarithm[0]=0;
  
  for (i=1; i<=n; i++)
    {
      sum += log((double) i);
      precomputedCummulativeLogarithm[i] = sum;
    };

  return 0;
}

// ================================================================================
//
// name:          freePrecomputedCummulativeLogarithms
//
// function:      frees the memory used by the array of precomputed cummulative 
//                logarithms 
//
// parameters:    (none)
//  
// returns:       (int) 0
//
// ================================================================================

int freePrecomputedCummulativeLogarithms()
{

  Free(precomputedCummulativeLogarithm);

  return 0;
}

// ================================================================================
//
// name:          round
//
// function:      rounds a float
//
// parameters:    x............the input floating-point number
//  
// returns:       (long) integer closest to the input number
//
// ================================================================================

long round(float x)
{
  return (long) (x+0.5);
}


// ================================================================================
//
// name:          log2
//
// function:      computes logarith of base 2
//
// parameters:    x............the input floating-point number
//  
// returns:       (double) log2(x)
//
// ================================================================================

double log2(double x)
{
  return invLog2*log(x);
}

#ifndef _population_h_
#define _population_h_

#include <stdio.h>

typedef struct {

  long  N;        // population size
  int   n;        // string length

  char  **x;      // strings
  float *f;       // fitness values

} Population;

int allocatePopulation(Population *population, long N, int n);
int freePopulation(Population *population);
int generatePopulation(Population *population);
int evaluatePopulation(Population *population);

int computeUnivariateFrequencies(Population *population, float *p1);

int copyIndividual(Population *population, long where, char *x, float f);
int swapIndividuals(Population *population, long first, long second);
int printIndividual(FILE *out, char *x, int n);

#endif

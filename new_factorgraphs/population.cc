// ################################################################################
//
// name:          population.cc
//
// author:        Martin Pelikan
//
// purpose:       functions for manipulation with the populations of strings and
//                the strings themselves
//
// last modified: August 2000
//
// ################################################################################

#include <stdio.h>
#include <string.h>

#include "population.h"
#include "random.h"
#include "memalloc.h"
#include "fitness.h"

// ================================================================================
//
// name:          allocatePopulation
//
// function:      allocates memory for a population and sets its size and string
//                length parameters
//
// parameters:    population...which population to allocate memory for
//                N............the number of strings for a population to contain
//                n............the length of strings
//
// returns:       (int) 0
//
// ================================================================================

int allocatePopulation(Population *population, long N, int n)
{
  long i;

  // set the size of the population and the string length

  population->N = N;
  population->n = n;

  // allocate memory for strings

  population->x = (char**) Calloc(N,sizeof(char*));
  
  for (i=0; i<N; i++)
    population->x[i] = (char*) Calloc(n,sizeof(char));

  // allocate memory for the array of fitness values

  population->f = (float*) Calloc(N,sizeof(float));

  // get back

  return 0;
}

// ================================================================================
//
// name:          freePopulation
//
// function:      frees the memory used by a population
//
// parameters:    population...which population to free memory used by
//
// returns:       (int) 0
//
// ================================================================================

int freePopulation(Population *population)
{
  long i;

  // free the memory used by strings
  
  for (i=0; i<population->N; i++)
    Free(population->x[i]);

  Free(population->x);

  // free the memory used by the array of fitness values

  Free(population->f);

  // get back

  return 0;
}

// ================================================================================
//
// name:          generatePopulation
//
// function:      generates all strings in a population at random with uniform 
//                distribution
//
// parameters:    population...which population to generate
//
// returns:       (int) 0
//
// ================================================================================

int generatePopulation(Population *population)
{
  long i;
  int  j;
	int turs=30;
	int tur_count =0;

  // generate bit by bit

  for (i=0; i<population->N; i++){
    for (j=0; j<population->n; j++){
				population->x[i][j]=0;
		}
		while (turs!=0){
			int r_xy = intRand(population->n);
			if (population->x[i][r_xy] == 0){
				population->x[i][r_xy] = 1;
				turs = turs - 1;
			}
		}
		turs = 30;
	}

/*

     long i;
  int  j;

  // generate bit by bit

  for (i=0; i<population->N; i++)
    for (j=0; j<population->n; j++)
      if (drand()<0.5)
	population->x[i][j]=0;
      else
	population->x[i][j]=1;

  // get back
*/
  return 0;
}

// ================================================================================
//
// name:          evaluatePopulation
//
// function:      evaluates fitness for all strings in a population
//
// parameters:    population...which population to evaluate
//
// returns:       (int) 0
//
// ================================================================================

int evaluatePopulation(Population *population)
{
  long i;

  // evaluate each individual

  for (i=0; i<population->N; i++)
    population->f[i] = getFitnessValue(population->x[i],population->n);

  // get back

  return 0;
}

// ================================================================================
//
// name:          computeUnivariateFrequencies
//
// function:      compute the frequencies of 1's on all positions in a population;
//                does not allocate memory for them-it has to be allocated already
//
// parameters:    population...which population to compute the frequencies for
//                p1...........the array of resulting frequencies
//
// returns:       (int) 0
//
// ================================================================================

int computeUnivariateFrequencies(Population *population, float *p1)
{
  long i;
  int  k;
  long *count1;

  // allocate memory for the counts
  
  count1 = (long*) Calloc(population->n,sizeof(long));

  // compute univariate counts
  
  for (k=0; k<population->n; k++)
    count1[k]=0;
  
  for (i=0; i<population->N; i++)
    for (k=0; k<population->n; k++)
      if (population->x[i][k])
	count1[k]++;
  
  // compute the frequencies

  for (k=0; k<population->n; k++)
    p1[k]=(float) count1[k]/(float) population->N;

  // free the memory used by the counts

  Free(count1);

  // get back

  return 0;
}

// ================================================================================
//
// name:          copyIndividual
//
// function:      copy a given string into a particular spot in a population
//
// parameters:    population...the population where to put the copied string
//                where........the position in a population where to put the string
//                x............the string to copy (put into the population)
//                f............the fitness of the copied string (if known)
//
// returns:       (int) 0
//
// ================================================================================

int copyIndividual(Population *population, long where, char *x, float f=0)
{
  // copy the individual's chromosome

  memcpy(population->x[where],x,population->n);

  // copy the fitness value

  population->f[where] = f;

  // get back

  return 0;
}

// ================================================================================
//
// name:          swapIndividuals
//
// function:      swaps specified two strings in a population
//
// parameters:    population...the population where to swap the specified string
//                first........a position of the first string to swap
//                second.......a position of the second string to swap
//
// returns:       (int) 0
//
// ================================================================================

int swapIndividuals(Population *population, long first, long second)
{
  char *auxX;
  float auxF;

  // allocate memory for an auxilary string

  auxX = (char*) Malloc(population->n);
  
  // swap the strings

  memcpy(auxX,population->x[first],population->n);
  memcpy(population->x[first],population->x[second],population->n);
  memcpy(population->x[second],auxX,population->n);
  
  // swap the fitness values

  auxF = population->f[first];
  population->f[first] = population->f[second];
  population->f[second] = auxF; 

  // free the memory used by an auxilary string

  Free(auxX);

  // get back

  return 0;
}

// ================================================================================
//
// name:          printIndividual
//
// function:      prints out a specified string to a stream
//
// parameters:    out..........the stream to print the string to
//                x............the string to print
//                i............the string length
//
// returns:       (int) 0
//
// ================================================================================

int printIndividual(FILE *out, char *x, int n)
{
  int k;

  // null file?

  if (out==NULL)
    return 0;

  // print it out

  for (k=0; k<n; k++)
    fprintf(out,"%u",x[k]);

  // get back

  return 0;
}
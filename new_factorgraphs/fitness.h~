#ifndef _fitness_h_
#define _fitness_h_

#include "boa.h"

typedef float FitnessFunction(char *x, int n);
typedef char  IsBest(char *x, int n);
typedef int   InitFitness(BoaParams *boaParams);
typedef int   DoneFitness(BoaParams *boaParams);

typedef struct {
  char            *description;

  FitnessFunction *fitness;
  IsBest          *isBest;
  InitFitness     *init;
  DoneFitness     *done;
} Fitness;

float onemax(char *x, int n);
float quadratic(char *x, int n);
float f3deceptive(char *x, int n);
float trap5(char *x, int n);
float f3deceptiveBipolar(char *x, int n);
float f3deceptiveOverlapping(char *x, int n);
float wflofitness(char *x, int n);

char areAllGenesOne(char *x, int n);
char areBlocks6ZeroOrOne(char *x, int n);

int  setFitness(int n);
char *getFitnessDesc(int n);

int initializeFitness(BoaParams *boaParams);
int doneFitness(BoaParams *boaParams);
float getFitnessValue(char *x, int n);
int isBestDefined();
int isOptimal(char *x, int n);

int resetFitnessCalls(void);
long fitnessCalled(void);
long getFitnessCalls(void);

#endif

#ifndef _statistics_h_
#define _statistics_h_

#include <stdio.h>

#include "population.h"
#include "boa.h"
#include "graph.h"
#include "frequencyDecisionGraph.h"

// ----------------
// basic statistics
// ----------------

typedef struct {

  long   generation;       // the number of generation
  long   N;                // population size
  int    n;                // problem size (number of variables)
  float  minF,maxF;        // minimal and maximal fitness
  double avgF;             // average fitness
  long   numOptimal;       // number of optimal solutions
  long   max;              // number of maximal individual
  float  *p1;              // univariate frequencies
  char   *bestX;           // best guy
  float guidanceThreshold; // guidance threshold

} BasicStatistics;

int intializeBasicStatistics(BasicStatistics *statistics, BoaParams *boaParams);
int doneBasicStatistics(BasicStatistics *statistics);
int computeBasicStatistics(BasicStatistics *statistics, long t, Population *population, BoaParams *boaParams);

int generationStatistics(FILE *out, BasicStatistics *statistics);
int fitnessStatistics(FILE *out, BasicStatistics *statistics);
int finalStatistics(FILE *out, char *termination, BasicStatistics *statistics);
int printModel(FILE *out, long t, AcyclicOrientedGraph *G, FrequencyDecisionGraph **T);
int printGuidance(FILE *out, float *p1, int n, float treshold);

#endif

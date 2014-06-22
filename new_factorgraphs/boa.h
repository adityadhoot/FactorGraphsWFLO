#ifndef _boa_h_
#define _boa_h_

#define NO_TERMINATION              0
#define MAXGENERATIONS_TERMINATION  1
#define EPSILON_TERMINATION         2
#define MAXOPTIMAL_TERMINATION      3
#define OPTIMUMFOUND_TERMINATION    4

#include "population.h"

typedef struct {

  long N;                      // population size
  float percentOffspring;      // size of offspring (in %)
  
  int fitnessNumber;           // number of fitness function to use
  int n;                       // size of a problem (length of a string)

  int   tournamentSize;        // size of the tournament (selection)
  long  maxGenerations;        // maximal number of generations to continue
  long  maxFitnessCalls;       // maximal number of fitness calls to continue
  float epsilon;               // distance threshold from univariate frequencies
  char  stopWhenFoundOptimum;  // stop when the optimum has been found?
  float maxOptimal;            // maximal proportion of optimal solutions to continue

  int   maxIncoming;           // maximal number of incoming edges in the networks
  char  allowMerge;            // allow the merge operator? (otherwise only splits are done)

  char *outputFilename;        // the name of ouput file
  float guidanceThreshold;     // the threshold for guidance in statistic info

  char pause;                  // wait for enter after printing out generation statistics?

  long randSeed;               // random seed

} BoaParams;

// ---------------------------------------

int initialize(BoaParams *boaParams);
int boa(BoaParams *boaParams);
int terminationCriteria(BoaParams *boaParams);
int generateOffspring(long t, Population *parents, Population *offspring, BoaParams *boaParams);
int pause(BoaParams *boaParams);
int done(BoaParams *boaParams);

FILE *getLogFile();
FILE *getModelFile();
FILE *getFitnessFile();

#endif

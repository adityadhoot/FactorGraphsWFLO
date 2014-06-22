// ################################################################################
//
// name:          boa.cc      
//
// author:        Martin Pelikan
//
// purpose:       functions for the initialization of the BOA, the BOA itself and 
//                a done method for the BOA
//
// last modified: August 2000
//
// ################################################################################

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "boa.h"
#include "population.h"
#include "statistics.h"
#include "replace.h"
#include "select.h"
#include "graph.h"
#include "bayesian.h"
#include "fitness.h"
#include "memalloc.h"
#include "random.h"
#include "frequencyDecisionGraph.h"
#include "WindFarmLayout.h"
#include "armadillo"

// --------------------
// various output files
// --------------------

FILE *logFile;
FILE *fitnessFile;
FILE *modelFile;

BasicStatistics populationStatistics;

// --------------------------------------------------------
// the description of termination criteria that are checked
// --------------------------------------------------------

char *terminationReasonDescription[5] = {
"No reason",
"Maximal number of generations reached",
"Bit convergence (with threshold epsilon)",
"Proportion of optima in a population reached the threshold",
"Optimum has been found"};

// ================================================================================
//
// name:          initialize
//
// function:      initializes everything the BOA needs to be run properly
//                (initialize fitness function, metric, random number generator,
//                etc.)
//
// parameters:    boaParams....the parameters sent to the BOA
//
// returns:       (int) 0
//
// ================================================================================

int initialize(BoaParams *boaParams)
{

	 char filename[200];

  // set the fitness function to be optimized

  setFitness(boaParams->fitnessNumber);
 
  // initialize fitness

  initializeFitness(boaParams);

  // initialize metric

  initializeMetric(boaParams);

  // reset the counter for fitness calls

  resetFitnessCalls();

  // set random seed

  setSeed(boaParams->randSeed);

  // initialize statistics

  intializeBasicStatistics(&populationStatistics,boaParams);

  // open output files (if the base of the output file names specified)

  if (boaParams->outputFilename)
    {
      sprintf(filename,"%s.log",boaParams->outputFilename);
      logFile = fopen(filename,"w");

      sprintf(filename,"%s.fitness",boaParams->outputFilename);
      fitnessFile = fopen(filename,"w");

      sprintf(filename,"%s.model",boaParams->outputFilename);
      modelFile = fopen(filename,"w");
    }
  else
    logFile = fitnessFile = modelFile = NULL;

  // get back

  return 0;
}

// ================================================================================
//
// name:          boa
//
// function:      the kernel of the BOA (runs the BOA for a chosen problem)
//
// parameters:    boaParams....the parameters sent to the BOA
//
// returns:       (int) 0
//
// ================================================================================

int boa(BoaParams *boaParams)
{
  long       N,numOffspring,numParents,t;
  int        n;
  Population population,parents,offspring;
  int        terminationReason;

  // set some variables

  N            = boaParams->N;
  numOffspring = (long) ((float) boaParams->N*boaParams->percentOffspring)/100;
  numParents   = (long) boaParams->N;
  n            = boaParams->n;

  // allocate the memory for the population

  allocatePopulation(&population,N,n);
  allocatePopulation(&parents,numParents,n);
  allocatePopulation(&offspring,numOffspring,n);

  // randomly generate first population according to uniform distribution

  generatePopulation(&population);

  // evaluate first population

  evaluatePopulation(&population);

  // main loop

  t=0;

  // compute basic statistics on initial population

  computeBasicStatistics(&populationStatistics,t,&population,boaParams);

  // output the statistics on first generation

  generationStatistics(stdout,&populationStatistics);
  generationStatistics(logFile,&populationStatistics);
  fitnessStatistics(fitnessFile,&populationStatistics);

  // pause after statistics?

  pause(boaParams);

  while (!(terminationReason=terminationCriteria(boaParams)))
    {
      // perform truncation (block) selection

      selectTheBest(&population,&parents,boaParams);

      // create offspring

      generateOffspring(t,&parents,&offspring,boaParams);
      
      // evaluate the offspring

      evaluatePopulation(&offspring);

      // replace the worst of the population with offspring

      replaceWorst(&population,&offspring);
      
      // increase the generation number
   
      t++;

      // compute basic statistics

      computeBasicStatistics(&populationStatistics,t,&population,boaParams);

      // output the statistics on current generation

      generationStatistics(stdout,&populationStatistics);
      generationStatistics(logFile,&populationStatistics);
      fitnessStatistics(fitnessFile,&populationStatistics);

      // pause after statistics?

      pause(boaParams);
    };

  // print out final statistics

  computeBasicStatistics(&populationStatistics,t,&population,boaParams);
  
  finalStatistics(stdout,terminationReasonDescription[terminationReason],&populationStatistics);
  finalStatistics(logFile,terminationReasonDescription[terminationReason],&populationStatistics);
  
  // free the memory used by the population, the parents, and the offspring

  freePopulation(&population);
  freePopulation(&parents);
  freePopulation(&offspring);

  // get back

  return 0;
}

// ================================================================================
//
// name:          terminationCriteria
//
// function:      checks whether some of the termination criteria wasn't matched
//                and returns the number of the criterion that has been met or 0
//
// parameters:    boaParams....the parameters sent to the BOA
//
// returns:       (int) the number of a met criterion or 0 if none has been met
//
// ================================================================================

int terminationCriteria(BoaParams *boaParams)
{
  int result;

  // no reason to finish yet

  result=0;

  // check if the proportion of optima reached the required value, if yes terminate

  if ((!result)&&(boaParams->maxOptimal>=0))
    result = (float(populationStatistics.numOptimal*100)/(float)populationStatistics.N>=boaParams->maxOptimal)? MAXOPTIMAL_TERMINATION:0;

  // check if should terminate if optimum has been found and if this is the case if yes

  if ((!result)&&(boaParams->stopWhenFoundOptimum))
    if (isBestDefined())
      result = (isOptimal(populationStatistics.bestX,populationStatistics.n))? OPTIMUMFOUND_TERMINATION:0;

  // if there's no reason to finish yet and the epsilon threshold was set, check it

  if ((!result)&&(boaParams->epsilon>=0))
    {
      int   k;
      float epsilon1;

      // set epsilon1 to (1-boaParams->epsilon)

      epsilon1 = 1-boaParams->epsilon;

      // are all frequencies closer than epsilon to either 0 or 1?

      result=EPSILON_TERMINATION;
      for (k=0; k<populationStatistics.n; k++)
	if ((populationStatistics.p1[k]>=boaParams->epsilon)&&(populationStatistics.p1[k]<=epsilon1))
	  result=0;
    }

  // check if the number of generations wasn't exceeded

  if ((!result)&&(boaParams->maxGenerations>=0))
    result = (populationStatistics.generation>=boaParams->maxGenerations)? MAXGENERATIONS_TERMINATION:0;

  // get back

  return result;
}

// ================================================================================
//
// name:          generateOffspring
//
// function:      generates offspring in the BOA (constructs the network for the
//                selected strings, and then uses it to generate new strings) 
//
// parameters:    t............the number of current generation
//                parents......the selected set of promising strings
//                offspring....the resulting population of offspring
//                boaParams....the parameters sent to the BOA
//
// returns:       (int) 0
//
// ================================================================================

int generateOffspring(long t, Population *parents, Population *offspring, BoaParams *boaParams)
{
  int k;
  AcyclicOrientedGraph *G;
  FrequencyDecisionGraph **T;

  // initialize the network (to an empty network)

  G = new AcyclicOrientedGraph(parents->n);

  // allocate the memory for frequency trees
  
  T = (FrequencyDecisionGraph**) Calloc(parents->n,sizeof(FrequencyDecisionGraph*));
  
  // initialize the frequency trees
  
  for (k=0; k<parents->n; k++)
    T[k] = new FrequencyDecisionGraph(parents,k);

  // construct the Bayesian network to model the selected set of parents

  constructTheNetwork(G,T,parents,boaParams);

  // print out the model for this generation

  printModel(modelFile,t,G,T);

  // generate the new points

  generateNewInstances(G,T,offspring);

  // delete the network

  delete G;

  // delete the trees

  for (k=0; k<parents->n; k++)
    delete T[k];

  // delete the array with the trees

  Free(T);

  // get back

  return 0;
}

// ================================================================================
//
// name:          pause
//
// function:      waits for enter key if required
//
// parameters:    (none)
//
// returns:       (int) 0
//
// ================================================================================

int pause(BoaParams *boaParams)
{
  if (boaParams->pause)
    {
      printf("Press Enter to continue.");
      getchar();
    };

  return 0;
}

// ================================================================================
//
// name:          done
//
// function:      gets back to normal what was changed in initialization of the BOA
//
// parameters:    boaParams....the parameters sent to the BOA
//
// returns:       (int) 0
//
// ================================================================================

int done(BoaParams *boaParams)
{
  // get rid of the metric

  doneMetric();

  // get rid of the fitness

  doneFitness(boaParams);

  // statistics done

  doneBasicStatistics(&populationStatistics);

  // close output streams

  if (logFile)
    fclose(logFile);

  if (fitnessFile)
    fclose(fitnessFile);
 
 if (modelFile)
    fclose(modelFile);
   
  // get back

  return 0;  
}

// ================================================================================
//
// name:          getLogFile
//
// function:      returns a pointer to the log file strea
//
// parameters:    (none)
//
// returns:       (FILE*) a pointer to the log file stream
//
// ================================================================================

FILE *getLogFile()
{
  return logFile;
}

// ================================================================================
//
// name:          getModelFile
//
// function:      returns a pointer to the model file stream
//
// parameters:    (none)
//
// returns:       (FILE*) a pointer to the model file stream
//
// ================================================================================

FILE *getModelFile()
{
  return modelFile;
}

// ================================================================================
//
// name:          getFitnessFile
//
// function:      returns a pointer to the fitness file stream
//
// parameters:    (none)
//
// returns:       (FILE*) a pointer to the fitness file stream
//
// ================================================================================

FILE *getFitnessFile()
{
  return fitnessFile;
}
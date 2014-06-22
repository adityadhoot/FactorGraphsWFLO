#ifndef __STATISTICS_H_INCLUDED__
#define __STATISTICS_H_INCLUDED__
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


class statistics {

	public:
	//Variables
	
	//Functions
	int intializeBasicStatistics(BasicStatistics *statistics, fgParams *fgParams);
	int doneBasicStatistics(BasicStatistics *statistics);
	int computeBasicStatistics(BasicStatistics *statistics, long t, Population *population, fgParams *fgParams);

	int generationStatistics(FILE *out, BasicStatistics *statistics);
	int fitnessStatistics(FILE *out, BasicStatistics *statistics);
	int finalStatistics(FILE *out, char *termination, BasicStatistics *statistics);
	int printModel(FILE *out, long t, AcyclicOrientedGraph *G, FrequencyDecisionGraph **T);
	int printGuidance(FILE *out, float *p1, int n, float treshold);

#endif

#ifndef __FACTORGRAPHS_H_INCLUDED__
#define __FACTORGRAPHS_H_INCLUDED__

typedef struct {
  long N;                      // Total Population Size
  float percentOffspring;      // size of offspring given in percentage 
  
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
} FactorGraphParams;


class population {
	public:
	
	FILE *logFile;
	FILE *fitnessFile;
	FILE *modelFILE;
	FILE *marginalprobabilitiesFILE;
	
	void initialize (FactorGraphParams *fgParams);
	void factorgraphs (FactorGraphParams *fgParams);
	int terminationCriteria (FactorGraphParams *fgParams);
	void generateOffspring (long t, Population *parents, Population *offspring, FactorGraphParams *fgParams);
	void pause (FactorGraphParams *fgParams);
	void done (FactorGraphParams *fgParams);
	

#endif

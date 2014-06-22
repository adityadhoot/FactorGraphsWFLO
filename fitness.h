#ifndef _FITNESS_h_
#define _FITNESS_h_

typedef float FitnessFunction (char *x);
typedef float IsBest (char *x);
typedef int InitFitness (FactorGraphParams *fgParams);
typedef int DoneFitness (FactorGraphParams *fgParams);

typedef struct {
	char *description;
	FitnessFunction *fitness;
	IsBest *isBest;
	InitFitness *init;
	DoneFitness *done;
} Fitness;

class population {
	public:
	float wlfofitness (char *x);
	char *getFitnessDesc ();
	int initializeFitness (FactorGraphParams fgParams);
	int doneFitness (FactorGraphParams fgParams);
	float getFitnessValue (char *x);
	int isBestDefined ();
	int isOptimal (char *x);
	
	int resetFitnessCalls (void);
	long fitnessCalled (void);
	long getFitnessCalls (void);
};

#endif

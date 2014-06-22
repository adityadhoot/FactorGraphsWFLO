#ifndef __POPULATION_H_INCLUDED__
#define __POPULATION_H_INCLUDED__

typedef struct {
	long N;			//population size
	int n;			//string length
	char **x;		//strings
	float *f;		//fitness values
	int t;			//total number of turbines	
} Population;


class population {

	public:
	//Variables
	
	//Functions
	void allocatePopulation(Population *population, long N, int n);
	void freePopulation(Population *population);
	void generatePopulation(Population *population);
	void evaluatePopulation(Population *population);
	
	void computeUnivariateFrequencies(Population *population, float *p1);
	void copyIndividual(Population *population, long where, char *x, float f);
	void swapIndividuals(Population *population, long first, long second);
	void printIndividual(FILE *out, char *x, int n); 
class population {

	public:

#endif
